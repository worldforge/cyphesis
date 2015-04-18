#ifndef RULESETS_PY_FILTER_H
#define RULESETS_PY_FILTER_H

#include "entityfilter/Filter.h"

#include <Python.h>

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

#define PyFilter_Check(_o) ((_o)->ob_type == &PyFilter_Type)

PyFilter * newPyFilter();

#endif // RULESETS_PY_FILTER_H
