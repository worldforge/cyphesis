#ifndef PY_OPERATION_H
#define PY_OPERATION_H

extern PyTypeObject RootOperation_Type;

#define PyOperation_Check(_o) ((PyTypeObject*)PyObject_Type((PyObject*)_o)==&RootOperation_Type)

RootOperationObject * newAtlasRootOperation(PyObject *arg);

#endif /* PY_OPERATION_H */
