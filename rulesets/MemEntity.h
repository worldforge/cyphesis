// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef MEM_ENTITY_H
#define MEM_ENTITY_H

#include "rulesets/Entity.h"

class MemEntity : public Entity {
  protected:
    bool m_visible;
    double m_lastSeen;
  public:
    explicit MemEntity(const std::string & id);
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
        } else if (d > 0) {
            std::cout << "Going back in time from " << m_lastSeen << " to "
                      << d << std::endl << std::flush;
        }
    }
};

#endif // MEM_ENTITY_H
