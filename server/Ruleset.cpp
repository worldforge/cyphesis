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


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Ruleset.h"

#include "EntityBuilder.h"
#include "EntityRuleHandler.h"
#include "OpRuleHandler.h"
#include "PropertyRuleHandler.h"
#include "TaskRuleHandler.h"
#include "ArchetypeRuleHandler.h"
#include "Persistence.h"

#include "common/log.h"
#include "common/debug.h"
#include "common/globals.h"
#include "common/const.h"
#include "common/Inheritance.h"
#include "common/AtlasFileLoader.h"
#include "common/compose.hpp"
#include "common/AssetsManager.h"

#include <Atlas/Objects/Anonymous.h>

#include <boost/filesystem.hpp>

#include <iostream>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Objects::Root;

using String::compose;

typedef std::map<std::string, Root> RootDict;

static const bool debug_flag = false;

Ruleset * Ruleset::m_instance = nullptr;

void Ruleset::init(const std::string & ruleset)
{
    m_instance = new Ruleset(EntityBuilder::instance());
    m_instance->loadRules(ruleset);
}


Ruleset::Ruleset(EntityBuilder * eb) : 
      m_taskHandler(new TaskRuleHandler(eb)),
      m_entityHandler(new EntityRuleHandler(eb)),
      m_opHandler(new OpRuleHandler(eb)),
      m_propertyHandler(new PropertyRuleHandler(eb)),
      m_archetypeHandler(new ArchetypeRuleHandler(eb))
{
}

Ruleset::~Ruleset()
{
}

int Ruleset::installRuleInner(const std::string & class_name,
                              const Root & class_desc,
                              std::string & dependent,
                              std::string & reason)
{
    assert(class_name == class_desc->getId());

    if (class_name.size() > consts::id_len) {
        log(ERROR, compose("Rule \"%1\" has name longer than %2 characters. "
                           "Skipping.", class_name, consts::id_len));
        return -1;
    }

    const std::string & parent = class_desc->getParent();
    if (parent.empty()) {
        log(ERROR, compose("Rule \"%1\" has empty parent. Skipping.",
                           class_name));
        return -1;
    }
    int ret = -1;
    if (m_opHandler->check(class_desc) == 0) {
        ret = m_opHandler->install(class_name, parent, class_desc,
                                   dependent, reason);
    } else if (m_taskHandler->check(class_desc) == 0) {
        ret = m_taskHandler->install(class_name, parent, class_desc,
                                     dependent, reason);
    } else if (m_entityHandler->check(class_desc) == 0) {
        ret = m_entityHandler->install(class_name, parent, class_desc,
                                       dependent, reason);
    } else if (m_propertyHandler->check(class_desc) == 0) {
        ret = m_propertyHandler->install(class_name, parent, class_desc,
                                       dependent, reason);
    } else if (m_archetypeHandler->check(class_desc) == 0) {
        ret = m_archetypeHandler->install(class_name, parent, class_desc,
                                       dependent, reason);
    } else {
        log(ERROR, compose(R"(Rule "%1" has unknown objtype="%2". Skipping.)",
                           class_name, class_desc->getObjtype()));
        return -1;
    }

    return ret;
}

int Ruleset::installRule(const std::string & class_name,
                         const std::string & section,
                         const Root & class_desc)
{
    std::string dependent, reason;
    // Possibly we should report some types of failure here.
    int ret = installRuleInner(class_name, class_desc, dependent, reason);
    if (ret == 0 && database_flag) {
        Persistence * p = Persistence::instance();
        p->storeRule(class_desc, class_name, section);
    }
    return ret;
}

void Ruleset::installItem(const std::string & class_name,
                          const Root & class_desc)
{
    std::string dependent, reason;
    int ret = installRuleInner(class_name, class_desc, dependent, reason);
    if (ret != 0) {
        if (ret > 0) {
            waitForRule(class_name, class_desc, dependent, reason);
        }
        return;
    }

    // Install any rules that were waiting for this rule before they
    // could be installed
    auto I = m_waitingRules.lower_bound(class_name);
    auto Iend = m_waitingRules.upper_bound(class_name);
    RootDict readyRules;
    for (; I != Iend; ++I) {
        const std::string & wClassName = I->second.name;
        const Root & wClassDesc = I->second.desc;
        readyRules.insert(std::make_pair(wClassName, wClassDesc));
        debug(std::cout << "WAITING rule " << wClassName
                        << " now ready from " << class_name
                        << std::endl << std::flush;);
    }
    m_waitingRules.erase(class_name);
        
    RootDict::const_iterator K = readyRules.begin();
    RootDict::const_iterator Kend = readyRules.end();
    for (; K != Kend; ++K) {
        const std::string & rClassName = K->first;
        const Root & rClassDesc = K->second;
        installItem(rClassName, rClassDesc);
    }
}

