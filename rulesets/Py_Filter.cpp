#include "Py_Filter.h"

PyTypeObject PyFilter_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                              // ob_size
        "Filter",                       // tp_name
        sizeof(PyFilter),               // tp_basicsize
        0,                              // tp_itemsize
        // methods
        0,                              // tp_dealloc
        0,                              // tp_print
        0,                              // tp_getattr
        0,                              // tp_setattr
        0,                              // tp_compare
        0,                              // tp_repr
        0,                              // tp_as_number
        0,                              // tp_as_sequence
        0,                              // tp_as_mapping
        0,                              // tp_hash
        0,                              // tp_call
        0,                              // tp_str
        0,                              // tp_getattro
        0,                              // tp_setattro
        0,                              // tp_as_buffer
        Py_TPFLAGS_DEFAULT,             // tp_flags
        "Filter object",               // tp_doc
        0,                              // tp_travers
        0,                              // tp_clear
        0,                              // tp_richcompare
        0,                              // tp_weaklistoffset
        0,                              // tp_iter
        0,                              // tp_iternext
        0,                              // tp_methods
        0,                              // tp_members
        0,                              // tp_getset
        0,                              // tp_base
        0,                              // tp_dict
        0,                              // tp_descr_get
        0,                              // tp_descr_set
        0,                              // tp_dictoffset
        0,                              // tp_init
        0,                              // tp_alloc
        0,                              // tp_new
};

PyFilter* newPyFilter(){
    return (PyFilter*)PyFilter_Type.tp_new(&PyFilter_Type, 0, 0);
}
