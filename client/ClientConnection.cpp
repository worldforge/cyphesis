// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#include "ClientConnection.h"

#include "common/debug.h"
#include "common/globals.h"

#include <Atlas/Codecs/XML.h>
#include <Atlas/Net/Stream.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Objects/Operation/Error.h>
#include <Atlas/Objects/Operation/Appearance.h>
#include <Atlas/Objects/Operation/Disappearance.h>
#include <Atlas/Objects/Operation/Create.h>
#include <Atlas/Objects/Operation/Delete.h>
#include <Atlas/Objects/Operation/Combine.h>
#include <Atlas/Objects/Operation/Divide.h>
#include <Atlas/Objects/Operation/Feel.h>
#include <Atlas/Objects/Operation/Imaginary.h>
#include <Atlas/Objects/Operation/Listen.h>
#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Objects/Operation/Logout.h>
#include <Atlas/Objects/Operation/Look.h>
#include <Atlas/Objects/Operation/Move.h>
#include <Atlas/Objects/Operation/Perceive.h>
#include <Atlas/Objects/Operation/Perception.h>
#include <Atlas/Objects/Operation/Sight.h>
#include <Atlas/Objects/Operation/Smell.h>
#include <Atlas/Objects/Operation/Sniff.h>
#include <Atlas/Objects/Operation/Sound.h>
#include <Atlas/Objects/Operation/Touch.h>
#include <Atlas/Objects/Operation/Talk.h>

#include <sys/socket.h>
#include <sys/un.h>

static bool debug_flag = false;

ClientConnection::ClientConnection() :
    client_fd(-1), encoder(NULL), serialNo(512)
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
#if 0
    const std::string & from = op.getFrom();
    if (from.empty()) {
        std::cerr << "ERROR: Operation with no destination" << std::endl << std::flush;
        return;
    }
    dict_t::const_iterator I = objects.find(from);
    if (I == objects.end()) {
        std::cerr << "ERROR: Operation with invalid destination" << std::endl << std::flush;
        return;
    }
    OpVector res = I->second->message(op);
    OpVector::const_iterator J = res.begin();
    for(J = res.begin(); J != res.end(); ++J) {
        (*J)->setFrom(I->first);
        send(*(*J));
    }
#endif
}

void ClientConnection::objectArrived(const Error&op)
{
    debug(std::cout << "ERROR" << std::endl << std::flush;);
    push(op);
    error_flag = true;
}

void ClientConnection::objectArrived(const Info & op)
{
    debug(std::cout << "INFO" << std::endl << std::flush;);
    const std::string & from = op.getFrom();
    if (from.empty()) {
        reply_flag = true;
        error_flag = false;
        try {
            const Element & ac = op.getArgs().front();
            reply = ac.asMap();
            // const std::string & acid = reply["id"].asString();
            // objects[acid] = new ClientAccount(acid, *this);
        }
        catch (...) {
            std::cerr << "WARNING: Malformed account from server" << std::endl << std::flush;
        }
    } else {
        operation(op);
    }
}

void ClientConnection::objectArrived(const Action& op)
{
    push(op);
}

void ClientConnection::objectArrived(const Appearance& op)
{
    push(op);
}

void ClientConnection::objectArrived(const Combine& op)
{
    push(op);
}

void ClientConnection::objectArrived(const Atlas::Objects::Operation::Communicate& op)
{
    push(op);
}

void ClientConnection::objectArrived(const Create& op)
{
    push(op);
}

void ClientConnection::objectArrived(const Delete& op)
{
    push(op);
}

void ClientConnection::objectArrived(const Disappearance& op)
{
    push(op);
}

void ClientConnection::objectArrived(const Divide& op)
{
    push(op);
}

void ClientConnection::objectArrived(const Atlas::Objects::Operation::Feel& op)
{
    push(op);
}

void ClientConnection::objectArrived(const Get& op)
{
    push(op);
}

void ClientConnection::objectArrived(const Imaginary& op)
{
    push(op);
}

void ClientConnection::objectArrived(const Atlas::Objects::Operation::Listen& op)
{
    push(op);
}

void ClientConnection::objectArrived(const Login& op)
{
    push(op);
}

void ClientConnection::objectArrived(const Logout& op)
{
    push(op);
}

void ClientConnection::objectArrived(const Look& op)
{
    push(op);
}

void ClientConnection::objectArrived(const Move& op)
{
    push(op);
}

void ClientConnection::objectArrived(const Atlas::Objects::Operation::Perceive& op)
{
    push(op);
}

void ClientConnection::objectArrived(const Atlas::Objects::Operation::Perception& op)
{
    push(op);
}

void ClientConnection::objectArrived(const RootOperation& op)
{
    push(op);
}

void ClientConnection::objectArrived(const Set& op)
{
    push(op);
}

void ClientConnection::objectArrived(const Sight& op)
{
    push(op);
}

void ClientConnection::objectArrived(const Atlas::Objects::Operation::Smell& op)
{
    push(op);
}

void ClientConnection::objectArrived(const Atlas::Objects::Operation::Sniff& op)
{
    push(op);
}

