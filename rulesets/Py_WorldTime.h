// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef RULESETS_PY_WORLDTIME_H
#define RULESETS_PY_WORLDTIME_H

#include <Python.h>

class WorldTime;

typedef struct {
    PyObject_HEAD
    WorldTime * time;
    bool own;
} PyWorldTime;

extern PyTypeObject PyWorldTime_Type;

#define PyWorldTime_Check(_o) ((PyTypeObject*)PyObject_Type((PyObject*)_o)==&PyWorldTime_Type)

PyWorldTime * newPyWorldTime();

#endif // RULESETS_PY_WORLDTIME_H
