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

#ifndef CYPHESIS_WRAPPERBASE_H
#define CYPHESIS_WRAPPERBASE_H

#include "external/pycxx/CXX/Objects.hxx"
#include "external/pycxx/CXX/Extensions.hxx"

template<typename TValue, typename TPythonClass>
class WrapperBase : public Py::PythonClass<TPythonClass>
{
    public:

        typedef TValue value_type;

        WrapperBase(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds);

        static Py::PythonClassObject<TPythonClass> wrap(TValue value);

        static TValue& value(const Py::Object& object);

        TValue m_value;

    protected:
        WrapperBase(Py::PythonClassInstance* self, TValue value);

};

template<typename TValue, typename TPythonClass>
WrapperBase<TValue, TPythonClass>::WrapperBase(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
    : Py::PythonClass<TPythonClass>::PythonClass(self, args, kwds)
{}

template<typename TValue, typename TPythonClass>
WrapperBase<TValue, TPythonClass>::WrapperBase(Py::PythonClassInstance* self, TValue value)
    : Py::PythonClass<TPythonClass>::PythonClass(self), m_value(std::move(value))
{}

template<typename TValue, typename TPythonClass>
Py::PythonClassObject<TPythonClass> WrapperBase<TValue, TPythonClass>::wrap(TValue value)
{
    auto obj = Py::PythonClass<TPythonClass>::extension_object_new(Py::PythonClass<TPythonClass>::type_object(), nullptr, nullptr);
    reinterpret_cast<Py::PythonClassInstance*>(obj)->m_pycxx_object = new TPythonClass(reinterpret_cast<Py::PythonClassInstance*>(obj), std::move(value));
    return Py::PythonClassObject<TPythonClass>(obj);
}


template<typename TValue, typename TPythonClass>
TValue& WrapperBase<TValue, TPythonClass>::value(const Py::Object& object)
{
    return Py::PythonClassObject<TPythonClass>(object).getCxxObject()->m_value;
}

std::string verifyString(const Py::Object& object, const std::string& message = "Must be string");

float verifyNumeric(const Py::Object& object, const std::string& message = "Must be numeric");

long verifyLong(const Py::Object& object, const std::string& message = "Must be long");

float verifyFloat(const Py::Object& object, const std::string& message = "Must be float");

Py::List verifyList(const Py::Object& object, const std::string& message = "Must be list");

Py::Dict verifyDict(const Py::Object& object, const std::string& message = "Must be dict");

template <typename T>
typename T::value_type& verifyObject(const Py::Object& object, const std::string& message = "")
{
    if (T::check(object)) {
        if (message.empty()) {
            throw Py::TypeError("Must be a " + std::string(T::type_object()->tp_name));
        }
        throw Py::TypeError(message);
    }
    return T::value(object);
}


#endif //CYPHESIS_WRAPPERBASE_H
