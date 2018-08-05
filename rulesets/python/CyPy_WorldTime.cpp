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

#include "CyPy_WorldTime.h"

CyPy_WorldTime::CyPy_WorldTime(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
    : WrapperBase(self, args, kwds)
{
    args.verify_length(1);
    m_value = decltype(m_value){WorldTime(static_cast<int>(verifyLong(args.front())))};
}

CyPy_WorldTime::CyPy_WorldTime(Py::PythonClassInstance* self, WorldTimeWrapper value)
    : WrapperBase(self, std::move(value))
{
}

CyPy_WorldTime::~CyPy_WorldTime()
{
}

void CyPy_WorldTime::init_type()
{
    behaviors().name("WorldTime");
    behaviors().doc("");

    PYCXX_ADD_VARARGS_METHOD(is_now, is_now, "");

    PYCXX_ADD_NOARGS_METHOD(seconds, seconds, "");

    behaviors().readyType();
}

WorldTime& CyPy_WorldTime::get_value()
{
    struct MyVisitor : public boost::static_visitor<WorldTime&> {
        WorldTime& operator()(Ref<BaseMind>& i) const
        {
            return *i->getTime();
        }
        WorldTime& operator()(WorldTime& i) const
        {
            return i;
        }
    };
    return boost::apply_visitor(MyVisitor{}, m_value.m_value);
}


Py::Object CyPy_WorldTime::seconds()
{
    return Py::Float(get_value().seconds());
}

Py::Object CyPy_WorldTime::is_now(const Py::Tuple& args)
{
    args.verify_length(1);
    return Py::Boolean(get_value() == verifyString(args.front()));
}

Py::Object CyPy_WorldTime::getattro(const Py::String& name)
{
    auto element = get_value()[name.as_string()];
    if (!element.empty()) {
        return Py::String(element);
    }
    return PythonExtensionBase::getattro(name);
}
