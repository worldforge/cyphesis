// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2001 Alistair Riddoch
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


#include "AdminClient.h"

#include "common/debug.h"

#include <Atlas/Net/Stream.h>

#include <Atlas/Objects/Encoder.h>

#include <Atlas/Objects/Entity.h>

#include <iostream>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;

using Atlas::Objects::Root;

using Atlas::Objects::Entity::Account;
using Atlas::Objects::Entity::Anonymous;

using Atlas::Objects::Operation::RootOperation;
using Atlas::Objects::Operation::Get;
using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Create;
using Atlas::Objects::Operation::Login;
using Atlas::Objects::Operation::Info;
using Atlas::Objects::Operation::Error;

static const bool debug_flag = false;

/// \brief AdminClient constructor
AdminClient::AdminClient(Atlas::Objects::Factories& factories, boost::asio::io_context& io_context)
: AtlasStreamClient(io_context, factories)
{
}

AdminClient::~AdminClient() = default;

/// \brief Read login credentials from standard input
void AdminClient::getLogin()
{
    // This needs to be re-written to hide input, so the password can be
    // secret
    std::cout << "Username: " << std::flush;
    std::cin >> m_username;
    std::cout << "Password: " << std::flush;
    std::cin >> m_password;
}

/// \brief Check if a rule exists in the rule table
///
/// @return 0 if a rule exists, 1 if it does not
int AdminClient::checkRule(const std::string & id)
{
    Get g;

    Anonymous get_arg;
    get_arg->setId(id);
    get_arg->setObjtype("class");

    g->setArgs1(get_arg);

    g->setFrom(m_accountId);

    send(g);

    waitForInfo();

    if (!error_flag) {
        return 0;
    }
    return 1;
}

/// \brief Upload a rule description to the server
///
/// @param id Identifier of the rule to be uploaded
/// @param set Name of the ruleset rule is from
/// @param rule Atlas description of the rule
int AdminClient::uploadRule(const std::string & id, const std::string & set,
                            const MapType & rule)
{
    if (m_uploadedRules.find(id) != m_uploadedRules.end()) {
        std::cout << "Overridden rule " << id << " ignored."
                  << std::endl << std::flush;

        return -1;
    }

    if (checkRule(id) == 0) {
        std::cout << "Updating " << id << " on server."
                  << std::endl << std::flush;

        Set s;

        Root set_arg(0);
        try {
            set_arg = m_factories.createObject(rule);
        }
        catch (Atlas::Message::WrongTypeException&) {
            std::cerr << "Malformed data in rule"
                      << std::endl << std::flush;
            return -1;
        }
        if (!set_arg.isValid()) {
            std::cerr << "Unknown error converting rule for upload"
                      << std::endl << std::flush;
            return -1;
        }
        set_arg->setAttr("ruleset", set);

        s->setArgs1(set_arg);

        s->setFrom(m_accountId);

        send(s);

        waitForInfo();

        if (error_flag) {
            std::cerr << "Failed to update existing \"" << id << "\" class."
                      << std::endl;
            std::cerr << "Server Error: \"" << m_errorMessage << "\"."
                      << std::endl << std::flush;
            return -1;
        }

        m_uploadedRules.insert(id);

        return 0;
    }

    MapType::const_iterator I = rule.find("parent");
    if (I == rule.end()) {
        std::cerr << "Rule " << id << " to be uploaded has no parent."
                  << std::endl << std::flush;
        return -1;
    }
    const Element & pelem = I->second;
    if (!pelem.isString()) {
        std::cerr << "Rule " << id << " to be uploaded has non-string parent."
                  << std::endl << std::flush;
        return -1;
    }
    const std::string& parent = pelem.asString();

    if (checkRule(parent) != 0) {
        debug(std::cerr << "Rule \"" << id << "\" to be uploaded has parent \""
                        << parent << "\" which does not exist on server yet."
                        << std::endl << std::flush;);
        RuleWaitList::const_iterator J = m_waitingRules.lower_bound(parent);
        RuleWaitList::const_iterator Jend = m_waitingRules.upper_bound(parent);
        for (; J != Jend; ++J) {
            if (id == J->second.first.first) {
                debug(std::cerr << "Discarding rule with ID \"" << id
                                << "\" as one is already waiting for upload."
                                << std::endl << std::flush;);
                return -1;
            }
        }
        m_waitingRules.insert(make_pair(parent, make_pair(make_pair(id, set), rule))
);
        return -1;
    }

    std::cout << "Uploading " << id << " to server." << std::endl << std::flush;

    Create c;

    Root create_arg;
    try {
        create_arg  = m_factories.createObject(rule);
    }
    catch (Atlas::Message::WrongTypeException&) {
        std::cerr << "Malformed data in rule"
                  << std::endl << std::flush;
        return -1;
    }
    if (!create_arg.isValid()) {
        std::cerr << "Unknown error converting rule for upload"
                  << std::endl << std::flush;
        return -1;
    }
    create_arg->setAttr("ruleset", set);

    c->setArgs1(create_arg);

    c->setFrom(m_accountId);

    send(c);

    waitForInfo();

    if (error_flag) {
        std::cerr << "Failed to upload new \"" << id << "\" class."
                  << std::endl;
        std::cerr << "Server Error: \"" << m_errorMessage << "\"."
                  << std::endl << std::flush;
        return -1;
    }

    m_uploadedRules.insert(id);

    int count = 1;

    RuleWaitList::const_iterator J = m_waitingRules.lower_bound(id);
    RuleWaitList::const_iterator Jend = m_waitingRules.upper_bound(id);
    for (; J != Jend; ++J) {
        const std::string & waitId = J->second.first.first;
        const std::string & waitSet = J->second.first.second;
        const MapType & waitRule = J->second.second;
        debug_print("WAITING rule " << waitId << " now ready");
        int ret = uploadRule(waitId, waitSet, waitRule);
        if (ret > 0) {
            count += ret;
        }
    }
    m_waitingRules.erase(id);

    return count;
}

/// \brief Keep polling the server connection until a response arrives.
///
/// An Info operation is typically expected but this function will also
/// return if an Error operation arrives.
void AdminClient::waitForInfo()
{
    for (int i = 0; i < 10 && !reply_flag; ++i) {
       poll(std::chrono::microseconds(100000));
    }
}

/// \brief Send a Login operation to the remote server
///
/// This function uses credentials that have been set earlier.
int AdminClient::login()
{
    return AtlasStreamClient::login(m_username, m_password);
}

/// \brief Report information about rules which didn't upload
///
/// When this client is used to upload rules to the server, sometimes it is
/// not possible to upload some until their location in the inheritance
/// tree has been found. This function reports any rules for which a place
/// was never found, typically because its parent did not exist in the tree.
void AdminClient::report()
{
    if (m_waitingRules.empty()) {
        return;
    }

    RuleWaitList::const_iterator I = m_waitingRules.begin();
    RuleWaitList::const_iterator Iend = m_waitingRules.end();
    for (; I != Iend; ++I) {
        std::cout << "Rule \"" << I->second.first.first << "\" with parent \""
                  << I->first << "\" from ruleset \""
                  << I->second.first.second
                  << "\" was never uploaded as its parent does not exist in "
                     "any of the available rulesets."
                  << std::endl << std::flush;
    }
}
