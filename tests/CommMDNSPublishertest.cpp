// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2011 Alistair Riddoch
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

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "server/CommMDNSPublisher.h"
#include "server/CommServer.h"
#include "server/ServerRouting.h"

#include <cassert>

int main()
{
#ifndef HAVE_AVAHI
}
#else // HAVE_AVAHI
    ServerRouting test_server(*(BaseWorld*)0, "", "", "1", 1, "2", 2);
    CommServer test_comm_server;

    {

    }

    return 0;
}

// Stub functions

#include "common/log.h"

ServerRouting::ServerRouting(BaseWorld & wrld,
                             const std::string & ruleset,
                             const std::string & name,
                             const std::string & id, long intId,
                             const std::string & lId, long lIntId) :
        Router(id, intId),
        m_svrRuleset(ruleset), m_svrName(name),
        m_numClients(0), m_world(wrld), m_lobby(*(Lobby*)0)
{
}

ServerRouting::~ServerRouting()
{
}

void ServerRouting::addToMessage(Atlas::Message::MapType & omap) const
{
}

void ServerRouting::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
}

Router::Router(const std::string & id, long intId) : m_id(id),
                                                     m_intId(intId)
{
}

Router::~Router()
{
}

void Router::addToMessage(Atlas::Message::MapType & omap) const
{
}

void Router::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
}

CommServer::CommServer() : m_congested(false)
{
}

CommServer::~CommServer()
{
}

CommSocket::CommSocket(CommServer & svr) : m_commServer(svr) { }

CommSocket::~CommSocket()
{
}

Idle::Idle(CommServer & svr) : m_idleManager(svr)
{
}

Idle::~Idle()
{
}

void log(LogLevel lvl, const std::string & msg)
{
}

namespace consts {

  const char * buildTime = __TIME__;
  const char * buildDate = __DATE__;
  const int buildId = -1;
  const char * version = "test_version";
}

int timeoffset = 0;
int client_port_num = 6767;

#include <avahi-common/error.h>
#include <avahi-client/client.h>
#include <avahi-client/publish.h>

AvahiStringList *avahi_string_list_new(const char *txt, ...)
{
    return 0;
}

void avahi_string_list_free(AvahiStringList *l)
{
}

const char *avahi_strerror(int error)
{
    static const char error_message[] = "test_stub_error";
    return &error_message[0];
}

AvahiClient * avahi_client_new(const AvahiPoll *poll_api,
                               AvahiClientFlags flags,
                               AvahiClientCallback callback,
                               void *userdata,
                               int *ret_error)
{
    static char dummy_object[sizeof(int)];

    // The object is opaque, so it probably doesn't even need to be non-null
    return (AvahiClient*)&dummy_object[0];
}

int avahi_entry_group_add_service_strlst(AvahiEntryGroup *group,
                                         AvahiIfIndex interface,
                                         AvahiProtocol protocol,
                                         AvahiPublishFlags flags,
                                         const char *name,
                                         const char *type,
                                         const char *domain,
                                         const char *host,
                                         uint16_t port,
                                         AvahiStringList *txt)
{
    return 0;
}

int avahi_entry_group_commit(AvahiEntryGroup*)
{
    return 0;
}

AvahiEntryGroup * avahi_entry_group_new(AvahiClient* c,
                                        AvahiEntryGroupCallback callback,
                                        void *userdata)
{
    static char dummy_object[sizeof(int)];

    // The object is opaque, so it probably doesn't even need to be non-null
    return (AvahiEntryGroup*)&dummy_object[0];
}

int avahi_client_errno(AvahiClient *)
{
    return 0;
}
#endif // HAVE_AVAHI
