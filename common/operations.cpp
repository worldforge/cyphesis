// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include <Atlas/Objects/Operation/RootOperation.h>

#include "Chop.h"
#include "Cut.h"
#include "Eat.h"
#include "Fire.h"
#include "Generic.h"
#include "Load.h"
#include "Nourish.h"
#include "Save.h"
#include "Setup.h"
#include "Tick.h"

namespace Atlas { namespace Objects { namespace Operation {

Chop::Chop() : RootOperation("chop", "root_operation") {
}

Chop::~Chop() { }

Chop Chop::Instantiate() {
    Chop value;
    value.SetParents(Message::Object::ListType(1,std::string("chop")));
    value.SetObjtype(std::string("op"));
    return value;
}

Cut::Cut() : RootOperation("cut", "root_operation") {
}

Cut::~Cut() { }

Cut Cut::Instantiate() {
    Cut value;
    value.SetParents(Message::Object::ListType(1,std::string("cut")));
    value.SetObjtype(std::string("op"));
    return value;
}

Eat::Eat() : RootOperation("eat", "root_operation") {
}

Eat::~Eat() { }

Eat Eat::Instantiate() {
    Eat value;
    value.SetParents(Message::Object::ListType(1,std::string("eat")));
    value.SetObjtype(std::string("op"));
    return value;
}

Fire::Fire() : RootOperation("fire", "root_operation") {
}

Fire::~Fire() { }

Fire Fire::Instantiate() {
    Fire value;
    value.SetParents(Message::Object::ListType(1, "fire"));
    value.SetObjtype(std::string("op"));
    return value;
}

Generic::Generic(const std::string & p) : RootOperation(p, "root_operation") {
}

Generic::~Generic() { }

Generic Generic::Instantiate(const std::string & p) {
    Generic value(p);
    value.SetParents(Message::Object::ListType(1,p));
    value.SetObjtype(std::string("op"));
    return value;
}

Load::Load() : RootOperation("load", "root_operation") {
}

Load::~Load() { }

Load Load::Instantiate() {
    Load value;
    value.SetParents(Message::Object::ListType(1, "load"));
    value.SetObjtype(std::string("op"));
    return value;
}

Nourish::Nourish() : RootOperation("nourish", "root_operation") {
}

Nourish::~Nourish() { }

Nourish Nourish::Instantiate() {
    Nourish value;
    value.SetParents(Message::Object::ListType(1, std::string("nourish")));
    value.SetObjtype(std::string("op"));
    return value;
}

Save::Save() : RootOperation("save", "root_operation") {
}

Save::~Save() { }

Save Save::Instantiate() {
    Save value;
    value.SetParents(Message::Object::ListType(1,"save"));
    value.SetObjtype(std::string("op"));
    return value;
}

Setup::Setup() : RootOperation("setup", "root_operation") {
}

Setup::~Setup() { }

Setup Setup::Instantiate() {
    Setup value;
    value.SetParents(Message::Object::ListType(1, "setup"));
    value.SetObjtype(std::string("op"));
    return value;
}

Tick::Tick() : RootOperation("tick", "root_operation") {
}

Tick::~Tick() { }

Tick Tick::Instantiate() {
    Tick value;
    value.SetParents(Message::Object::ListType(1,"tick"));
    value.SetObjtype(std::string("op"));
    return value;
}

} } }
