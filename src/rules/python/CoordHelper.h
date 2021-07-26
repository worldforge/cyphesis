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

#ifndef CYPHESIS_COORDHELPER_H
#define CYPHESIS_COORDHELPER_H

#include "pycxx/CXX/Objects.hxx"
#include "pycxx/CXX/Extensions.hxx"
#include "CyPy_Element.h"
#include <wfmath/vector.h>
#include <wfmath/atlasconv.h>
#include <pythonbase/PythonDebug.h>

struct CoordHelper
{


    template<typename T>
    static Py::Object getattro(T& self, const Py::String& name);

    template<typename T>
    static int setattro(T& coords, const Py::String& name, const Py::Object& object);

    template<typename T>
    static Py::Object repr(const T& coords);

    template<typename TValue, typename TPythonClass>
    static TValue parse(const Py::Object& object);

    template<typename TValue>
    static Py::Boolean is_valid(const TValue& coords);

    template<typename TValue, typename TPythonClass>
    static Py::Object rich_compare(const TValue& coords, const Py::Object& object, int type);

    template<typename TValue>
    static Py::Object sequence_item(const TValue& coords, Py_ssize_t pos);

    template<typename TValue>
    static int sequence_ass_item(TValue& coords, Py_ssize_t pos, const Py::Object& object);

    template<typename TValue>
    static void init(TValue& value, const Py::Tuple& args);
};

template<typename T>
Py::Object CoordHelper::getattro(T& self, const Py::String& name)
{
    std::string nameStr = name;
    if (nameStr == "x") { return Py::Float(self.m_value.x()); }
    if (nameStr == "y") { return Py::Float(self.m_value.y()); }
    if (nameStr == "z") { return Py::Float(self.m_value.z()); }
    return Py::asObject(PyObject_GenericGetAttr(self.selfPtr(), name.ptr()));
}

template<typename T>
int CoordHelper::setattro(T& coords, const Py::String& name, const Py::Object& object)
{
    std::string nameStr = name;
    if (!object.isNumeric()) {
        throw Py::TypeError("Coord attributes must be numeric");
    }
    Py::Float val(object);
    if (nameStr == "x") {
        coords.x() = val.as_double();
    } else if (nameStr == "y") {
        coords.y() = val.as_double();
    } else if (nameStr == "z") {
        coords.z() = val.as_double();
    } else {
        throw Py::AttributeError("Coord attribute does not exist");
    }
    return 0;
}

template<typename T>
Py::Object CoordHelper::repr(const T& coords)
{
    char buf[64];
    ::snprintf(buf, 64, "(%f, %f, %f)", coords.x(), coords.y(), coords.z());
    return Py::String(buf);
}

template<typename TValue, typename TPythonClass>
TValue CoordHelper::parse(const Py::Object& object)
{
    if (TPythonClass::check(object)) {
        return Py::PythonClassObject<TPythonClass>::getCxxObject(object)->m_value;
    }

    if (object.isList() || object.isTuple()) {
        Py::Sequence seq(object);
        if (seq.length() == 3) {
            if (seq[0].isNumeric() && seq[1].isNumeric() && seq[2].isNumeric()) {
                return TValue(Py::Float(seq[0]), Py::Float(seq[1]), Py::Float(seq[2]));
            }
        }
    }

    if (object.isNone()) {
        return TValue{};
    }

    throw Py::TypeError("Object can not be converted into a Coord.");
}

template<typename TValue>
Py::Boolean CoordHelper::is_valid(const TValue& coords)
{
    return Py::Boolean(coords.isValid());
}

template<typename TValue, typename TPythonClass>
Py::Object CoordHelper::rich_compare(const TValue& coords, const Py::Object& object, int type)
{
    if (type == Py_EQ) {
        if (!TPythonClass::check(object)) {
            return Py::False();
        }
        return Py::Boolean(coords == TPythonClass::value(object));
    } else if (type == Py_NE) {
        if (!TPythonClass::check(object)) {
            return Py::True();
        }
        return Py::Boolean(coords != TPythonClass::value(object));
    }
    throw Py::NotImplementedError("Not implemented");
}

template<typename TValue>
Py::Object CoordHelper::sequence_item(const TValue& coords, Py_ssize_t pos)
{
    if (pos < 0 || pos > 2) {
        pythonStackTrace();
        throw Py::IndexError("Index needs to be [0..2]");
    }
    return Py::Float(coords[pos]);
}

template<typename TValue>
int CoordHelper::sequence_ass_item(TValue& coords, Py_ssize_t pos, const Py::Object& object)
{
    if (pos < 0 || pos >= 3) {
        throw Py::IndexError("Coords.[]: Index out of range.");
    }
    if (!object.isNumeric()) {
        throw Py::TypeError("Coords.[]: Value must be numeric.");
    }
    coords[pos] = Py::Float(object).as_double();
    return 0;
}

template<typename TValue>
void CoordHelper::init(TValue& value, const Py::Tuple& args)
{
    switch (args.size()) {
        case 0:
            break;
        case 1: {
            auto arg = args.front();
            if (!arg.isSequence()) {
                throw Py::TypeError("Coords from single value must be a sequence");
            }
            Py::Sequence sequence(arg);
            if (sequence.size() != 3) {
                throw Py::ValueError("Coords from a sequence must be 3 long");
            }
            for (int i = 0; i < 3; i++) {
                auto item = sequence[i];
                if (item.isNumeric()) {
                    value[i] = Py::Float(item).as_double();
                } else if (CyPy_ElementList::check(item)) {
                    value.fromAtlas(CyPy_ElementList::value(item));
                } else {
                    throw Py::TypeError("Coords must take list of floats, or ints");
                }
            }
            value.setValid();
        }
            break;
        case 3:

            for (int i = 0; i < 3; i++) {
                auto item = args[i];

                if (item.isNumeric()) {
                    value[i] = Py::Float(item).as_double();
                } else if (CyPy_ElementList::check(item)) {
                    value.fromAtlas(CyPy_ElementList::value(item));
                } else {
                    throw Py::TypeError("Coords must take list of floats, or ints");
                }
            }
            value.setValid();
            break;
        default:
            throw Py::TypeError("Coords must take list of floats, or ints, 3 ints or 3 floats");
    }
}


#endif //CYPHESIS_COORDHELPER_H
