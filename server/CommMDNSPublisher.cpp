// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2004-2011 Alistair Riddoch
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


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#undef PACKAGE
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_VERSION
#undef PACKAGE_BUGREPORT
#undef VERSION

#include "CommMDNSPublisher.h"

#include "CommServer.h"
#include "ServerRouting.h"

#include "common/log.h"
#include "common/const.h"
#include "common/debug.h"
#include "common/BaseWorld.h"
#include "common/compose.hpp"
#include "common/sockets.h"

#include <iostream>

#if defined(HAVE_AVAHI)

#include <avahi-client/client.h>
#include <avahi-client/publish.h>

#include <avahi-common/error.h>

#include <cassert>

static const bool debug_flag = false;

static void client_callback(AvahiClient * s,
                            AvahiClientState state,
                            void * userdata)
{
    CommMDNSPublisher * cmp = static_cast<CommMDNSPublisher*>(userdata);

    switch (state) {
        case AVAHI_CLIENT_S_RUNNING:
            // Check we have not already started them
            if (cmp->m_group == 0) {
                cmp->setup_service(s);
            }
            break;

        case AVAHI_CLIENT_S_COLLISION:
            log(WARNING, "Name collision while publishing using avahi MDNS");
            break;

        case AVAHI_CLIENT_FAILURE:
            log(WARNING, "Failure while publishing using avahi MDNS");
            break;

        case AVAHI_CLIENT_CONNECTING:
            log(NOTICE, "Avahi returned connecting");
            break;

        case AVAHI_CLIENT_S_REGISTERING:
            log(WARNING, "Avahi registering");
            break;

        default:
            log(WARNING, "Unknown state");
            break;
    }
}

static void group_callback(AvahiEntryGroup * g,
                           AvahiEntryGroupState state,
                           void * userdata)
{
     switch (state) {
         case AVAHI_ENTRY_GROUP_ESTABLISHED :
             /* The entry group has been established successfully */

             break;
 
         case AVAHI_ENTRY_GROUP_COLLISION : {
             log(NOTICE, "Avahi callback reported group collision");
             
             /* A service name collision happened. Let's pick a new name */
             // char * n = avahi_alternative_service_name(name);
             // avahi_free(name);
             // name = n;
             
             // fprintf(stderr, "Service name collision, renaming service to '%s'\n", name);
             
             /* And recreate the services */
             // create_services(avahi_entry_group_get_client(g));
             break;
         }
 
         case AVAHI_ENTRY_GROUP_FAILURE :
             log(NOTICE, "Avahi callback reported group failure");
 
             /* Some kind of failure happened while we were registering our services */
             // avahi_simple_poll_quit(simple_poll);
             break;
 
         case AVAHI_ENTRY_GROUP_UNCOMMITED:
         case AVAHI_ENTRY_GROUP_REGISTERING:
             // This will happen when the group is created or registering,
             // and is not noteworthy
             break;
     }
}

struct AvahiWatch {
    CommMDNSPublisher * m_publisher;
    AvahiWatchEvent m_requiredEvent;
    AvahiWatchCallback m_callback;
    AvahiWatchEvent m_events;
    void * m_userdata;
};

static AvahiWatch* watch_new(const AvahiPoll *api,
                             int fd,
                             AvahiWatchEvent event,
                             AvahiWatchCallback callback,
                             void *userdata)
{
    debug(std::cout << "avahi_watch_new " << fd << " " << callback
                    << std::endl << std::flush;);
    CommMDNSPublisher * cmp = static_cast<CommMDNSPublisher*>(api->userdata);
    if (cmp->getFd() != -1) {
        log(ERROR, "Avahi asked for multiple fds. Unable to comply.");
    } else {
        cmp->m_avahiFd = fd;
    }

    if (~event & AVAHI_WATCH_IN) {
        log(ERROR, "Avahi watcher does not require read events.");
    }
    if (event & ~AVAHI_WATCH_IN) {
        log(WARNING, "Avahi watcher requires unsupported events.");
    }

    AvahiWatch * aw = new AvahiWatch;
    aw->m_publisher = cmp;
    aw->m_requiredEvent = event;
    aw->m_callback = callback;
    aw->m_events = (AvahiWatchEvent)0;
    aw->m_userdata = userdata;

    cmp->m_avahiWatch = aw;
    return aw;
}

