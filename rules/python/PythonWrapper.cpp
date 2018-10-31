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


#include "PythonWrapper.h"
#include "CyPy_Operation.h"
#include "CyPy_Oplist.h"
#include "CyPy_LocatedEntity.h"

#include "common/compose.hpp"
#include "common/log.h"
#include "common/operations/Tick.h"
#include "common/debug.h"
#include "common/log.h"

static const bool debug_flag = false;

/// \brief PythonWrapper constructor
PythonWrapper::PythonWrapper(const Py::Object& wrapper)
    : m_wrapper(wrapper)
{
}

PythonWrapper::~PythonWrapper() = default;


HandlerResult PythonWrapper::operation(const std::string& op_type,
                                            const Operation& op,
                                            OpVector& res)
{
    assert(!m_wrapper.isNull());
    std::string op_name = op_type + "_operation";
    debug_print("Got script object for " << op_name);
    if (!m_wrapper.hasAttr(op_name)) {
        debug_print("No method to be found for " << op_name);
        return OPERATION_IGNORED;
    }

    try {
        auto ret = m_wrapper.callMemberFunction(op_name, Py::TupleN(CyPy_Operation::wrap(op)));

        debug_print("Called python method " << op_name);
        return processScriptResult(op_name, ret, res);

    } catch (const Py::BaseException& py_ex) {
        log(ERROR, String::compose("Python error calling \"%1\" on " +
                                   m_wrapper.as_string(), op_name));
        if (PyErr_Occurred()) {
            PyErr_Print();
        }
        if (op->getClassNo() == Atlas::Objects::Operation::TICK_NO) {
            log(ERROR,
                String::compose("Script for %1 has reported an error "
                                "processing a tick operation. "
                                "This entity is probably now inactive.",
                                m_wrapper.as_string()));
        }
        return OPERATION_IGNORED;
    }
}

void PythonWrapper::hook(const std::string& function,
                              LocatedEntity* entity)
{
    auto wrapper = CyPy_LocatedEntity::wrap(entity);
    if (wrapper.isNull()) {
        return;
    }

    try {
        auto ret = m_wrapper.callMemberFunction(function, Py::TupleN(wrapper));
    } catch (const Py::BaseException& py_ex) {
        log(ERROR, "Could not call hook function " + function + " on " + wrapper.type().as_string());
        if (PyErr_Occurred()) {
            PyErr_Print();
        }
    }

}



HandlerResult PythonWrapper::processScriptResult(const std::string& scriptName, const Py::Object& ret, OpVector& res)
{
    HandlerResult result = OPERATION_IGNORED;

    auto processPythonResultFn = [&](const Py::Object& pythonResult) {
        if (pythonResult.isLong()) {
            auto numRet = Py::Long(pythonResult).as_long();
            if (numRet == 0) {
                result = OPERATION_IGNORED;
            } else if (numRet == 1) {
                result = OPERATION_HANDLED;
            } else if (numRet == 2) {
                result = OPERATION_BLOCKED;
            } else {
                log(ERROR, String::compose("Unrecognized return code %1 for script '%2'", numRet, scriptName));
            }

        } else if (CyPy_Operation::check(pythonResult)) {
            res.push_back(std::move(CyPy_Operation::value(pythonResult)));
        } else if (CyPy_Oplist::check(pythonResult)) {
            auto& o = CyPy_Oplist::value(pythonResult);
            for (auto& opRes : o) {
                res.push_back(opRes);
            }
        } else {
            log(ERROR, String::compose("Python script \"%1\" returned an invalid "
                                       "result.", scriptName));
        }
    };

    if (ret.isNone()) {
        debug_print("Returned none");
    } else {
        //Check if it's a tuple and process it.
        if (ret.isTuple()) {
            for (auto item : Py::Tuple(ret)) {
                processPythonResultFn(item);
            }
        } else {
            processPythonResultFn(ret);
        }
    }

    return result;
}
