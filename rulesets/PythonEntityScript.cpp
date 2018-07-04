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

static const bool debug_flag = false;

/// \brief PythonEntityScript constructor
PythonEntityScript::PythonEntityScript(PyObject * o) :
                    PythonWrapper(o)
{
}

HandlerResult PythonEntityScript::operation(const std::string & op_type,
                                   const Operation & op,
                                   OpVector & res)
{
    assert(m_wrapper != nullptr);
    std::string op_name = op_type + "_operation";
    debug( std::cout << "Got script object for " << op_name << std::endl
                                                            << std::flush;);
    // This check isn't really necessary, except it saves the conversion
    // time.
    if (!PyObject_HasAttrString(m_wrapper, (char *)(op_name.c_str()))) {
        debug( std::cout << "No method to be found for " << op_name
                         << std::endl << std::flush;);
        return OPERATION_IGNORED;
    }

    // Construct appropriate python object thingies from op
    PyOperation * py_op = newPyConstOperation();
    if (py_op == nullptr) {
        return OPERATION_IGNORED;
    }
    py_op->operation = op;
    PyObject * ret = PyObject_CallMethod(m_wrapper, (char *)(op_name.c_str()),
                                            (char *)"(O)", py_op);
    Py_DECREF(py_op);
    if (ret == nullptr) {
        if (PyErr_Occurred() == nullptr) {
            debug( std::cout << "No method to be found for " << std::endl
                             << std::flush;);
        } else {
            log(ERROR, String::compose("Python error calling \"%1\"", op_name));
            PyErr_Print();
            if (op->getClassNo() == Atlas::Objects::Operation::TICK_NO) {
                log(ERROR,
                    String::compose("Script for \"%1\" has reported an error "
                                    "processing a tick operation. "
                                    "This entity is probably now inactive.",
                                    op->getTo()));
            }
        }
        return OPERATION_IGNORED;
    }
    debug( std::cout << "Called python method " << op_name
                     << std::endl << std::flush;);

    HandlerResult result = OPERATION_IGNORED;

    auto processPythonResultFn = [&](PyObject* pythonResult){
        if (PyLong_Check(pythonResult)) {
            auto numRet = PyLong_AsLong(pythonResult);
            if (numRet == 0) {
                result = OPERATION_IGNORED;
            } else if (numRet == 1) {
                result = OPERATION_HANDLED;
            } else if (numRet == 2) {
                result = OPERATION_BLOCKED;
            } else {
                log(ERROR, String::compose("Unrecognized return code %1 for operation handler '%2'", numRet, op_name));
            }

        } else if (PyOperation_Check(pythonResult)) {
            auto *operation = (PyOperation *) pythonResult;
            assert(operation->operation.isValid());
            //Filter out raw operations, as these are meant to be used to short circuit goals. They should thus never be sent on.
            if (operation->operation->getParent() != "operation") {
                res.push_back(operation->operation);
            }
        } else if (PyOplist_Check(pythonResult)) {
            auto *oplist = (PyOplist *) pythonResult;
            assert(oplist->ops != nullptr);
            const OpVector &o = *oplist->ops;
            auto Iend = o.end();
            for (auto I = o.begin(); I != Iend; ++I) {
                //Filter out raw operations, as these are meant to be used to short circuit goals. They should thus never be sent on.
                if ((*I)->getParent() != "operation") {
                    res.push_back(*I);
                }
            }
        } else {
            log(ERROR, String::compose("Python script \"%1\" returned an invalid "
                                       "result.", op_name));
        }
    };

    if (ret == Py_None) {
        debug(std::cout << "Returned none" << std::endl << std::flush;);
    } else {
        //Check if it's a tuple and process it.
        if (PyTuple_Check(ret)) {
            auto size = PyTuple_Size(ret);
            for (Py_ssize_t i = 0; i < size; ++i) {
                processPythonResultFn(PyTuple_GetItem(ret, i));
            }
        } else {
            processPythonResultFn(ret);
        }
    }

    Py_DECREF(ret);
    return result;
}

void PythonEntityScript::hook(const std::string & function,
                              LocatedEntity * entity)
{
    PyObject * wrapper = wrapEntity(entity);
    if (wrapper == nullptr) {
        return;
    }

    PyObject * ret = PyObject_CallMethod(m_wrapper,
                                         (char *)(function.c_str()),
                                         (char *)"(O)",
                                         wrapper);
    Py_DECREF(wrapper);
    if (ret == nullptr) {
        if (PyErr_Occurred() == nullptr) {
            log(NOTICE, "No hook");
        } else {
            log(ERROR, "Reporting python error");
            PyErr_Print();
        }
    } else {
        Py_DECREF(ret);
    }
}
