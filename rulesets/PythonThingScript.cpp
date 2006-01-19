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

#include "PythonThingScript.h"

#include "Py_Operation.h"
#include "Py_Oplist.h"

#include "Entity.h"

#include "common/log.h"
#include "common/debug.h"
#include "common/compose.hpp"

#include <Atlas/Objects/RootOperation.h>

static const bool debug_flag = false;

PythonEntityScript::PythonEntityScript(PyObject * o, PyObject * wrapper) :
                    PythonScript(o, wrapper)
{
}

PythonEntityScript::~PythonEntityScript()
{
}

bool PythonEntityScript::operation(const std::string & op_type,
                                   const Operation & op,
                                   OpVector & ret_list,
                                   const Operation * sub_op)
{
    assert(scriptObject != NULL);
    std::string op_name = op_type + "_operation";
    debug( std::cout << "Got script object for " << op_name << std::endl
                                                            << std::flush;);
    // This check isn't really necessary, except it saves the conversion
    // time.
    if (!PyObject_HasAttrString(scriptObject, (char *)(op_name.c_str()))) {
        debug( std::cout << "No method to be found for " << op_name
                         << std::endl << std::flush;);
        return false;
    }
    // Construct apropriate python object thingies from op
    PyConstOperation * py_op = newPyConstOperation();
    py_op->operation = op;
    PyObject * ret;
    ret = PyObject_CallMethod(scriptObject, (char *)(op_name.c_str()),
                                         "(O)", py_op);
    Py_DECREF(py_op);
    if (ret == NULL) {
        if (PyErr_Occurred() == NULL) {
            debug( std::cout << "No method to be found for " << std::endl
                             << std::flush;);
        } else {
            log(ERROR, "Reporting python error");
            PyErr_Print();
        }
        return false;
    }
    debug( std::cout << "Called python method " << op_name
                     << std::endl << std::flush;);
    if (ret == Py_None) {
        debug(std::cout << "Returned none" << std::endl << std::flush;);
    } else if (PyOperation_Check(ret)) {
        PyOperation * op = (PyOperation*)ret;
        assert(op->operation.isValid());
        ret_list.push_back(op->operation);
    } else if (PyOplist_Check(ret)) {
        PyOplist * op = (PyOplist*)ret;
        assert(op->ops != NULL);
        const OpVector & o = *op->ops;
        OpVector::const_iterator Iend = o.end();
        for (OpVector::const_iterator I = o.begin(); I != Iend; ++I) {
            ret_list.push_back(*I);
        }
    } else {
       log(ERROR, String::compose("Python script \"%1\" returned an invalid result", op_name).c_str());
    }
    
    Py_DECREF(ret);
    return true;
}

void PythonEntityScript::hook(const std::string &, Entity *)
{
}
