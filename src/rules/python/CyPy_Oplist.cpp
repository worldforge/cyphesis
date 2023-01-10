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

#include "CyPy_Oplist.h"
#include "CyPy_Operation.h"

CyPy_Oplist::CyPy_Oplist(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
    : WrapperBase(self, args, kwds)
{
    for (auto op : args) {
        appendToList(op, m_value);
    }
}

CyPy_Oplist::CyPy_Oplist(Py::PythonClassInstance* self, std::vector<Atlas::Objects::Operation::RootOperation> value)
    : WrapperBase(self, std::move(value))
{
}

void CyPy_Oplist::init_type()
{
    behaviors().name("Oplist");
    behaviors().doc("A list of Atlas Operation.");

    behaviors().supportNumberType(Py::PythonType::support_number_add);

    behaviors().supportSequenceType(Py::PythonType::support_sequence_length
                                    | Py::PythonType::support_sequence_inplace_concat);

    PYCXX_ADD_VARARGS_METHOD(append, append, PYCXX_SIG_DOC("append(object)", "Appends new operations. The supplied object can be a single Operation, or another Oplist, or a sequence of the aforementioned two."));

    behaviors().readyType();
}

void CyPy_Oplist::appendToList(const Py::Object& op, std::vector<Atlas::Objects::Operation::RootOperation>& list)
{
    if (CyPy_Operation::check(op)) {
        auto& rootOp = CyPy_Operation::value(op);
        if (!rootOp.isValid()) {
            throw Py::ValueError("Invalid Operation in other of Oplist.num_add");
        }
        list.push_back(rootOp);
    } else if (CyPy_Oplist::check(op)) {
        auto& opList = CyPy_Oplist::value(op);
        for (auto& entry : opList) {
            if (!entry.isValid()) {
                throw Py::ValueError("Invalid Operation in other of Oplist.num_add");
            }
            list.push_back(entry);
        }
    } else if (op.isSequence()) {
        Py::Sequence seq(op);
        for (auto innerOp : seq) {
            appendToList(innerOp, list);
        }
    } else if (!op.isNone() && !op.isNull()) {
        throw Py::TypeError("Append must be an op or sequence");
    }
}

Py::Object CyPy_Oplist::append(const Py::Tuple& args)
{
    for (auto& op : args) {
        appendToList(op, m_value);
    }
    return Py::None();
}

Py::Object CyPy_Oplist::number_add(const Py::Object& other)
{
    auto list = m_value;

    appendToList(other, list);

    return wrap(std::move(list));
}

Py::Object CyPy_Oplist::sequence_inplace_concat(const Py::Object& other)
{
    appendToList(other, m_value);
    return self();
}

PyCxx_ssize_t CyPy_Oplist::sequence_length()
{
    return m_value.size();
}

