// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#include "CharacterClient.h"
#include "ClientConnection.h"

CharacterClient::CharacterClient(const std::string & id,
                                 const std::string & name,
                                 ClientConnection & c) :
                                 BaseMind(id,name), connection(c)
{
}

oplist CharacterClient::sightImaginaryOperation(const Sight &, Imaginary &)
{
    return oplist();
}

oplist CharacterClient::soundTalkOperation(const Sound & op, Talk & subop)
{
    return oplist();
}

#if 0
//FIXME Dunno what all this trigger lark is about.
bad_type CharacterClient::add_trigger(bad_type event_name, bad_type func) {
    dictlist.add_value(CharacterClient::event_triggers,event_name, func);
}
bad_type CharacterClient::call_triggers(bad_type op) {
    event_name, sub_op = CharacterClient::get_op_name_and_sub(op);
    reply = Message();
    for (/*func in CharacterClient::event_triggers.get(event_name,[])*/) {
        reply = reply + func(op, sub_op);
    }
    return reply;
}
bad_type CharacterClient::set_from_op(bad_type op) {
    op.from_=self;
}
bad_type CharacterClient::set_from(bad_type msg) {
    CharacterClient::apply_to_operation(CharacterClient::set_from_op,msg);
}
#endif

void CharacterClient::send(RootOperation & op)
{
    op.SetFrom(getId());
    connection.send(op);
}

inline bool CharacterClient::findRefnoOp(const RootOperation & op, long refno)
{
    if (refno == op.GetRefno()) {
        return true;
    }
    return false;
}

inline bool CharacterClient::findRefno(const RootOperation & msg, long refno)
{
    return findRefnoOp(msg,refno);
}

oplist CharacterClient::sendAndWaitReply(RootOperation & op)
{
    send(op);
    long no = op.GetSerialno();
    while (true) {
        if (connection.pending()) {
            RootOperation * input=CharacterClient::connection.pop();
            if (input != NULL) {
                // What the hell is this!
                oplist result = message(*input);
                oplist::const_iterator I;
                for (I=result.begin();I!=result.end();I++) {
                    send(*(*I));
                }
    
                if (findRefno(*input,no)) {
                    return oplist(1,input);
                }
            }
            delete input;
        } else {
            connection.wait();
        }
    }
}
