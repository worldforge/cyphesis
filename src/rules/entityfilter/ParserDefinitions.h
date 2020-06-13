#ifndef RULESETS_FILTER_PARSERDEFINITIONS_H_
#define RULESETS_FILTER_PARSERDEFINITIONS_H_

#include "Filter.h"
#include "Providers.h"
#include "Predicates.h"
#include "ProviderFactory.h"

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_real.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

namespace EntityFilter {
    namespace parser {

    }
}

BOOST_FUSION_ADAPT_STRUCT(
        EntityFilter::ProviderFactory::Segment,
        (std::string, delimiter)
                (std::string, attribute))

namespace EntityFilter {
    namespace parser {

        // this solution for lazy make shared comes from the SO forum, user sehe.
        // https://stackoverflow.com/questions/21516201/how-to-create-boost-phoenix-make-shared
        //    post found using google search terms `phoenix construct shared_ptr`
        // changed from boost::shared_ptr to std::shared_ptr
        namespace {
            template<typename T>
            struct make_shared_f
            {
                template<typename... A>
                struct result
                {
                    typedef std::shared_ptr<T> type;
                };

                template<typename... A>
                typename result<A...>::type operator()(A&& ... a) const
                {
                    return std::make_shared<T>(std::forward<A>(a)...);
                }
            };

            template<typename T> using make_shared_ = boost::phoenix::function<make_shared_f<T> >;
        }

