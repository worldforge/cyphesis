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
#include <Atlas/Objects/Entity/Account.h>
#include <Atlas/Objects/Operation/Appearance.h>
#include <Atlas/Objects/Operation/Combine.h>
#include <Atlas/Objects/Operation/Delete.h>
#include <Atlas/Objects/Operation/Feel.h>
#include <Atlas/Objects/Operation/Imaginary.h>
#include <Atlas/Objects/Operation/Listen.h>
#include <Atlas/Objects/Operation/Move.h>
#include <Atlas/Objects/Operation/Disappearance.h>
#include <Atlas/Objects/Operation/Smell.h>
#include <Atlas/Objects/Operation/Touch.h>
#include <Atlas/Objects/Operation/Divide.h>
#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Objects/Operation/Logout.h>
#include <Atlas/Objects/Operation/Get.h>
#include <Atlas/Objects/Operation/Look.h>
#include <Atlas/Objects/Operation/Talk.h>
#include <Atlas/Objects/Operation/Error.h>
#include <Atlas/Objects/Operation/Sound.h>

#include "common/utility.h"
#include "common/Generic.h"

#include <skstream/skstream_unix.h>

#include <sigc++/object_slot.h>
#if SIGC_MAJOR_VERSION == 1 && SIGC_MINOR_VERSION == 0
#include <sigc++/signal_system.h>
#else
#include <sigc++/signal.h>
#endif

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
using Atlas::Objects::Operation::Appearance;
using Atlas::Objects::Operation::Disappearance;
using Atlas::Objects::Operation::Generic;
using Atlas::Objects::Operation::Get;
using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Look;
using Atlas::Objects::Operation::Logout;
using Atlas::Objects::Operation::Talk;

static void help()
{
    std::cout << "Cyphesis commands:" << std::endl << std::endl;
    std::cout << "    get       Examine a class on the server" << std::endl;
    std::cout << "    help      Display this help" << std::endl;
    std::cout << "    look      Return current server lobby" << std::endl;
    std::cout << "    logout    Log user out of server" << std::endl;
    std::cout << "    monitor   Enable in-game op monitoring" << std::endl;
    std::cout << "    query     Examine an object on the server" << std::endl;
    std::cout << "    stat      Return current server status" << std::endl;
    std::cout << "    unmonitor Disable in-game op monitoring" << std::endl;
    std::cout << std::endl << std::flush;
}

template <class Stream>
class Interactive : public Atlas::Objects::Decoder, public SigC::Object
{
  private:
    bool error_flag, reply_flag, login_flag;
    int cli_fd;
    Atlas::Objects::Encoder * encoder;
    Atlas::Codec<std::iostream> * codec;
    Stream ios;
    std::string password;
    std::string username;
    std::string accountId;
    bool exit;
    int monitor_op_count;
    int monitor_start_time;

    void output(const Atlas::Message::Element & item, bool recurse = true);
    void logOp(const Atlas::Objects::Operation::RootOperation &);
  protected:
    void objectArrived(const Atlas::Objects::Operation::Action& op) { logOp(op); }
    void objectArrived(const Atlas::Objects::Operation::Combine& op) { logOp(op); }
    void objectArrived(const Atlas::Objects::Operation::Communicate& op) { logOp(op); }
    void objectArrived(const Atlas::Objects::Operation::Create& op) { logOp(op); }
    void objectArrived(const Atlas::Objects::Operation::Delete& op) { logOp(op); }
    void objectArrived(const Atlas::Objects::Operation::Divide& op) { logOp(op); }
    void objectArrived(const Atlas::Objects::Operation::Feel& op) { logOp(op); }
    void objectArrived(const Atlas::Objects::Operation::Get& op) { logOp(op); }
    void objectArrived(const Atlas::Objects::Operation::Imaginary& op) { logOp(op); }
    void objectArrived(const Atlas::Objects::Operation::Listen& op) { logOp(op); }
    void objectArrived(const Atlas::Objects::Operation::Login& op) { logOp(op); }
    void objectArrived(const Atlas::Objects::Operation::Logout& op) { logOp(op); }
    void objectArrived(const Atlas::Objects::Operation::Look& op) { logOp(op); }
    void objectArrived(const Atlas::Objects::Operation::Move& op) { logOp(op); }
    void objectArrived(const Atlas::Objects::Operation::Perceive& op) { logOp(op); }
    void objectArrived(const Atlas::Objects::Operation::Perception& op) { logOp(op); }
    void objectArrived(const Atlas::Objects::Operation::RootOperation& op) { logOp(op); }
    void objectArrived(const Atlas::Objects::Operation::Set& op) { logOp(op); }
    void objectArrived(const Atlas::Objects::Operation::Smell& op) { logOp(op); }
    void objectArrived(const Atlas::Objects::Operation::Talk& op) { logOp(op); }
    void objectArrived(const Atlas::Objects::Operation::Touch& op) { logOp(op); }

