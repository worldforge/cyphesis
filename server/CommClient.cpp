// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "CommClient.h"
#include "CommServer.h"
#include "Connection.h"
#include "ServerRouting.h"

#include "common/log.h"
#include "common/debug.h"
#include "common/utility.h"
#include "common/stringstream.h"

#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Objects/Operation/Logout.h>
#include <Atlas/Objects/Operation/Create.h>
#include <Atlas/Objects/Operation/Imaginary.h>
#include <Atlas/Objects/Operation/Move.h>
#include <Atlas/Objects/Operation/Set.h>
#include <Atlas/Objects/Operation/Touch.h>
#include <Atlas/Objects/Operation/Look.h>
#include <Atlas/Objects/Operation/Talk.h>
#include <Atlas/Objects/Operation/Get.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Net/Stream.h>
#include <Atlas/Codec.h>

#include <iostream>
#include <stdexcept>

static const bool debug_flag = false;

CommClient::CommClient(CommServer & svr, int fd, Connection & c) :
            CommSocket(svr),
            clientIos(fd),
            codec(NULL), encoder(NULL),
            connection(c)
{
    clientIos.setTimeout(0,1000);
    commServer.server.incClients();
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
    commServer.server.decClients();
}

void CommClient::setup()
{
    debug( std::cout << "Negotiating started" << std::endl << std::flush; );
    // Create the server side negotiator
    accept =  new Atlas::Net::StreamAccept("cyphesis " + commServer.server.getName(), clientIos, this);

    accept->poll(false);

    clientIos << std::flush;
}

bool CommClient::negotiate()
{
    debug(std::cout << "Negotiating... " << std::flush;);
    // poll and check if negotiation is complete
    accept->poll();

    if (accept->getState() == Atlas::Net::StreamAccept::IN_PROGRESS) {
        return false;
    }
    debug(std::cout << "done" << std::endl;);

    // Check if negotiation failed
    if (accept->getState() == Atlas::Net::StreamAccept::FAILED) {
        log(NOTICE, "Failed to negotiate");
        return true;
    }
    // Negotiation was successful

    // Get the codec that negotiation established
    codec = accept->getCodec();

    // Create a new encoder to send high level objects to the codec
    encoder = new Atlas::Objects::Encoder(codec);

    // This should always be sent at the beginning of a session
    codec->streamBegin();

    // Acceptor is now finished with
    delete accept;
    accept = NULL;

    return false;
}

void CommClient::message(const RootOperation & op)
{
    OpVector reply = connection.message(op);
    for(OpVector::const_iterator I = reply.begin(); I != reply.end(); ++I) {
        debug(std::cout << "sending reply" << std::endl << std::flush;);
        send(**I);
        delete *I;
    }
}

template <class OpType>
void CommClient::queue(const OpType & op)
{
    OpType * nop = new OpType(op);
    opQueue.push_back(nop);
}

void CommClient::dispatch()
{
    DispatchQueue::const_iterator I = opQueue.begin();
    for(; I != opQueue.end(); ++I) {
        debug(std::cout << "dispatching op" << std::endl << std::flush;);
        message(**I);
        delete *I;
    }
    opQueue.clear();
}

void CommClient::unknownObjectArrived(const Atlas::Message::Element& o)
{
    debug(std::cout << "An unknown has arrived." << std::endl << std::flush;);
    RootOperation r;
    bool isOp = utility::Object_asOperation(o.asMap(), r);
    if (isOp) {
        queue(r);
    }
    if (debug_flag) {
        std::cout << "An unknown has arrived." << std::endl << std::flush;
        Atlas::Message::Element::MapType::const_iterator I;
        for(I = o.asMap().begin(); I != o.asMap().end(); I++) {
            std::cout << I->first << std::endl << std::flush;
            if (I->second.isString()) {
                std::cout << I->second.asString() << std::endl << std::flush;
            }
        }
    }
}

void CommClient::objectArrived(const Login & op)
{
    debug(std::cout << "A login operation thingy here!" << std::endl << std::flush;);
    queue(op);
}

void CommClient::objectArrived(const Logout & op)
{
    debug(std::cout << "A logout operation thingy here!" << std::endl << std::flush;);
    queue(op);
}

void CommClient::objectArrived(const Create & op)
{
    debug(std::cout << "A create operation thingy here!" << std::endl << std::flush;);
    queue(op);
}

void CommClient::objectArrived(const Imaginary & op)
{
    debug(std::cout << "A imaginary operation thingy here!" << std::endl << std::flush;);
    queue(op);
}

void CommClient::objectArrived(const Move & op)
{
    debug(std::cout << "A move operation thingy here!" << std::endl << std::flush;);
    queue(op);
}

void CommClient::objectArrived(const Set & op)
{
    debug(std::cout << "A set operation thingy here!" << std::endl << std::flush;);
    queue(op);
}

void CommClient::objectArrived(const Touch & op)
{
    debug(std::cout << "A touch operation thingy here!" << std::endl << std::flush;);
    queue(op);
}

void CommClient::objectArrived(const Look & op)
{
    debug(std::cout << "A look operation thingy here!" << std::endl << std::flush;);
    queue(op);
}

void CommClient::objectArrived(const Talk & op)
{
    debug(std::cout << "A talk operation thingy here!" << std::endl << std::flush;);
    queue(op);
}

void CommClient::objectArrived(const Get & op)
{
    debug(std::cout << "A get operation thingy here!" << std::endl << std::flush;);
    queue(op);
}

bool CommClient::read() {
    if (codec != NULL) {
        codec->poll();
        return false;
    } else {
        return negotiate();
    }
}

void CommClient::send(const Atlas::Objects::Operation::RootOperation & op)
{
    if (isOpen()) {
        encoder->streamMessage(&op);
        struct timeval tv = {0, 0};
        fd_set sfds;
        int cfd = clientIos.getSocket();
        FD_ZERO(&sfds);
        FD_SET(cfd, &sfds);
        if (select(++cfd, NULL, &sfds, NULL, &tv) > 0) {
            // We only flush to the client if the client is ready
            clientIos << std::flush;
        } else {
            debug(std::cout << "Client not ready" << std::endl << std::flush;);
        }
        // This timeout should only occur if the client was really not
        // ready
        if (clientIos.timeout()) {
            debug(std::cerr << "TIMEOUT" << std::endl << std::flush;);
            clientIos.close();
        }
    }
}