int Ruleset::modifyRule(const std::string & class_name,
                        const Root & class_desc)
{
    assert(class_name == class_desc->getId());

    Root o = Inheritance::instance().getClass(class_name);
    if (!o.isValid()) {
        log(ERROR, compose("Could not find existing type \"%1\" in "
                           "inheritance", class_name));
        return -1;
    }
    assert(!o->isDefaultParent());
    assert(!o->getParent().empty());
    if (class_desc->isDefaultParent() || class_desc->getParent().empty()) {
        log(ERROR, compose("Updated type \"%1\" has no parent in its "
                           "description", class_name));
        return -1;
    }
    if (class_desc->getParent() != o->getParent()) {
        log(ERROR, compose("Updated type \"%1\" attempting to change parent "
                           "from %2 to %3", class_name,
                           o->getParent(), class_desc->getParent()));
        return -1;
    }
    int ret = -1;
    if (m_opHandler->check(class_desc) == 0) {
        ret = m_opHandler->update(class_name, class_desc);
    } else if (m_taskHandler->check(o) == 0) {
        ret = m_taskHandler->update(class_name, class_desc);
    } else if (m_entityHandler->check(class_desc) == 0) {
        ret = m_entityHandler->update(class_name, class_desc);
    } else if (m_propertyHandler->check(class_desc) == 0) {
        ret = m_propertyHandler->update(class_name, class_desc);
    }
    if (ret == 0) {
        Inheritance::instance().updateClass(class_name, class_desc);
        if (database_flag) {
            Persistence * p = Persistence::instance();
            p->updateRule(class_desc, class_name);
        }
    }
    return ret;
}

/// \brief Mark a rule down as waiting for another.
///
/// Note that a rule cannot yet be installed because it depends on something
/// that has not yet occurred, or a more fatal condition has occurred.
void Ruleset::waitForRule(const std::string & rulename,
                          const Root & ruledesc,
                          const std::string & dependent,
                          const std::string & reason)
{
    RuleWaiting rule;
    rule.name = rulename;
    rule.desc = ruledesc;
    rule.reason = reason;

    m_waitingRules.insert(std::make_pair(dependent, rule));
}

void Ruleset::getRulesFromFiles(const std::string & ruleset,
                                RootDict & rules)
{

    boost::filesystem::path directory = boost::filesystem::path(etc_directory) / "cyphesis" / (ruleset + ".d");

    if (boost::filesystem::is_directory(directory)) {

        AssetsManager::instance().observeDirectory(directory, [&](const boost::filesystem::path& path) {
            if (boost::filesystem::is_regular_file(path)) {
                try {
                    log(NOTICE, compose("Reloading rule file %1", path));
                    auto& filename = path.native();
                    RootDict updatedRules;
                    AtlasFileLoader f(filename, updatedRules);
                    if (!f.isOpen()) {
                        log(ERROR, compose("Unable to open rule file \"%1\".", filename));
                    } else {
                        log(INFO, compose("Rule file \"%1\" reloaded.", filename));
                        f.read();

                        for (auto& entry: updatedRules) {
                            auto& class_name = entry.first;
                            auto& class_desc = entry.second;
                            if (Inheritance::instance().hasClass(class_name)) {
                                log(INFO, compose("Updating existing rule \"%1\".", class_name));
                                modifyRule(class_name, class_desc);
                            } else {
                                log(INFO, compose("Installing new rule \"%1\".", class_name));
                                installItem(class_name, class_desc);
                            }
                        }
                    }
                } catch (const std::exception& e) {
                    log(ERROR, compose("Error when reacting to changed file at '%1': %2", path, e.what()));
                }
            }
        });

        boost::filesystem::recursive_directory_iterator dir(directory), end;
        log(INFO, compose("Trying to load rules from directory '%1'", directory));

        int count = 0;
        while (dir != end) {
            if (boost::filesystem::is_regular_file(dir->status())) {
                auto filename = dir->path().native();
                AtlasFileLoader f(filename, rules);
                if (!f.isOpen()) {
                    log(ERROR, compose("Unable to open rule file \"%1\".", filename));
                } else {
                    f.read();
                    count += f.count();
                }
            }
            ++dir;
        }

        log(INFO, compose("Loaded %1 rules.", count));
    }


}

void Ruleset::loadRules(const std::string & ruleset)
{
    RootDict ruleTable;

    bool loadFromDatabase = database_flag;
    loadFromDatabase = false;

    if (loadFromDatabase) {
        Persistence * p = Persistence::instance();
        p->getRules(ruleTable);
    } else {
        getRulesFromFiles(ruleset, ruleTable);
    }

    if (ruleTable.empty()) {
        log(ERROR, "Rule database table contains no rules.");
        if (loadFromDatabase) {
            log(NOTICE, "Attempting to load temporary ruleset from files.");
            getRulesFromFiles(ruleset, ruleTable);
        }
    }

    for (auto& entry : ruleTable) {
        const std::string & class_name = entry.first;
        const Root & class_desc = entry.second;
        installItem(class_name, class_desc);
    }
    // Report on the non-cleared rules.
    // Perhaps we can keep them too?
    // m_waitingRules.clear();
    for (auto& entry : m_waitingRules) {
        log(ERROR, entry.second.reason);
    }
}
