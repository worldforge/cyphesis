#ifndef PY_OPTIME_H
#define PY_OPTIME_H

// extern PyMethodDef Thing_methods[];

extern PyTypeObject Optime_Type;

OptimeObject * newOptimeObject(PyObject *);

#endif /* PY_OPTIME_H */
