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


#ifdef HAVE_CONFIG_H
#endif

#include "Interactive.h"

#include "Flusher.h"
#include "OperationMonitor.h"
#include "EntityExporter.h"
#include "EntityImporter.h"

#include "tools/AccountContext.h"
#include "tools/ConnectionContext.h"

#include "common/operations/Tick.h"

#include <Atlas/Objects/Encoder.h>
#include <Atlas/Net/Stream.h>
#include <Atlas/Objects/Entity.h>

#include "common/operations/Monitor.h"
#include "common/operations/Connect.h"
#include "common/compose.hpp"
#include "common/utils.h"

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

using std::shared_ptr;

/// \brief Entry in the global command table for cycmd
struct command {
    const char * cmd_string;
    const char * cmd_description;
    int (Interactive::*cmd_method)(struct command *, const std::string &);
    int cmd_flags;
    const char * cmd_longhelp;
};

static const unsigned int CMD_DEFAULT = 0u;
static const unsigned int CMD_CONTEXT = 1u << 1u;

struct command commands[] = {
    { "add_agent",      "Create an in-game agent",
      &Interactive::commandUnknown, CMD_DEFAULT, nullptr, },
    { "cancel",         "Cancel the current admin task",
      &Interactive::commandUnknown, CMD_DEFAULT, nullptr, },
    { "connect",        "Connect server to a peer",
      &Interactive::commandUnknown, CMD_DEFAULT, nullptr, },
    { "create",         "Use account to create server objects",
      &Interactive::commandUnknown, CMD_DEFAULT, nullptr, },
    { "delete",         "Delete an entity from the server",
      &Interactive::commandUnknown, CMD_DEFAULT, nullptr, },
    { "dump",           "Write a copy of the world to an Atlas file",
      &Interactive::commandUnknown, CMD_DEFAULT, nullptr, },
    { "get",            "Examine any object on the server",
      &Interactive::commandUnknown, CMD_DEFAULT, nullptr, },
    { "find_by_name",   "Find an entity with the given name",
      &Interactive::commandUnknown, CMD_DEFAULT, nullptr, },
    { "find_by_type",   "Find an entity with the given type",
      &Interactive::commandUnknown, CMD_DEFAULT, nullptr, },
    { "flush",          "Flush entities from the server",
      &Interactive::commandUnknown, CMD_DEFAULT, nullptr, },
    { "help",           "Display this help",
      &Interactive::commandUnknown, CMD_DEFAULT, nullptr, },
    { "install",        "Install a new type",
      &Interactive::commandUnknown, CMD_DEFAULT, nullptr, },
    { "login",          "Log into a peer server",
      &Interactive::commandUnknown, CMD_DEFAULT, nullptr, },
    { "restore",        "Read world data from file and add it to the world",
      &Interactive::commandUnknown, CMD_DEFAULT, nullptr, },
    { "look",           "Return the current server lobby",
      &Interactive::commandUnknown, CMD_DEFAULT, nullptr, },
    { "logout",         "Log user out of server",
      &Interactive::commandUnknown, CMD_DEFAULT, nullptr, },
    { "monitor",        "Enable in-game op monitoring",
      &Interactive::commandUnknown, CMD_DEFAULT, nullptr, },
    { "query",          "Synonym for \"get\" (deprecated)",
      &Interactive::commandUnknown, CMD_DEFAULT, nullptr, },
    { "reload",         "Reload the script for a type",
      &Interactive::commandUnknown, CMD_DEFAULT, nullptr, },
    { "stat",           "Return current server status",
      &Interactive::commandUnknown, CMD_DEFAULT, nullptr, },
    { "unmonitor",      "Disable in-game op monitoring",
      &Interactive::commandUnknown, CMD_DEFAULT, nullptr, },
    { nullptr,             "Guard", nullptr, 0, }
};


