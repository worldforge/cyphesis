// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#include "CharacterClient.h"

#include "ClientConnection.h"

#include "common/debug.h"

#include <Atlas/Objects/Operation/RootOperation.h>

static const bool debug_flag = false;

CharacterClient::CharacterClient(const std::string & id,
                                 const std::string & name,
                                 ClientConnection & c) :
                                 BaseMind(id, name), m_connection(c)
{
}

void CharacterClient::sightImaginaryOperation(const Sight &,
                                              Imaginary &, OpVector &)
{
}

void CharacterClient::soundTalkOperation(const Sound & , Talk &, OpVector &)
{
}

void CharacterClient::send(RootOperation & op)
{
    op.setFrom(getId());
    m_connection.send(op);
}

inline bool CharacterClient::findRefnoOp(const RootOperation & op, long refno)
{
    if (refno == op.getRefno()) {
        return true;
    }
    return false;
}

inline bool CharacterClient::findRefno(const RootOperation & msg, long refno)
{
    return findRefnoOp(msg,refno);
}

int CharacterClient::sendAndWaitReply(RootOperation & op, OpVector & res)
{
    send(op);
    long no = op.getSerialno();
    while (true) {
        if (m_connection.pending()) {
            RootOperation * input = CharacterClient::m_connection.pop();
            if (input != NULL) {
                // What the hell is this!
                OpVector result;
                operation(*input, result);
                OpVector::const_iterator I;
                for (I = result.begin(); I != result.end(); I++) {
                    send(*(*I));
                }
    
                if (findRefno(*input,no)) {
                    res.push_back(input);
                    return 0;
                }
                delete input;
            }
        } else if (m_connection.wait() != 0) {
            return -1;
        }
    }
}
