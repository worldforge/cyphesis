// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "inheritance.h"

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

Inheritance * Inheritance::m_instance = NULL;

Inheritance::Inheritance()
{
    atlasObjects["root"] = new Atlas::Objects::Root();
}

void Inheritance::flush()
{
    std::map<std::string, Atlas::Objects::Root *>::const_iterator I;
    for(I = atlasObjects.begin(); I != atlasObjects.end(); ++I) {
        delete I->second;
    }
    atlasObjects.clear();
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
    const Atlas::Message::Element::ListType & parents = op.getParents();
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
    std::map<std::string, Atlas::Objects::Root *>::const_iterator I = atlasObjects.find(parent);
    if (I == atlasObjects.end()) {
        return NULL;
    }
    return I->second;
}

bool Inheritance::addChild(Atlas::Objects::Root * obj)
{
    const std::string & child = obj->getId();
    const std::string & parent = obj->getParents().front().asString();
    if (atlasObjects.find(child) != atlasObjects.end()) {
        std::string msg = std::string("Installing type ") + child 
                        + "(" + parent + ") which was already installed";
        log(WARNING, msg.c_str());
        delete obj;
        return true;
    }
    std::map<std::string, Atlas::Objects::Root *>::const_iterator I = atlasObjects.find(parent);
    if (I == atlasObjects.end()) {
        throw InheritanceException(parent);
    }
    Atlas::Message::Element::ListType children(1, child);
    if (I->second->hasAttr("children")) {
        children = I->second->getAttr("children").asList();
        children.push_back(child);
    }
    I->second->setAttr("children", Atlas::Message::Element(children));
    atlasObjects[child] = obj;
    return false;
}

void installStandardObjects()
{
    Inheritance & i = Inheritance::instance();

    i.addChild(new Atlas::Objects::Operation::RootOperation(Atlas::Objects::Operation::RootOperation::Class()));
    i.addChild(new Atlas::Objects::Operation::Action(Atlas::Objects::Operation::Action::Class()));
    i.opInstall("action", OP_ACTION);
    i.addChild(new Atlas::Objects::Operation::Create(Atlas::Objects::Operation::Create::Class()));
    i.opInstall("create", OP_CREATE);
    i.addChild(new Atlas::Objects::Operation::Delete(Atlas::Objects::Operation::Delete::Class()));
    i.opInstall("delete", OP_DELETE);
    i.addChild(new Atlas::Objects::Operation::Info(Atlas::Objects::Operation::Info::Class()));
    i.opInstall("info", OP_INFO);
    i.addChild(new Atlas::Objects::Operation::Set(Atlas::Objects::Operation::Set::Class()));
    i.opInstall("set", OP_SET);
    i.addChild(new Atlas::Objects::Operation::Get(Atlas::Objects::Operation::Get::Class()));
    i.opInstall("get", OP_GET);
    i.addChild(new Atlas::Objects::Operation::Perception(Atlas::Objects::Operation::Perception::Class()));
    i.addChild(new Atlas::Objects::Operation::Error(Atlas::Objects::Operation::Error::Class()));
    i.opInstall("error", OP_ERROR);
    i.addChild(new Atlas::Objects::Operation::Combine(Atlas::Objects::Operation::Combine::Class()));
    i.opInstall("combine", OP_COMBINE);
    i.addChild(new Atlas::Objects::Operation::Divide(Atlas::Objects::Operation::Divide::Class()));
    i.opInstall("divide", OP_DIVIDE);
    i.addChild(new Atlas::Objects::Operation::Communicate(Atlas::Objects::Operation::Communicate::Class()));
    i.addChild(new Atlas::Objects::Operation::Move(Atlas::Objects::Operation::Move::Class()));
    i.opInstall("move", OP_MOVE);
    i.addChild(new Atlas::Objects::Operation::Perceive(Atlas::Objects::Operation::Perceive::Class()));
    i.addChild(new Atlas::Objects::Operation::Login(Atlas::Objects::Operation::Login::Class()));
    i.opInstall("login", OP_LOGIN);
    i.addChild(new Atlas::Objects::Operation::Logout(Atlas::Objects::Operation::Logout::Class()));
    i.opInstall("logout", OP_LOGOUT);
    i.addChild(new Atlas::Objects::Operation::Sight(Atlas::Objects::Operation::Sight::Class()));
    i.opInstall("sight", OP_SIGHT);
    i.addChild(new Atlas::Objects::Operation::Sound(Atlas::Objects::Operation::Sound::Class()));
    i.opInstall("sound", OP_SOUND);
    i.addChild(new Atlas::Objects::Operation::Smell(Atlas::Objects::Operation::Smell::Class()));
    i.addChild(new Atlas::Objects::Operation::Feel(Atlas::Objects::Operation::Feel::Class()));
    i.addChild(new Atlas::Objects::Operation::Imaginary(Atlas::Objects::Operation::Imaginary::Class()));
    i.opInstall("imaginary", OP_IMAGINARY);
    i.addChild(new Atlas::Objects::Operation::Talk(Atlas::Objects::Operation::Talk::Class()));
    i.opInstall("talk", OP_TALK);
    i.addChild(new Atlas::Objects::Operation::Look(Atlas::Objects::Operation::Look::Class()));
    i.opInstall("look", OP_LOOK);
    i.addChild(new Atlas::Objects::Operation::Listen(Atlas::Objects::Operation::Listen::Class()));
    i.addChild(new Atlas::Objects::Operation::Sniff(Atlas::Objects::Operation::Sniff::Class()));
    i.addChild(new Atlas::Objects::Operation::Touch(Atlas::Objects::Operation::Touch::Class()));
    i.opInstall("touch", OP_TOUCH);
    i.addChild(new Atlas::Objects::Operation::Appearance(Atlas::Objects::Operation::Appearance::Class()));
    i.opInstall("appearance", OP_APPEARANCE);
    i.addChild(new Atlas::Objects::Operation::Disappearance(Atlas::Objects::Operation::Disappearance::Class()));
    i.opInstall("disappearance", OP_DISAPPEARANCE);

    i.addChild(new Atlas::Objects::Entity::RootEntity(Atlas::Objects::Entity::RootEntity::Class()));
    i.addChild(new Atlas::Objects::Entity::AdminEntity(Atlas::Objects::Entity::AdminEntity::Class()));
    i.addChild(new Atlas::Objects::Entity::Account(Atlas::Objects::Entity::Account::Class()));
    i.addChild(new Atlas::Objects::Entity::Player(Atlas::Objects::Entity::Player::Class()));
    i.addChild(new Atlas::Objects::Entity::Admin(Atlas::Objects::Entity::Admin::Class()));
    i.addChild(new Atlas::Objects::Entity::Game(Atlas::Objects::Entity::Game::Class()));
    i.addChild(new Atlas::Objects::Entity::GameEntity(Atlas::Objects::Entity::GameEntity::Class()));

    // And from here on we need to define the hierarchy as found in the C++
    // base classes. Script classes defined in rulsets need to be added
    // at runtime.
}
