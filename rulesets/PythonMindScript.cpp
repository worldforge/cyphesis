// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include "PythonMindScript.h"

#include "Python_API.h"

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

bool PythonMindScript::Operation(const string& op_type, const RootOperation& op,
                        oplist & ret_list, RootOperation * sub_op)
{
    if (scriptObject != NULL) {
        debug( cout << "Got script object for " << endl << flush;);
        string op_name = op_type+"_operation";
        // Construct apropriate python object thingies from op
        if (!PyObject_HasAttrString(scriptObject, (char *)(op_name.c_str()))) {
            debug( cout << "No method to be found for " 
                 << "." << op_name << endl << flush;);
            return false;
        }
        RootOperationObject * py_op = newAtlasRootOperation(NULL);
        py_op->operation = new RootOperation(op);
        py_op->own = 0;
        py_op->from = mind.map.getAdd(op.GetFrom());
        py_op->to = mind.map.getAdd(op.GetTo());
        PyObject * ret;
        if (sub_op == NULL) {
            ret = PyObject_CallMethod(scriptObject, (char *)(op_name.c_str()),
                                             "(O)", py_op);
        } else {
            RootOperationObject * py_sub_op = newAtlasRootOperation(NULL);
            py_sub_op->operation = sub_op;
            py_sub_op->own = 0;
            py_sub_op->from = mind.map.getAdd(sub_op->GetFrom());
            py_sub_op->to = mind.map.getAdd(sub_op->GetTo());
            ret = PyObject_CallMethod(scriptObject, (char *)(op_name.c_str()),
                                             "(OO)", py_op, py_sub_op);
            Py_DECREF(py_sub_op);
        }
        delete py_op->operation;
        Py_DECREF(py_op);
        if (ret != NULL) {
            debug( cout << "Called python method " << op_name
                                << " for object " << endl << flush;);
            if (PyOperation_Check(ret)) {
                RootOperationObject * op = (RootOperationObject*)ret;
                if (op->operation != NULL) {
                    ret_list.push_back(op->operation);
                    op->own = 0;
                } else {
                    debug( cout << "Method returned invalid operation"
                         << endl << flush;);
                }
            } else if (PyOplist_Check(ret)) {
                OplistObject * op = (OplistObject*)ret;
                if (op->ops != NULL) {
                    ret_list = *op->ops;
                } else {
                    debug( cout << "Method returned invalid oplist"
                         << endl << flush;);
                }
            } else {
                debug( cout << "Method returned invalid object" << endl << flush;);
            }
            
            Py_DECREF(ret);
            return true;
        } else {
            if (PyErr_Occurred() == NULL) {
                debug( cout << "No method to be found for " << endl << flush;);
            } else {
                cerr << "Reporting python error for " << endl << flush;
                PyErr_Print();
            }
        }
    } else {
        debug( cout << "No script object asociated" << endl << flush;);
    }
    return false;
}

void PythonMindScript::hook(const string & method, Entity * object)
{
    // Should be able to scrap the NULL check
    if (scriptObject != NULL) {
        ThingObject * obj = newThingObject(NULL);
        obj->m_thing = object;
        PyObject_CallMethod(scriptObject, (char *)(method.c_str()), "(O)",obj);
        Py_DECREF(obj);
    }
}
