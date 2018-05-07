//
// Created by erik on 2018-05-05.
//

#ifndef CYPHESIS_CYPY_PROPS_H
#define CYPHESIS_CYPY_PROPS_H

#include <Python.h>

class LocatedEntity;

/// \brief Wrapper for Location in Python
/// \ingroup PythonWrappers
typedef struct {
    PyObject_HEAD
    /// \brief Entity object
    LocatedEntity * owner;
} CyPyProps;

extern PyTypeObject CyPyProps_Type;

#define CyPyProps_Check(_o) (Py_TYPE((_o)) == &CyPyProps_Type)

CyPyProps * newCyPyProps();

#endif //CYPHESIS_CYPY_PROPS_H
