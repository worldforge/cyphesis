#include "Predicates.h"

#include "common/TypeNode.h"

#include <algorithm>

namespace EntityFilter {
    ComparePredicate::ComparePredicate(std::shared_ptr<Consumer<QueryContext>> lhs,
                                       std::shared_ptr<Consumer<QueryContext>> rhs,
                                       Comparator comparator,
                                       std::shared_ptr<Consumer<QueryContext>> with) :
            m_lhs(std::move(lhs)),
            m_rhs(std::move(rhs)),
            m_comparator(comparator),
            m_with(std::move(with))
    {
        //make sure rhs and lhs exist
        if (!m_lhs) {
            throw std::invalid_argument("Left side statement is invalid");
        }
        if (!m_rhs) {
            throw std::invalid_argument("Right side statement is invalid");
        }
        if (m_comparator == Comparator::INSTANCE_OF) {
            //make sure that left hand side returns an entity and right hand side a typenode
            if (m_lhs->getType() != &typeid(const LocatedEntity*)) {
                throw std::invalid_argument(
                        "When using the 'instanceof' comparator, left statement must return an entity. For example, 'entity instance_of types.world'.");
            }
            if (m_rhs->getType() != &typeid(const TypeNode*)) {
                throw std::invalid_argument(
                        "When using the 'instanceof' comparator, right statement must return a TypeNode. For example, 'entity instance_of types.world'.");
            }
        } else if (m_comparator == Comparator::CAN_REACH) {
            //make sure that both sides return an entity
            if ((m_lhs->getType() != &typeid(const LocatedEntity*))
                || ((m_rhs->getType() != &typeid(const LocatedEntity*)) && (m_rhs->getType() != &typeid(const QueryEntityLocation*)))) {
                throw std::invalid_argument(
                        "When using the 'can_reach' comparator, both sides must return an entity. For example, 'actor can_reach entity'.");
            }
            if (m_with) {
                if (m_with->getType() != &typeid(const LocatedEntity*)) {
                    throw std::invalid_argument(
                            "When using the 'can_reach ... with' comparator, all three inputs must return an entity. For example, 'actor can_reach entity with tool'.");
                }
            }
        }

    }

