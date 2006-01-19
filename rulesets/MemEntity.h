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

#ifndef RULESETS_MEM_ENTITY_H
#define RULESETS_MEM_ENTITY_H

#include "rulesets/Entity.h"

/// \brief This class is used to represent entities inside MemMap used
/// by the mind of an AI.
///
/// It adds a flag to indicate if this entity is currently visible, and
/// a means of tracking when it was last seen, so garbage entities can
/// be cleaned up.
class MemEntity : public Entity {
  protected:
    bool m_visible;
    double m_lastSeen;
  public:
    explicit MemEntity(const std::string & id, long intId);
    virtual ~MemEntity();

    bool isVisible() const {
        return m_visible;
    }

    void setVisible(bool v = true) {
        m_visible = true;
    }

    const double & lastSeen() const {
        return m_lastSeen;
    }

    void update(const double & d) {
        if (d >= m_lastSeen) {
            m_lastSeen = d;
        }
    }
};

#endif // RULESETS_MEM_ENTITY_H
