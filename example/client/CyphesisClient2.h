// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

/*
        CyphesisClient.h
        ----------------
        begin           : 1999.12.18
        copyright       : (C) 1999 by Aloril
                          2000 by Alistair Riddoch and Aloril
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

#define USE_SOCKET_PP 1

#if USE_SOCKET_PP
// iosockinet - the iostream-based socket class
#include <sockinet.h>
#else

class sockbuf : public filebuf {
  public:
    sockbuf() { }
    sockbuf(int fd) : filebuf(fd) { }
    virtual streampos sys_seek(streamoff, _seek_dir) { return streampos(); }
};

#endif

using namespace Atlas;
using namespace Objects;

class SightDecoder : public Objects::Decoder
{
protected:
   void unknownObjectArrived(const Root&);
   void objectArrived(const Operation::Create&);
   void objectArrived(const Operation::Move&);
   void objectArrived(const Operation::Set&);
public:
   void processSight(const Root& o);
};

class CyphesisClient : public Objects::Decoder
{
private:
   int erflag;
   int cli_fd;
   int reply_flag;
   Encoder * encoder;
   Codec<iostream> * codec;
#if USE_SOCKET_PP
   iosockinet *ios;
#else
   fstream * ios;
   //iostream * ios;
#endif
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
   //void objectArrived(const Atlas::Message::Object&);
   void unknownObjectArrived(const Atlas::Message::Object&);
   void objectArrived(const Operation::Info&);
   void objectArrived(const Operation::Error&);
   void objectArrived(const Operation::Sight&);
public:
   CyphesisClient() : erflag(0), reply_flag(0),
     encoder(NULL), codec(NULL), state(INIT) { }
     
   void send(Objects::Operation::RootOperation &);
   int connect();
   void login();
   void createChar();
   void look();
   void move();
   void loop();
};

#endif
