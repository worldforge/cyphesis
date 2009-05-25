// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2005 Alistair Riddoch
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

// $Id$

#include "Python.h"

#include "Python_API.h"
#include "Python_Script_Utils.h"

#include "Py_BBox.h"
#include "Py_Object.h"
#include "Py_Thing.h"
#include "Py_Mind.h"
#include "Py_Map.h"
#include "Py_Location.h"
#include "Py_Vector3D.h"
#include "Py_Point3D.h"
#include "Py_Quaternion.h"
#include "Py_WorldTime.h"
#include "Py_World.h"
#include "Py_Operation.h"
#include "Py_RootEntity.h"
#include "Py_Oplist.h"
#include "Py_Property.h"
#include "Py_Task.h"

#include "PythonEntityScript.h"
#include "BaseMind.h"

#include "common/Inheritance.h"
#include "common/compose.hpp"
#include "common/globals.h"
#include "common/const.h"
#include "common/debug.h"
#include "common/log.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

using Atlas::Message::Element;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::RootOperation;
using Atlas::Objects::Entity::Anonymous;

static const bool debug_flag = false;

/// \defgroup PythonWrappers Python Wrapper Types
///
/// Structure types based on the PyObject header used to wrap C++ objects
/// in Python.

/// \brief Python wrapper for C++ functions to be exposed to Python
typedef struct {
    PyObject_HEAD
} FunctionObject;

static void Function_dealloc(FunctionObject * self)
{
    PyObject_Free(self);
}

static PyObject * log_debug(PyObject * self, PyObject * args, PyObject * kwds)
{
    if (consts::debug_level != 0) {
        int level;
        char *message;
        PyObject * op;

        if (!PyArg_ParseTuple(args, "is|O", &level, &message, &op)) {
            return NULL;
        }

        if (consts::debug_level >= level) {
            log(SCRIPT, message);
        }
    }

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * log_think(PyObject * self, PyObject * args, PyObject * kwds)
{
    if (consts::debug_thinking != 0) {
        char *message;

        if (!PyArg_ParseTuple(args, "s", &message)) {
            return NULL;
        }

        log(SCRIPT, message);
    }

    Py_INCREF(Py_None);
    return Py_None;
}

PyTypeObject log_debug_type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,
        "Function",
        sizeof(FunctionObject),
        0,
        /* methods */
        (destructor)Function_dealloc,
        0,              /* tp_print */
        0,              /* tp_getattr */
        0,              /* tp_setattr */
        0,              /* tp_compare */
        0,              /* tp_repr */
        0,              /* tp_as_number */
        0,              /* tp_as_sequence */
        0,              /* tp_as_mapping */
        0,              /* tp_hash */
        log_debug,      /* tp_call */
};

PyTypeObject log_think_type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,
        "Function",
        sizeof(FunctionObject),
        0,
        /* methods */
        (destructor)Function_dealloc,
        0,              /* tp_print */
        0,              /* tp_getattr */
        0,              /* tp_setattr */
        0,              /* tp_compare */
        0,              /* tp_repr */
        0,              /* tp_as_number */
        0,              /* tp_as_sequence */
        0,              /* tp_as_mapping */
        0,              /* tp_hash */
        log_think,      /* tp_call */
};

//////////////////////////////////////////////////////////////////////////
// Logger replaces sys.stdout and sys.stderr so the nothing goes to output
//////////////////////////////////////////////////////////////////////////

/// \brief Python struct to handle output from python scripts
///
/// In instance of this struct is used to replace sys.stdout and sys.stderr
/// in the Python interpreter so that all script output goes to the cyphesis
/// log subsystem
typedef struct {
    PyObject_HEAD
} PyLogger;

static void python_log(LogLevel lvl, const char * msg)
{
    static std::string message;

    message += msg;
    std::string::size_type n = 0;
    std::string::size_type p;
    for (p = message.find_first_of('\n');
         p != std::string::npos;
         p = message.find_first_of('\n', n)) {
        log(lvl, message.substr(n, p - n));
        n = p + 1;
    }
    if (message.size() > n) {
        message = message.substr(n, message.size() - n);
    } else {
        message.clear();
    }
}