        namespace qi = boost::spirit::qi;
        //Parser definitions

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
                qi::space_type, qi::locals<std::shared_ptr<Predicate>>>
        {
            query_parser(const ProviderFactory& factory) :
                    query_parser::base_type(parenthesised_predicate_g),
                    m_factory(factory)
            {
                using qi::int_;
                using qi::lit;
                using qi::double_;
                using qi::no_skip;
                using qi::no_case;
                using qi::char_;
                using qi::space;
                using qi::_val;
                using qi::_1;
                using qi::_2;
                using qi::_3;
                using qi::_4;
                using qi::_5;
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
                                   | qi::no_skip[+space >> no_case[qi::string("includes")] >> +space]
                                   | qi::no_skip[+space >> no_case[qi::string("in")] >> +space]
                                   | qi::no_skip[+space >> no_case[qi::string("can_reach")] >> +space];
                comp_operator_g.name("comp operator");

                //A list of logical operators
                //String operators ("and", "or") require at least one space before and after to distinguish
                //them from other words.
                logical_operator_g %= qi::string("&&") | qi::string("||") | qi::string("!") | no_skip[+space >> no_case[qi::string("and")] >> +space] |
                                      no_skip[+space >> no_case[qi::string("or")] >> +space] |
                                      no_skip[+space >> no_case[qi::string("not")] >> +space];
                logical_operator_g.name("logical operator");

                //An attribute of a segment. no_skip is used to disable skipper parser and read white spaces
                segment_attribute_g %= qi::no_skip[+(qi::char_ - space - "." - ":" - comp_operator_g - logical_operator_g - "(" - ")" - "|" - ",")];
                segment_attribute_g.name("segment attribute");

                //A single segment. Consists of a delimiter followed by the attribute. (i.e. ".type")
                segment_g %= (char_(".") | char_(":")) >> segment_attribute_g;
                segment_g.name("segment");

                //Special segment is used as the first segment in a token (it has no delimiter)
                //qi eps is used to specify that the delimiter is empty
                special_segment_g = (qi::eps[at_c<0>(_val) = ""] >> (segment_attribute_g)[at_c<1>(_val) = _1]);
                special_segment_g.name("special segment");

                //Collects segments into SegmentsList, which is used to construct
                //a consumer by a consumer factory
                segmented_expr_g %= special_segment_g >> +segment_g;
                segmented_expr_g.name("segmented expr");

                delimited_segmented_expr_g %= +segment_g;
                delimited_segmented_expr_g.name("delimited segmented expr");

                //Distinguish between string literals from regular segments
                //(i.e. entity.type="entity")

                quoted_string_g =
                        qi::omit[char_("'\"")[_a = _1]]
                                >> no_skip[*(char_ - char_(_a))]
                                >> lit(_a);

                quoted_string_g.name("quoted string");

                //Construct a new consumer. Simple values are constructed via FixedElementProvider.
                //Doubles have to have a dot, otherwise numbers are parsed as int (this affects type of Element)
                //If we have a SegmentsList, use the given factory to construct Consumer
                consumer_g =
                        //If parsing a list, stuff everything in a vector of elements then create FixedElementProvider
                        ("[" >> qi::real_parser<double, qi::strict_real_policies<double>>()[push_back(_a, _1)] % "," >> "]")
                        [_val = make_shared_<FixedElementProvider>()(_a)] |

                        ("[" >> int_[push_back(_a, _1)] % "," >> "]")[_val = make_shared_<FixedElementProvider>()(_a)] |

                        ("[" >> quoted_string_g[push_back(_a, _1)] >> "]")[_val = make_shared_<FixedElementProvider>()(_a)] |

                        ("[" >> (no_case[qi::string("true")[_b = true] | qi::string("false")[_b = false]])
                        [push_back(_a, _b)] >> "]")[_val = make_shared_<FixedElementProvider>()(_a)] |

                        qi::real_parser<double, qi::strict_real_policies<double>>()
                        [_val = make_shared_<FixedElementProvider>()(_1)] |

                        int_[_val = make_shared_<FixedElementProvider>()(_1)] |

                        quoted_string_g[_val = make_shared_<FixedElementProvider>()(_1)] |

                        no_case[qi::string("true")[_b = true] | qi::string("false")[_b = false]]
                        [_val = make_shared_<FixedElementProvider>()(_b)] |

                        no_case[qi::string("none")][_val = make_shared_<FixedElementProvider>()(Atlas::Message::Element())] |

                        //contains_recursive function takes a consumer (contains_recursive is itself a consumer),
                        //and a predicate as arguments.
                        (no_case[qi::lit("contains_recursive")] >> "(" >> consumer_g >> "," >> parenthesised_predicate_g >> ")")
                        [_val = make_shared_<ContainsRecursiveFunctionProvider>()(_1, _2, true)] |

                        //contains function takes a consumer (contains is itself a consumer),
                        //and a predicate as arguments.
                        (no_case[qi::lit("contains")] >> "(" >> consumer_g >> "," >> parenthesised_predicate_g >> ")")
                        [_val = make_shared_<ContainsRecursiveFunctionProvider>()(_1, _2, false)] |

                        //allows the "get_entity" function which is used to extract an entity from any property
                        //which follows the "entity ref protocol", i.e. returns a map with a "$eid" entry.
                        ((no_case[qi::lit("get_entity")] >> "(" >> consumer_g >> ")") >> delimited_segmented_expr_g)
                        [_val = boost::phoenix::bind(&ProviderFactory::createGetEntityFunctionProvider, m_factory, _1, _2)]
                        |
                        ((no_case[qi::lit("get_entity")] >> "(" >> consumer_g >> ")"))
                        [_val = boost::phoenix::bind(&ProviderFactory::createSimpleGetEntityFunctionProvider, m_factory, _1)]
                        |
                        segmented_expr_g[_val = boost::phoenix::bind(&ProviderFactory::createProviders, m_factory, _1)] |

                        special_segment_g[_val = boost::phoenix::bind(&ProviderFactory::createProvider, m_factory, _1)];

                consumer_g.name("consumer");

                //Construct comparer predicate, depending on which comparison operator we encounter.
                comparer_predicate_g =
                        //Try to match a normal case, but save LHS and comparator into local variables
                        (consumer_g[_b = _1] >> no_case[comparators][_d = _1] >> consumer_g >> no_case[qi::string("with")] >> consumer_g)
                        [_c = make_shared_<ComparePredicate>()(_1, _3, _d, _5)]
                                //Then try to match a list case
                                //Syntax example: entity.type instance_of types.bear|types.tiger
                                //is interpreted as entity.type instance_of types.bear || entity.type instance_of types.tiger
                                >> *("|" >> consumer_g[_c = make_shared_<OrPredicate>()(_c, make_shared_<ComparePredicate>()(_b, _1, _d))])
                                >> qi::eps[_val = _c]
                        |
                        (consumer_g[_b = _1] >> no_case[comparators][_d = _1] >> consumer_g)
                        [_c = make_shared_<ComparePredicate>()(_1, _3, _d)]
                                //Then try to match a list case
                                //Syntax example: entity.type instance_of types.bear|types.tiger
                                //is interpreted as entity.type instance_of types.bear || entity.type instance_of types.tiger
                                >> *("|" >> consumer_g[_c = make_shared_<OrPredicate>()(_c, make_shared_<ComparePredicate>()(_b, _1, _d))])
                                >> qi::eps[_val = _c];
                comparer_predicate_g.name("comparer predicate");

                //Construct a predicate with logical operators
                predicate_g =
                        //Try unary not on a comparer since it not has highest precedence
                        (no_case["not"] | "!") >> comparer_predicate_g[_val = make_shared_<NotPredicate>()(_1)] |
                        //Try unary not on a logical predicate
                        (no_case["not"] | "!") >> predicate_g[_val = make_shared_<NotPredicate>()(_1)] |

                        (no_case[qi::lit("describe(")] >> quoted_string_g >> "," >> parenthesised_predicate_g >> ")")
                        [_val = make_shared_<DescribePredicate>()(_1, _2)] |

                        //Try binary operators (or, and)
                        comparer_predicate_g[_a = _1] >>
                                                      ((("&&" | no_case["and"]) >> predicate_g[_val = make_shared_<AndPredicate>()(_a, _1)]) |
                                                       (("||" | no_case["or"]) >> predicate_g[_val = make_shared_<OrPredicate>()(_a, _1)]) |
                                                       qi::eps[_val = _a]) |
                        //Collect parentheses
                        "(" >> predicate_g[_val = _1] >> ")"
                        |
                        //A single predicate should be evaluated as if boolean
                        consumer_g[_val = make_shared_<BoolPredicate>()(_1)];
                predicate_g.name("predicate");

                //Another level that constructs predicates after parentheses were consumed
                parenthesised_predicate_g = predicate_g[_a = _1] >>
                                                                 ((("&&" | no_case["and"]) >> parenthesised_predicate_g[_val = make_shared_<AndPredicate>()(_a, _1)]) |
                                                                  (("||" | no_case["or"]) >> parenthesised_predicate_g[_val = make_shared_<OrPredicate>()(_a, _1)]) |
                                                                  qi::eps[_val = _a]) |
                                            "(" >> parenthesised_predicate_g[_val = _1] >> ")";

                parenthesised_predicate_g.name("parenthesised predicate");
                BOOST_SPIRIT_DEBUG_NODE(consumer_g);
                BOOST_SPIRIT_DEBUG_NODE(comparer_predicate_g);
                BOOST_SPIRIT_DEBUG_NODE(predicate_g);
                BOOST_SPIRIT_DEBUG_NODE(parenthesised_predicate_g);

            }

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
