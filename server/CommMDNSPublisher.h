// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef SERVER_COMM_MDNS_PUBLISHER_H
#define SERVER_COMM_MDNS_PUBLISHER_H

#include "CommSocket.h"
#include "Idle.h"

#include <set>

#include <stdint.h>

/// \brief Handle a socket used to communicate with the MDNS responder daemon.
class CommMDNSPublisher : public CommSocket, virtual public Idle {
  private:
    // Howl data
    /// Discovery session identifier.
    uint32_t m_oid;
    ///
    struct _sw_discovery * m_session;

    // Avahi data
    ///
    struct AvahiClient * m_avahiClient;
    ///
    int m_avahiError;

  public:
    ///
    int m_avahiFd;
    ///
    struct AvahiWatch * m_avahiWatch;
    ///
    struct AvahiEntryGroup * m_group;
    ///
    std::set<struct AvahiTimeout *> m_avahiTimeouts;

    explicit CommMDNSPublisher(CommServer & svr);

    virtual ~CommMDNSPublisher();

    int setup();
    void setup_service(struct AvahiClient * );

    virtual void idle(time_t);

    int getFd() const;
    bool isOpen() const;
    bool eof();
    int read();
    void dispatch();
};

#endif // SERVER_COMM_MDNS_PUBLISHER_H
