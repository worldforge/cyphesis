// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef COMMON_OPERATIONS_H
#define COMMON_OPERATIONS_H

// Forward declarations of all the operation classes used in cyphesis. Use
// of this file reduces complexities in the dependency tree, and speeds up
// compile time.

#include <Atlas/Objects/Operation/RootOperation.h>

namespace Atlas {
    namespace Objects {
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
            class Sight;
            class Sound;
            class Touch;
            class Talk;
            class Look;
            class Info;
            class Appearance;
            class Disappearance;
            class Error;
            class Load;
            class Save;
            class Setup;
            class Tick;
            class Eat;
            class Nourish;
            class Cut;
            class Chop;
            class Fire;
            class Generic;
            class RootOperation;
        }
    }
}

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
using Atlas::Objects::Operation::Load;
using Atlas::Objects::Operation::Save;
using Atlas::Objects::Operation::Setup;
using Atlas::Objects::Operation::Tick;
using Atlas::Objects::Operation::Eat;
using Atlas::Objects::Operation::Nourish;
using Atlas::Objects::Operation::Cut;
using Atlas::Objects::Operation::Chop;
using Atlas::Objects::Operation::Fire;

#endif // COMMON_OPERATIONS_H
