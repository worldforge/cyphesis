// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#include <Atlas/Objects/Encoder.h>
#include <Atlas/Net/Stream.h>
#include <Atlas/Objects/Decoder.h>
#include <Atlas/Codec.h>
#include <Atlas/Objects/Entity/Account.h>
#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Objects/Operation/Get.h>

#include <common/Load.h>
#include <common/Save.h>
#include <common/accountbase.h>
#include <common/const.h>

#include <skstream.h>

#include <cstdio>

#include <readline/readline.h>
#include <readline/history.h>

using Atlas::Message::Object;
using Atlas::Objects::Operation::Get;
using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Load;
using Atlas::Objects::Operation::Save;

static void help()
{
    std::cout << "Cyphesis commands:" << std::endl << std::endl;
    std::cout << "    stat	Return current server status" << std::endl;
    std::cout << "    load	Load world state from database status" << std::endl;
    std::cout << "    save	Save world state to database status" << std::endl;
    std::cout << "    shutdown	Initiate server shutdown" << std::endl<< std::endl;
    std::cout << "Other commands will be passed on to the server using a set operation" << std::endl << std::flush;
}

class Interactive : public Atlas::Objects::Decoder
{
  private:
    bool error_flag, reply_flag;
    int cli_fd;
    Atlas::Objects::Encoder * encoder;
    Atlas::Codec<std::iostream> * codec;
    client_socket_stream ios;
    std::string password;
    enum {
       INIT,
       LOGGED_IN
    };
    int state;
  protected:
    //void UnknownObjectArrived(const Object&);
    void ObjectArrived(const Atlas::Objects::Operation::Info&);
    void ObjectArrived(const Atlas::Objects::Operation::Error&);

  public:
    Interactive() : error_flag(false), reply_flag(false), encoder(NULL),
                       codec(NULL), state(INIT) { }

    void send(const Atlas::Objects::Operation::RootOperation &);
    bool connect(const std::string & host);
    bool login();
    void exec(const std::string & cmd, const std::string & arg);
    void loop();
    void getPassword();
    void prompt();

    void setPassword(const std::string & passwd) {
        password = passwd;
    }
};

void Interactive::ObjectArrived(const Atlas::Objects::Operation::Info& o)
{
    reply_flag = true;
    std::cout << "An info operation arrived." << std::endl << std::flush;
    if (state == INIT) {
        state = LOGGED_IN;
    } else if (state == LOGGED_IN) {
        const Object::MapType & ent = o.GetArgs().front().AsMap();
        Object::MapType::const_iterator I;
        for (I = ent.begin(); I != ent.end(); I++) {
            const Object & item = I->second;
            switch (item.GetType()) {
                case Object::TYPE_INT:
                    std::cout << "    " << I->first << ": " << item.AsInt() << std::endl;
                    break;
                case Object::TYPE_FLOAT:
                    std::cout << "    " << I->first <<": " << item.AsFloat() << std::endl;
                    break;
                case Object::TYPE_STRING:
                    std::cout << "    " << I->first <<": "<< item.AsString() << std::endl;
                    break;
                case Object::TYPE_LIST:
                    std::cout << "    " << I->first << ": (list)" << std::endl;
                    break;
                case Object::TYPE_MAP:
                    std::cout << "    " << I->first << ": (map)" << std::endl;
                    break;
                default:
                    std::cout << "    " << I->first << ": (\?\?\?)" << std::endl;
                    break;
            }
                
        }
        // Display results of command
    }
}

void Interactive::ObjectArrived(const Atlas::Objects::Operation::Error& o)
{
    reply_flag = true;
    error_flag = true;
    std::cout << "Error from server:" << std::endl << std::flush;
    const Object::ListType & args = o.GetArgs();
    const Object & arg = args.front();
    if (arg.IsString()) {
        std::cout << arg.AsString() << std::endl << std::flush;
    } else if (arg.IsMap()) {
        std::cout << arg.AsMap().find("message")->second.AsString() << std::endl << std::flush;
    }
}

