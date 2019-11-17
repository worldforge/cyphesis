//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
#include "rules/simulation/ArithmeticScript.h"
#ifndef STUB_WorldRouter_WorldRouter
#define STUB_WorldRouter_WorldRouter
WorldRouter::WorldRouter(Ref<LocatedEntity> baseEntity, EntityBuilder& entityBuilder)
    : BaseWorld(),
      m_operationsDispatcher([&](const Operation & op, Ref<LocatedEntity> from){}, [&]()->double {return getTime();}),
      m_entityCount(1),
      m_entityBuilder(entityBuilder)
{

}
#endif //STUB_WorldRouter_WorldRouter

#ifndef STUB_WorldRouter_newArithmetic
#define STUB_WorldRouter_newArithmetic
std::unique_ptr<ArithmeticScript> WorldRouter::newArithmetic(const std::string&, LocatedEntity*)
{
    return {};
}
#endif //STUB_WorldRouter_newArithmetic
