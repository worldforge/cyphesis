#ifndef PY_LOCATION_H
#define PY_LOCATION_H

extern PyTypeObject Location_Type;

#define PyLocation_Check(_o) ((PyTypeObject*)PyObject_Type((PyObject*)_o)==&Location_Type)

LocationObject * newLocationObject(PyObject *);

#endif /* PY_LOCATION_H */
