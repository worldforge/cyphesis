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

#ifndef CYPHESIS_CYPY_CREATORCLIENT_H
#define CYPHESIS_CYPY_CREATORCLIENT_H


#include "CreatorClient.h"
#include "pythonbase/WrapperBase.h"

class CyPy_CreatorClient : public WrapperBase<Ref<CreatorClient>, CyPy_CreatorClient>
{
    public:
        CyPy_CreatorClient(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds);

        CyPy_CreatorClient(Py::PythonClassInstance* self, Ref<CreatorClient> value);

        ~CyPy_CreatorClient() override;

        static void init_type();

        Py::Object getattro(const Py::String& name) override;

        int setattro(const Py::String& name, const Py::Object& attr) override;

        Py::Object make(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_CreatorClient, make);

        Py::Object as_entity();

        PYCXX_NOARGS_METHOD_DECL(CyPy_CreatorClient, as_entity);

        Py::Object set(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_CreatorClient, set);

        Py::Object look(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_CreatorClient, look);

        Py::Object look_for(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_CreatorClient, look_for);

        Py::Object send(const Py::Tuple& argsp);

        PYCXX_VARARGS_METHOD_DECL(CyPy_CreatorClient, send);

        Py::Object delete_(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_CreatorClient, delete_);


    protected:

        std::map<std::string, Py::Object> m_dict;
};


#endif //CYPHESIS_CYPY_CREATORCLIENT_H
