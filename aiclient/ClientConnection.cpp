// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#include "ClientConnection.h"

#include "ClientAccount.h"

#include "common/debug.h"

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

static bool debug_flag = false;

using Atlas::Message::Element;

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

void ClientConnection::operation(const RootOperation & op)
{
    const std::string & from = op.getFrom();
    if (from.empty()) {
        cerr << "ERROR: Operation with no destination" << endl << flush;
        return;
    }
    dict_t::const_iterator I = objects.find(from);
    if (I == objects.end()) {
        cerr << "ERROR: Operation with invalid destination" << endl << flush;
        return;
    }
    oplist res = I->second->message(op);
    oplist::const_iterator Jend = res.end();
    for (oplist::const_iterator J = res.begin(); J != Jend; ++J) {
        (*J)->setFrom(I->first);
        send(*(*J));
    }
}

void ClientConnection::objectArrived(const Error&)
{
    cout << "ERROR" << endl << flush;
    reply_flag = true;
    error_flag = true;
}

void ClientConnection::objectArrived(const Info & op)
{
    cout << "INFO" << endl << flush;
    const std::string & from = op.getFrom();
    if (from.empty()) {
        reply_flag = true;
        error_flag = false;
        try {
            Object ac = op.getArgs().front();
            const std::string & acid = ac.asMap()["id"].asString();
            objects[acid] = new ClientAccount(acid, *this);
        }
        catch (...) {
            cerr << "WARNING: Malformed account from server" << endl << flush;
        }
    } else {
        operation(op);
    }
}

void ClientConnection::objectArrived(const Sight&)
{
}

void ClientConnection::objectArrived(const Sound&)
{
}

void ClientConnection::objectArrived(const Touch&)
{
}

void ClientConnection::objectArrived(const Appearance&)
{
}

void ClientConnection::objectArrived(const Disappearance&)
{
}


int ClientConnection::read() {
    if (*ios) {
        codec->poll();
        return 0;
    } else {
        return -1;
    }
}

bool ClientConnection::connect(const std::string & server)
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
      conn.poll();
    }
    cout << "done" << endl;
  
    if (conn.GetState() == Atlas::Net::StreamConnect::FAILED) {
        cerr << "Failed to negotiate" << endl;
        return false;
    }

    codec = conn.GetCodec();

    encoder = new Atlas::Objects::Encoder(codec);

    codec->streamBegin();

    return true;
}

bool ClientConnection::login(const std::string & account,
                             const std::string & password)
{
    Atlas::Objects::Operation::Login l;
    Element::MapType acmap;
    acmap["id"] = account;
    acmap["password"] = password;

    acName = account;

    l.setArgs(Element::ListType(1,Object(acmap)));

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
      codec->poll();
   }
   return error_flag;
}

void ClientConnection::send(const Atlas::Objects::Root & obj) {
    encoder->streamMessage(&obj);
    *ios << flush;
}

void ClientConnection::error(const std::string & message) {
    // FIXME Need operation based error function
}

void ClientConnection::loop()
{
}
