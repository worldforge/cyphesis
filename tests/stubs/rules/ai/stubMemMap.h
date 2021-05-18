// AUTOGENERATED file, created by the tool generate_stub.py, don't edit!
// If you want to add your own functionality, instead edit the stubMemMap_custom.h file.

#ifndef STUB_RULES_AI_MEMMAP_H
#define STUB_RULES_AI_MEMMAP_H

#include "rules/ai/MemMap.h"
#include "stubMemMap_custom.h"

#ifndef STUB_MemMap_readEntity
//#define STUB_MemMap_readEntity
  void MemMap::readEntity(const Ref<MemEntity>&, const Atlas::Objects::Entity::RootEntity&, double timestamp)
  {
    
  }
#endif //STUB_MemMap_readEntity

#ifndef STUB_MemMap_updateEntity
//#define STUB_MemMap_updateEntity
  void MemMap::updateEntity(const Ref<MemEntity>&, const Atlas::Objects::Entity::RootEntity&, double timestamp)
  {
    
  }
#endif //STUB_MemMap_updateEntity

#ifndef STUB_MemMap_newEntity
//#define STUB_MemMap_newEntity
  Ref<MemEntity> MemMap::newEntity(const std::string&, long, const Atlas::Objects::Entity::RootEntity&, double timestamp)
  {
    return *static_cast<Ref<MemEntity>*>(nullptr);
  }
#endif //STUB_MemMap_newEntity

#ifndef STUB_MemMap_addContents
//#define STUB_MemMap_addContents
  void MemMap::addContents(const Atlas::Objects::Entity::RootEntity&)
  {
    
  }
#endif //STUB_MemMap_addContents

#ifndef STUB_MemMap_addId
//#define STUB_MemMap_addId
  Ref<MemEntity> MemMap::addId(const std::string&, long)
  {
    return *static_cast<Ref<MemEntity>*>(nullptr);
  }
#endif //STUB_MemMap_addId

#ifndef STUB_MemMap_applyTypePropertiesToEntity
//#define STUB_MemMap_applyTypePropertiesToEntity
  void MemMap::applyTypePropertiesToEntity(const Ref<MemEntity>& entity)
  {
    
  }
#endif //STUB_MemMap_applyTypePropertiesToEntity

#ifndef STUB_MemMap_MemMap
//#define STUB_MemMap_MemMap
   MemMap::MemMap(TypeResolver& typeResolver)
    : m_listener(nullptr)
  {
    
  }
#endif //STUB_MemMap_MemMap

#ifndef STUB_MemMap_MemMap_DTOR
//#define STUB_MemMap_MemMap_DTOR
   MemMap::~MemMap()
  {
    
  }
#endif //STUB_MemMap_MemMap_DTOR

#ifndef STUB_MemMap_resolveEntitiesForType
//#define STUB_MemMap_resolveEntitiesForType
  std::vector<Ref<MemEntity>> MemMap::resolveEntitiesForType(const TypeNode* typeNode)
  {
    return std::vector<Ref<MemEntity>>();
  }
#endif //STUB_MemMap_resolveEntitiesForType

#ifndef STUB_MemMap_addEntity
//#define STUB_MemMap_addEntity
  void MemMap::addEntity(const Ref<MemEntity>&)
  {
    
  }
#endif //STUB_MemMap_addEntity

#ifndef STUB_MemMap_find
//#define STUB_MemMap_find
  bool MemMap::find(const std::string& id) const
  {
    return false;
  }
#endif //STUB_MemMap_find

#ifndef STUB_MemMap_sendLooks
//#define STUB_MemMap_sendLooks
  void MemMap::sendLooks(OpVector&)
  {
    
  }
#endif //STUB_MemMap_sendLooks

#ifndef STUB_MemMap_del
//#define STUB_MemMap_del
  Ref<MemEntity> MemMap::del(const std::string& id)
  {
    return *static_cast<Ref<MemEntity>*>(nullptr);
  }
