// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch and Aloril

/* 
 * This client queries the Atlas inheritance tree of the server
 *
 * Al Riddoch
 * <ajr@ecs.soton.ac.uk>
 */

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

// Atlas negotiation
#include <Atlas/Codec.h>
#include <Atlas/Net/Stream.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Objects/Entity/Account.h>
#include <Atlas/Objects/Entity/GameEntity.h>
#include <Atlas/Objects/Operation/Login.h>

// The DebugBridge
#include "InheritanceClient.h"

#include <sys/time.h>
#include <unistd.h>
#include <math.h>

using Atlas::Objects::Operation::RootOperation;
using Atlas::Objects::Operation::Info;
using Atlas::Objects::Operation::Error;
using Atlas::Objects::Operation::Get;

using Atlas::Net::StreamConnect;

using Atlas::Message::Object;

int main(int argc, char ** argv)
{
  InheritanceClient bridge;
  if (bridge.connect() != 0) {
    cout << "Failed to connect" << endl;
    exit(1);
  }
  bridge.start();
  bridge.loop();
}

void InheritanceClient::send(RootOperation * op)
{
    encoder->StreamMessage(op);
    ios << flush;
}

void InheritanceClient::get(const std::string & id)
{
    Get g(Get::Instantiate());
    Object::MapType ent;
    ent["id"] = id;
    g.SetArgs(Object::ListType(1,ent));
    send(&g);
    cout << "sent get for " << id << endl << flush;
}

void InheritanceClient::start()
{
    get("root");
}

int InheritanceClient::connect()
{

  cout << "Connecting to cyphesis.." << endl << flush;

  ios.open("localhost", 6767);
  if (!ios.is_open()) {
    std::cerr << "ERROR: Could not connect to localhost"
              << std::endl << std::flush;
    return false;
  }

  cout << "Connected to cyphesis" << endl << flush;
  // Connect to the server

  // Do client negotiation with the server
  StreamConnect conn("cyphesis_client", ios, this);

  cout << "Negotiating... " << flush;
  // conn.Poll() does all the negotiation
  while (conn.GetState() == StreamConnect::IN_PROGRESS) {
    conn.Poll();
  }
  cout << "done" << endl;

  // Check whether negotiation was successful
  if (conn.GetState() == StreamConnect::FAILED) {
      cerr << "Failed to negotiate" << endl;
      exit(2);
  }
  // Negotiation was successful

  // Get the codec that negotiation established
  codec = conn.GetCodec();

  // This should always be sent at the beginning of a session

  encoder = new Atlas::Objects::Encoder(codec);

  codec->StreamBegin();
  return 0;

}

void InheritanceClient::loop()
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
              if (ios.peek() == -1) {
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

void InheritanceClient::UnknownObjectArrived(const Object & o)
{
    cout << "An unknown has arrived." << endl << flush;
#if 0 
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

void InheritanceClient::ObjectArrived(const Info& o)
{
    cout << "An info operation arrived." << endl << flush;
    const Object::ListType & args = o.GetArgs();
    if (!args.empty()) {
        const Object::MapType & definition = args.front().AsMap();
        Object::MapType::const_iterator I = definition.find("id");
        if (I == definition.end()) {
            std::cout << "response has no id" << std::endl << std::flush;
            return;
        }
        const std::string & id = I->second.AsString();
        I = definition.find("children");
        if (I == definition.end()) {
            std::cout << "response has no children" << std::endl << std::flush;
            return;
        }
        const Object::ListType & children = I->second.AsList();
        if (children.empty()) {
            std::cout << id << " has no children" << std::endl << std::flush;
            return;
        }
        std::cout << id << " has children [";
        Object::ListType::const_iterator J = children.begin();
        for (; J != children.end(); ++J) {
            const std::string & child = J->AsString();
            std::cout << child << " ";
            get(child);
        }
        std::cout << "]" << std::endl << std::flush;
    }
}

void InheritanceClient::ObjectArrived(const Error& o)
{
    cout << "An error operation arrived." << endl << flush;
    const Object::ListType & args = o.GetArgs();
}
