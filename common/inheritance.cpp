// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2004 Alistair Riddoch

#include "inheritance_impl.h"

#include "log.h"

#include "Generic.h"

#include <Atlas/Objects/Operation/Appearance.h>
#include <Atlas/Objects/Operation/Combine.h>
#include <Atlas/Objects/Operation/Delete.h>
#include <Atlas/Objects/Operation/Disappearance.h>
#include <Atlas/Objects/Operation/Divide.h>
#include <Atlas/Objects/Operation/Error.h>
#include <Atlas/Objects/Operation/Feel.h>
#include <Atlas/Objects/Operation/Imaginary.h>
#include <Atlas/Objects/Operation/Listen.h>
#include <Atlas/Objects/Operation/Logout.h>
#include <Atlas/Objects/Operation/Look.h>
#include <Atlas/Objects/Operation/Move.h>
#include <Atlas/Objects/Operation/Sound.h>
#include <Atlas/Objects/Operation/Smell.h>
#include <Atlas/Objects/Operation/Sniff.h>
#include <Atlas/Objects/Operation/Talk.h>
#include <Atlas/Objects/Operation/Touch.h>

#include <Atlas/Objects/Entity/Game.h>
#include <Atlas/Objects/Entity/Player.h>
#include <Atlas/Objects/Entity/Admin.h>
#include <Atlas/Objects/Entity/GameEntity.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;

using Atlas::Objects::Operation::RootOperation;
using Atlas::Objects::Operation::Generic;

using Atlas::Objects::Operation::Login;
using Atlas::Objects::Operation::Logout;
using Atlas::Objects::Operation::Action;
using Atlas::Objects::Operation::Combine;
using Atlas::Objects::Operation::Create;
using Atlas::Objects::Operation::Delete;
using Atlas::Objects::Operation::Divide;
using Atlas::Objects::Operation::Imaginary;
using Atlas::Objects::Operation::Info;
using Atlas::Objects::Operation::Move;
using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Get;
using Atlas::Objects::Operation::Sight;
using Atlas::Objects::Operation::Sound;
using Atlas::Objects::Operation::Touch;
using Atlas::Objects::Operation::Talk;
using Atlas::Objects::Operation::Look;
using Atlas::Objects::Operation::Appearance;
using Atlas::Objects::Operation::Disappearance;
using Atlas::Objects::Operation::Error;

Inheritance * Inheritance::m_instance = NULL;

Inheritance::Inheritance()
{
    atlasObjects["root"] = new Atlas::Objects::Root(Atlas::Objects::Root::Class());
}

void Inheritance::flush()
{
    RootDict::const_iterator Iend = atlasObjects.end();
    for (RootDict::const_iterator I = atlasObjects.begin(); I != Iend; ++I) {
        delete I->second;
    }
    atlasObjects.clear();
    OpFactoryDict::const_iterator J = opFactories.begin();
    OpFactoryDict::const_iterator Jend = opFactories.end();
    for (; J != Jend; ++J) {
        delete J->second;
    }
    opFactories.clear();
}

Inheritance & Inheritance::instance()
{
    if (m_instance == NULL) {
        m_instance = new Inheritance();
        installStandardObjects();
        installCustomOperations();
        installCustomEntities();
    }
    return *m_instance;
}

void Inheritance::clear()
{
    if (m_instance != NULL) {
        m_instance->flush();
        delete m_instance;
    }
}

OpNo Inheritance::opEnumerate(const std::string & parent) const
{
    OpNoDict::const_iterator I = opLookup.find(parent);
    if (I != opLookup.end()) {
        return I->second;
    } else {
        return OP_INVALID;
    }
}

OpNo Inheritance::opEnumerate(const RootOperation & op) const
{
    const ListType & parents = op.getParents();
    if (parents.size() != 1) {
        log(ERROR, "op with no parents");
    }
    if (!parents.begin()->isString()) {
        log(ERROR, "op with non-string parent");
    }
    const std::string & parent = parents.begin()->asString();
    return opEnumerate(parent);
}

Atlas::Objects::Root * Inheritance::get(const std::string & parent)
{
    RootDict::const_iterator I = atlasObjects.find(parent);
    if (I == atlasObjects.end()) {
        return NULL;
    }
    return I->second;
}

int Inheritance::addChild(Atlas::Objects::Root * obj)
{
    const std::string & child = obj->getId();
    const std::string & parent = obj->getParents().front().asString();
    if (atlasObjects.find(child) != atlasObjects.end()) {
        std::string msg = std::string("Installing type ") + child 
                        + "(" + parent + ") which was already installed";
        log(WARNING, msg.c_str());
        delete obj;
        return -1;
    }
    RootDict::const_iterator I = atlasObjects.find(parent);
    if (I == atlasObjects.end()) {
        std::string msg = std::string("Installing type ") + child 
                        + " which has unknown parent \"" + parent + "\"";
        log(WARNING, msg.c_str());
        delete obj;
        return -1;
    }
    ListType children(1, child);
    if (I->second->hasAttr("children")) {
        children = I->second->getAttr("children").asList();
        children.push_back(child);
    }
    I->second->setAttr("children", Element(children));
    atlasObjects[child] = obj;
    return 0;
}

RootOperation * Inheritance::newOperation(const std::string & op_type)
{
    OpFactoryDict::const_iterator I = opFactories.find(op_type);
    if (I == opFactories.end()) {
        return NULL;
    }
    return I->second->newOperation();
}

