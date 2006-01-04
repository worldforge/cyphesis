// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#ifndef TOOLS_ADMIN_CLIENT_H
#define TOOLS_ADMIN_CLIENT_H

#include "common/const.h"
#include "common/globals.h"

#include <Atlas/Objects/Decoder.h>
#include <Atlas/Objects/ObjectsFwd.h>

#include <skstream/skstream_unix.h>

#include <iostream>
#include <set>

#include <cstdio>

namespace Atlas {
  class Codec;
  namespace Objects { class ObjectsEncoder; }
}

typedef std::multimap<std::string, std::pair<std::pair<std::string, std::string>, Atlas::Message::MapType> > RuleWaitList;

class AdminClient : public Atlas::Objects::ObjectsDecoder
{
  private:
    bool error_flag, reply_flag, login_flag;
    int cli_fd;
    Atlas::Objects::ObjectsEncoder * encoder;
    Atlas::Codec * codec;
    basic_socket_stream * ios;
    std::string password;
    std::string username;
    std::string accountId;
    std::string m_errorMessage;
    bool exit;
    RuleWaitList m_waitingRules;
    std::set<std::string> m_uploadedRules;

    void output(const Atlas::Message::Element & item, bool recurse = true);
  protected:

    void objectArrived(const Atlas::Objects::Root &);

    void infoArrived(const Atlas::Objects::Operation::RootOperation &);
    void errorArrived(const Atlas::Objects::Operation::RootOperation &);

    int negotiate();

    void waitForInfo();
    int checkRule(const std::string & id);
  public:
    AdminClient();
    ~AdminClient();

    void send(const Atlas::Objects::Operation::RootOperation &);
    int connect(const std::string & host);
    int connect_unix(const std::string & host);
    int login();
    void loop();
    void poll();
    void getLogin();
    int uploadRule(const std::string & id, const std::string & set,
                    const Atlas::Message::MapType &);

    void setPassword(const std::string & passwd) {
        password = passwd;
    }

    void setUsername(const std::string & uname) {
        username = uname;
    }

    void report();
};

#endif // TOOLS_ADMIN_CLIENT_H
