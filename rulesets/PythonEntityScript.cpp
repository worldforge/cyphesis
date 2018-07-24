// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
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

#include "PythonEntityScript.h"

#include "Py_Operation.h"
#include "Py_Oplist.h"
#include "Py_Thing.h"

#include "common/Tick.h"

#include "common/log.h"
#include "common/debug.h"
#include "common/compose.hpp"
#include "common/OperationRouter.h"

#include <iostream>
#include <memory>
#include <utility>
#include <rulesets/python/CyPy_Operation.h>
#include <rulesets/python/CyPy_Oplist.h>
#include <rulesets/python/CyPy_LocatedEntity.h>

static const bool debug_flag = false;

/// \brief PythonEntityScript constructor
PythonEntityScript::PythonEntityScript(Py::Object obj) :
                    PythonWrapper(std::move(obj))
{
}

HandlerResult PythonEntityScript::operation(const std::string & op_type,
                                   const Operation & op,
                                   OpVector & res)
{
        assert(!m_wrapper.isNull());
        std::string op_name = op_type + "_operation";
        debug(std::cout << "Got script object for " << op_name << std::endl
                        << std::flush;);
        // This check isn't really necessary, except it saves the conversion
        // time.
        if (!m_wrapper.hasAttr(op_name)) {
            debug(std::cout << "No method to be found for " << op_name
                            << std::endl << std::flush;);
            return OPERATION_IGNORED;
        }

        if (!m_wrapper.isCallable()) {
            debug(std::cout << "Python object can't be called"
                            << std::endl << std::flush;);
            return OPERATION_IGNORED;
        }
    try {
        Py::Callable callable(m_wrapper);
        auto ret = callable.callMemberFunction(op_name, CyPy_Operation::wrap(op));
        if (ret.isNull()) {
                log(ERROR, String::compose("Python error calling \"%1\"", op_name));
                PyErr_Print();
                if (op->getClassNo() == Atlas::Objects::Operation::TICK_NO) {
                    log(ERROR,
                        String::compose("Script for \"%1\" has reported an error "
                                        "processing a tick operation. "
                                        "This entity is probably now inactive.",
                                        op->getTo()));
                }
            return OPERATION_IGNORED;
        }
        debug(std::cout << "Called python method " << op_name
                        << std::endl << std::flush;);

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
                    log(ERROR, String::compose("Unrecognized return code %1 for operation handler '%2'", numRet, op_name));
                }

            } else if (CyPy_Operation::check(pythonResult)) {
                auto operation = CyPy_Operation::value(pythonResult);
                assert(operation);
                //Filter out raw operations, as these are meant to be used to short circuit goals. They should thus never be sent on.
                if (operation->getParent() != "operation") {
                    res.push_back(std::move(operation));
                }
            } else if (CyPy_Oplist::check(pythonResult)) {
                auto& o = CyPy_Oplist::value(pythonResult);
                for (auto& opRes : o) {
                    if (opRes->getParent() != "operation") {
                        res.push_back(opRes);
                    }
                }
            } else {
                log(ERROR, String::compose("Python script \"%1\" returned an invalid "
                                           "result.", op_name));
            }
        };

        if (ret.isNone()) {
            debug(std::cout << "Returned none" << std::endl << std::flush;);
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
    } catch (const Py::BaseException& py_ex) {
        log(ERROR, String::compose("Python error calling \"%1\"", op_name));
        PyErr_Print();
        if (op->getClassNo() == Atlas::Objects::Operation::TICK_NO) {
            log(ERROR,
                String::compose("Script for \"%1\" has reported an error "
                                "processing a tick operation. "
                                "This entity is probably now inactive.",
                                op->getTo()));
        }
        return OPERATION_IGNORED;
    }
}

void PythonEntityScript::hook(const std::string & function,
                              LocatedEntity * entity)
{
    auto wrapper = CyPy_LocatedEntity::wrap(entity);
    if (wrapper.isNull()) {
        return;
    }

    try {
        auto ret = m_wrapper.callMemberFunction(function, wrapper);
    } catch (const Py::BaseException& py_ex) {
        log(ERROR, "Reporting python error");
        PyErr_Print();
    }

}
