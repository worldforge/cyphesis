#include <Atlas/Message/Object.h>
#include <Atlas/Net/Stream.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Objects/Decoder.h>


#include <iostream.h>
#include <fstream.h>


extern "C" {
    #include <stdio.h>
    #include <sys/time.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
}

#include <common/const.h>
#include <common/log.h>

#include "server.h"

int profile_flag=0;

using namespace Atlas;


#if 0
void CommClient::destroy() {
    CommClient::server->remove_client(this);
    CommClient::server=NULL;
}

void CommClient::setup() {
    CommClient::client=Connection(self);
    if (consts::debug_level>=1) {
        log_name=CommClient::layer.addr[1]+".log";
        cout << "consts::debug_level>=1, logging to" << log_name << endl << flush;
        CommClient::log_file=open(log_name,"w");
    }
}

void CommClient::message(bad_type msg) {
    reply=CommClient::client.message(msg);
    CommClient::send(reply);
}
#endif


bad_type CommClient::send(Objects::Operation::RootOperation * op)
{
    if (op) {
        encoder->StreamMessage(op);
        client_ios << flush;
    }
    return None;
}

int CommClient::read()
{
    if (client_ios) {
        codec->Poll();
        return(0);
    } else {
        return(-1);
    }
}

void CommClient::setup()
{
    Atlas::Net::StreamAccept accept("cyphesis", client_ios, this);

    cout << "Negotiating... " << flush;
    while (accept.GetState() == Negotiate<iostream>::IN_PROGRESS) {
        accept.Poll();
    }
    cout << "done" << endl;

    if (accept.GetState() == Negotiate<iostream>::FAILED) {
        cerr << "Failed to negotiate" << endl;
        exit(2);
    }
    // Negotiation was successful

    // Get the codec that negotiation established
    codec = accept.GetCodec();

    // This should always be sent at the beginning of a session

    encoder = new Objects::Encoder(codec);

    codec->StreamBegin();

    client=new Connection(this);
}

void CommClient::message(const Objects::Operation::RootOperation & obj)
{
    Objects::Operation::RootOperation * reply = client->message(obj);
    if (NULL!=reply) {
        cout << "sending reply" << endl << flush;
        send(reply);
    }
}

void CommClient::UnknownObjectArrived(const Atlas::Message::Object& o)
{
#if 0
    cout << "An unknown has arrived." << endl << flush;
    if (o.IsMap()) {
        for(Message::Object::MapType::const_iterator I = o.AsMap().begin();
		I != o.AsMap().end();
		I++) {
		cout << I->first << endl << flush;
                if (I->second.IsString()) {
		    cout << I->second.AsString() << endl << flush;
                }
	}
    } else {
        cout << "Its not a map." << endl << flush;
    }
#endif
}

void CommClient::ObjectArrived(const Objects::Operation::Login & obj)
{
    cout << "A login object thingy here!" << endl << flush;
    message(obj);
}

void CommClient::ObjectArrived(const Objects::Operation::Create & obj)
{
    cout << "A create object thingy here!" << endl << flush;
    message(obj);
}

void CommClient::ObjectArrived(const Objects::Operation::Move & obj)
{
    cout << "A move object thingy here!" << endl << flush;
    message(obj);
}

void CommClient::ObjectArrived(const Objects::Operation::Set & obj)
{
    cout << "A set object thingy here!" << endl << flush;
    message(obj);
}

void CommClient::ObjectArrived(const Objects::Operation::Touch & obj)
{
    cout << "A touch object thingy here!" << endl << flush;
    message(obj);
}

void CommClient::ObjectArrived(const Objects::Operation::Look & obj)
{
    cout << "A look object thingy here!" << endl << flush;
    message(obj);
}

int CommServer::setup(int port)
{
    struct sockaddr_in sin;

    server_port = port;
    server_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        return(-1);
    }
    setsockopt(server_fd, 6, SO_REUSEADDR, NULL, 0);
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = 0L;
    if (bind(server_fd, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        close(server_fd);
        return(-1);
    }
    listen(server_fd, 5);
    cout << "Listening... " << flush;
    server=new ServerRouting(this,"server");
    return(0);
}

inline int CommClient::peek()
{
    return client_ios.peek();
}

