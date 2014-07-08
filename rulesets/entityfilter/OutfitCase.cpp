#include "OutfitCase.h"
#include "AttributeCases.h"
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
        m_outfitPart(outfit_part), m_outfitProperty(outfit_property)
//FIXME: nested outfits are not currently supported
{
    if (outfit_property == "type" | outfit_property == "Type") {
        m_comparer = EntityTypeCase::getComparer(value, comp_operator);
    } else {
        m_comparer = EntityAttributeCase::getComparer(outfit_property, value,
                                                      comp_operator);
    }
}
bool OutfitCase::testCase(LocatedEntity& entity)
{
    const OutfitProperty* prop = entity.getPropertyClass<OutfitProperty>("outfit");

    if (prop == 0) {
        return false;
    }
    LocatedEntity* ent = prop->getEntity(m_outfitPart);
    if (ent == 0) {
        return false;
    }
    return m_comparer->compare(*ent);

}
}
}
