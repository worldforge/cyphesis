// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2004 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

// $Id$

#include "inheritance.h"

#include "log.h"
#include "TypeNode.h"
#include "compose.hpp"
#include "OperationRouter.h"

#include <Atlas/Objects/Operation.h>

#include <Atlas/Objects/Entity.h>

using Atlas::Message::Element;
using Atlas::Message::ListType;

using Atlas::Objects::Root;

using Atlas::Objects::Operation::Login;
using Atlas::Objects::Operation::Logout;
using Atlas::Objects::Operation::Action;
using Atlas::Objects::Operation::Affect;
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
using Atlas::Objects::Operation::Use;
using Atlas::Objects::Operation::Wield;
using Atlas::Objects::Operation::Look;
using Atlas::Objects::Operation::Appearance;
using Atlas::Objects::Operation::Disappearance;
using Atlas::Objects::Operation::Error;
using Atlas::Objects::Operation::Use;
using Atlas::Objects::Operation::Wield;

Inheritance * Inheritance::m_instance = NULL;

Root atlasOpDefinition(const std::string & name, const std::string & parent)
{
    Atlas::Objects::Entity::Anonymous r;

    r->setParents(std::list<std::string>(1, parent));
    r->setObjtype("op_definition");
    r->setId(name);

    return r;
}

Root atlasClass(const std::string & name, const std::string & parent)
{
    Atlas::Objects::Entity::Anonymous r;

    r->setParents(std::list<std::string>(1, parent));
    r->setObjtype("class");
    r->setId(name);

    return r;
}

Inheritance::Inheritance() : noClass(0)
{
    Atlas::Objects::Entity::Anonymous root_desc;

    root_desc->setParents(std::list<std::string>(0));
    root_desc->setObjtype("meta");
    root_desc->setId("root");

    TypeNode * root = new TypeNode;
    root->name() = "root";
    root->description() = root_desc;

    atlasObjects["root"] = root;
}

