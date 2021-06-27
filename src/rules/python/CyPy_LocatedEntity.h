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

#ifndef CYPHESIS_CYPY_LOCATEDENTITY_H
#define CYPHESIS_CYPY_LOCATEDENTITY_H


#include <boost/optional.hpp>
#include "rules/LocatedEntity.h"
#include "pythonbase/WrapperBase.h"

/**
 * \ingroup PythonWrappers
 */
template<typename TValue, typename TPythonClass>
class CyPy_LocatedEntityBase : public WrapperBase<TValue, TPythonClass, Py::PythonClassInstanceWeak>
{
    public:
        CyPy_LocatedEntityBase(Py::PythonClassInstanceWeak* self, const Py::Tuple& args, const Py::Dict& kwds);

        ~CyPy_LocatedEntityBase() override;

        Py::Object getattro(const Py::String&) override;

        int setattro(const Py::String& name, const Py::Object& attr) override;

        Py::Object rich_compare(const Py::Object&, int) override;

        static Py::Object wrap(LocatedEntity* value);

        Py::Object str() override;


    protected:


        CyPy_LocatedEntityBase(Py::PythonClassInstanceWeak* self, TValue value);

        Py::Object get_child(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_LocatedEntityBase, get_child);

        Py::Object as_entity();

        PYCXX_NOARGS_METHOD_DECL(CyPy_LocatedEntityBase, as_entity);

        Py::Object can_reach(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_LocatedEntityBase, can_reach);

        Py::Object describe_entity();

        PYCXX_NOARGS_METHOD_DECL(CyPy_LocatedEntityBase, describe_entity);

        Py::Object client_error(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_LocatedEntityBase, client_error);

        Py::Object is_type(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_LocatedEntityBase, is_type);


        Py::Object get_prop_num(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_LocatedEntityBase, get_prop_num);

        Py::Object has_prop_num(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_LocatedEntityBase, has_prop_num);


        Py::Object get_prop_float(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_LocatedEntityBase, get_prop_float);

        Py::Object has_prop_float(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_LocatedEntityBase, has_prop_float);


        Py::Object get_prop_int(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_LocatedEntityBase, get_prop_int);

        Py::Object has_prop_int(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_LocatedEntityBase, has_prop_int);


        Py::Object get_prop_string(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_LocatedEntityBase, get_prop_string);

        Py::Object has_prop_string(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_LocatedEntityBase, has_prop_string);


        Py::Object get_prop_bool(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_LocatedEntityBase, get_prop_bool);

        Py::Object has_prop_bool(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_LocatedEntityBase, has_prop_bool);


        Py::Object get_prop_list(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_LocatedEntityBase, get_prop_list);

        Py::Object has_prop_list(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_LocatedEntityBase, has_prop_list);


        Py::Object get_prop_map(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_LocatedEntityBase, get_prop_map);

        Py::Object has_prop_map(const Py::Tuple& args);

        PYCXX_VARARGS_METHOD_DECL(CyPy_LocatedEntityBase, has_prop_map);

        Py::Object getPropertyFromFirstArg(const Py::Tuple& args, const std::function<Py::Object(const Atlas::Message::Element&, Py::Object)>& checkFn) const;

        Py::Object hasPropertyFromFirstArg(const Py::Tuple& args, const std::function<bool(const Atlas::Message::Element&)>& checkFn) const;


};

/**
 * \ingroup PythonWrappers
 */
struct LocatedEntityScriptProvider {
    std::function<Py::Object(const Ref<LocatedEntity>& locatedEntity)> wrapFn;
    std::function<bool(PyObject*)> checkFn;
    std::function<Ref<LocatedEntity>*(const Py::Object&)> valueFn;

};

/**
 * \ingroup PythonWrappers
 */
class CyPy_LocatedEntity
{
    public:

        typedef Ref<LocatedEntity> value_type;
        struct type_def
        {
            std::string tp_name = "LocatedEntity";
        };

        static type_def* type_object()
        {
            static type_def type;
            return &type;
        }

        static bool check(PyObject* object);

        static bool check(const Py::Object& object);

        static Py::Object wrap(Ref<LocatedEntity> value);

        static Ref<LocatedEntity>& value(const Py::Object& object);

        static std::vector<LocatedEntityScriptProvider> entityPythonProviders;


};





Py::Object wrapLocatedEntity(Ref<LocatedEntity> locatedEntity);



#endif //CYPHESIS_CYPY_LOCATEDENTITY_H
