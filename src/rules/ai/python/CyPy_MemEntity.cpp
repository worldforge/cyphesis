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

#include "CyPy_MemEntity.h"
#include "common/id.h"

CyPy_MemEntity::CyPy_MemEntity(Py::PythonClassInstanceWeak* self, Py::Tuple& args, Py::Dict& kwds)
    : CyPy_LocatedEntityBase(self, args, kwds)
{
    args.verify_length(1);

    auto arg = args.front();
    if (arg.isString()) {
        auto id = verifyString(args.front());

        long intId = integerId(id);
        if (intId == -1L) {
            throw Py::TypeError("MemEntity() requires string/int ID");
        }
        m_value = new MemEntity(RouterId(intId));
    } else if (CyPy_MemEntity::check(arg)) {
        m_value = CyPy_MemEntity::value(arg);
    } else {
        throw Py::TypeError("MemEntity() requires string ID or MemEntity");
    }
}

CyPy_MemEntity::~CyPy_MemEntity() = default;

void CyPy_MemEntity::init_type()
{
    behaviors().name("MemEntity");
    behaviors().doc("");

    behaviors().supportRichCompare();
    behaviors().supportStr();

    PYCXX_ADD_VARARGS_METHOD(get_child, get_child, "");
    PYCXX_ADD_NOARGS_METHOD(as_entity, as_entity, "");
    PYCXX_ADD_VARARGS_METHOD(can_reach, can_reach, "");
    PYCXX_ADD_NOARGS_METHOD(describe_entity, describe_entity, "");
    PYCXX_ADD_VARARGS_METHOD(client_error, client_error, "");
    PYCXX_ADD_VARARGS_METHOD(is_type, is_type, "Returns true if the current entity is of supplied string type.");

    PYCXX_ADD_VARARGS_METHOD(get_prop_num, get_prop_num, "");
    PYCXX_ADD_VARARGS_METHOD(has_prop_num, has_prop_num, "");
    PYCXX_ADD_VARARGS_METHOD(get_prop_float, get_prop_float, "");
    PYCXX_ADD_VARARGS_METHOD(has_prop_float, has_prop_float, "");
    PYCXX_ADD_VARARGS_METHOD(get_prop_int, get_prop_int, "");
    PYCXX_ADD_VARARGS_METHOD(has_prop_int, has_prop_int, "");
    PYCXX_ADD_VARARGS_METHOD(get_prop_string, get_prop_string, "");
    PYCXX_ADD_VARARGS_METHOD(has_prop_string, has_prop_string, "");
    PYCXX_ADD_VARARGS_METHOD(get_prop_bool, get_prop_bool, "");
    PYCXX_ADD_VARARGS_METHOD(has_prop_bool, has_prop_bool, "");
    PYCXX_ADD_VARARGS_METHOD(get_prop_list, get_prop_list, "");
    PYCXX_ADD_VARARGS_METHOD(has_prop_list, has_prop_list, "");
    PYCXX_ADD_VARARGS_METHOD(get_prop_map, get_prop_map, "");
    PYCXX_ADD_VARARGS_METHOD(has_prop_map, has_prop_map, "");


    //behaviors().type_object()->tp_base = base;

    behaviors().readyType();
    //Also register a provider for LocatedEntity
    LocatedEntityScriptProvider provider{
        [](const Ref<LocatedEntity>& locatedEntity) -> Py::Object {
            auto memEntity = dynamic_cast<MemEntity*>(locatedEntity.get());
            if (memEntity) {
                return WrapperBase<Ref<MemEntity>, CyPy_MemEntity, Py::PythonClassInstanceWeak>::wrap(memEntity);
            }
            return Py::None();
        },
        [](PyObject* obj) -> bool {
            return CyPy_MemEntity::check(obj);
        }, [](const Py::Object& object) -> Ref<LocatedEntity>* {
            if (check(object)) {
                //This cast should work.
                return reinterpret_cast<Ref<LocatedEntity>*>(&WrapperBase<Ref<MemEntity>, CyPy_MemEntity, Py::PythonClassInstanceWeak>::value(object));
            }
            return nullptr;
        }
    };
    CyPy_LocatedEntity::entityPythonProviders.emplace_back(std::move(provider));
}


CyPy_MemEntity::CyPy_MemEntity(Py::PythonClassInstanceWeak* self, Ref<MemEntity> value)
    : CyPy_LocatedEntityBase(self, std::move(value))
{

}
