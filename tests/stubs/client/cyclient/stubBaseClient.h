// AUTOGENERATED file, created by the tool generate_stub.py, don't edit!
// If you want to add your own functionality, instead edit the stubBaseClient_custom.h file.

#ifndef STUB_CLIENT_CYCLIENT_BASECLIENT_H
#define STUB_CLIENT_CYCLIENT_BASECLIENT_H

#include "client/cyclient/BaseClient.h"
#include "stubBaseClient_custom.h"

#ifndef STUB_BaseClient_extractFirstArg
//#define STUB_BaseClient_extractFirstArg
  Atlas::Objects::Entity::RootEntity BaseClient::extractFirstArg(Atlas::Objects::Operation::RootOperation op)
  {
    return *static_cast<Atlas::Objects::Entity::RootEntity*>(nullptr);
  }
#endif //STUB_BaseClient_extractFirstArg

#ifndef STUB_BaseClient_BaseClient
//#define STUB_BaseClient_BaseClient
   BaseClient::BaseClient(boost::asio::io_context& io_context, Atlas::Objects::Factories& factories, TypeStore& typeStore)
  {
    
  }
#endif //STUB_BaseClient_BaseClient

#ifndef STUB_BaseClient_BaseClient_DTOR
//#define STUB_BaseClient_BaseClient_DTOR
   BaseClient::~BaseClient()
  {
    
  }
#endif //STUB_BaseClient_BaseClient_DTOR

#ifndef STUB_BaseClient_createSystemAccount
//#define STUB_BaseClient_createSystemAccount
  Atlas::Objects::Root BaseClient::createSystemAccount()
  {
    return *static_cast<Atlas::Objects::Root*>(nullptr);
  }
#endif //STUB_BaseClient_createSystemAccount

#ifndef STUB_BaseClient_createAccount
//#define STUB_BaseClient_createAccount
  Atlas::Objects::Root BaseClient::createAccount(const std::string& name, const std::string& pword)
  {
    return *static_cast<Atlas::Objects::Root*>(nullptr);
  }
#endif //STUB_BaseClient_createAccount

#ifndef STUB_BaseClient_createCharacter
//#define STUB_BaseClient_createCharacter
  Ref<CreatorClient> BaseClient::createCharacter(const std::string& name)
  {
    return *static_cast<Ref<CreatorClient>*>(nullptr);
  }
#endif //STUB_BaseClient_createCharacter

#ifndef STUB_BaseClient_logout
//#define STUB_BaseClient_logout
  void BaseClient::logout()
  {
    
  }
#endif //STUB_BaseClient_logout

#ifndef STUB_BaseClient_handleNet
//#define STUB_BaseClient_handleNet
  void BaseClient::handleNet()
  {
    
  }
#endif //STUB_BaseClient_handleNet

#ifndef STUB_BaseClient_idle
//#define STUB_BaseClient_idle
  void BaseClient::idle()
  {
    
  }
#endif //STUB_BaseClient_idle

#ifndef STUB_BaseClient_send
//#define STUB_BaseClient_send
  void BaseClient::send(const Atlas::Objects::Operation::RootOperation& op)
  {
    
  }
#endif //STUB_BaseClient_send


#endif