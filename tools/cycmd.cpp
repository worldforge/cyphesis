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

// $Id: cycmd.cpp,v 1.113 2007-11-14 22:40:18 alriddoch Exp $

/// \page cycmd_index
///
/// \section Introduction
///
/// cycmd is a commandline tool to administrate the running server. For
/// information on the commands available, please see the unix manual page.
/// The manual page is generated from docbook sources, so can
/// also be converted into other formats.
///
/// The majority of the functionality is encapsulated by the Interactive
/// class template.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "common/log.h"
#include "common/types.h"
#include "common/globals.h"

#include "common/Tick.h"

#include <Atlas/Objects/Encoder.h>
#include <Atlas/Net/Stream.h>
#include <Atlas/Objects/Decoder.h>
#include <Atlas/Codec.h>
#include <Atlas/Objects/Entity.h>
#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>

#include <varconf/config.h>

#include "common/Monitor.h"
#include "common/Connect.h"
#include "common/compose.hpp"

#include <skstream/skstream_unix.h>

#include <sigc++/connection.h>

#ifndef READLINE_CXX_SANE   // defined in config.h
extern "C" {
#endif
#define USE_VARARGS
#define PREFER_STDARG
#include <readline/readline.h>
#include <readline/history.h>
#ifndef READLINE_CXX_SANE
}
#endif

#include <iostream>
#include <algorithm>

#include <cstdio>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;

using Atlas::Objects::Root;

using Atlas::Objects::Operation::Appearance;
using Atlas::Objects::Operation::Create;
using Atlas::Objects::Operation::Delete;
using Atlas::Objects::Operation::Disappearance;
using Atlas::Objects::Operation::Get;
using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Look;
using Atlas::Objects::Operation::Login;
using Atlas::Objects::Operation::Logout;
using Atlas::Objects::Operation::Talk;
using Atlas::Objects::Operation::Tick;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;

using Atlas::Objects::smart_dynamic_cast;

using Atlas::Objects::Operation::Monitor;
using Atlas::Objects::Operation::Connect;

/// \brief Entry in the global command table for cycmd
struct command {
    const char * cmd_string;
    const char * cmd_description;
};

struct command commands[] = {
    { "add_agent",      "Create an in-game agent", },
    { "connect",        "Connect server to a peer", },
    { "cancel",         "Cancel the current admin task", },
    { "delete",         "Delete an entity from the server", },
    { "get",            "Examine a class on the server", },
    { "find_by_name",   "Find an entity with the given name", },
    { "find_by_type",   "Find an entity with the given type", },
    { "flush",          "Flush entities from the server", },
    { "help",           "Display this help", },
    { "install",        "Install a new type", },
    { "look",           "Return the current server lobby", },
    { "logout",         "Log user out of server", },
    { "monitor",        "Enable in-game op monitoring", },
    { "query",          "Examine an object on the server", },
    { "reload",         "Reload the script for a type", },
    { "stat",           "Return current server status", },
    { "unmonitor",      "Disable in-game op monitoring", },
    { NULL,             "Guard", }
};


static void help()
{
    size_t max_length = 0;

    for (struct command * I = &commands[0]; I->cmd_string != NULL; ++I) {
       max_length = std::max(max_length, strlen(I->cmd_string));
    }
    max_length += 2;

    std::cout << "Cyphesis commands:" << std::endl << std::endl;

    for (struct command * I = &commands[0]; I->cmd_string != NULL; ++I) {
        std::cout << "    " << I->cmd_string
                  << std::string(max_length - strlen(I->cmd_string), ' ')
                  << I->cmd_description << std::endl;
    }
    std::cout << std::endl << std::flush;
}

/// \brief Base class for admin tasks which run for some time.
///
/// Typical tasks that inherit from this class are ones which last for
/// non trivial time and will typically require the user to be able to
/// continue issuing commands.
class AdminTask {
  protected:
    /// \brief Flag that indicates when the task is complete
    bool m_complete;

    ///\brief AdminTask constructor
    AdminTask() : m_complete(false) { }
  public:
    virtual ~AdminTask() { }

    /// \brief Set up the task processing user arguments
    virtual void setup(const std::string & arg, OpVector &) = 0;
    /// \brief Handle an operation from the server
    virtual void operation(const Operation &, OpVector &) = 0;

    /// \brief Check whether the task is complete
    ///
    /// @return true if the task is complete, false otherwise
    bool isComplete() const { return m_complete; }
};

