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


#ifndef RULESETS_EXTERNAL_MIND_H
#define RULESETS_EXTERNAL_MIND_H

#include "common/Router.h"

class Link;
class Entity;
class LocatedEntity;

/// \brief This class connects in-game entities to the Link of the client
/// controlling it.
///
/// Essentially used to relay in-game operations that pass to the mind on
/// to the client.
class ExternalMind : public Router {
  protected:
    Link * m_external;
    LocatedEntity & m_entity;
    double m_lossTime;

    void deleteEntity(const std::string & id, bool forceDelete);
    void purgeEntity(const LocatedEntity & ent, bool forceDelete = false);
  public:

    ExternalMind(LocatedEntity &);
    virtual ~ExternalMind();

    void externalOperation(const Operation & op, Link &) override;
    void operation(const Operation &, OpVector &) override;

    bool isLinked() { return m_external != 0; }
    bool isLinkedTo(Link * c) { return m_external == c; }

    const std::string & connectionId();

    void linkUp(Link * c);
};

#endif // RULESETS_EXTERNAL_MIND_H
