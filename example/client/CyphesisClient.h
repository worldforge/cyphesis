/*
        CyphesisClient.h
        ----------------
        begin           : 1999.12.18
        copyright       : (C) 1999 by Aloril
        email           : aloril@iki.fi
*/

#ifndef __CyphesisClient_h_
#define __CyphesisClient_h_

#ifdef _WIN32
#include <Atlas-C++/src/Net/TCPSocket.h>
#include <Atlas-C++/src/Net/XMLProtocol.h>
#include <Atlas-C++/src/Object/Object.h>
#include <Atlas-C++/src/Debug/Debug.h>
#include <Atlas-C++/src/Content/UserClient.h>
#include <Atlas-C++/src/Content/Arg.h>
#else
#include <Atlas/Objects/Decoder.h>
#endif

//#endif
#include <string.h>
#include <stdio.h>
#include <list>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace Atlas;
using namespace Objects;

class SightDecoder : public Objects::Decoder
{
protected:
   void UnknownObjectArrived(const Atlas::Message::Object&);
   void ObjectArrived(const Operation::Create&);
   void ObjectArrived(const Operation::Move&);
   void ObjectArrived(const Operation::Set&);
public:
   void processSight(const Atlas::Message::Object&);
};

class CyphesisClient : public Objects::Decoder
{
private:
   int erflag;
   int cli_fd;
   int reply_flag;
   Encoder * encoder;
   Codec<iostream> * codec;
   fstream * ios;
   string account_id;
   string character_id;
   enum {
      INIT,
      LOGGED_IN,
      CREATED_CHAR
   };
   int state;
   SightDecoder sdecode;
protected:
   //void ObjectArrived(const Atlas::Message::Object&);
   void UnknownObjectArrived(const Atlas::Message::Object&);
   void ObjectArrived(const Operation::Info&);
   void ObjectArrived(const Operation::Error&);
   void ObjectArrived(const Operation::Sight&);
public:
   CyphesisClient() : erflag(0), reply_flag(0), encoder(NULL), codec(NULL), state(INIT) { }
     
   void send(Objects::Operation::RootOperation *);
   int connect();
   void login();
   void create_char();
   void look();
   void move();
   void loop();
};

#endif
