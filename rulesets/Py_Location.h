// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef RULESETS_PY_LOCATION_H
#define RULESETS_PY_LOCATION_H

#include <Python.h>

class Location;
class Entity;

/// \brief Wrapper for Location in Python
typedef struct {
    PyObject_HEAD
    Location * location;
    Entity * owner;
} PyLocation;

extern PyTypeObject PyLocation_Type;

#define PyLocation_Check(_o) ((_o)->ob_type == &PyLocation_Type)

PyLocation * newPyLocation();

#endif // RULESETS_PY_LOCATION_H
