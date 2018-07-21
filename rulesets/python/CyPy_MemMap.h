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

#ifndef CYPHESIS_CYPY_MEMMAP_H
#define CYPHESIS_CYPY_MEMMAP_H

#include <rulesets/MemMap.h>
#include "WrapperBase.h"

class CyPy_MemMap : public WrapperBase<MemMap*, CyPy_MemMap>
{
    public:
        CyPy_MemMap(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds);

        CyPy_MemMap(Py::PythonClassInstance* self, MemMap* value);

        Py::Object find_by_location(const Py::Tuple& args);

        Py::Object find_by_type(const Py::Tuple& args);

        Py::Object updateAdd(const Py::Tuple& args);
};


#endif //CYPHESIS_CYPY_MEMMAP_H
