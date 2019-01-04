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

#include "CyPy_BaseMind.h"
#include "CyPy_MemMap.h"
#include "CyPy_MemEntity.h"
#include "rules/python/CyPy_Point3D.h"
#include "rules/python/CyPy_Props.h"
#include "rules/python/PythonScriptFactory.h"
#include "rules/python/CyPy_WorldTime.h"
#include "rules/entityfilter/python/CyPy_EntityFilter.h"
#include "rules/entityfilter/Providers.h"
#include "../AwareMind.h"


#include "navigation/Steering.h"

#include "common/id.h"

Py::Object wrapPython(BaseMind* value) {
    return CyPy_BaseMind::wrap(value);
}

CyPy_BaseMind::CyPy_BaseMind(Py::PythonClassInstance* self, Ref<BaseMind> value)
    : WrapperBase(self, value)
{

}

CyPy_BaseMind::CyPy_BaseMind(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
    : WrapperBase(self, args, kwds)
{
    args.verify_length(1);

    auto arg = args.front();
//    if (arg.isString()) {
//        auto id = verifyString(args.front());
//
//        long intId = integerId(id);
//        if (intId == -1L) {
//            throw Py::TypeError("Mind() requires string/int ID");
//        }
//        m_value = new BaseMind(id, intId);
//    } else
    if (CyPy_BaseMind::check(arg)) {
        m_value = CyPy_BaseMind::value(arg);
    } else {
        throw Py::TypeError("Mind() requires string ID or Mind");
    }
}

CyPy_BaseMind::~CyPy_BaseMind() = default;

void CyPy_BaseMind::init_type()
{
    behaviors().name("Mind");
    behaviors().doc("");

    behaviors().supportRichCompare();

    behaviors().supportStr();

//    PYCXX_ADD_NOARGS_METHOD(as_entity, as_entity, "");
//    PYCXX_ADD_VARARGS_METHOD(can_reach, can_reach, "");
//    PYCXX_ADD_NOARGS_METHOD(describe_entity, describe_entity, "");
//    PYCXX_ADD_VARARGS_METHOD(client_error, client_error, "");

    //TODO: split into a CyPy_Steering class
    PYCXX_ADD_VARARGS_METHOD(set_destination, setDestination, "");
    PYCXX_ADD_VARARGS_METHOD(set_speed, setSpeed, "");

    PYCXX_ADD_VARARGS_METHOD(add_property_callback, addPropertyCallback, "");
    PYCXX_ADD_NOARGS_METHOD(refresh_path, refreshPath, "");


    PYCXX_ADD_VARARGS_METHOD(match_entity, matchEntity, "");
    PYCXX_ADD_VARARGS_METHOD(match_entities, matchEntities, "");

    //behaviors().type_object()->tp_base = base;

    behaviors().readyType();
}

Py::Object CyPy_BaseMind::matchEntity(const Py::Tuple& args)
{
    args.verify_length(2);

    auto& filter = verifyObject<CyPy_Filter>(args.getItem(0));
    auto& entity = verifyObject<CyPy_LocatedEntity>(args.getItem(1));

    EntityFilter::QueryContext queryContext = CyPy_MemMap::createFilterContext(entity.get(), m_value->getMap());
    queryContext.actor = m_value->getEntity().get();

    return Py::Boolean(filter->match(queryContext));
}

Py::Object CyPy_BaseMind::matchEntities(const Py::Tuple& args)
{
    args.verify_length(2);

    auto& filter = verifyObject<CyPy_Filter>(args.getItem(0));
    auto entities = verifyList(args.getItem(1));
    Py::List list;

    for (auto entity : entities) {
        EntityFilter::QueryContext queryContext = CyPy_MemMap::createFilterContext(verifyObject<CyPy_LocatedEntity>(entity).get(), m_value->getMap());
        queryContext.actor = m_value->getEntity().get();

        if (filter->match(queryContext)) {
            list.append(entity);
        }
    }

    return list;
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

    if (!awareMind->getSteering()) {
        return Py::None();
    }

    //FIXME: provide a "stopSteering" method instead
    if (args.length() == 0) {
        awareMind->getSteering()->stopSteering();

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
            if (awareMind->getEntity()->m_location.m_parent) {
                entityId = awareMind->getEntity()->m_location.m_parent->getIntId();
            } else {
                throw Py::RuntimeError("Mind has no location.");
            }
        }

        awareMind->getSteering()->setDestination(entityId, destination, radius, awareMind->getCurrentServerTime());
        awareMind->getSteering()->startSteering();
    }

    return Py::None();

}

