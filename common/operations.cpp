// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "Chop.h"
#include "Cut.h"
#include "Eat.h"
#include "Burn.h"
#include "Generic.h"
#include "Nourish.h"
#include "Setup.h"
#include "Tick.h"

namespace Atlas { namespace Objects { namespace Operation {

Chop::Chop() : Action("chop", "action") {
}

Chop::~Chop() { }

Chop Chop::Instantiate() {
    Chop value;
    value.setParents(Message::Element::ListType(1,std::string("chop")));
    value.setObjtype(std::string("op"));
    return value;
}

Cut::Cut() : Action("cut", "action") {
}

Cut::~Cut() { }

Cut Cut::Instantiate() {
    Cut value;
    value.setParents(Message::Element::ListType(1,std::string("cut")));
    value.setObjtype(std::string("op"));
    return value;
}

Eat::Eat() : Action("eat", "action") {
}

Eat::~Eat() { }

Eat Eat::Instantiate() {
    Eat value;
    value.setParents(Message::Element::ListType(1,std::string("eat")));
    value.setObjtype(std::string("op"));
    return value;
}

Burn::Burn() : Action("burn", "action") {
}

Burn::~Burn() { }

Burn Burn::Instantiate() {
    Burn value;
    value.setParents(Message::Element::ListType(1, "burn"));
    value.setObjtype(std::string("op"));
    return value;
}

Generic::Generic(const std::string & p) : RootOperation(p.c_str(), "root_operation") {
}

Generic::~Generic() { }

Generic Generic::Instantiate(const std::string & p) {
    Generic value(p);
    value.setParents(Message::Element::ListType(1,p));
    value.setObjtype(std::string("op"));
    return value;
}

Nourish::Nourish() : Action("nourish", "action") {
}

Nourish::~Nourish() { }

Nourish Nourish::Instantiate() {
    Nourish value;
    value.setParents(Message::Element::ListType(1, std::string("nourish")));
    value.setObjtype(std::string("op"));
    return value;
}

Setup::Setup() : RootOperation("setup", "root_operation") {
}

Setup::~Setup() { }

Setup Setup::Instantiate() {
    Setup value;
    value.setParents(Message::Element::ListType(1, "setup"));
    value.setObjtype(std::string("op"));
    return value;
}

Tick::Tick() : RootOperation("tick", "root_operation") {
}

Tick::~Tick() { }

Tick Tick::Instantiate() {
    Tick value;
    value.setParents(Message::Element::ListType(1,"tick"));
    value.setObjtype(std::string("op"));
    return value;
}

} } }
