#include "OutfitCase.h"
#include "AttributeCases.h"
#include "BBoxCase.h"
#include "../OutfitProperty.h"
#include "rulesets/LocatedEntity.h"
#include <Atlas/Objects/Operation.h>

namespace EntityFilter
{
namespace Cases
{

OutfitCase::OutfitCase(const std::string& outfit_part,
                       const std::string& value,
                       const std::string& comp_operator)
{
    auto iter_begin = outfit_part.begin();
    auto iter_end = outfit_part.end();
    std::list<std::string> outfit_list;
    std::string property;

    using boost::spirit::qi::char_;
    using namespace boost::spirit;

    //Parse the subject.
    //the first token is the outfit part that we are looking for.
    //the rest of the string is used to compose a new ParsedCondition
    std::string rest;
    bool subject_check = boost::spirit::qi::phrase_parse(
            iter_begin, iter_end, +(char_ - ".") >> "." >> +(char_),
            boost::spirit::ascii::space, m_outfitPart, rest);

    if (subject_check && iter_begin == iter_end) {
        //Combine "Entity." with the rest of the subject to create a new ParsedCondition.
        //We can use "Entity." since we know that the match applies to the entity from Outfit.
        m_condition = new ParsedCondition("Entity." + rest, comp_operator,
                                          value);
        return;
    } else {
        InvalidQueryException invalid_query;
        throw invalid_query;
    }
}

OutfitCase::~OutfitCase(){
    delete m_condition;
}

bool OutfitCase::testCase(LocatedEntity& entity)
{
    //Get the outfit property, check it and then retrieve the entity we need if it exists.
    const OutfitProperty* prop = entity.getPropertyClass<OutfitProperty>(
            "outfit");

    if (prop == 0) {
        return false;
    }

    LocatedEntity* ent = prop->getEntity(m_outfitPart);
    if (ent == 0) {
        return false;
    }
    return m_condition->isTrue(*ent);
}

}
}
