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

// $Id: 8c1a8928b595bfd952f68230678f41a99d543b14 $

#ifndef SERVER_EXTERNAL_MIND_H
#define SERVER_EXTERNAL_MIND_H

#include "common/Identified.h"

class Connection;
class Entity;
class LocatedEntity;

/// \brief This class connects in-game entities to the Connection of the client
/// controlling it.
///
/// Essentially used to relay in-game operations that pass to the mind on
/// to the client.
class ExternalMind : public Router {
  protected:
    Connection * m_connection;
    Entity & m_entity;
    double m_lossTime;

    void deleteEntity(const std::string & id);
    void purgeEntity(const LocatedEntity & ent);
  public:

    ExternalMind(Entity &);
    virtual ~ExternalMind();

    virtual void operation(const Operation &, OpVector &);

    bool isConnected() { return m_connection != 0; }
    bool isConnectedTo(Connection * c) { return m_connection == c; }

    const std::string & connectionId();

    void connect(Connection * c);
};

#endif // SERVER_EXTERNAL_MIND_H
