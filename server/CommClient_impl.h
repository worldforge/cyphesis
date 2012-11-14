// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

// $Id$

#ifndef SERVER_COMM_CLIENT_IMPL_H
#define SERVER_COMM_CLIENT_IMPL_H

#include "CommClient.h"
#include "CommServer.h"

#include "server/Link.h"

#include "common/log.h"
#include "common/compose.hpp"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Net/Stream.h>
#include <Atlas/Codec.h>

template <class StreamT>
CommClient<StreamT>::CommClient(CommServer & svr,
                                const std::string & name,
                                int fd) :
            CommStreamClient<StreamT>(svr, fd), Idle(svr),
            m_codec(NULL), m_encoder(NULL), m_link(NULL),
            m_connectTime(svr.time())
{
    this->m_clientIos.setTimeout(0,1000);

    m_negotiate = new Atlas::Net::StreamAccept("cyphesis " + name,
                                               this->m_clientIos);
}

template <class StreamT>
CommClient<StreamT>::CommClient(CommServer & svr,
                                const std::string & name) :
            CommStreamClient<StreamT>(svr), Idle(svr),
            m_codec(NULL), m_encoder(NULL), m_link(NULL),
            m_connectTime(svr.time())
{
    this->m_clientIos.setTimeout(0,1000);

    m_negotiate = new Atlas::Net::StreamConnect("cyphesis " + name,
                                                this->m_clientIos);
}

template <class StreamT>
CommClient<StreamT>::~CommClient()
{
    delete this->m_link;
    delete this->m_negotiate;
    delete this->m_encoder;
    delete this->m_codec;
}

template <class StreamT>
void CommClient<StreamT>::setup(Link * connection)
{
    // Create the server side negotiator

    this->m_link = connection;

    this->m_negotiate->poll(false);

    this->m_clientIos << std::flush;
}

template <class StreamT>
int CommClient<StreamT>::negotiate()
{
    // poll and check if negotiation is complete
    this->m_negotiate->poll();

    if (this->m_negotiate->getState() == Atlas::Negotiate::IN_PROGRESS) {
        return 0;
    }

    // Check if negotiation failed
    if (this->m_negotiate->getState() == Atlas::Negotiate::FAILED) {
        log(NOTICE, "Failed to negotiate");
        return -1;
    }
    // Negotiation was successful

    // Get the codec that negotiation established
    this->m_codec = this->m_negotiate->getCodec(*this);

    // Create a new encoder to send high level objects to the codec
    this->m_encoder = new Atlas::Objects::ObjectsEncoder(*this->m_codec);

    assert(this->m_link != 0);
    this->m_link->setEncoder(this->m_encoder);

    // This should always be sent at the beginning of a session
    this->m_codec->streamBegin();

    // Acceptor is now finished with
    delete this->m_negotiate;
    this->m_negotiate = 0;

    return 0;
}

template <class StreamT>
int CommClient<StreamT>::operation(const Atlas::Objects::Operation::RootOperation & op)
{
    assert(m_link != 0);
    m_link->externalOperation(op);
    return 0;
}

template <class StreamT>
void CommClient<StreamT>::dispatch()
{
    DispatchQueue::const_iterator Iend = m_opQueue.end();
    for(DispatchQueue::const_iterator I = m_opQueue.begin(); I != Iend; ++I) {
        if (operation(*I) != 0) {
            return;
        }
    }
    m_opQueue.clear();
}

template <class StreamT>
void CommClient<StreamT>::objectArrived(const Atlas::Objects::Root & obj)
{
    Atlas::Objects::Operation::RootOperation op = Atlas::Objects::smart_dynamic_cast<Atlas::Objects::Operation::RootOperation>(obj);
    if (!op.isValid()) {
        const std::list<std::string> & parents = obj->getParents();
        if (parents.empty()) {
            log(ERROR, String::compose("Object of type \"%1\" with no parent "
                                       "arrived from client",
                                       obj->getObjtype()));
        } else {
            log(ERROR, String::compose("Object of type \"%1\" with parent "
                                       "\"%2\" arrived from client",
                                       obj->getObjtype(),
                                       obj->getParents().front()));
        }
        return;
    }
    m_opQueue.push_back(op);
}

template <class StreamT>
void CommClient<StreamT>::idle(time_t t)
{
    if (m_negotiate != 0) {
        if ((t - m_connectTime) > 10) {
            log(NOTICE, "Client disconnected because of negotiation timeout.");
            this->m_clientIos.shutdown();
        }
    }
}

template <class StreamT>
int CommClient<StreamT>::read()
{
    if (m_codec != NULL) {
        m_codec->poll();
        return 0;
    } else {
        return negotiate();
    }
}

template <class StreamT>
int CommClient<StreamT>::send(const Atlas::Objects::Operation::RootOperation & op)
{
    if (!this->isOpen()) {
        log(ERROR, "Writing to closed client");
        return -1;
    }
    if (this->m_clientIos.fail()) {
        return -1;
    }
    if (this->m_encoder == 0) {
        log(ERROR, "Encoder not initialized");
        return -1;
    }
    m_encoder->streamObjectsMessage(op);
    return this->flush();
}

#endif // SERVER_COMM_CLIENT_IMPL_H
