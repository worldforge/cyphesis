// AUTOGENERATED file, created by the tool generate_stub.py, don't edit!
// If you want to add your own functionality, instead edit the stubProviderFactory_custom.h file.

#ifndef STUB_RULES_ENTITYFILTER_PROVIDERFACTORY_H
#define STUB_RULES_ENTITYFILTER_PROVIDERFACTORY_H

#include "rules/entityfilter/ProviderFactory.h"
#include "stubProviderFactory_custom.h"

namespace EntityFilter {

#ifndef STUB_ProviderFactory_createProviders
//#define STUB_ProviderFactory_createProviders
  std::shared_ptr<Consumer<QueryContext>> ProviderFactory::createProviders(SegmentsList segments) const
  {
    return *static_cast<std::shared_ptr<Consumer<QueryContext>>*>(nullptr);
  }
#endif //STUB_ProviderFactory_createProviders

#ifndef STUB_ProviderFactory_createProvider
//#define STUB_ProviderFactory_createProvider
  std::shared_ptr<Consumer<QueryContext>> ProviderFactory::createProvider(Segment segment) const
  {
    return *static_cast<std::shared_ptr<Consumer<QueryContext>>*>(nullptr);
  }
#endif //STUB_ProviderFactory_createProvider

#ifndef STUB_ProviderFactory_createSimpleGetEntityFunctionProvider
//#define STUB_ProviderFactory_createSimpleGetEntityFunctionProvider
  std::shared_ptr<Consumer<QueryContext>> ProviderFactory::createSimpleGetEntityFunctionProvider(std::shared_ptr <Consumer<QueryContext>> entity_provider) const
  {
    return *static_cast<std::shared_ptr<Consumer<QueryContext>>*>(nullptr);
  }
#endif //STUB_ProviderFactory_createSimpleGetEntityFunctionProvider

#ifndef STUB_ProviderFactory_createGetEntityFunctionProvider
//#define STUB_ProviderFactory_createGetEntityFunctionProvider
  std::shared_ptr<Consumer<QueryContext>> ProviderFactory::createGetEntityFunctionProvider(std::shared_ptr <Consumer<QueryContext>> entity_provider, SegmentsList segments) const
  {
    return *static_cast<std::shared_ptr<Consumer<QueryContext>>*>(nullptr);
  }
#endif //STUB_ProviderFactory_createGetEntityFunctionProvider

#ifndef STUB_ProviderFactory_createDynamicTypeNodeProvider
//#define STUB_ProviderFactory_createDynamicTypeNodeProvider
  std::shared_ptr<DynamicTypeNodeProvider> ProviderFactory::createDynamicTypeNodeProvider(SegmentsList segments) const
  {
    return *static_cast<std::shared_ptr<DynamicTypeNodeProvider>*>(nullptr);
  }
#endif //STUB_ProviderFactory_createDynamicTypeNodeProvider

#ifndef STUB_ProviderFactory_createEntityProvider
//#define STUB_ProviderFactory_createEntityProvider
  std::shared_ptr<T> ProviderFactory::createEntityProvider(SegmentsList segments) const
  {
    return *static_cast<std::shared_ptr<T>*>(nullptr);
  }
#endif //STUB_ProviderFactory_createEntityProvider

#ifndef STUB_ProviderFactory_createSelfEntityProvider
//#define STUB_ProviderFactory_createSelfEntityProvider
  std::shared_ptr<SelfEntityProvider> ProviderFactory::createSelfEntityProvider(SegmentsList segments) const
  {
    return *static_cast<std::shared_ptr<SelfEntityProvider>*>(nullptr);
  }
#endif //STUB_ProviderFactory_createSelfEntityProvider

#ifndef STUB_ProviderFactory_createBBoxProvider
//#define STUB_ProviderFactory_createBBoxProvider
  std::shared_ptr<BBoxProvider> ProviderFactory::createBBoxProvider(SegmentsList segments) const
  {
    return *static_cast<std::shared_ptr<BBoxProvider>*>(nullptr);
  }
#endif //STUB_ProviderFactory_createBBoxProvider

#ifndef STUB_ProviderFactory_createPropertyProvider
//#define STUB_ProviderFactory_createPropertyProvider
  std::shared_ptr<Consumer<LocatedEntity>> ProviderFactory::createPropertyProvider(SegmentsList segments) const
  {
    return *static_cast<std::shared_ptr<Consumer<LocatedEntity>>*>(nullptr);
  }
#endif //STUB_ProviderFactory_createPropertyProvider

#ifndef STUB_ProviderFactory_createMapProvider
//#define STUB_ProviderFactory_createMapProvider
  std::shared_ptr<MapProvider> ProviderFactory::createMapProvider(SegmentsList segments) const
  {
    return *static_cast<std::shared_ptr<MapProvider>*>(nullptr);
  }
#endif //STUB_ProviderFactory_createMapProvider

#ifndef STUB_ProviderFactory_createTypeNodeProvider
//#define STUB_ProviderFactory_createTypeNodeProvider
  std::shared_ptr<TypeNodeProvider> ProviderFactory::createTypeNodeProvider(SegmentsList segments) const
  {
    return *static_cast<std::shared_ptr<TypeNodeProvider>*>(nullptr);
  }
#endif //STUB_ProviderFactory_createTypeNodeProvider

#ifndef STUB_ProviderFactory_createMemoryProvider
//#define STUB_ProviderFactory_createMemoryProvider
  std::shared_ptr<MemoryProvider> ProviderFactory::createMemoryProvider(SegmentsList segments) const
  {
    return *static_cast<std::shared_ptr<MemoryProvider>*>(nullptr);
  }
#endif //STUB_ProviderFactory_createMemoryProvider


}  // namespace EntityFilter

#endif