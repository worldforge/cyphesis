#ifndef PY_WORLDTIME_H
#define PY_WORLDTIME_H

extern PyTypeObject WorldTime_Type;

#define PyWorldTime_Check(_o) ((PyTypeObject*)PyObject_Type((PyObject*)_o)==&WorldTime_Type)

WorldTimeObject * newWorldTimeObject(PyObject *);

#endif /* PY_WORLDTIME_H */
