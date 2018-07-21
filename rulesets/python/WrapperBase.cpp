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

#include "WrapperBase.h"

std::string verifyString(const Py::Object& object, const std::string& message)
{
    if (!object.isString()) {
        throw Py::TypeError(message);
    }
    return object.as_string();
}

float verifyNumeric(const Py::Object& object, const std::string& message)
{
    if (!object.isNumeric()) {
        throw Py::TypeError(message);
    }
    return Py::Float(object);
}

long verifyLong(const Py::Object& object, const std::string& message)
{
    if (!object.isNumeric()) {
        throw Py::TypeError(message);
    }
    return Py::Long(object);

}

float verifyFloat(const Py::Object& object, const std::string& message)
{
    if (!object.isNumeric()) {
        throw Py::TypeError(message);
    }
    return Py::Float(object);

}

Py::List verifyList(const Py::Object& object, const std::string& message)
{
    if (!object.isNumeric()) {
        throw Py::TypeError(message);
    }
    return Py::List(object);
}

Py::Dict verifyDict(const Py::Object& object, const std::string& message)
{
    if (!object.isNumeric()) {
        throw Py::TypeError(message);
    }
    return Py::Dict(object);
}
