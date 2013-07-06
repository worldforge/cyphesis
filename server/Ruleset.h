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


#ifndef SERVER_RULESET_H
#define SERVER_RULESET_H

#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/SmartPtr.h>

class EntityBuilder;
class EntityKit;
class RuleHandler;
class TaskKit;

/// \brief Class to handle rules that cannot yet be installed, and the reason
class RuleWaiting {
  public:
    /// Name of the rule.
    std::string name;
    /// Complete description of the rule.
    Atlas::Objects::Root desc;
    /// Message giving a description of why this rule has not been installed.
    std::string reason;
};

typedef std::multimap<std::string, RuleWaiting> RuleWaitList;

/// \brief Builder to handle the creation of all entities for the world.
///
/// Uses PersistantThingFactory to store information about entity types, and
/// create them. Handles connecting entities to their persistor as required.
class Ruleset {
  protected:
    explicit Ruleset(EntityBuilder * eb);
    ~Ruleset();
    static Ruleset * m_instance;
    RuleHandler * const m_taskHandler;
    RuleHandler * const m_entityHandler;
    RuleHandler * const m_opHandler;
    RuleHandler * const m_propertyHandler;

    RuleWaitList m_waitingRules;

    void installItem(const std::string & class_name,
                     const Atlas::Objects::Root & class_desc);
    int installRuleInner(const std::string & class_name,
                         const Atlas::Objects::Root & class_desc,
                         std::string & dependent,
                         std::string & reason);
    void getRulesFromFiles(const std::string &,
                           std::map<std::string, Atlas::Objects::Root> &);
    void loadRules(const std::string &);

    void waitForRule(const std::string & class_name,
                     const Atlas::Objects::Root & class_desc,
                     const std::string & dependent,
                     const std::string & reason);
  public:
    static void init(const std::string &);

    static Ruleset * instance() {
        return m_instance;
    }
    static void del() {
        if (m_instance != 0) {
            delete m_instance;
            m_instance = 0;
        }
    }

    int installRule(const std::string & class_name,
                    const std::string & section,
                    const Atlas::Objects::Root & class_desc);
    int modifyRule(const std::string & class_name,
                   const Atlas::Objects::Root & class_desc);

    friend class Rulesetintegration;
};

#endif // SERVER_RULESET_H
