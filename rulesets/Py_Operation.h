// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef PY_OPERATION_H
#define PY_OPERATION_H

extern PyTypeObject RootOperation_Type;

#define PyOperation_Check(_o) ((PyTypeObject*)PyObject_Type((PyObject*)_o)==&RootOperation_Type)

RootOperationObject * newAtlasRootOperation(PyObject *arg);

#endif /* PY_OPERATION_H */
