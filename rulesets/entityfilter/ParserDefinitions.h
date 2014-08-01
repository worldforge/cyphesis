#ifndef RULESETS_FILTER_PARSERDEFINITIONS_H_
#define RULESETS_FILTER_PARSERDEFINITIONS_H_

#include "Filter.h"
#include "Providers.h"
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_real.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/variant/recursive_wrapper.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>

namespace EntityFilter
{
namespace parser
{

}
}

BOOST_FUSION_ADAPT_STRUCT(
        EntityFilter::ProviderFactory::Segment,
        (std::string, delimiter)
        (std::string, attribute))

namespace EntityFilter
{
namespace parser
{
namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
//Parser definitions


template<typename Iterator, class Factory>
struct query_parser : qi::grammar<Iterator, Predicate*(),
        ascii::space_type, qi::locals<Predicate*>> {
        query_parser() :
                query_parser::base_type(parenthesised_predicate_g)
        {
            using qi::int_;
            using qi::lit;
            using qi::double_;
            using qi::no_skip;
            using qi::no_case;
            using ascii::char_;
            using qi::space;
            using qi::_val;
            using qi::_1;
            using qi::_2;
            using qi::_a;
            using namespace boost::phoenix;

            //A list of what we would consider comparison operators
            comp_operator_g %= qi::string("!=") | qi::string("<=")
                                | qi::string(">=") | qi::string("==") | qi::string("!==")
                                | char_("=") | char_(">") | char_("<")
                                | qi::no_skip[+space >> no_case[qi::string("is_instance")] >> +space]
                                | qi::no_skip[+space >> no_case[qi::string("contains")] >> +space];

            //A list of logical operators
            //String operators ("and", "or") require at least one space before and after to distinguish
            //them from other words.
            logical_operator_g %= char_("&") | char_("|") | no_skip[+space >> no_case[qi::string("and")] >> +space] |
                                no_skip[+space >> no_case[qi::string("or")] >> +space];

            //An attribute of a segment. no_skip is used to disable skipper parser and read white spaces
            segment_attribute_g %= qi::no_skip[+(qi::char_ - space - "." - ":" - comp_operator_g - logical_operator_g - "(" - ")")];

            //A single segment. Consists of a delimiter followed by the attribute. (i.e. ".type")
            segment_g %= (char_(".") | char_(":")) >>
                        segment_attribute_g;

            //Special segment is used as the first segment in a token (it has no delimiter)
            //qi eps is used to specify that the delimiter is empty
            special_segment_g = (qi::eps[at_c<0>(_val) = ""] >>
                    (segment_attribute_g)[at_c<1>(_val) = _1]);

            //Collects segments into SegmentsList, which is used to construct
            //a consumer by a consumer factory
            segmented_expr_g %= special_segment_g >> +segment_g;

            //Distinguish between string literals from regular segments
            //(i.e. entity.type="entity")
            quoted_string_g %= "'" >> +(char_ - "'") >> "'";

            //Construct a new consumer. Simple values are constructed via FixedElementProvider.
            //Doubles have to have a dot, otherwise numbers are parsed as int (this affects type of Element)
            //If we have a SegmentsList, use the given factory to construct Consumer
            consumer_g = qi::real_parser<double, qi::strict_real_policies<double>>()
                    [_val = new_<FixedElementProvider>(_1)]                             |
                    int_[_val = new_<FixedElementProvider>(_1)]                         |
                    quoted_string_g[_val = new_<FixedElementProvider>(_1)]              |
                    segmented_expr_g[_val = boost::phoenix::bind(
                            &Factory::createProviders, &f, _1)];

            //Construct comparer predicate, depending on which comparison operator we encounter.
            comparer_predicate_g =
                    (consumer_g >> "=" >> consumer_g)[_val = new_<
                            ComparePredicate>(
                            _1, qi::_2, ComparePredicate::Comparator::EQUALS)]  |

                            (consumer_g >> "!=" >> consumer_g)
                            [_val = new_<ComparePredicate>(_1, qi::_2,
                            ComparePredicate::Comparator::NOT_EQUALS)]          |

                            (consumer_g >> ">" >> consumer_g)
                            [_val = new_<ComparePredicate>(_1, qi::_2,
                            ComparePredicate::Comparator::GREATER)]             |

                            (consumer_g >> ">=" >> consumer_g)
                            [_val = new_<ComparePredicate>(_1, qi::_2,
                            ComparePredicate::Comparator::GREATER_EQUAL)]       |

                            (consumer_g >> "<" >> consumer_g)
                            [_val = new_<ComparePredicate>(_1, qi::_2,
                            ComparePredicate::Comparator::LESS)]                |

                            (consumer_g >> "<=" >> consumer_g)
                            [_val = new_<ComparePredicate>(_1, qi::_2,
                            ComparePredicate::Comparator::LESS_EQUAL)]          |

                            (consumer_g >> no_case["is_instance"] >> consumer_g)
                            [_val = new_<ComparePredicate>(_1, qi::_2,
                            ComparePredicate::Comparator::INSTANCE_OF)]         |
                            (consumer_g >> no_case["contains"] >> consumer_g)
                            [_val = new_<ComparePredicate>(_1, qi::_2,
                            ComparePredicate::Comparator::CONTAINS)];

            //Construct a predicate depending on which logical operator we encounter.
            //"and" is matched before or to implement precedence.
            //When everything within parentheses is parsed into a single predicate, parentheses are consumed
            predicate_g = comparer_predicate_g[_a = _1] >>
                    ((("&" | no_case["and"]) >> predicate_g[_val = new_<AndPredicate>(_a, _1)]) |
                    (("|" | no_case["or"]) >> predicate_g[_val = new_<OrPredicate>(_a, _1)])    |
                    qi::eps[_val = _a])                                                             |
                    "(" >> predicate_g[_val = _1] >> ")";

            //Another level that constructs predicates after parentheses were consumed
            parenthesised_predicate_g = predicate_g[_a = _1] >>
                    ((("&" | no_case["and"]) >> parenthesised_predicate_g[_val = new_<AndPredicate>(_a, _1)])|
                    (("|" | no_case["or"]) >> parenthesised_predicate_g[_val = new_<OrPredicate>(_a, _1)])|
                    qi::eps[_val = _a]) |
                    "(" >> parenthesised_predicate_g[_val = _1] >> ")";
        }

        Factory f;
        qi::rule<Iterator, std::string()> comp_operator_g;
        qi::rule<Iterator, std::string()> logical_operator_g;
        qi::rule<Iterator, std::string(), ascii::space_type> segment_attribute_g;
        qi::rule<Iterator, EntityFilter::ProviderFactory::Segment(), ascii::space_type> segment_g;
        qi::rule<Iterator, EntityFilter::ProviderFactory::Segment(), ascii::space_type> special_segment_g;
        qi::rule<Iterator, std::string(), ascii::space_type> quoted_string_g;
        qi::rule<Iterator, ProviderFactory::SegmentsList(), ascii::space_type> segmented_expr_g;
        qi::rule<Iterator, Consumer<QueryContext>*(), ascii::space_type> consumer_g;
        qi::rule<Iterator, ComparePredicate*(), ascii::space_type, qi::locals<Predicate*>> comparer_predicate_g;
        qi::rule<Iterator, Predicate*(), ascii::space_type, qi::locals<Predicate*>> predicate_g;
        qi::rule<Iterator, Predicate*(), ascii::space_type, qi::locals<Predicate*>> parenthesised_predicate_g;
};

}
}
#endif
