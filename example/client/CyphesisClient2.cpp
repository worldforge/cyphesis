// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

/*
CyphesisClient.cpp
------------------
begin           : 1999.12.18
copyright       : (C) 1999 by Aloril
                  2000 by Alistair Riddoch and Aloril
email           : aloril@iki.fi
*/

/* Created a branch to tag to develop the Atlas version 0.4
 * version of this client 
 *
 * Al Riddoch
 * <ajr@ecs.soton.ac.uk>
 */

//#include <stdiostream.h>
#include <fstream.h>

extern "C" {
    #include <stdio.h>
    #include <stdlib.h>
    #include <sys/time.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
}

// iosockinet - the iostream-based socket class
// Atlas negotiation
#include <Atlas/Net/Stream.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Objects/Entity.h>
#include <Atlas/Objects/Operation.h>
#include "Atlas/Objects/loadDefaults.h"
// The DebugBridge
#include "CyphesisClient2.h"

#ifdef _WIN32
#define sleep(x) Sleep(x*1000)
#define usleep(x) Sleep(x/1000)
#else
#include <sys/time.h>
#include <unistd.h>
#endif
#include <math.h>

std::string defMessage="No message was given by server!";
std::string emptyString="";

using namespace Atlas;
using namespace std;

int main(int argc, char ** argv)
{
  try {
    loadDefaults("../../../../protocols/atlas/spec/atlas.xml");
  } catch(DefaultLoadingException e) {
    cout << "DefaultLoadingException: "
         << e.msg << endl;
    return 1;
  }

  CyphesisClient bridge;
  if (bridge.connect() != 0) {
    cout << "Failed to connect" << endl;
    exit(1);
  }
  bridge.login();
  bridge.createChar();
  bridge.look();
  bridge.move();
  bridge.loop();
}

void CyphesisClient::send(Objects::Operation::RootOperation & op)
{
    encoder->streamMessage((Root&)op);
    *ios << flush;
}

void CyphesisClient::login()
{
   Objects::Entity::Account account;
   Objects::Operation::Login l;
   erflag = 0;
   reply_flag = 0;

   account->setId("al");
   account->setAttr("password", std::string("ping"));

   l->setArgs1((Root&)account);


   send((Operation::RootOperation&)l);
   
   while (!reply_flag) {
      codec->poll();
   }
   if (!erflag) {
      cout << "login was a success" << endl << flush;
      return;
   }
   cout << "login failed" << endl << flush;

   Objects::Operation::Create c;
   erflag = 0;
   reply_flag = 0;

   c->setArgs1((Root&)account);

   send((Operation::RootOperation&)c);

   while (!reply_flag) {
      codec->poll();
   }
   if (!erflag) {
      cout << "create account was a success" << endl << flush;
      return;
   }
}

