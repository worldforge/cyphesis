// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

/*
CyphesisClient.cpp
------------------
begin           : 1999.12.18
copyright       : (C) 1999 by Aloril
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
#include <Atlas/Objects/Entity/Account.h>
#include <Atlas/Objects/Entity/GameEntity.h>
#include <Atlas/Objects/Operation/Login.h>
// The DebugBridge
#include "CyphesisClient.h"


#ifdef _WIN32
#define sleep(x) Sleep(x*1000)
#define usleep(x) Sleep(x/1000)
#else
#include <sys/time.h>
#include <unistd.h>
#endif
#include <math.h>

string defMessage="No message was given by server!";
string emptyString="";

using namespace Atlas;
using namespace std;

int main(int argc, char ** argv)
{
  CyphesisClient bridge;
  if (bridge.connect() != 0) {
    cout << "Failed to connect" << endl;
    exit(1);
  }
  bridge.login();
  bridge.create_char();
  bridge.look();
  bridge.move();
  bridge.loop();
}

void CyphesisClient::send(Objects::Operation::RootOperation * op)
{
    encoder->StreamMessage(op);
    *ios << flush;
}

void CyphesisClient::login()
{
   Objects::Entity::Account account = Objects::Entity::Account::Instantiate();
   Objects::Operation::Login l = Objects::Operation::Login::Instantiate();
   erflag = 0;
   reply_flag = 0;

   account.SetAttr("id", string("al"));
   account.SetAttr("password", string("ping"));

   Message::Object::ListType args(1,account.AsObject());

   l.SetArgs(args);

   encoder->StreamMessage(&l);

   while (!reply_flag) {
      codec->Poll();
   }
   if (!erflag) {
      cout << "login was a success" << endl << flush;
      return;
   }
   cout << "login failed" << endl << flush;

   Objects::Operation::Create c = Objects::Operation::Create::Instantiate();
   erflag = 0;
   reply_flag = 0;

   c.SetAttr("args", Message::Object(args));

   encoder->StreamMessage(&c);

   while (!reply_flag) {
      codec->Poll();
   }
   if (!erflag) {
      cout << "create account was a success" << endl << flush;
      return;
   }
}

int CyphesisClient::connect()
{

  struct sockaddr_in sin;

  cli_fd = socket(PF_INET, SOCK_STREAM, 0);
  if (cli_fd < 0) {
    return 1;
  }
  sin.sin_family = AF_INET;
  sin.sin_port = htons(6767);
  sin.sin_addr.s_addr = htonl(0x7f000001);

  cout << "Connecting to cyphesis.." << endl << flush;

  if (::connect(cli_fd, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
    close(cli_fd);
    return -1;
  }
  cout << "Connected to cyphesis" << endl << flush;
  // Connect to the server
  ios = new fstream(cli_fd);

  // Do client negotiation with the server
  Net::StreamConnect conn("cyphesis_client", *ios, this);

  cout << "Negotiating... " << flush;
  // conn.Poll() does all the negotiation
  while (conn.GetState() == Negotiate<iostream>::IN_PROGRESS) {
    conn.Poll();
  }
  cout << "done" << endl;

  // Check whether negotiation was successful
  if (conn.GetState() == Negotiate<iostream>::FAILED) {
      cerr << "Failed to negotiate" << endl;
      exit(2);
  }
  // Negotiation was successful

  // Get the codec that negotiation established
  codec = conn.GetCodec();

  // This should always be sent at the beginning of a session

  encoder = new Objects::Encoder(codec);

  codec->StreamBegin();
  return 0;

}

void CyphesisClient::create_char()
{
   Objects::Entity::GameEntity character = Objects::Entity::GameEntity::Instantiate();
   Objects::Operation::Create c = Objects::Operation::Create::Instantiate();
   erflag = 0;
   reply_flag = 0;

   Message::Object::ListType parlist(1,string("farmer"));
   character.SetAttr("parents", Message::Object(parlist));
   character.SetAttr("name", string("Al Riddoch"));

   Message::Object::ListType args(1,character.AsObject());

   c.SetAttr("args", Message::Object(args));
   c.SetAttr("from", account_id);
   
   encoder->StreamMessage(&c);
   while (!reply_flag) {
      codec->Poll();
   }
   if (!erflag) {
       cout << "Created character: " << character_id << " :using account: " << account_id << " :" << endl << flush;
   }
}

void CyphesisClient::look()
{
   Objects::Operation::Look l = Objects::Operation::Look::Instantiate();
   l.SetAttr("from", character_id);

   encoder->StreamMessage(&l);
   *ios << flush;
}

void CyphesisClient::move()
{
    Message::Object::MapType ent;
    ent["id"] = character_id;
    Message::Object::ListType coords;
    coords.push_back(100.0);
    coords.push_back(100.0);
    coords.push_back(0.0);
    ent["pos"] = coords;
    ent["loc"] = string("world_0");
    Objects::Operation::Move m = Objects::Operation::Move::Instantiate();
    m.SetAttr("from", character_id);

    m.SetArgs(Message::Object::ListType(1,ent));

    encoder->StreamMessage(&m);
    *ios << flush;
}

void CyphesisClient::loop()
{
   while (1) {
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
              codec->Poll();
          }
      }
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
void CyphesisClient::ObjectArrived(const Message::Object& o)
{
    cout << "An object has arrived." << endl << flush;

    if (!o.IsMap()) {
        cout << "I don't know what to do with it" << endl << flush;
        return;
    } else {
        cout << "I do know what to" << endl << flush;
    }
    const std::map<std::string, Message::Object> & omap = o.AsMap();

    for (Message::Object::MapType::const_iterator I = omap.begin();
            I != o.AsMap().end(); I++) {
        cout << I->first << " : " << (I->second.IsString() ? I->second.AsString() : string("NOT A STRING")) << endl << flush;
        if (I->second.IsList()) {
            const Message::Object::ListType & alist = I->second.AsList();
            for(Message::Object::ListType::const_iterator J = alist.begin(); J != alist.end(); J++) {
                cout << "-> " << (J->IsString() ? J->AsString() : string("NOT A STRING")) << endl << flush;
            }
        }
    }
 
}
#else

void CyphesisClient::UnknownObjectArrived(const Message::Object& o)
{
#if 0 
    cout << "An unknown has arrived." << endl << flush;
    if (o.IsMap()) {
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

void CyphesisClient::ObjectArrived(const Operation::Info& o)
{
    reply_flag = 1;
    cout << "An info operation arrived." << endl << flush;
    const Message::Object & args = o.GetAttr("args");
    if (!args.IsList()) {
       cout << "args not list" << endl << flush;
    }
    if (state == CREATED_CHAR) {
        return;
    }
    if (state == INIT) {
        Message::Object account = args.AsList().front();
        Objects::Entity::Account obj = Objects::Entity::Account::Instantiate();
        for (Message::Object::MapType::const_iterator I = account.AsMap().begin();
            I != account.AsMap().end(); I++)
            obj.SetAttr(I->first, I->second);
        account_id = obj.GetAttr("id").AsString();
        state = LOGGED_IN;
        Message::Object::MapType & ac_map = account.AsMap();
        if (ac_map.find("characters") != ac_map.end()) {
            const Message::Object::ListType & chars = ac_map["characters"].AsList();
            cout << "Got " << chars.size() << " characters:" << endl << flush;
            for(Message::Object::ListType::const_iterator I = chars.begin();
                        I != chars.end(); I++) {
                cout << "Character " << I->AsString() << endl << flush;
            }
        }
    } else if (state == LOGGED_IN) {
        Message::Object::MapType character = args.AsList().front().AsMap();
        character_id = character["id"].AsString();
        cout << "got char [" << character_id << "]" << endl << flush;
        state = CREATED_CHAR;
    }
}

void CyphesisClient::ObjectArrived(const Operation::Error& o)
{
    reply_flag = 1;
    erflag = 1;
    cout << "An error operation arrived." << endl << flush;
    Message::Object::ListType args = o.GetArgs();
    Message::Object & arg = args.front();
    if (arg.IsString()) {
        cout << arg.AsString() << endl << flush;
    } else {
	cout << arg.GetType() << endl << flush;
    } 
}

void CyphesisClient::ObjectArrived(const Operation::Sight& o)
{
    cout << "An sight operation arrived." << endl << flush;
    const Message::Object & args = o.GetAttr("args");
    if (!args.IsList()) {
       cout << "args not list" << endl << flush;
    }
    const Message::Object & objseen = args.AsList().front();
    sdecode.processSight(objseen);
}
#endif


void SightDecoder::processSight(const Message::Object& o)
{
    Objects::Decoder::ObjectArrived(o);
}

void SightDecoder::UnknownObjectArrived(const Message::Object& o)
{
    cout << "An sight of an unknown operation arrived." << endl << flush;
}

void SightDecoder::ObjectArrived(const Operation::Create& o)
{
    cout << "An sight of an create operation arrived." << endl << flush;
}

void SightDecoder::ObjectArrived(const Operation::Move& o)
{
    cout << "An sight of an move operation arrived." << endl << flush;
}

void SightDecoder::ObjectArrived(const Operation::Set& o)
{
    cout << "An sight of an set operation arrived." << endl << flush;
}

