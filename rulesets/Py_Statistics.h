// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef RULESETS_PY_STATISTICS_H
#define RULESETS_PY_STATISTICS_H

#include "rulesets/Py_EntityWrapper.h"

typedef PyEntityWrapper PyStatistics;

extern PyTypeObject PyStatistics_Type;

#define PyStatistics_Check(_o) ((PyTypeObject(PyObject*)_o)==&PyStatistics_Type)

PyStatistics * newPyStatistics();

#endif // RULESETS_PY_STATISTICS_H