    bool ComparePredicate::isMatch(const QueryContext& context) const
    {
        switch (m_comparator) {
            case Comparator::EQUALS: {
                Atlas::Message::Element left, right;
                m_lhs->value(left, context);
                m_rhs->value(right, context);
                return left == right;
            }
            case Comparator::NOT_EQUALS: {
                Atlas::Message::Element left, right;
                m_lhs->value(left, context);
                m_rhs->value(right, context);
                return left != right;
            }
            case Comparator::LESS: {
                Atlas::Message::Element left, right;
                m_lhs->value(left, context);
                if (left.isNum()) {
                    m_rhs->value(right, context);
                    if (right.isNum()) {
                        return left.asNum() < right.asNum();
                    }
                }
                return false;
            }
            case Comparator::LESS_EQUAL: {
                Atlas::Message::Element left, right;
                m_lhs->value(left, context);
                if (left.isNum()) {
                    m_rhs->value(right, context);
                    if (right.isNum()) {
                        return left.asNum() <= right.asNum();
                    }
                }
                return false;
            }
            case Comparator::GREATER: {
                Atlas::Message::Element left, right;
                m_lhs->value(left, context);
                if (left.isNum()) {
                    m_rhs->value(right, context);
                    if (right.isNum()) {
                        return left.asNum() > right.asNum();
                    }
                }
                return false;
            }
            case Comparator::GREATER_EQUAL: {
                Atlas::Message::Element left, right;
                m_lhs->value(left, context);
                if (left.isNum()) {
                    m_rhs->value(right, context);
                    if (right.isNum()) {
                        return left.asNum() >= right.asNum();
                    }
                }
                return false;
            }
            case Comparator::INSTANCE_OF: {
                //We know that left returns an entity and right returns a type, since we checked in the constructor.
                Atlas::Message::Element left, right;
                m_lhs->value(left, context);
                if (left.isPtr()) {
                    auto leftType = static_cast<const LocatedEntity*>(left.Ptr())->getType();
                    if (leftType) {
                        m_rhs->value(right, context);
                        if (right.isPtr()) {
                            auto rightType = static_cast<const TypeNode*>(right.Ptr());
                            if (rightType) {
                                return leftType->isTypeOf(rightType);
                            }
                        }
                    }
                }
                return false;
            }
            case Comparator::IN: {
                Atlas::Message::Element left, right;
                m_lhs->value(left, context);
                if (!left.isNone()) {
                    m_rhs->value(right, context);
                    if (right.isList()) {
                        const auto& right_end = right.List().end();
                        const auto& right_begin = right.List().begin();
                        return std::find(right_begin, right_end, left) != right_end;
                    }
                }
                return false;
            }
            case Comparator::INCLUDES: {
                Atlas::Message::Element left, right;
                m_lhs->value(left, context);
                if (left.isList()) {
                    m_rhs->value(right, context);
                    if (!right.isNone()) {
                        const auto& left_end = left.List().end();
                        const auto& left_begin = left.List().begin();
                        return std::find(left_begin, left_end, right) != left_end;
                    }
                }
                return false;
            }
            case Comparator::CAN_REACH: {
                Atlas::Message::Element left, right;
                m_lhs->value(left, context);
                auto leftEntity = static_cast<const LocatedEntity*>(left.Ptr());
                m_rhs->value(right, context);

                EntityLocation entityLocation;
                if (m_rhs->getType() == &typeid(const QueryEntityLocation*)) {
                    auto queryEntityLocation = *static_cast<QueryEntityLocation*>(right.Ptr());
                    entityLocation.m_parent = &queryEntityLocation.entity;
                    if (queryEntityLocation.pos) {
                        entityLocation.m_pos = *queryEntityLocation.pos;
                    }
                } else {
                    entityLocation.m_parent = static_cast<LocatedEntity*>(right.Ptr());
                }
                float extraReach = 0;
                if (m_with) {
                    Atlas::Message::Element with;
                    m_with->value(with, context);
                    auto withEntity = static_cast<LocatedEntity*>(with.Ptr());
                    auto reachProp = withEntity->getPropertyType<double>("reach");
                    if (reachProp) {
                        extraReach = (float) reachProp->data();
                    }
                }

                if (leftEntity && entityLocation.m_parent) {
                    return leftEntity->canReach(entityLocation, extraReach);
                }
                return false;
            }
            default:
                log(ERROR, "Could not recognize comparator.");
                return false;
        }
    }

    AndPredicate::AndPredicate(std::shared_ptr<Predicate> lhs, std::shared_ptr<Predicate> rhs) :
            m_lhs(std::move(lhs)),
            m_rhs(std::move(rhs))
    {
    }

    bool AndPredicate::isMatch(const QueryContext& context) const
    {
        return m_lhs->isMatch(context) && m_rhs->isMatch(context);

    }

    OrPredicate::OrPredicate(std::shared_ptr<Predicate> lhs, std::shared_ptr<Predicate> rhs) :
            m_lhs(std::move(lhs)),
            m_rhs(std::move(rhs))
    {
    }

    bool OrPredicate::isMatch(const QueryContext& context) const
    {
        return m_lhs->isMatch(context) || m_rhs->isMatch(context);

    }

    NotPredicate::NotPredicate(std::shared_ptr<Predicate> pred) :
            m_pred(std::move(pred))
    {
    }

    bool NotPredicate::isMatch(const QueryContext& context) const
    {
        return !m_pred->isMatch(context);
    }

    BoolPredicate::BoolPredicate(std::shared_ptr<Consumer<QueryContext>> consumer) :
            m_consumer(std::move(consumer))
    {
    }

    bool BoolPredicate::isMatch(const QueryContext& context) const
    {
        if (!m_consumer) {
            return false;
        }
        Atlas::Message::Element value;
        m_consumer->value(value, context);
        return value.isInt() && value.Int() != 0;
    }

    DescribePredicate::DescribePredicate(std::string description,
                                         std::shared_ptr<Predicate> predicate) :
            m_description(std::move(description)),
            m_predicate(std::move(predicate))
    {
    }

    bool DescribePredicate::isMatch(const QueryContext& context) const
    {
        bool isMatch = m_predicate->isMatch(context);
        if (!isMatch) {
            if (context.report_error_fn) {
                context.report_error_fn(m_description);
            }
        }
        return isMatch;
    }
}
