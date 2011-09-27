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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Ruleset.h"

#include "EntityBuilder.h"
#include "EntityRuleHandler.h"
#include "OpRuleHandler.h"
#include "TaskRuleHandler.h"
#include "Persistence.h"

#include "rulesets/MindFactory.h"

#include "common/log.h"
#include "common/debug.h"
#include "common/globals.h"
#include "common/const.h"
#include "common/Inheritance.h"
#include "common/AtlasFileLoader.h"
#include "common/compose.hpp"

#include <Atlas/Message/Element.h>
#include <Atlas/Objects/objectFactory.h>

#include <iostream>

#include <sys/types.h>
#ifdef HAVE_DIRENT_H
#include <dirent.h>
#endif // HAS_DIRENT_H

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Objects::Root;

using String::compose;

typedef std::map<std::string, Root> RootDict;

static const bool debug_flag = false;

Ruleset * Ruleset::m_instance = NULL;

void Ruleset::init()
{
    m_instance = new Ruleset(EntityBuilder::instance());
    m_instance->loadRules();
}


Ruleset::Ruleset(EntityBuilder * eb) : 
      m_taskHandler(new TaskRuleHandler(eb)),
      m_entityHandler(new EntityRuleHandler(eb)),
      m_opHandler(new OpRuleHandler(eb))
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

    const std::list<std::string> & parents = class_desc->getParents();
    if (parents.empty()) {
        log(ERROR, compose("Rule \"%1\" has empty parents. Skipping.",
                           class_name));
        return -1;
    }
    const std::string & parent = parents.front();
    if (parent.empty()) {
        log(ERROR, compose("Rule \"%1\" has empty first parent. Skipping.",
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
    } else {
        log(ERROR, compose("Rule \"%1\" has unknown objtype=\"%2\". Skipping.",
                           class_name, class_desc->getObjtype()));
        return -1;
    }

    return ret;
}

int Ruleset::installRule(const std::string & class_name,
                         const Root & class_desc)
{
    std::string dependent, reason;
    // Possibly we should report some types of failure here.
    int ret = installRuleInner(class_name, class_desc, dependent, reason);
    if (ret == 0 && database_flag) {
        Persistence * p = Persistence::instance();
        p->storeRule(class_desc, class_name);
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
    RuleWaitList::iterator I = m_waitingRules.lower_bound(class_name);
    RuleWaitList::iterator Iend = m_waitingRules.upper_bound(class_name);
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
    return;
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
    assert(!o->isDefaultParents());
    assert(!o->getParents().empty());
    if (class_desc->isDefaultParents()) {
        log(ERROR, compose("Updated type \"%1\" has no parents in its "
                           "description", class_name));
        return -1;
    }
    const std::list<std::string> & class_parents = class_desc->getParents();
    if (class_parents.empty()) {
        log(ERROR, compose("Updated type \"%1\" has empty parents in its "
                           "description", class_name));
        return -1;
    }
    if (class_parents.front() != o->getParents().front()) {
        log(ERROR, compose("Updated type \"%1\" attempting to change parent "
                           "from %2 to %3", class_name,
                           o->getParents().front(), class_parents.front()));
        return -1;
    }
    int ret = -1;
    if (m_opHandler->check(class_desc) == 0) {
        ret = m_opHandler->update(class_name, class_desc);
    } else if (m_taskHandler->check(o) == 0) {
        ret = m_taskHandler->update(class_name, class_desc);
    } else if (m_entityHandler->check(class_desc) == 0) {
        ret = m_entityHandler->update(class_name, class_desc);
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
/// that has not yet occured, or a more fatal condition has occured.
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

void Ruleset::getRulesFromFiles(RootDict & rules)
{
    std::string filename;

    std::string dirname = etc_directory + "/cyphesis/" + ruleset + ".d";
    DIR * rules_dir = ::opendir(dirname.c_str());
    if (rules_dir == 0) {
        filename = etc_directory + "/cyphesis/" + ruleset + ".xml";
        AtlasFileLoader f(filename, rules);
        if (f.isOpen()) {
            log(WARNING, compose("Reading legacy rule data from \"%1\".",
                                 filename));
            f.read();
        }
        return;
    }
    while (struct dirent * rules_entry = ::readdir(rules_dir)) {
        if (rules_entry->d_name[0] == '.') {
            continue;
        }
        filename = dirname + "/" + rules_entry->d_name;
        
        AtlasFileLoader f(filename, rules);
        if (!f.isOpen()) {
            log(ERROR, compose("Unable to open rule file \"%1\".", filename));
        } else {
            f.read();
        }
    }
    ::closedir(rules_dir);
}

void Ruleset::loadRules()
{
    RootDict ruleTable;

    if (database_flag) {
        Persistence * p = Persistence::instance();
        p->getRules(ruleTable);
    } else {
        getRulesFromFiles(ruleTable);
    }

    if (ruleTable.empty()) {
        log(ERROR, "Rule database table contains no rules.");
        if (database_flag) {
            log(NOTICE, "Attempting to load temporary ruleset from files.");
            getRulesFromFiles(ruleTable);
        }
    }

    RootDict::const_iterator Iend = ruleTable.end();
    for (RootDict::const_iterator I = ruleTable.begin(); I != Iend; ++I) {
        const std::string & class_name = I->first;
        const Root & class_desc = I->second;
        installItem(class_name, class_desc);
    }
    // Report on the non-cleared rules.
    // Perhaps we can keep them too?
    // m_waitingRules.clear();
    RuleWaitList::const_iterator J = m_waitingRules.begin();
    RuleWaitList::const_iterator Jend = m_waitingRules.end();
    for (; J != Jend; ++J) {
        log(ERROR, J->second.reason);
    }
}
