// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Net/Stream.h>
#include <Atlas/Objects/Decoder.h>
#include <Atlas/Codecs/XML.h>
#include <Atlas/Objects/Entity/Account.h>
#include <Atlas/Objects/Operation/Login.h>

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

class Interactive : public Atlas::Objects::Decoder
{
  private:
    bool erflag;
    int cli_fd;
    int reply_flag;
    Atlas::Objects::Encoder * encoder;
    Atlas::Codec<iostream> * codec;
    fstream * ios;
    string account_id;
    string character_id;
    enum {
       INIT,
       LOGGED_IN
    };
    int state;
  protected:
    //void UnknownObjectArrived(const Atlas::Message::Object&);
    //void ObjectArrived(const Atlas::Objects::Operation::Info&);
    //void ObjectArrived(const Atlas::Objects::Operation::Error&);

  public:
    Interactive() : erflag(false), reply_flag(0), encoder(NULL),
                       codec(NULL), state(INIT) { }

    void send(const Atlas::Objects::Operation::RootOperation &);
    bool connect(const string & host);
    void login();
    void exec(const string & cmd, const string & arg);
    void loop();
    //void prompt();
};

#if 0
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
        
        if (exec(cmd, arg)) {
            getReply();
        }
    }
}
#endif

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
        close(cli_fd);
        return false;
    }
    cout << "Connected to cyphesis" << endl << flush;
    // Connect to the server
    ios = new fstream(cli_fd);

    // Do client negotiation with the server
    Atlas::Net::StreamConnect conn("cycmd", *ios, this);

    cout << "Negotiating... " << flush;
    while (conn.GetState() == Atlas::Negotiate<iostream>::IN_PROGRESS) {
        // conn.Poll() does all the negotiation
        conn.Poll();
    }
    cout << "done" << endl;

    // Check whether negotiation was successful
    if (conn.GetState() == Atlas::Negotiate<iostream>::FAILED) {
        cerr << "Failed to negotiate" << endl;
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

void Interactive::login()
{
   Atlas::Objects::Entity::Account account = Atlas::Objects::Entity::Account::Instantiate();
   Atlas::Objects::Operation::Login l = Atlas::Objects::Operation::Login::Instantiate();
   erflag = 0;
   reply_flag = 0;

   account.SetAttr("id", string("al"));
   account.SetAttr("password", string("ping"));

   Atlas::Message::Object::ListType args(1,account.AsObject());

   l.SetArgs(args);

   encoder->StreamMessage(&l);

   while (!reply_flag) {
      codec->Poll();
   }
   if (!erflag) {
      cout << "login was a success" << endl << flush;
      return;
   }
   cout << "login failed" << endl << flush;

}

void Interactive::exec(const string & cmd, const string & arg)
{
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
    char * server;
    if (argc != 1) {
        if (argc == 2) {
            server = "localhost";
        } else {
            server = argv[2];
        }
        cmd = argv[1];
    } else {
        interactive = true;
        cout << "Interactive mode as yet unsupported" << endl << flush;
        return 0;
    }
    Interactive bridge;
    if (!interactive) {
        if (!bridge.connect(server)) {
            return 0;
        }
        bridge.login();
        bridge.loop();
        bridge.exec(cmd, "");
        return 0;
    }
}