/// \brief Task class for flushing the server of character entities
class Flusher : public AdminTask {
  protected:
    const std::string agentId;
    std::string type;
  public:
    explicit Flusher(const std::string & agent_id) : agentId(agent_id) { }

    void setup(const std::string & arg, OpVector & ret) {
        type = arg;

        // Send a look to search by type.
        Look l;

        Anonymous lmap;
        lmap->setParents(std::list<std::string>(1, type));
        l->setArgs1(lmap);
        l->setFrom(agentId);

        ret.push_back(l);
    }

    void operation(const Operation & op, OpVector & res) {
        if (op->getClassNo() == Atlas::Objects::Operation::SIGHT_NO) {
            // We have a sight op, check if its the sight of an entity we
            // want to delete.
            const std::vector<Root> & args = op->getArgs();
            if (args.empty()) {
                std::cerr << "Got empty sight" << std::endl << std::flush;
                return;
            }
            const Root & arg = args.front();
            assert(arg.isValid());
            RootEntity sight_ent = smart_dynamic_cast<RootEntity>(arg);
            if (!sight_ent.isValid()) {
                return;
            }
            if (!sight_ent->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
                std::cerr << "Got sight no ID" << std::endl << std::flush;
                return;
            }
            if (!sight_ent->hasAttrFlag(Atlas::Objects::PARENTS_FLAG)) {
                std::cerr << "Got sight no PARENTS" << std::endl << std::flush;
                return;
            }
            if (sight_ent->getParents().empty() ||
                sight_ent->getParents().front() != type) {
                return;
            }
            const std::string & id = sight_ent->getId();

            std::cout << "Deleting: " << type << "(" << id << ")"
                      << std::endl << std::flush;

            // Send a delete to the entity we have seen.
            Delete d;

            Anonymous dmap;
            dmap->setId(id);
            d->setArgs1(dmap);
            d->setFrom(agentId);
            d->setTo(id);

            res.push_back(d);

            // Send a tick for a short time in the future so that
            // we can look again once this entity is definitly gone.
            Tick t;

            Anonymous tick_arg;
            tick_arg->setName("flusher");

            t->setFrom(agentId);
            t->setTo(agentId);
            t->setFutureSeconds(0.1);
            t->setArgs1(tick_arg);

            res.push_back(t);
        } else if (op->getParents().front() == "tick") {
            // We have a tick op, check if its the one we sent ourselves
            // to schedule the next look.
            if (op->getArgs().empty() ||
                op->getArgs().front()->getName() != "flusher") {
                std::cout << "Not for us" << std::endl << std::flush;
                return;
            }

            // Send another look by type.
            Look l;

            Anonymous lmap;
            lmap->setParents(std::list<std::string>(1, type));
            l->setArgs1(lmap);
            l->setFrom(agentId);

            res.push_back(l);
        } else if (op->getParents().front() == "unseen") {
            // We have an unseen op, which signals our last look returned
            // no results.
            m_complete = true;
        }
    }
};

/// \brief Task class for monitoring all in-game operations occuring.
class OperationMonitor : public AdminTask {
  protected:
    int op_count;
    int start_time;
  public:
    int count() {
        return op_count;
    }

    int startTime() {
        return start_time;
    }

    virtual void setup(const std::string & arg, OpVector &) {
        struct timeval tv;

        gettimeofday(&tv, NULL);
        start_time = tv.tv_sec;
        op_count = 0;
    }

    virtual void operation(const Operation & op, OpVector &) {
        ++op_count;
        std::cout << op->getParents().front() << "(from=\"" << op->getFrom()
                  << "\",to=\"" << op->getTo() << "\")"
                  << std::endl << std::flush;
    }
};

