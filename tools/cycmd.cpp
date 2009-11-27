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

#include "Flusher.h"
#include "OperationMonitor.h"

#include "common/AtlasStreamClient.h"
#include "common/log.h"
#include "common/OperationRouter.h"
#include "common/globals.h"
#include "common/sockets.h"

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
#include <sys/time.h>

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
    { "creator_create", "Use agent to create an entity", },
    { "creator_look",   "Use agent to look at an entity", },
    { "delete",         "Delete an entity from the server", },
    { "get",            "Examine any object on the server", },
    { "find_by_name",   "Find an entity with the given name", },
    { "find_by_type",   "Find an entity with the given type", },
    { "flush",          "Flush entities from the server", },
    { "help",           "Display this help", },
    { "install",        "Install a new type", },
    { "look",           "Return the current server lobby", },
    { "logout",         "Log user out of server", },
    { "monitor",        "Enable in-game op monitoring", },
    { "query",          "Synonym for \"get\" (deprecated)", },
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

/// \brief Class template for clients used to connect to and administrate
/// a cyphesis server.
class Interactive : public AtlasStreamClient,
                    virtual public sigc::trackable
{
  private:
    bool error_flag, reply_flag, login_flag, avatar_flag, server_flag;
    std::string password;
    std::string accountType;
    std::string accountId;
    std::string agentId;
    std::string agentName;
    std::string serverName;
    std::string systemType;
    std::string prompt;
    bool exit;
    AdminTask * currentTask;

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
                    avatar_flag(false), server_flag(false),
                    serverName("cyphesis"), prompt("cyphesis> "),
                    exit(false), currentTask(0)
                    { }
    ~Interactive() {
    }

    int login();
    int setup();
    void exec(const std::string & cmd, const std::string & arg);
    void loop();
    void select(bool rewrite_prompt = true);
    void getLogin();
    void runCommand(char *);
    int runTask(AdminTask * task, const std::string & arg);
    int endTask();

    void setPassword(const std::string & passwd) {
        password = passwd;
    }

    void setUsername(const std::string & uname) {
        m_username = uname;
    }

    static void gotCommand(char *);
};

void Interactive::objectArrived(const Atlas::Objects::Root & obj)
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
            send(*I);
        }

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

void Interactive::appearanceArrived(const Operation & op)
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

void Interactive::disappearanceArrived(const Operation & op)
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

void Interactive::infoArrived(const Operation & op)
{
    reply_flag = true;
    if (op->getArgs().empty()) {
        return;
    }
    const Root & ent = op->getArgs().front();
    if (login_flag) {
        std::cout << "login success" << std::endl << std::flush;
        if (ent->isDefaultId()) {
            std::cerr << "ERROR: Response to login does not contain account id"
                      << std::endl << std::flush;
            
        } else {
            accountId = ent->getId();
        }
        if (!ent->isDefaultParents()) {
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

void Interactive::errorArrived(const Operation & op)
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

void Interactive::sightArrived(const Operation & op)
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

void Interactive::soundArrived(const Operation & op)
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

void Interactive::gotCommand(char * cmd)
{
    CmdLine.emit(cmd);
}

void Interactive::runCommand(char * cmd)
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
        arg = (char *)"";
    }

    exec(cmd, arg);
}

int Interactive::runTask(AdminTask * task, const std::string & arg)
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
        send(*I);
    }
    return 0;
}

int Interactive::endTask()
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

void Interactive::loop()
{
    rl_callback_handler_install(prompt.c_str(),
                                &Interactive::gotCommand);
    rl_completion_entry_function = &completion_generator;
    CmdLine.connect(sigc::mem_fun(this, &Interactive::runCommand));
    while (!exit) {
        select();
    };
    std::cout << std::endl << std::flush;
    rl_callback_handler_remove();
}

