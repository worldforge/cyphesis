#ifndef SERVER_H
#define SERVER_H

#include <fstream>

#include <common/const.h>

typedef int bad_type; // Remove this to get unset type reporting

extern int profile_flag;

// ClientHandler is an libatlaspy class

class CommServer;

#include "ServerRouting.h"

using namespace Atlas;

class CommClient : Message::DecoderBase {
    CommServer * server;
    int client_fd;
    bad_type layer;
    ofstream log_file;
    fstream client_ios;
    Codec<iostream> * codec;
    Objects::Encoder * encoder;

  protected:
    void ObjectArrived(const Message::Object & obj);

  public:
#if 0
    CommClient(int fd, int port);
#else
    CommClient(int fd, int port) : client_fd(fd), client_ios(fd) {
        if (consts::debug_level>=1) {
            char * log_name = "log.log";
            //const char * log_name = port+".log";
            //cout << log_name;
            log_file.open(log_name);
        }
    }
#endif
    int read();
    //void destroy();
    void setup();
};

// BaseServer is an libatlaspy class
typedef std::map<int, CommClient *> client_map_t;

class CommServer {
    int accept();
    void idle();

    int server_fd;
    int server_port;
    bad_type id;
    client_map_t clients;

  public:
    int setup(int port);
    void loop();
    void remove_client(CommClient * client, char * msg);
    void remove_client(CommClient * client);

    int loop_max;
    ServerRouting * server;
    //bad_type * server;
};

int main(int argc, char ** argv);

#endif /* SERVER_H */
