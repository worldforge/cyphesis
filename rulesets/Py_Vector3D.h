#ifndef PY_VECTOR3D_H
#define PY_VECTOR3D_H

// extern PyMethodDef Thing_methods[];

extern PyTypeObject Vector3D_Type;

Vector3DObject * newVector3DObject(PyObject *);

#endif /* PY_VECTOR3D_H */
