// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#ifndef CHARACTER_CLIENT_H
#define CHARACTER_CLIENT_H

#include "rulesets/BaseMind.h"

class ClientConnection;

/// \brief Class to implement a character entity in an admin client
class CharacterClient : public BaseMind {
  protected:
    ClientConnection & m_connection;

    int sendAndWaitReply(Operation &, OpVector &);
  public:
    CharacterClient(const std::string&, const std::string&, ClientConnection&);

    virtual void sightImaginaryOperation(const Operation &,
                                         Operation &,
                                         OpVector &);
    virtual void soundTalkOperation(const Operation &,
                                    Operation &,
                                    OpVector &);

    void send(Operation & op);
};

#endif // CHARACTER_CLIENT_H
