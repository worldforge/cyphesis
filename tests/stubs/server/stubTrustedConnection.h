// AUTOGENERATED file, created by the tool generate_stub.py, don't edit!
// If you want to add your own functionality, instead edit the stubTrustedConnection_custom.h file.

#ifndef STUB_SERVER_TRUSTEDCONNECTION_H
#define STUB_SERVER_TRUSTEDCONNECTION_H

#include "server/TrustedConnection.h"
#include "stubTrustedConnection_custom.h"

#ifndef STUB_TrustedConnection_newAccount
//#define STUB_TrustedConnection_newAccount
  std::unique_ptr<Account> TrustedConnection::newAccount(const std::string& type, const std::string& username, const std::string& passwd, const std::string& id, long intId)
  {
    return *static_cast<std::unique_ptr<Account>*>(nullptr);
  }
#endif //STUB_TrustedConnection_newAccount

#ifndef STUB_TrustedConnection_TrustedConnection
//#define STUB_TrustedConnection_TrustedConnection
   TrustedConnection::TrustedConnection(CommSocket& client, ServerRouting& svr, const std::string& addr, const std::string& id, long iid)
    : Connection(client, svr, addr, id, iid)
  {
    
  }
#endif //STUB_TrustedConnection_TrustedConnection

#ifndef STUB_TrustedConnection_TrustedConnection_DTOR
//#define STUB_TrustedConnection_TrustedConnection_DTOR
   TrustedConnection::~TrustedConnection()
  {
    
  }
#endif //STUB_TrustedConnection_TrustedConnection_DTOR


#endif