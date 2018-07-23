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

#ifndef CYPHESIS_CYPY_CHARACTER_H
#define CYPHESIS_CYPY_CHARACTER_H

#include "rulesets/Character.h"
#include "CyPy_LocatedEntity.h"

class CyPy_Character : public CyPy_LocatedEntityBase<Ref<Character>, CyPy_Character>
{
    public:
        CyPy_Character(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds);
        CyPy_Character(Py::PythonClassInstance* self, Ref<Character> value);

        ~CyPy_Character() override;

        static void init_type();

    protected:

        Py::Object start_task(const Py::Tuple& args);
        PYCXX_VARARGS_METHOD_DECL(CyPy_Character, start_task)

        Py::Object mind2body(const Py::Tuple& args);
        PYCXX_VARARGS_METHOD_DECL(CyPy_Character, mind2body)

        Py::Object send_world(const Py::Tuple& args);
        PYCXX_VARARGS_METHOD_DECL(CyPy_Character, send_world)
};


#endif //CYPHESIS_CYPY_CHARACTER_H