void Inheritance::flush()
{
    TypeNodeDict::const_iterator I = atlasObjects.begin();
    TypeNodeDict::const_iterator Iend = atlasObjects.end();
    for (; I != Iend; ++I) {
        delete I->second;
    }
    atlasObjects.clear();
    opLookup.clear();
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
        m_instance = NULL;
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

OpNo Inheritance::opEnumerate(const Atlas::Objects::Operation::RootOperation & op) const
{
    return op->getClassNo();
}

const Root & Inheritance::getClass(const std::string & parent)
{
    TypeNodeDict::const_iterator I = atlasObjects.find(parent);
    if (I == atlasObjects.end()) {
        return noClass;
    }
    return I->second->description();
}

const TypeNode * Inheritance::getType(const std::string & parent)
{
    TypeNodeDict::const_iterator I = atlasObjects.find(parent);
    if (I == atlasObjects.end()) {
        return 0;
    }
    return I->second;
}

bool Inheritance::hasClass(const std::string & parent)
{
    TypeNodeDict::const_iterator I = atlasObjects.find(parent);
    if (I == atlasObjects.end()) {
        return false;
    }
    return true;
}

TypeNode * Inheritance::addChild(const Root & obj,
                                 const PropertyDict & defaults)
{
    const std::string & child = obj->getId();
    const std::string & parent = obj->getParents().front();
    if (atlasObjects.find(child) != atlasObjects.end()) {
        log(ERROR, String::compose("Installing type \"%1\"(\"%2\") "
                                   "which was already installed",
                                   child, parent));
        return 0;
    }
    TypeNodeDict::iterator I = atlasObjects.find(parent);
    if (I == atlasObjects.end()) {
        log(ERROR, String::compose("Installing type \"%1\" "
                                   "which has unknown parent \"%2\".",
                                   child, parent));;
        return 0;
    }
    Element children(ListType(1, child));
    if (I->second->description()->copyAttr("children", children) == 0) {
        assert(children.isList());
        children.asList().push_back(child);
    }
    I->second->description()->setAttr("children", children);

    TypeNode * type = new TypeNode;
    type->name() = child;
    type->description() = obj;
    type->defaults() = defaults;
    type->setParent(I->second);

    atlasObjects[child] = type;

    return type;
}

bool Inheritance::isTypeOf(const std::string & instance,
                           const std::string & base_type) const
{
    if (instance == base_type) {
        return true;
    }
    TypeNodeDict::const_iterator I = atlasObjects.find(instance);
    TypeNodeDict::const_iterator Iend = atlasObjects.end();
    if (I == Iend) {
        return false;
    }
    return this->isTypeOf(I->second, base_type);
}

bool Inheritance::isTypeOf(const TypeNode * instance,
                           const std::string & base_type) const
{
    const TypeNode * node = instance;
    for (; node->parent() != 0;) {
        const TypeNode * parent = node->parent();
        if (parent->name() == base_type) {
            return true;
        }
        node = node->parent();
    }
    return false;
}

using Atlas::Objects::Operation::RootOperation;
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

void installStandardObjects()
{
    Inheritance & i = Inheritance::instance();

    i.addChild(atlasOpDefinition("root_operation", "root"));
    i.addChild(atlasOpDefinition("action", "root_operation"));
    i.opInstall("action", Atlas::Objects::Operation::ACTION_NO);
    i.addChild(atlasOpDefinition("create", "action"));
    i.opInstall("create", Atlas::Objects::Operation::CREATE_NO);
    i.addChild(atlasOpDefinition("delete", "action"));
    i.opInstall("delete", Atlas::Objects::Operation::DELETE_NO);
    i.addChild(atlasOpDefinition("info", "root_operation"));
    i.opInstall("info", Atlas::Objects::Operation::INFO_NO);
    i.addChild(atlasOpDefinition("set", "action"));
    i.opInstall("set", Atlas::Objects::Operation::SET_NO);
    i.addChild(atlasOpDefinition("get", "action"));
    i.opInstall("get", Atlas::Objects::Operation::GET_NO);
    i.addChild(atlasOpDefinition("perception", "info"));
    i.addChild(atlasOpDefinition("error", "info"));
    i.opInstall("error", Atlas::Objects::Operation::ERROR_NO);
    i.addChild(atlasOpDefinition("combine", "create"));
    i.opInstall("combine", Atlas::Objects::Operation::COMBINE_NO);
    i.addChild(atlasOpDefinition("divide", "create"));
    i.opInstall("divide", Atlas::Objects::Operation::DIVIDE_NO);
    i.addChild(atlasOpDefinition("communicate", "create"));
    i.addChild(atlasOpDefinition("move", "set"));
    i.opInstall("move", Atlas::Objects::Operation::MOVE_NO);
    i.addChild(atlasOpDefinition("affect", "set"));
    i.opInstall("affect", Atlas::Objects::Operation::MOVE_NO);
    i.addChild(atlasOpDefinition("perceive", "get"));
    i.addChild(atlasOpDefinition("login", "get"));
    i.opInstall("login", Atlas::Objects::Operation::LOGIN_NO);
    i.addChild(atlasOpDefinition("logout", "login"));
    i.opInstall("logout", Atlas::Objects::Operation::LOGOUT_NO);
    i.addChild(atlasOpDefinition("sight", "perception"));
    i.opInstall("sight", Atlas::Objects::Operation::SIGHT_NO);
    i.addChild(atlasOpDefinition("sound", "perception"));
    i.opInstall("sound", Atlas::Objects::Operation::SOUND_NO);
    i.addChild(atlasOpDefinition("smell", "perception"));
    i.addChild(atlasOpDefinition("feel", "perception"));
    i.addChild(atlasOpDefinition("imaginary", "action"));
    i.opInstall("imaginary", Atlas::Objects::Operation::IMAGINARY_NO);
    i.addChild(atlasOpDefinition("talk", "communicate"));
    i.opInstall("talk", Atlas::Objects::Operation::TALK_NO);
    i.addChild(atlasOpDefinition("look", "perceive"));
    i.opInstall("look", Atlas::Objects::Operation::LOOK_NO);
    i.addChild(atlasOpDefinition("listen", "perceive"));
    i.addChild(atlasOpDefinition("sniff", "perceive"));
    i.addChild(atlasOpDefinition("touch", "perceive"));
    i.opInstall("touch", Atlas::Objects::Operation::TOUCH_NO);
    i.addChild(atlasOpDefinition("appearance", "sight"));
    i.opInstall("appearance", Atlas::Objects::Operation::APPEARANCE_NO);
    i.addChild(atlasOpDefinition("disappearance", "sight"));
    i.opInstall("disappearance", Atlas::Objects::Operation::DISAPPEARANCE_NO);
    i.addChild(atlasOpDefinition("use", "action"));
    i.opInstall("use", Atlas::Objects::Operation::USE_NO);
    i.addChild(atlasOpDefinition("wield", "set"));
    i.opInstall("wield", Atlas::Objects::Operation::WIELD_NO);



    i.addChild(atlasClass("root_entity", "root"));
    i.addChild(atlasClass("admin_entity", "root_entity"));
    i.addChild(atlasClass("account", "admin_entity"));
    i.addChild(atlasClass("player", "account"));
    i.addChild(atlasClass("admin", "account"));
    i.addChild(atlasClass("game", "admin_entity"));
    i.addChild(atlasClass("game_entity", "root_entity"));

    // And from here on we need to define the hierarchy as found in the C++
    // base classes. Script classes defined in rulsets need to be added
    // at runtime.
}
