// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "CommClient.h"
#include "CommServer.h"

#include <iostream>

#include <common/debug.h>
#include <common/utility.h>

#include "Connection.h"

using Atlas::Message::Object;

static const bool debug_flag = false;

CommClient::CommClient(CommServer & svr, int fd, int port) :
            commServer(svr),
            clientFd(fd), clientIos(fd),
            codec(NULL), encoder(NULL),
            connection(*new Connection(*this))
{
}


CommClient::~CommClient()
{
    connection.destroy();
    delete &connection;
    if (encoder != NULL) {
        delete encoder;
    }
    if (codec != NULL) {
        delete codec;
    }
    close(clientFd);
}

void CommClient::setup()
{
    // Create the server side negotiator
    accept =  new Atlas::Net::StreamAccept("cyphesis " + commServer.identity, clientIos, this);

    accept->Poll(false);

    clientIos << flush;
}

bool CommClient::negotiate()
{
    debug(cout << "Negotiating... " << flush;);
    // Poll and check if negotiation is complete
    accept->Poll();

    if (accept->GetState() == Atlas::Net::StreamAccept::IN_PROGRESS) {
        return false;
    }
    debug(cout << "done" << endl;);

    // Check if negotiation failed
    if (accept->GetState() == Atlas::Net::StreamAccept::FAILED) {
        cerr << "Failed to negotiate" << endl;
        return true;
    }
    // Negotiation was successful

    // Get the codec that negotiation established
    codec = accept->GetCodec();

    // Create a new encoder to send high level objects to the codec
    encoder = new Atlas::Objects::Encoder(codec);

    // This should always be sent at the beginning of a session
    codec->StreamBegin();

    // Acceptor is now finished with
    delete accept;
    accept = NULL;

    return false;
}

void CommClient::message(const RootOperation & op)
{
    oplist reply = connection.message(op);
    for(oplist::const_iterator I = reply.begin(); I != reply.end(); I++) {
        debug(cout << "sending reply" << endl << flush;);
        send(*I);
        delete *I;
    }
}

void CommClient::UnknownObjectArrived(const Object& o)
{
    debug(cout << "An unknown has arrived." << endl << flush;);
    RootOperation * r = (RootOperation*)utility::Object_asRoot(o);
    if (r != NULL) {
        message(*r);
    }
    if (debug_flag) {
        debug(cout << "An unknown has arrived." << endl << flush;);
        if (o.IsMap()) {
            for(Object::MapType::const_iterator I = o.AsMap().begin();
		    I != o.AsMap().end();
		    I++) {
		    debug(cout << I->first << endl << flush;);
                    if (I->second.IsString()) {
		        debug(cout << I->second.AsString() << endl << flush;);
                    }
	    }
        } else {
            debug(cout << "Its not a map." << endl << flush;);
        }
    }
}

void CommClient::ObjectArrived(const Login & op)
{
    debug(cout << "A login operation thingy here!" << endl << flush;);
    message(op);
}

void CommClient::ObjectArrived(const Create & op)
{
    debug(cout << "A create operation thingy here!" << endl << flush;);
    message(op);
}

void CommClient::ObjectArrived(const Imaginary & op)
{
    debug(cout << "A imaginary operation thingy here!" << endl << flush;);
    message(op);
}

void CommClient::ObjectArrived(const Move & op)
{
    debug(cout << "A move operation thingy here!" << endl << flush;);
    message(op);
}

void CommClient::ObjectArrived(const Set & op)
{
    debug(cout << "A set operation thingy here!" << endl << flush;);
    message(op);
}

void CommClient::ObjectArrived(const Touch & op)
{
    debug(cout << "A touch operation thingy here!" << endl << flush;);
    message(op);
}

void CommClient::ObjectArrived(const Look & op)
{
    debug(cout << "A look operation thingy here!" << endl << flush;);
    message(op);
}

void CommClient::ObjectArrived(const Talk & op)
{
    debug(cout << "A talk operation thingy here!" << endl << flush;);
    message(op);
}

void CommClient::ObjectArrived(const Get & op)
{
    debug(cout << "A get operation thingy here!" << endl << flush;);
    message(op);
}
