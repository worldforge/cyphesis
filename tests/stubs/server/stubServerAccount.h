// AUTOGENERATED file, created by the tool generate_stub.py, don't edit!
// If you want to add your own functionality, instead edit the stubServerAccount_custom.h file.

#ifndef STUB_SERVER_SERVERACCOUNT_H
#define STUB_SERVER_SERVERACCOUNT_H

#include "server/ServerAccount.h"
#include "stubServerAccount_custom.h"

#ifndef STUB_ServerAccount_ServerAccount
//#define STUB_ServerAccount_ServerAccount
   ServerAccount::ServerAccount(Connection * conn, const std::string & username, const std::string & passwd, RouterId id)
    : Account(conn, username, passwd, id)
  {
    
  }
#endif //STUB_ServerAccount_ServerAccount

#ifndef STUB_ServerAccount_getType
//#define STUB_ServerAccount_getType
  const char* ServerAccount::getType() const
  {
    return nullptr;
  }
#endif //STUB_ServerAccount_getType


#endif