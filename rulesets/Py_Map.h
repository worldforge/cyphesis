#ifndef PY_MAP_H
#define PY_MAP_H

// extern PyMethodDef Thing_methods[];

extern PyTypeObject Map_Type;

MapObject * newMapObject(PyObject *);

#endif /* PY_MAP_H */
