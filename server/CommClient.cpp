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
            clientIos(fd),
            codec(NULL), encoder(NULL),
            connection(*new Connection(*this))
{
    clientIos.setTimeout(0,1);
}


CommClient::~CommClient()
{
    connection.destroy();
    delete &connection;
    if (accept != NULL) {
        delete accept;
    }
    if (encoder != NULL) {
        delete encoder;
    }
    if (codec != NULL) {
        delete codec;
    }
    clientIos.close();
}

void CommClient::setup()
{
    debug( std::cout << "Negotiating started" << std::endl << std::flush; );
    // Create the server side negotiator
    accept =  new Atlas::Net::StreamAccept("cyphesis " + commServer.identity, clientIos, this);

    accept->Poll(false);

    clientIos << std::flush;
}

bool CommClient::negotiate()
{
    debug(std::cout << "Negotiating... " << std::flush;);
    // Poll and check if negotiation is complete
    accept->Poll();

    if (accept->GetState() == Atlas::Net::StreamAccept::IN_PROGRESS) {
        return false;
    }
    debug(std::cout << "done" << std::endl;);

    // Check if negotiation failed
    if (accept->GetState() == Atlas::Net::StreamAccept::FAILED) {
        std::cerr << "Failed to negotiate" << std::endl;
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
    OpVector reply = connection.message(op);
    for(OpVector::const_iterator I = reply.begin(); I != reply.end(); I++) {
        debug(std::cout << "sending reply" << std::endl << std::flush;);
        send(*I);
        delete *I;
    }
}

void CommClient::UnknownObjectArrived(const Object& o)
{
    debug(std::cout << "An unknown has arrived." << std::endl << std::flush;);
    RootOperation r;
    bool isOp = utility::Object_asOperation(o.AsMap(), r);
    if (isOp) {
        message(r);
    }
    if (debug_flag) {
        std::cout << "An unknown has arrived." << std::endl << std::flush;
        Object::MapType::const_iterator I;
        for(I = o.AsMap().begin(); I != o.AsMap().end(); I++) {
            std::cout << I->first << std::endl << std::flush;
            if (I->second.IsString()) {
                std::cout << I->second.AsString() << std::endl << std::flush;
            }
        }
    }
}

void CommClient::ObjectArrived(const Login & op)
{
    debug(std::cout << "A login operation thingy here!" << std::endl << std::flush;);
    message(op);
}

void CommClient::ObjectArrived(const Create & op)
{
    debug(std::cout << "A create operation thingy here!" << std::endl << std::flush;);
    message(op);
}

void CommClient::ObjectArrived(const Imaginary & op)
{
    debug(std::cout << "A imaginary operation thingy here!" << std::endl << std::flush;);
    message(op);
}

void CommClient::ObjectArrived(const Move & op)
{
    debug(std::cout << "A move operation thingy here!" << std::endl << std::flush;);
    message(op);
}

void CommClient::ObjectArrived(const Set & op)
{
    debug(std::cout << "A set operation thingy here!" << std::endl << std::flush;);
    message(op);
}

void CommClient::ObjectArrived(const Touch & op)
{
    debug(std::cout << "A touch operation thingy here!" << std::endl << std::flush;);
    message(op);
}

void CommClient::ObjectArrived(const Look & op)
{
    debug(std::cout << "A look operation thingy here!" << std::endl << std::flush;);
    message(op);
}

void CommClient::ObjectArrived(const Talk & op)
{
    debug(std::cout << "A talk operation thingy here!" << std::endl << std::flush;);
    message(op);
}

void CommClient::ObjectArrived(const Get & op)
{
    debug(std::cout << "A get operation thingy here!" << std::endl << std::flush;);
    message(op);
}
