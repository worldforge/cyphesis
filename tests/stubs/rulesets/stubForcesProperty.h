// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2016 Erik Ogenvik
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

#include "rulesets/ForcesProperty.h"
using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Objects::Entity::RootEntity;

const std::string ForcesProperty::property_name = "forces";

ForcesProperty::ForcesProperty() {
}

int ForcesProperty::get(Element & val) const {
    return 0;
}

void ForcesProperty::set(const Element & val) {
}

ForcesProperty * ForcesProperty::copy() const {
    return new ForcesProperty(*this);
}

std::map<std::string, WFMath::Vector<3>>& ForcesProperty::forces() {
    return mForces;
}

const std::map<std::string, WFMath::Vector<3>>& ForcesProperty::forces() const {
    return mForces;
}

