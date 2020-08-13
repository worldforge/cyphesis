// AUTOGENERATED file, created by the tool generate_stub.py, don't edit!
// If you want to add your own functionality, instead edit the stubMonitors_custom.h file.

#ifndef STUB_COMMON_MONITORS_H
#define STUB_COMMON_MONITORS_H

#include "common/Monitors.h"
#include "stubMonitors_custom.h"

#ifndef STUB_Monitors_Monitors
//#define STUB_Monitors_Monitors
   Monitors::Monitors()
    : Singleton()
  {
    
  }
#endif //STUB_Monitors_Monitors

#ifndef STUB_Monitors_Monitors_DTOR
//#define STUB_Monitors_Monitors_DTOR
   Monitors::~Monitors()
  {
    
  }
#endif //STUB_Monitors_Monitors_DTOR

#ifndef STUB_Monitors_insert
//#define STUB_Monitors_insert
  void Monitors::insert(const std::string &, const Atlas::Message::Element &)
  {
    
  }
#endif //STUB_Monitors_insert

#ifndef STUB_Monitors_watch
//#define STUB_Monitors_watch
  void Monitors::watch(const std::string &, VariableBase *)
  {
    
  }
#endif //STUB_Monitors_watch

#ifndef STUB_Monitors_send
//#define STUB_Monitors_send
  void Monitors::send(std::ostream &) const
  {
    
  }
#endif //STUB_Monitors_send

#ifndef STUB_Monitors_sendNumerics
//#define STUB_Monitors_sendNumerics
  void Monitors::sendNumerics(std::ostream &) const
  {
    
  }
#endif //STUB_Monitors_sendNumerics

#ifndef STUB_Monitors_readVariable
//#define STUB_Monitors_readVariable
  int Monitors::readVariable(const std::string& key, std::ostream& out_stream) const
  {
    return 0;
  }
#endif //STUB_Monitors_readVariable


#endif