static PyObject * PyOutLogger_write(PyObject * self, PyObject * arg)
{
    if (!PyString_CheckExact(arg)) {
        PyErr_SetString(PyExc_TypeError, "write must be a string");
        return 0;
    }
    char * mesg = PyString_AsString(arg);

    python_log(SCRIPT, mesg);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * PyErrLogger_write(PyObject * self, PyObject * arg)
{
    if (!PyString_CheckExact(arg)) {
        PyErr_SetString(PyExc_TypeError, "write must be a string");
        return 0;
    }
    char * mesg = PyString_AsString(arg);

    python_log(SCRIPT_ERROR, mesg);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef PyOutLogger_methods[] = {
    {"write",       PyOutLogger_write,          METH_O},
    {NULL,          NULL}                       /* Sentinel */
};

static PyMethodDef PyErrLogger_methods[] = {
    {"write",       PyErrLogger_write,          METH_O},
    {NULL,          NULL}                       /* Sentinel */
};

static void PyLogger_dealloc(PyObject * self)
{
    PyObject_Free(self);
}

static PyObject * PyOutLogger_getattr(PyObject * self, char *name)
{
    return Py_FindMethod(PyOutLogger_methods, self, name);
}

static PyObject * PyErrLogger_getattr(PyObject * self, char *name)
{
    return Py_FindMethod(PyErrLogger_methods, self, name);
}

PyTypeObject PyOutLogger_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                   // ob_size
        "OutLogger",         // tp_name
        sizeof(PyLogger),    // tp_basicsize
        0,                   // tp_itemsize
        //  methods 
        PyLogger_dealloc,    // tp_dealloc
        0,                   // tp_print
        PyOutLogger_getattr, // tp_getattr
        0,                   // tp_setattr
        0,                   // tp_compare
        0,                   // tp_repr
        0,                   // tp_as_number
        0,                   // tp_as_sequence
        0,                   // tp_as_mapping
        0,                   // tp_hash
};

PyTypeObject PyErrLogger_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                   // ob_size
        "ErrLogger",         // tp_name
        sizeof(PyLogger),    // tp_basicsize
        0,                   // tp_itemsize
        //  methods 
        PyLogger_dealloc,    // tp_dealloc
        0,                   // tp_print
        PyErrLogger_getattr, // tp_getattr
        0,                   // tp_setattr
        0,                   // tp_compare
        0,                   // tp_repr
        0,                   // tp_as_number
        0,                   // tp_as_sequence
        0,                   // tp_as_mapping
        0,                   // tp_hash
};

static PyObject * Get_PyClass(const std::string & package,
                              const std::string & type)
{
    std::string classname(type);
    classname[0] = toupper(classname[0]);
    PyObject * package_name = PyString_FromString((char *)package.c_str());
    PyObject * module = PyImport_Import(package_name);
    Py_DECREF(package_name);
    if (module == NULL) {
        log(ERROR, String::compose("Missing python module \"%1\"", package));
        PyErr_Print();
        return NULL;
    }
    PyObject * py_class = PyObject_GetAttrString(module, (char *)classname.c_str());
    Py_DECREF(module);
    if (py_class == NULL) {
        log(ERROR, String::compose("Could not find python class \"%1.%2\"",
                                   package, classname));
        PyErr_Print();
        return NULL;
    }
    if (PyCallable_Check(py_class) == 0) {
        log(ERROR, String::compose("Could not instance python class \"%1.%2\"",
                                   package, classname));
        Py_DECREF(py_class);
        return NULL;
    }
#if 0
    // In later versions of python using PyType_* will become the right thing
    // to do. This might become true when things have been done right with
    // installing types.
    if (PyType_Check(py_class) == 0) {
        std::cerr << "PyCallable_Check returned true, but PyType_Check returned false " << package << "." << type << std::endl << std::flush;
    } else {
        std::cerr << "PyType_Check returned true" << std::endl << std::flush;
    }
#endif
    return py_class;
}

PyObject * Create_PyScript(PyObject * wrapper, PyObject * py_class)
{
    PyObject * pyob = PyEval_CallFunction(py_class,"(O)", wrapper);
    
    if (pyob == NULL) {
        if (PyErr_Occurred() == NULL) {
            log(ERROR, "Could not create python instance");
        } else {
            log(ERROR, "Reporting python error");
            PyErr_Print();
        }
    }
    Py_DECREF(wrapper);
    return pyob;
}

void Create_PyMind(BaseMind * mind, const std::string & package,
                                    const std::string & type)
{
    PyObject * py_class = Get_PyClass(package, type);
    if (py_class == NULL) { return; }
    PyMind * wrapper = newPyMind();
    wrapper->m_mind = mind;
    PyObject * o = Create_PyScript((PyObject *)wrapper, py_class);
    Py_DECREF(py_class);

    if (o != NULL) {
        mind->setScript(new PythonEntityScript(o, (PyObject *)wrapper));
    }
}

