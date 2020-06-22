// AUTOGENERATED file, created by the tool generate_stub.py, don't edit!
// If you want to add your own functionality, instead edit the stubAdmin_custom.h file.

#ifndef STUB_SERVER_ADMIN_H
#define STUB_SERVER_ADMIN_H

#include "server/Admin.h"
#include "stubAdmin_custom.h"

#ifndef STUB_Admin_opDispatched
//#define STUB_Admin_opDispatched
  void Admin::opDispatched(const Operation& op)
  {
    
  }
#endif //STUB_Admin_opDispatched

#ifndef STUB_Admin_setAttribute
//#define STUB_Admin_setAttribute
  void Admin::setAttribute(const Atlas::Objects::Root& arg)
  {
    
  }
#endif //STUB_Admin_setAttribute

#ifndef STUB_Admin_createMind
//#define STUB_Admin_createMind
  ExternalMind* Admin::createMind(const Ref<LocatedEntity>& entity) const
  {
    return nullptr;
  }
#endif //STUB_Admin_createMind

#ifndef STUB_Admin_processExternalOperation
//#define STUB_Admin_processExternalOperation
  void Admin::processExternalOperation(const Operation& op, OpVector& res)
  {
    
  }
#endif //STUB_Admin_processExternalOperation

#ifndef STUB_Admin_Admin
//#define STUB_Admin_Admin
   Admin::Admin(Connection* conn, const std::string& username, const std::string& passwd, const std::string& id, long intId)
    : Account(conn, username, passwd, id, intId)
  {
    
  }
#endif //STUB_Admin_Admin

#ifndef STUB_Admin_Admin_DTOR
//#define STUB_Admin_Admin_DTOR
   Admin::~Admin()
  {
    
  }
#endif //STUB_Admin_Admin_DTOR

#ifndef STUB_Admin_getType
//#define STUB_Admin_getType
  const char* Admin::getType() const
  {
    return nullptr;
  }
#endif //STUB_Admin_getType

#ifndef STUB_Admin_LogoutOperation
//#define STUB_Admin_LogoutOperation
  void Admin::LogoutOperation(const Operation&, OpVector&)
  {
    
  }
#endif //STUB_Admin_LogoutOperation

#ifndef STUB_Admin_GetOperation
//#define STUB_Admin_GetOperation
  void Admin::GetOperation(const Operation&, OpVector&)
  {
    
  }
#endif //STUB_Admin_GetOperation

#ifndef STUB_Admin_CreateOperation
//#define STUB_Admin_CreateOperation
  void Admin::CreateOperation(const Operation&, OpVector&)
  {
    
  }
#endif //STUB_Admin_CreateOperation

#ifndef STUB_Admin_SetOperation
//#define STUB_Admin_SetOperation
  void Admin::SetOperation(const Operation&, OpVector&)
  {
    
  }
#endif //STUB_Admin_SetOperation

#ifndef STUB_Admin_OtherOperation
//#define STUB_Admin_OtherOperation
  void Admin::OtherOperation(const Operation&, OpVector&)
  {
    
  }
#endif //STUB_Admin_OtherOperation

#ifndef STUB_Admin_customMonitorOperation
//#define STUB_Admin_customMonitorOperation
  void Admin::customMonitorOperation(const Operation&, OpVector&)
  {
    
  }
#endif //STUB_Admin_customMonitorOperation


#endif