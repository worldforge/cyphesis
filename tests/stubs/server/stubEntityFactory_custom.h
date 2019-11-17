//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
#ifndef STUB_EntityFactoryBase_duplicateFactory
#define STUB_EntityFactoryBase_duplicateFactory
std::unique_ptr<EntityFactoryBase> EntityFactoryBase::duplicateFactory()
{
    return std::unique_ptr<EntityFactoryBase>();
}
#endif //STUB_EntityFactoryBase_duplicateFactory

#ifndef STUB_EntityFactory_duplicateFactory
#define STUB_EntityFactory_duplicateFactory
template <typename T>
std::unique_ptr<EntityFactoryBase> EntityFactory<T>::duplicateFactory()
{
    return std::unique_ptr<EntityFactoryBase>();
}
#endif //STUB_EntityFactory_duplicateFactory
