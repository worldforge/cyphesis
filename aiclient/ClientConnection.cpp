// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include <Atlas/Codec.h>
#include <Atlas/Message/Object.h>
#include <Atlas/Net/Stream.h>
#include <Atlas/Objects/Decoder.h>
#include <Atlas/Objects/Encoder.h>

#include <varconf/Config.h>

#include <iostream.h>
#include <fstream.h>

extern "C" {
    #include <stdio.h>
    #include <stdlib.h>
    #include <errno.h>
    #include <fcntl.h>
    #include <sys/time.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <sys/stat.h>
    #include <unistd.h>
    #include <netdb.h>
}

#include "ClientConnection.h"

#include "common/debug.h"

#include "config.h"

static bool debug_flag = false;

using Atlas::Message::Object;

ClientConnection::ClientConnection() :
    client_fd(-1), client_buf(NULL), ios(NULL), encoder(NULL)
{
}

ClientConnection::~ClientConnection()
{
    if (encoder != NULL) {
        delete encoder;
    }
}

void ClientConnection::operation(const RootOperation&)
{
}

void ClientConnection::ObjectArrived(const Error&)
{
    cout << "ERROR" << endl << flush;
    reply_flag = true;
    error_flag = true;
}

void ClientConnection::ObjectArrived(const Info&)
{
    cout << "INFO" << endl << flush;
    reply_flag = true;
    error_flag = false;
}

void ClientConnection::ObjectArrived(const Sight&)
{
}

void ClientConnection::ObjectArrived(const Sound&)
{
}

void ClientConnection::ObjectArrived(const Touch&)
{
}

void ClientConnection::ObjectArrived(const Appearance&)
{
}

void ClientConnection::ObjectArrived(const Disappearance&)
{
}


int ClientConnection::read() {
    if (*ios) {
        codec->Poll();
        return(0);
    } else {
        return(-1);
    }
}

bool ClientConnection::connect(const string & server)
{
    struct sockaddr_in serv_sa;

    cout << "Connecting to " << server << endl << flush;
    memset(&serv_sa, 0, sizeof(serv_sa));
    serv_sa.sin_family = AF_INET;
    serv_sa.sin_port = htons(6767);

    struct hostent * serv_addr = gethostbyname(server.c_str());
    if (serv_addr == NULL) {
        cerr << "ERROR: Lookup failed for " << server << endl;
        return false;
    }
    memcpy(&serv_sa.sin_addr, serv_addr->h_addr_list[0], serv_addr->h_length);

    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) {
        cerr << "ERROR: Could not create server connection" <<endl<<flush;
        perror("socket");
        return false;
    }

    int ret;
    ret = ::connect(client_fd, (struct sockaddr *)&serv_sa, sizeof(serv_sa));
    if (ret < 0) {
        cerr << "ERROR: Could not connect to " << server<<"." << endl << flush;
        perror("connect");
        return false;
    }

    client_buf = new sockbuf(client_fd);
    ios = new iostream(client_buf);

    Atlas::Net::StreamConnect conn("cyphesis_aiclient", *ios, this);

    cout << "Negotiating... " << flush;
    while (conn.GetState() == Atlas::Net::StreamConnect::IN_PROGRESS) {
      conn.Poll();
    }
    cout << "done" << endl;
  
    if (conn.GetState() == Atlas::Net::StreamConnect::FAILED) {
        cerr << "Failed to negotiate" << endl;
        return false;
    }

    codec = conn.GetCodec();

    encoder = new Atlas::Objects::Encoder(codec);

    codec->StreamBegin();

    return true;
}

bool ClientConnection::login(const string & account, const string & password)
{
    Atlas::Objects::Operation::Login l = Atlas::Objects::Operation::Login::Instantiate();
    Object::MapType acmap;
    acmap["id"] = account;
    acmap["password"] = password;

    l.SetArgs(Object::ListType(1,Object(acmap)));

    reply_flag = false;
    error_flag = false;
    send(l);
    return true;
}

bool ClientConnection::wait()
// Waits for response from server. Used when we are expecting a login response
// Return whether or not an error occured
{
   while (!reply_flag) {
      codec->Poll();
   }
   return error_flag;
}

void ClientConnection::send(const Atlas::Objects::Root & obj) {
    encoder->StreamMessage(&obj);
    *ios << flush;
}

void ClientConnection::error(const string & message) {
    // FIXME Need operation based error function
}

void ClientConnection::loop()
{
}
