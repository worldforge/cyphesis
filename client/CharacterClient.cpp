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

void CharacterClient::sightImaginaryOperation(const Atlas::Objects::Operation::RootOperation &,
                                              Atlas::Objects::Operation::RootOperation &, OpVector &)
{
}

void CharacterClient::soundTalkOperation(const Atlas::Objects::Operation::RootOperation & ,
                                         Atlas::Objects::Operation::RootOperation &, OpVector &)
{
}

void CharacterClient::send(Atlas::Objects::Operation::RootOperation & op)
{
    op.setFrom(getId());
    m_connection.send(op);
}

inline bool CharacterClient::findRefnoOp(const Atlas::Objects::Operation::RootOperation & op, long refno)
{
    if (refno == op.getRefno()) {
        return true;
    }
    return false;
}

inline bool CharacterClient::findRefno(const Atlas::Objects::Operation::RootOperation & msg, long refno)
{
    return findRefnoOp(msg,refno);
}

int CharacterClient::sendAndWaitReply(Atlas::Objects::Operation::RootOperation & op, OpVector & res)
{
    send(op);
    long no = op.getSerialno();
    while (true) {
        if (m_connection.pending()) {
            Atlas::Objects::Operation::RootOperation * input = CharacterClient::m_connection.pop();
            if (input != NULL) {
                // What the hell is this!
                OpVector result;
                operation(*input, result);
                OpVector::const_iterator I = result.begin();
                OpVector::const_iterator Iend = result.end();
                for (; I != Iend; ++I) {
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
