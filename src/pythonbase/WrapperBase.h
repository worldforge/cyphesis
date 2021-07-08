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

#include <common/compose.hpp>
#include <common/log.h>
#include "pycxx/CXX/Extensions.hxx"

template<typename TValue, typename TPythonClass, typename TClassInstance=Py::PythonClassInstance>
class WrapperBase : public Py::PythonClass<TPythonClass, TClassInstance>
{
    public:

        typedef TValue value_type;

        WrapperBase(TClassInstance* self, const Py::Tuple& args, const Py::Dict& kwds);

        static Py::Object wrap(TValue value);

        static TValue& value(const Py::Object& object);

        TValue m_value;

        void reinit(Py::Tuple&/*args*/, Py::Dict&/*kwds*/ )
        {
            //noop
        }
        //Py::Object rich_compare(const Py::Object&, int) override;

    protected:
        WrapperBase(TClassInstance* self, TValue value);

        /**
         * This differs from Py::PythonClass::extension_object_new in that it will create the C++ instance directly.
         */
        static PyObject* wrapper_extension_object_new(PyTypeObject* subtype, PyObject* args_, PyObject* kwds_)
        {
#ifdef PYCXX_DEBUG
            std::cout << "extension_object_new()" << std::endl;
#endif
            auto* o = reinterpret_cast<TClassInstance*>( subtype->tp_alloc(subtype, 0));
            if (o == nullptr) {
                return nullptr;
            }

            Py::Tuple args(args_);
            Py::Dict kwds;
            if (kwds_ != nullptr) {
                kwds = kwds_;
            }

            //Create the C++ object here already, before __init__ is called. This is to guarantee that there's
            //an C++ instance even if a subclass forgets to call __init__ on the superclass.
            try {
                o->m_pycxx_object = new TPythonClass(o, args, kwds);
                TClassInstance::_new(o);
            } catch (const Py::BaseException& e) {
                log(WARNING, String::compose("Error when creating Python class %1.", subtype->tp_name));
                subtype->tp_free(o);
                return nullptr;
            }


            auto* self = reinterpret_cast<PyObject*>( o );

#ifdef PYCXX_DEBUG
            std::cout << "extension_object_new() => self=0x" << std::hex << reinterpret_cast< unsigned long >( self ) << std::dec << std::endl;
#endif
            return self;
        }

        static Py::PythonType& behaviors()
        {
            //We need to replace the standard "new" function with our own.
            auto& type = Py::PythonClass<TPythonClass, TClassInstance>::behaviors();
            type.set_tp_new(wrapper_extension_object_new);
            return type;
        }

};


template<typename TValue, typename TPythonClass, typename TClassInstance>
WrapperBase<TValue, TPythonClass, TClassInstance>::WrapperBase(TClassInstance* self, const Py::Tuple& args, const Py::Dict& kwds)
        : Py::PythonClass<TPythonClass, TClassInstance>::PythonClass(self, args, kwds)
{}

template<typename TValue, typename TPythonClass, typename TClassInstance>
WrapperBase<TValue, TPythonClass, TClassInstance>::WrapperBase(TClassInstance* self, TValue value)
        : Py::PythonClass<TPythonClass, TClassInstance>::PythonClass(self), m_value(std::move(value))
{}

template<typename TValue, typename TPythonClass, typename TClassInstance>
Py::Object WrapperBase<TValue, TPythonClass, TClassInstance>::wrap(TValue value)
{
    //Use the base class extension_object_new since it will only allocate memory and won't create the C++ instance
    auto obj = Py::PythonClass<TPythonClass, TClassInstance>::extension_object_new(Py::PythonClass<TPythonClass, TClassInstance>::type_object(), nullptr, nullptr);
    reinterpret_cast<TClassInstance*>(obj)->m_pycxx_object = new TPythonClass(reinterpret_cast<TClassInstance*>(obj), std::move(value));
    return Py::PythonClassObject<TPythonClass>(obj, true);
}


template<typename TValue, typename TPythonClass, typename TClassInstance>
TValue& WrapperBase<TValue, TPythonClass, TClassInstance>::value(const Py::Object& object)
{
    return Py::PythonClassObject<TPythonClass>::getCxxObject(object)->m_value;
}

//template<typename TValue, typename TPythonClass>
//Py::Object WrapperBase<TValue, TPythonClass>::rich_compare(const Py::Object& other, int op)
//{
//    if (op == Py_EQ || op == Py_NE) {
//        if (!TPythonClass::check(other)) {
//            return Py_EQ == op ? Py::False() : Py::True();
//        }
//        if (op == Py_EQ) {
//            return Py::Boolean(m_value == TPythonClass::value(other));
//        } else if (op == Py_NE) {
//            return Py::Boolean(m_value != TPythonClass::value(other));
//        }
//
//    }
//    throw Py::NotImplementedError("Objects can only be checked for == or !=");
//}

std::string verifyString(const Py::Object& object, const std::string& message = "Must be string.");

float verifyNumeric(const Py::Object& object, const std::string& message = "Must be numeric.");

long verifyLong(const Py::Object& object, const std::string& message = "Must be long.");

float verifyFloat(const Py::Object& object, const std::string& message = "Must be float.");

Py::List verifyList(const Py::Object& object, const std::string& message = "Must be list.");

Py::Dict verifyDict(const Py::Object& object, const std::string& message = "Must be dict.");

template<typename T>
typename T::value_type& verifyObject(const Py::Object& object, const std::string& message = "")
{
    if (!T::check(object)) {
        if (message.empty()) {
            throw Py::TypeError(String::compose("Must be %1. Instead got %2", T::type_object()->tp_name, object.type().as_string()));
        }
        throw Py::TypeError(message);
    }
    return T::value(object);
}


#endif //CYPHESIS_WRAPPERBASE_H
