#ifndef PY_OPLIST_H
#define PY_OPLIST_H

// extern PyMethodDef Thing_methods[];

extern PyTypeObject Oplist_Type;

OplistObject * newOplistObject(PyObject *);

#endif /* PY_VECTOR3D_H */
