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

// $Id: MindFactory.h,v 1.11 2007-11-28 20:22:43 alriddoch Exp $

#ifndef RULESETS_MIND_FACTORY_H
#define RULESETS_MIND_FACTORY_H

#include <string>
#include <map>

class BaseMind;
class TypeNode;

/// \brief Factory class for creating minds
///
/// A mind consists of a C++ base class and an associated script object
class MindFactory {
  public:
    typedef std::map<std::string, std::string> mindmap_t;
  private:
    MindFactory();
    static MindFactory * m_instance;
    mindmap_t m_mindTypes;

  public:
    static MindFactory * instance() {
        if (m_instance == NULL) {
            m_instance = new MindFactory();
        }
        return m_instance;
    }
    static void del() {
        if (m_instance != NULL) {
            delete m_instance;
        }
    }
    BaseMind * newMind(const std::string &, long,
                       const TypeNode * const );

    void addMindType(const std::string & type, const std::string & mind) {
        m_mindTypes[type] = mind;
    }
};

#endif // RULESETS_MIND_FACTORY_H