    void unknownObjectArrived(const Element &);
    void objectArrived(const Atlas::Objects::Operation::Appearance&);
    void objectArrived(const Atlas::Objects::Operation::Disappearance&);
    void objectArrived(const Atlas::Objects::Operation::Info&);
    void objectArrived(const Atlas::Objects::Operation::Error&);
    void objectArrived(const Atlas::Objects::Operation::Sight&);
    void objectArrived(const Atlas::Objects::Operation::Sound&);

    bool negotiate();
  public:
    Interactive() : error_flag(false), reply_flag(false), login_flag(false),
                    encoder(NULL), codec(NULL), exit(false),
                    monitor_op_count(0), monitor_start_time(0) { }

    void send(const Atlas::Objects::Operation::RootOperation &);
    bool connect(const std::string & host);
    bool login();
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
void Interactive<Stream>::output(const Atlas::Message::Element & item, bool recurse)
{
    std::cout << " ";
    switch (item.getType()) {
        case Atlas::Message::Element::TYPE_INT:
            std::cout << item.asInt();
            break;
        case Atlas::Message::Element::TYPE_FLOAT:
            std::cout << item.asFloat();
            break;
        case Atlas::Message::Element::TYPE_STRING:
            std::cout << item.asString();
            break;
        case Atlas::Message::Element::TYPE_LIST:
            if (recurse) {
                Atlas::Message::Element::ListType::const_iterator I = item.asList().begin();
                std::cout << "[ ";
                for(; I != item.asList().end(); ++I) {
                    output(*I, true);
                }
                std::cout << " ]";
            } else {
                std::cout << "(list)";
            }
            break;
        case Atlas::Message::Element::TYPE_MAP:
            if (recurse) {
                Atlas::Message::Element::MapType::const_iterator I = item.asMap().begin();
                std::cout << "{ ";
                for(; I != item.asMap().end(); ++I) {
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
void Interactive<Stream>::logOp(const Atlas::Objects::Operation::RootOperation & op)
{
    ++monitor_op_count;
    std::cout << op.getParents().front().asString() << "(from=\"" << op.getFrom()
              << "\",to=\"" << op.getTo() << "\")" << std::endl << std::flush;
}

template <class Stream>
void Interactive<Stream>::objectArrived(const Atlas::Objects::Operation::Appearance& o)
{
    if (accountId.empty()) {
        return;
    }
    if (accountId != o.getTo()) {
        // This is an IG op we are monitoring
        logOp(o);
        return;
    }
    if (o.getArgs().empty()) {
        return;
    }
    const Atlas::Message::Element::MapType & ent = o.getArgs().front().asMap();
    Atlas::Message::Element::MapType::const_iterator I = ent.find("id");
    if (!I->second.isString()) {
        std::cerr << "Got Appearance of non-string ID" << std::endl << std::flush;
        return;
    }
    const std::string & id = I->second.asString();
    std::cout << "Appearance(id: " << id << ")";
    I = ent.find("loc");
    if (I == ent.end()) {
        std::cout << std::endl << std::flush;
        return;
    }
    if (!I->second.isString()) {
        std::cerr << " with non-string LOC" << std::endl << std::flush;
        return;
    }
    const std::string & loc = I->second.asString();
    std::cout << " in " << loc << std::endl;
    if (loc == "lobby") {
        std::cout << id << " has logged in." << std::endl;
    }
    std::cout << std::flush;
}

template <class Stream>
void Interactive<Stream>::unknownObjectArrived(const Element & e)
{
    RootOperation r;
    bool isOp = utility::Object_asOperation(e.asMap(), r);
    if (isOp) {
        logOp(r);
    }
}

template <class Stream>
void Interactive<Stream>::objectArrived(const Atlas::Objects::Operation::Disappearance& o)
{
    if (accountId.empty()) {
        return;
    }
    if (accountId != o.getTo()) {
        // This is an IG op we are monitoring
        logOp(o);
        return;
    }
    if (o.getArgs().empty()) {
        return;
    }
    const Atlas::Message::Element::MapType & ent = o.getArgs().front().asMap();
    Atlas::Message::Element::MapType::const_iterator I = ent.find("id");
    if (!I->second.isString()) {
        std::cerr << "Got Disappearance of non-string ID" << std::endl << std::flush;
        return;
    }
    const std::string & id = I->second.asString();
    std::cout << "Disappearance(id: " << id << ")";
    I = ent.find("loc");
    if (I == ent.end()) {
        std::cout << std::endl << std::flush;
        return;
    }
    if (!I->second.isString()) {
        std::cerr << " with non-string LOC" << std::endl << std::flush;
        return;
    }
    const std::string & loc = I->second.asString();
    std::cout << " in " << loc << std::endl;
    if (loc == "lobby") {
        std::cout << id << " has logged out." << std::endl;
    }
    std::cout << std::flush;
}

template <class Stream>
void Interactive<Stream>::objectArrived(const Atlas::Objects::Operation::Info& o)
{
    reply_flag = true;
    if (o.getArgs().empty()) {
        return;
    }
    const Atlas::Message::Element::MapType & ent = o.getArgs().front().asMap();
    Atlas::Message::Element::MapType::const_iterator I;
    if (login_flag) {
        I = ent.find("id");
        if (I == ent.end() || !I->second.isString()) {
            std::cerr << "ERROR: Response to login does not contain account id"
                      << std::endl << std::flush;
            
        } else {
            accountId = I->second.asString();
        }
    } else {
        std::cout << "Info(" << std::endl;
        for (I = ent.begin(); I != ent.end(); I++) {
            const Atlas::Message::Element & item = I->second;
            std::cout << "     " << I->first << ": ";
            output(item);
            std::cout << std::endl;
        }
        std::cout << ")" << std::endl << std::flush;
        // Display results of command
    }
}

template <class Stream>
void Interactive<Stream>::objectArrived(const Atlas::Objects::Operation::Error& o)
{
    reply_flag = true;
    error_flag = true;
    std::cout << "Error(";
    const Atlas::Message::Element::ListType & args = o.getArgs();
    const Atlas::Message::Element & arg = args.front();
    if (arg.isString()) {
        std::cout << arg.asString();
    } else if (arg.isMap()) {
        std::cout << arg.asMap().find("message")->second.asString();
    }
    std::cout << ")" << std::endl << std::flush;
}

template <class Stream>
void Interactive<Stream>::objectArrived(const Atlas::Objects::Operation::Sight& o)
{
    if (accountId.empty()) {
        return;
    }
    if (accountId != o.getTo()) {
        // This is an IG op we are monitoring
        logOp(o);
        return;
    }
    reply_flag = true;
    std::cout << "Sight(" << std::endl;
    const Atlas::Message::Element::MapType & ent = o.getArgs().front().asMap();
    Atlas::Message::Element::MapType::const_iterator I;
    for (I = ent.begin(); I != ent.end(); I++) {
        const Atlas::Message::Element & item = I->second;
        std::cout << "      " << I->first << ":";
        output(item);
        std::cout << std::endl;
    }
    std::cout << ")" << std::endl << std::flush;
}

template <class Stream>
void Interactive<Stream>::objectArrived(const Atlas::Objects::Operation::Sound& o)
{
    if (accountId.empty()) {
        return;
    }
    if (accountId != o.getTo()) {
        // This is an IG op we are monitoring
        logOp(o);
        return;
    }
    reply_flag = true;
    const Atlas::Message::Element::MapType & arg = o.getArgs().front().asMap();
    Atlas::Message::Element::MapType::const_iterator I = arg.find("from");
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
    const Atlas::Message::Element::MapType & ent = I->second.asList().front().asMap();
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

template <class Stream>
void Interactive<Stream>::loop()
{
    rl_callback_handler_install("cyphesis> ", &Interactive<Stream>::gotCommand);
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

bool Interactive<tcp_socket_stream>::connect(const std::string & host)
{
    std::cout << "Connecting... " << std::flush;
    ios.open(host, port_num);
    if (!ios.is_open()) {
        std::cout << "failed." << std::endl << std::flush;
        return false;
    }
    std::cout << "done." << std::endl << std::flush;
    cli_fd = ios.getSocket();

    return negotiate();
}

bool Interactive<unix_socket_stream>::connect(const std::string & filename)
{
    std::cout << "Connecting... " << std::flush;
    ios.open(filename);
    if (!ios.is_open()) {
        std::cout << "failed." << std::endl << std::flush;
        return false;
    }
    std::cout << "done." << std::endl << std::flush;
    cli_fd = ios.getSocket();

    return negotiate();
}

template <class Stream>
bool Interactive<Stream>::negotiate()
{
    // Do client negotiation with the server
    Atlas::Net::StreamConnect conn("cycmd", ios, this);

    std::cout << "Negotiating... " << std::flush;
    while (conn.getState() == Atlas::Negotiate<std::iostream>::IN_PROGRESS) {
        // conn.poll() does all the negotiation
        conn.poll();
    }
    std::cout << "done." << std::endl << std::flush;

    // Check whether negotiation was successful
    if (conn.getState() == Atlas::Negotiate<std::iostream>::FAILED) {
        std::cerr << "Failed to negotiate." << std::endl;
        return false;
    }
    // Negotiation was successful

    // Get the codec that negotiation established
    codec = conn.getCodec();

    // Create the encoder
    encoder = new Atlas::Objects::Encoder(codec);

    // Send whatever codec specific data marks the beginning of a stream
    codec->streamBegin();
    return true;

}

template <class Stream>
bool Interactive<Stream>::login()
{
    Atlas::Objects::Entity::Account account;
    Atlas::Objects::Operation::Login l;
    error_flag = false;
    reply_flag = false;
    login_flag = true;
 
    account.setAttr("username", username);
    account.setAttr("password", password);
 
    Atlas::Message::Element::ListType args(1,account.asObject());
 
    l.setArgs(args);
 
    encoder->streamMessage(&l);

    ios << std::flush;
 
    while (!reply_flag) {
       codec->poll();
    }

    login_flag = false;

    if (!error_flag) {
       return true;
    }
    return false;
}

template <class Stream>
void Interactive<Stream>::exec(const std::string & cmd, const std::string & arg)
{
    bool reply_expected = true;
    reply_flag = false;
    error_flag = false;

    if (cmd == "stat") {
        Get g;
        encoder->streamMessage(&g);
    } else if (cmd == "look") {
        Look l;
        l.setFrom(accountId);
        encoder->streamMessage(&l);
    } else if (cmd == "logout") {
        Logout l;
        l.setFrom(accountId);
        if (!arg.empty()) {
            Atlas::Message::Element::MapType lmap;
            lmap["id"] = arg;
            l.setArgs(Atlas::Message::Element::ListType(1,lmap));
            reply_expected = false;
        }
        encoder->streamMessage(&l);
    } else if (cmd == "say") {
        Talk t;
        Atlas::Message::Element::MapType ent;
        ent["say"] = arg;
        t.setArgs(Atlas::Message::Element::ListType(1,ent));
        t.setFrom(accountId);
        encoder->streamMessage(&t);
    } else if ((cmd == "help") || (cmd == "?")) {
        reply_expected = false;
        help();
    } else if (cmd == "query") {
        Get g;

        Atlas::Message::Element::MapType cmap;
        cmap["objtype"] = "obj";
        if (!arg.empty()) {
            cmap["id"] = arg;
        }
        g.setArgs(Atlas::Message::Element::ListType(1,cmap));
        g.setFrom(accountId);

        encoder->streamMessage(&g);
    } else if (cmd == "get") {
        Get g;

        Atlas::Message::Element::MapType cmap;
        cmap["objtype"] = "class";
        if (!arg.empty()) {
            cmap["id"] = arg;
        }
        g.setArgs(Atlas::Message::Element::ListType(1,cmap));
        g.setFrom(accountId);

        encoder->streamMessage(&g);
    } else if (cmd == "monitor") {
        reply_expected = false;
        Generic m("monitor");

        m.getArgs().push_back(Atlas::Message::Element::MapType());
        m.setFrom(accountId);

        encoder->streamMessage(&m);

        monitor_op_count = 0;
        struct timeval tv;
        gettimeofday(&tv, NULL);
        monitor_start_time = tv.tv_sec;
    } else if (cmd == "unmonitor") {
        reply_expected = false;
        Generic m("monitor");

        m.setFrom(accountId);

        encoder->streamMessage(&m);

        struct timeval tv;
        gettimeofday(&tv, NULL);
        int monitor_time = tv.tv_sec - monitor_start_time;
        std::cout << monitor_op_count << " operations monitored in " << monitor_time << " seconds = " << monitor_op_count / monitor_time << " operations per second" << std::endl << std::flush;
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
    std::cout << "usage: " << prg << " [ cmd [ server ] ]" << std::endl << std::flush;
}

int main(int argc, char ** argv)
{
    int cargc = 0;
    char * cargv[0];

    if (loadConfig(cargc, cargv)) {
        // Fatal error loading config file
        return 1;
    }

    bool interactive = true;
    std::string cmd;
    char * server = 0;
    if (argc > 1) {
        if (argc == 3) {
            server = argv[2];
        } else if (argc > 3) {
            usage(argv[0]);
            return 1;
        }
        cmd = argv[1];
        interactive = false;
    }

    if (server == 0) {
        std::string localSocket = var_directory + "/tmp/cyphesis.sock";

        std::cerr << "Attempting local connection" << std::endl << std::flush;
        Interactive<unix_socket_stream> bridge;
        if (bridge.connect(localSocket)) {
            bridge.setUsername("admin");

            std::cout << "Logging in... " << std::flush;
            if (!bridge.login()) {
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
    }
    
    std::cerr << "Attempting tcp connection" << std::endl << std::flush;

    Interactive<tcp_socket_stream> bridge;

    std::string hostname;
    if (server == 0) {
        if (!interactive) {
            std::cerr << "No server hostname given in non-interactive mode."
                      << std::endl << std::flush;
            return 1;
        }
        std::cout << "Hostname: " << std::flush;
        std::cin >> hostname;
    } else {
        hostname = server;
    }

    if (!bridge.connect(hostname)) {
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
        if (!bridge.login()) {
            std::cout << "failed." << std::endl << std::flush;
            return 1;
        }
        std::cout << "done." << std::endl << std::flush;
    }
    if (!interactive) {
        bridge.exec(cmd, "");
        return 0;
    } else {
        bridge.loop();
    }
    return 0;
}
