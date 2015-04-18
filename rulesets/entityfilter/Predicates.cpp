#include "Predicates.h"
#include "Providers.h"

#include "../../common/TypeNode.h"

#include <algorithm>

namespace EntityFilter
{
ComparePredicate::ComparePredicate(const Consumer<QueryContext>* lhs,
                                   const Consumer<QueryContext>* rhs,
                                   Comparator comparator) :
        m_lhs(lhs), m_rhs(rhs), m_comparator(comparator)
{
    //make sure rhs and lhs exist
    if (!m_lhs || !m_rhs) {
        throw std::invalid_argument(
                "At least one side of the comparison operator is invalid");
    }
    if (m_comparator == Comparator::INSTANCE_OF) {
        //make sure that both providers return TypeNode intances
        if ((m_lhs->getType() != &typeid(const TypeNode*))
                || (m_rhs->getType() != &typeid(const TypeNode*))) {
            throw std::invalid_argument(
                    "When using the 'instanceof' comparator, both statements must return a TypeNode. For example, 'entity.type == types.world'.");
        }
    }

}

bool ComparePredicate::isMatch(const QueryContext& context) const
{
    switch (m_comparator) {
    case Comparator::EQUALS:
    {
        Atlas::Message::Element left, right;
        m_lhs->value(left, context);
        if (!left.isNone()) {
            m_rhs->value(right, context);
            if (!right.isNone()) {
                return left == right;
            }
        }

        return false;
    }
        break;
    case Comparator::NOT_EQUALS:
    {
        Atlas::Message::Element left, right;
        m_lhs->value(left, context);
        if (!left.isNone()) {
            m_rhs->value(right, context);
            if (!right.isNone()) {
                return left != right;
            }
        }

        return true;
    }
        break;
    case Comparator::LESS:
    {
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
        break;
    case Comparator::LESS_EQUAL:
    {
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
        break;
    case Comparator::GREATER:
    {
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
        break;
    case Comparator::GREATER_EQUAL:
    {
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
        break;
    case Comparator::INSTANCE_OF:
    {
        //We know that both providers return type node instances, since we checked in the constructor.
        Atlas::Message::Element left, right;
        m_lhs->value(left, context);
        if (left.isPtr()) {
            const TypeNode* leftType = static_cast<const TypeNode*>(left.Ptr());
            if (leftType) {
                m_rhs->value(right, context);
                if (right.isPtr()) {
                    const TypeNode* rightType =
                            static_cast<const TypeNode*>(right.Ptr());
                    if (rightType) {
                        return rightType->isTypeOf(leftType);
                    }
                }
            }
        }
        return false;
    }
    case Comparator::IN:
    {
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
    case Comparator::CONTAINS:
    {
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
        break;
    }
    return false;
}

AndPredicate::AndPredicate(const Predicate* lhs, const Predicate* rhs) :
        m_lhs(lhs), m_rhs(rhs)
{
}
bool AndPredicate::isMatch(const QueryContext& context) const
{
    return m_lhs->isMatch(context) && m_rhs->isMatch(context);

}

OrPredicate::OrPredicate(const Predicate* lhs, const Predicate* rhs) :
        m_lhs(lhs), m_rhs(rhs)
{
}
bool OrPredicate::isMatch(const QueryContext& context) const
{
    return m_lhs->isMatch(context) || m_rhs->isMatch(context);

}
}