void ClientConnection::objectArrived(const Sound& op)
{
    push(op);
}

void ClientConnection::objectArrived(const Talk& op)
{
    push(op);
}

void ClientConnection::objectArrived(const Touch& op)
{
    push(op);
}

int ClientConnection::read() {
    if (ios.is_open()) {
        codec->poll();
        return 0;
    } else {
        return -1;
    }
}

#define UNIX_PATH_MAX 108

int ClientConnection::connectLocal(const std::string & sockname)
{
    debug(std::cout << "Attempting local connect." << std::endl << std::flush;);
    std::string socket;
    if (sockname == "") {
        socket = var_directory + "/tmp/cyphesis.sock";
    } else if (sockname[0] != '/') {
        socket = var_directory + "/tmp/" + sockname;
    } else {
        socket = sockname;
    }

    struct sockaddr_un sun;
    sun.sun_family = AF_UNIX;
    strncpy(sun.sun_path, socket.c_str(), UNIX_PATH_MAX);

    int fd = ::socket(PF_UNIX, SOCK_STREAM, 0);

    if (0 != ::connect(fd, (struct sockaddr *)&sun, sizeof(sun))) {
        debug(std::cout << "Local connect refused" << std::endl << std::flush;);
        return -1;
    }

    ios.setSocket(fd);
    if (!ios.is_open()) {
        std::cerr << "ERROR: For some reason " << sockname << " not open."
                  << std::endl << std::flush;
        return -1;
    }

    client_fd = ios.getSocket();

    int ret = negotiate();

    if (ret == -1) {
        ios.close();
    }
    return ret;
}

int ClientConnection::connect(const std::string & server)
{
    debug(std::cout << "Connecting to " << server << std::endl << std::flush;);

    ios.open(server, port_num);
    if (!ios.is_open()) {
        std::cerr << "ERROR: Could not connect to " << server << "."
                  << std::endl << std::flush;
        return -1;
    }

    client_fd = ios.getSocket();

    return negotiate();
}

int ClientConnection::negotiate()
{
    Atlas::Net::StreamConnect conn("cyphesis_aiclient", ios, this);

    debug(std::cout << "Negotiating... " << std::flush;);
    while (conn.getState() == Atlas::Net::StreamConnect::IN_PROGRESS) {
      conn.poll();
    }
    debug(std::cout << "done" << std::endl;);
  
    if (conn.getState() == Atlas::Net::StreamConnect::FAILED) {
        std::cerr << "Failed to negotiate" << std::endl;
        return -1;
    }

    codec = conn.getCodec();

    encoder = new Atlas::Objects::Encoder(codec);

    codec->streamBegin();

    return 0;
}

void ClientConnection::login(const std::string & account,
                             const std::string & password)
{
    Login l;
    MapType acmap;
    acmap["username"] = account;
    acmap["password"] = password;

    l.setArgs(ListType(1,acmap));

    reply_flag = false;
    error_flag = false;
    send(l);
}

void ClientConnection::create(const std::string & account,
                              const std::string & password)
{
    Create c;
    MapType acmap;
    acmap["id"] = account;
    acmap["password"] = password;

    c.setArgs(ListType(1,acmap));

    reply_flag = false;
    error_flag = false;
    send(c);
}

int ClientConnection::wait()
// Waits for response from server. Used when we are expecting a login response
// Return whether or not an error occured
{
   error_flag = false;
   reply_flag = false;
   while (!reply_flag) {
      poll(1);
   }
   return error_flag ? -1 : 0;
}

void ClientConnection::send(RootOperation & op)
{
    /* debug(Atlas::Codecs::XML c((std::iostream&)std::cout, (Atlas::Bridge*)this);
          Atlas::Objects::Encoder enc(&c);
          enc.streamMessage(&op);
          std::cout << std::endl << std::flush;); */

    op.setSerialno(++serialNo);
    encoder->streamMessage(&op);
    ios << std::flush;
}

void ClientConnection::poll(int timeOut)
{
    fd_set infds;
    struct timeval tv;

    FD_ZERO(&infds);

    FD_SET(client_fd, &infds);

    tv.tv_sec = timeOut;
    tv.tv_usec = 0;

    int retval = select(client_fd+1, &infds, NULL, NULL, &tv);

    if (retval && (FD_ISSET(client_fd, &infds))) {
        if (ios.peek() == -1) {
            std::cerr << "Server disconnected" << std::endl << std::flush;
            error_flag = true;
            reply_flag = true;
            return;
        }
        codec->poll();
        return;
    }
    return;

}

RootOperation * ClientConnection::pop()
{
    poll();
    if (operationQueue.empty()) {
        return NULL;
    }
    RootOperation * op = operationQueue.front();
    operationQueue.pop_front();
    return op;
}

bool ClientConnection::pending()
{
    return !operationQueue.empty();
}

template<class O>
void ClientConnection::push(const O & op)
{
    reply_flag = true;
    RootOperation * new_op = new O(op); 
    operationQueue.push_back(new_op);
}