Py::Object CyPy_BaseMind::getattro(const Py::String& name)
{
    auto nameStr = name.as_string();
    if (nameStr == "entity") {
        if (m_value->getEntity()) {
            return WrapperBase<Ref<MemEntity>, CyPy_MemEntity>::wrap(m_value->getEntity());
        }
        return Py::None();
    }
    if (nameStr == "id") {
        return Py::String(m_value->getId());
    }
    if (nameStr == "map") {
        return CyPy_MemMap::wrap(m_value->getMap());
    }
    if (nameStr == "unawareTilesCount") {

        auto awareMind = dynamic_cast<AwareMind*>(m_value.get());
        if (!awareMind) {
            throw Py::TypeError("Not an AwareMind");
        }
        if (awareMind->getSteering()) {
            return Py::Long(static_cast<unsigned long>(awareMind->getSteering()->unawareAreaCount()));
        }
        return Py::Long(0);
    }
    if (nameStr == "path") {
        auto awareMind = dynamic_cast<AwareMind*>(m_value.get());
        if (!awareMind) {
            throw Py::TypeError("Not an AwareMind");
        }
        Py::List list;
        if (awareMind->getSteering()) {
            const auto& path = awareMind->getSteering()->getPath();

            for (auto& point : path) {
                list.append(CyPy_Point3D::wrap(point));
            }
        }

        return list;
    }

    if (nameStr == "pathResult") {
        auto awareMind = dynamic_cast<AwareMind*>(m_value.get());
        if (!awareMind) {
            throw Py::TypeError("Not an AwareMind");
        }

        if (!awareMind->getSteering()) {
            return Py::Long(0);
        }

        return Py::Long(awareMind->getSteering()->getPathResult());
    }

    if (nameStr == "props") {
        return CyPy_Props::wrap(m_value->getEntity());
    }


    if (nameStr == "time") {
        return CyPy_WorldTime::wrap(m_value->getTime());
    }

    return PythonExtensionBase::getattro(name);
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

    return PythonExtensionBase::setattro(name, attr);
}

Py::Object CyPy_BaseMind::addPropertyCallback(const Py::Tuple& args)
{
    args.verify_length(2);
    m_value->addPropertyScriptCallback(verifyString(args[0]), verifyString(args[1]));
    return Py::None();
}

Py::Object CyPy_BaseMind::str()
{
    if (m_value->getEntity()) {
        return Py::String(String::compose("BaseMind %1, entity %2", m_value->getId(), m_value->getEntity()->describeEntity()));
    }
    return Py::String(String::compose("BaseMind %1", m_value->getId()));
}

Py::Object CyPy_BaseMind::setSpeed(const Py::Tuple& args)
{
    args.verify_length(1);
    auto awareMind = dynamic_cast<AwareMind*>(m_value.get());
    if (!awareMind) {
        throw Py::TypeError("Not an AwareMind");
    }
    if (awareMind->getSteering()) {
        awareMind->getSteering()->setDesiredSpeed(verifyFloat(args[0]));
    }
    return Py::None();
}

template<>
PythonScriptFactory<BaseMind>::PythonScriptFactory(const std::string & package,
                                                   const std::string & type) :
    PythonClass(package,
                type)
{
}
template class PythonScriptFactory<BaseMind>;

#include "rules/python/PythonScriptFactory_impl.h"
