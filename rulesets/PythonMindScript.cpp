// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "PythonMindScript.h"

#include "Py_Operation.h"
#include "Py_Oplist.h"
#include "Py_Thing.h"

#include "Entity.h"
#include "BaseMind.h"
#include "MemMap.h"

#include "common/log.h"
#include "common/debug.h"

#include <Atlas/Objects/Operation/RootOperation.h>

static const bool debug_flag = false;

PythonMindScript::PythonMindScript(PyObject * o, BaseMind & m) :
    PythonScript(o, m), mind(m)
{
}

PythonMindScript::~PythonMindScript()
{
}

bool PythonMindScript::Operation(const std::string & op_type,
                                 const RootOperation & op,
                                 OpVector & ret_list, RootOperation * sub_op)
{
    std::string op_name = op_type + "_operation";
    debug( std::cout << "Got script object for " << op_name << std::endl
                                                            << std::flush;);
    // Construct apropriate python object thingies from op
    if (!PyObject_HasAttrString(scriptObject, (char *)(op_name.c_str()))) {
        debug( std::cout << "No method found for " << op_name << std::endl
                         << std::flush;);
        return false;
    }
    PyConstOperation * py_op = newPyConstOperation();
    py_op->operation = &op;
    py_op->own = 0;
    py_op->from = mind.m_map.getAdd(op.getFrom());
    py_op->to = mind.m_map.getAdd(op.getTo());
    PyObject * ret;
    if (sub_op == NULL) {
        ret = PyObject_CallMethod(scriptObject, (char *)(op_name.c_str()),
                                         "(O)", py_op);
    } else {
        PyOperation * py_sub_op = newPyOperation();
        py_sub_op->operation = sub_op;
        py_sub_op->own = 0;
        py_sub_op->from = mind.m_map.getAdd(sub_op->getFrom());
        py_sub_op->to = mind.m_map.getAdd(sub_op->getTo());
        ret = PyObject_CallMethod(scriptObject, (char *)(op_name.c_str()),
                                         "(OO)", py_op, py_sub_op);
        Py_DECREF(py_sub_op);
    }
    Py_DECREF(py_op);
    if (ret != NULL) {
        debug( std::cout << "Called python method " << op_name << std::endl
                         << std::flush;);
        if (PyOperation_Check(ret)) {
            PyOperation * op = (PyOperation*)ret;
            if (op->operation != NULL) {
                ret_list.push_back(op->operation);
                op->own = 0;
            } else {
                debug( std::cout << "Method returned invalid op" << std::endl
                                 << std::flush;);
            }
        } else if (PyOplist_Check(ret)) {
            PyOplist * op = (PyOplist*)ret;
            if (op->ops != NULL) {
                const OpVector & o = *op->ops;
                OpVector::const_iterator Iend = o.end();
                for (OpVector::const_iterator I = o.begin(); I != Iend; ++I) {
                    ret_list.push_back(*I);
                }
            } else {
                debug(std::cout << "Method returned invalid OpVector" << std::endl
                                << std::flush;);
            }
        } else {
            debug( std::cout << "Method returned invalid object" << std::endl << std::flush;);
        }
        
        Py_DECREF(ret);
        return true;
    } else {
        if (PyErr_Occurred() == NULL) {
            debug( std::cout << "No method to be found for " << std::endl
                             << std::flush;);
        } else {
            log(ERROR, "Reporting python error");
            PyErr_Print();
            if (op_type == "tick") {
                std::string msg = std::string("Mind script for ") + mind.getId() + ", named " + mind.getName() + " has reported an error processing a tick operation. This NPC is probably now inactive.";
                log(ERROR, msg.c_str());
            }
        }
    }
    return false;
}

void PythonMindScript::hook(const std::string & method, Entity * object)
{
    PyEntity * obj = newPyEntity();
    obj->m_entity = object;
    PyObject * ret = PyObject_CallMethod(scriptObject, (char *)(method.c_str()), "(O)", obj);
    Py_DECREF(ret);
    Py_DECREF(obj);
}
