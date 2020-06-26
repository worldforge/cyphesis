// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2001-2004 Alistair Riddoch
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


#ifndef RULESETS_SCRIPT_H
#define RULESETS_SCRIPT_H

#include "common/OperationRouter.h"
#include <string>
#include <vector>

#include <Atlas/Objects/ObjectsFwd.h>
#include <Atlas/Message/Element.h>

typedef std::vector<Atlas::Objects::Operation::RootOperation> OpVector;

class LocatedEntity;

/// \defgroup Scripts Script types.
///
/// Classes which handle scripts called by the server.

/// \brief Base class for script objects attached to entities.
///
/// This base class allows scripts to override operation handlers, and
/// handlers for hooks.
/// \ingroup Scripts
class Script
{
    public:
        Script() = default;

        virtual ~Script() = default;

        /// \brief Pass an operation to the script for processing
        ///
        /// @param opname The string representing the type of the operation
        /// @param op The operation to be passed
        /// @param res The result of the operation is returned here
        /// @return true if operation was accepted, false if it was not handled
        /// or an error occured.
        virtual HandlerResult operation(const std::string& opname,
                                        const Atlas::Objects::Operation::RootOperation& op,
                                        OpVector& res);

        /// \brief Call a named function on the script, passing in the entity
        ///
        /// This function is used when object have registered function names to be
        /// called any time a certain event occurs. Currently the mechanism is only
        /// used and supported in the mind code.
        /// @param function name of the function to call in the script
        /// @param entity which has triggered this hook
        virtual void hook(const std::string& function, LocatedEntity* entity, OpVector& res);

        virtual void attachPropertyCallbacks(LocatedEntity& entity);

        static int getScriptDetails(const Atlas::Message::MapType&,
                                    const std::string&,
                                    const std::string&,
                                    std::string&,
                                    std::string&);

};

#endif // RULESETS_SCRIPT_H
