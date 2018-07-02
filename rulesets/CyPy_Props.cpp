//
// Created by erik on 2018-05-05.
//

#include "CyPy_Props.h"
#include "Py_Message.h"


#include "LocatedEntity.h"
#include "StatisticsProperty.h"
#include "TerrainProperty.h"
#include "TerrainModProperty.h"
#include "Py_Property.h"
#include <Atlas/Message/Element.h>


using Atlas::Message::Element;
using Atlas::Message::MapType;


static CyPyProps * Props_new(PyTypeObject * type,
                             PyObject * args,
                             PyObject * kwds)
{
    CyPyProps * self = (CyPyProps *)type->tp_alloc(type, 0);
    return self;
}


static void Props_dealloc(CyPyProps *self)
{
    if (self->owner) {
        self->owner->decRef();
    }
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject * Props_getattro(CyPyProps *self, PyObject *oname)
{
    char * name = PyUnicode_AsUTF8(oname);

    LocatedEntity * locatedEntity = self->owner;
    auto prop = locatedEntity->getProperty(name);
    if (prop) {
        //Check if it's a special prop
        if (dynamic_cast<const StatisticsProperty*>(prop) || dynamic_cast<const TerrainProperty*>(prop)
            || dynamic_cast<const TerrainModProperty*>(prop)) {
            PyObject* ret = Property_asPyObject(prop, locatedEntity);
            if (ret) {
                return ret;
            }
        } else {
            Element attr;
            // If this property is not set with a value, return none.
            if (prop->get(attr) == 0) {
                return MessageElement_asPyObject(attr);
            }
        }
    }

    Py_INCREF(Py_None);
    return Py_None;
}

static int Props_setattro(CyPyProps *self, PyObject *oname, PyObject *v)
{
    char * name = PyUnicode_AsUTF8(oname);
    LocatedEntity * entity = self->owner;
    // Should we support removal of attributes?
    //std::string attr(name);
    //if (v == nullptr) {
    //entity->attributes.erase(attr);
    //return 0;
    //}
    Element obj;
    if (PyObject_asMessageElement(v, obj, false) == 0) {
        // In the Python wrapper for Entity in Py_Thing.cpp notices are issued
        // for some types.
        entity->setAttr(name, obj);
        return 0;
    } else {
        PyErr_SetString(PyExc_AttributeError, name);
        log(WARNING, "Value submitted to Props.set can not be converted to an Atlas Message.");
    }
    return PyObject_GenericSetAttr((PyObject *)self, oname, v);
}



PyTypeObject CyPyProps_Type = {
        PyVarObject_HEAD_INIT(&PyType_Type, 0)
        "server.Props",                 /*tp_name*/
        sizeof(CyPyProps),              /*tp_basicsize*/
        0,                              /*tp_itemsize*/
        /* methods */
        (destructor)Props_dealloc,      /*tp_dealloc*/
        0,                              /*tp_print*/
        0,                              /*tp_getattr*/
        0,                              /*tp_setattr*/
        0,                              /*tp_compare*/
        0,                              /*tp_repr*/
        0,                              /*tp_as_number*/
        0,                              /*tp_as_sequence*/
        0,                              /*tp_as_mapping*/
        0,                              /*tp_hash*/
        0,                              // tp_call
        0,                              // tp_str
        (getattrofunc)Props_getattro,   // tp_getattro
        (setattrofunc)Props_setattro,   // tp_setattro
        0,                              // tp_as_buffer
        Py_TPFLAGS_DEFAULT,             // tp_flags
        "Props objects",                // tp_doc
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
        PyType_GenericAlloc,            // tp_alloc
        0,                              // tp_new
};

CyPyProps * newCyPyProps()
{
    return (CyPyProps *)Props_new(&CyPyProps_Type, 0, 0);
}
