// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#ifndef CHARACTER_CLIENT_H
#define CHARACTER_CLIENT_H

#include "rulesets/BaseMind.h"

class ClientConnection;

class CharacterClient : public BaseMind {
  protected:
    ClientConnection & m_connection;

    bool findRefnoOp(const Atlas::Objects::Operation::RootOperation &,
                     long refno);
    bool findRefno(const Atlas::Objects::Operation::RootOperation &,
                   long refno);
    int sendAndWaitReply(Atlas::Objects::Operation::RootOperation &,
                         OpVector &);
  public:
    CharacterClient(const std::string&, const std::string&, ClientConnection&);

    virtual void sightImaginaryOperation(const Atlas::Objects::Operation::RootOperation &,
                                         Atlas::Objects::Operation::RootOperation &,
                                         OpVector &);
    virtual void soundTalkOperation(const Atlas::Objects::Operation::RootOperation &,
                                    Atlas::Objects::Operation::RootOperation &,
                                    OpVector &);

    void send(Atlas::Objects::Operation::RootOperation & op);
};

#endif // CHARACTER_CLIENT_H
