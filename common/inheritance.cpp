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

    i.addChild(new RootOperation());
    i.addChild(new Action());
    i.addChild(new Create());
    i.addChild(new Delete());
    i.addChild(new Info());
    i.addChild(new Set());
    i.addChild(new Get());
    i.addChild(new Perception());
    i.addChild(new Error());
    i.addChild(new Combine());
    i.addChild(new Divide());
    i.addChild(new Communicate());
    i.addChild(new Move());
    i.addChild(new Perceive());
    i.addChild(new Login());
    i.addChild(new Logout());
    i.addChild(new Sight());
    i.addChild(new Sound());
    i.addChild(new Smell());
    i.addChild(new Feel());
    i.addChild(new Imaginary());
    i.addChild(new Talk());
    i.addChild(new Look());
    i.addChild(new Listen());
    i.addChild(new Sniff());
    i.addChild(new Touch());
    i.addChild(new Appearance());
    i.addChild(new Disappearance());

    i.addChild(new RootEntity());
    i.addChild(new AdminEntity());
    i.addChild(new Account());
    i.addChild(new Player());
    i.addChild(new Admin());
    i.addChild(new Game());
    i.addChild(new GameEntity());

    // And from here on we need to define the hierarchy as found in the C++
    // base classes. Script classes defined in rulsets need to be added
    // at runtime.
}
