// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <common/accountbase.h>
#include <common/const.h>
#include <common/globals.h>

#include <Atlas/Objects/Encoder.h>
#include <Atlas/Net/Stream.h>
#include <Atlas/Objects/Decoder.h>
#include <Atlas/Codec.h>
#include <Atlas/Objects/Entity/Account.h>
#include <Atlas/Objects/Operation/Appearance.h>
#include <Atlas/Objects/Operation/Disappearance.h>
#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Objects/Operation/Logout.h>
#include <Atlas/Objects/Operation/Get.h>
#include <Atlas/Objects/Operation/Look.h>
#include <Atlas/Objects/Operation/Talk.h>

#include <skstream/skstream.h>

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

using Atlas::Message::Object;
using Atlas::Objects::Operation::Appearance;
using Atlas::Objects::Operation::Disappearance;
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
    std::cout << "    query     Examine an object on the server" << std::endl;
    std::cout << "    stat      Return current server status" << std::endl;
    std::cout << std::endl << std::flush;
}

class Interactive : public Atlas::Objects::Decoder, public SigC::Object
{
  private:
    bool error_flag, reply_flag;
    int cli_fd;
    Atlas::Objects::Encoder * encoder;
    Atlas::Codec<std::iostream> * codec;
    tcp_socket_stream ios;
    std::string password;
    std::string username;
    std::string accountId;
    enum {
       INIT,
       LOGGED_IN
    } state;
    bool exit;

    void output(const Atlas::Message::Object & item, bool recurse = true);
  protected:
    //void UnknownObjectArrived(const Object&);
    void ObjectArrived(const Atlas::Objects::Operation::Appearance&);
    void ObjectArrived(const Atlas::Objects::Operation::Disappearance&);
    void ObjectArrived(const Atlas::Objects::Operation::Info&);
    void ObjectArrived(const Atlas::Objects::Operation::Error&);
    void ObjectArrived(const Atlas::Objects::Operation::Sight&);
    void ObjectArrived(const Atlas::Objects::Operation::Sound&);

