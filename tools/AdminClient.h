// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#ifndef TOOLS_ADMIN_CLIENT_H
#define TOOLS_ADMIN_CLIENT_H

#include "common/const.h"
#include "common/globals.h"

#include "common/utility.h"
#include "common/Generic.h"

#include <Atlas/Objects/Decoder.h>
#include <Atlas/Codec.h>

#include <varconf/Config.h>

#include <skstream/skstream_unix.h>

#include <iostream>
#include <cstdio>

namespace Atlas { namespace Objects { class Encoder; } }

typedef std::multimap<std::string, std::pair<std::pair<std::string, std::string>, Atlas::Message::MapType> > RuleWaitList;

class AdminClient : public Atlas::Objects::Decoder
{
  private:
    bool error_flag, reply_flag, login_flag;
    int cli_fd;
    Atlas::Objects::Encoder * encoder;
    Atlas::Codec<std::iostream> * codec;
    basic_socket_stream * ios;
    std::string password;
    std::string username;
    std::string accountId;
    std::string m_errorMessage;
    bool exit;
    RuleWaitList m_waitingRules;

    void output(const Atlas::Message::Element & item, bool recurse = true);
  protected:

    void objectArrived(const Atlas::Objects::Operation::Info&);
    void objectArrived(const Atlas::Objects::Operation::Error&);

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
