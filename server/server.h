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
    void UnknownObjectArrived(const Atlas::Message::Object&);
    void ObjectArrived(const Objects::Operation::Login & obj);
    void ObjectArrived(const Objects::Operation::Create & obj);
    void ObjectArrived(const Objects::Operation::Move & obj);
    void ObjectArrived(const Objects::Operation::Set & obj);
    void ObjectArrived(const Objects::Operation::Touch & obj);

  public:
    CommServer * server;

    CommClient(CommServer * svr, int fd, int port) :
		server(svr), client_fd(fd), client_ios(fd) {
        if (consts::debug_level>=1) {
            char * log_name = "log.log";
            //const char * log_name = port+".log";
            //cout << log_name;
            log_file.open(log_name);
        }
    }
    int read();
    bad_type send(Objects::Operation::RootOperation *);
    void message(const Objects::Operation::RootOperation &);
    //void destroy();
    void setup();
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
