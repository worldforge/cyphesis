// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef RULESETS_PY_WORLD_H
#define RULESETS_PY_WORLD_H

extern PyTypeObject World_Type;

#define PyWorld_Check(_o) ((PyTypeObject*)PyObject_Type((PyObject*)_o)==&World_Type)

WorldObject * newWorldObject(PyObject *);

#endif // RULESETS_PY_WORLD_H