static PyObject * is_location(PyObject * self, PyObject * loc)
{
    if (PyLocation_Check(loc)) {
        Py_INCREF(Py_True);
        return Py_True;
    }
    Py_INCREF(Py_False);
    return Py_False;
}

static PyObject * distance_to(PyObject * self, PyObject * args)
{
    PyObject * near, * other;
    if (!PyArg_ParseTuple(args, "OO", &near, &other)) {
        return NULL;
    }
    if (!PyLocation_Check(near) || !PyLocation_Check(other)) {
        PyErr_SetString(PyExc_TypeError, "Arg Location required");
        return NULL;
    }
    PyLocation * sloc = (PyLocation *)near,
               * oloc = (PyLocation *)other;
#ifndef NDEBUG
    if (sloc->location == NULL || oloc == NULL) {
        PyErr_SetString(PyExc_AssertionError, "Null location pointer");
        return NULL;
    }
#endif // NDEBUG
    PyVector3D * ret = newPyVector3D();
    if (ret == NULL) {
        return NULL;
    }
    ret->coords = distanceTo(*sloc->location, *oloc->location);
    return (PyObject *)ret;
}

static PyObject * square_distance(PyObject * self, PyObject * args)
{
    PyObject * near, * other;
    if (!PyArg_ParseTuple(args, "OO", &near, &other)) {
        return NULL;
    }
    if (!PyLocation_Check(near) || !PyLocation_Check(other)) {
        PyErr_SetString(PyExc_TypeError, "Arg Location required");
        return NULL;
    }
    PyLocation * sloc = (PyLocation *)near,
               * oloc = (PyLocation *)other;
#ifndef NDEBUG
    if (sloc->location == NULL || oloc == NULL) {
        PyErr_SetString(PyExc_AssertionError, "Null location pointer");
        return NULL;
    }
#endif // NDEBUG
    return PyFloat_FromDouble(squareDistance(*sloc->location, *oloc->location));
}

static PyObject * square_horizontal_distance(PyObject * self, PyObject * args)
{
    PyObject * near, * other;
    if (!PyArg_ParseTuple(args, "OO", &near, &other)) {
        return NULL;
    }
    if (!PyLocation_Check(near) || !PyLocation_Check(other)) {
        PyErr_SetString(PyExc_TypeError, "Arg Location required");
        return NULL;
    }
    PyLocation * sloc = (PyLocation *)near,
               * oloc = (PyLocation *)other;
#ifndef NDEBUG
    if (sloc->location == NULL || oloc == NULL) {
        PyErr_SetString(PyExc_AssertionError, "Null location pointer");
        return NULL;
    }
#endif // NDEBUG
    return PyFloat_FromDouble(squareHorizontalDistance(*sloc->location, *oloc->location));
}

// In Python 2.3 or later this it is okay to pass in null for the methods
// of a module, making this obsolete.
static PyMethodDef no_methods[] = {
    {NULL,          NULL}                       /* Sentinel */
};

static PyMethodDef atlas_methods[] = {
    {"isLocation", is_location,                 METH_O},
    {NULL,          NULL}                       /* Sentinel */
};

static PyMethodDef physics_methods[] = {
    {"distance_to",distance_to,                 METH_VARARGS},
    {"square_distance",square_distance,         METH_VARARGS},
    {"square_horizontal_distance",
      square_horizontal_distance,               METH_VARARGS},
    {NULL,          NULL}                       /* Sentinel */
};

static PyMethodDef common_methods[] = {
    //{"null",      null_new,                   METH_VARARGS},
    {NULL,          NULL}                       /* Sentinel */
};