/// \brief Class template for clients used to connect to and administrate
/// a cyphesis server.
template <class Stream>
class Interactive : public Atlas::Objects::ObjectsDecoder,
                    virtual public sigc::trackable
{
  private:
    bool error_flag, reply_flag, login_flag, avatar_flag, server_flag;
    int cli_fd;
    Atlas::Objects::ObjectsEncoder * encoder;
    Atlas::Codec * codec;
    Stream ios;
    std::string password;
    std::string username;
    std::string accountType;
    std::string accountId;
    std::string agentId;
    std::string agentName;
    std::string serverName;
    std::string systemType;
    std::string prompt;
    bool exit;
    AdminTask * currentTask;

    void output(const Element & item, int depth = 0);
  protected:

    void objectArrived(const Atlas::Objects::Root &);

    void appearanceArrived(const Operation &);
    void disappearanceArrived(const Operation &);
    void infoArrived(const Operation &);
    void errorArrived(const Operation &);
    void sightArrived(const Operation &);
    void soundArrived(const Operation &);

    int negotiate();
    void updatePrompt();
  public:
    Interactive() : error_flag(false), reply_flag(false), login_flag(false),
                    avatar_flag(false), server_flag(false), encoder(0),
                    codec(0), serverName("cyphesis"), prompt("cyphesis> "),
                    exit(false), currentTask(0)
                    { }
    ~Interactive() {
        if (encoder != 0) {
            delete encoder;
        }
        if (codec != 0) {
            delete codec;
        }
    }

    void send(const Operation &);
    int connect(const std::string & host);
    int login();
    int setup();
    void exec(const std::string & cmd, const std::string & arg);
    void loop();
    void poll(bool rewrite_prompt = true);
    void getLogin();
    void runCommand(char *);
    int runTask(AdminTask * task, const std::string & arg);
    int endTask();

    void setPassword(const std::string & passwd) {
        password = passwd;
    }

    void setUsername(const std::string & uname) {
        username = uname;
    }

    static void gotCommand(char *);
};

template <class Stream>
void Interactive<Stream>::output(const Element & item, int depth)
{
    switch (item.getType()) {
        case Element::TYPE_INT:
            std::cout << item.Int();
            break;
        case Element::TYPE_FLOAT:
            std::cout << item.Float();
            break;
        case Element::TYPE_STRING:
            std::cout << "\"" << item.String() << "\"";
            break;
        case Element::TYPE_LIST:
            {
                std::cout << "[ ";
                ListType::const_iterator I = item.List().begin();
                ListType::const_iterator Iend = item.List().end();
                for(; I != Iend; ++I) {
                    output(*I, depth);
                    std::cout << " ";
                }
                std::cout << "]";
            }
            break;
        case Element::TYPE_MAP:
            {
                std::cout << "{" << std::endl << std::flush;
                MapType::const_iterator I = item.Map().begin();
                MapType::const_iterator Iend = item.Map().end();
                for(; I != Iend; ++I) {
                    std::cout << std::string((depth + 1) * 4, ' ') << I->first << ": ";
                    output(I->second, depth + 1);
                    std::cout << std::endl;
                }
                std::cout << std::string(depth * 4, ' ') << "}";
            }
            break;
        default:
            std::cout << "(\?\?\?)";
            break;
    }
}

