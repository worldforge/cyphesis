// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include <Atlas/Objects/Operation/Sight.h>
#include <Atlas/Objects/Operation/Look.h>
#include <Atlas/Objects/Operation/Error.h>

#include "BaseEntity.h"

#include <common/debug.h>
#include <common/op_switch.h>
#include <common/serialno.h>

#include <iostream>

static const bool debug_flag = false;

using Atlas::Message::Object;

BaseEntity::BaseEntity() : game(false)
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
    debug( std::cout << "BaseEntity::asObject" << std::endl << std::flush;);
    Object::MapType omap;
    omap["objtype"] = "object";
    addToObject(omap);
    return Object(omap);
}


void BaseEntity::addToObject(Object::MapType & omap) const
{
    debug( std::cout << "BaseEntity::addToObject" << std::endl << std::flush;);
    omap["id"] = getId();
}

OpVector BaseEntity::externalMessage(const RootOperation & op)
{
    return message(op);
}

OpVector BaseEntity::message(const RootOperation & op)
{
    debug( std::cout << "BaseEntity::message" << std::endl << std::flush;);
    return operation(op);
}

OpVector BaseEntity::LookOperation(const Look & op)
{
    debug( std::cout << "look op got all the way to here"
                     << std::endl << std::flush;);
    Sight * s = new Sight( Sight::Instantiate());
    Object::ListType args(1,asObject());
    s->SetArgs(args);
    s->SetTo(op.GetFrom());

    // Set refno?
    return OpVector(1,s);
}

OpVector BaseEntity::operation(const RootOperation & op)
{
    debug( std::cout << "BaseEntity::operation" << std::endl << std::flush;);
    return callOperation(op);
}

OpVector BaseEntity::externalOperation(const RootOperation & op)
{
    return operation(op);
}

OpVector BaseEntity::LoginOperation(const Login & op) { OpVector res; return res; }
OpVector BaseEntity::LogoutOperation(const Logout & op) { OpVector res; return res; }
OpVector BaseEntity::ActionOperation(const Action & op) { OpVector res; return res; }
OpVector BaseEntity::ChopOperation(const Chop & op) { OpVector res; return res; }
OpVector BaseEntity::CombineOperation(const Combine & op) { OpVector res; return res; }
OpVector BaseEntity::CreateOperation(const Create & op) { OpVector res; return res; }
OpVector BaseEntity::CutOperation(const Cut & op) { OpVector res; return res; }
OpVector BaseEntity::DeleteOperation(const Delete & op) { OpVector res; return res; }
OpVector BaseEntity::DivideOperation(const Divide & op) { OpVector res; return res; }
OpVector BaseEntity::EatOperation(const Eat & op) { OpVector res; return res; }
OpVector BaseEntity::FireOperation(const Fire & op) { OpVector res; return res; }
OpVector BaseEntity::GetOperation(const Get & op) { OpVector res; return res; }
OpVector BaseEntity::ImaginaryOperation(const Imaginary & op) { OpVector res; return res; }
OpVector BaseEntity::InfoOperation(const Info & op) { OpVector res; return res; }
OpVector BaseEntity::MoveOperation(const Move & op) { OpVector res; return res; }
OpVector BaseEntity::NourishOperation(const Nourish & op) { OpVector res; return res; }
OpVector BaseEntity::SetOperation(const Set & op) { OpVector res; return res; }
OpVector BaseEntity::SightOperation(const Sight & op) { OpVector res; return res; }
OpVector BaseEntity::SoundOperation(const Sound & op) { OpVector res; return res; }
OpVector BaseEntity::TalkOperation(const Talk & op) { OpVector res; return res; }
OpVector BaseEntity::TouchOperation(const Touch & op) { OpVector res; return res; }
OpVector BaseEntity::TickOperation(const Tick & op) { OpVector res; return res; }
OpVector BaseEntity::LoadOperation(const Load & op) { OpVector res; return res; }
OpVector BaseEntity::SaveOperation(const Save & op) { OpVector res; return res; }
OpVector BaseEntity::SetupOperation(const Setup & op) { OpVector res; return res; }
OpVector BaseEntity::AppearanceOperation(const Appearance & op) { OpVector res; return res; }
OpVector BaseEntity::DisappearanceOperation(const Disappearance & op) { OpVector res; return res; }
OpVector BaseEntity::OtherOperation(const RootOperation & op) { OpVector res; return res; }
OpVector BaseEntity::ErrorOperation(const RootOperation & op) { OpVector res; return res; }

void BaseEntity::setRefno(const OpVector& ret, const RootOperation & ref_op) const
{
    for(OpVector::const_iterator I = ret.begin(); I != ret.end(); I++) {
        setRefnoOp(*I, ref_op);
    }
}

OpNo BaseEntity::opEnumerate(const RootOperation & op) const
{
    const Atlas::Message::Object::ListType & parents = op.GetParents();
    if (parents.size() != 1) {
        std::cerr << "This is a weird operation." << std::endl << std::flush;
    }
    if (!parents.begin()->IsString()) {
        std::cerr << "This op has invalid parent.\n" << std::endl << std::flush;
    }
    const std::string & parent = parents.begin()->AsString();
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

OpVector BaseEntity::callOperation(const RootOperation & op)
{
    const OpNo op_no = opEnumerate(op);
    OP_SWITCH(op, op_no,)
}

OpVector BaseEntity::error(const RootOperation& op, const char* errstring) const
{
    Error * e = new Error(Error::Instantiate());

    std::cerr << "ERROR generated by " << getId() << " with message:" << std::endl;
    std::cerr << " [" << errstring << "]" << std::endl << std::flush;
    Atlas::Message::Object::ListType args;
    Atlas::Message::Object::MapType errmsg;
    errmsg["message"] = Object(errstring);
    args.push_back(Object(errmsg));
    args.push_back(op.AsObject());

    e->SetArgs(args);
    e->SetRefno(op.GetSerialno());
    e->SetSerialno(opSerialNo());

    return OpVector(1,e);
}

