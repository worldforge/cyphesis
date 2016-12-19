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

#include "rulesets/Vector3Property.h"

const std::string Vector3Property::property_atlastype = "list";

Vector3Property::Vector3Property() {
}

Vector3Property::Vector3Property(const Vector3Property &rhs) {
}

Vector3Property::~Vector3Property() {
}

int Vector3Property::get(Atlas::Message::Element &val) const {
    return 1;

}

void Vector3Property::set(const Atlas::Message::Element &val) {
}

Vector3Property *Vector3Property::copy() const {
    return new Vector3Property(*this);
}

