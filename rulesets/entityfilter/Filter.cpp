#include "Filter.h"
#include "ParserDefinitions.h"
#include "AttributeCases.h"
#include "OutfitCase.h"
#include "BBoxCase.h"
#include "../LocatedEntity.h"
#include "../../common/TypeNode.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>
#include <boost/tuple/tuple.hpp>

#include <boost/variant/recursive_wrapper.hpp>

using namespace boost;
namespace qi = boost::spirit::qi;
using qi::no_case;



namespace EntityFilter
{
//Visitor class for filter's AST.
//This class describes how to handle the AST nodes by providing an overload for each possible case
class ASTVisitor : public boost::static_visitor<bool> {
    public:
        //Entity to be tested has to be stored as a member.
        //FIXME: assigning m_entity may be faster than reinitializing
        ASTVisitor(LocatedEntity& entity) :
                m_entity(entity)
        {
        }
        LocatedEntity& m_entity;

        //A node should never be an integer, but there has to be an overload for it.
        bool operator()(int & i) const
        {
            return false;
        }
        //If a node is a condition - just check if it's true for a given entity
        bool operator()(ParsedCondition& condition) const
        {
            return condition.isTrue(m_entity);
        }
        //If a node is a binary operation - recurse on its branches by applying this visitor.
        bool operator()(parser::binop<parser::op_and>& node) const
        {
            return boost::apply_visitor(*this, node.m_left)
                    && boost::apply_visitor(*this, node.m_right);
        }
        bool operator()(parser::binop<parser::op_or>& node) const
        {
            return boost::apply_visitor(*this, node.m_left)
                    || boost::apply_visitor(*this, node.m_right);
        }
};

Filter::Filter(const std::string &what)
{
    parser::query_ast_parser<std::string::const_iterator> grammar;
    auto iter_begin = what.begin();
    auto iter_end = what.end();
    parser::expr expression;

    bool parse_success = qi::phrase_parse(iter_begin, iter_end, grammar,
                                          boost::spirit::ascii::space, m_tree);
    if (!(parse_success && iter_begin == iter_end)) {
        InvalidQueryException invalid_query;
        throw invalid_query;
    }
}

bool Filter::match(LocatedEntity& entity)
{
    return boost::apply_visitor(ASTVisitor(entity), m_tree);
}

ParsedCondition::ParsedCondition(const std::string& attribute,
                                 const std::string& comp_operator,
                                 const std::string& value_str)
{
    //Determine what kind of subject we have using spirit::qi grammar rules.
    std::string subject = attribute;
    std::string::iterator iter_begin = subject.begin();
    std::string::iterator iter_end = subject.end();
    std::string parsed_object;

    //grammar for "entity.type" token
    //This rule will match if we use "entity.type" token
    qi::rule<std::string::iterator, boost::spirit::ascii::space_type> type_subject_g =
            no_case["Entity"] >> "." >> no_case["Type"];

    //See if we have entity.type case.
    bool subject_test = qi::phrase_parse(iter_begin, iter_end, type_subject_g,
                                         boost::spirit::ascii::space);
    if (subject_test && iter_begin == iter_end) {
        m_case = new Cases::EntityTypeCase(value_str,
                                           comp_operator);
        return;
    } else {
        iter_begin = subject.begin();
    }

    //see if we have entity.outfit.*.* case;
    std::string outfit_part;
    // Grammar is defined within the function call.
    subject_test = qi::phrase_parse(
            iter_begin,
            iter_end,
            //grammar definition
            no_case["Entity"] >> "." >> no_case["outfit"] >> "."
                    >> +(qi::char_ - "=" - "<" - ">" - "!"),
            boost::spirit::ascii::space, outfit_part);
    if (subject_test && iter_begin == iter_end) {
        m_case = new Cases::OutfitCase(outfit_part, value_str, comp_operator);
        return;
    }

    //See if we have BBox case.
    //BBox grammar currently allows for volume/height/width/lenght or area
    //properties. Anything else will not be considered a match
    qi::rule<std::string::iterator, std::string(),
            boost::spirit::ascii::space_type> bbox_subject_g = no_case["Entity"]
            >> "." >> no_case["BBox"] >> "."
            >>       (no_case[qi::string("Volume")]
                    | no_case[qi::string("Height")]
                    | no_case[qi::string("Length")]
                    | no_case[qi::string("Width")]
                    | no_case[qi::string("Area")]);
    std::string bbox_property;
    subject_test = qi::phrase_parse(iter_begin, iter_end, bbox_subject_g,
                                    boost::spirit::ascii::space, bbox_property);
    if (subject_test && iter_begin == iter_end) {
        m_case = new Cases::BBoxCase(bbox_property,
                                     value_str,
                                     comp_operator);
        return;
    } else {
        iter_begin = subject.begin();
    }

    //See if we have entity.attribute case.

    //grammar for "entity.attribute" token
    //This rule will match if we use "entity.attribute" token
    //This rule has to be checked last, since it could match other cases
    qi::rule<std::string::iterator, std::string(),
            boost::spirit::ascii::space_type> attribute_subject_g =
            no_case["Entity"] >> "." >> +(qi::char_ - "=" - "<" - ">" - "!");

    std::string attribute_name;

    subject_test = qi::phrase_parse(iter_begin, iter_end, attribute_subject_g,
                                    boost::spirit::ascii::space, attribute_name);
    if (subject_test && iter_begin == iter_end) {
        m_case = new Cases::EntityAttributeCase(
                attribute_name, value_str,
                comp_operator);
        return;
    } else {
        iter_begin = subject.begin();
    }

    //See if we have memory.* case.

    //grammar for "memory.*" token
    //NOTE: there may exist a better way to ignore case sensitivity.
    qi::rule<std::string::iterator, std::string(),
            boost::spirit::ascii::space_type> memory_subject_g =
            no_case["Memory"] >> "." >> +(qi::char_);

    subject_test = qi::phrase_parse(iter_begin, iter_end, attribute_subject_g,
                                    boost::spirit::ascii::space, attribute_name);
    if (subject_test && iter_begin == iter_end) {
        //NOTE: This is not yet implemented!
        // m_case = new Cases::MemoryCase(attribute, unparsed_condition.value_str,
        //                                unparsed_condition.comp_operator);
        return;
    }
    InvalidQueryException invalid_query;
    throw invalid_query;
}
bool ParsedCondition::isTrue(LocatedEntity& entity)
{
    return m_case->testCase(entity);
}
}
