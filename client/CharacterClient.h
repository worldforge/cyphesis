// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#ifndef CLIENT_CHARACTER_CLIENT_H
#define CLIENT_CHARACTER_CLIENT_H

#include "rulesets/BaseMind.h"

class ClientConnection;

/// \brief Class to implement a character entity in an admin client
class CharacterClient : public BaseMind {
  protected:
    ClientConnection & m_connection;

    int sendAndWaitReply(const Operation &, OpVector &);
  public:
    CharacterClient(const std::string &, long, const std::string&, ClientConnection&);

    virtual void sightImaginaryOperation(const Operation &,
                                         Operation &,
                                         OpVector &);
    virtual void soundTalkOperation(const Operation &,
                                    Operation &,
                                    OpVector &);

    void send(const Operation & op);
};

#endif // CLIENT_CHARACTER_CLIENT_H
