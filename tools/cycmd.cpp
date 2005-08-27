// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "common/const.h"
#include "common/globals.h"

#include <Atlas/Objects/Encoder.h>
#include <Atlas/Net/Stream.h>
#include <Atlas/Objects/Decoder.h>
#include <Atlas/Codec.h>
#include <Atlas/Objects/Entity.h>
#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>

#include <varconf/Config.h>

#include "common/utility.h"
#include "common/Monitor.h"
#include "common/Connect.h"

#include <skstream/skstream_unix.h>

#include <sigc++/object_slot.h>
#include <sigc++/signal.h>
#include <sigc++/connection.h>

#ifndef READLINE_CXX_SANE   // defined in config.h
extern "C" {
#endif
#include <readline/readline.h>
#include <readline/history.h>
#ifndef READLINE_CXX_SANE
}
#endif

#include <iostream>
#include <cstdio>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;

using Atlas::Objects::Root;

using Atlas::Objects::Operation::Appearance;
using Atlas::Objects::Operation::Create;
using Atlas::Objects::Operation::Disappearance;
using Atlas::Objects::Operation::Get;
using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Look;
using Atlas::Objects::Operation::Login;
using Atlas::Objects::Operation::Logout;
using Atlas::Objects::Operation::Talk;
using Atlas::Objects::Operation::RootOperation;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;

using Atlas::Objects::smart_dynamic_cast;

using Atlas::Objects::Operation::Monitor;
using Atlas::Objects::Operation::Connect;

struct command {
    const char * cmd_string;
    const char * cmd_description;
};

struct command commands[] = {
    { "connect",        "Connect server to a peer", },
    { "get",            "Examine a class on the server", },
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
    std::cout << "Cyphesis commands:" << std::endl << std::endl;
    std::cout << "    connect   Connect server to a peer" << std::endl;
    std::cout << "    get       Examine a class on the server" << std::endl;
    std::cout << "    help      Display this help" << std::endl;
    std::cout << "    install   Install a new type" << std::endl;
    std::cout << "    look      Return current server lobby" << std::endl;
    std::cout << "    logout    Log user out of server" << std::endl;
    std::cout << "    monitor   Enable in-game op monitoring" << std::endl;
    std::cout << "    query     Examine an object on the server" << std::endl;
    std::cout << "    reload    Reload the script for a type" << std::endl;
    std::cout << "    stat      Return current server status" << std::endl;
    std::cout << "    unmonitor Disable in-game op monitoring" << std::endl;
    std::cout << std::endl << std::flush;
}

template <class Stream>
class Interactive : public Atlas::Objects::ObjectsDecoder, public SigC::Object
{
  private:
    bool error_flag, reply_flag, login_flag, avatar_flag;
    int cli_fd;
    Atlas::Objects::ObjectsEncoder * encoder;
    Atlas::Codec * codec;
    Stream ios;
    std::string password;
    std::string username;
    std::string accountId;
    std::string agentId;
    bool exit;
    int monitor_op_count;
    int monitor_start_time;

    void output(const Element & item, bool recurse = true);
    void logOp(const RootOperation &);
  protected:

    void objectArrived(const Atlas::Objects::Root &);

    void appearanceArrived(const RootOperation &);
    void disappearanceArrived(const RootOperation &);
    void infoArrived(const RootOperation &);
    void errorArrived(const RootOperation &);
    void sightArrived(const RootOperation &);
    void soundArrived(const RootOperation &);

    int negotiate();
  public:
    Interactive() : error_flag(false), reply_flag(false), login_flag(false),
                    avatar_flag(false), encoder(0), codec(0), exit(false),
                    monitor_op_count(0), monitor_start_time(0) { }
    ~Interactive() {
        if (encoder != 0) {
            delete encoder;
        }
        if (codec != 0) {
            delete codec;
        }
    }

    void send(const RootOperation &);
    int connect(const std::string & host);
    int login();
    void exec(const std::string & cmd, const std::string & arg);
    void loop();
    void poll();
    void getLogin();
    void runCommand(char *);

    void setPassword(const std::string & passwd) {
        password = passwd;
    }

    void setUsername(const std::string & uname) {
        username = uname;
    }

    static void gotCommand(char *);
};

template <class Stream>
void Interactive<Stream>::output(const Element & item, bool recurse)
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
            std::cout << "\"" << item.asString() << "\"";
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

template <class Stream>
void Interactive<Stream>::logOp(const RootOperation & op)
{
    ++monitor_op_count;
    std::cout << op->getParents().front() << "(from=\"" << op->getFrom()
              << "\",to=\"" << op->getTo() << "\")" << std::endl << std::flush;
}

