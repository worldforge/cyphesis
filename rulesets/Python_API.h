// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef RULESETS_PYTHON_API_H
#define RULESETS_PYTHON_API_H

class Entity;
class Thing;
class BaseMind;
class MemMap;
class Location;
class WorldTime;
class BaseWorld;

#include <common/types.h>
#include <common/operations.h>

#include <physics/Vector3D.h>

#include <Python.h>

using Atlas::Message::Object;

typedef struct {
    PyObject_HEAD
    PyObject	* Object_attr;	// Attributes dictionary
    Object	* m_obj;
} AtlasObject;

typedef struct {
    PyObject_HEAD
    PyObject	* Thing_attr;	// Attributes dictionary
    Entity	* m_thing;
} ThingObject;

typedef struct {
    PyObject_HEAD
    PyObject	* Mind_attr;	// Attributes dictionary
    BaseMind	* m_mind;
} MindObject;

typedef struct {
    PyObject_HEAD
    MemMap	* m_map;
} MapObject;

typedef struct {
    PyObject_HEAD
    Location	* location;
    int		own;
} LocationObject;

typedef struct {
    PyObject_HEAD
    Vector3D	coords;
} Vector3DObject;

typedef struct {
    PyObject_HEAD
    WorldTime	* time;
    bool	own;
} WorldTimeObject;

typedef struct {
    PyObject_HEAD
    BaseWorld	* world;
} WorldObject;

typedef struct {
    PyObject_HEAD
    RootOperation	* operation;
    int			own;
    Entity		* from;
    Entity		* to;
} RootOperationObject;

typedef struct {
    PyObject_HEAD
    oplist	* ops;
} OplistObject;

typedef struct {
    PyObject_HEAD
} FunctionObject;

typedef struct {
    PyObject_HEAD
    RootOperation	* operation;
} OptimeObject;

#include "Py_Object.h"
#include "Py_Thing.h"
#include "Py_Mind.h"
#include "Py_Map.h"
#include "Py_Location.h"
#include "Py_Vector3D.h"
#include "Py_WorldTime.h"
#include "Py_World.h"
#include "Py_Operation.h"
#include "Py_Oplist.h"
#include "Py_Optime.h"

void Create_PyThing(Thing * thing, const std::string & package,
                                   const std::string & type);
void Create_PyMind(BaseMind * mind, const std::string & package,
                                    const std::string & type);

#endif // RULESETS_PYTHON_API_H
