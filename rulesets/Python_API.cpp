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

// $Id: Python_API.cpp,v 1.164 2007-06-19 13:19:58 alriddoch Exp $

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
#include "Py_Statistics.h"

#include "PythonThingScript.h"
#include "PythonMindScript.h"
#include "World.h"
#include "Entity.h"
#include "BaseMind.h"

#include "common/inheritance.h"
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
        log(lvl, message.substr(n, p - n).c_str());
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
        std::string msg = std::string("Missing python module ") + package;
        log(ERROR, msg.c_str());
        PyErr_Print();
        return NULL;
    }
    PyObject * pyClass = PyObject_GetAttrString(module, (char *)classname.c_str());
    Py_DECREF(module);
    if (pyClass == NULL) {
        std::string msg = std::string("Could not find python class ")
                        + package + "." + classname;
        log(ERROR, msg.c_str());
        PyErr_Print();
        return NULL;
    }
    if (PyCallable_Check(pyClass) == 0) {
        std::string msg = std::string("Could not instance python class ")
                        + package + "." + classname;
        log(ERROR, msg.c_str());
        Py_DECREF(pyClass);
        return NULL;
    }
#if 0
    // In later versions of python using PyType_* will become the right thing
    // to do. This might become true when things have been done right with
    // installing types.
    if (PyType_Check(pyClass) == 0) {
        std::cerr << "PyCallable_Check returned true, but PyType_Check returned false " << package << "." << type << std::endl << std::flush;
    } else {
        std::cerr << "PyType_Check returned true" << std::endl << std::flush;
    }
#endif
    return pyClass;
}