template <class Stream>
void Interactive<Stream>::objectArrived(const Atlas::Objects::Root & obj)
{
    RootOperation op = Atlas::Objects::smart_dynamic_cast<RootOperation>(obj);
    if (!op.isValid()) {
        // FIXME report the parents and objtype
        std::cerr << "Non op object received from client" << std::endl << std::flush;
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
            logOp(op);
            break;
    }
}

template <class Stream>
void Interactive<Stream>::appearanceArrived(const RootOperation & op)
{
    if (accountId.empty()) {
        return;
    }
    if (accountId != op->getTo()) {
        // This is an IG op we are monitoring
        logOp(op);
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
void Interactive<Stream>::disappearanceArrived(const RootOperation & op)
{
    if (accountId.empty()) {
        return;
    }
    if (accountId != op->getTo()) {
        // This is an IG op we are monitoring
        logOp(op);
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
void Interactive<Stream>::infoArrived(const RootOperation & op)
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
    } else if (avatar_flag) {
        std::cout << "Create agent success" << std::endl << std::flush;
        if (!ent->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
            std::cerr << "ERROR: Response to agent create does not contain agent id"
                      << std::endl << std::flush;
            
        } else {
            agentId = ent->getId();
            avatar_flag = false;
        }
    } else {
        std::cout << "Info(" << std::endl;
        MapType entmap = ent->asMessage();
        MapType::const_iterator Iend = entmap.end();
        for (MapType::const_iterator I = entmap.begin(); I != Iend; ++I) {
            const Element & item = I->second;
            std::cout << "     " << I->first << ": ";
            output(item);
            std::cout << std::endl;
        }
        std::cout << ")" << std::endl << std::flush;
        // Display results of command
    }
}

template <class Stream>
void Interactive<Stream>::errorArrived(const RootOperation & op)
{
    reply_flag = true;
    error_flag = true;
    std::cout << "Error(";
    const std::vector<Root> & args = op->getArgs();
    const Root & arg = args.front();
    Element message_attr;
    if (arg->getAttr("message", message_attr) == 0 && message_attr.isString()) {
        std::cout << message_attr.asString();
    }
    std::cout << ")" << std::endl << std::flush;
}

template <class Stream>
void Interactive<Stream>::sightArrived(const RootOperation & op)
{
    if (accountId.empty()) {
        return;
    }
    if (accountId != op->getTo() && agentId != op->getTo()) {
        // This is an IG op we are monitoring
        logOp(op);
        return;
    }
    reply_flag = true;
    std::cout << "Sight(" << std::endl;
    const MapType & ent = op->getArgs().front()->asMessage();
    MapType::const_iterator Iend = ent.end();
    for (MapType::const_iterator I = ent.begin(); I != Iend; ++I) {
        const Element & item = I->second;
        std::cout << "      " << I->first << ":";
        output(item);
        std::cout << std::endl;
    }
    std::cout << ")" << std::endl << std::flush;
}

template <class Stream>
void Interactive<Stream>::soundArrived(const RootOperation & op)
{
    if (accountId.empty()) {
        return;
    }
    if (accountId != op->getTo()) {
        // This is an IG op we are monitoring
        logOp(op);
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

SigC::Signal1<void, char *> CmdLine;

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
        while ((len > 0) && (arg[--len] == ' ')) { arg[len] = 0; }
    } else {
        arg = "";
    }

    exec(cmd, arg);
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
    rl_callback_handler_install("cyphesis> ", &Interactive<Stream>::gotCommand);
    rl_completion_entry_function = &completion_generator;
    SigC::Connection c = CmdLine.connect(SigC::slot(*this, &Interactive<Stream>::runCommand));
    while (!exit) {
        poll();
    };
    c.disconnect();
    rl_callback_handler_remove();
}

template <class Stream>
void Interactive<Stream>::poll()
// poll the codec if select says there is something there.
{
    fd_set infds;
    struct timeval tv;

    FD_ZERO(&infds);

    FD_SET(cli_fd, &infds);
    FD_SET(STDIN_FILENO, &infds);

    tv.tv_sec = 0;
    tv.tv_usec = 100000;

    int retval = select(cli_fd+1, &infds, NULL, NULL, &tv);

    if (retval) {
        if (FD_ISSET(cli_fd, &infds)) {
            if (ios.peek() == -1) {
                std::cout << "Server disconnected" << std::endl << std::flush;
                exit = true;
            } else {
                std::cout << std::endl;
                codec->poll();
                rl_forced_update_display();
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
    Atlas::Net::StreamConnect conn("cycmd", ios, *this);

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
    codec = conn.getCodec();

    // Create the encoder
    encoder = new Atlas::Objects::ObjectsEncoder(*codec);

    // Send whatever codec specific data marks the beginning of a stream
    codec->streamBegin();
    return 0;

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
        if ((space == std::string::npos) || (space >= (arg.size() - 1))) {
            std::cout << "usage: install <type id> <parent id>"
                      << std::endl << std::flush;
        } else {
            Set s;
            s->setFrom(accountId);
            Anonymous ent;
            ent->setId(std::string(arg, 0, space));
            ent->setObjtype("class");
            ent->setParents(std::list<std::string>(1, std::string(arg, space + 1)));
            s->setArgs1(ent);
            encoder->streamObjectsMessage(s);
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
    } else if ((cmd == "help") || (cmd == "?")) {
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
        reply_expected = false;
        Monitor m;

        m->setArgs1(Anonymous());
        m->setFrom(accountId);

        encoder->streamObjectsMessage(m);

        monitor_op_count = 0;
        struct timeval tv;
        gettimeofday(&tv, NULL);
        monitor_start_time = tv.tv_sec;
    } else if (cmd == "unmonitor") {
        reply_expected = false;
        Monitor m;

        m->setFrom(accountId);

        encoder->streamObjectsMessage(m);

        struct timeval tv;
        gettimeofday(&tv, NULL);
        int monitor_time = tv.tv_sec - monitor_start_time;
        std::cout << monitor_op_count << " operations monitored in " << monitor_time << " seconds = " << monitor_op_count / monitor_time << " operations per second" << std::endl << std::flush;
    } else if (cmd == "connect") {
        reply_expected = false;
        Connect m;

        Anonymous cmap;
        cmap->setAttr("hostname", arg);
        m->setArgs1(cmap);
        m->setFrom(accountId);

        encoder->streamObjectsMessage(m);
    } else if (cmd == "add_agent") {
        Create c;

        Anonymous cmap;
        cmap->setParents(std::list<std::string>(1, "creator"));
        cmap->setObjtype("obj");
        c->setArgs1(cmap);
        c->setFrom(accountId);

        avatar_flag = true;

        encoder->streamObjectsMessage(c);
    } else if (cmd == "find_by_name") {
        if (agentId.empty()) {
            std::cout << "Use add_egent to add an in-game agent first" << std::endl << std::flush;
            reply_expected = false;
        } else {
            Look l;

            Anonymous lmap;
            lmap->setName(arg);
            l->setArgs1(lmap);
            l->setFrom(agentId);

            encoder->streamObjectsMessage(l);
        }
    } else if (cmd == "find_by_type") {
        if (agentId.empty()) {
            std::cout << "Use add_egent to add an in-game agent first" << std::endl << std::flush;
            reply_expected = false;
        } else {
            Look l;

            Anonymous lmap;
            lmap->setParents(std::list<std::string>(1, arg));
            l->setArgs1(lmap);
            l->setFrom(agentId);

            encoder->streamObjectsMessage(l);
        }
    } else {
        reply_expected = false;
        std::cout << cmd << ": Command not known" << std::endl << std::flush;
    }

    ios << std::flush;

    if (!reply_expected) { return; }
    // Wait for reply
    // FIXME Only wait a reasonable ammount of time
    while (!reply_flag) {
       codec->poll();
    }
}

static void usage(char * prg)
{
    std::cerr << "usage: " << prg << " [ cmd [ server ] ]" << std::endl << std::flush;
}

int main(int argc, char ** argv)
{
    int optind;

    if ((optind = loadConfig(argc, argv)) < 0) {
        // Fatal error loading config file
        return 1;
    }

    std::string server;
    if (global_conf->findItem("client", "serverhost")) {
        server = global_conf->getItem("client", "serverhost").as_string();
    }

    int useslave = 0;
    if (global_conf->findItem("client", "useslave")) {
        useslave = global_conf->getItem("client", "useslave");
    }

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
        // FIXME This socket name should be an option
        std::string localSocket = var_directory + "/tmp/";
        if (useslave != 0) {
            localSocket += slave_socket_name;
        } else {
            localSocket += client_socket_name;
        }

        std::cerr << "Attempting local connection" << std::endl << std::flush;
        Interactive<unix_socket_stream> bridge;
        if (bridge.connect(localSocket) == 0) {
            bridge.setUsername("admin");

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
