// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2004 Alistair Riddoch

#include "BaseEntity.h"

#include "log.h"
#include "debug.h"
#include "op_switch.h"
#include "serialno.h"
#include "inheritance.h"

#include <Atlas/Objects/Operation/Error.h>

#include <iostream>

using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Objects::Operation::Error;
using Atlas::Objects::Operation::RootOperation;

static const bool debug_flag = false;

BaseEntity::BaseEntity(const std::string & id) : m_id(id)
{
    subscribe("look", OP_LOOK);
}

BaseEntity::~BaseEntity()
{
}

void BaseEntity::addToMessage(MapType & omap) const
{
    debug( std::cout << "BaseEntity::addToMessage" << std::endl << std::flush;);
    omap["objtype"] = "obj";
    omap["id"] = getId();
}

void BaseEntity::operation(const RootOperation & op, OpVector & res)
{
    debug( std::cout << "BaseEntity::operation" << std::endl << std::flush;);
    return callOperation(op, res);
}

/// \brief Process a Login operation.
///
/// @param op The operation to be processed.
/// @param res The result of the operation is returned here.
void BaseEntity::LoginOperation(const RootOperation & op, OpVector & res)
{
}

/// \brief Process a Logout operation.
///
/// @param op The operation to be processed.
/// @param res The result of the operation is returned here.
void BaseEntity::LogoutOperation(const RootOperation & op, OpVector & res)
{
}

/// \brief Process a Action operation.
///
/// @param op The operation to be processed.
/// @param res The result of the operation is returned here.
void BaseEntity::ActionOperation(const RootOperation & op, OpVector & res)
{
}

/// \brief Process a Chop operation.
///
/// @param op The operation to be processed.
/// @param res The result of the operation is returned here.
void BaseEntity::ChopOperation(const RootOperation & op, OpVector & res)
{
}

/// \brief Process a Combine operation.
///
/// @param op The operation to be processed.
/// @param res The result of the operation is returned here.
void BaseEntity::CombineOperation(const RootOperation & op, OpVector & res)
{
}

/// \brief Process a Create operation.
///
/// @param op The operation to be processed.
/// @param res The result of the operation is returned here.
void BaseEntity::CreateOperation(const RootOperation & op, OpVector & res)
{
}

/// \brief Process a Cut operation.
///
/// @param op The operation to be processed.
/// @param res The result of the operation is returned here.
void BaseEntity::CutOperation(const RootOperation & op, OpVector & res)
{
}

/// \brief Process a Delete operation.
///
/// @param op The operation to be processed.
/// @param res The result of the operation is returned here.
void BaseEntity::DeleteOperation(const RootOperation & op, OpVector & res)
{
}

/// \brief Process a Divide operation.
///
/// @param op The operation to be processed.
/// @param res The result of the operation is returned here.
void BaseEntity::DivideOperation(const RootOperation & op, OpVector & res)
{
}

/// \brief Process a Eat operation.
///
/// @param op The operation to be processed.
/// @param res The result of the operation is returned here.
void BaseEntity::EatOperation(const RootOperation & op, OpVector & res)
{
}

/// \brief Process a Burn operation.
///
/// @param op The operation to be processed.
/// @param res The result of the operation is returned here.
void BaseEntity::BurnOperation(const RootOperation & op, OpVector & res)
{
}

/// \brief Process a Get operation.
///
/// @param op The operation to be processed.
/// @param res The result of the operation is returned here.
void BaseEntity::GetOperation(const RootOperation & op, OpVector & res)
{
}

/// \brief Process a Imaginary operation.
///
/// @param op The operation to be processed.
/// @param res The result of the operation is returned here.
void BaseEntity::ImaginaryOperation(const RootOperation & op, OpVector & res)
{
}

/// \brief Process a Info operation.
///
/// @param op The operation to be processed.
/// @param res The result of the operation is returned here.
void BaseEntity::InfoOperation(const RootOperation & op, OpVector & res)
{
}

/// \brief Process a Move operation.
///
/// @param op The operation to be processed.
/// @param res The result of the operation is returned here.
void BaseEntity::MoveOperation(const RootOperation & op, OpVector & res)
{
}

/// \brief Process a Nourish operation.
///
/// @param op The operation to be processed.
/// @param res The result of the operation is returned here.
void BaseEntity::NourishOperation(const RootOperation & op, OpVector & res)
{
}

/// \brief Process a Set operation.
///
/// @param op The operation to be processed.
/// @param res The result of the operation is returned here.
void BaseEntity::SetOperation(const RootOperation & op, OpVector & res)
{
}

