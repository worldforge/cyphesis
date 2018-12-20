// AUTOGENERATED file, created by the tool generate_stub.py, don't edit!
// If you want to add your own functionality, instead edit the stubMemEntity_custom.h file.

#ifndef STUB_RULES_AI_MEMENTITY_H
#define STUB_RULES_AI_MEMENTITY_H

#include "rules/ai/MemEntity.h"
#include "stubMemEntity_custom.h"

#ifndef STUB_MemEntity_MemEntity
//#define STUB_MemEntity_MemEntity
   MemEntity::MemEntity(const std::string & id, long intId)
    : LocatedEntity(id, intId)
  {
    
  }
#endif //STUB_MemEntity_MemEntity

#ifndef STUB_MemEntity_externalOperation
//#define STUB_MemEntity_externalOperation
  void MemEntity::externalOperation(const Operation & op, Link &)
  {
    
  }
#endif //STUB_MemEntity_externalOperation

#ifndef STUB_MemEntity_operation
//#define STUB_MemEntity_operation
  void MemEntity::operation(const Operation &, OpVector &)
  {
    
  }
#endif //STUB_MemEntity_operation

#ifndef STUB_MemEntity_setAttr
//#define STUB_MemEntity_setAttr
  PropertyBase* MemEntity::setAttr(const std::string & name, const Atlas::Message::Element & attr)
  {
    return nullptr;
  }
#endif //STUB_MemEntity_setAttr

#ifndef STUB_MemEntity_destroy
//#define STUB_MemEntity_destroy
  void MemEntity::destroy()
  {
    
  }
#endif //STUB_MemEntity_destroy

#ifndef STUB_MemEntity_getProperty
//#define STUB_MemEntity_getProperty
  const PropertyBase* MemEntity::getProperty(const std::string & name) const
  {
    return nullptr;
  }
#endif //STUB_MemEntity_getProperty


#endif