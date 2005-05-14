// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "AdminClient.h"

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;

using Atlas::Objects::Operation::Appearance;
using Atlas::Objects::Operation::Disappearance;
using Atlas::Objects::Operation::Generic;
using Atlas::Objects::Operation::Get;
using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Look;
using Atlas::Objects::Operation::Logout;
using Atlas::Objects::Operation::Talk;

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

void AdminClient::objectArrived(const Atlas::Objects::Operation::Info& o)
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

void AdminClient::objectArrived(const Atlas::Objects::Operation::Error& o)
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

int AdminClient::uploadRule(const std::string & id, const std::string & set,
                             const Atlas::Message::MapType & rule)
{
    error_flag = false;
    reply_flag = false;
    login_flag = false;

    Atlas::Objects::Operation::Get g;

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
        return -1;
    }

    std::cout << "Uploading " << id << " to server." << std::endl << std::flush;

    error_flag = false;
    reply_flag = false;
    login_flag = false;

    Atlas::Objects::Operation::Set s;

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
    return 0;
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
    Atlas::Objects::Entity::Account account;
    Atlas::Objects::Operation::Login l;
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
