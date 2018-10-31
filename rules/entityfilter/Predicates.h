#ifndef PREDICATES_H_
#define PREDICATES_H_

#include "Providers.h"

namespace EntityFilter{

struct QueryContext;

template<class T>
class Consumer;

class Predicate {
    public:
        virtual ~Predicate() = default;

        virtual bool isMatch(const QueryContext& context) const = 0;
};


class ComparePredicate : public Predicate {
    public:

        enum class Comparator {
            EQUALS, NOT_EQUALS, INSTANCE_OF, IN, CONTAINS, GREATER, GREATER_EQUAL, LESS, LESS_EQUAL, CAN_REACH
        };
        ComparePredicate(const Consumer<QueryContext>* lhs, const Consumer<QueryContext>* rhs, Comparator comparator, const Consumer<QueryContext>* with=nullptr);

        bool isMatch(const QueryContext& context) const override;

        const Consumer<QueryContext>* m_lhs;
        const Consumer<QueryContext>* m_rhs;
        const Comparator m_comparator;
        const Consumer<QueryContext>* m_with;
};

class AndPredicate : public Predicate {
    public:
        AndPredicate(const Predicate* lhs, const Predicate* rhs);

        bool isMatch(const QueryContext& context) const override;

        const Predicate* m_lhs;
        const Predicate* m_rhs;
};

class OrPredicate : public Predicate {
    public:
        OrPredicate(const Predicate* lhs, const Predicate* rhs);

        bool isMatch(const QueryContext& context) const override;

        const Predicate* m_lhs;
        const Predicate* m_rhs;
};

class NotPredicate : public Predicate {
    public:
        explicit NotPredicate(const Predicate* pred);

        bool isMatch(const QueryContext& context) const override;

        const Predicate* m_pred;
};

class BoolPredicate : public Predicate {
    public:
        explicit BoolPredicate(const Consumer<QueryContext>* consumer);

        bool isMatch(const QueryContext& context) const override;

        const Consumer<QueryContext>* m_consumer;
};

}

#endif
