#include "AbstractCase.h"
#include "AttributeComparerWrapper.h"

namespace EntityFilter
{
namespace Cases
{

class OutfitCase : public AbstractCase {
    public:
        OutfitCase(const std::string& outfit_part,
                   const std::string& outfit_property,
                   const std::string& value,
                   const std::string& comp_operator);
        ~OutfitCase()
        {
        }
        ;
        bool testCase(LocatedEntity& entity);
    private:
        std::string m_outfitPart;
        std::string m_outfitProperty;
        std::string m_value;
        std::string m_compOperator;
        Comparers::AttributeComparerWrapper* m_comparer;
};

}
}
