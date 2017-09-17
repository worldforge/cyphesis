//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
#ifndef STUB_WorldRouter_WorldRouter
#define STUB_WorldRouter_WorldRouter
WorldRouter::WorldRouter(const SystemTime & systemTime)
    : BaseWorld(*new Entity(consts::rootWorldId, consts::rootWorldIntId)),
      m_operationsDispatcher([&](const Operation & op, LocatedEntity & from){}, [&]()->double {return getTime();}), m_entityCount(1)
{

}
#endif //STUB_WorldRouter_WorldRouter
