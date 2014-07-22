#include "Filter.h"
#include "AbstractCase.h"
#include "AttributeComparerWrapper.h"

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

namespace EntityFilter
{
namespace Cases
{
///\brief class to handle Outfit Case
///
///Upon initialization, parse for what part of the outfit we are looking for.
///
///Outfit case contains another ParsedCondition and simply serves as a "bridge"
///We simply get the Entity in a given outfit part and use it to check for a match instead
///of our original entity
class OutfitCase : public AbstractCase {
    public:
        ///\brief construct an outfit case by determining which outfit part we are looking for
        ///and composing a new ParsedCondition.
        OutfitCase(const std::string& outfit_part,
                   const std::string& value,
                   const std::string& comp_operator);
        ~OutfitCase();
        ///\brief match an entity by retrieving another entity from the given outfit part (if it exists)
        ///and using it to match the given ParsedCondition.
        bool testCase(LocatedEntity& entity);
    private:
        std::string m_outfitPart;
        ParsedCondition* m_condition;
};

}
}
