// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "CommClient.h"
#include "CommServer.h"
#include "Connection.h"

#include <common/log.h>
#include <common/debug.h>
#include <common/utility.h>
#include <common/stringstream.h>

#include <iostream>

static const bool debug_flag = false;

CommClient::CommClient(CommServer & svr, int fd, int port) :
            commServer(svr),
            clientIos(fd),
            codec(NULL), encoder(NULL),
            connection(*new Connection(*this)),
            reading(false)
{
    char ipno[255];
    sockaddr_in client = clientIos.getOutpeer();
    std::stringstream connectionId;
    if (!inet_ntop(AF_INET, &client.sin_addr, ipno, 255)) {
        connectionId << "UNKNOWN";
    } else {
        connectionId << ipno;
    }
    connectionId << ":" << client.sin_port;
    connection.setId(connectionId.str());
    clientIos.setTimeout(0,1000);
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
        log(NOTICE, "Failed to negotiate");
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
        send(**I);
        delete *I;
    }
}

void CommClient::UnknownObjectArrived(const Atlas::Message::Object& o)
{
    debug(std::cout << "An unknown has arrived." << std::endl << std::flush;);
    RootOperation r;
    bool isOp = utility::Object_asOperation(o.AsMap(), r);
    if (isOp) {
        message(r);
    }
    if (debug_flag) {
        std::cout << "An unknown has arrived." << std::endl << std::flush;
        Atlas::Message::Object::MapType::const_iterator I;
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

void CommClient::ObjectArrived(const Logout & op)
{
    debug(std::cout << "A logout operation thingy here!" << std::endl << std::flush;);
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

bool CommClient::read() {
    if (codec != NULL) {
        reading = true;
        codec->Poll();
        reading = false;
        return false;
    } else {
        return negotiate();
    }
}


void CommClient::send(const Atlas::Objects::Operation::RootOperation & op)
{
    if (isOpen()) {
        encoder->StreamMessage(&op);
        struct timeval tv = {0, 0};
        fd_set sfds;
        int cfd = clientIos.getSocket();
        FD_ZERO(&sfds);
        FD_SET(cfd, &sfds);
        if (select(++cfd, NULL, &sfds, NULL, &tv) > 0) {
            // We only flush to the client if the client is ready
            std::cout << "Client is ready for data" << std::endl << std::flush;
            clientIos << std::flush;
        } else {
            std::cout << "Client isn't ready" << std::endl << std::flush;
        }
        // This timeout should only occur if the client was really not
        // ready
        if (clientIos.timeout()) {
            if (reading) {
                std::cerr << "TIMEOUT while readind" << std::endl << std::flush;
                throw ClientTimeOutException();
            } else {
                std::cerr << "TIMEOUT" << std::endl << std::flush;
                clientIos.close();
            }
        }
    }
}