PyObject * Create_PyScript(PyObject * wrapper, PyObject * pyClass)
{
    PyObject * pyob = PyEval_CallFunction(pyClass,"(O)", wrapper);
    
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

void Subscribe_Script(Entity * entity, PyObject * pyclass,
                      const std::string& package)
{
#if 0
    PyObject * dmap = PyObject_GetAttrString(pyclass, "__dict__");
    if (dmap == NULL) {
        std::string msg = std::string( "Python class for ") + package
                        + " has no __dict__";
        log(ERROR, msg.c_str());
        return;
    }
    if (!PyDict_Check(dmap)) {
        std::string msg = std::string( "Python class for ") + package
                        + " is malformed";
        log(ERROR, msg.c_str());
        return;
    }
    PyObject * keys = PyDict_Keys(dmap);
#else
    PyObject * keys = PyObject_Dir(pyclass);
#endif
    if (keys == NULL) {
        std::string msg = std::string("Error getting attribute list of Python class for ") + package;
        log(ERROR, msg.c_str());
        return;
    }
    for(int i = 0; i < PyList_Size(keys); i++) {
        std::string method(PyString_AsString(PyList_GetItem(keys, i)));
        std::string::size_type l = method.find("_operation", 0, 10);
        if (l == std::string::npos) {
            debug(std::cout << method << " is not a method" << std::endl
                            << std::flush;);
        } else {
            std::string op_name = method.substr(0,l);
            debug(std::cout << method << " is a method, it contains _op.. at "
                            << l << " so we can register for "
                            << method.substr(0,l) << std::endl << std::flush;);
            entity->scriptSubscribe(op_name);
        }
    }
    Py_DECREF(keys);
}

void Create_PyMind(BaseMind * mind, const std::string & package,
                                    const std::string & type)
{
    PyObject * pyClass = Get_PyClass(package, type);
    if (pyClass == NULL) { return; }
    PyMind * wrapper = newPyMind();
    wrapper->m_mind = mind;
    Subscribe_Script(mind, pyClass, package);
    PyObject * o = Create_PyScript((PyObject *)wrapper, pyClass);
    Py_DECREF(pyClass);

    if (o != NULL) {
        mind->setScript(new PythonMindScript(o, (PyObject *)wrapper, *mind));
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

static PyObject * location_new(PyObject * self, PyObject * args)
{
    PyLocation *o;
    // We need to deal with actual args here
    PyObject * refO = NULL, * coordsO = NULL;
    Entity * ref_ent = NULL;
    bool decrefO = false;
    if (!PyArg_ParseTuple(args, "|OO", &refO, &coordsO)) {
        return NULL;
    }
    if (refO != NULL) {
        if ((!PyEntity_Check(refO)) && (!PyWorld_Check(refO)) && (!PyMind_Check(refO))) {
            if (PyObject_HasAttrString(refO, "cppthing")) {
                refO = PyObject_GetAttrString(refO, "cppthing");
                decrefO = true;
            }
            if (!PyEntity_Check(refO) && !PyMind_Check(refO)) {
                PyErr_SetString(PyExc_TypeError, "Arg ref required");
                if (decrefO) { Py_DECREF(refO); }
                return NULL;
            }
        }
        if ((coordsO != NULL) && (!PyPoint3D_Check(coordsO))) {
            PyErr_SetString(PyExc_TypeError, "Arg coords required");
            if (decrefO) { Py_DECREF(refO); }
            return NULL;
        }
    
        if (PyWorld_Check(refO)) {
            PyWorld * ref = (PyWorld*)refO;
#ifndef NDEBUG
            if (ref->world == NULL) {
                PyErr_SetString(PyExc_AssertionError, "Parent world is invalid");
                if (decrefO) { Py_DECREF(refO); }
                return NULL;
            }
#endif // NDEBUG
            ref_ent = &ref->world->m_gameWorld;
        } else if (PyMind_Check(refO)) {
            PyMind * ref = (PyMind*)refO;
#ifndef NDEBUG
            if (ref->m_mind == NULL) {
                PyErr_SetString(PyExc_AssertionError, "Parent mind is invalid");
                if (decrefO) { Py_DECREF(refO); }
                return NULL;
            }
#endif // NDEBUG
            ref_ent = ref->m_mind;
        } else {
            PyEntity * ref = (PyEntity*)refO;
#ifndef NDEBUG
            if (ref->m_entity == NULL) {
                PyErr_SetString(PyExc_AssertionError, "Parent thing is invalid");
                if (decrefO) { Py_DECREF(refO); }
                return NULL;
            }
#endif // NDEBUG
            ref_ent = ref->m_entity;
        }
    }
    if (decrefO) { Py_DECREF(refO); }
    PyPoint3D * coords = (PyPoint3D*)coordsO;
    o = newPyLocation();
    if ( o == NULL ) {
        return NULL;
    }
    if (coords == NULL) {
        o->location = new Location(ref_ent);
    } else {
        o->location = new Location(ref_ent, coords->coords);
    }
    return (PyObject *)o;
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

static PyObject * bbox_new(PyObject * self, PyObject * args)
{
    std::vector<float> val;

    PyObject * clist;
    int tuple_size = PyTuple_Size(args);
    int clist_size;
    switch(tuple_size) {
        case 0:
            break;
        case 1:
            clist = PyTuple_GetItem(args, 0);
            clist_size = PyList_Size(clist);
            if (!PyList_Check(clist) || (clist_size != 3 && clist_size != 6)) {
                PyErr_SetString(PyExc_TypeError, "BBox() from single value must a list 3 or 6 long");
                return NULL;
            }
            
            val.resize(clist_size);
            for(int i = 0; i < clist_size; i++) {
                PyObject * item = PyList_GetItem(clist, i);
                if (PyInt_Check(item)) {
                    val[i] = (float)PyInt_AsLong(item);
                } else if (PyFloat_Check(item)) {
                    val[i] = PyFloat_AsDouble(item);
                } else if (PyMessageElement_Check(item)) {
                    PyMessageElement * mitem = (PyMessageElement*)item;
                    if (!mitem->m_obj->isNum()) {
                        PyErr_SetString(PyExc_TypeError, "BBox() must take list of floats, or ints");
                        return NULL;
                    }
                    val[i] = mitem->m_obj->asNum();
                } else {
                    PyErr_SetString(PyExc_TypeError, "BBox() must take list of floats, or ints");
                    return NULL;
                }
            }
            break;
        case 3:
        case 6:
            val.resize(tuple_size);
            for(int i = 0; i < tuple_size; i++) {
                PyObject * item = PyTuple_GetItem(args, i);
                if (PyInt_Check(item)) {
                    val[i] = (float)PyInt_AsLong(item);
                } else if (PyFloat_Check(item)) {
                    val[i] = PyFloat_AsDouble(item);
                } else {
                    PyErr_SetString(PyExc_TypeError, "BBox() must take list of floats, or ints");
                    return NULL;
                }
            }
            break;
        default:
            PyErr_SetString(PyExc_TypeError, "Point3D must take list of floats, or ints, 3 ints or 3 floats");
            return NULL;
            break;
    }
        
    PyBBox * o = newPyBBox();
    if ( o == NULL ) {
            return NULL;
    }
    if (val.size() == 3) {
        o->box = BBox(WFMath::Point<3>(0.f, 0.f, 0.f),
                      WFMath::Point<3>(val[0], val[1], val[2]));
    } else {
        o->box = BBox(WFMath::Point<3>(val[0], val[1], val[2]),
                      WFMath::Point<3>(val[3], val[4], val[5]));
    }
    return (PyObject *)o;
}

static PyObject * quaternion_new(PyObject * self, PyObject * args)
{
        PyQuaternion *o;
        Quaternion val;

        PyObject * clist;
        switch (PyTuple_Size(args)) {
            case 0:
                break;
            case 1:
                clist = PyTuple_GetItem(args, 0);
                if ((!PyList_Check(clist)) || (PyList_Size(clist) != 4)) {
                    PyErr_SetString(PyExc_TypeError, "Quaternion() from single value must a list 4 long");
                    return NULL;
                }
                {
                float quaternion[4];
                for(int i = 0; i < 4; i++) {
                    PyObject * item = PyList_GetItem(clist, i);
                    if (PyInt_Check(item)) {
                        quaternion[i] = (WFMath::CoordType)PyInt_AsLong(item);
                    } else if (PyFloat_Check(item)) {
                        quaternion[i] = PyFloat_AsDouble(item);
                    } else {
                        PyErr_SetString(PyExc_TypeError, "Quaternion() must take list of floats, or ints");
                        return NULL;
                    }
                }
                val = Quaternion(quaternion[3], quaternion[0],
                                 quaternion[1], quaternion[2]);
                }
                break;
            case 2:
                {
                PyObject * v1 = PyTuple_GetItem(args, 0);
                PyObject * v2 = PyTuple_GetItem(args, 1);
                if (!PyVector3D_Check(v1)) {
                    PyErr_SetString(PyExc_TypeError, "Quaternion(a,b) must take a vector");
                    return NULL;
                }
                PyVector3D * arg1 = (PyVector3D *)v1;
                if (PyVector3D_Check(v2)) {
                    PyVector3D * to = (PyVector3D *)v2;
                    val = quaternionFromTo(arg1->coords, to->coords);
                } else if (PyFloat_Check(v2)) {
                    float angle = PyFloat_AsDouble(v2);
                    val.rotation(arg1->coords, angle);
                } else {
                    PyErr_SetString(PyExc_TypeError, "Quaternion(a,b) must take a vector");
                    return NULL;
                }
                }
                break;
            case 4:
                {
                float quaternion[4];
                for(int i = 0; i < 4; i++) {
                    PyObject * item = PyTuple_GetItem(args, i);
                    if (PyInt_Check(item)) {
                        quaternion[i] = (WFMath::CoordType)PyInt_AsLong(item);
                    } else if (PyFloat_Check(item)) {
                        quaternion[i] = PyFloat_AsDouble(item);
                    } else {
                        PyErr_SetString(PyExc_TypeError, "Quaternion() must take list of floats, or ints");
                        return NULL;
                    }
                }
                val = Quaternion(quaternion[3], quaternion[0],
                                 quaternion[1], quaternion[2]);
                }
                break;
            default:
                PyErr_SetString(PyExc_TypeError, "Quaternion must take list of floats, or ints, 4 ints or 4 floats");
                return NULL;
                break;
        }

        o = newPyQuaternion();
        if ( o == NULL ) {
                return NULL;
        }
        o->rotation = val;
        return (PyObject *)o;
}

static inline void addToOplist(PyOperation * op, PyOplist * o)
{
    if (op != NULL) {
       if (PyOperation_Check(op)) {
           o->ops->push_back(op->operation);
       } else if ((PyObject*)op != Py_None) {
           PyErr_SetString(PyExc_TypeError, "Argument must be an op");
           return;
       }
    }
}


static PyObject * oplist_new(PyObject * self, PyObject * args)
{
        PyOplist *o;
        
        PyOperation *op1 = NULL, *op2 = NULL, *op3 = NULL, *op4 = NULL;
        if (!PyArg_ParseTuple(args, "|OOOO", &op1, &op2, &op3, &op4)) {
                return NULL;
        }
        o = newPyOplist();
        if ( o == NULL ) {
                return NULL;
        }
        o->ops = new OpVector();
        addToOplist(op1, o);
        addToOplist(op2, o);
        addToOplist(op3, o);
        addToOplist(op4, o);
        return (PyObject *)o;
}

static int PySequence_asVector(PyObject * o, std::vector<double> & ret)
{
    int len;
    PyObject * item;
    if (PyList_Check(o)) {
        len = PyList_Size(o);
        ret.resize(len);
        for(int i = 0; i < len; i++) {
            item = PyList_GetItem(o, i);
            if (PyFloat_Check(item)) {
                ret[i] = PyFloat_AsDouble(item);
            } else if (PyInt_Check(item)) {
                ret[i] = PyInt_AsLong(item);
            } else {
                return -1;
            }
        }
    } else if (PyTuple_Check(o)) {
        len = PyTuple_Size(o);
        ret.resize(len);
        for(int i = 0; i < len; i++) {
            item = PyTuple_GetItem(o, i);
            if (PyFloat_Check(item)) {
                ret[i] = PyFloat_AsDouble(item);
            } else if (PyInt_Check(item)) {
                ret[i] = PyInt_AsLong(item);
            } else {
                return -1;
            }
        }
    } else {
        return -1;
    }
    return 0;
}

static PyObject * entity_new(PyObject * self, PyObject * args, PyObject * kwds)
{
    char * id = NULL;
    
    if (!PyArg_ParseTuple(args, "|s", &id)) {
        return NULL;
    }
    Anonymous ent;
    if (id != NULL) {
        ent->setId(id);
    }
    if ((kwds != NULL) && (PyDict_Check(kwds))) {
        PyObject * keys = PyDict_Keys(kwds);
        PyObject * vals = PyDict_Values(kwds);
        if ((keys == NULL) || (vals == NULL)) {
            PyErr_SetString(PyExc_RuntimeError, "Error in keywords");
            return NULL;
        }
        int i, size = PyList_Size(keys); 
        for(i = 0; i < size; i++) {
            char * key = PyString_AsString(PyList_GetItem(keys, i));
            PyObject * val = PyList_GetItem(vals, i);
            if (strcmp(key, "location") == 0) {
                if (!PyLocation_Check(val)) {
                    PyErr_SetString(PyExc_TypeError, "location must be a Location object");
                    return NULL;
                }
                PyLocation * loc = (PyLocation*)val;
                loc->location->addToEntity(ent);
            } else if (strcmp(key, "pos") == 0) {
                std::vector<double> vector_val;
                if (PySequence_asVector(val, vector_val) != 0) {
                    PyErr_SetString(PyExc_TypeError, "pos must be a number sequence.");
                    return NULL;
                }
                ent->setPos(vector_val);
            } else if (strcmp(key, "parent") == 0) {
                if (!PyString_Check(val)) {
                    PyErr_SetString(PyExc_TypeError, "parent must be a string.");
                    return NULL;
                }
                ent->setLoc(PyString_AsString(val));
            } else if (strcmp(key, "type") == 0) {
                if (!PyString_Check(val)) {
                    PyErr_SetString(PyExc_TypeError, "type must be a string.");
                    return NULL;
                }
                ent->setParents(std::list<std::string>(1, PyString_AsString(val)));
                ent->setObjtype("obj");
            } else {
                Element val_obj = PyObject_asMessageElement(val);
                if (val_obj.getType() == Element::TYPE_NONE) {
                    Py_DECREF(keys);
                    Py_DECREF(vals);
                    PyErr_SetString(PyExc_TypeError, "Arg has no type.");
                    return NULL;
                }
                ent->setAttr(key, val_obj);
            }
        }
        Py_DECREF(keys);
        Py_DECREF(vals);
    }

    PyRootEntity * o = newPyRootEntity();
    if ( o == NULL ) {
        return NULL;
    }
    o->entity = ent;
    return (PyObject *)o;
}

static int addToArgs(std::vector<Root> & args, PyObject * arg)
{
    if (PyMessageElement_Check(arg)) {
        PyMessageElement * obj = (PyMessageElement*)arg;
#ifndef NDEBUG
        if (obj->m_obj == NULL) {
            PyErr_SetString(PyExc_AssertionError,"NULL MessageElement in Operation constructor argument");
            return -1;
        }
#endif // NDEBUG
        const Element & o = *obj->m_obj;
        if (o.isMap()) {
            args.push_back(Atlas::Objects::Factories::instance()->createObject(o.asMap()));
        } else {
            PyErr_SetString(PyExc_TypeError, "Operation arg is not a map");
            return -1;
        }
    } else if (PyOperation_Check(arg)) {
        PyOperation * op = (PyOperation*)arg;
#ifndef NDEBUG
        if (!op->operation.isValid()) {
            PyErr_SetString(PyExc_AssertionError,"Invalid operation in Operation constructor argument");
            return -1;
        }
#endif // NDEBUG
        args.push_back(op->operation);
    } else if (PyRootEntity_Check(arg)) {
        PyRootEntity * ent = (PyRootEntity*)arg;
#ifndef NDEBUG
        if (!ent->entity.isValid()) {
            PyErr_SetString(PyExc_AssertionError,"Invalid rootentity in Operation constructor argument");
            return -1;
        }
#endif // NDEBUG
        args.push_back(ent->entity);
    } else {
        PyErr_SetString(PyExc_TypeError, "Operation arg is of unknown type");
        return -1;
    }
    return 0;
}

static PyObject * operation_new(PyObject * self, PyObject * args, PyObject * kwds)
{
    PyOperation * op;

    char * type;
    PyObject * arg1 = NULL;
    PyObject * arg2 = NULL;
    PyObject * arg3 = NULL;

    if (!PyArg_ParseTuple(args, "s|OOO", &type, &arg1, &arg2, &arg3)) {
        return NULL;
    }
    op = newPyOperation();
    if (op == NULL) {
        return NULL;
    }
    if (strcmp(type, "thought") == 0 || strcmp(type, "goal_info") == 0) {
        Py_DECREF(op);
        Py_INCREF(Py_None);
        return Py_None;
    } else {
        Root r = Atlas::Objects::Factories::instance()->createObject(type);
        op->operation = Atlas::Objects::smart_dynamic_cast<RootOperation>(r);
        if (!op->operation.isValid()) {
            Py_DECREF(op);
            PyErr_SetString(PyExc_TypeError, "Operation() unknown operation type requested");
            return NULL;
        }
    }
    if (kwds != NULL) {
        PyObject * from = PyDict_GetItemString(kwds, "from_");
        if (from != NULL) {
            PyObject * from_id = 0;
            if (PyString_Check(from)) {
                from_id = from;
                Py_INCREF(from_id);
            } else if ((from_id = PyObject_GetAttrString(from, "id")) == NULL) {
                PyErr_SetString(PyExc_TypeError, "from is not a string and has no id");
                return NULL;
            }
            if (!PyString_Check(from_id)) {
                Py_DECREF(from_id);
                PyErr_SetString(PyExc_TypeError, "id of from is not a string");
                return NULL;
            }
            op->operation->setFrom(PyString_AsString(from_id));
            Py_DECREF(from_id);
        }
        PyObject * to = PyDict_GetItemString(kwds, "to");
        if (to != NULL) {
            PyObject * to_id = 0;
            if (PyString_Check(to)) {
                to_id = to;
                Py_INCREF(to_id);
            } else if ((to_id = PyObject_GetAttrString(to, "id")) == NULL) {
                PyErr_SetString(PyExc_TypeError, "to is not a string and has no id");
                return NULL;
            }
            if (!PyString_Check(to_id)) {
                Py_DECREF(to_id);
                PyErr_SetString(PyExc_TypeError, "id of to is not a string");
                return NULL;
            }
            op->operation->setTo(PyString_AsString(to_id));
            Py_DECREF(to_id);
        }
    }
    std::vector<Root> & args_list = op->operation->modifyArgs();
    assert(args_list.empty());
    if (arg1 != 0 && addToArgs(args_list, arg1) != 0) {
        Py_DECREF(op);
        op = NULL;
    }
    if (arg2 != 0 && addToArgs(args_list, arg2) != 0) {
        Py_DECREF(op);
        op = NULL;
    }
    if (arg3 != 0 && addToArgs(args_list, arg3) != 0) {
        Py_DECREF(op);
        op = NULL;
    }
    return (PyObject *)op;
}

// In Python 2.3 or later this it is okay to pass in null for the methods
// of a module, making this obsolete.
static PyMethodDef no_methods[] = {
    {NULL,          NULL}                       /* Sentinel */
};

static PyMethodDef atlas_methods[] = {
    {"Operation",  (PyCFunction)operation_new,  METH_VARARGS|METH_KEYWORDS},
    {"isLocation", is_location,                 METH_O},
    {"Location",   location_new,                METH_VARARGS},
    {"Entity",     (PyCFunction)entity_new,     METH_VARARGS|METH_KEYWORDS},
    {"Message",    oplist_new,                  METH_VARARGS},
    {NULL,          NULL}                       /* Sentinel */
};

static PyMethodDef physics_methods[] = {
    {"distance_to",distance_to,                 METH_VARARGS},
    {"square_distance",square_distance,         METH_VARARGS},
    {"square_horizontal_distance",
      square_horizontal_distance,               METH_VARARGS},
    {NULL,          NULL}                       /* Sentinel */
};

static PyMethodDef bbox_methods[] = {
    {"BBox",        bbox_new,                 METH_VARARGS},
    {NULL,          NULL}                       /* Sentinel */
};

static PyMethodDef quaternion_methods[] = {
    {"Quaternion",  quaternion_new,             METH_VARARGS},
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

    PyObject * outLogger = (PyObject*)PyObject_NEW(PyLogger, &PyOutLogger_Type);
    PyObject_SetAttrString(sys_module, "stdout", outLogger);
    Py_DECREF(outLogger);

    PyObject * errLogger = (PyObject*)PyObject_NEW(PyLogger, &PyErrLogger_Type);
    PyObject_SetAttrString(sys_module, "stderr", errLogger);
    Py_DECREF(errLogger);

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

    if (Py_InitModule("atlas", atlas_methods) == NULL) {
        log(CRITICAL, "Python init failed to create atlas module\n");
        return;
    }

    if (Py_InitModule("physics", physics_methods) == NULL) {
        log(CRITICAL, "Python init failed to create physics module\n");
        return;
    }

    if (Py_InitModule("BBox", bbox_methods) == NULL) {
        log(CRITICAL, "Python init failed to create BBox module\n");
        return;
    }

    if (Py_InitModule("Quaternion", quaternion_methods) == NULL) {
        log(CRITICAL, "Python init failed to create Quaternion module\n");
        return;
    }

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

    o = PyInt_FromLong(0);
    PyObject_SetAttrString(_const, "server_python", o);
    Py_DECREF(o);

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
    // PyWorldTime_Type.tp_new = PyType_GenericNew;
    if (PyType_Ready(&PyWorldTime_Type) < 0) {
        log(CRITICAL, "Python init failed to ready WorldTime wrapper type");
        return;
    }
    PyModule_AddObject(server, "WorldTime", (PyObject *)&PyWorldTime_Type);

    PyObject * rules = Py_InitModule("rulesets", no_methods);
    if (rules == NULL) {
        log(CRITICAL, "Python init failed to create rules module");
        return;
    }
    if (PyType_Ready(&PyStatistics_Type) < 0) {
        log(CRITICAL, "Python init failed to ready Statistics wrapper type");
        return;
    }
    PyModule_AddObject(rules, "Statistics", (PyObject *)&PyStatistics_Type);

    PyObject * point3d = Py_InitModule("Point3D", no_methods);
    if (point3d == NULL) {
        log(CRITICAL, "Python init failed to create Point3D module\n");
        return;
    }
    // PyPoint3D_Type.tp_new = PyType_GenericNew;
    if (PyType_Ready(&PyPoint3D_Type) < 0) {
        log(CRITICAL, "Python init failed to ready Point3D wrapper type");
        return;
    }
    PyModule_AddObject(point3d, "Point3D", (PyObject *)&PyPoint3D_Type);

    PyObject * vector3d = Py_InitModule("Vector3D", no_methods);
    if (vector3d == NULL) {
        log(CRITICAL, "Python init failed to create Vector3D module\n");
        return;
    }
    // PyVector3D_Type.tp_new = PyType_GenericNew;
    if (PyType_Ready(&PyVector3D_Type) < 0) {
        log(CRITICAL, "Python init failed to ready Vector3D wrapper type");
        return;
    }
    PyModule_AddObject(vector3d, "Vector3D", (PyObject *)&PyVector3D_Type);

    // PyRun_SimpleString("from hooks import ruleset_import_hooks\n");
    // PyRun_SimpleString((char *)importCmd.c_str());

    debug(std::cout << Py_GetPath() << std::endl << std::flush;);
}

void shutdown_python_api()
{
    
    Py_Finalize();
}
