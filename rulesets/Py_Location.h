#ifndef PY_LOCATION_H
#define PY_LOCATION_H

// extern PyMethodDef Thing_methods[];

extern PyTypeObject Location_Type;

LocationObject * newLocationObject(PyObject *);

#endif /* PY_LOCATION_H */