int CyphesisClient::connect()
{

#if USE_SOCKET_PP
  ios = new iosockinet(sockbuf::sock_stream);
  cout << "Connecting..." << flush;
  (*ios)->connect("127.0.0.1", 6767);
#else
  struct sockaddr_in sin;

  cli_fd = socket(PF_INET, SOCK_STREAM, 0);
  if (cli_fd < 0) {
    return 1;
  }
  sin.sin_family = AF_INET;
  sin.sin_port = htons(6767);
  //sin.sin_port = htons(1111);
  sin.sin_addr.s_addr = htonl(0x7f000001);

  cout << "Connecting to cyphesis.." << endl << flush;

  if (::connect(cli_fd, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
    close(cli_fd);
    return -1;
  }
  cout << "Connected to cyphesis" << endl << flush;
  // Connect to the server
  //ios = new fstream(cli_fd);
  sockbuf client_buf(cli_fd);
  ios = new iostream(&client_buf);
#endif

  // Do client negotiation with the server
  Net::StreamConnect conn("cyphesis_client", *ios, this);

  cout << "Negotiating... " << flush;
  // conn.Poll() does all the negotiation
  while (conn.getState() == Negotiate<iostream>::IN_PROGRESS) {
    conn.poll();
  }
  cout << "done" << endl;

  // Check whether negotiation was successful
  if (conn.getState() == Negotiate<iostream>::FAILED) {
      cerr << "Failed to negotiate" << endl;
      exit(2);
  }
  // Negotiation was successful

  // Get the codec that negotiation established
  codec = conn.getCodec();

  // This should always be sent at the beginning of a session

  encoder = new Objects::ObjectsEncoder(codec);

  codec->streamBegin();
  return 0;

}

void CyphesisClient::createChar()
{
   Objects::Entity::Player character;
   Objects::Operation::Create c;
   erflag = 0;
   reply_flag = 0;

   list<std::string> parlist(1, std::string("farmer"));
   character->setParents(parlist);
   character->setName("Al Riddoch");

   c->setArgs1((Root&)character);
   c->setFrom(account_id);
   
   send((Operation::RootOperation&)c);
   while (!reply_flag) {
      codec->poll();
   }
   if (!erflag) {
       cout << "Created character: " << character_id << " :using account: " << account_id << " :" << endl << flush;
   }
}

void CyphesisClient::look()
{
   Objects::Operation::Look l;
   l->setFrom(character_id);

   send((Operation::RootOperation&)l);
}

void CyphesisClient::move()
{
    Objects::Entity::GameEntity ent;
    ent->setId(character_id);
    ent->modifyPos()[0] = 100.0;
    ent->modifyPos()[1] = 100.0;
    ent->modifyPos()[2] = 0.0;
    ent->setLoc("world_0");
    Objects::Operation::Move m;
    m->setFrom(character_id);

    m->setArgs1((Root&)ent);

    send((Operation::RootOperation&)m);
}

void CyphesisClient::loop()
{
   while (1) {
#if !USE_SOCKET_PP
      fd_set infds;
      struct timeval tv;

      FD_ZERO(&infds);

      FD_SET(cli_fd, &infds);

      tv.tv_sec = 60;
      tv.tv_usec = 0;

      int retval = select(cli_fd+1, &infds, NULL, NULL, &tv);

      if (retval) {
          if (FD_ISSET(cli_fd, &infds)) {
              if (ios->peek() == -1) {
                  cout << "Server disconnected" << endl << flush;
                  exit(1);
              }
#endif
              codec->poll();
#if !USE_SOCKET_PP
          }
      }
#endif
   }
}

#ifdef _WIN32
int WINAPI WinMain( 
                   HINSTANCE hInstance, 	  // handle to current instance 
                   HINSTANCE hPrevInstance,  // handle to previous instance 
                   LPSTR lpCmdLine,	  // pointer to command line 
                   int nCmdShow		  // show state of window 
                   ) {
  main(0,NULL);
}
#endif

#ifdef DEBUG_DECODER
void CyphesisClient::objectArrived(const Message::Object& o)
{
    cout << "An object has arrived." << endl << flush;

    if (!o.isMap()) {
        cout << "I don't know what to do with it" << endl << flush;
        return;
    } else {
        cout << "I do know what to" << endl << flush;
    }
    const std::map<std::string, Message::Object> & omap = o.asMap();

    for (Message::Object::MapType::const_iterator I = omap.begin();
            I != o.asMap().end(); I++) {
        cout << I->first << " : " << (I->second.IsString() ? I->second.AsString() : std::string("NOT A STRING")) << endl << flush;
        if (I->second.isList()) {
            const Message::Object::ListType & alist = I->second.asList();
            for(Message::Object::ListType::const_iterator J = alist.begin(); J != alist.end(); J++) {
                cout << "-> " << (J->isString() ? J->asString() : std::string("NOT A STRING")) << endl << flush;
            }
        }
    }
 
}
#else

void CyphesisClient::unknownObjectArrived(const Message::Object& o)
{
#if 0 
    cout << "An unknown has arrived." << endl << flush;
    if (o.isMap()) {
        for(Message::Object::MapType::const_iterator I = o.AsMap().begin();
		I != o.AsMap().end();
		I++) {
		cout << I->first << endl << flush;
                if (I->second.IsString()) {
		    cout << I->second.AsString() << endl << flush;
                }
	}
    } else {
        cout << "Its not a map." << endl << flush;
    }
#endif
}

void CyphesisClient::objectArrived(const Operation::Info& o)
{
    reply_flag = 1;
    cout << "An info operation arrived." << endl << flush;
    const vector<Root>& args = o->getArgs();
    if (args.empty()) {
       cout << "args is empty list" << endl << flush;
       return;
    }
    if (state == CREATED_CHAR) {
        return;
    }
    if (state == INIT) {
#if 0
        const Message::Object & account = args.AsList().front();
        Objects::Entity::AccountInstance obj;
        for (Message::Object::MapType::const_iterator I = account.AsMap().begin();
            I != account.AsMap().end(); I++)
            obj->SetAttr(I->first, I->second);
        account_id = obj->GetAttr("id").AsString();
#endif
        account_id = args[0]->getId();
        state = LOGGED_IN;
    } else if (state == LOGGED_IN) {
        character_id = args[0]->getId();
        cout << "got char [" << character_id << "]" << endl << flush;
        state = CREATED_CHAR;
    }
}

void CyphesisClient::objectArrived(const Operation::Error& o)
{
    reply_flag = 1;
    erflag = 1;
    cout << "An error operation arrived." << endl << flush;
    if (o->getArgs().empty()) {
       cout << "args is empty list" << endl << flush;
       return;
    }
    const Root arg = o->getArgs()[0];
#if 0
    if (arg.IsString()) {
        cout << arg.AsString() << endl << flush;
    } else {
	cout << arg.GetType() << endl << flush;
    }
#endif
    if(arg->getParents().empty()) {
        cout << "no parents" << endl << flush;
    } else {
        cout << arg->getParents().front() << endl << flush;
    }
}

void CyphesisClient::objectArrived(const Operation::Sight& o)
{
    cout << "An sight operation arrived." << endl << flush;
    const vector<Root>& args = o->getArgs();
    if (args.empty()) {
       cout << "args is empty list" << endl << flush;
    }
    sdecode.processSight(args[0]);
}
#endif


void SightDecoder::processSight(const Root& o)
{
    dispatchObject(o);
}

void SightDecoder::unknownObjectArrived(const Root& o)
{
    cout << "An sight of an unknown operation arrived: ";
    if(o->getParents().empty()) cout << "No parents." << endl << flush;
    else cout << "parents[0] = " << o->getParents().front() << endl << flush;
}

void SightDecoder::objectArrived(const Operation::Create& o)
{
    cout << "An sight of an create operation arrived." << endl << flush;
}

void SightDecoder::objectArrived(const Operation::Move& o)
{
    cout << "An sight of an move operation arrived." << endl << flush;
}

void SightDecoder::objectArrived(const Operation::Set& o)
{
    cout << "An sight of an set operation arrived." << endl << flush;
}