#endif //STUB_MemMap_del

#ifndef STUB_MemMap_get
//#define STUB_MemMap_get
  Ref<MemEntity> MemMap::get(const std::string& id) const
  {
    return *static_cast<Ref<MemEntity>*>(nullptr);
  }
#endif //STUB_MemMap_get

#ifndef STUB_MemMap_getAdd
//#define STUB_MemMap_getAdd
  Ref<MemEntity> MemMap::getAdd(const std::string& id)
  {
    return *static_cast<Ref<MemEntity>*>(nullptr);
  }
#endif //STUB_MemMap_getAdd

#ifndef STUB_MemMap_updateAdd
//#define STUB_MemMap_updateAdd
  Ref<MemEntity> MemMap::updateAdd(const Atlas::Objects::Entity::RootEntity&, const double&)
  {
    return *static_cast<Ref<MemEntity>*>(nullptr);
  }
#endif //STUB_MemMap_updateAdd

#ifndef STUB_MemMap_addEntityMemory
//#define STUB_MemMap_addEntityMemory
  void MemMap::addEntityMemory(const std::string& id, const std::string& memory, Atlas::Message::Element value)
  {
    
  }
#endif //STUB_MemMap_addEntityMemory

#ifndef STUB_MemMap_removeEntityMemory
//#define STUB_MemMap_removeEntityMemory
  void MemMap::removeEntityMemory(const std::string& id, const std::string& memory)
  {
    
  }
#endif //STUB_MemMap_removeEntityMemory

#ifndef STUB_MemMap_recallEntityMemory
//#define STUB_MemMap_recallEntityMemory
  void MemMap::recallEntityMemory(const std::string& id, const std::string& memory, Atlas::Message::Element& value) const
  {
    
  }
#endif //STUB_MemMap_recallEntityMemory

#ifndef STUB_MemMap_getEntityRelatedMemory
//#define STUB_MemMap_getEntityRelatedMemory
  const std::map<std::string, std::map<std::string, Atlas::Message::Element>>& MemMap::getEntityRelatedMemory() const
  {
    static std::map<std::string, std::map<std::string, Atlas::Message::Element>> instance; return instance;
  }
#endif //STUB_MemMap_getEntityRelatedMemory

#ifndef STUB_MemMap_findByType
//#define STUB_MemMap_findByType
  EntityVector MemMap::findByType(const std::string& what)
  {
    return *static_cast<EntityVector*>(nullptr);
  }
#endif //STUB_MemMap_findByType

#ifndef STUB_MemMap_findByLocation
//#define STUB_MemMap_findByLocation
  EntityVector MemMap::findByLocation(const EntityLocation& where, WFMath::CoordType radius, const std::string& what)
  {
    return *static_cast<EntityVector*>(nullptr);
  }
#endif //STUB_MemMap_findByLocation

#ifndef STUB_MemMap_check
//#define STUB_MemMap_check
  void MemMap::check(const double&)
  {
    
  }
#endif //STUB_MemMap_check

#ifndef STUB_MemMap_flush
//#define STUB_MemMap_flush
  void MemMap::flush()
  {
    
  }
#endif //STUB_MemMap_flush

#ifndef STUB_MemMap_setListener
//#define STUB_MemMap_setListener
  void MemMap::setListener(MapListener* listener)
  {
    
  }
#endif //STUB_MemMap_setListener

#ifndef STUB_MemMap_collectTypeResolverOps
//#define STUB_MemMap_collectTypeResolverOps
  void MemMap::collectTypeResolverOps(OpVector& res)
  {
    
  }
#endif //STUB_MemMap_collectTypeResolverOps

#ifndef STUB_MemMap_getTypeStore
//#define STUB_MemMap_getTypeStore
  const TypeStore& MemMap::getTypeStore() const
  {
    return *static_cast<const TypeStore*>(nullptr);
  }
#endif //STUB_MemMap_getTypeStore


#endif