static void help()
{
    size_t max_length = 0;

    for (struct command * I = &commands[0]; I->cmd_string != nullptr; ++I) {
       max_length = std::max(max_length, strlen(I->cmd_string));
    }
    max_length += 2;

    std::cout << "Cyphesis commands:" << std::endl << std::endl;

    for (struct command * I = &commands[0]; I->cmd_string != nullptr; ++I) {
        std::cout << " " << I->cmd_string
                  << std::string(max_length - strlen(I->cmd_string), ' ')
                  << I->cmd_description << std::endl;
    }
    std::cout << std::endl << std::flush;
}

Interactive::Interactive(Atlas::Objects::Factories& factories, boost::asio::io_context& io_context) :
    AdminClient(factories, io_context),
    m_server_flag(false),
    m_serverName("cyphesis"),
    m_prompt("cyphesis> ")
{
}

Interactive::~Interactive() = default;

void Interactive::operation(const Operation & op)
{
    auto J = m_contexts.begin();
    auto Jend = m_contexts.end();
    for (; J != Jend; ++J) {
        ObjectContext & c = **J;
        if (c.accept(op)) {
            if (c.dispatch(op) == 0) {
                // It is not safe to carry on iterating, as the
                // dispatch may have modified the context map.
                break;
            }
        }
    }
    AdminClient::operation(op);
}

