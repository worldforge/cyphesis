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
///Upon initialization, parse for what part of the outfit we are looking for
///and what property we want to compare.
///
///Outfit case can contain any other case, except that we use the entity
///pointed by the outfit for comparison
class OutfitCase : public AbstractCase {
    public:
        OutfitCase(const std::string& outfit_part,
                   const std::string& outfit_property,
                   const std::string& value,
                   const std::string& comp_operator);
        ~OutfitCase();
        bool testCase(LocatedEntity& entity);
    private:
        //first outfit part
        std::string m_outfitPart;
        //a list of nested outfit parts
        std::list<std::string> m_outfitParts;
        std::string m_outfitProperty;
        std::string m_compOperator;
        AbstractCase* m_case;
};

}
}