/// \brief Process a Sight operation.
///
/// @param op The operation to be processed.
/// @param res The result of the operation is returned here.
void BaseEntity::SightOperation(const RootOperation & op, OpVector & res)
{
}

/// \brief Process a Sound operation.
///
/// @param op The operation to be processed.
/// @param res The result of the operation is returned here.
void BaseEntity::SoundOperation(const RootOperation & op, OpVector & res)
{
}

/// \brief Process a Talk operation.
///
/// @param op The operation to be processed.
/// @param res The result of the operation is returned here.
void BaseEntity::TalkOperation(const RootOperation & op, OpVector & res)
{
}

/// \brief Process a Touch operation.
///
/// @param op The operation to be processed.
/// @param res The result of the operation is returned here.
void BaseEntity::TouchOperation(const RootOperation & op, OpVector & res)
{
}

/// \brief Process a Tick operation.
///
/// @param op The operation to be processed.
/// @param res The result of the operation is returned here.
void BaseEntity::TickOperation(const RootOperation & op, OpVector & res)
{
}

/// \brief Process a Look operation.
///
/// @param op The operation to be processed.
/// @param res The result of the operation is returned here.
void BaseEntity::LookOperation(const RootOperation & op, OpVector & res)
{
}

/// \brief Process a Setup operation.
///
/// @param op The operation to be processed.
/// @param res The result of the operation is returned here.
void BaseEntity::SetupOperation(const RootOperation & op, OpVector & res)
{
}

/// \brief Process a Appearance operation.
///
/// @param op The operation to be processed.
/// @param res The result of the operation is returned here.
void BaseEntity::AppearanceOperation(const RootOperation & op, OpVector & res)
{
}

/// \brief Process a Disappearance operation.
///
/// @param op The operation to be processed.
/// @param res The result of the operation is returned here.
void BaseEntity::DisappearanceOperation(const RootOperation & op, OpVector & res)
{
}

/// \brief Process a Use operation.
///
/// @param op The operation to be processed.
/// @param res The result of the operation is returned here.
void BaseEntity::UseOperation(const RootOperation & op, OpVector & res)
{
}

/// \brief Process a Wield operation.
///
/// @param op The operation to be processed.
/// @param res The result of the operation is returned here.
void BaseEntity::WieldOperation(const RootOperation & op, OpVector & res)
{
}

/// \brief Process a Other operation.
///
/// @param op The operation to be processed.
/// @param res The result of the operation is returned here.
void BaseEntity::OtherOperation(const RootOperation & op, OpVector & res)
{
}

/// \brief Process a Error operation.
///
/// @param op The operation to be processed.
/// @param res The result of the operation is returned here.
void BaseEntity::ErrorOperation(const RootOperation & op, OpVector & res)
{
}

OpNo BaseEntity::opEnumerate(const RootOperation & op) const
{
    const ListType & parents = op.getParents();
    if (parents.size() != 1) {
        log(ERROR, "op with no parents");
    }
    if (!parents.begin()->isString()) {
        log(ERROR, "op with non-string parent");
    }
    const std::string & parent = parents.begin()->asString();
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
    const ListType & parents = op.getParents();
    if (parents.size() != 1) {
        log(ERROR, "op with no parents");
    }
    if (!parents.begin()->isString()) {
        log(ERROR, "op with non-string parent");
    }
    const std::string & parent = parents.begin()->asString();
    OpNoDict::const_iterator I = d.find(parent);
    if (I != d.end()) {
        return I->second;
    } else {
        debug(std::cout << getId() << " is rejecting 2ry op of type " << parent
                        << std::endl << std::flush;);
        return OP_INVALID;
    }
}

void BaseEntity::subscribe(const std::string & op)
{
    OpNo opNo = Inheritance::instance().opEnumerate(op);
    if (opNo != OP_INVALID) {
        subscribe(op, opNo);
    }
}

void BaseEntity::callOperation(const RootOperation & op, OpVector & res)
{
    const OpNo op_no = opEnumerate(op);
    OP_SWITCH(op, op_no, res,)
}

void BaseEntity::error(const RootOperation& op, const char* errstring,
                       OpVector & res, const std::string & to) const
{
    Error * e = new Error;

    std::string msg = "ERROR generated by " + getId() + " with message [" + errstring + "]";
    log(NOTICE, msg.c_str());
    ListType & args = e->getArgs();
    args.push_back(MapType());
    MapType & errmsg = args.back().asMap();
    errmsg["message"] = errstring;
    args.push_back(op.asObject());

    e->setRefno(op.getSerialno());
    e->setSerialno(opSerialNo());
    e->setTo(to);

    res.push_back(e);
}

