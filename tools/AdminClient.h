// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#ifndef TOOLS_ADMIN_CLIENT_H
#define TOOLS_ADMIN_CLIENT_H

#include "common/const.h"
#include "common/globals.h"

#include <Atlas/Objects/Encoder.h>
#include <Atlas/Net/Stream.h>
#include <Atlas/Objects/Decoder.h>
#include <Atlas/Codec.h>
#include <Atlas/Objects/Entity/Account.h>
#include <Atlas/Objects/Operation/Appearance.h>
#include <Atlas/Objects/Operation/Combine.h>
#include <Atlas/Objects/Operation/Delete.h>
#include <Atlas/Objects/Operation/Feel.h>
#include <Atlas/Objects/Operation/Imaginary.h>
#include <Atlas/Objects/Operation/Listen.h>
#include <Atlas/Objects/Operation/Move.h>
#include <Atlas/Objects/Operation/Disappearance.h>
#include <Atlas/Objects/Operation/Smell.h>
#include <Atlas/Objects/Operation/Touch.h>
#include <Atlas/Objects/Operation/Divide.h>
#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Objects/Operation/Logout.h>
#include <Atlas/Objects/Operation/Get.h>
#include <Atlas/Objects/Operation/Look.h>
#include <Atlas/Objects/Operation/Talk.h>
#include <Atlas/Objects/Operation/Error.h>
#include <Atlas/Objects/Operation/Sound.h>

#include <varconf/Config.h>

#include "common/utility.h"
#include "common/Generic.h"

#include <skstream/skstream_unix.h>

#include <iostream>
#include <cstdio>

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
    AdminClient() : error_flag(false), reply_flag(false), login_flag(false),
                    encoder(0), codec(0), ios(0), exit(false) { }
    ~AdminClient() {
        if (encoder != 0) {
            delete encoder;
        }
        if (codec != 0) {
            delete codec;
        }
        if (ios != 0) {
            delete ios;
        }
    }

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
