#ifndef PY_OPLIST_H
#define PY_OPLIST_H

extern PyTypeObject Oplist_Type;

#define PyOplist_Check(_o) ((PyTypeObject*)PyObject_Type((PyObject*)_o)==&Oplist_Type)

OplistObject * newOplistObject(PyObject *);

#endif /* PY_VECTOR3D_H */
