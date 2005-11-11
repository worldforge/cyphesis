// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef RULESETS_MIND_FACTORY_H
#define RULESETS_MIND_FACTORY_H

#include <string>
#include <map>

class BaseMind;

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
                       const std::string &,
                       const std::string &);

    void addMindType(const std::string & type, const std::string & mind) {
        m_mindTypes[type] = mind;
    }
};

#endif // RULESETS_MIND_FACTORY_H
