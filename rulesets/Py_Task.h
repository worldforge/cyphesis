// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef RULESETS_PY_TASK_H
#define RULESETS_PY_TASK_H

#include <Python.h>

class Task;

/// \brief Wrapper for Task in Python
typedef struct {
    PyObject_HEAD
    PyObject * Task_attr;
    Task * m_task;
} PyTask;

extern PyTypeObject PyTask_Type;

#define PyTask_Check(_o) ((_o)->ob_type == &PyTask_Type)

PyTask * newPyTask();

#endif // RULESETS_PY_TASK_H
