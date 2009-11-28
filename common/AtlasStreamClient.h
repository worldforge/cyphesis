// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2009 Alistair Riddoch
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

#ifndef COMMON_ATLAS_STREAM_CLIENT_H
#define COMMON_ATLAS_STREAM_CLIENT_H

#include <Atlas/Objects/Decoder.h>
#include <Atlas/Objects/ObjectsFwd.h>

namespace Atlas {
  class Codec;
} // namespace Atlas

class basic_socket_stream;

class AtlasStreamClient : public Atlas::Objects::ObjectsDecoder
{
  protected:
    /// \brief Flag to indicate that a reply has been received from the server
    bool reply_flag;
    /// \brief Flag to indicate that an error has been received from the server
    bool error_flag;
    /// \brief Counter used to track serial numbers sent to the server
    int serialNo;

    int m_fd;
    Atlas::Objects::ObjectsEncoder * m_encoder;
    Atlas::Codec * m_codec;
    basic_socket_stream * m_ios;
    std::string m_username;

    /// \brief Account identifier returned after successful login
    std::string accountId;
    /// \brief Stored error message from the last received Error operation
    std::string m_errorMessage;

    // void objectArrived(const Atlas::Objects::Root &);
    int authenticateLocal();
    int negotiate();
    int linger();
    void output(const Atlas::Message::Element & item, int depth = 0) const;

    virtual void objectArrived(const Atlas::Objects::Root &);

    virtual void operation(const Atlas::Objects::Operation::RootOperation &);

    virtual void infoArrived(const Atlas::Objects::Operation::RootOperation &) = 0;
    virtual void errorArrived(const Atlas::Objects::Operation::RootOperation &);

  public:
    AtlasStreamClient();
    virtual ~AtlasStreamClient();

    int newSerialNo() {
        return ++serialNo;
    }

    void send(const Atlas::Objects::Operation::RootOperation & op);
    int connect(const std::string & host, int port = 6767);
    int connectLocal(const std::string & host);
    int login(const std::string & username, const std::string & password);
    int create(const std::string & username, const std::string & password);
    int poll(int timeout = 0, int msec = 0);

};

#endif // COMMON_ATLAS_STREAM_CLIENT_H
