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

using Atlas::Message::Object;

using Atlas::Objects::Root;

using Atlas::Objects::Operation::Perception;
using Atlas::Objects::Operation::Communicate;
using Atlas::Objects::Operation::Perceive;
using Atlas::Objects::Operation::Sound;
using Atlas::Objects::Operation::Smell;
using Atlas::Objects::Operation::Feel;
using Atlas::Objects::Operation::Listen;
using Atlas::Objects::Operation::Sniff;

using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::AdminEntity;
using Atlas::Objects::Entity::Player;
using Atlas::Objects::Entity::Admin;
using Atlas::Objects::Entity::Game;
using Atlas::Objects::Entity::GameEntity;

Inheritance * Inheritance::m_instance = NULL;

Inheritance::Inheritance()
{
    atlasObjects["root"] = new Root();
}

void installStandardObjects()
{
    Inheritance & i = Inheritance::instance();

    i.addChild(new RootOperation());
    i.addChild(new Action());
    i.opInstall("action", OP_ACTION);
    i.addChild(new Create());
    i.opInstall("create", OP_CREATE);
    i.addChild(new Delete());
    i.opInstall("delete", OP_DELETE);
    i.addChild(new Info());
    i.opInstall("info", OP_INFO);
    i.addChild(new Set());
    i.opInstall("set", OP_SET);
    i.addChild(new Get());
    i.opInstall("get", OP_GET);
    i.addChild(new Perception());
    i.addChild(new Error());
    i.opInstall("error", OP_ERROR);
    i.addChild(new Combine());
    i.opInstall("combine", OP_COMBINE);
    i.addChild(new Divide());
    i.opInstall("divide", OP_DIVIDE);
    i.addChild(new Communicate());
    i.addChild(new Move());
    i.opInstall("move", OP_MOVE);
    i.addChild(new Perceive());
    i.addChild(new Login());
    i.opInstall("login", OP_LOGIN);
    i.addChild(new Logout());
    i.opInstall("logout", OP_LOGOUT);
    i.addChild(new Sight());
    i.opInstall("sight", OP_SIGHT);
    i.addChild(new Sound());
    i.opInstall("sound", OP_SOUND);
    i.addChild(new Smell());
    i.addChild(new Feel());
    i.addChild(new Imaginary());
    i.opInstall("imaginary", OP_IMAGINARY);
    i.addChild(new Talk());
    i.opInstall("talk", OP_TALK);
    i.addChild(new Look());
    i.opInstall("look", OP_LOOK);
    i.addChild(new Listen());
    i.addChild(new Sniff());
    i.addChild(new Touch());
    i.opInstall("touch", OP_TOUCH);
    i.addChild(new Appearance());
    i.opInstall("appearance", OP_APPEARANCE);
    i.addChild(new Disappearance());
    i.opInstall("disappearance", OP_DISAPPEARANCE);

    i.addChild(new RootEntity());
    i.addChild(new AdminEntity());
    i.addChild(new Atlas::Objects::Entity::Account());
    i.addChild(new Player());
    i.addChild(new Admin());
    i.addChild(new Game());
    i.addChild(new GameEntity());

    // And from here on we need to define the hierarchy as found in the C++
    // base classes. Script classes defined in rulsets need to be added
    // at runtime.
}
