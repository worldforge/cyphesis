#ifndef RULESETS_FILTER_PARSERDEFINITIONS_H_
#define RULESETS_FILTER_PARSERDEFINITIONS_H_

#include "Filter.h"
#include "Providers.h"
#include "Predicates.h"

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_real.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

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

//A symbol table for comparators. Matches char and synthesises Comparator attribute
struct comparators_ : qi::symbols<char, ComparePredicate::Comparator>
{

    comparators_()
    {
        add
            ("="    , ComparePredicate::Comparator::EQUALS)
            ("!="   , ComparePredicate::Comparator::NOT_EQUALS)
            (">"    , ComparePredicate::Comparator::GREATER)
            (">="   , ComparePredicate::Comparator::GREATER_EQUAL)
            ("<"    , ComparePredicate::Comparator::LESS)
            ("<="   , ComparePredicate::Comparator::LESS_EQUAL)
            ("instance_of", ComparePredicate::Comparator::INSTANCE_OF)
            ("in"   , ComparePredicate::Comparator::IN)
            ("contains", ComparePredicate::Comparator::CONTAINS);
        ;
    }

};

template<typename Iterator>
struct query_parser : qi::grammar<Iterator, Predicate*(),
        ascii::space_type, qi::locals<Predicate*>> {
        query_parser(ProviderFactory* factory):
                query_parser::base_type(parenthesised_predicate_g),
                m_factory(factory)
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
            using qi::_3;
            using qi::_a;
            using qi::_b;
            using qi::_c;
            using qi::_d;
            using namespace boost::phoenix;

            //A list of what we would consider comparison operators
            comp_operator_g %= qi::string("!=") | qi::string("<=")
                                | qi::string(">=") | qi::string("==") | qi::string("!==")
                                | char_("=") | char_(">") | char_("<")
                                | qi::no_skip[+space >> no_case[qi::string("instance_of")] >> +space]
                                | qi::no_skip[+space >> no_case[qi::string("contains")] >> +space]
                                | qi::no_skip[+space >> no_case[qi::string("in")] >> +space];

            //A list of logical operators
            //String operators ("and", "or") require at least one space before and after to distinguish
            //them from other words.
            logical_operator_g %= qi::string("&&") | qi::string("||") | no_skip[+space >> no_case[qi::string("and")] >> +space] |
                                no_skip[+space >> no_case[qi::string("or")] >> +space];

            //An attribute of a segment. no_skip is used to disable skipper parser and read white spaces
            segment_attribute_g %= qi::no_skip[+(qi::char_ - space - "." - ":" - comp_operator_g - logical_operator_g - "(" - ")" - "|" - ",")];

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
            consumer_g =
                    //If parsing a list, stuff everything in a vector of elements then create FixedElementProvider
                    ("[" >> qi::real_parser<double, qi::strict_real_policies<double>>()[push_back(_a, _1)] % "," >> "]")
                    [_val = new_<FixedElementProvider>(_a)]                                                             |

                    ("[" >> int_[push_back(_a, _1)] % "," >> "]")[_val = new_<FixedElementProvider>(_a)]                |

                    ("[" >> quoted_string_g[push_back(_a, _1)] >> "]")[_val = new_<FixedElementProvider>(_a)]           |

                    ("[" >> (no_case[qi::string("true")[_b = true] | qi::string("false")[_b = false]])
                            [push_back(_a, _b)] >> "]" )[_val = new_<FixedElementProvider>(_a)]                         |

                    qi::real_parser<double, qi::strict_real_policies<double>>()
                    [_val = new_<FixedElementProvider>(_1)]                                                             |

                    int_[_val = new_<FixedElementProvider>(_1)]                                                         |

                    quoted_string_g[_val = new_<FixedElementProvider>(_1)]                                              |

                    no_case[qi::string("true")[_b = true] | qi::string("false")[_b = false]]
                    [_val = new_<FixedElementProvider>(_b)]                                                             |

                    //contains_recursive function takes a consumer (contains_recursive is itself a consumer),
                    //and a predicate as arguments.
                    (no_case[qi::lit("contains_recursive")] >> "(" >> consumer_g >> "," >> parenthesised_predicate_g >> ")")
                    [_val = new_<ContainsRecursiveFunctionProvider>(_1, _2)]                                            |

                    segmented_expr_g[_val = boost::phoenix::bind(&ProviderFactory::createProviders, *m_factory, _1)];

            //Construct comparer predicate, depending on which comparison operator we encounter.
            comparer_predicate_g =
                    //Try to match a normal case, but save LHS and comparator into local variables
                    (consumer_g[_b = _1] >> no_case[comparators][_d = _1] >> consumer_g)
                    [_c = new_<ComparePredicate>(_1, _3, _d)]
                    //Then try to match a list case
                    //Syntax example: entity.type instance_of types.bear|types.tiger
                    //is interpreted as entity.type instance_of types.bear || entity.type instance_of types.tiger
                    >> *("|" >> consumer_g[_c = new_<OrPredicate>(_c, new_<ComparePredicate>(_b, _1,_d))])
                    >> qi::eps[_val = _c];

            predicate_g = comparer_predicate_g[_a = _1] >>
                    ((("&&" | no_case["and"]) >> predicate_g[_val = new_<AndPredicate>(_a, _1)]) |
                    (("||" | no_case["or"]) >> predicate_g[_val = new_<OrPredicate>(_a, _1)])    |
                    qi::eps[_val = _a])                                                             |
                    "(" >> predicate_g[_val = _1] >> ")";

            //Another level that constructs predicates after parentheses were consumed
            parenthesised_predicate_g = predicate_g[_a = _1] >>
                    ((("&&" | no_case["and"]) >> parenthesised_predicate_g[_val = new_<AndPredicate>(_a, _1)])   |
                    (("||" | no_case["or"]) >> parenthesised_predicate_g[_val = new_<OrPredicate>(_a, _1)])      |
                    qi::eps[_val = _a])                                                                             |
                    "(" >> parenthesised_predicate_g[_val = _1] >> ")";
        }
        ProviderFactory* m_factory;
        qi::rule<Iterator, std::string()> comp_operator_g;
        qi::rule<Iterator, std::string()> logical_operator_g;
        qi::rule<Iterator, std::string(), ascii::space_type> segment_attribute_g;
        qi::rule<Iterator, EntityFilter::ProviderFactory::Segment(), ascii::space_type> segment_g;
        qi::rule<Iterator, EntityFilter::ProviderFactory::Segment(), ascii::space_type> special_segment_g;
        qi::rule<Iterator, std::string(), ascii::space_type> quoted_string_g;
        qi::rule<Iterator, ProviderFactory::SegmentsList(), ascii::space_type> segmented_expr_g;
        qi::rule<Iterator, Consumer<QueryContext>*(), qi::locals<std::vector<Atlas::Message::Element>, bool>, ascii::space_type> consumer_g;
        qi::rule<Iterator, Predicate*(), ascii::space_type, qi::locals<Predicate*, Consumer<QueryContext>*, Predicate*,ComparePredicate::Comparator>> comparer_predicate_g;
        qi::rule<Iterator, Predicate*(), ascii::space_type, qi::locals<Predicate*>> predicate_g;
        qi::rule<Iterator, Predicate*(), ascii::space_type, qi::locals<Predicate*>> parenthesised_predicate_g;

        //An instance of comparators symbol table
        comparators_ comparators;
};

}
}
#endif
