// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Net/Stream.h>
#include <Atlas/Objects/Decoder.h>
#include <Atlas/Codecs/XML.h>
#include <Atlas/Objects/Entity/Account.h>
#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Objects/Operation/Get.h>

#include <common/Load.h>
#include <common/Save.h>

extern "C" {
    #include <stdio.h>
    #include <sys/time.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <unistd.h>
    #include <netdb.h>
}

#include <fstream>

extern "C" {
    #include <readline/readline.h>
    #include <readline/history.h>
};

using Atlas::Message::Object;
using Atlas::Objects::Operation::Get;
using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Load;
using Atlas::Objects::Operation::Save;

static void help()
{
    std::cout << "Cyphesis commands:" << endl << endl;
    std::cout << "    stat	Return current server status" << endl;
    std::cout << "    load	Load world state from database status" << endl;
    std::cout << "    save	Save world state to database status" << endl;
    std::cout << "    shutdown	Initiate server shutdown" << endl<< endl;
    std::cout << "Other commands will be passed on to the server using a set operation" << endl << flush;
}

class Interactive : public Atlas::Objects::Decoder
{
  private:
    bool error_flag, reply_flag;
    int cli_fd;
    Atlas::Objects::Encoder * encoder;
    Atlas::Codec<iostream> * codec;
    std::fstream * ios;
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
    void getpassword();
    void prompt();
};

void Interactive::ObjectArrived(const Atlas::Objects::Operation::Info& o)
{
    reply_flag = true;
    std::cout << "An info operation arrived." << endl << flush;
    if (state == INIT) {
        state = LOGGED_IN;
    } else if (state == LOGGED_IN) {
        const Object::MapType & ent = o.GetArgs().front().AsMap();
        Object::MapType::const_iterator I;
        for (I = ent.begin(); I != ent.end(); I++) {
            const Object & item = I->second;
            switch (item.GetType()) {
                case Object::TYPE_INT:
                    std::cout << "    " << I->first << ": " << item.AsInt() << endl;
                    break;
                case Object::TYPE_FLOAT:
                    std::cout << "    " << I->first <<": " << item.AsFloat() << endl;
                    break;
                case Object::TYPE_STRING:
                    std::cout << "    " << I->first <<": "<< item.AsString() << endl;
                    break;
                case Object::TYPE_LIST:
                    std::cout << "    " << I->first << ": (list)" << endl;
                    break;
                case Object::TYPE_MAP:
                    std::cout << "    " << I->first << ": (map)" << endl;
                    break;
                default:
                    std::cout << "    " << I->first << ": (???)" << endl;
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
    std::cout << "Error from server:" << endl << flush;
    const Object::ListType & args = o.GetArgs();
    const Object & arg = args.front();
    if (arg.IsString()) {
        std::cout << arg.AsString() << endl << flush;
    } else if (arg.IsMap()) {
        std::cout << arg.AsMap().find("message")->second.AsString() << endl << flush;
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
            if (ios->peek() == -1) {
                std::cout << "Server disconnected" << endl << flush;
                exit(1);
            }
            codec->Poll();
        }
    }
}

void Interactive::getpassword()
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
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));

    struct hostent * ms_addr = gethostbyname(host.c_str());
    if (ms_addr == NULL) {
        cerr << "server lookup failed." <<endl<<flush;
        return false;
    }
    memcpy(&sin.sin_addr, ms_addr->h_addr_list[0], ms_addr->h_length);

    cli_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (cli_fd < 0) {
        return false;
    }
    sin.sin_family = AF_INET;
    sin.sin_port = htons(6767);

    std::cout << "Connecting to cyphesis.." << endl << flush;

    if (::connect(cli_fd, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        std::cout << "Connection failed." << endl << flush;
        close(cli_fd);
        return false;
    }
    std::cout << "Connected to cyphesis." << endl << flush;
    // Connect to the server
    ios = new fstream(cli_fd);

    // Do client negotiation with the server
    Atlas::Net::StreamConnect conn("cycmd", *ios, this);

    std::cout << "Negotiating... " << flush;
    while (conn.GetState() == Atlas::Negotiate<iostream>::IN_PROGRESS) {
        // conn.Poll() does all the negotiation
        conn.Poll();
    }
    std::cout << "done." << endl;

    // Check whether negotiation was successful
    if (conn.GetState() == Atlas::Negotiate<iostream>::FAILED) {
        cerr << "Failed to negotiate." << endl;
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
 
    while (!reply_flag) {
       codec->Poll();
    }

    if (!error_flag) {
       std::cout << "login was a success" << endl << flush;
       return true;
    }
    std::cout << "login failed" << endl << flush;
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

    *ios << flush;

    if (!reply_expected) { return; }
    while (!reply_flag) {
       codec->Poll();
    }
}

static void usage(char * prg)
{
    std::cout << "usage: " << prg << " [ server [ cmd ] ]" << endl << flush;
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
        return 0;
    }
    bridge.getpassword();
    if (!bridge.login()) {
        return 0;
    }
    if (!interactive) {
        bridge.exec(cmd, "");
        return 0;
    } else {
	bridge.prompt();
    }
}
