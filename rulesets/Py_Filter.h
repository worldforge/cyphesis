#ifndef RULESETS_PY_FILTER_H
#define RULESETS_PY_FILTER_H

#include <Python.h>

#include "entityfilter/Filter.h"


class Filter;
PyObject* get_filter(PyObject* self, PyObject* query);
/// \brief Wrapper for Entity Filter in Python
/// \ingroup PythonWrappers
typedef struct {
    PyObject_HEAD
    /// \brief Filter object handled by this wrapper
    EntityFilter::Filter * m_filter;
} PyFilter;

extern PyTypeObject PyFilter_Type;

#define PyFilter_Check(_o) (Py_TYPE((_o)) == &PyFilter_Type)

PyFilter * newPyFilter();

#endif // RULESETS_PY_FILTER_H
