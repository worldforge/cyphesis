// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef PY_MAP_H
#define PY_MAP_H

extern PyTypeObject Map_Type;

#define PyMap_Check(_o) ((PyTypeObject*)PyObject_Type((PyObject*)_o)==&Map_Type)

MapObject * newMapObject(PyObject *);

#endif // PY_MAP_H
