#ifndef PY_WORLDTIME_H
#define PY_WORLDTIME_H

// extern PyMethodDef Thing_methods[];

extern PyTypeObject WorldTime_Type;

WorldTimeObject * newWorldTimeObject(PyObject *);

#endif /* PY_WORLDTIME_H */
