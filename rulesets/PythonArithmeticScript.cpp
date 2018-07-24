// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2005 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA


#include <Python.h>

#include "PythonArithmeticScript.h"

#include "common/log.h"
#include "common/compose.hpp"

/// \brief PythonArithmeticScript
///
/// @param script Python instance object implementing the script
PythonArithmeticScript::PythonArithmeticScript(Py::Object script) :
    m_script(std::move(script))
{
}

PythonArithmeticScript::~PythonArithmeticScript()
{
    if (m_script.reference_count() != 1) {
        log(WARNING, String::compose("Deleting arithmetic script with %1 > 1 refs to its script", m_script.reference_count()));
    }
}

int PythonArithmeticScript::attribute(const std::string& name, float& val)
{
    auto ret = m_script.getAttr(name);
    if (ret.isNull()) {
        if (PyErr_Occurred() == nullptr) {
            // std::cout << "No attribute method" << std::endl << std::flush;
        } else {
            log(ERROR, "Reporting python error");
            PyErr_Print();
        }
        return -1;
    }
    if (ret.isNumeric()) {
        val = static_cast<float>(Py::Float(ret).as_double());
    } else if (ret.isNone()) {
        return -1;
    } else {
        log(ERROR, "Invalid response from script");
        return -1;
    }
    return 0;
}

void PythonArithmeticScript::set(const std::string& name, const float& val)
{
    m_script.setAttr(name, Py::Float(val));
}
