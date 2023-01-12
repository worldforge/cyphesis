#ifndef RULESETS_FILTER_PARSERDEFINITIONS_H_
#define RULESETS_FILTER_PARSERDEFINITIONS_H_

#include "Filter.h"
#include "Providers.h"
#include "Predicates.h"
#include "ProviderFactory.h"

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_real.hpp>


namespace EntityFilter {
    namespace parser {

        namespace qi = boost::spirit::qi;

        //A symbol table for comparators. Matches char and synthesises Comparator attribute
        struct comparators_ : qi::symbols<char, ComparePredicate::Comparator>
        {

            comparators_()
            {
                add
                        ("=", ComparePredicate::Comparator::EQUALS)
                        ("!=", ComparePredicate::Comparator::NOT_EQUALS)
                        (">", ComparePredicate::Comparator::GREATER)
                        (">=", ComparePredicate::Comparator::GREATER_EQUAL)
                        ("<", ComparePredicate::Comparator::LESS)
                        ("<=", ComparePredicate::Comparator::LESS_EQUAL)
                        ("instance_of", ComparePredicate::Comparator::INSTANCE_OF)
                        ("in", ComparePredicate::Comparator::IN)
                        ("includes", ComparePredicate::Comparator::INCLUDES)
                        ("can_reach", ComparePredicate::Comparator::CAN_REACH);;
            }

        };

        template<typename Iterator>
        struct query_parser : qi::grammar<Iterator, std::shared_ptr<Predicate>(),
                qi::space_type, qi::locals<std::shared_ptr<Predicate>>> {
            query_parser(const ProviderFactory& factory);

            const ProviderFactory& m_factory;
            qi::rule<Iterator, std::string()> comp_operator_g;
            qi::rule<Iterator, std::string()> logical_operator_g;
            qi::rule<Iterator, std::string(), qi::space_type> segment_attribute_g;
            qi::rule<Iterator, EntityFilter::ProviderFactory::Segment(), qi::space_type> segment_g;
            qi::rule<Iterator, EntityFilter::ProviderFactory::Segment(), qi::space_type> special_segment_g;
            qi::rule<Iterator, std::string(), qi::space_type, qi::locals<char> > quoted_string_g;
            qi::rule<Iterator, ProviderFactory::SegmentsList(), qi::space_type> segmented_expr_g;
            qi::rule<Iterator, ProviderFactory::SegmentsList(), qi::space_type> delimited_segmented_expr_g;
            qi::rule<Iterator, std::shared_ptr<Consumer<QueryContext>>(), qi::locals<std::vector<Atlas::Message::Element>, bool>, qi::space_type> consumer_g;
            qi::rule<Iterator, std::shared_ptr<Predicate>(), qi::space_type, qi::locals<std::shared_ptr<Predicate>, std::shared_ptr<Consumer<QueryContext>>, std::shared_ptr<Predicate>, ComparePredicate::Comparator>> comparer_predicate_g;
            qi::rule<Iterator, std::shared_ptr<Predicate>(), qi::space_type, qi::locals<std::shared_ptr<Predicate>>> predicate_g;
            qi::rule<Iterator, std::shared_ptr<Predicate>(), qi::space_type, qi::locals<std::shared_ptr<Predicate>>> parenthesised_predicate_g;

            //An instance of comparators symbol table
            comparators_ comparators;
        };

    }
}
#endif