int Inheritance::newOperation(const std::string & op_type, RootOperation & ret) const
{
    OpFactoryDict::const_iterator I = opFactories.find(op_type);
    if (I == opFactories.end()) {
        return -1;
    }
    I->second->newOperation(ret);
    return 0;
}

bool Inheritance::isTypeOf(const std::string & instance,
                           const std::string & type) const
{
    if (instance == type) {
        return true;
    }
    RootDict::const_iterator I = atlasObjects.find(instance);
    RootDict::const_iterator Iend = atlasObjects.end();
    for (; I != Iend;) {
        const ListType & parents = I->second->getParents();
        if (parents.empty()) {
            break;
        }
        const std::string & parent = I->second->getParents().front().asString();
        if (parent == type) {
            return true;
        }
        I = atlasObjects.find(parent);
    }
    return false;
    // Walk up the tree.
}

using Atlas::Objects::Operation::Perception;
using Atlas::Objects::Operation::Communicate;
using Atlas::Objects::Operation::Perceive;
using Atlas::Objects::Operation::Smell;
using Atlas::Objects::Operation::Feel;
using Atlas::Objects::Operation::Listen;
using Atlas::Objects::Operation::Sniff;

using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::AdminEntity;
using Atlas::Objects::Entity::Game;
using Atlas::Objects::Entity::GameEntity;

OpFactoryBase::~OpFactoryBase()
{
}

GenericOpFactory::GenericOpFactory(const std::string &opType) : m_opType(opType)
{
}

RootOperation * GenericOpFactory::newOperation()
{
    return new Generic(m_opType);
}

void GenericOpFactory::newOperation(RootOperation & ret)
{
    ret = Generic(m_opType);
}

void installStandardObjects()
{
    Inheritance & i = Inheritance::instance();

    i.addChild(new RootOperation(RootOperation::Class()));
    i.addChild(new Action(Action::Class()));
    i.opInstall("action", OP_ACTION, new OpFactory<Action>);
    i.addChild(new Create(Create::Class()));
    i.opInstall("create", OP_CREATE, new OpFactory<Create>);
    i.addChild(new Delete(Delete::Class()));
    i.opInstall("delete", OP_DELETE, new OpFactory<Delete>);
    i.addChild(new Info(Info::Class()));
    i.opInstall("info", OP_INFO, new OpFactory<Info>);
    i.addChild(new Set(Set::Class()));
    i.opInstall("set", OP_SET, new OpFactory<Set>);
    i.addChild(new Get(Get::Class()));
    i.opInstall("get", OP_GET, new OpFactory<Get>);
    i.addChild(new Perception(Perception::Class()));
    i.addChild(new Error(Error::Class()));
    i.opInstall("error", OP_ERROR, new OpFactory<Error>);
    i.addChild(new Combine(Combine::Class()));
    i.opInstall("combine", OP_COMBINE, new OpFactory<Combine>);
    i.addChild(new Divide(Divide::Class()));
    i.opInstall("divide", OP_DIVIDE, new OpFactory<Divide>);
    i.addChild(new Communicate(Communicate::Class()));
    i.addChild(new Move(Move::Class()));
    i.opInstall("move", OP_MOVE, new OpFactory<Move>);
    i.addChild(new Perceive(Perceive::Class()));
    i.addChild(new Login(Login::Class()));
    i.opInstall("login", OP_LOGIN, new OpFactory<Login>);
    i.addChild(new Logout(Logout::Class()));
    i.opInstall("logout", OP_LOGOUT, new OpFactory<Logout>);
    i.addChild(new Sight(Sight::Class()));
    i.opInstall("sight", OP_SIGHT, new OpFactory<Sight>);
    i.addChild(new Sound(Sound::Class()));
    i.opInstall("sound", OP_SOUND, new OpFactory<Sound>);
    i.addChild(new Smell(Smell::Class()));
    i.addChild(new Feel(Feel::Class()));
    i.addChild(new Imaginary(Imaginary::Class()));
    i.opInstall("imaginary", OP_IMAGINARY, new OpFactory<Imaginary>);
    i.addChild(new Talk(Talk::Class()));
    i.opInstall("talk", OP_TALK, new OpFactory<Talk>);
    i.addChild(new Look(Look::Class()));
    i.opInstall("look", OP_LOOK, new OpFactory<Look>);
    i.addChild(new Listen(Listen::Class()));
    i.addChild(new Sniff(Sniff::Class()));
    i.addChild(new Touch(Touch::Class()));
    i.opInstall("touch", OP_TOUCH, new OpFactory<Touch>);
    i.addChild(new Appearance(Appearance::Class()));
    i.opInstall("appearance", OP_APPEARANCE, new OpFactory<Appearance>);
    i.addChild(new Disappearance(Disappearance::Class()));
    i.opInstall("disappearance", OP_DISAPPEARANCE, new OpFactory<Disappearance>);

    i.addChild(new RootEntity(RootEntity::Class()));
    i.addChild(new AdminEntity(AdminEntity::Class()));
    i.addChild(new Atlas::Objects::Entity::Account(Atlas::Objects::Entity::Account::Class()));
    i.addChild(new Atlas::Objects::Entity::Player(Atlas::Objects::Entity::Player::Class()));
    i.addChild(new Atlas::Objects::Entity::Admin(Atlas::Objects::Entity::Admin::Class()));
    i.addChild(new Game(Game::Class()));
    i.addChild(new GameEntity(GameEntity::Class()));

    // And from here on we need to define the hierarchy as found in the C++
    // base classes. Script classes defined in rulsets need to be added
    // at runtime.
}
