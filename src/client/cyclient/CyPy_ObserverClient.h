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

#ifndef CYPHESIS_CYPY_OBSERVERCLIENT_H
#define CYPHESIS_CYPY_OBSERVERCLIENT_H

#include "pythonbase/WrapperBase.h"
#include "ObserverClient.h"

class CyPy_ObserverClient : public WrapperBase<ObserverClient*, CyPy_ObserverClient>
{
    public:
        CyPy_ObserverClient(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds);

        CyPy_ObserverClient(Py::PythonClassInstance* self, ObserverClient* value);

        static void init_type();

        Py::Object getattro(const Py::String& name) override;

        int setattro(const Py::String& name, const Py::Object& attr) override;


        Py::Object setup(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_ObserverClient, setup);

        Py::Object teardown();

        PYCXX_NOARGS_METHOD_DECL(CyPy_ObserverClient, teardown);

        Py::Object create_avatar(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_ObserverClient, create_avatar);

        Py::Object run();

        PYCXX_NOARGS_METHOD_DECL(CyPy_ObserverClient, run);

        Py::Object send(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_ObserverClient, send);

        Py::Object send_wait(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_ObserverClient, send_wait);

        Py::Object wait();

        PYCXX_NOARGS_METHOD_DECL(CyPy_ObserverClient, wait);

};


#endif //CYPHESIS_CYPY_OBSERVERCLIENT_H
