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

void verifyString(const Py::Object& object, const std::string& message)
{
    if (!object.isString()) {
        throw Py::TypeError(message);
    }
}

void verifyNumeric(const Py::Object& object, const std::string& message)
{
    if (!object.isNumeric()) {
        throw Py::TypeError(message);
    }
}

void verifyLong(const Py::Object& object, const std::string& message)
{
    if (!object.isNumeric()) {
        throw Py::TypeError(message);
    }
}

void verifyFloat(const Py::Object& object, const std::string& message)
{
    if (!object.isNumeric()) {
        throw Py::TypeError(message);
    }
}

void verifyList(const Py::Object& object, const std::string& message)
{
    if (!object.isNumeric()) {
        throw Py::TypeError(message);
    }
}

void verifyDict(const Py::Object& object, const std::string& message)
{
    if (!object.isNumeric()) {
        throw Py::TypeError(message);
    }
}
