//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
#ifndef STUB_ArchetypeFactory_ArchetypeFactory
#define STUB_ArchetypeFactory_ArchetypeFactory
ArchetypeFactory::ArchetypeFactory(ArchetypeFactory & o)
    : EntityKit(o)
    , m_entityBuilder(o.m_entityBuilder)
    , m_parent(nullptr)
{

}

ArchetypeFactory::ArchetypeFactory(EntityBuilder& entityBuilder)
    : EntityKit()
    , m_entityBuilder(entityBuilder)
    , m_parent(nullptr)
{

}
#endif //STUB_ArchetypeFactory_ArchetypeFactory
