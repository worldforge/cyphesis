#ifndef SERVER_H
#define SERVER_H

#include <fstream>

#include <common/const.h>

typedef int bad_type; // Remove this to get unset type reporting

extern int profile_flag;

// ClientHandler is an libatlaspy class

class CommServer;

#include "ServerRouting.h"
#include "Connection.h"

using namespace Atlas;

class CommClient : Objects::Decoder {
    int client_fd;
    bad_type layer;
    ofstream log_file;
    fstream client_ios;
    Codec<iostream> * codec;
    Objects::Encoder * encoder;
    Connection * client;

  protected:
    virtual void UnknownObjectArrived(const Atlas::Message::Object&);
    virtual void ObjectArrived(const Objects::Operation::Login & obj);
    virtual void ObjectArrived(const Objects::Operation::Create & obj);
    virtual void ObjectArrived(const Objects::Operation::Move & obj);
    virtual void ObjectArrived(const Objects::Operation::Set & obj);
    virtual void ObjectArrived(const Objects::Operation::Touch & obj);
    virtual void ObjectArrived(const Objects::Operation::Look & obj);

  public:
    CommServer * server;

    CommClient(CommServer * svr, int fd, int port) :
		client_fd(fd), client_ios(fd), server(svr) {
        if (consts::debug_level>=1) {
            char * log_name = "log.log";
            //const char * log_name = port+".log";
            //cout << log_name;
            log_file.open(log_name);
        }
    }
    virtual ~CommClient() { }
    int read();
    bad_type send(const Objects::Operation::RootOperation *);
    void message(const Objects::Operation::RootOperation &);
    //void destroy();
    void setup();
    int peek();
};

// BaseServer is an libatlaspy class
typedef std::map<int, CommClient *> client_map_t;

class CommServer {
    int server_fd;
    int server_port;
    bad_type id;
    client_map_t clients;

    int accept();
    void idle();

  public:
    ServerRouting * server;
    int loop_max;

    int setup(int port);
    void loop();
    void remove_client(CommClient * client, char * msg);
    void remove_client(CommClient * client);
};

int main(int argc, char ** argv);

#endif /* SERVER_H */
