#ifndef PY_OPERATION_H
#define PY_OPERATION_H

/*
 * This is a python type that wraps up operation objects from
 * Atlas::Objects::Operation namespace.
 */

/*
 * Operation methods.
 */

#if 0
static PyObject * Operation_SetSerialno(RootOperationObject *, PyObject *);
static PyObject * Operation_SetRefno(RootOperationObject *, PyObject *);
static PyObject * Operation_SetFrom(RootOperationObject *, PyObject *);
static PyObject * Operation_SetTo(RootOperationObject *, PyObject *);
static PyObject * Operation_SetSeconds(RootOperationObject *, PyObject *);
static PyObject * Operation_SetFutureSeconds(RootOperationObject *, PyObject *);
static PyObject * Operation_SetTimeString(RootOperationObject *, PyObject *);
static PyObject * Operation_SetArgs(RootOperationObject *, PyObject *);
static PyObject * Operation_GetSerialno(RootOperationObject *, PyObject *);
static PyObject * Operation_GetRefno(RootOperationObject *, PyObject *);
static PyObject * Operation_GetFrom(RootOperationObject *, PyObject *);
static PyObject * Operation_GetTo(RootOperationObject *, PyObject *);
static PyObject * Operation_GetSeconds(RootOperationObject *, PyObject *);
static PyObject * Operation_GetFutureSeconds(RootOperationObject *, PyObject *);
static PyObject * Operation_GetTimeString(RootOperationObject *, PyObject *);
static PyObject * Operation_GetArgs(RootOperationObject *, PyObject *);
#endif

/*
 * Operation methods structure.
 *
 * Generated from a macro in case we need one for each type of operation.
 *
 */

// extern PyMethodDef RootOperation_methods[];

extern PyTypeObject RootOperation_Type;

/*
 * Beginning of Operation creation functions section.
 */

RootOperationObject * newAtlasRootOperation(PyObject *arg);


#endif /* PY_OPERATION_H */
