#ifndef PY_WORLD_H
#define PY_WORLD_H

// extern PyMethodDef Thing_methods[];

extern PyTypeObject World_Type;

WorldObject * newWorldObject(PyObject *);

#endif /* PY_WORLD_H */
