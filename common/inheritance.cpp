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

void installStandardObjects()
{
    Inheritance & i = Inheritance::instance();

    i.addChild(new Atlas::Objects::Operation::RootOperation());
    i.addChild(new Atlas::Objects::Operation::Action());
    i.opInstall("action", OP_ACTION);
    i.addChild(new Atlas::Objects::Operation::Create());
    i.opInstall("create", OP_CREATE);
    i.addChild(new Atlas::Objects::Operation::Delete());
    i.opInstall("delete", OP_DELETE);
    i.addChild(new Atlas::Objects::Operation::Info());
    i.opInstall("info", OP_INFO);
    i.addChild(new Atlas::Objects::Operation::Set());
    i.opInstall("set", OP_SET);
    i.addChild(new Atlas::Objects::Operation::Get());
    i.opInstall("get", OP_GET);
    i.addChild(new Atlas::Objects::Operation::Perception());
    i.addChild(new Atlas::Objects::Operation::Error());
    i.opInstall("error", OP_ERROR);
    i.addChild(new Atlas::Objects::Operation::Combine());
    i.opInstall("combine", OP_COMBINE);
    i.addChild(new Atlas::Objects::Operation::Divide());
    i.opInstall("divide", OP_DIVIDE);
    i.addChild(new Atlas::Objects::Operation::Communicate());
    i.addChild(new Atlas::Objects::Operation::Move());
    i.opInstall("move", OP_MOVE);
    i.addChild(new Atlas::Objects::Operation::Perceive());
    i.addChild(new Atlas::Objects::Operation::Login());
    i.opInstall("login", OP_LOGIN);
    i.addChild(new Atlas::Objects::Operation::Logout());
    i.opInstall("logout", OP_LOGOUT);
    i.addChild(new Atlas::Objects::Operation::Sight());
    i.opInstall("sight", OP_SIGHT);
    i.addChild(new Atlas::Objects::Operation::Sound());
    i.opInstall("sound", OP_SOUND);
    i.addChild(new Atlas::Objects::Operation::Smell());
    i.addChild(new Atlas::Objects::Operation::Feel());
    i.addChild(new Atlas::Objects::Operation::Imaginary());
    i.opInstall("imaginary", OP_IMAGINARY);
    i.addChild(new Atlas::Objects::Operation::Talk());
    i.opInstall("talk", OP_TALK);
    i.addChild(new Atlas::Objects::Operation::Look());
    i.opInstall("look", OP_LOOK);
    i.addChild(new Atlas::Objects::Operation::Listen());
    i.addChild(new Atlas::Objects::Operation::Sniff());
    i.addChild(new Atlas::Objects::Operation::Touch());
    i.opInstall("touch", OP_TOUCH);
    i.addChild(new Atlas::Objects::Operation::Appearance());
    i.opInstall("appearance", OP_APPEARANCE);
    i.addChild(new Atlas::Objects::Operation::Disappearance());
    i.opInstall("disappearance", OP_DISAPPEARANCE);

    i.addChild(new Atlas::Objects::Entity::RootEntity());
    i.addChild(new Atlas::Objects::Entity::AdminEntity());
    i.addChild(new Atlas::Objects::Entity::Account());
    i.addChild(new Atlas::Objects::Entity::Player());
    i.addChild(new Atlas::Objects::Entity::Admin());
    i.addChild(new Atlas::Objects::Entity::Game());
    i.addChild(new Atlas::Objects::Entity::GameEntity());

    // And from here on we need to define the hierarchy as found in the C++
    // base classes. Script classes defined in rulsets need to be added
    // at runtime.
}
