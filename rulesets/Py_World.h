#ifndef PY_WORLD_H
#define PY_WORLD_H

extern PyTypeObject World_Type;

#define PyWorld_Check(_o) ((PyTypeObject*)PyObject_Type((PyObject*)_o)==&World_Type)

WorldObject * newWorldObject(PyObject *);

#endif /* PY_WORLD_H */
