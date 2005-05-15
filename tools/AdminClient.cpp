// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "AdminClient.h"

#include "common/debug.h"

#include <Atlas/Net/Stream.h>

#include <Atlas/Objects/Encoder.h>

#include <Atlas/Objects/Entity/Account.h>

#include <Atlas/Objects/Operation/Get.h>
#include <Atlas/Objects/Operation/Set.h>
#include <Atlas/Objects/Operation/Create.h>
#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Objects/Operation/Error.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;

using Atlas::Objects::Entity::Account;

using Atlas::Objects::Operation::Get;
using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Create;
using Atlas::Objects::Operation::Login;
using Atlas::Objects::Operation::Info;
using Atlas::Objects::Operation::Error;

static const bool debug_flag = false;

void AdminClient::output(const Element & item, bool recurse)
{
    std::cout << " ";
    switch (item.getType()) {
        case Element::TYPE_INT:
            std::cout << item.asInt();
            break;
        case Element::TYPE_FLOAT:
            std::cout << item.asFloat();
            break;
        case Element::TYPE_STRING:
            std::cout << item.asString();
            break;
        case Element::TYPE_LIST:
            if (recurse) {
                std::cout << "[ ";
                ListType::const_iterator I = item.asList().begin();
                ListType::const_iterator Iend = item.asList().end();
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
                MapType::const_iterator I = item.asMap().begin();
                MapType::const_iterator Iend = item.asMap().end();
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

void AdminClient::objectArrived(const Info& o)
{
    reply_flag = true;
    if (o.getArgs().empty()) {
        return;
    }
    const MapType & ent = o.getArgs().front().asMap();
    MapType::const_iterator Iend = ent.end();
    if (login_flag) {
        MapType::const_iterator I = ent.find("id");
        if (I == Iend || !I->second.isString()) {
            std::cerr << "ERROR: Response to login does not contain account id"
                      << std::endl << std::flush;
            
        } else {
            accountId = I->second.asString();
        }
    }
}

void AdminClient::objectArrived(const Error& o)
{
    reply_flag = true;
    error_flag = true;
    const ListType & args = o.getArgs();
    const Element & arg = args.front();
    if (arg.isString()) {
        m_errorMessage = arg.asString();
    } else if (arg.isMap()) {
        m_errorMessage = arg.asMap().find("message")->second.asString();
    }
}

AdminClient::AdminClient() : error_flag(false), reply_flag(false),
                             login_flag(false), encoder(0), codec(0),
                             ios(0), exit(false)
{
}

AdminClient::~AdminClient()
{
    if (encoder != 0) {
        delete encoder;
    }
    if (codec != 0) {
        delete codec;
    }
    if (ios != 0) {
        delete ios;
    }
}


void AdminClient::loop()
{
    while (!exit) {
        poll();
    };
}

void AdminClient::poll()
// poll the codec if select says there is something there.
{
    fd_set infds;
    struct timeval tv;

    FD_ZERO(&infds);

    FD_SET(cli_fd, &infds);

    tv.tv_sec = 0;
    tv.tv_usec = 100000;

    int retval = select(cli_fd+1, &infds, NULL, NULL, &tv);

    if (retval) {
        if (FD_ISSET(cli_fd, &infds)) {
            if (ios->peek() == -1) {
                std::cerr << "Server disconnected" << std::endl << std::flush;
                exit = true;
            } else {
                codec->poll();
            }
        }
    }
}

void AdminClient::getLogin()
{
    // This needs to be re-written to hide input, so the password can be
    // secret
    std::cout << "Username: " << std::flush;
    std::cin >> username;
    std::cout << "Password: " << std::flush;
    std::cin >> password;
}

int AdminClient::checkRule(const std::string & id)
/// @return 0 if a rule exists, 1 if it does not
{
    error_flag = false;
    reply_flag = false;
    login_flag = false;

    Get g;

    ListType & get_args = g.getArgs();
    MapType arg;
    arg["id"] = id;
    arg["objtype"] = "class";

    get_args.push_back(arg);

    g.setFrom(accountId);

    encoder->streamMessage(&g);

    (*ios) << std::flush;

    waitForInfo();

    if (!error_flag) {
        return 0;
    }
    return 1;
}

int AdminClient::uploadRule(const std::string & id, const std::string & set,
                            const MapType & rule)
{
    error_flag = false;
    reply_flag = false;
    login_flag = false;

    if (checkRule(id) == 0) {
        return -1;
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
        // FIXME Make sure there is not a rule with the same ID already waiting
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

    Create s;

    ListType & set_args = s.getArgs();
    MapType new_rule(rule);
    new_rule["ruleset"] = set;
    new_rule["objtype"] = "class";

    set_args.push_back(new_rule);

    s.setFrom(accountId);

    encoder->streamMessage(&s);

    (*ios) << std::flush;

    waitForInfo();

    if (error_flag) {
        std::cerr << "Failed to upload new \"" << id << "\" class."
                  << std::endl;
        std::cerr << "Error \"" << m_errorMessage << "\"."
                  << std::endl << std::flush;
        return -1;
    }

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

int AdminClient::connect(const std::string & host)
{
    tcp_socket_stream * stream = new tcp_socket_stream;
    stream->open(host, client_port_num);
    if (!stream->is_open()) {
        return -1;
    }
    cli_fd = stream->getSocket();

    ios = stream;
    return negotiate();
}

int AdminClient::connect_unix(const std::string & filename)
{
    unix_socket_stream * stream = new unix_socket_stream;
    stream->open(filename);
    if (!stream->is_open()) {
        return -1;
    }
    cli_fd = stream->getSocket();

    ios = stream;
    return negotiate();
}

int AdminClient::negotiate()
{
    // Do client negotiation with the server
    Atlas::Net::StreamConnect conn("cycmd", *ios, this);

    while (conn.getState() == Atlas::Negotiate<std::iostream>::IN_PROGRESS) {
        // conn.poll() does all the negotiation
        conn.poll();
    }

    // Check whether negotiation was successful
    if (conn.getState() == Atlas::Negotiate<std::iostream>::FAILED) {
        std::cerr << "Failed to negotiate." << std::endl;
        return -1;
    }
    // Negotiation was successful

    // Get the codec that negotiation established
    codec = conn.getCodec();

    // Create the encoder
    encoder = new Atlas::Objects::Encoder(codec);

    // Send whatever codec specific data marks the beginning of a stream
    codec->streamBegin();
    return 0;

}

void AdminClient::waitForInfo()
{
    for (int i = 0; i < 10 && !reply_flag; ++i) {
       poll();
    }
}

int AdminClient::login()
{
    Account account;
    Login l;
    error_flag = false;
    reply_flag = false;
    login_flag = true;
 
    account.setAttr("username", username);
    account.setAttr("password", password);
 
    ListType args(1,account.asObject());
 
    l.setArgs(args);
 
    encoder->streamMessage(&l);

    (*ios) << std::flush;
 
    waitForInfo();

    login_flag = false;

    if (!error_flag) {
       return 0;
    }
    return -1;
}

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
