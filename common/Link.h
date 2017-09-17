// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2012 Alistair Riddoch
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


#ifndef COMMON_LINK_H
#define COMMON_LINK_H

#include "common/Router.h"

class CommSocket;

namespace Atlas {
  namespace Objects {
    class ObjectsEncoder;
  }
}

/// \brief Class representing links from a client at the Atlas level.
class Link : public Router {
  protected:
    /// \brief The Atlas encoder used to send objects over this link
    Atlas::Objects::ObjectsEncoder * m_encoder;
  public:
    CommSocket & m_commSocket;

    Link(CommSocket & commSocket, const std::string & id, long iid);
    virtual ~Link();

    void setEncoder(Atlas::Objects::ObjectsEncoder * e) {
        m_encoder = e;
    }

    /**
     * Sends an op and flushes the socket.
     *
     * If you intend to send multiple ops, consider using the overload which
     * accepts an OpVector instead, since the socket will be flushed in this call.
     * @param op An op to send.
     */
    void send(const Operation & op) const;

    /**
     * Sends multiple ops, and flushes the socket.
     *
     * @param opVector A vector of ops to send.
     */
    void send(const OpVector& opVector) const;

    void sendError(const Operation & op,
                   const std::string &,
                   const std::string &) const;
    void disconnect();
};

#endif // COMMON_LINK_H
