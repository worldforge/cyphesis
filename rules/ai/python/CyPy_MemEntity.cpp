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

CyPy_MemEntity::CyPy_MemEntity(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
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
        m_value = new MemEntity(id, intId);
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

    PYCXX_ADD_NOARGS_METHOD(as_entity, as_entity, "");
    PYCXX_ADD_VARARGS_METHOD(can_reach, can_reach, "");
    PYCXX_ADD_NOARGS_METHOD(describe_entity, describe_entity, "");
    PYCXX_ADD_VARARGS_METHOD(client_error, client_error, "");

    //behaviors().type_object()->tp_base = base;

    behaviors().readyType();
    //Also register a provider for LocatedEntity
    LocatedEntityScriptProvider provider{
        [](const Ref<LocatedEntity>& locatedEntity) -> boost::optional<Py::Object> {
            auto memEntity = dynamic_cast<MemEntity*>(locatedEntity.get());
            if (memEntity) {
                return WrapperBase<Ref<MemEntity>, CyPy_MemEntity>::wrap(memEntity);
            }
            return boost::none;
        },
        [](PyObject* obj) -> bool {
            return CyPy_MemEntity::check(obj);
        }, [](const Py::Object& object) -> boost::optional<Ref<LocatedEntity>> {
            if (check(object)) {
                return boost::optional<Ref<LocatedEntity>>(WrapperBase<Ref<MemEntity>, CyPy_MemEntity>::value(object));
            }
            return boost::none;
        }
    };
    CyPy_LocatedEntity::entityPythonProviders.emplace_back(std::move(provider));
}


CyPy_MemEntity::CyPy_MemEntity(Py::PythonClassInstance* self, Ref<MemEntity> value)
    : CyPy_LocatedEntityBase(self, std::move(value))
{

}
