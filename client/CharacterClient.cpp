// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#include "CharacterClient.h"

#include "ClientConnection.h"

#include "common/debug.h"

#include <Atlas/Objects/RootOperation.h>

static const bool debug_flag = false;

CharacterClient::CharacterClient(const std::string & id, long intId,
                                 const std::string & name,
                                 ClientConnection & c) :
                                 BaseMind(id, intId, name), m_connection(c)
{
}

void CharacterClient::sightImaginaryOperation(const Operation &,
                                              Operation &, OpVector &)
{
}

void CharacterClient::soundTalkOperation(const Operation & ,
                                         Operation &, OpVector &)
{
}

void CharacterClient::send(const Operation & op)
{
    op->setFrom(getId());
    m_connection.send(op);
}

int CharacterClient::sendAndWaitReply(const Operation & op, OpVector & res)
{
    send(op);
    debug(std::cout << "Waiting for reply to " << op->getParents().front()
                    << std::endl << std::flush;);
    long no = op->getSerialno();
    while (true) {
        if (m_connection.pending()) {
            Operation input = CharacterClient::m_connection.pop();
            assert(input.isValid());
            if (input.isValid()) {
                // FIXME What the hell is this!
#if 0
                // Should we really do this here?
                OpVector result;
                operation(*input, result);
                OpVector::const_iterator I = result.begin();
                OpVector::const_iterator Iend = result.end();
                for (; I != Iend; ++I) {
                    send(*(*I));
                }
#endif
    
                if (input->getRefno() == no) {
                    debug(std::cout << "Got reply" << std::endl << std::flush;);
                    res.push_back(input);
                    return 0;
                } else {
                    debug(std::cout << "Not reply" << std::endl << std::flush;);
                }
            } else {
                debug(std::cout << "Not op" << std::endl << std::flush;);
            }
        } else if (m_connection.wait() != 0) {
            return -1;
        }
    }
}
