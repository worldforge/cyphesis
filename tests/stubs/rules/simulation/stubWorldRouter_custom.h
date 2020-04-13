//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
#ifndef STUB_WorldRouter_WorldRouter
#define STUB_WorldRouter_WorldRouter
WorldRouter::WorldRouter(Ref<LocatedEntity> baseEntity, EntityCreator& entityCreator)
        : BaseWorld(),
          m_operationsDispatcher([&](const Operation & op, Ref<LocatedEntity> from){}, [&]()->double {return getTime();}),
          m_entityCount(1),
          m_entityCreator(entityCreator)
{

}
#endif //STUB_WorldRouter_WorldRouter