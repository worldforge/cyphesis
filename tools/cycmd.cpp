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

void help()
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
    bool connect(const string & host);
    bool login();
    void exec(const string & cmd, const string & arg);
    void loop();
    void getpassword();
    void prompt();
};

void Interactive::ObjectArrived(const Atlas::Objects::Operation::Info& o)
{
    reply_flag = true;
    cout << "An info operation arrived." << endl << flush;
    if (state == INIT) {
        state = LOGGED_IN;
    } else if (state == LOGGED_IN) {
        const Object::MapType & ent = o.GetArgs().front().AsMap();
        Object::MapType::const_iterator I;
        for (I = ent.begin(); I != ent.end(); I++) {
            const Object & item = I->second;
            switch (item.GetType()) {
                case Object::TYPE_INT:
                    cout << "    " << I->first << ": " << item.AsInt() << endl;
                    break;
                case Object::TYPE_FLOAT:
                    cout << "    " << I->first <<": " << item.AsFloat() << endl;
                    break;
                case Object::TYPE_STRING:
                    cout << "    " << I->first <<": "<< item.AsString() << endl;
                    break;
                case Object::TYPE_LIST:
                    cout << "    " << I->first << ": (list)" << endl;
                    break;
                case Object::TYPE_MAP:
                    cout << "    " << I->first << ": (map)" << endl;
                    break;
                default:
                    cout << "    " << I->first << ": (???)" << endl;
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
    cout << "Error from server:" << endl << flush;
    const Object::ListType & args = o.GetArgs();
    const Object & arg = args.front();
    if (arg.IsString()) {
        cout << arg.AsString() << endl << flush;
    } else if (arg.IsMap()) {
        cout << arg.AsMap().find("message")->second.AsString() << endl << flush;
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

        //string command(cmd), argument(arg);
        
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
                cout << "Server disconnected" << endl << flush;
                exit(1);
            }
            codec->Poll();
        }
    }
}

void Interactive::getpassword()
{
    std::cout << "Password: " << std::flush;
    std::cin >> password;
}

bool Interactive::connect(const string & host)
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

    cout << "Connecting to cyphesis.." << endl << flush;

    if (::connect(cli_fd, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        cout << "Connection failed." << endl << flush;
        close(cli_fd);
        return false;
    }
    cout << "Connected to cyphesis." << endl << flush;
    // Connect to the server
    ios = new fstream(cli_fd);

    // Do client negotiation with the server
    Atlas::Net::StreamConnect conn("cycmd", *ios, this);

    cout << "Negotiating... " << flush;
    while (conn.GetState() == Atlas::Negotiate<iostream>::IN_PROGRESS) {
        // conn.Poll() does all the negotiation
        conn.Poll();
    }
    cout << "done." << endl;

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
 
    account.SetAttr("id", string("admin"));
    account.SetAttr("password", password);
 
    Object::ListType args(1,account.AsObject());
 
    l.SetArgs(args);
 
    encoder->StreamMessage(&l);
 
    while (!reply_flag) {
       codec->Poll();
    }

    if (!error_flag) {
       cout << "login was a success" << endl << flush;
       return true;
    }
    cout << "login failed" << endl << flush;
    return false;
}

void Interactive::exec(const string & cmd, const string & arg)
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

void usage(char * prg)
{
    std::cout << "usage: " << prg << " cmd [ server ]" << endl << flush;
    exit(0);
}

int main(int argc, char ** argv)
{
    bool interactive = false;
    string cmd;
    char * server = "localhost";
    if (argc != 1) {
        if (argc != 2) {
            server = argv[2];
        }
        cmd = argv[1];
    } else {
        interactive = true;
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
