// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

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

#include "inheritance.h"

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
using Atlas::Objects::Entity::Account;
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

    i.addChild("root", new RootOperation());
    i.addChild("root_operation", new Action());
    i.addChild("action", new Create());
    i.addChild("action", new Delete());
    i.addChild("root_operation", new Info());
    i.addChild("action", new Set());
    i.addChild("action", new Get());
    i.addChild("info", new Perception());
    i.addChild("info", new Error());
    i.addChild("create", new Combine());
    i.addChild("create", new Divide());
    i.addChild("create", new Communicate());
    i.addChild("set", new Move());
    i.addChild("get", new Perceive());
    i.addChild("get", new Login());
    i.addChild("login", new Logout());
    i.addChild("perception", new Sight());
    i.addChild("perception", new Sound());
    i.addChild("perception", new Smell());
    i.addChild("perception", new Feel());
    i.addChild("action", new Imaginary());
    i.addChild("communicate", new Talk());
    i.addChild("perceive", new Look());
    i.addChild("perceive", new Listen());
    i.addChild("perceive", new Sniff());
    i.addChild("perceive", new Touch());
    i.addChild("sight", new Appearance());
    i.addChild("sight", new Disappearance());

    i.addChild("root", new RootEntity());
    i.addChild("root_entity", new AdminEntity());
    i.addChild("admin_entity", new Account());
    i.addChild("account", new Player());
    i.addChild("account", new Admin());
    i.addChild("admin_entity", new Game());
    i.addChild("root_entity", new GameEntity());

    // And from here on we need to define the hierarchy as found in the C++
    // base classes. Script classes defined in rulsets need to be added
    // at runtime.
}
