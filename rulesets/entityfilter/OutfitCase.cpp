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
                       const std::string& outfit_property,
                       const std::string& value,
                       const std::string& comp_operator) :
        m_outfitPart(outfit_part)
{
    auto iter_begin = outfit_property.begin();
    auto iter_end = outfit_property.end();
    std::list<std::string> outfit_list;
    std::string property;

    using boost::spirit::qi::char_;
    using namespace boost::spirit;

    //Record every nested outfit part into a list
    //We can find nested outfits by parsing repetitions of "outfit".[key]"
    //The rest of the string should indicate property we're looking for
    boost::spirit::qi::phrase_parse(
            iter_begin,
            iter_end,
            *("outfit" >> qi::lit(".") >> +(char_ - ".") >> ".")
            >> +(char_ - "=" - "<" - ">" - "!"),
            boost::spirit::ascii::space, m_outfitParts, m_outfitProperty);

    //Since Outfit case can be any case, we'll have to check subject again.
    iter_begin = m_outfitProperty.begin();
    iter_end = m_outfitProperty.end();

    if (m_outfitProperty == "type" || m_outfitProperty == "Type") {
        m_case = new EntityTypeCase(value, comp_operator);
        return;
    }

    //Check if we have bbox_case
    std::string bbox_property;
    bool subject_check = qi::phrase_parse(iter_begin, iter_end,
                                          qi::no_case["BBox"] >> "." >>  +(char_),
                                          ascii::space, bbox_property);
    if (subject_check && iter_begin == iter_end) {
        m_case = new BBoxCase(bbox_property, value, comp_operator);
        return;
    }

    //use soft property by default
    m_case = new EntityAttributeCase(m_outfitProperty, comp_operator, value);

}

OutfitCase::~OutfitCase(){
    delete m_case;
}

bool OutfitCase::testCase(LocatedEntity& entity)
{
    //Get our initial outfit entity.
    const OutfitProperty* prop = entity.getPropertyClass<OutfitProperty>(
            "outfit");

    if (prop == 0) {
        return false;
    }

    LocatedEntity* ent = prop->getEntity(m_outfitPart);
    if (ent == 0) {
        return false;
    }

    //If we have a nested case, go through the chain and get the right entity.
    for (auto& iter : m_outfitParts) {
        prop = ent->getPropertyClass<OutfitProperty>("outfit");

        if (prop == 0) {
            return false;
        }

        ent = prop->getEntity(iter);
        if (ent == 0) {
            return false;
        }
    }
    return m_case->testCase(*ent);
}
}
}