void Interactive::appearanceArrived(const Operation & op)
{
    if (m_accountId.empty()) {
        return;
    }
    if (m_accountId != op->getTo()) {
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
    if (m_accountId.empty()) {
        return;
    }
    if (m_accountId != op->getTo()) {
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
    if (m_server_flag) {
        std::cout << "Server query success" << std::endl << std::flush;
        if (!ent->isDefaultName()) {
            m_serverName = ent->getName();
            std::string::size_type p = m_serverName.find('.');
            if (p != std::string::npos) {
                m_serverName = m_serverName.substr(0, p);
            }
            updatePrompt();
        }
        Element raw_attr;
        if (ent->copyAttr("server", raw_attr) == 0) {
            if (raw_attr.isString()) {
                m_systemType = raw_attr.String();
                updatePrompt();
            }
        }
        m_server_flag = false;
    } else if (m_currentTask == nullptr && op->isDefaultRefno()) {
        std::cout << "Info(" << std::endl;
        output(ent);
        std::cout << ")" << std::endl << std::flush;
        // Display results of command
    }
    AtlasStreamClient::infoArrived(op);
}

void Interactive::errorArrived(const Operation & op)
{
    reply_flag = true;
    error_flag = true;
    
    if (m_currentTask != nullptr) {
        return;
    }
    std::cout << "Error(";
    const std::vector<Root> & args = op->getArgs();
    const Root & arg = args.front();
    Element message_attr;
    if (arg->copyAttr("message", message_attr) == 0 && message_attr.isString()) {
        std::cout << message_attr.asString();
    }
    std::cout << ")" << std::endl << std::flush;
}

void Interactive::soundArrived(const Operation & op)
{
    if (m_accountId.empty()) {
        return;
    }
    if (m_accountId != op->getTo()) {
        // This is an IG op we are monitoring
        return;
    }
    reply_flag = true;
    if (op->getArgs().empty()) {
        std::cout << "Sound op has no args" << std::endl << std::flush;
        return;
    }
    Operation sub_op = smart_dynamic_cast<Operation>(op->getArgs().front());
    if (!sub_op.isValid()) {
        return;
    }
    if (sub_op->isDefaultFrom()) {
        std::cout << "Sound arg has no from" << std::endl << std::flush;
        return;
    }
    const std::string & from = sub_op->getFrom();
    if (sub_op->getArgs().empty()) {
        std::cout << "Sound arg has no args" << std::endl << std::flush;
        return;
    }
    const Root & arg = sub_op->getArgs().front();
    Element say;
    if (arg->copyAttr("say", say) != 0 || !say.isString()) {
        std::cout << "Sound arg arg has no say" << std::endl << std::flush;
        return;
    }
    std::cout << "[" << from << "] " << say.String()
              << std::endl << std::flush;
}

void Interactive::loginSuccess(const Atlas::Objects::Root & arg)
{
    // Create a new account context, store it in our context set,
    // and assign it as the current context
    ObjectContext * ac = new AccountContext(*this, m_accountId, m_username);
    // This is slightly unwieldy, but it ensures we get a weak pointer to
    // the context object with a minimum of weak copies.
    addCurrentContext(shared_ptr<ObjectContext>(ac));
}

sigc::signal<void(char *)> CmdLine;
sigc::signal<void(int, int)> ContextSwitch;

void Interactive::gotCommand(char * cmd)
{
    CmdLine.emit(cmd);
}

void Interactive::runCommand(char * cmd)
{
    if (cmd == nullptr) {
        cleanDisconnect();
        return;
    }

    if (*cmd == 0) {
        free(cmd);
        return;
    }

    add_history(cmd);

    char * arg = strchr(cmd, ' ');
    if (arg != nullptr) {
        *arg++ = 0;
        int len = strlen(arg);
        while (len > 0 && arg[--len] == ' ') { arg[len] = 0; }
    } else {
        arg = const_cast<char*>("");
    }

    for (struct command * I = &commands[0]; I->cmd_string != nullptr; ++I) {
        if (strcmp(cmd, I->cmd_string) == 0) {
            (this->*(I->cmd_method))(I, arg);
        }
    }

    exec(cmd, arg);
}

void Interactive::switchContext(int, int)
{
    auto I = m_contexts.begin();
    auto Iend = m_contexts.end();
    for (; I != Iend; ++I) {
        const std::shared_ptr<ObjectContext> & i = *I;
        if (m_currentContext.lock() == i) {
            if (++I == Iend) {
                I = m_contexts.begin();
            }
            m_currentContext = *I;
            break;
        }
    }
    updatePrompt();
    rl_redisplay();
}

const shared_ptr<ObjectContext> & Interactive::addContext(
      const shared_ptr<ObjectContext> & c)
{
    return *m_contexts.insert(c).first;
}

void Interactive::addCurrentContext(const shared_ptr<ObjectContext> & c)
{
    m_currentContext = *m_contexts.insert(c).first;
    updatePrompt();
    rl_redisplay();
}

int completion_iterator = 0;

namespace {

    char* completion_generator(const char* text, int state)
    {
        if (state == 0) {
            completion_iterator = 0;
        }
        for (int i = completion_iterator; commands[i].cmd_string != nullptr; ++i) {
            if (strncmp(text, commands[i].cmd_string, strlen(text)) == 0) {
                completion_iterator = i + 1;
                return strdup(commands[i].cmd_string);
            }
        }
        return nullptr;
    }

}

static int context_switch(int a, int b)
{
    ContextSwitch.emit(a, b);
    return 0;
}

void Interactive::loop()
{
    if (rl_bind_keyseq("`", &context_switch) != 0) {
        std::cout << "BINDING FAILED" << std::endl;
    }
    rl_callback_handler_install(m_prompt.c_str(),
                                &Interactive::gotCommand);
    rl_completion_entry_function = &completion_generator;
    CmdLine.connect(sigc::mem_fun(*this, &Interactive::runCommand));
    ContextSwitch.connect(sigc::mem_fun(*this, &Interactive::switchContext));
    while (select() == 0);
    std::cout << std::endl << std::flush;
    rl_callback_handler_remove();
}

int Interactive::select(bool rewrite_prompt)
{
    fd_set rfds;
    struct timeval tv{};
    int retval;
    FD_ZERO(&rfds);
    FD_SET(0, &rfds);

    tv.tv_sec = 0;
    tv.tv_usec = 0;

    retval = ::select(1, &rfds, nullptr, nullptr, &tv);

    if (retval){
        rl_callback_read_char();
    }


    int result = poll(std::chrono::microseconds(10000));
    if (result >= 0) {
        return 0;
    }
    std::cout << "Server disconnected" << std::endl << std::flush;
    return -1;
}

void Interactive::updatePrompt()
{
    std::string designation(">");
    if (m_accountType == "admin" || m_accountType == "sys") {
        designation = "#";
    } else {
        designation = "$";
    }
    std::string status = "idle";
    if (m_currentTask != nullptr) {
        status = m_currentTask->description();
    }
    std::string context;
    std::shared_ptr<ObjectContext> c = m_currentContext.lock();
    if (c) {
        context = c->repr();
    }
    m_prompt = String::compose("[%1@%2 %3{%4}]%5 ", context, m_serverName,
                               m_systemType, status, designation);
    rl_set_prompt(m_prompt.c_str());
}

int Interactive::setup()
{
    ObjectContext * cc = new ConnectionContext(*this);
    addContext(shared_ptr<ObjectContext>(cc));

    Get get;

    send(get);

    m_server_flag = true;

    reply_flag = true;
    while (m_server_flag && !error_flag) {
       poll(std::chrono::seconds(10));
    }

    m_server_flag = false;
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

    std::shared_ptr<ObjectContext> command_context = m_currentContext.lock();
    if (!command_context) {
        std::cout << "ERROR: Context free" << std::endl << std::flush;
        return;
    }

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
            c->setFrom(m_accountId);
            Anonymous ent;
            ent->setId(std::string(arg, 0, space));
            ent->setObjtype("class");
            ent->setParent(std::string(arg, space + 1));
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
        l->setSerialno(newSerialNo());
        command_context->setFromContext(l);
        send(l);
        reply_expected = false;
    } else if (cmd == "logout") {
        Logout l;
        l->setFrom(m_accountId);
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
        t->setFrom(m_accountId);
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
        g->setFrom(m_accountId);

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
            s->setFrom(m_accountId);

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
        g->setFrom(m_accountId);

        send(g);
    } else if (cmd == "monitor") {
        if (runTask(std::make_shared<OperationMonitor>(), arg) == 0) {
            Monitor m;

            m->setArgs1(Anonymous());
            m->setFrom(m_accountId);

            send(m);
        }

        reply_expected = false;
    } else if (cmd == "unmonitor") {
        auto om = dynamic_cast<OperationMonitor *>(m_currentTask.get());

        if (om) {
            Monitor m;

            m->setFrom(m_accountId);

            send(m);

            reply_expected = false;


            auto monitor_time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - om->startTime);

            std::cout << om->count() << " operations monitored in "
                      << monitor_time.count() << " seconds = "
                      << om->count() / monitor_time.count()
                      << " operations per second"
                      << std::endl << std::flush;

            endTask();
        }
    } else if (cmd == "connect") {
        std::vector<std::string> args;
        tokenize(arg, args);

        if (args.size() != 2) {
            std::cout << "usage: connect <hostname> <port>"
                      << std::endl << std::flush;

            reply_expected = false;
        } else {
            Anonymous cmap;
            cmap->setAttr("hostname", args[0]);
            cmap->setAttr("port", strtol(args[1].c_str(), nullptr, 10));

            Connect m;
            m->setArgs1(cmap);
            // No serialno yet
            // FIXME add serialno once Juncture context can handle this

            command_context->setFromContext(m);

            send(m);
        }
    } else if (cmd == "add_agent") {
        std::string agent_type("creator");

        if (!arg.empty()) {
            agent_type = arg;
        }
        
        Create c;

        Anonymous cmap;
        cmap->setParent(agent_type);
        cmap->setName("cycmd agent");
        cmap->setObjtype("obj");
        c->setArgs1(cmap);
        c->setSerialno(newSerialNo());

        command_context->setFromContext(c);

        send(c);
    } else if (cmd == "delete") {
        if (arg.empty()) {
            std::cout << "Please specify the entity to delete" << std::endl << std::flush;
            reply_expected = false;
        } else {
            Delete del;

            Anonymous del_arg;
            del_arg->setId(arg);
            del->setArgs1(del_arg);

            command_context->setFromContext(del);

            send(del);

            reply_expected = false;
        }
    } else if (cmd == "find_by_name") {
        if (arg.empty()) {
            std::cout << "Please specify the name to search for" << std::endl << std::flush;
            reply_expected = false;
        } else {
            Look l;

            Anonymous lmap;
            lmap->setName(arg);
            l->setArgs1(lmap);
            l->setSerialno(newSerialNo());

            command_context->setFromContext(l);

            send(l);

            reply_expected = false;
        }
    } else if (cmd == "find_by_type") {
        if (arg.empty()) {
            std::cout << "Please specify the type to search for" << std::endl << std::flush;
            reply_expected = false;
        } else {
            Look l;

            Anonymous lmap;
            lmap->setParent(arg);
            l->setArgs1(lmap);
            l->setSerialno(newSerialNo());

            command_context->setFromContext(l);

            send(l);

            reply_expected = false;
        }
    } else if (cmd == "flush") {
        if (arg.empty()) {
            // FIXME usage
            std::cout << "Please specify the type to flush" << std::endl << std::flush;
            reply_expected = false;
        } else {
            runTask(std::make_shared<Flusher>(command_context), arg);
            reply_expected = false;
        }
    } else if (cmd == "cancel") {
        if (endTask() != 0) {
            std::cout << "No task currently running" << std::endl << std::flush;
        }
    } else if (cmd == "dump") {
        if (command_context->repr() != "avatar") {
            std::cout << "You must have an agent in the world in order to dump the world." << std::endl << std::flush;
        } else {
            //Extract the avatar id by "misusing" the setFromContext method
            Operation op;
            command_context->setFromContext(op);
            runTask(std::make_shared<EntityExporter>(m_accountId, op->getFrom()), "world.xml");
            reply_expected = false;
        }
    } else if (cmd == "restore") {
        if (command_context->repr() != "avatar") {
            std::cout << "You must have an agent in the world in order to dump the world." << std::endl << std::flush;
        } else {
            //Extract the avatar id by "misusing" the setFromContext method
            Operation op;
            command_context->setFromContext(op);
            runTask(std::make_shared<EntityImporter>(m_accountId, op->getFrom()), "world.xml");
            reply_expected = false;
        }
    } else if (cmd == "create") {
        std::vector<std::string> args;
        tokenize(arg, args);

        if (args.empty()) {
            std::cout << "usage: create <type> <params> ... "
                      << std::endl << std::flush;
        } else {
            Anonymous cmap;
            cmap->setParent(args[0]);
            cmap->setObjtype("obj");

            Create c;
            c->setArgs1(cmap);
            c->setSerialno(newSerialNo());
            command_context->setFromContext(c);

            send(c);
        }
        reply_expected = false;
    } else if (cmd == "login") {
        std::vector<std::string> args;
        tokenize(arg, args);

        if (args.size() != 2) {
            std::cout << "usage: login <username> <password>"
                      << std::endl << std::flush;
            reply_expected = false;
        } else {
            Anonymous cmap;
            cmap->setAttr("username", args[0]);
            cmap->setAttr("password", args[1]);

            Login m;
            m->setArgs1(cmap);
            m->setSerialno(newSerialNo());

            command_context->setFromContext(m);

            send(m);
        }
    } else {
        reply_expected = false;
        std::cout << cmd << ": Command not known" << std::endl << std::flush;
    }

    if (!reply_expected) {
        updatePrompt();
        return;
    }
    // Wait for reply
    time_t wait_start_time = time(nullptr);
    while (!reply_flag) {
       if (time(nullptr) - wait_start_time > 5) {
           std::cout << cmd << ": No reply from server" << std::endl << std::flush;
           return;
       }
       if (select(false) != 0) {
           return;
       }
    }
}

int Interactive::commandUnknown(struct command * cmd,
                                const std::string & arg)
{
    return 0;
}