void Interactive::prompt()
{
    bool exit = false;
    while (!exit) {
        char * cmd = readline("cyphesis> ");
        if (cmd == NULL) {
            exit = true;
            break;
        }

        if (*cmd == 0) {
            free(cmd);
            continue;
        }

        add_history(cmd);

        char * arg = strchr(cmd, ' ');
        if (arg != NULL) {
            *arg++=0;
            int len = strlen(arg);
            while ((len > 0) && (arg[--len] == ' ')) { arg[len] = 0; }
        } else {
            arg = "";
        }

        //std::string command(cmd), argument(arg);
        
        exec(cmd, arg);
    }
}

void Interactive::loop()
// Poll the codec if select says there is something there.
{
    fd_set infds;
    struct timeval tv;

    FD_ZERO(&infds);

    FD_SET(cli_fd, &infds);

    tv.tv_sec = 1;
    tv.tv_usec = 0;

    int retval = select(cli_fd+1, &infds, NULL, NULL, &tv);

    if (retval) {
        if (FD_ISSET(cli_fd, &infds)) {
            if (ios.peek() == -1) {
                std::cout << "Server disconnected" << std::endl << std::flush;
                exit(1);
            }
            codec->Poll();
        }
    }
}

void Interactive::getPassword()
{
    // This needs to be re-written to hide input, so the password can be
    // secret
    std::cout << "Password: " << std::flush;
    std::cin >> password;
}

bool Interactive::connect(const std::string & host)
// This deals with icky low-level socket rubbish. All this should be rubbed
// and replaced with propper use of an iostream based socket library
{
    ios.open(host.c_str(), 6767);
    if (!ios.is_open()) {
        std::cout << "Connection failed." << std::endl << std::flush;
    }
    cli_fd = ios.getSocket();

    // Do client negotiation with the server
    Atlas::Net::StreamConnect conn("cycmd", ios, this);

    std::cout << "Negotiating... " << std::flush;
    while (conn.GetState() == Atlas::Negotiate<std::iostream>::IN_PROGRESS) {
        // conn.Poll() does all the negotiation
        conn.Poll();
    }
    std::cout << "done." << std::endl;

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
 
    account.SetAttr("id", std::string("admin"));
    account.SetAttr("password", password);
 
    Object::ListType args(1,account.AsObject());
 
    l.SetArgs(args);
 
    encoder->StreamMessage(&l);

    ios << std::flush;
 
    while (!reply_flag) {
       codec->Poll();
    }

    if (!error_flag) {
       std::cout << "login was a success" << std::endl << std::flush;
       return true;
    }
    std::cout << "login failed" << std::endl << std::flush;
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
    } else if (cmd == "load") {
        Load l = Load::Instantiate();
        l.SetFrom("admin");
        encoder->StreamMessage(&l);
    } else if (cmd == "save") {
        Save s = Save::Instantiate();
        s.SetFrom("admin");
        encoder->StreamMessage(&s);
    } else if (cmd == "help") {
        reply_expected = false;
        help();
    } else if (cmd == "query") {
        Get g = Get::Instantiate();

        Object::MapType cmap;
        cmap["id"] = "server";
        cmap["cmd"] = cmd;
        if (arg.size() != 0) {
            cmap["arg"] = arg;
        }
        g.SetArgs(Object::ListType(1,cmap));
        g.SetFrom("admin");

        encoder->StreamMessage(&g);
    } else {
        Set s = Set::Instantiate();

        Object::MapType cmap;
        cmap["id"] = "server";
        cmap["cmd"] = cmd;
        if (arg.size() != 0) {
            cmap["arg"] = arg;
        }
        s.SetArgs(Object::ListType(1,cmap));
        s.SetFrom("admin");

        encoder->StreamMessage(&s);
    }

    ios << std::flush;

    if (!reply_expected) { return; }
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
    Object::MapType adminAccount;
    if (strcmp(server, "localhost") == 0) {
        if (!AccountBase::instance()->getAccount("admin", adminAccount)) {
            std::cerr << "WARNING: Unable to read admin account from database"
                      << std::endl << "Using default"
                      << std::endl << std::flush;
            bridge.setPassword(consts::defaultAdminPassword);
        } else {
            Object::MapType::const_iterator I = adminAccount.find("password");
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
        bridge.getPassword();
    }
    if (!bridge.login()) {
        return 1;
    }
    if (!interactive) {
        bridge.exec(cmd, "");
        return 0;
    } else {
	bridge.prompt();
    }
    return 0;
}
