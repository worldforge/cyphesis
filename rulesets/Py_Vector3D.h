#ifndef PY_VECTOR3D_H
#define PY_VECTOR3D_H

extern PyTypeObject Vector3D_Type;

#define PyVector3D_Check(_o) ((PyTypeObject*)PyObject_Type((PyObject*)_o)==&Vector3D_Type)

Vector3DObject * newVector3DObject(PyObject *);

#endif /* PY_VECTOR3D_H */
