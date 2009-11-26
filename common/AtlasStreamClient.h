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
    int m_fd;
    Atlas::Objects::ObjectsEncoder * m_encoder;
    Atlas::Codec * m_codec;
    basic_socket_stream * m_ios;
    std::string m_username;

    // void objectArrived(const Atlas::Objects::Root &);
    int authenticateLocal();
    int negotiate();
    int linger();
    void output(const Atlas::Message::Element & item, int depth = 0) const;

  public:
    AtlasStreamClient();
    virtual ~AtlasStreamClient();

    void send(const Atlas::Objects::Operation::RootOperation & op);
    int connect(const std::string & host, int port = 6767);
    int connectLocal(const std::string & host);
    int login(const std::string & username, const std::string & password);
    int create(const std::string & username, const std::string & password);
    int poll(int timeout = 0, int msec = 0);

};

#endif // COMMON_ATLAS_STREAM_CLIENT_H
