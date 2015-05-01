#include "Py_Filter.h"
#include "Py_Thing.h"
#include "entityfilter/Filter.h"

///\brief Create a new Filter object for a given query
PyObject * get_filter(PyObject * self, PyObject* query){
    if (!PyString_CheckExact(query)){
            PyErr_SetString(PyExc_TypeError, "Map_get_filter what must be string");
                    return NULL;
        }
        char * query_str = PyString_AsString(query);
        PyFilter* f = newPyFilter();
        try {
            //FIXME: creating and accessing an instance of a factory should be done in a better way
            EntityFilter::MindProviderFactory factory;
            f->m_filter = new EntityFilter::Filter(query_str, &factory);
        }
        catch (std::invalid_argument& e){
            PyErr_SetString(PyExc_TypeError, "Invalid query for Entity Filter");
            return NULL;
        }
        return (PyObject*)f;
}

///\brief Match a single entity using a filter that called this method.
static PyObject * match_entity(PyFilter * self, PyObject * py_entity)
{
    if (!PyLocatedEntity_Check(py_entity)) {
        PyErr_SetString(PyExc_AssertionError, "Argument must be Entity in match_entity");
        return NULL;
    }

    if(self->m_filter == NULL){
        PyErr_SetString(PyExc_AssertionError, "NULL Filter in Filter.match_entity");
        return NULL;
    }

    //Not sure if this is safest or the most correct way to get an entity pointer
    LocatedEntity* entity = ((PyEntity*)py_entity)->m_entity.l;

    EntityFilter::Filter* filter = self->m_filter;

    if (entity && filter->match(*entity)) {
        Py_INCREF(Py_True);
        return Py_True;
    } else {
        Py_INCREF(Py_False);
        return Py_False;
    }
}

///\This method is used to search the "contains" property of an entity,
///returning a list of entities that match a given filter
///
///This is useful when the client code knows that the query always requires
///to search within a contains property (i.e. inventory of a given entity)
///
///@param self - filter used to match entities
///@param py_entity - an entity whose "contains" property to search
PyObject* search_contains(PyFilter* self, PyEntity* py_entity){
#ifndef NDEBUG
    if (self->m_filter == NULL) {
        PyErr_SetString(PyExc_AssertionError,
                        "NULL Filter in Entity_filter.search_contains");
        return NULL;
    }
#endif // NDEBUG

    //This function is often used on mind's own entity, in which case, the type is PyMind
    if (!PyMind_Check(py_entity) && !PyEntity_Check(py_entity)){
        return NULL;
    }
    LocatedEntity* ent = py_entity->m_entity.l;

    if(!ent->m_contains){
        return PyList_New(0);
    }
    //Perform actual search
    auto iter = ent->m_contains->begin();
    auto iter_end = ent->m_contains->end();

    std::vector<LocatedEntity*> res;

    for (;iter != iter_end; ++iter){
        if((**iter).isVisible() && self->m_filter->match(**iter)){
            res.push_back(*iter);
        }
    }

    //Create a python list an fill it with the entities we got
    //FIXME: the code below is reused in multiple places
    PyObject * list = PyList_New(res.size());
    if (list == NULL) {
        return NULL;
    }
    std::vector<LocatedEntity*>::const_iterator Iend = res.end();
    int i = 0;
    for (std::vector<LocatedEntity*>::const_iterator I = res.begin(); I != Iend; ++I, ++i) {
        PyObject * thing = wrapEntity(*I);
        if (thing == NULL) {
            Py_DECREF(list);
            return NULL;
        }
        PyList_SetItem(list, i, thing);
    }
    return list;
}

static void Filter_dealloc(PyFilter *self)
{
    if (self->m_filter != NULL) {
        delete self->m_filter;
    }
    self->ob_type->tp_free((PyObject*)self);
}

static PyMethodDef Filter_methods[] = {
    {"match_entity",        (PyCFunction)match_entity,       METH_O},
    {"search_contains",     (PyCFunction)search_contains,    METH_O},
    {NULL,                  NULL}           // sentinel
};

PyTypeObject PyFilter_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                              // ob_size
        "Filter",                       // tp_name
        sizeof(PyFilter),               // tp_basicsize
        0,                              // tp_itemsize
        // methods
        (destructor)Filter_dealloc,     // tp_dealloc
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
        "Filter object",                // tp_doc
        0,                              // tp_travers
        0,                              // tp_clear
        0,                              // tp_richcompare
        0,                              // tp_weaklistoffset
        0,                              // tp_iter
        0,                              // tp_iternext
        Filter_methods,                 // tp_methods
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