static void watch_update(AvahiWatch *w, AvahiWatchEvent event)
{
    debug(std::cout << "avahi_watch_update" << std::endl << std::flush;);
    w->m_requiredEvent = event;
}

static AvahiWatchEvent watch_get_events(AvahiWatch *w)
{
    debug(std::cout << "avahi_watch_get_events" << std::endl << std::flush;);
    return w->m_events;
}

static void watch_free(AvahiWatch *w)
{
    debug(std::cout << "avahi_watch_free" << std::endl << std::flush;);
    log(WARNING, "avahi watch_free handler called");
}

struct AvahiTimeout {
    CommMDNSPublisher * m_publisher;
    struct timeval m_expiry;
    enum { NEW = 0, ENABLED, EXPIRED, DEAD } m_state;
    AvahiTimeoutCallback m_callback;
    void * m_userdata;
};

static AvahiTimeout* timeout_new(const AvahiPoll * api,
                                 const struct timeval * tv,
                                 AvahiTimeoutCallback callback,
                                 void *userdata)
{
    debug(std::cout << "avahi_timeout_new("
                    << (tv ? tv->tv_sec : -1)  << "," << callback << ")"
                    << std::endl << std::flush;);
    CommMDNSPublisher * cmp = static_cast<CommMDNSPublisher*>(api->userdata);

    AvahiTimeout * at = new AvahiTimeout;
    at->m_publisher = cmp;
    if (tv != 0) {
        at->m_expiry = *tv;
        at->m_state = AvahiTimeout::ENABLED;
    } else {
        at->m_state = AvahiTimeout::NEW;
    }
    at->m_callback = callback;
    at->m_userdata = userdata;

    cmp->m_avahiTimeouts.insert(at);

    debug(std::cout << "avahi_timeout_new " << at
                    << std::endl << std::flush;);
    return at;
}

static void timeout_update(AvahiTimeout * at, const struct timeval *tv)
{
    CommMDNSPublisher * cmp = at->m_publisher;

    debug(std::cout << "avahi_timeout_update(" << at << ","
                    << (tv ? tv->tv_sec : -1) << ")"
                    << std::endl << std::flush;);

    if (tv != 0) {
        at->m_expiry = *tv;
        at->m_state = AvahiTimeout::ENABLED;
        if (tv->tv_sec == 0) {
            cmp->setImmediate();
        }
    } else {
        at->m_state = AvahiTimeout::NEW; // FIXME Really, new again?
    }
}

static void timeout_free(AvahiTimeout * at)
{
    debug(std::cout << "avahi_timeout_free(" << at << ")"
                    << std::endl << std::flush;);
    at->m_state = AvahiTimeout::DEAD;
    at->m_publisher->m_avahiTimeouts.erase(at);
    delete at;
}

CommMDNSPublisher::CommMDNSPublisher(CommServer & svr,
                                     ServerRouting & s) : Idle(svr),
                                                         CommSocket(svr),
                                                         m_avahiClient(0),
                                                         m_avahiError(0),
                                                         m_server(s),
                                                         m_immediate(false),
                                                         m_avahiFd(-1),
                                                         m_avahiWatch(0),
                                                         m_group(0)
{
}

CommMDNSPublisher::~CommMDNSPublisher()
{
    // Finalise and delete
}

