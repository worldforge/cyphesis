// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include <Atlas/Objects/Operation/Sight.h>
#include <Atlas/Objects/Operation/Look.h>
#include <Atlas/Objects/Operation/Error.h>

#include "BaseEntity.h"

#include <common/debug.h>
#include <common/op_switch.h>

static const bool debug_flag = false;

using Atlas::Message::Object;

BaseEntity::BaseEntity() : inGame(false)
{
}

BaseEntity::~BaseEntity()
{
}

void BaseEntity::destroy()
{
}

Object BaseEntity::asObject() const
{
    debug( cout << "BaseEntity::asObject" << endl << flush;);
    Object::MapType omap;
    omap["objtype"] = "object";
    addToObject(omap);
    return Object(omap);
}


void BaseEntity::addToObject(Object::MapType & omap) const
{
    debug( cout << "BaseEntity::addToObject" << endl << flush;);
    omap["id"] = fullid;
}

oplist BaseEntity::externalMessage(const RootOperation & op)
{
    return message(op);
}

oplist BaseEntity::message(const RootOperation & op)
{
    debug( cout << "BaseEntity::message" << endl << flush;);
    return operation(op);
}

oplist BaseEntity::LookOperation(const Look & op)
{
    debug( cout << "look op got all the way to here" << endl << flush;);
    Sight * s = new Sight( Sight::Instantiate());
    Object::ListType args(1,asObject());
    s->SetArgs(args);
    s->SetTo(op.GetFrom());

    // Set refno?
    return oplist(1,s);
}

oplist BaseEntity::operation(const RootOperation & op)
{
    debug( cout << "BaseEntity::operation" << endl << flush;);
    return callOperation(op);
}

oplist BaseEntity::externalOperation(const RootOperation & op)
{
    return operation(op);
}

oplist BaseEntity::LoginOperation(const Login & op) { oplist res; return res; }
oplist BaseEntity::LogoutOperation(const Logout & op) { oplist res; return res; }
oplist BaseEntity::ActionOperation(const Action & op) { oplist res; return res; }
oplist BaseEntity::ChopOperation(const Chop & op) { oplist res; return res; }
oplist BaseEntity::CombineOperation(const Combine & op) { oplist res; return res; }
oplist BaseEntity::CreateOperation(const Create & op) { oplist res; return res; }
oplist BaseEntity::CutOperation(const Cut & op) { oplist res; return res; }
oplist BaseEntity::DeleteOperation(const Delete & op) { oplist res; return res; }
oplist BaseEntity::DivideOperation(const Divide & op) { oplist res; return res; }
oplist BaseEntity::EatOperation(const Eat & op) { oplist res; return res; }
oplist BaseEntity::FireOperation(const Fire & op) { oplist res; return res; }
oplist BaseEntity::GetOperation(const Get & op) { oplist res; return res; }
oplist BaseEntity::ImaginaryOperation(const Imaginary & op) { oplist res; return res; }
oplist BaseEntity::InfoOperation(const Info & op) { oplist res; return res; }
oplist BaseEntity::MoveOperation(const Move & op) { oplist res; return res; }
oplist BaseEntity::NourishOperation(const Nourish & op) { oplist res; return res; }
oplist BaseEntity::SetOperation(const Set & op) { oplist res; return res; }
oplist BaseEntity::SightOperation(const Sight & op) { oplist res; return res; }
oplist BaseEntity::SoundOperation(const Sound & op) { oplist res; return res; }
oplist BaseEntity::TalkOperation(const Talk & op) { oplist res; return res; }
oplist BaseEntity::TouchOperation(const Touch & op) { oplist res; return res; }
oplist BaseEntity::TickOperation(const Tick & op) { oplist res; return res; }
oplist BaseEntity::LoadOperation(const Load & op) { oplist res; return res; }
oplist BaseEntity::SaveOperation(const Save & op) { oplist res; return res; }
oplist BaseEntity::SetupOperation(const Setup & op) { oplist res; return res; }
oplist BaseEntity::AppearanceOperation(const Appearance & op) { oplist res; return res; }
oplist BaseEntity::DisappearanceOperation(const Disappearance & op) { oplist res; return res; }
oplist BaseEntity::OtherOperation(const RootOperation & op) { oplist res; return res; }
oplist BaseEntity::ErrorOperation(const RootOperation & op) { oplist res; return res; }

void BaseEntity::setRefno(const oplist& ret, const RootOperation & ref_op) const
{
    for(oplist::const_iterator I = ret.begin(); I != ret.end(); I++) {
        setRefnoOp(*I, ref_op);
    }
}

op_no_t BaseEntity::opEnumerate(const RootOperation & op) const
{
    const Atlas::Message::Object::ListType & parents = op.GetParents();
    if (parents.size() != 1) {
        cerr << "This is a weird operation." << endl << flush;
    }
    if (!parents.begin()->IsString()) {
        cerr << "This op has invalid parent.\n" << endl << flush;
    }
    const string & parent = parents.begin()->AsString();
    if ("tick" == parent)  { return OP_TICK; }
    if ("move" == parent)  { return OP_MOVE; }
    if ("sight" == parent)  { return OP_SIGHT; }
    if ("appearance" == parent)  { return OP_APPEARANCE; }
    if ("disappearance" == parent)  { return OP_DISAPPEARANCE; }
    if ("look" == parent)  { return OP_LOOK; }
    if ("create" == parent)  { return OP_CREATE; }
    if ("action" == parent)  { return OP_ACTION; }
    if ("delete" == parent)  { return OP_DELETE; }
    if ("eat" == parent)  { return OP_EAT; }
    if ("fire" == parent)  { return OP_FIRE; }
    if ("get" == parent)  { return OP_GET; }
    if ("imaginary" == parent)  { return OP_IMAGINARY; }
    if ("nourish" == parent)  { return OP_NOURISH; }
    if ("set" == parent)  { return OP_SET; }
    if ("sound" == parent)  { return OP_SOUND; }
    if ("talk" == parent)  { return OP_TALK; }
    if ("touch" == parent)  { return OP_TOUCH; }
    if ("divide" == parent)  { return OP_DIVIDE; }
    if ("combine" == parent)  { return OP_COMBINE; }
    if ("cut" == parent)  { return OP_CUT; }
    if ("chop" == parent)  { return OP_CHOP; }
    if ("setup" == parent)  { return OP_SETUP; }
    if ("error" == parent)  { return OP_ERROR; }
    if ("info" == parent)  { return OP_INFO; }
    if ("login" == parent)  { return OP_LOGIN; }
    if ("load" == parent)  { return OP_LOAD; }
    if ("save" == parent)  { return OP_SAVE; }
    return (OP_INVALID);
}

oplist BaseEntity::callOperation(const RootOperation & op)
{
    const op_no_t op_no = opEnumerate(op);
    OP_SWITCH(op, op_no,)
}

oplist BaseEntity::error(const RootOperation& op, const char* errstring) const
{
    Error * e = new Error(Error::Instantiate());

    cerr << "ERROR generated by " << fullid << " with message:" << endl;
    cerr << " [" << errstring << "]" << endl << flush;
    Atlas::Message::Object::ListType args;
    Atlas::Message::Object::MapType errmsg;
    errmsg["message"] = Object(errstring);
    args.push_back(Object(errmsg));
    args.push_back(op.AsObject());

    e->SetArgs(args);
    e->SetRefno(op.GetSerialno());

    return oplist(1,e);
}

