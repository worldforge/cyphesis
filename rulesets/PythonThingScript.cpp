// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "PythonThingScript.h"

#include "Python_API.h"

#include "Thing.h"

#include <common/BaseWorld.h>
#include <common/debug.h>

using Atlas::Objects::Operation::RootOperation;

static const bool debug_flag = false;

PythonThingScript::PythonThingScript(PyObject * o, Thing & t) :
    PythonScript(o, t), thing(t)
{
}

PythonThingScript::~PythonThingScript()
{
}

bool PythonThingScript::Operation(const std::string & op_type,
                                  const RootOperation & op,
                                  oplist & ret_list, RootOperation * sub_op)
{
    if (scriptObject == NULL) {
        debug( std::cout << "No script object asociated" << std::endl
                         << std::flush;);
        return false;
    }
    debug( std::cout << "Got script object for " << std::endl << std::flush;);
    std::string op_name = op_type+"_operation";
    // Construct apropriate python object thingies from op
    if (!PyObject_HasAttrString(scriptObject, (char *)(op_name.c_str()))) {
        debug( std::cout << "No method to be found for " 
                         << "." << op_name << std::endl << std::flush;);
        return false;
    }
    RootOperationObject * py_op = newAtlasRootOperation(NULL);
    py_op->operation = new RootOperation(op);
    py_op->own = 0;
    py_op->from = thing.world->getObject(op.GetFrom());
    py_op->to = thing.world->getObject(op.GetTo());
    PyObject * ret;
    ret = PyObject_CallMethod(scriptObject, (char *)(op_name.c_str()),
                                         "(O)", py_op);
    delete py_op->operation;
    Py_DECREF(py_op);
    if (ret != NULL) {
        debug( std::cout << "Called python method " << op_name
                         << " for object " << std::endl << std::flush;);
        if (PyOperation_Check(ret)) {
            RootOperationObject * op = (RootOperationObject*)ret;
            if (op->operation != NULL) {
                ret_list.push_back(op->operation);
                op->own = 0;
            } else {
                debug( std::cout << "Method returned invalid operation"
                                 << std::endl << std::flush;);
            }
        } else if (PyOplist_Check(ret)) {
            OplistObject * op = (OplistObject*)ret;
            if (op->ops != NULL) {
                ret_list = *op->ops;
            } else {
                debug( std::cout << "Method returned invalid oplist"
                                 << std::endl << std::flush;);
            }
        } else {
            debug( std::cout << "Method returned invalid object" << std::endl
                             << std::flush;);
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

void PythonThingScript::hook(const std::string &, Entity *)
{
}
