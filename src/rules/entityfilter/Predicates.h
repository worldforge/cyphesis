#ifndef PREDICATES_H_
#define PREDICATES_H_

#include "Providers.h"

#include <memory>
#include <string>

namespace EntityFilter {

    class QueryContext;

    template<class T>
    class Consumer;

    class Predicate
    {
        public:
            virtual ~Predicate() = default;

            virtual bool isMatch(const QueryContext& context) const = 0;
    };


    class ComparePredicate : public Predicate
    {
        public:

            enum class Comparator
            {
                    EQUALS, NOT_EQUALS, INSTANCE_OF, IN, INCLUDES, GREATER, GREATER_EQUAL, LESS, LESS_EQUAL, CAN_REACH
            };

            ComparePredicate(std::shared_ptr<Consumer<QueryContext>> lhs,
                             std::shared_ptr<Consumer<QueryContext>> rhs,
                             Comparator comparator,
                             std::shared_ptr<Consumer<QueryContext>> with = nullptr);

            bool isMatch(const QueryContext& context) const override;

            std::shared_ptr<Consumer<QueryContext>> m_lhs;
            std::shared_ptr<Consumer<QueryContext>> m_rhs;
            const Comparator m_comparator;
            std::shared_ptr<Consumer<QueryContext>> m_with;
    };

    class DescribePredicate : public Predicate
    {
        public:

            DescribePredicate(std::string description,
                              std::shared_ptr<Predicate> predicate);

            bool isMatch(const QueryContext& context) const override;

            std::string m_description;
            std::shared_ptr<Predicate> m_predicate;
    };

    class AndPredicate : public Predicate
    {
        public:
            AndPredicate(std::shared_ptr<Predicate> lhs, std::shared_ptr<Predicate> rhs);

            bool isMatch(const QueryContext& context) const override;

            std::shared_ptr<Predicate> m_lhs;
            std::shared_ptr<Predicate> m_rhs;
    };

    class OrPredicate : public Predicate
    {
        public:
            OrPredicate(std::shared_ptr<Predicate> lhs, std::shared_ptr<Predicate> rhs);

            bool isMatch(const QueryContext& context) const override;

            std::shared_ptr<Predicate> m_lhs;
            std::shared_ptr<Predicate> m_rhs;
    };

    class NotPredicate : public Predicate
    {
        public:
            explicit NotPredicate(std::shared_ptr<Predicate> pred);

            bool isMatch(const QueryContext& context) const override;

            const std::shared_ptr<Predicate> m_pred;
    };

    class BoolPredicate : public Predicate
    {
        public:
            explicit BoolPredicate(std::shared_ptr<Consumer<QueryContext>> consumer);

            bool isMatch(const QueryContext& context) const override;

            const std::shared_ptr<Consumer<QueryContext>> m_consumer;
    };

}

#endif
