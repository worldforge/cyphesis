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
    if (status == SW_DISCOVERY_PUBLISH_STARTED) {
        // log(WARNING, "Started publishing using MDNS");
    } else if (status == SW_DISCOVERY_PUBLISH_STOPPED) {
        // log(WARNING, "Stopped publishing using MDNS");
    } else if (status == SW_DISCOVERY_PUBLISH_NAME_COLLISION) {
        log(WARNING, "Name collision publishing using MDNS");
    } else if (status == SW_DISCOVERY_PUBLISH_INVALID) {
        log(WARNING, "Invalid error publishing using MDNS");
    } else {
        log(ERROR, "Unknown error code using MDNS");
    }
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

    if (sw_discovery_publish(m_session, 0, "WorldForge Server",
                             "_worldforge._tcp.", NULL, NULL,
                             client_port_num, NULL, 0,
                             reply_callback, this, &m_oid) != SW_OKAY) {
        log(WARNING, "Unable to publish our presence using MDNS");
        return -1;
    }
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

    // FIXME Check return value
    if (sw_discovery_read_socket(m_session) != SW_OKAY) {
        log(WARNING, "Error publishing our presence using MDNS. Disabled.");
        sw_discovery_fina(m_session);
        m_session = 0;
        return -1;
    }

    return 0;
}

void CommMDNSPublisher::dispatch()
{
}

#endif // defined(HAVE_LIBHOWL)
