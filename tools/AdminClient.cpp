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

// $Id$

#include "AdminClient.h"

#include "common/debug.h"
#include "common/sockets.h"

#include <Atlas/Codec.h>
#include <Atlas/Net/Stream.h>

#include <Atlas/Objects/Encoder.h>
#include <Atlas/Objects/objectFactory.h>

#include <Atlas/Objects/Entity.h>
#include <Atlas/Objects/Anonymous.h>

#include <Atlas/Objects/Operation.h>

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

/// \brief Output formatted representation of Atlas message data.
///
/// @param item Atlas value to be output
/// @param recurse flag indicating whether to recurse into container messages
void AdminClient::output(const Element & item, bool recurse)
{
    std::cout << " ";
    switch (item.getType()) {
        case Element::TYPE_INT:
            std::cout << item.Int();
            break;
        case Element::TYPE_FLOAT:
            std::cout << item.Float();
            break;
        case Element::TYPE_STRING:
            std::cout << item.String();
            break;
        case Element::TYPE_LIST:
            if (recurse) {
                std::cout << "[ ";
                ListType::const_iterator I = item.List().begin();
                ListType::const_iterator Iend = item.List().end();
                for(; I != Iend; ++I) {
                    output(*I, true);
                }
                std::cout << " ]";
            } else {
                std::cout << "(list)";
            }
            break;
        case Element::TYPE_MAP:
            if (recurse) {
                std::cout << "{ ";
                MapType::const_iterator I = item.Map().begin();
                MapType::const_iterator Iend = item.Map().end();
                for(; I != Iend; ++I) {
                    std::cout << I->first << ": ";
                    output(I->second, true);
                }
                std::cout << " }";
            } else {
                std::cout << "(map)";
            }
            break;
        default:
            std::cout << "(\?\?\?)";
            break;
    }
}

/// \brief Function call from the base class when an object arrives from the
/// server
///
/// @param obj Object that has arrived from the server
void AdminClient::objectArrived(const Root & obj)
{
    RootOperation op = Atlas::Objects::smart_dynamic_cast<RootOperation>(obj);
    if (!op.isValid()) {
        std::cerr << "ERROR: Non op object received from server"
                  << std::endl << std::flush;;
        if (!obj->isDefaultParents() && !obj->getParents().empty()) {
            std::cerr << "NOTICE: Unexpected object has parent "
                      << obj->getParents().front()
                      << std::endl << std::flush;
        }
        if (!obj->isDefaultObjtype()) {
            std::cerr << "NOTICE: Unexpected object has objtype "
                      << obj->getObjtype()
                      << std::endl << std::flush;
        }
        return;
    }
    debug(std::cout << "A " << op->getParents().front() << " op from client!" << std::endl << std::flush;);

    int class_no = op->getClassNo();
    if (class_no == Atlas::Objects::Operation::INFO_NO) {
        infoArrived(op);
    } else if (class_no == Atlas::Objects::Operation::ERROR_NO) {
        errorArrived(op);
    }
}

/// \brief Called when an Info operation arrives
///
/// @param op Operation to be processed
void AdminClient::infoArrived(const RootOperation & op)
{
    reply_flag = true;
    if (op->getArgs().empty()) {
        return;
    }
    const Root & ent = op->getArgs().front();
    if (login_flag) {
        if (!ent->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
            std::cerr << "ERROR: Response to login does not contain account id"
                      << std::endl << std::flush;
            
        } else {
            accountId = ent->getId();
        }
    }
}

/// \brief Called when an Error operation arrives
///
/// @param op Operation to be processed
void AdminClient::errorArrived(const RootOperation & op)
{
    reply_flag = true;
    error_flag = true;
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        return;
    }
    const Root & arg = args.front();
    Element message_attr;
    if (arg->copyAttr("message", message_attr) == 0 && message_attr.isString()) {
        m_errorMessage = message_attr.String();
    }
}

