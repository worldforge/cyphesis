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
#include <sstream>
#include <stdexcept>

using Atlas::Message::MapType;

static const bool debug_flag = false;

CommClient::CommClient(CommServer & svr, int fd, Connection & c) :
            CommSocket(svr),
            m_clientIos(fd),
            m_codec(NULL), m_encoder(NULL),
            m_connection(c)
{
    m_clientIos.setTimeout(0,1000);
    m_commServer.m_server.incClients();
}

CommClient::~CommClient()
{
    m_connection.destroy();
    delete &m_connection;
    if (m_accept != NULL) {
        delete m_accept;
    }
    if (m_encoder != NULL) {
        delete m_encoder;
    }
    if (m_codec != NULL) {
        delete m_codec;
    }
    m_clientIos.close();
    m_commServer.m_server.decClients();
}

void CommClient::setup()
{
    debug( std::cout << "Negotiating started" << std::endl << std::flush; );
    // Create the server side negotiator
    m_accept =  new Atlas::Net::StreamAccept("cyphesis " + m_commServer.m_server.getName(), m_clientIos, this);

    m_accept->poll(false);

    m_clientIos << std::flush;
}

int CommClient::negotiate()
{
    debug(std::cout << "Negotiating... " << std::flush;);
    // poll and check if negotiation is complete
    m_accept->poll();

    if (m_accept->getState() == Atlas::Net::StreamAccept::IN_PROGRESS) {
        return 0;
    }
    debug(std::cout << "done" << std::endl;);

    // Check if negotiation failed
    if (m_accept->getState() == Atlas::Net::StreamAccept::FAILED) {
        log(NOTICE, "Failed to negotiate");
        return -1;
    }
    // Negotiation was successful

    // Get the codec that negotiation established
    m_codec = m_accept->getCodec();

    // Create a new encoder to send high level objects to the codec
    m_encoder = new Atlas::Objects::Encoder(m_codec);

    // This should always be sent at the beginning of a session
    m_codec->streamBegin();

    // Acceptor is now finished with
    delete m_accept;
    m_accept = NULL;

    return 0;
}

void CommClient::message(const RootOperation & op)
{
    OpVector reply;
    m_connection.operation(op, reply);
    OpVector::const_iterator Iend = reply.end();
    for(OpVector::const_iterator I = reply.begin(); I != Iend; ++I) {
        debug(std::cout << "sending reply" << std::endl << std::flush;);
        send(**I);
        delete *I;
    }
}

template <class OpType>
void CommClient::queue(const OpType & op)
{
    OpType * nop = new OpType(op);
    m_opQueue.push_back(nop);
}

void CommClient::dispatch()
{
    DispatchQueue::const_iterator Iend = m_opQueue.end();
    for(DispatchQueue::const_iterator I = m_opQueue.begin(); I != Iend; ++I) {
        debug(std::cout << "dispatching op" << std::endl << std::flush;);
        message(**I);
        delete *I;
    }
    m_opQueue.clear();
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
        log(ERROR, "An unknown object has arrived from a client.");
        MapType::const_iterator Iend = o.asMap().end();
        for(MapType::const_iterator I = o.asMap().begin(); I != Iend; ++I) {
            std::cerr << I->first << std::endl << std::flush;
            if (I->second.isString()) {
                std::cerr << I->second.asString() << std::endl << std::flush;
            }
        }
    }
}

void CommClient::objectArrived(const Atlas::Objects::Operation::Login & op)
{
    debug(std::cout << "A login operation thingy here!" << std::endl << std::flush;);
    queue(op);
}

void CommClient::objectArrived(const Atlas::Objects::Operation::Logout & op)
{
    debug(std::cout << "A logout operation thingy here!" << std::endl << std::flush;);
    queue(op);
}

void CommClient::objectArrived(const Atlas::Objects::Operation::Create & op)
{
    debug(std::cout << "A create operation thingy here!" << std::endl << std::flush;);
    queue(op);
}

void CommClient::objectArrived(const Atlas::Objects::Operation::Imaginary & op)
{
    debug(std::cout << "A imaginary operation thingy here!" << std::endl << std::flush;);
    queue(op);
}

void CommClient::objectArrived(const Atlas::Objects::Operation::Move & op)
{
    debug(std::cout << "A move operation thingy here!" << std::endl << std::flush;);
    queue(op);
}

void CommClient::objectArrived(const Atlas::Objects::Operation::Set & op)
{
    debug(std::cout << "A set operation thingy here!" << std::endl << std::flush;);
    queue(op);
}

void CommClient::objectArrived(const Atlas::Objects::Operation::Touch & op)
{
    debug(std::cout << "A touch operation thingy here!" << std::endl << std::flush;);
    queue(op);
}

void CommClient::objectArrived(const Atlas::Objects::Operation::Look & op)
{
    debug(std::cout << "A look operation thingy here!" << std::endl << std::flush;);
    queue(op);
}

void CommClient::objectArrived(const Atlas::Objects::Operation::Talk & op)
{
    debug(std::cout << "A talk operation thingy here!" << std::endl << std::flush;);
    queue(op);
}

void CommClient::objectArrived(const Atlas::Objects::Operation::Get & op)
{
    debug(std::cout << "A get operation thingy here!" << std::endl << std::flush;);
    queue(op);
}

int CommClient::read()
{
    if (m_codec != NULL) {
        m_codec->poll();
        return 0;
    } else {
        return negotiate();
    }
}

int CommClient::getFd() const
{
    return m_clientIos.getSocket();
}

bool CommClient::isOpen() const
{
    return m_clientIos.is_open();
}

bool CommClient::eof()
{
    return m_clientIos.peek() == EOF;
}

void CommClient::send(const Atlas::Objects::Operation::RootOperation & op)
{
    if (isOpen()) {
        m_encoder->streamMessage(&op);
        struct timeval tv = {0, 0};
        fd_set sfds;
        int cfd = m_clientIos.getSocket();
        FD_ZERO(&sfds);
        FD_SET(cfd, &sfds);
        if (select(++cfd, NULL, &sfds, NULL, &tv) > 0) {
            // We only flush to the client if the client is ready
            m_clientIos << std::flush;
        } else {
            debug(std::cout << "Client not ready" << std::endl << std::flush;);
        }
        // This timeout should only occur if the client was really not
        // ready
        if (m_clientIos.timeout()) {
            debug(std::cerr << "TIMEOUT" << std::endl << std::flush;);
            m_clientIos.shutdown();
        }
    }
}
