// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if defined(HAVE_LIBHOWL)

#undef PACKAGE
#undef VERSION

#include "CommMDNSPublisher.h"

#include <discovery/discovery.h>

#include <common/log.h>
#include <common/globals.h>

#include <cassert>

static sw_result reply_callback(sw_discovery,
                                sw_discovery_oid,
                                sw_discovery_publish_status status,
                                sw_opaque)
{
    fprintf(stderr, "STATUS: %x\n", status);
    return SW_OKAY;
}

CommMDNSPublisher::CommMDNSPublisher(CommServer & svr) : CommSocket(svr),
                                                         m_session(0)
{
}

CommMDNSPublisher::~CommMDNSPublisher()
{
}

int CommMDNSPublisher::setup()
{
    if (sw_discovery_init(&m_session) != SW_OKAY) {
        log(WARNING, "Unable to create MDNS publisher session");
        return -1;
    }

    fprintf(stderr, "Session %x found\n", m_session);

    log(INFO, "Created MDNS publisher session");
    if (sw_discovery_publish(m_session, 0, "WorldForge Server",
                             "worldforge._tcp.", NULL, NULL, port_num, NULL,
                             0, reply_callback, this, &m_oid) != SW_OKAY) {
        log(WARNING, "Unable to publish our presence");
        return -1;
    }
    log(INFO, "Published service information");
    return 0;
}

int CommMDNSPublisher::getFd() const
{
    assert(m_session != 0);

    return sw_discovery_socket(m_session);
}

bool CommMDNSPublisher::isOpen() const
{
    return true;
}

bool CommMDNSPublisher::eof()
{
    return false;
}

int CommMDNSPublisher::read()
{
    assert(m_session != 0);

    fprintf(stderr, "Session is %x\n", m_session);

    // FIXME Check return value
    sw_discovery_read_socket(m_session);

    return 0;
}

void CommMDNSPublisher::dispatch()
{
}

#endif // defined(HAVE_LIBHOWL)