int CommServer::accept() {
    struct sockaddr_in sin;
    unsigned int addr_len = sizeof(sin);

    sin.sin_family = AF_INET;
    sin.sin_port = htons(server_port);
    sin.sin_addr.s_addr = 0L;

    cout << "Accepting.." << endl << flush;
    int asockfd = ::accept(server_fd, (struct sockaddr *)&sin, &addr_len);

    if (asockfd < 0) {
        return(-1);
    }
    cout << "Accepted" << endl << flush;
    CommClient * newcli = new CommClient(this, asockfd, sin.sin_port);
    newcli->setup();
    clients.insert(std::pair<int, CommClient *>(asockfd, newcli));
    return(0);
}

void CommServer::loop() {
    fd_set sock_fds;
    int highest = server_fd;
    int client_fd;
    CommClient * client;
    struct timeval tv;

    tv.tv_sec=0;
    tv.tv_usec=100000;

    FD_ZERO(&sock_fds);

    FD_SET(server_fd, &sock_fds);
    client_map_t::const_iterator I;
    for(I = clients.begin(); I != clients.end(); I++) {
       client_fd = I->first;
       FD_SET(client_fd, &sock_fds);
       if (client_fd > highest) {
           highest=client_fd;
       }
    }
    highest++;
    int rval = select(highest, &sock_fds, NULL, NULL, &tv);

    if (rval < 0) {
        return;
    }
    
    for(I = clients.begin(); I != clients.end(); I++) {
       client_fd = I->first;
       if (FD_ISSET(client_fd, &sock_fds)) {
           client = I->second;
           if (client->peek() == -1) {
               remove_client(client);
               cout << "Client disconnected. Handle it here" << endl << flush;
               exit(1);
           } else {
               client->read();
           }
       }
    }
    if (FD_ISSET(server_fd, &sock_fds)) {
        cout << "selected on server" << endl << flush;
        accept();
    }
    idle();
}

void CommServer::remove_client(CommClient * client) {
    remove_client(client,"You caused exception. Connection closed");
}

void CommServer::remove_client(CommClient * client, char * error_msg) {
    /* 
    err=Entity(message=error_msg);
    if (client) {
        try {
            client.send(Operation("error",err));
            client.client.comm_client.layer.socket.close();
        }
        catch (socket.error) {
            pass;
        }
        try {
            self.clients.remove(client);
        }
        catch (ValueError) {
            pass;
        }
    }
    */
}

void CommServer::idle() {
    server->idle();
}



int main(int argc, char ** argv) {
    if (consts::debug_level>=1) {
        cout << "consts::debug_level>=1, logging to cyphesis_server*.log files" << endl << flush;
	//ofstream log_stream("cyphesis_server.log",ios::out);
        log::inform_fp.open("cyphesis_server.log",ios::out);
        log::debug_fp.open("cyphesis_server_debug.log",ios::out);
    }
    if (consts::debug_thinking>=1) {
        char * log_name="thinking.log";
        cout << "consts::debug_thinking>=1:, logging to" << log_name << endl;
        log::thinking_fp.open(log_name,ios::out);
    }
    CommServer s;
    if (s.setup(6767)) {
        cout << "BOOM" << endl << flush;
        exit(1);
    }
    if (profile_flag) {
        s.loop_max=1000;
    }
    if (consts::debug_level>=1) {
        char * log_name="cyphesis_world.log";
        cout << "consts::debug_level>=1:, logging to" << log_name << endl;
        //s.server.world.queue_fp.open(log_name,ios::out);
    }
    int exit_flag=0;
    while (!exit_flag) {
        //try {
            //try {
                if (profile_flag) {
                    //profile.run('s.loop()', 'server.prof');
                } else {
                    s.loop();
                }
            //}
            //catch (KeyboardInterrupt) {
                //print "Exit requested";
                //exit_flag=1;
                //raise;
            //}
        //}
        //catch (ConnectionError) {
            //print "Removing disconnected client", s.current_client;
            //s.remove_client(s.current_client);
        //}
        //catch () {
            //fp = StringIO();
            //traceback.print_exc(None,fp);
            //error_msg = string.replace(fp.getvalue(),"\n","[newline]");
            //fp.close();
//
            //if (init.security_flag and not exit_flag) {
                //print "Exception at client",\;
                      //s.current_client;
            //}
            //else {
                //exit_flag=0;
                //traceback.print_exc();
                //info=sys.exc_info();
                //print "Use --security argument to continue on errors!";
                //print "To continue now: press q<enter>";
                //print "Press (again) ctrl-c to exit";
                //pdb.post_mortem(info[2]);
            //}
            //s.remove_client(s.current_client, error_msg);
    }
}
