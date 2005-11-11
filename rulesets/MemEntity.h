// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

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