/// \brief AdminClient constructor
AdminClient::AdminClient() : error_flag(false), reply_flag(false),
                             login_flag(false), exit(false)
{
}

AdminClient::~AdminClient()
{
}

/// \brief Main client application loop
///
/// Check for incoming data until the client is ready to exit
void AdminClient::loop()
{
    while (!exit) {
        poll();
    };
}

/// \brief Poll the codec to see if data is available
void AdminClient::poll()
{
    fd_set infds;
    struct timeval tv;

    FD_ZERO(&infds);

    FD_SET(m_fd, &infds);

    tv.tv_sec = 0;
    tv.tv_usec = 100000;

    int retval = select(m_fd+1, &infds, NULL, NULL, &tv);

    if (retval < 1) {
        return;
    }

    if (FD_ISSET(m_fd, &infds)) {
        if (m_ios->peek() == -1) {
            std::cerr << "Server disconnected" << std::endl << std::flush;
            exit = true;
        } else {
            m_codec->poll();
        }
    }
}

/// \brief Read login credentials from standard input
void AdminClient::getLogin()
{
    // This needs to be re-written to hide input, so the password can be
    // secret
    std::cout << "Username: " << std::flush;
    std::cin >> m_username;
    std::cout << "Password: " << std::flush;
    std::cin >> password;
}

/// \brief Check if a rule exists in the rule table
///
/// @return 0 if a rule exists, 1 if it does not
int AdminClient::checkRule(const std::string & id)
{
    error_flag = false;
    reply_flag = false;
    login_flag = false;

    Get g;

    Anonymous get_arg;
    get_arg->setId(id);
    get_arg->setObjtype("class");

    g->setArgs1(get_arg);

    g->setFrom(accountId);

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
    error_flag = false;
    reply_flag = false;
    login_flag = false;

    if (m_uploadedRules.find(id) != m_uploadedRules.end()) {
        std::cout << "Overriden rule " << id << " ignored."
                  << std::endl << std::flush;

        return -1;
    }

    if (checkRule(id) == 0) {
        std::cout << "Updating " << id << " on server."
                  << std::endl << std::flush;

        error_flag = false;
        reply_flag = false;
        login_flag = false;

        Set s;

        Root set_arg(0);
        try {
            set_arg = Atlas::Objects::Factories::instance()->createObject(rule);
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

        s->setFrom(accountId);

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

    MapType::const_iterator I = rule.find("parents");
    if (I == rule.end()) {
        std::cerr << "Rule " << id << " to be uploaded has no parents."
                  << std::endl << std::flush;
        return -1;
    }
    const Element & pelem = I->second;
    if (!pelem.isList()) {
        std::cerr << "Rule " << id << " to be uploaded has non-list parents."
                  << std::endl << std::flush;
        return -1;
    }
    const ListType & parents = pelem.asList();
    if (parents.empty() || !parents.front().isString()) {
        std::cerr << "Rule " << id << " to be uploaded has malformed parents."
                  << std::endl << std::flush;
        return -1;
    }
    const std::string & parent = parents.front().asString();

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

    error_flag = false;
    reply_flag = false;
    login_flag = false;

    Create c;

    Root create_arg;
    try {
        create_arg  = Atlas::Objects::Factories::instance()->createObject(rule);
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

    c->setFrom(accountId);

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
        debug(std::cout << "WAITING rule " << waitId
                        << " now ready" << std::endl << std::flush;);
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
       poll();
    }
}

/// \brief Send a Login operation to the remote server
///
/// This function uses credentials that have been set earlier.
int AdminClient::login()
{
    error_flag = false;
    reply_flag = false;
    login_flag = true;
 
    AtlasStreamClient::login(m_username, password);
 
    waitForInfo();

    login_flag = false;

    if (!error_flag) {
       return 0;
    }
    return -1;
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
                  << "\" was never uploaded as its parent does not exist in any of the available rulesets."
                  << std::endl << std::flush;
    }
}