void init_python_api()
{
    Py_Initialize();

    PyObject * sys_name = PyString_FromString("sys");
    PyObject * sys_module = PyImport_Import(sys_name);
    Py_DECREF(sys_name);

    if (sys_module == 0) {
        log(CRITICAL, "Python could not import sys module");
        return;
    }

    PyObject * out_logger = (PyObject*)PyObject_NEW(PyLogger, &PyOutLogger_Type);
    PyObject_SetAttrString(sys_module, "stdout", out_logger);
    Py_DECREF(out_logger);

    PyObject * err_logger = (PyObject*)PyObject_NEW(PyLogger, &PyErrLogger_Type);
    PyObject_SetAttrString(sys_module, "stderr", err_logger);
    Py_DECREF(err_logger);

    PyObject * sys_path = PyObject_GetAttrString(sys_module, "path");
    if (sys_path != 0) {
        if (PyList_Check(sys_path)) {
            // Add the path to the non-ruleset specific code.
            std::string p = share_directory + "/cyphesis/scripts";
            PyObject * path = PyString_FromString(p.c_str());
            PyList_Append(sys_path, path);
            Py_DECREF(path);

            p = share_directory + "/cyphesis/rulesets/basic";
            path = PyString_FromString(p.c_str());
            PyList_Append(sys_path, path);
            Py_DECREF(path);

            // Add the path to the ruleset specific code.
            p = share_directory + "/cyphesis/rulesets/" + ruleset;
            path = PyString_FromString(p.c_str());
            PyList_Append(sys_path, path);
            Py_DECREF(path);
        } else {
            log(CRITICAL, "Python sys.path is not a list");
        }
    } else {
        log(CRITICAL, "Python could not import sys.path");
    }
    Py_DECREF(sys_module);

    PyObject * atlas = Py_InitModule("atlas", atlas_methods);
    if (atlas == NULL) {
        log(CRITICAL, "Python init failed to create atlas module\n");
        return;
    }
    if (PyType_Ready(&PyOperation_Type) < 0) {
        log(CRITICAL, "Python init failed to ready Operation wrapper type");
        return;
    }
    PyModule_AddObject(atlas, "Operation", (PyObject *)&PyOperation_Type);
    if (PyType_Ready(&PyRootEntity_Type) < 0) {
        log(CRITICAL, "Python init failed to ready RootEntity wrapper type");
        return;
    }
    PyModule_AddObject(atlas, "Entity", (PyObject *)&PyRootEntity_Type);
    if (PyType_Ready(&PyOplist_Type) < 0) {
        log(CRITICAL, "Python init failed to ready Oplist wrapper type");
        return;
    }
    PyModule_AddObject(atlas, "Oplist", (PyObject *)&PyOplist_Type);
    if (PyType_Ready(&PyLocation_Type) < 0) {
        log(CRITICAL, "Python init failed to ready Location wrapper type");
        return;
    }
    PyModule_AddObject(atlas, "Location", (PyObject *)&PyLocation_Type);
    if (PyType_Ready(&PyMessageElement_Type) < 0) {
        log(CRITICAL, "Python init failed to ready Message wrapper type");
        return;
    }
    PyModule_AddObject(atlas, "Message", (PyObject *)&PyMessageElement_Type);

    PyObject * physics = Py_InitModule("physics", physics_methods);
    if (physics == NULL) {
        log(CRITICAL, "Python init failed to create physics module\n");
        return;
    }
    if (PyType_Ready(&PyVector3D_Type) < 0) {
        log(CRITICAL, "Python init failed to ready Vector3D wrapper type");
        return;
    }
    PyModule_AddObject(physics, "Vector3D", (PyObject *)&PyVector3D_Type);
    if (PyType_Ready(&PyPoint3D_Type) < 0) {
        log(CRITICAL, "Python init failed to ready Point3D wrapper type");
        return;
    }
    PyModule_AddObject(physics, "Point3D", (PyObject *)&PyPoint3D_Type);
    if (PyType_Ready(&PyBBox_Type) < 0) {
        log(CRITICAL, "Python init failed to ready BBox wrapper type");
        return;
    }
    PyModule_AddObject(physics, "BBox", (PyObject *)&PyBBox_Type);
    if (PyType_Ready(&PyQuaternion_Type) < 0) {
        log(CRITICAL, "Python init failed to ready Quaternion wrapper type");
        return;
    }
    PyModule_AddObject(physics, "Quaternion", (PyObject *)&PyQuaternion_Type);

    PyObject * common = Py_InitModule("common", common_methods);
    if (common == NULL) {
        log(CRITICAL, "Python init failed to create common module\n");
        return;
    }

    PyObject * common_dict = PyModule_GetDict(common);

    /// Create the common.log module
    PyObject * log_mod = PyModule_New("log");
    PyDict_SetItemString(common_dict, "log", log_mod);

    PyObject * debug = (PyObject*)PyObject_NEW(FunctionObject, &log_debug_type);
    PyObject_SetAttrString(log_mod, "debug", debug);
    Py_DECREF(debug);

    PyObject * think = (PyObject*)PyObject_NEW(FunctionObject, &log_think_type);
    PyObject_SetAttrString(log_mod, "thinking", think);
    Py_DECREF(think);

    Py_DECREF(log_mod);

    PyObject * o;

    /// Create the common.const module
    PyObject * _const = PyModule_New("const");
    PyDict_SetItemString(common_dict, "const", _const);

    o = PyInt_FromLong(consts::debug_level);
    PyObject_SetAttrString(_const, "debug_level", o);
    Py_DECREF(o);

    o = PyInt_FromLong(consts::debug_thinking);
    PyObject_SetAttrString(_const, "debug_thinking", o);
    Py_DECREF(o);

    o = PyFloat_FromDouble(consts::time_multiplier);
    PyObject_SetAttrString(_const, "time_multiplier", o);
    Py_DECREF(o);

    o = PyFloat_FromDouble(consts::base_velocity_coefficient);
    PyObject_SetAttrString(_const, "base_velocity_coefficient", o);
    Py_DECREF(o);

    o = PyFloat_FromDouble(consts::base_velocity);
    PyObject_SetAttrString(_const, "base_velocity", o);
    Py_DECREF(o);

    o = PyFloat_FromDouble(consts::basic_tick);
    PyObject_SetAttrString(_const, "basic_tick", o);
    Py_DECREF(o);

    o = PyFloat_FromDouble(WFMATH_EPSILON);
    PyObject_SetAttrString(_const, "epsilon", o);
    Py_DECREF(o);

    Py_DECREF(_const);

    /// Create the common.globals module
    PyObject * globals = PyModule_New("globals");
    PyDict_SetItemString(common_dict, "globals", globals);
    o = PyString_FromString(share_directory.c_str());
    PyObject_SetAttrString(globals, "share_directory", o);
    Py_DECREF(o);
    Py_DECREF(globals);

    PyObject * server = Py_InitModule("server", no_methods);
    if (server == NULL) {
        log(CRITICAL, "Python init failed to create server module");
        return;
    }
    
    // New module code
    if (PyType_Ready(&PyMap_Type) < 0) {
        log(CRITICAL, "Python init failed to ready Map wrapper type");
        return;
    }
    PyModule_AddObject(server, "Map", (PyObject *)&PyMap_Type);
    if (PyType_Ready(&PyMind_Type) < 0) {
        log(CRITICAL, "Python init failed to ready Mind wrapper type");
        return;
    }
    PyModule_AddObject(server, "Mind", (PyObject *)&PyMind_Type);
    if (PyType_Ready(&PyTask_Type) < 0) {
        log(CRITICAL, "Python init failed to ready Task wrapper type");
        return;
    }
    PyModule_AddObject(server, "Task", (PyObject *)&PyTask_Type);
    if (PyType_Ready(&PyLocatedEntity_Type) < 0) {
        log(CRITICAL, "Python init failed to ready Entity wrapper type");
        return;
    }
    PyModule_AddObject(server, "LocatedEntity", (PyObject *)&PyLocatedEntity_Type);
    if (PyType_Ready(&PyEntity_Type) < 0) {
        log(CRITICAL, "Python init failed to ready Thing wrapper type");
        return;
    }
    PyModule_AddObject(server, "Thing", (PyObject *)&PyEntity_Type);
    if (PyType_Ready(&PyCharacter_Type) < 0) {
        log(CRITICAL, "Python init failed to ready Character wrapper type");
        return;
    }
    PyModule_AddObject(server, "Character", (PyObject *)&PyCharacter_Type);
    if (PyType_Ready(&PyWorld_Type) < 0) {
        log(CRITICAL, "Python init failed to ready World wrapper type");
        return;
    }
    // PyWorldTime_Type.tp_new = PyType_GenericNew;
    if (PyType_Ready(&PyWorldTime_Type) < 0) {
        log(CRITICAL, "Python init failed to ready WorldTime wrapper type");
        return;
    }
    PyModule_AddObject(server, "WorldTime", (PyObject *)&PyWorldTime_Type);

    PyWorld * world = newPyWorld();
    PyModule_AddObject(server, "world", (PyObject *)world);

    // FIXME Remove once we are sure.
    // PyObject * rules = Py_InitModule("rulesets", no_methods);
    // if (rules == NULL) {
        // log(CRITICAL, "Python init failed to create rules module");
        // // return;
    // }
    // if (PyType_Ready(&PyStatistics_Type) < 0) {
        // log(CRITICAL, "Python init failed to ready Statistics wrapper type");
        // return;
    // }
    // PyModule_AddObject(rules, "Statistics", (PyObject *)&PyStatistics_Type);

    if (PyType_Ready(&PyTerrainProperty_Type) < 0) {
        log(CRITICAL, "Python init failed to ready TerrainProperty wrapper type");
        return;
    }


    debug(std::cout << Py_GetPath() << std::endl << std::flush;);
}

void shutdown_python_api()
{
    
    Py_Finalize();
}
