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

#include <rulesets/mind/AwareMind.h>
#include "navigation/Steering.h"
#include "CyPy_BaseMind.h"
#include "CyPy_Point3D.h"
#include "CyPy_MemMap.h"
#include "CyPy_WorldTime.h"

CyPy_BaseMind::CyPy_BaseMind(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
    : CyPy_LocatedEntityBase(self, args, kwds)
{
    args.verify_length(1);

    auto arg = args.front();
    if (arg.isString()) {
        auto id = verifyString(args.front());

        long intId = integerId(id);
        if (intId == -1L) {
            throw Py::TypeError("Mind() requires string/int ID");
        }
        m_value = new BaseMind(id, intId);
    } else if (CyPy_BaseMind::check(arg)) {
        m_value = CyPy_BaseMind::value(arg);
    } else {
        throw Py::TypeError("Mind() requires string ID or Mind");
    }
    m_value->incRef();
}

CyPy_BaseMind::~CyPy_BaseMind() = default;

void CyPy_BaseMind::init_type()
{
    behaviors().name("Mind");
    behaviors().doc("");

    behaviors().supportRichCompare();

    PYCXX_ADD_NOARGS_METHOD(as_entity, as_entity, "");
    PYCXX_ADD_VARARGS_METHOD(is_reachable_for_other_entity, is_reachable_for_other_entity, "");
    PYCXX_ADD_NOARGS_METHOD(describe_entity, describe_entity, "");
    PYCXX_ADD_VARARGS_METHOD(client_error, client_error, "");

    PYCXX_ADD_VARARGS_METHOD(setDestination, setDestination, "");
    PYCXX_ADD_NOARGS_METHOD(refreshPath, refreshPath, "");

    //behaviors().type_object()->tp_base = base;

    behaviors().readyType();
}

CyPy_BaseMind::CyPy_BaseMind(Py::PythonClassInstance* self, Ref<BaseMind> value)
    : CyPy_LocatedEntityBase(self, std::move(value))
{

}

Py::Object CyPy_BaseMind::refreshPath()
{

    auto awareMind = dynamic_cast<AwareMind*>(m_value.get());
    if (!awareMind) {
        throw Py::TypeError("Not an AwareMind");
    }
    return Py::Long(awareMind->updatePath());
}


Py::Object CyPy_BaseMind::setDestination(const Py::Tuple& args)
{
    auto awareMind = dynamic_cast<AwareMind*>(m_value.get());
    if (!awareMind) {
        throw Py::TypeError("Not an AwareMind");
    }

    //FIXME: provide a "stopSteering" method instead
    if (args.length() == 0) {
        awareMind->getSteering().stopSteering();

    } else {
        args.verify_length(2, 3);

        auto destination = verifyObject<CyPy_Point3D>(args[0]);
        float radius = verifyNumeric(args[1]);

        if (!destination.isValid()) {
            throw Py::RuntimeError("Destination must be a valid location.");
        }

        long entityId;
        //If no entity id was specified, the location is relative to the parent entity.
        if (args.size() == 3) {
            auto entityIdString = verifyString(args[2]);
            entityId = std::stol(entityIdString);
        } else {
            if (awareMind->m_location.m_loc) {
                entityId = awareMind->m_location.m_loc->getIntId();
            } else {
                throw Py::RuntimeError("Mind has no location.");
            }
        }

        awareMind->getSteering().setDestination(entityId, destination, radius, awareMind->getCurrentServerTime());
        awareMind->getSteering().startSteering();
    }

    return Py::None();

}

Py::Object CyPy_BaseMind::getattro(const Py::String& name)
{
    auto nameStr = name.as_string();
    if (nameStr == "map") {
        return CyPy_MemMap::wrap(m_value->getMap());
    }
    if (nameStr == "unawareTilesCount") {

        auto awareMind = dynamic_cast<AwareMind*>(m_value.get());
        if (!awareMind) {
            throw Py::TypeError("Not an AwareMind");
        }
        return Py::Long(awareMind->getSteering().unawareAreaCount());
    }
    if (nameStr == "path") {
        auto awareMind = dynamic_cast<AwareMind*>(m_value.get());
        if (!awareMind) {
            throw Py::TypeError("Not an AwareMind");
        }
        Py::List list;
        const auto& path = awareMind->getSteering().getPath();

        for (auto& point : path) {
            list.append(CyPy_Point3D::wrap(point));
        }

        return list;
    }

    if (nameStr == "pathResult") {
        auto awareMind = dynamic_cast<AwareMind*>(m_value.get());
        if (!awareMind) {
            throw Py::TypeError("Not an AwareMind");
        }

        return Py::Long(awareMind->getSteering().getPathResult());
    }


    if (nameStr == "time") {
        return CyPy_WorldTime::wrap(WorldTimeWrapper(decltype(WorldTimeWrapper::m_value){m_value}));
    }

    return CyPy_LocatedEntityBase::getattro(name);
}

int CyPy_BaseMind::setattro(const Py::String& name, const Py::Object& attr)
{
    auto nameStr = name.as_string();

//    if (nameStr == "mind") {
//        if (!PyMind_Check(v)) {
//            PyErr_SetString(PyExc_TypeError, "Mind.mind requires Mind");
//            return -1;
//        }
//        self->m_entity.m = ((PyEntity*) v)->m_entity.m;
//        return 0;
//    }

    if (nameStr == "map") {
        throw Py::AttributeError("Setting map on mind is forbidden");
    }

    return CyPy_LocatedEntityBase::setattro(name, attr);
}