  public:
    Interactive() : error_flag(false), reply_flag(false), encoder(NULL),
                       codec(NULL), state(INIT), exit(false) { }

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

void Interactive::output(const Atlas::Message::Object & item, bool recurse)
{
    std::cout << " ";
    switch (item.GetType()) {
        case Atlas::Message::Object::TYPE_INT:
            std::cout << item.AsInt();
            break;
        case Atlas::Message::Object::TYPE_FLOAT:
            std::cout << item.AsFloat();
            break;
        case Atlas::Message::Object::TYPE_STRING:
            std::cout << item.AsString();
            break;
        case Atlas::Message::Object::TYPE_LIST:
            if (recurse) {
                Atlas::Message::Object::ListType::const_iterator I = item.AsList().begin();
                for(; I != item.AsList().end(); ++I) {
                    output(*I, false);
                }
            } else {
                std::cout << "(list)";
            }
            break;
        case Atlas::Message::Object::TYPE_MAP:
            if (recurse) {
                Atlas::Message::Object::MapType::const_iterator I = item.AsMap().begin();
                for(; I != item.AsMap().end(); ++I) {
                    std::cout << I->first << ": ";
                    output(I->second, false);
                }
            } else {
                std::cout << "(map)";
            }
            break;
        default:
            std::cout << "(\?\?\?)";
            break;
    }
}

void Interactive::ObjectArrived(const Atlas::Objects::Operation::Appearance& o)
{
    if (o.GetArgs().empty()) {
        return;
    }
    const Atlas::Message::Object::MapType & ent = o.GetArgs().front().AsMap();
    Atlas::Message::Object::MapType::const_iterator I = ent.find("id");
    if (!I->second.IsString()) {
        std::cerr << "Got Appearance of non-string ID" << std::endl << std::flush;
        return;
    }
    const std::string & id = I->second.AsString();
    std::cout << "Appearance(id: " << id << ")";
    I = ent.find("loc");
    if (I == ent.end()) {
        std::cout << std::endl << std::flush;
        return;
    }
    if (!I->second.IsString()) {
        std::cerr << " with non-string LOC" << std::endl << std::flush;
        return;
    }
    const std::string & loc = I->second.AsString();
    std::cout << " in " << loc << std::endl;
    if (loc == "lobby") {
        std::cout << id << " has logged in." << std::endl;
    }
    std::cout << std::flush;
}

void Interactive::ObjectArrived(const Atlas::Objects::Operation::Disappearance& o)
{
    if (o.GetArgs().empty()) {
        return;
    }
    const Atlas::Message::Object::MapType & ent = o.GetArgs().front().AsMap();
    Atlas::Message::Object::MapType::const_iterator I = ent.find("id");
    if (!I->second.IsString()) {
        std::cerr << "Got Disappearance of non-string ID" << std::endl << std::flush;
        return;
    }
    const std::string & id = I->second.AsString();
    std::cout << "Disappearance(id: " << id << ")";
    I = ent.find("loc");
    if (I == ent.end()) {
        std::cout << std::endl << std::flush;
        return;
    }
    if (!I->second.IsString()) {
        std::cerr << " with non-string LOC" << std::endl << std::flush;
        return;
    }
    const std::string & loc = I->second.AsString();
    std::cout << " in " << loc << std::endl;
    if (loc == "lobby") {
        std::cout << id << " has logged out." << std::endl;
    }
    std::cout << std::flush;
}

void Interactive::ObjectArrived(const Atlas::Objects::Operation::Info& o)
{
    reply_flag = true;
    if (o.GetArgs().empty()) {
        return;
    }
    const Atlas::Message::Object::MapType & ent = o.GetArgs().front().AsMap();
    Atlas::Message::Object::MapType::const_iterator I;
    if (state == INIT) {
        I = ent.find("id");
        if (I == ent.end() || !I->second.IsString()) {
            std::cerr << "ERROR: Response to login does not contain account id"
                      << std::endl << std::flush;
            
        } else {
            state = LOGGED_IN;
            accountId = I->second.AsString();
        }
    } else if (state == LOGGED_IN) {
        std::cout << "Info(" << std::endl;
        for (I = ent.begin(); I != ent.end(); I++) {
            const Atlas::Message::Object & item = I->second;
            std::cout << "     " << I->first << ": ";
            output(item);
            std::cout << std::endl;
        }
        std::cout << ")" << std::endl << std::flush;
        // Display results of command
    }
}

void Interactive::ObjectArrived(const Atlas::Objects::Operation::Error& o)
{
    reply_flag = true;
    error_flag = true;
    std::cout << "Error(";
    const Atlas::Message::Object::ListType & args = o.GetArgs();
    const Atlas::Message::Object & arg = args.front();
    if (arg.IsString()) {
        std::cout << arg.AsString();
    } else if (arg.IsMap()) {
        std::cout << arg.AsMap().find("message")->second.AsString();
    }
    std::cout << ")" << std::endl << std::flush;
}

void Interactive::ObjectArrived(const Atlas::Objects::Operation::Sight& o)
{
    reply_flag = true;
    std::cout << "Sight(" << std::endl;
    const Atlas::Message::Object::MapType & ent = o.GetArgs().front().AsMap();
    Atlas::Message::Object::MapType::const_iterator I;
    for (I = ent.begin(); I != ent.end(); I++) {
        const Atlas::Message::Object & item = I->second;
        std::cout << "      " << I->first << ":";
        output(item);
        std::cout << std::endl;
    }
    std::cout << ")" << std::endl << std::flush;
}

void Interactive::ObjectArrived(const Atlas::Objects::Operation::Sound& o)
{
    reply_flag = true;
    const Atlas::Message::Object::MapType & arg = o.GetArgs().front().AsMap();
    Atlas::Message::Object::MapType::const_iterator I = arg.find("from");
    if (I == arg.end() || !I->second.IsString()) {
        std::cout << "Sound arg has no from" << std::endl << std::flush;
        return;
    }
    const std::string & from = I->second.AsString();
    I = arg.find("args");
    if (I == arg.end() || !I->second.IsList()
                       || I->second.AsList().empty()
                       || !I->second.AsList().front().IsMap()) {
        std::cout << "Sound arg has no args" << std::endl << std::flush;
        return;
    }
    const Atlas::Message::Object::MapType & ent = I->second.AsList().front().AsMap();
    I = ent.find("say");
    if (I == ent.end() || !I->second.IsString()) {
        std::cout << "Sound arg arg has no say" << std::endl << std::flush;
        return;
    }
    const std::string & say = I->second.AsString();
    std::cout << "[" << from << "] " << say
              << std::endl << std::flush;
}

SigC::Signal1<void, char *> CmdLine;

void Interactive::gotCommand(char * cmd)
{
    CmdLine.emit(cmd);
}

void Interactive::runCommand(char * cmd)
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

void Interactive::loop()
{
    rl_callback_handler_install("cyphesis> ", &Interactive::gotCommand);
    SigC::Connection c = CmdLine.connect(SigC::slot(*this, &Interactive::runCommand));
    while (!exit) {
        poll();
    };
    c.disconnect();
    rl_callback_handler_remove();
}

void Interactive::poll()
// Poll the codec if select says there is something there.
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
                codec->Poll();
                rl_forced_update_display();
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
    std::cin >> username;
    std::cout << "Password: " << std::flush;
    std::cin >> password;
}

bool Interactive::connect(const std::string & host)
{
    std::cout << "Connecting... " << std::flush;
    ios.open(host, port_num);
    if (!ios.is_open()) {
        std::cout << "failed." << std::endl << std::flush;
        return false;
    }
    std::cout << "done." << std::endl << std::flush;
    cli_fd = ios.getSocket();

    // Do client negotiation with the server
    Atlas::Net::StreamConnect conn("cycmd", ios, this);

    std::cout << "Negotiating... " << std::flush;
    while (conn.GetState() == Atlas::Negotiate<std::iostream>::IN_PROGRESS) {
        // conn.Poll() does all the negotiation
        conn.Poll();
    }
    std::cout << "done." << std::endl << std::flush;

    // Check whether negotiation was successful
    if (conn.GetState() == Atlas::Negotiate<std::iostream>::FAILED) {
        std::cerr << "Failed to negotiate." << std::endl;
        return false;
    }
    // Negotiation was successful

    // Get the codec that negotiation established
    codec = conn.GetCodec();

    // Create the encoder
    encoder = new Atlas::Objects::Encoder(codec);

    // Send whatever codec specific data marks the beginning of a stream
    codec->StreamBegin();
    return true;

}

