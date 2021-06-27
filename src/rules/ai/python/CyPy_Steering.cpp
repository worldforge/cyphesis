/*
 Copyright (C) 2020 Erik Ogenvik

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

#include <rules/python/CyPy_Point3D.h>
#include <rules/python/CyPy_EntityLocation.h>
#include <rules/python/CyPy_Location.h>
#include <rules/python/CyPy_LocatedEntity.h>
#include <rules/python/CyPy_Vector3D.h>
#include "CyPy_Steering.h"
#include "navigation/Steering.h"

namespace {
    auto toMeasureType = [](long value) {
        if (value == 0) {
            return Steering::MeasureType::CENTER;
        }
        return Steering::MeasureType::EDGE;
    };

    //Allow both Location and EntityLocation as well as LocatedEntity
    auto toEntityLocation = [](const Py::Object& pyobj) -> EntityLocation {

        if (CyPy_LocatedEntity::check(pyobj)) {
            return EntityLocation(CyPy_LocatedEntity::value(pyobj));
        }
        if (CyPy_Location::check(pyobj)) {
            return CyPy_Location::value(pyobj);
        }
        return verifyObject<CyPy_EntityLocation>(pyobj);
    };
}

CyPy_Steering::CyPy_Steering(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
        : WrapperBase(self, args, kwds)
{
}


CyPy_Steering::CyPy_Steering(Py::PythonClassInstance* self, Ref<AwareMind> value)
        : WrapperBase(self, std::move(value))
{
}


void CyPy_Steering::init_type()
{
    behaviors().name("Steering");
    behaviors().doc("");

    PYCXX_ADD_VARARGS_METHOD(set_destination, setDestination, "");
    PYCXX_ADD_NOARGS_METHOD(is_at_current_destination, isAtCurrentDestination, "");
    PYCXX_ADD_VARARGS_METHOD(set_speed, setSpeed, "");
    PYCXX_ADD_VARARGS_METHOD(query_destination, queryDestination, "");
    // PYCXX_ADD_VARARGS_METHOD(is_at_location, isAtLocation, "");
    PYCXX_ADD_VARARGS_METHOD(distance_to, distanceTo, "");
    PYCXX_ADD_VARARGS_METHOD(direction_to, direction_to, "");
    //PYCXX_ADD_VARARGS_METHOD(distance_between, distanceBetween, "");
    PYCXX_ADD_NOARGS_METHOD(refresh_path, refreshPath, "");

    behaviors().readyType();
}

Py::Object CyPy_Steering::getattro(const Py::String& name)
{
    auto nameStr = name.as_string();

    if (nameStr == "unaware_tiles_count") {


        if (m_value->getSteering()) {
            return Py::Long(static_cast<unsigned long>(m_value->getSteering()->unawareAreaCount()));
        }
        return Py::Long(0);
    }
    if (nameStr == "path") {

        Py::List list;
        if (m_value->getSteering()) {
            const auto& path = m_value->getSteering()->getPath();

            for (auto& point : path) {
                list.append(CyPy_Point3D::wrap(point));
            }
        }

        return list;
    }
    if (nameStr == "current_path_index") {

        Py::List list;
        if (m_value->getSteering()) {
            return Py::Long(static_cast<long>(m_value->getSteering()->getCurrentPathIndex()));
        }

        return Py::Long(0);
    }

    if (nameStr == "path_result") {

        if (!m_value->getSteering()) {
            return Py::Long(0);
        }

        return Py::Long(m_value->getSteering()->getPathResult());
    }


    return PythonExtensionBase::getattro(name);
}


Py::Object CyPy_Steering::refreshPath()
{
    return Py::Long(m_value->updatePath());
}


Py::Object CyPy_Steering::setDestination(const Py::Tuple& args)
{
    if (!m_value->getSteering()) {
        return Py::None();
    }

    //FIXME: provide a "stopSteering" method instead
    if (args.length() == 0) {
        m_value->getSteering()->stopSteering();

    } else {
        args.verify_length(4);


        m_value->getSteering()->setDestination(
                {
                        toEntityLocation(args[0]),
                        toMeasureType(verifyLong(args[1])),
                        toMeasureType(verifyLong(args[2])),
                        verifyFloat(args[3])
                },
                m_value->getCurrentServerTime());

        m_value->getSteering()->startSteering();
    }

    return Py::None();

}


Py::Object CyPy_Steering::setSpeed(const Py::Tuple& args)
{
    args.verify_length(1);

    if (m_value->getSteering()) {
        m_value->getSteering()->setDesiredSpeed(verifyFloat(args[0]));
    }
    return Py::None();
}

Py::Object CyPy_Steering::queryDestination(const Py::Tuple& args)
{
    args.verify_length(1);


    auto& entityLocation = verifyObject<CyPy_EntityLocation>(args[0]);

    auto steering = m_value->getSteering();
    if (!steering) {
        return Py::None();
    }

    auto result = steering->queryDestination(entityLocation, m_value->getCurrentServerTime());

    return Py::Long(result);
}

//Py::Object CyPy_Steering::isAtLocation(const Py::Tuple& args)
//{
//    args.verify_length(1);
//
//    auto& entityLocation = verifyObject<CyPy_Location>(args[0]);
//
//    auto steering = m_value->getSteering();
//    if (!steering) {
//        return Py::None();
//    }
//
//    return Py::False();
////    return Py::Boolean(steering->isAtDestination(m_value->getCurrentServerTime(), entityLocation.m_pos));
//}

Py::Object CyPy_Steering::distanceTo(const Py::Tuple& args)
{
    args.verify_length(3);

    auto steering = m_value->getSteering();
    if (!steering) {
        return Py::None();
    }

    auto distance = steering->distanceTo(m_value->getCurrentServerTime(),
                                         toEntityLocation(args[0]),
                                         toMeasureType(verifyLong(args[1])),
                                         toMeasureType(verifyLong(args[2])));
    if (!distance) {
        return Py::None();
    }
    return Py::Float(*distance);
}

Py::Object CyPy_Steering::direction_to(const Py::Tuple& args)
{
    args.verify_length(1);

    auto steering = m_value->getSteering();
    if (!steering) {
        return Py::None();
    }
    auto direction = steering->directionTo(m_value->getCurrentServerTime(), toEntityLocation(args[0]));
    if (!direction.isValid()) {
        return Py::None();
    }
    return CyPy_Vector3D::wrap(direction);
}


Py::Object CyPy_Steering::isAtCurrentDestination()
{
    if (!m_value->getSteering()) {
        return Py::False();
    }

    return Py::Boolean(m_value->getSteering()->isAtCurrentDestination(m_value->getCurrentServerTime()));

}
