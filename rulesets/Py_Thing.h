#ifndef PY_THING_H
#define PY_THING_H

extern PyTypeObject Thing_Type;

#define PyThing_Check(_o) ((PyTypeObject*)PyObject_Type((PyObject*)_o)==&Thing_Type)

ThingObject * newThingObject(PyObject *);

#endif /* PY_THING_H */