int CommMDNSPublisher::setup()
{
    static AvahiPoll poll = { this,
                              watch_new,
                              watch_update,
                              watch_get_events,
                              watch_free,
                              timeout_new,
                              timeout_update,
                              timeout_free
                            };

    m_avahiClient = avahi_client_new(&poll,
                                     (AvahiClientFlags)AVAHI_CLIENT_NO_FAIL,
                                     &client_callback,
                                     this,
                                     &m_avahiError);

    if (m_avahiClient == 0) {
        log(ERROR, String::compose("Avahi client creation failed: %1",
                                   avahi_strerror(m_avahiError)));
        return -1;
    }

    while (m_immediate) {
        m_immediate = false;
        checkTimers(0);
    }

    if (m_avahiFd == -1) {
        log(ERROR, "Avahi client has not registed a file descriptor");
        return -1;
    }

    return 0;
}

void CommMDNSPublisher::setup_service(AvahiClient * client)
{
    if (m_group == 0) {
        m_group = avahi_entry_group_new(client, &group_callback, this);
    }

    if (m_group == 0) {
        log(ERROR, String::compose("Avahi group creation failure. %1",
                                   avahi_strerror(avahi_client_errno(client))));
        return;
    }

    AvahiStringList * txt;
    txt = avahi_string_list_new(
      String::compose("builddate=%1",
                      std::string(consts::buildTime) + ", " +
                      std::string(consts::buildDate)).c_str(),
      String::compose("clients=%1", m_server.getClients()).c_str(),
      String::compose("ruleset=%1", m_server.getRuleset()).c_str(),
      String::compose("server=%1", "cyphesis").c_str(),
      String::compose("uptime=%1", m_server.m_world.upTime()).c_str(),
      String::compose("version=%1", std::string(consts::version)).c_str(),
      NULL);

    int ret;
    ret = avahi_entry_group_add_service_strlst(
      m_group,
      AVAHI_IF_UNSPEC,
      AVAHI_PROTO_UNSPEC,
      (AvahiPublishFlags)0,
      m_server.getName().c_str(),
      "_worldforge._tcp", NULL, NULL,
      client_port_num, txt);

    avahi_string_list_free(txt);
    if (ret < 0) {
        log(ERROR, "Avahi service publish failed");
        return;
    }

    ret = avahi_entry_group_commit(m_group);
    if (ret < 0) {
        log(ERROR, "Avahi service commit failed");
        return;
    }
}

void CommMDNSPublisher::checkTimers(time_t t)
{
    std::set<AvahiTimeout *>::const_iterator I = m_avahiTimeouts.begin();
    std::set<AvahiTimeout *>::const_iterator Iend = m_avahiTimeouts.end();
    for (; I != Iend; ++I) {
        if ((*I)->m_state == AvahiTimeout::ENABLED &&
            (*I)->m_expiry.tv_sec <= t) {
            debug(std::cout << "TImeout " << (*I) << " is now due at " << t
                            << std::endl << std::flush;);
            (*I)->m_state = AvahiTimeout::EXPIRED;
            (*I)->m_callback(*I, (*I)->m_userdata);
        }
    }
}

void CommMDNSPublisher::idle(time_t t)
{
    debug(std::cout << "idle " << t << std::endl;);
    checkTimers(t);
}

int CommMDNSPublisher::getFd() const
{
    return m_avahiFd;
}

bool CommMDNSPublisher::isOpen() const
{
    return m_avahiFd != -1;
}

bool CommMDNSPublisher::eof()
{
    return false;
}

int CommMDNSPublisher::read()
{
    assert(m_avahiWatch != 0);
    m_avahiWatch->m_events = AVAHI_WATCH_IN;
    m_avahiWatch->m_callback(m_avahiWatch,
                             m_avahiFd,
                             AVAHI_WATCH_IN,
                             m_avahiWatch->m_userdata);
    m_avahiWatch->m_events = (AvahiWatchEvent)0;
    return 0;
}

void CommMDNSPublisher::dispatch()
{
}

void CommMDNSPublisher::disconnect()
{
}

int CommMDNSPublisher::flush()
{
    return 0;
}

#endif // defined(HAVE_AVAHI)
