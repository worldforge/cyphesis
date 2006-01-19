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

#error This file has been removed from the build

#ifndef COMMON_OPERATIONS_H
#define COMMON_OPERATIONS_H

#include <string>
#include <map>
#include <vector>

// Forward declarations of all the operation classes used in cyphesis. Use
// of this file reduces complexities in the dependency tree, and speeds up
// compile time.

namespace Atlas {
    namespace Message {
        class Element;
        typedef std::map<std::string, Element> MapType;
        typedef std::vector<Element> ListType;
    }
    namespace Objects {
        class Root;
        namespace Operation {
            class Login;
            class Logout;
            class Action;
            class Combine;
            class Create;
            class Delete;
            class Divide;
            class Imaginary;
            class Move;
            class Set;
            class Get;
            class Perception;
            class Sight;
            class Sound;
            class Touch;
            class Talk;
            class Look;
            class Info;
            class Appearance;
            class Disappearance;
            class Error;
            class Setup;
            class Tick;
            class Eat;
            class Nourish;
            class Cut;
            class Chop;
            class Burn;
            class Delve;
            class Use;
            class Wield;
            class Unseen;
            class Update;
            class Generic;
            class RootOperation;
        }
    }
}

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;

using Atlas::Objects::Operation::RootOperation;
using Atlas::Objects::Operation::Generic;

using Atlas::Objects::Operation::Login;
using Atlas::Objects::Operation::Logout;
using Atlas::Objects::Operation::Action;
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
using Atlas::Objects::Operation::Look;
using Atlas::Objects::Operation::Appearance;
using Atlas::Objects::Operation::Disappearance;
using Atlas::Objects::Operation::Error;
using Atlas::Objects::Operation::Setup;
using Atlas::Objects::Operation::Tick;
using Atlas::Objects::Operation::Eat;
using Atlas::Objects::Operation::Nourish;
using Atlas::Objects::Operation::Cut;
using Atlas::Objects::Operation::Chop;
using Atlas::Objects::Operation::Burn;
using Atlas::Objects::Operation::Unseen;
using Atlas::Objects::Operation::Update;
using Atlas::Objects::Operation::Use;
using Atlas::Objects::Operation::Wield;

#endif // COMMON_OPERATIONS_H