bool Interactive::login()
{
    Atlas::Objects::Entity::Account account = Atlas::Objects::Entity::Account::Instantiate();
    Atlas::Objects::Operation::Login l = Atlas::Objects::Operation::Login::Instantiate();
    error_flag = false;
    reply_flag = false;
 
    account.SetAttr("username", username);
    account.SetAttr("password", password);
 
    Atlas::Message::Object::ListType args(1,account.AsObject());
 
    l.SetArgs(args);
 
    encoder->StreamMessage(&l);

    ios << std::flush;
 
    while (!reply_flag) {
       codec->Poll();
    }

    if (!error_flag) {
       return true;
    }
    return false;
}

void Interactive::exec(const std::string & cmd, const std::string & arg)
{
    bool reply_expected = true;
    reply_flag = false;
    error_flag = false;

    if (cmd == "stat") {
        Get g = Get::Instantiate();
        encoder->StreamMessage(&g);
    } else if (cmd == "look") {
        Look l = Look::Instantiate();
        l.SetFrom(accountId);
        encoder->StreamMessage(&l);
    } else if (cmd == "logout") {
        Logout l = Logout::Instantiate();
        l.SetFrom(accountId);
        if (!arg.empty()) {
            Atlas::Message::Object::MapType lmap;
            lmap["id"] = arg;
            l.SetArgs(Atlas::Message::Object::ListType(1,lmap));
            reply_expected = false;
        }
        encoder->StreamMessage(&l);
    } else if (cmd == "say") {
        Talk t = Talk::Instantiate();
        Atlas::Message::Object::MapType ent;
        ent["say"] = arg;
        t.SetArgs(Atlas::Message::Object::ListType(1,ent));
        t.SetFrom(accountId);
        encoder->StreamMessage(&t);
    } else if ((cmd == "help") || (cmd == "?")) {
        reply_expected = false;
        help();
    } else if (cmd == "query") {
        Get g = Get::Instantiate();

        Atlas::Message::Object::MapType cmap;
        cmap["objtype"] = "object";
        if (!arg.empty()) {
            cmap["id"] = arg;
        }
        g.SetArgs(Atlas::Message::Object::ListType(1,cmap));
        g.SetFrom(accountId);

        encoder->StreamMessage(&g);
    } else if (cmd == "get") {
        Get g = Get::Instantiate();

        Atlas::Message::Object::MapType cmap;
        cmap["objtype"] = "class";
        if (!arg.empty()) {
            cmap["id"] = arg;
        }
        g.SetArgs(Atlas::Message::Object::ListType(1,cmap));
        g.SetFrom(accountId);

        encoder->StreamMessage(&g);
    } else {
        reply_expected = false;
        std::cout << cmd << ": Command not know" << std::endl << std::flush;
    }

    ios << std::flush;

    if (!reply_expected) { return; }
    // Wait for reply
    // FIXME Only wait a reasonable ammount of time
    while (!reply_flag) {
       codec->Poll();
    }
}

static void usage(char * prg)
{
    std::cout << "usage: " << prg << " [ server [ cmd ] ]" << std::endl << std::flush;
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
    char * server = "localhost";
    if (argc > 1) {
        if (argc == 3) {
            cmd = argv[2];
            interactive = false;
        } else if (argc > 3) {
            usage(argv[0]);
            return 1;
        }
        server = argv[1];
    }
    Interactive bridge;
    if (!bridge.connect(server)) {
        return 1;
    }
    Atlas::Message::Object::MapType adminAccount;
    if (strcmp(server, "localhost") == 0) {
        bridge.setUsername("admin");
        if (!AccountBase::instance()->getAccount("admin", adminAccount)) {
            std::cerr << "WARNING: Unable to read admin account from database"
                      << std::endl << "Using default"
                      << std::endl << std::flush;
            bridge.setPassword(consts::defaultAdminPassword);
        } else {
            Atlas::Message::Object::MapType::const_iterator I = adminAccount.find("password");
            if (I == adminAccount.end()) {
                std::cerr << "WARNING: Admin account has no password"
                          << std::endl << "Using default"
                          << std::endl << std::flush;
                bridge.setPassword(consts::defaultAdminPassword);
            } else {
                bridge.setPassword(I->second.AsString());
            }
        }
        AccountBase::del();
    } else {
        bridge.getLogin();
    }
    std::cout << "Logging in... " << std::flush;
    if (!bridge.login()) {
        std::cout << "failed." << std::endl << std::flush;
        return 1;
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
