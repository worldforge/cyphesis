#ifndef PY_THING_H
#define PY_THING_H

extern PyMethodDef Thing_methods[];

extern PyTypeObject Thing_Type;

ThingObject * newThingObject(PyObject *);

#endif /* PY_THING_H */
