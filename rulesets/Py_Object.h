#include <stdio.h>
#include <unistd.h>

#include <Python.h>

#include "Python_API.h"

/*
 * Object methods structure.
 */

// extern PyMethodDef Object_methods[];

/*
 * Object type structure.
 */

extern PyTypeObject Object_Type;

/*
 * Object creation function.
 */

AtlasObject * newAtlasObject(PyObject *arg);

/*
 * Utility functions to munge between Object related types and python types
 */

PyObject * MapType_asPyObject(Object::MapType & map);
PyObject * ListType_asPyObject(Object::ListType & list);
PyObject * Object_asPyObject(Object & obj);
Object::ListType PyListObject_asListType(PyObject * list);
Object::MapType PyDictObject_asMapType(PyObject * dict);
Object PyObject_asObject(PyObject * o);

