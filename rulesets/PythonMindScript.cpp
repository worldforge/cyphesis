// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "PythonMindScript.h"

#include "Py_Operation.h"
#include "Py_Oplist.h"
#include "Py_Thing.h"

#include <common/debug.h>

#include "Entity.h"
#include "BaseMind.h"
#include "MemMap_methods.h"

using Atlas::Objects::Operation::RootOperation;

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
    debug( std::cout << "Got script object for " << std::endl << std::flush;);
    std::string op_name = op_type+"_operation";
    // Construct apropriate python object thingies from op
    if (!PyObject_HasAttrString(scriptObject, (char *)(op_name.c_str()))) {
        debug( std::cout << "No method found for " << op_name << std::endl
                         << std::flush;);
        return false;
    }
    ConstOperationObject * py_op = newAtlasConstRootOperation(NULL);
    py_op->operation = &op;
    py_op->own = 0;
    py_op->from = mind.map.getAdd(op.GetFrom());
    py_op->to = mind.map.getAdd(op.GetTo());
    PyObject * ret;
    if (sub_op == NULL) {
        ret = PyObject_CallMethod(scriptObject, (char *)(op_name.c_str()),
                                         "(O)", py_op);
    } else {
        OperationObject * py_sub_op = newAtlasRootOperation(NULL);
        py_sub_op->operation = sub_op;
        py_sub_op->own = 0;
        py_sub_op->from = mind.map.getAdd(sub_op->GetFrom());
        py_sub_op->to = mind.map.getAdd(sub_op->GetTo());
        ret = PyObject_CallMethod(scriptObject, (char *)(op_name.c_str()),
                                         "(OO)", py_op, py_sub_op);
        Py_DECREF(py_sub_op);
    }
    Py_DECREF(py_op);
    if (ret != NULL) {
        debug( std::cout << "Called python method " << op_name << std::endl
                         << std::flush;);
        if (PyOperation_Check(ret)) {
            OperationObject * op = (OperationObject*)ret;
            if (op->operation != NULL) {
                ret_list.push_back(op->operation);
                op->own = 0;
            } else {
                debug( std::cout << "Method returned invalid op" << std::endl
                                 << std::flush;);
            }
        } else if (PyOplist_Check(ret)) {
            OplistObject * op = (OplistObject*)ret;
            if (op->ops != NULL) {
                ret_list = *op->ops;
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
            std::cerr << "Reporting python error for " << std::endl
                      << std::flush;
            PyErr_Print();
        }
    }
    return false;
}

void PythonMindScript::hook(const std::string & method, Entity * object)
{
    ThingObject * obj = newThingObject(NULL);
    obj->m_thing = object;
    PyObject * ret = PyObject_CallMethod(scriptObject, (char *)(method.c_str()), "(O)", obj);
    Py_DECREF(ret);
    Py_DECREF(obj);
}
