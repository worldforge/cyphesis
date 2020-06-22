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

#ifndef CYPHESIS_CYPY_SERVER_H
#define CYPHESIS_CYPY_SERVER_H

#include "pycxx/CXX/Extensions.hxx"

class BaseWorld;
/**
 * \ingroup PythonWrappers
 */
class CyPy_Server : public Py::ExtensionModule<CyPy_Server>
{
    public:
        CyPy_Server();

        static void registerWorld(BaseWorld* world);

        static std::string init();

    private:

        Py::Object get_alias_entity(const Py::Tuple& args);
        PYCXX_VARARGS_METHOD_DECL(CyPy_Server, get_alias_entity);

};


#endif //CYPHESIS_CYPY_SERVER_H
