// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef PY_OPTIME_H
#define PY_OPTIME_H

extern PyTypeObject Optime_Type;

#define PyOptime_Check(_o) ((PyTypeObject*)PyObject_Type((PyObject*)_o)==&Optime_Type)

OptimeObject * newOptimeObject(PyObject *);

#endif /* PY_OPTIME_H */
