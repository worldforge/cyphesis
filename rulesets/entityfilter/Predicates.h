#ifndef PREDICATES_H_
#define PREDICATES_H_

#include "Providers.h"

namespace EntityFilter{

struct QueryContext;

template<class T>
class Consumer;

class Predicate {
    public:
        virtual ~Predicate(){}
        virtual bool isMatch(const QueryContext& context) const = 0;
};


class ComparePredicate : public Predicate {
    public:

        enum class Comparator {
            EQUALS, NOT_EQUALS, INSTANCE_OF, IN, CONTAINS, GREATER, GREATER_EQUAL, LESS, LESS_EQUAL
        };
        ComparePredicate(const Consumer<QueryContext>* lhs, const Consumer<QueryContext>* rhs, Comparator comparator);
        virtual bool isMatch(const QueryContext& context) const;
    protected:
        const Consumer<QueryContext>* m_lhs;
        const Consumer<QueryContext>* m_rhs;
        Comparator m_comparator;


};

class AndPredicate : public Predicate {
    public:
        AndPredicate(const Predicate* lhs, const Predicate* rhs);
        virtual bool isMatch(const QueryContext& context) const;
    protected:
        const Predicate* m_lhs;
        const Predicate* m_rhs;
};

class OrPredicate : public Predicate {
    public:
        OrPredicate(const Predicate* lhs, const Predicate* rhs);
        virtual bool isMatch(const QueryContext& context) const;
   protected:
        const Predicate* m_lhs;
        const Predicate* m_rhs;
};
}

#endif