void Interactive::select(bool rewrite_prompt)
// poll the codec if select says there is something there.
{
    fd_set infds;
    struct timeval tv;
    int retval;

    FD_ZERO(&infds);

    FD_SET(m_fd, &infds);
    FD_SET(STDIN_FILENO, &infds);

    tv.tv_sec = 0;
    tv.tv_usec = 500000;

    if (rewrite_prompt) {
        retval = ::select(m_fd+1, &infds, NULL, NULL, NULL);
    } else {
        retval = ::select(m_fd+1, &infds, NULL, NULL, &tv);
    }

    if (retval > 0) {
        if (FD_ISSET(m_fd, &infds)) {
            if (m_ios->peek() == -1) {
                std::cout << "Server disconnected" << std::endl << std::flush;
                exit = true;
            } else {
                if (rewrite_prompt) {
                    std::cout << std::endl;
                }
                m_codec->poll();
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

void Interactive::getLogin()
{
    // This needs to be re-written to hide input, so the password can be
    // secret
    std::cout << "Username: " << std::flush;
    std::cin >> m_username;
    std::cout << "Password: " << std::flush;
    std::cin >> password;
}

void Interactive::updatePrompt()
{
    std::string designation(">");
    if (!m_username.empty()) {
        prompt = m_username + "@";
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

int Interactive::login()
{
    error_flag = false;
    reply_flag = false;
    login_flag = true;
 
    AtlasStreamClient::login(m_username, password);
 
    while (!reply_flag) {
       m_codec->poll();
    }

    login_flag = false;

    if (!error_flag) {
       updatePrompt();
       return 0;
    }
    return -1;
}

int Interactive::setup()
{
    Get get;

    send(get);

    server_flag = true;

    reply_flag = true;
    while (server_flag && !error_flag) {
       m_codec->poll();
    }

    server_flag = false;
    if (!error_flag) {
       return 0;
    }
    return -1;
}

void Interactive::exec(const std::string & cmd, const std::string & arg)
{
    bool reply_expected = true;
    reply_flag = false;
    error_flag = false;

    if (cmd == "stat") {
        Get g;
        send(g);
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
            send(c);
        }
        reply_expected = false;
    } else if (cmd == "look") {
        Look l;
        if (!arg.empty()) {
            Anonymous cmap;
            cmap->setId(arg);
            l->setArgs1(cmap);
        }
        l->setFrom(accountId);
        send(l);
    } else if (cmd == "logout") {
        Logout l;
        l->setFrom(accountId);
        if (!arg.empty()) {
            Anonymous lmap;
            lmap->setId(arg);
            l->setArgs1(lmap);
            reply_expected = false;
        }
        send(l);
    } else if (cmd == "say") {
        Talk t;
        Anonymous ent;
        ent->setAttr("say", arg);
        t->setArgs1(ent);
        t->setFrom(accountId);
        send(t);
    } else if (cmd == "help" || cmd == "?") {
        reply_expected = false;
        help();
    } else if (cmd == "query") {
        Get g;

        if (!arg.empty()) {
            Anonymous cmap;
            if (::isdigit(arg[0])) {
                cmap->setObjtype("obj");
            } else {
                cmap->setObjtype("meta");
            }
            cmap->setId(arg);
            g->setArgs1(cmap);
        }
        g->setFrom(accountId);

        send(g);
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

            send(s);
        }
    } else if (cmd == "get") {
        Get g;

        if (!arg.empty()) {
            Anonymous cmap;
            if (::isdigit(arg[0])) {
                cmap->setObjtype("obj");
            } else {
                cmap->setObjtype("meta");
            }
            cmap->setId(arg);
            g->setArgs1(cmap);
        }
        g->setFrom(accountId);

        send(g);
    } else if (cmd == "monitor") {
        AdminTask * task = new OperationMonitor;
        if (runTask(task, arg) == 0) {
            Monitor m;

            m->setArgs1(Anonymous());
            m->setFrom(accountId);

            send(m);
        }

        reply_expected = false;
    } else if (cmd == "unmonitor") {
        OperationMonitor * om = dynamic_cast<OperationMonitor *>(currentTask);

        if (om != 0) {
            Monitor m;

            m->setFrom(accountId);

            send(m);

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

        send(m);
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

        send(c);
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

            send(del);

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

            send(l);

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

            send(l);

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
    } else if (cmd == "creator_create") {
        if (agentId.empty()) {
            std::cout << "Use add_agent to add an in-game agent first" << std::endl << std::flush;
            reply_expected = false;
        } else if (arg.empty()) {
            std::cout << "Please specify the type to create" << std::endl << std::flush;
            reply_expected = false;
        } else {
            Create c;

            Anonymous thing;
            thing->setParents(std::list<std::string>(1, arg));
            c->setArgs1(thing);
            c->setFrom(agentId);

            send(c);

            reply_expected = false;
        }
    } else if (cmd == "creator_look") {
        if (agentId.empty()) {
            std::cout << "Use add_agent to add an in-game agent first" << std::endl << std::flush;
            reply_expected = false;
        } else {
            Look l;

            if (!arg.empty()) {
                Anonymous cmap;
                cmap->setId(arg);
                l->setArgs1(cmap);
            }
            l->setFrom(agentId);

            send(l);
            reply_expected = true;
        }
    } else if (cmd == "cancel") {
        if (endTask() != 0) {
            std::cout << "No task currently running" << std::endl << std::flush;
        }
    } else {
        reply_expected = false;
        std::cout << cmd << ": Command not known" << std::endl << std::flush;
    }

    if (!reply_expected) { return; }
    // Wait for reply
    time_t wait_start_time = time(NULL);
    while (!reply_flag) {
       if (time(NULL) - wait_start_time > 5) {
           std::cout << cmd << ": No reply from server" << std::endl << std::flush;
           return;
       }
       select(false);
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

    Interactive bridge;

    if (server.empty()) {
        std::string localSocket;
        if (useslave != 0) {
            localSocket = slave_socket_name;
        } else {
            localSocket = client_socket_name;
        }

        std::cout << "Attempting local connection" << std::endl << std::flush;
        if (bridge.connectLocal(localSocket) == 0) {
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
