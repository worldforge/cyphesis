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

    bool findRefnoOp(const RootOperation & op, long refno);
    bool findRefno(const RootOperation & op, long refno);
    int sendAndWaitReply(RootOperation & op, OpVector &);
  public:
    CharacterClient(const std::string&, const std::string&, ClientConnection&);

    virtual void sightImaginaryOperation(const Sight&, Imaginary &, OpVector &);
    virtual void soundTalkOperation(const Sound &, Talk &, OpVector &);

    void send(RootOperation & op);
};

#endif // CHARACTER_CLIENT_H
