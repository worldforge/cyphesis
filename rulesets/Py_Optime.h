#ifndef PY_OPTIME_H
#define PY_OPTIME_H

extern PyTypeObject Optime_Type;

#define PyOptime_Check(_o) ((PyTypeObject*)PyObject_Type((PyObject*)_o)==&Optime_Type)

OptimeObject * newOptimeObject(PyObject *);

#endif /* PY_OPTIME_H */
