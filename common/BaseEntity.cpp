// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "BaseEntity.h"

#include "log.h"
#include "debug.h"
#include "op_switch.h"
#include "serialno.h"

#include <Atlas/Objects/Operation/Sight.h>
#include <Atlas/Objects/Operation/Look.h>
#include <Atlas/Objects/Operation/Error.h>

#include <iostream>

static const bool debug_flag = false;

using Atlas::Message::Object;

BaseEntity::BaseEntity()
{
    subscribe("look", OP_LOOK);
}

BaseEntity::~BaseEntity()
{
}

Fragment BaseEntity::asObject() const
{
    debug( std::cout << "BaseEntity::asObject" << std::endl << std::flush;);
    Fragment::MapType omap;
    omap["objtype"] = "object";
    addToObject(omap);
    return Fragment(omap);
}


void BaseEntity::addToObject(Fragment::MapType & omap) const
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
    Fragment::ListType args(1,asObject());
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

OpVector BaseEntity::LoginOperation(const Login & op) { return OpVector(); }
OpVector BaseEntity::LogoutOperation(const Logout & op) { return OpVector(); }
OpVector BaseEntity::ActionOperation(const Action & op) { return OpVector(); }
OpVector BaseEntity::ChopOperation(const Chop & op) { return OpVector(); }
OpVector BaseEntity::CombineOperation(const Combine & op) { return OpVector(); }
OpVector BaseEntity::CreateOperation(const Create & op) { return OpVector(); }
OpVector BaseEntity::CutOperation(const Cut & op) { return OpVector(); }
OpVector BaseEntity::DeleteOperation(const Delete & op) { return OpVector(); }
OpVector BaseEntity::DivideOperation(const Divide & op) { return OpVector(); }
OpVector BaseEntity::EatOperation(const Eat & op) { return OpVector(); }
OpVector BaseEntity::BurnOperation(const Burn & op) { return OpVector(); }
OpVector BaseEntity::GetOperation(const Get & op) { return OpVector(); }
OpVector BaseEntity::ImaginaryOperation(const Imaginary & op) { return OpVector(); }
OpVector BaseEntity::InfoOperation(const Info & op) { return OpVector(); }
OpVector BaseEntity::MoveOperation(const Move & op) { return OpVector(); }
OpVector BaseEntity::NourishOperation(const Nourish & op) { return OpVector(); }
OpVector BaseEntity::SetOperation(const Set & op) { return OpVector(); }
OpVector BaseEntity::SightOperation(const Sight & op) { return OpVector(); }
OpVector BaseEntity::SoundOperation(const Sound & op) { return OpVector(); }
OpVector BaseEntity::TalkOperation(const Talk & op) { return OpVector(); }
OpVector BaseEntity::TouchOperation(const Touch & op) { return OpVector(); }
OpVector BaseEntity::TickOperation(const Tick & op) { return OpVector(); }
OpVector BaseEntity::LoadOperation(const Load & op) { return OpVector(); }
OpVector BaseEntity::SaveOperation(const Save & op) { return OpVector(); }
OpVector BaseEntity::SetupOperation(const Setup & op) { return OpVector(); }
OpVector BaseEntity::AppearanceOperation(const Appearance & op) { return OpVector(); }
OpVector BaseEntity::DisappearanceOperation(const Disappearance & op) { return OpVector(); }
OpVector BaseEntity::OtherOperation(const RootOperation & op) { return OpVector(); }
OpVector BaseEntity::ErrorOperation(const RootOperation & op) { return OpVector(); }

void BaseEntity::setRefno(const OpVector& ret, const RootOperation & ref_op) const
{
    for(OpVector::const_iterator I = ret.begin(); I != ret.end(); I++) {
        setRefnoOp(*I, ref_op);
    }
}

OpNo BaseEntity::opEnumerate(const RootOperation & op) const
{
    const Fragment::ListType & parents = op.GetParents();
    if (parents.size() != 1) {
        log(ERROR, "op with no parents");
    }
    if (!parents.begin()->IsString()) {
        log(ERROR, "op with non-string parent");
    }
    const std::string & parent = parents.begin()->AsString();
    OpNoDict::const_iterator I = opLookup.find(parent);
    if (I != opLookup.end()) {
        return I->second;
    } else {
        debug(std::cout << getId() << " is rejecting op of type " << parent
                        << std::endl << std::flush;);
        return OP_INVALID;
    }
}

OpNo BaseEntity::opEnumerate(const RootOperation& op, const OpNoDict& d) const
{
    const Fragment::ListType & parents = op.GetParents();
    if (parents.size() != 1) {
        log(ERROR, "op with no parents");
    }
    if (!parents.begin()->IsString()) {
        log(ERROR, "op with non-string parent");
    }
    const std::string & parent = parents.begin()->AsString();
    OpNoDict::const_iterator I = d.find(parent);
    if (I != d.end()) {
        return I->second;
    } else {
        debug(std::cout << getId() << " is rejecting 2ry op of type " << parent
                        << std::endl << std::flush;);
        return OP_INVALID;
    }
}

OpVector BaseEntity::callOperation(const RootOperation & op)
{
    const OpNo op_no = opEnumerate(op);
    OP_SWITCH(op, op_no,)
    return OpVector();
}

OpVector BaseEntity::error(const RootOperation& op, const char* errstring) const
{
    Error * e = new Error(Error::Instantiate());

    std::string msg = "ERROR generated by " + getId() + " with message [" + errstring + "]";
    log(NOTICE, msg.c_str());
    Fragment::ListType args;
    Fragment::MapType errmsg;
    errmsg["message"] = Fragment(errstring);
    args.push_back(Fragment(errmsg));
    args.push_back(op.AsObject());

    e->SetArgs(args);
    e->SetRefno(op.GetSerialno());
    e->SetSerialno(opSerialNo());

    return OpVector(1,e);
}

