/*
 Copyright (C) 2018 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef CYPHESIS_SCRIPTUTILS_H
#define CYPHESIS_SCRIPTUTILS_H


#include "common/OperationRouter.h"
#include "pycxx/CXX/Objects.hxx"
#include "rules/LocatedEntity.h"
#include <string>

class ScriptUtils
{
    public:
        static HandlerResult processScriptResult(const std::string& scriptName,
                                                 const Py::Object& ret,
                                                 OpVector& res,
                                                 LocatedEntity& e);
};


#endif //CYPHESIS_SCRIPTUTILS_H
