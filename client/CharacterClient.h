// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#ifndef CHARACTER_CLIENT_H
#define CHARACTER_CLIENT_H

#include "rulesets/BaseMind.h"

class ClientConnection;

class CharacterClient : public BaseMind {
  protected:
    ClientConnection & connection;

    bool findRefnoOp(const RootOperation & op, long refno);
    bool findRefno(const RootOperation & op, long refno);
    OpVector sendAndWaitReply(RootOperation & op);
  public:
    CharacterClient(const std::string&, const std::string&, ClientConnection&);

    virtual OpVector sightImaginaryOperation(const Sight& op,
                                             Imaginary & sub_op);

    virtual OpVector soundTalkOperation(const Sound & op, Talk & sub_op);

    void send(RootOperation & op);
};

#endif // CHARACTER_CLIENT_H