template <class Stream>
void Interactive<Stream>::objectArrived(const Atlas::Objects::Root & obj)
{
    Operation op = Atlas::Objects::smart_dynamic_cast<Operation>(obj);
    if (!op.isValid()) {
        std::cerr << "Non op object received from client" << std::endl << std::flush;
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

    if (currentTask != 0) {
        OpVector res;
        currentTask->operation(op, res);
        OpVector::const_iterator Iend = res.end();
        for (OpVector::const_iterator I = res.begin(); I != Iend; ++I) {
            encoder->streamObjectsMessage(*I);
        }

        ios << std::flush;

        if (currentTask->isComplete()) {
            delete currentTask;
            currentTask = 0;
        }
    }

    switch (op->getClassNo()) {
        case Atlas::Objects::Operation::APPEARANCE_NO:
            appearanceArrived(op);
            break;
        case Atlas::Objects::Operation::DISAPPEARANCE_NO:
            disappearanceArrived(op);
            break;
        case Atlas::Objects::Operation::INFO_NO:
            infoArrived(op);
            break;
        case Atlas::Objects::Operation::ERROR_NO:
            errorArrived(op);
            break;
        case Atlas::Objects::Operation::SIGHT_NO:
            sightArrived(op);
            break;
        case Atlas::Objects::Operation::SOUND_NO:
            soundArrived(op);
            break;
        default:
            break;
    }
}

template <class Stream>
void Interactive<Stream>::appearanceArrived(const Operation & op)
{
    if (accountId.empty()) {
        return;
    }
    if (accountId != op->getTo()) {
        // This is an IG op we are monitoring
        return;
    }
    if (op->getArgs().empty()) {
        return;
    }
    RootEntity ent = smart_dynamic_cast<RootEntity>(op->getArgs().front());
    if (!ent.isValid()) {
        std::cerr << "Got Appearance of non-entity" << std::endl << std::flush;
        return;
    }
    if (!ent->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        std::cerr << "Got Appearance of non-string ID" << std::endl << std::flush;
        return;
    }
    const std::string & id = ent->getId();
    std::cout << "Appearance(id: " << id << ")";
    if (!ent->hasAttrFlag(Atlas::Objects::Entity::LOC_FLAG)) {
        std::cout << std::endl << std::flush;
        return;
    }
    const std::string & loc = ent->getLoc();
    std::cout << " in " << loc << std::endl;
    if (loc == "lobby") {
        std::cout << id << " has logged in." << std::endl;
    }
    std::cout << std::flush;
}

template <class Stream>
void Interactive<Stream>::disappearanceArrived(const Operation & op)
{
    if (accountId.empty()) {
        return;
    }
    if (accountId != op->getTo()) {
        // This is an IG op we are monitoring
        return;
    }
    if (op->getArgs().empty()) {
        return;
    }
    RootEntity ent = smart_dynamic_cast<RootEntity>(op->getArgs().front());
    if (!ent.isValid()) {
        std::cerr << "Got Disappearance of non-entity" << std::endl << std::flush;
        return;
    }
    if (!ent->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        std::cerr << "Got Disappearance of non-string ID" << std::endl << std::flush;
        return;
    }
    const std::string & id = ent->getId();
    std::cout << "Disappearance(id: " << id << ")";
    if (!ent->hasAttrFlag(Atlas::Objects::Entity::LOC_FLAG)) {
        std::cout << std::endl << std::flush;
        return;
    }
    const std::string & loc = ent->getLoc();
    std::cout << " in " << loc << std::endl;
    if (loc == "lobby") {
        std::cout << id << " has logged out." << std::endl;
    }
    std::cout << std::flush;
}

template <class Stream>
void Interactive<Stream>::infoArrived(const Operation & op)
{
    reply_flag = true;
    if (op->getArgs().empty()) {
        return;
    }
    const Root & ent = op->getArgs().front();
    if (login_flag) {
        std::cout << "login success" << std::endl << std::flush;
        if (!ent->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
            std::cerr << "ERROR: Response to login does not contain account id"
                      << std::endl << std::flush;
            
        } else {
            accountId = ent->getId();
        }
        if (ent->hasAttrFlag(Atlas::Objects::PARENTS_FLAG)) {
            const std::list<std::string> & parents = ent->getParents();
            if (!parents.empty()) {
                accountType = parents.front();
            }
        }
    } else if (avatar_flag) {
        std::cout << "Create agent success" << std::endl << std::flush;
        if (!ent->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
            std::cerr << "ERROR: Response to agent create does not contain agent id"
                      << std::endl << std::flush;
            
        } else {
            agentId = ent->getId();
            avatar_flag = false;
        }
    } else if (server_flag) {
        std::cout << "Server query success" << std::endl << std::flush;
        if (!ent->isDefaultName()) {
            serverName = ent->getName();
            std::string::size_type p = serverName.find(".");
            if (p != std::string::npos) {
                serverName = serverName.substr(0, p);
            }
            updatePrompt();
        }
        Element raw_attr;
        if (ent->copyAttr("server", raw_attr) == 0) {
            if (raw_attr.isString()) {
                systemType = raw_attr.String();
                updatePrompt();
            }
        }
        server_flag = false;
    } else {
        std::cout << "Info(" << std::endl;
        MapType entmap = ent->asMessage();
        MapType::const_iterator Iend = entmap.end();
        for (MapType::const_iterator I = entmap.begin(); I != Iend; ++I) {
            const Element & item = I->second;
            std::cout << "    " << I->first << ": ";
            output(item, 1);
            std::cout << std::endl;
        }
        std::cout << ")" << std::endl << std::flush;
        // Display results of command
    }
}

template <class Stream>
void Interactive<Stream>::errorArrived(const Operation & op)
{
    reply_flag = true;
    error_flag = true;
    std::cout << "Error(";
    const std::vector<Root> & args = op->getArgs();
    const Root & arg = args.front();
    Element message_attr;
    if (arg->copyAttr("message", message_attr) == 0 && message_attr.isString()) {
        std::cout << message_attr.asString();
    }
    std::cout << ")" << std::endl << std::flush;
}

template <class Stream>
void Interactive<Stream>::sightArrived(const Operation & op)
{
    if (accountId.empty()) {
        return;
    }
    if (accountId != op->getTo() && agentId != op->getTo()) {
        // This is an IG op we are monitoring
        return;
    }
    reply_flag = true;
    std::cout << "Sight(" << std::endl;
    const MapType & ent = op->getArgs().front()->asMessage();
    MapType::const_iterator Iend = ent.end();
    for (MapType::const_iterator I = ent.begin(); I != Iend; ++I) {
        const Element & item = I->second;
        std::cout << "      " << I->first << ":";
        output(item, 1);
        std::cout << std::endl;
    }
    std::cout << ")" << std::endl << std::flush;
}

template <class Stream>
void Interactive<Stream>::soundArrived(const Operation & op)
{
    if (accountId.empty()) {
        return;
    }
    if (accountId != op->getTo()) {
        // This is an IG op we are monitoring
        return;
    }
    reply_flag = true;
    const MapType & arg = op->getArgs().front()->asMessage();
    MapType::const_iterator I = arg.find("from");
    if (I == arg.end() || !I->second.isString()) {
        std::cout << "Sound arg has no from" << std::endl << std::flush;
        return;
    }
    const std::string & from = I->second.asString();
    I = arg.find("args");
    if (I == arg.end() || !I->second.isList()
                       || I->second.asList().empty()
                       || !I->second.asList().front().isMap()) {
        std::cout << "Sound arg has no args" << std::endl << std::flush;
        return;
    }
    const MapType & ent = I->second.asList().front().asMap();
    I = ent.find("say");
    if (I == ent.end() || !I->second.isString()) {
        std::cout << "Sound arg arg has no say" << std::endl << std::flush;
        return;
    }
    const std::string & say = I->second.asString();
    std::cout << "[" << from << "] " << say
              << std::endl << std::flush;
}

sigc::signal<void, char *> CmdLine;

template <class Stream>
void Interactive<Stream>::gotCommand(char * cmd)
{
    CmdLine.emit(cmd);
}

template <class Stream>
void Interactive<Stream>::runCommand(char * cmd)
{
    if (cmd == NULL) {
        exit = true;
        std::cout << std::endl << std::flush;
        return;
    }

    if (*cmd == 0) {
        free(cmd);
        return;
    }

    add_history(cmd);

    char * arg = strchr(cmd, ' ');
    if (arg != NULL) {
        *arg++ = 0;
        int len = strlen(arg);
        while (len > 0 && arg[--len] == ' ') { arg[len] = 0; }
    } else {
        arg = "";
    }

    exec(cmd, arg);
}

template <class Stream>
int Interactive<Stream>::runTask(AdminTask * task, const std::string & arg)
{
    assert(task != 0);

    if (currentTask != 0) {
        std::cout << "Busy" << std::endl << std::flush;
        return -1;
    }

    currentTask = task;

    OpVector res;

    currentTask->setup(arg, res);

    OpVector::const_iterator Iend = res.end();
    for (OpVector::const_iterator I = res.begin(); I != Iend; ++I) {
        encoder->streamObjectsMessage(*I);
    }
    return 0;
}

template <class Stream>
int Interactive<Stream>::endTask()
{
    if (currentTask == 0) {
        return -1;
    }
    delete currentTask;
    currentTask = 0;
    return 0;
}

int completion_iterator = 0;

char * completion_generator(const char * text, int state)
{
    if (state == 0) {
        completion_iterator = 0;
    }
    for (int i = completion_iterator; commands[i].cmd_string != 0; ++i) {
        if (strncmp(text, commands[i].cmd_string, strlen(text)) == 0) {
            completion_iterator = i + 1;
            return strdup(commands[i].cmd_string);
        }
    }
    return 0;
}

template <class Stream>
void Interactive<Stream>::loop()
{
    rl_callback_handler_install(prompt.c_str(),
                                &Interactive<Stream>::gotCommand);
    rl_completion_entry_function = &completion_generator;
    CmdLine.connect(sigc::mem_fun(this, &Interactive<Stream>::runCommand));
    while (!exit) {
        poll();
    };
    std::cout << std::endl << std::flush;
    rl_callback_handler_remove();
}

template <class Stream>
void Interactive<Stream>::poll(bool rewrite_prompt)
// poll the codec if select says there is something there.
{
    fd_set infds;
    struct timeval tv;
    int retval;

    FD_ZERO(&infds);

    FD_SET(cli_fd, &infds);
    FD_SET(STDIN_FILENO, &infds);

    tv.tv_sec = 0;
    tv.tv_usec = 500000;

    if (rewrite_prompt) {
        retval = select(cli_fd+1, &infds, NULL, NULL, NULL);
    } else {
        retval = select(cli_fd+1, &infds, NULL, NULL, &tv);
    }

    if (retval > 0) {
        if (FD_ISSET(cli_fd, &infds)) {
            if (ios.peek() == -1) {
                std::cout << "Server disconnected" << std::endl << std::flush;
                exit = true;
            } else {
                if (rewrite_prompt) {
                    std::cout << std::endl;
                }
                codec->poll();
                if (rewrite_prompt) {
                    rl_forced_update_display();
                }
            }
        }
        if (FD_ISSET(STDIN_FILENO, &infds)) {
            rl_callback_read_char();
        }
    }
}

template <class Stream>
void Interactive<Stream>::getLogin()
{
    // This needs to be re-written to hide input, so the password can be
    // secret
    std::cout << "Username: " << std::flush;
    std::cin >> username;
    std::cout << "Password: " << std::flush;
    std::cin >> password;
}

template<>
int Interactive<tcp_socket_stream>::connect(const std::string & host)
{
    std::cout << "Connecting... " << std::flush;
    ios.open(host, client_port_num);
    if (!ios.is_open()) {
        std::cout << "failed." << std::endl << std::flush;
        return -1;
    }
    std::cout << "done." << std::endl << std::flush;
    cli_fd = ios.getSocket();

    return negotiate();
}

template<>
int Interactive<unix_socket_stream>::connect(const std::string & filename)
{
    std::cout << "Connecting... " << std::flush;
    ios.open(filename);
    if (!ios.is_open()) {
        std::cout << "failed." << std::endl << std::flush;
        return -1;
    }
    std::cout << "done." << std::endl << std::flush;
    cli_fd = ios.getSocket();

    return negotiate();
}

template <class Stream>
int Interactive<Stream>::negotiate()
{
    // Do client negotiation with the server
    Atlas::Net::StreamConnect conn("cycmd", ios);

    std::cout << "Negotiating... " << std::flush;
    while (conn.getState() == Atlas::Negotiate::IN_PROGRESS) {
        // conn.poll() does all the negotiation
        conn.poll();
    }
    std::cout << "done." << std::endl << std::flush;

    // Check whether negotiation was successful
    if (conn.getState() == Atlas::Negotiate::FAILED) {
        std::cerr << "Failed to negotiate." << std::endl;
        return -1;
    }
    // Negotiation was successful

    // Get the codec that negotiation established
    codec = conn.getCodec(*this);

    // Create the encoder
    encoder = new Atlas::Objects::ObjectsEncoder(*codec);

    // Send whatever codec specific data marks the beginning of a stream
    codec->streamBegin();

    return 0;

}

template <class Stream>
void Interactive<Stream>::updatePrompt()
{
    std::string designation(">");
    if (!username.empty()) {
        prompt = username + "@";
        if (accountType == "admin") {
            designation = "#";
        } else {
            designation = "$";
        }
    } else {
        prompt = "";
    }
    prompt += serverName;
    prompt += " ";
    prompt += systemType;
    prompt += designation;
    prompt += " ";
    rl_set_prompt(prompt.c_str());
}

template <class Stream>
int Interactive<Stream>::login()
{
    Atlas::Objects::Entity::Account account;
    Login l;
    error_flag = false;
    reply_flag = false;
    login_flag = true;
 
    account->setAttr("username", username);
    account->setAttr("password", password);
 
    l->setArgs1(account);
 
    encoder->streamObjectsMessage(l);

    ios << std::flush;
 
    while (!reply_flag) {
       codec->poll();
    }

    login_flag = false;

    if (!error_flag) {
       updatePrompt();
       return 0;
    }
    return -1;
}

template <class Stream>
int Interactive<Stream>::setup()
{
    Get get;

    encoder->streamObjectsMessage(get);
    ios << std::flush;
    server_flag = true;

    reply_flag = true;
    while (server_flag && !error_flag) {
       codec->poll();
    }

    server_flag = false;
    if (!error_flag) {
       return 0;
    }
    return -1;
}

template <class Stream>
void Interactive<Stream>::exec(const std::string & cmd, const std::string & arg)
{
    bool reply_expected = true;
    reply_flag = false;
    error_flag = false;

    if (cmd == "stat") {
        Get g;
        encoder->streamObjectsMessage(g);
    } else if (cmd == "install") {
        size_t space = arg.find(' ');
        if (space == std::string::npos || space >= (arg.size() - 1)) {
            std::cout << "usage: install <type id> <parent id>"
                      << std::endl << std::flush;
        } else {
            Create c;
            c->setFrom(accountId);
            Anonymous ent;
            ent->setId(std::string(arg, 0, space));
            ent->setObjtype("class");
            ent->setParents(std::list<std::string>(1, std::string(arg, space + 1)));
            c->setArgs1(ent);
            encoder->streamObjectsMessage(c);
        }
        reply_expected = false;
    } else if (cmd == "look") {
        Look l;
        l->setFrom(accountId);
        encoder->streamObjectsMessage(l);
    } else if (cmd == "logout") {
        Logout l;
        l->setFrom(accountId);
        if (!arg.empty()) {
            Anonymous lmap;
            lmap->setId(arg);
            l->setArgs1(lmap);
            reply_expected = false;
        }
        encoder->streamObjectsMessage(l);
    } else if (cmd == "say") {
        Talk t;
        Anonymous ent;
        ent->setAttr("say", arg);
        t->setArgs1(ent);
        t->setFrom(accountId);
        encoder->streamObjectsMessage(t);
    } else if (cmd == "help" || cmd == "?") {
        reply_expected = false;
        help();
    } else if (cmd == "query") {
        Get g;

        Anonymous cmap;
        cmap->setObjtype("obj");
        if (!arg.empty()) {
            cmap->setId(arg);
        }
        g->setArgs1(cmap);
        g->setFrom(accountId);

        encoder->streamObjectsMessage(g);
    } else if (cmd == "reload") {
        if (arg.empty()) {
            reply_expected = false;
            std::cout << "reload: Argument required" << std::endl << std::flush;
        } else {
            Set s;

            Anonymous tmap;
            tmap->setObjtype("class");
            tmap->setId(arg);
            s->setArgs1(tmap);
            s->setFrom(accountId);

            encoder->streamObjectsMessage(s);
        }
    } else if (cmd == "get") {
        Get g;

        Anonymous cmap;
        cmap->setObjtype("class");
        if (!arg.empty()) {
            cmap->setId(arg);
        }
        g->setArgs1(cmap);
        g->setFrom(accountId);

        encoder->streamObjectsMessage(g);
    } else if (cmd == "monitor") {
        AdminTask * task = new OperationMonitor;
        if (runTask(task, arg) == 0) {
            Monitor m;

            m->setArgs1(Anonymous());
            m->setFrom(accountId);

            encoder->streamObjectsMessage(m);
        }

        reply_expected = false;
    } else if (cmd == "unmonitor") {
        OperationMonitor * om = dynamic_cast<OperationMonitor *>(currentTask);

        if (om != 0) {
            Monitor m;

            m->setFrom(accountId);

            encoder->streamObjectsMessage(m);

            reply_expected = false;

            struct timeval tv;
            gettimeofday(&tv, NULL);
            int monitor_time = tv.tv_sec - om->startTime();

            std::cout << om->count() << " operations monitored in "
                      << monitor_time << " seconds = "
                      << om->count() / monitor_time
                      << " operations per second"
                      << std::endl << std::flush;

            endTask();
        }
    } else if (cmd == "connect") {
        reply_expected = false;
        Connect m;

        Anonymous cmap;
        cmap->setAttr("hostname", arg);
        m->setArgs1(cmap);
        m->setFrom(accountId);

        encoder->streamObjectsMessage(m);
    } else if (cmd == "add_agent") {
        std::string agent_type("creator");

        if (!arg.empty()) {
            agent_type = arg;
        }
        
        Create c;

        Anonymous cmap;
        cmap->setParents(std::list<std::string>(1, agent_type));
        cmap->setName("cycmd agent");
        cmap->setObjtype("obj");
        c->setArgs1(cmap);
        c->setFrom(accountId);

        avatar_flag = true;

        encoder->streamObjectsMessage(c);
    } else if (cmd == "delete") {
        if (agentId.empty()) {
            std::cout << "Use add_agent to add an in-game agent first" << std::endl << std::flush;
            reply_expected = false;
        } else if (arg.empty()) {
            std::cout << "Please specify the entity to delete" << std::endl << std::flush;
            reply_expected = false;
        } else {
            Delete del;

            Anonymous del_arg;
            del_arg->setId(arg);
            del->setArgs1(del_arg);
            del->setFrom(agentId);
            del->setTo(arg);

            encoder->streamObjectsMessage(del);

            reply_expected = false;
        }
    } else if (cmd == "find_by_name") {
        if (agentId.empty()) {
            std::cout << "Use add_agent to add an in-game agent first" << std::endl << std::flush;
            reply_expected = false;
        } else if (arg.empty()) {
            std::cout << "Please specify the name to search for" << std::endl << std::flush;
            reply_expected = false;
        } else {
            Look l;

            Anonymous lmap;
            lmap->setName(arg);
            l->setArgs1(lmap);
            l->setFrom(agentId);

            encoder->streamObjectsMessage(l);

            reply_expected = false;
        }
    } else if (cmd == "find_by_type") {
        if (agentId.empty()) {
            std::cout << "Use add_agent to add an in-game agent first" << std::endl << std::flush;
            reply_expected = false;
        } else if (arg.empty()) {
            std::cout << "Please specify the type to search for" << std::endl << std::flush;
            reply_expected = false;
        } else {
            Look l;

            Anonymous lmap;
            lmap->setParents(std::list<std::string>(1, arg));
            l->setArgs1(lmap);
            l->setFrom(agentId);

            encoder->streamObjectsMessage(l);

            reply_expected = false;
        }
    } else if (cmd == "flush") {
        if (agentId.empty()) {
            std::cout << "Use add_agent to add an in-game agent first" << std::endl << std::flush;
            reply_expected = false;
        } else if (arg.empty()) {
            std::cout << "Please specify the type to flush" << std::endl << std::flush;
            reply_expected = false;
        } else {
            AdminTask * task = new Flusher(agentId);
            runTask(task, arg);
            reply_expected = false;
        }
    } else if (cmd == "cancel") {
        if (endTask() != 0) {
            std::cout << "No task currently running" << std::endl << std::flush;
        }
    } else {
        reply_expected = false;
        std::cout << cmd << ": Command not known" << std::endl << std::flush;
    }

    ios << std::flush;

    if (!reply_expected) { return; }
    // Wait for reply
    time_t wait_start_time = time(NULL);
    while (!reply_flag) {
       if (time(NULL) - wait_start_time > 5) {
           std::cout << cmd << ": No reply from server" << std::endl << std::flush;
           return;
       }
       poll(false);
    }
}

static void usage(char * prg)
{
    std::cerr << "usage: " << prg << " [ cmd [ server ] ]" << std::endl << std::flush;
}

int main(int argc, char ** argv)
{
    int config_status = loadConfig(argc, argv, USAGE_CYCMD); 
    if (config_status < 0) {
        if (config_status == CONFIG_VERSION) {
            reportVersion(argv[0]);
            return 0;
        } else if (config_status == CONFIG_HELP) {
            showUsage(argv[0], USAGE_CYCMD, "[ cmd [ server ] ]");
            return 0;
        } else if (config_status != CONFIG_ERROR) {
            log(ERROR, "Unknown error reading configuration.");
        }
        // Fatal error loading config file
        return 1;
    }

    int optind = config_status;

    std::string server;
    readConfigItem("client", "serverhost", server);

    int useslave = 0;
    readConfigItem("client", "useslave", useslave);

    bool interactive = true;
    std::string cmd;
    if (optind < argc) {
        if ((argc - optind) == 2) {
            server = argv[optind + 1];
        } else if ((argc - optind) > 2) {
            usage(argv[0]);
            return 1;
        }
        cmd = argv[optind];
        interactive = false;
    }

    if (server.empty()) {
        std::string localSocket = var_directory + "/tmp/";
        if (useslave != 0) {
            localSocket += slave_socket_name;
        } else {
            localSocket += client_socket_name;
        }

        std::cout << "Attempting local connection" << std::endl << std::flush;
        Interactive<unix_socket_stream> bridge;
        if (bridge.connect(localSocket) == 0) {
            bridge.setUsername("admin");

            bridge.setup();
            std::cout << "Logging in... " << std::flush;
            if (bridge.login() != 0) {
                std::cout << "failed." << std::endl << std::flush;
                bridge.getLogin();

                std::cout << "Logging in... " << std::flush;
                if (!bridge.login()) {
                    std::cout << "failed." << std::endl << std::flush;
                    return 1;
                }
            }
            std::cout << "done." << std::endl << std::flush;
            if (!interactive) {
                bridge.exec(cmd, "");
                return 0;
            } else {
                bridge.loop();
            }
            return 0;
        }
        server = "localhost";
    }
    
    std::cerr << "Attempting tcp connection" << std::endl << std::flush;

    Interactive<tcp_socket_stream> bridge;

    if (bridge.connect(server) != 0) {
        return 1;
    }
    bridge.setup();
    if (!interactive) {
        std::cerr << "WARNING: No login details available for remote host"
                  << std::endl
                  << "WARNING: Attempting command without logging in"
                  << std::endl << std::flush;
    } else {
        bridge.getLogin();
        std::cout << "Logging in... " << std::flush;
        if (bridge.login() != 0) {
            std::cout << "failed." << std::endl << std::flush;
            return 1;
        }
        std::cout << "done." << std::endl << std::flush;
    }
    if (!interactive) {
        bridge.exec(cmd, "");
    } else {
        bridge.loop();
    }
    delete global_conf;
    return 0;
}
