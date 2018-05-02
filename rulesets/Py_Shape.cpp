// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2011 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA


#include "Py_Shape.h"

#include "Py_BBox.h"
#include "Py_Message.h"
#include "Py_Point3D.h"

#include "physics/Shape.h"

#include "physics/Course.h"

#include <Atlas/Message/Element.h>

#include <wfmath/line.h>
#include <wfmath/polygon.h>

#include <sstream>
#include <iostream>

template<int dim> class LinearCourse : public Course<dim, WFMath::Line>
{
};

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;

static PyObject * Shape_area(PyShape * self)
{
#ifndef NDEBUG
    if (self->shape.s == nullptr) {
        PyErr_SetString(PyExc_AssertionError, "nullptr Shape in Shape.area");
        return nullptr;
    }
#endif // NDEBUG
    return PyFloat_FromDouble(self->shape.s->area());
}

static PyObject * Shape_footprint(PyShape * self)
{
#ifndef NDEBUG
    if (self->shape.s == nullptr) {
        PyErr_SetString(PyExc_AssertionError, "nullptr Shape in Shape.footprint");
        return nullptr;
    }
#endif // NDEBUG
    PyShape * res = newPyBox();
    if (res != 0) {
        res->shape.p = new MathShape<WFMath::AxisBox, 2>(self->shape.s->footprint());
    }
    return (PyObject*)res;
}

static PyObject * Shape_as_data(PyShape * self)
{
#ifndef NDEBUG
    if (self->shape.s == nullptr) {
        PyErr_SetString(PyExc_AssertionError, "nullptr Shape in Shape.as_data");
        return nullptr;
    }
#endif // NDEBUG
    MapType atlas_data;
    self->shape.s->toAtlas(atlas_data);
    return MessageElement_asPyObject(atlas_data);
}

static PyMethodDef Shape_methods[] = {
    {"area",               (PyCFunction)Shape_area,              METH_NOARGS},
    {"footprint",          (PyCFunction)Shape_footprint,         METH_NOARGS},
    {"as_data",            (PyCFunction)Shape_as_data,           METH_NOARGS},
    {nullptr,              nullptr}           /* sentinel */
};

static PyObject * Area_centre(PyShape * self)
{
#ifndef NDEBUG
    if (self->shape.s == nullptr) {
        PyErr_SetString(PyExc_AssertionError, "nullptr Shape in Shape.centre");
        return nullptr;
    }
#endif // NDEBUG
    PyPoint3D * res = newPyPoint3D();
    if (res != 0) {
        res->coords = self->shape.b->centre();
    }
    return (PyObject*)res;
}

static PyObject * Area_low_corner(PyShape * self)
{
#ifndef NDEBUG
    if (self->shape.s == nullptr) {
        PyErr_SetString(PyExc_AssertionError, "nullptr Shape in Shape.low_corner");
        return nullptr;
    }
#endif // NDEBUG
    PyPoint3D * res = newPyPoint3D();
    if (res != 0) {
        res->coords = self->shape.b->lowCorner();
    }
    return (PyObject*)res;
}

static PyObject * Area_high_corner(PyShape * self)
{
#ifndef NDEBUG
    if (self->shape.s == nullptr) {
        PyErr_SetString(PyExc_AssertionError, "nullptr Shape in Shape.high_corner");
        return nullptr;
    }
#endif // NDEBUG
    PyPoint3D * res = newPyPoint3D();
    if (res != 0) {
        res->coords = self->shape.b->highCorner();
    }
    return (PyObject*)res;
}

static PyMethodDef Area_methods[] = {
    {"centre",             (PyCFunction)Area_centre,             METH_NOARGS},
    {"low_corner",         (PyCFunction)Area_low_corner,         METH_NOARGS},
    {"high_corner",        (PyCFunction)Area_high_corner,        METH_NOARGS},
    {nullptr,              nullptr}           /* sentinel */
};

static PyObject * Body_centre(PyShape * self)
{
#ifndef NDEBUG
    if (self->shape.s == nullptr) {
        PyErr_SetString(PyExc_AssertionError, "nullptr Shape in Shape.centre");
        return nullptr;
    }
#endif // NDEBUG
    PyPoint3D * res = newPyPoint3D();
    if (res != 0) {
        res->coords = self->shape.b->centre();
    }
    return (PyObject*)res;
}

static PyObject * Body_low_corner(PyShape * self)
{
#ifndef NDEBUG
    if (self->shape.s == nullptr) {
        PyErr_SetString(PyExc_AssertionError, "nullptr Shape in Shape.low_corner");
        return nullptr;
    }
#endif // NDEBUG
    PyPoint3D * res = newPyPoint3D();
    if (res != 0) {
        res->coords = self->shape.b->lowCorner();
    }
    return (PyObject*)res;
}

static PyObject * Body_high_corner(PyShape * self)
{
#ifndef NDEBUG
    if (self->shape.s == nullptr) {
        PyErr_SetString(PyExc_AssertionError, "nullptr Shape in Shape.high_corner");
        return nullptr;
    }
#endif // NDEBUG
    PyPoint3D * res = newPyPoint3D();
    if (res != 0) {
        res->coords = self->shape.b->highCorner();
    }
    return (PyObject*)res;
}

static PyMethodDef Body_methods[] = {
    {"centre",             (PyCFunction)Body_centre,             METH_NOARGS},
    {"low_corner",         (PyCFunction)Body_low_corner,         METH_NOARGS},
    {"high_corner",        (PyCFunction)Body_high_corner,        METH_NOARGS},
    {nullptr,              nullptr}           /* sentinel */
};

static PyBBox * Box_extrude(PyShape * self, PyObject * args)
{
#ifndef NDEBUG
    if (self->shape.s == nullptr) {
        PyErr_SetString(PyExc_AssertionError, "nullptr Shape in Shape.as_data");
        return nullptr;
    }
#endif // NDEBUG
    MathShape<WFMath::AxisBox, 2> * shape =
          dynamic_cast<MathShape<WFMath::AxisBox, 2> *>(self->shape.p);
    if (shape == 0) {
        PyErr_SetString(PyExc_RuntimeError, "Shape is not a 2D axisbox");
        return nullptr;
    }
    float low, high;
    if (!PyArg_ParseTuple(args, "ff", &low, &high)) {
        return nullptr;
    }
    PyBBox * ret = newPyBBox();
    if (ret != 0) {
        ret->box = BBox(Point3D(shape->shape().lowCorner().x(),
                                low,
                                shape->shape().lowCorner().y()),
                        Point3D(shape->shape().highCorner().x(),
                                high,
                                shape->shape().highCorner().y()),
                        true);
    }
    return ret;
}

static PyMethodDef Box_methods[] = {
    {"extrude",            (PyCFunction)Box_extrude,             METH_VARARGS},
    {nullptr,              nullptr}           /* sentinel */
};

static PyMethodDef Course_methods[] = {
    {nullptr,              nullptr}           /* sentinel */
};

static void Shape_dealloc(PyShape *self)
{
    if (self->shape.s != nullptr) {
        delete self->shape.s;
    }
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject * Shape_getattro(PyShape *self, PyObject * oname)
{
#ifndef NDEBUG
    if (self->shape.s == nullptr) {
        PyErr_SetString(PyExc_AssertionError, "nullptr Shape in Shape.getattro");
        return nullptr;
    }
#endif // NDEBUG
    // char * name = PyUnicode_AsUTF8(oname);
    return PyObject_GenericGetAttr((PyObject *)self, oname);
}

static int Shape_setattro(PyShape *self, PyObject *oname, PyObject *v)
{
#ifndef NDEBUG
    if (self->shape.s == nullptr) {
        PyErr_SetString(PyExc_AssertionError, "nullptr Shape in Shape.setattr");
        return -1;
    }
#endif // NDEBUG
    // char * name = PyUnicode_AsUTF8(oname);
    PyErr_SetString(PyExc_AttributeError, "unknown attribute");
    return -1;
}

static PyObject * Polygon_getattro(PyShape *self, PyObject * oname)
{
#ifndef NDEBUG
    if (self->shape.s == nullptr) {
        PyErr_SetString(PyExc_AssertionError, "nullptr Shape in Polygon.getattro");
        return nullptr;
    }
#endif // NDEBUG
    // char * name = PyUnicode_AsUTF8(oname);
    return PyObject_GenericGetAttr((PyObject *)self, oname);
}

static int Polygon_setattro(PyShape *self, PyObject *oname, PyObject *v)
{
#ifndef NDEBUG
    if (self->shape.s == nullptr) {
        PyErr_SetString(PyExc_AssertionError, "nullptr Shape in Polygon.setattr");
        return -1;
    }
#endif // NDEBUG
    // char * name = PyUnicode_AsUTF8(oname);
    PyErr_SetString(PyExc_AttributeError, "unknown attribute");
    return -1;
}

static PyObject * Shape_repr(PyShape *self)
{
    std::stringstream r;
    r << *self->shape.s;
    return PyUnicode_FromString(r.str().c_str());
}

static int Shape_init(PyShape * self, PyObject * args, PyObject * kwds)
{
    PyObject * arg = 0;
    if (!PyArg_ParseTuple(args, "|O", &arg)) {
        return -1;
    }
    if (arg == 0) {
        return 0;
    }
    if (PyDict_Check(arg)) {
        MapType data;
        if (PyDictObject_asElement(arg, data) != 0) {
            PyErr_SetString(PyExc_TypeError, "Error converting dict to atlas");
            return -1;
        }
        self->shape.s = Shape::newFromAtlas(data);
        if (self->shape.s == 0) {
            PyErr_SetString(PyExc_TypeError, "Error converting atlas to shape");
            return -1;
        }
        return 0;
    }
    if (PyMessage_Check(arg)) {
        Element * data = ((PyMessage*)arg)->m_obj;
        if (!data->isMap()) {
            PyErr_SetString(PyExc_TypeError, "Error converting dict to atlas");
            return -1;
        }
        self->shape.s = Shape::newFromAtlas(data->Map());
        if (self->shape.s == 0) {
            PyErr_SetString(PyExc_TypeError, "Error converting atlas to shape");
            return -1;
        }
        return 0;
    }
    PyErr_SetString(PyExc_TypeError, "Error converting unknown to shape");
    return -1;
}

template<template <int> class ShapeT, int dim>
static int MathShape_fromListType(PyShape * self, const ListType & data)
{
    self->shape.p = new MathShape<ShapeT, dim>;
    if (self->shape.p == 0) {
        PyErr_SetString(PyExc_MemoryError, "Error allocating shape object");
        return -1;
    }
    int ret = self->shape.p->fromAtlas(data);
    if (ret != 0) {
        PyErr_SetString(PyExc_TypeError, "Error converting list to polygon");
        return -1;
    }
    return 0;
}

template<template <int> class ShapeT, int dim>
static int MathShape_init(PyShape * self, PyObject * arg)
{
    if (PyList_Check(arg)) {
        ListType data;
        if (PyListObject_asElement(arg, data) != 0) {
            PyErr_SetString(PyExc_TypeError, "Error converting list to atlas");
            return -1;
        }
        return MathShape_fromListType<ShapeT, dim>(self, data);
    }
    if (PyMessage_Check(arg)) {
        Element * data = ((PyMessage*)arg)->m_obj;
        if (!data->isList()) {
            PyErr_SetString(PyExc_TypeError, "Atlas message is not a list");
            return -1;
        }
        return MathShape_fromListType<ShapeT, dim>(self, data->List());
    }
    PyErr_SetString(PyExc_TypeError, "Error converting unknown to polygon");
    return -1;
}

static int Box_init(PyShape * self, PyObject * args, PyObject * kwds)
{
    if (PyTuple_Size(args) != 0) {
        PyErr_SetString(PyExc_TypeError, "Box shape takes no args");
        return -1;
    }
    self->shape.p = new MathShape<WFMath::AxisBox, 2>;
    return 0;
}

static int Course_init(PyShape * self, PyObject * args, PyObject * kwds)
{
    PyObject * arg = 0;
    if (!PyArg_ParseTuple(args, "|O", &arg)) {
        return -1;
    }
    if (arg != 0) {
        if (PyLine_Check(arg)) {
        }
    }
    self->shape.p = new MathShape<LinearCourse, 2>;
    return 0;
}

static int Line_init(PyShape * self, PyObject * args, PyObject * kwds)
{
    PyObject * arg = 0;
    if (!PyArg_ParseTuple(args, "O", &arg)) {
        return -1;
    }
    return MathShape_init<WFMath::Line, 2>(self, arg);
}

static int Polygon_init(PyShape * self, PyObject * args, PyObject * kwds)
{
    PyObject * arg = 0;
    if (!PyArg_ParseTuple(args, "O", &arg)) {
        return -1;
    }
    return MathShape_init<WFMath::Polygon, 2>(self, arg);
}

static PyObject * Shape_new(PyTypeObject * type, PyObject *, PyObject *)
{
    // This looks allot like the default implementation
    PyShape * self = (PyShape *)type->tp_alloc(type, 0);
    if (self != nullptr) {
        self->shape.s = nullptr;
    }
    return (PyObject *)self;
}

Py_ssize_t Shape_sq_length(PyShape * self)
{
#ifndef NDEBUG
    if (self->shape.s == nullptr) {
        PyErr_SetString(PyExc_AssertionError, "nullptr Shape in Shape.copy");
        return 0;
    }
#endif // NDEBUG
    return self->shape.s->size();
}

PyShape * Area_sq_item(PyShape * self, Py_ssize_t index)
{
#ifndef NDEBUG
    if (self->shape.s == nullptr) {
        PyErr_SetString(PyExc_AssertionError, "nullptr Shape in Shape.copy");
        return 0;
    }
#endif // NDEBUG
    if (static_cast<size_t>(index) >= self->shape.s->size()) {
        PyErr_SetString(PyExc_IndexError, "Number of corners exceeded");
        return 0;
    }
    PyShape * a = newPyArea();
    if (a != 0) {
        a->shape.p =
              new MathShape<WFMath::Point, 2>(self->shape.p->getCorner(index));
        if (a->shape.p == 0) {
            Py_DECREF(a);
            a = nullptr;
        }
    }
    return a;
}

PyPoint3D * Body_sq_item(PyShape * self, Py_ssize_t index)
{
#ifndef NDEBUG
    if (self->shape.s == nullptr) {
        PyErr_SetString(PyExc_AssertionError, "nullptr Shape in Shape.copy");
        return 0;
    }
#endif // NDEBUG
    if (static_cast<size_t>(index) >= self->shape.s->size()) {
        PyErr_SetString(PyExc_IndexError, "Number of corners exceeded");
        return 0;
    }
    PyPoint3D * v = newPyPoint3D();
    if (v != nullptr) {
        v->coords = self->shape.b->getCorner(index);
    }
    return v;
}

static PyObject * Shape_inplace_multiply(PyShape * self, PyObject * other)
{
    if (!PyFloat_CheckExact(other)) {
        PyErr_SetString(PyExc_TypeError, "Can only scale with a float");
        return nullptr;
    }
    double val = PyFloat_AsDouble(other);
    self->shape.s->scale(val);
    Py_INCREF(self);
    return (PyObject*)self;
}

static PyNumberMethods Shape_number = {
    0,                                        // nb_add;
    0,                                        // nb_subtract;
    0,                                        // nb_multiply;
    0,                                        // nb_divide;
    0,                                        // nb_remainder;
    0,                                        // nb_divmod;
    0,                                        // nb_power;
    0,                                        // nb_negative;
    0,                                        // nb_positive;
    0,                                        // nb_absolute;
    0,                                        // nb_nonzero;
    0,                                        // nb_invert;
    0,                                        // nb_lshift;
    0,                                        // nb_rshift;
    0,                                        // nb_and;
    0,                                        // nb_xor;
    0,                                        // nb_or;
    0,                                        // nb_coerce;
    0,                                        // nb_int;
    0,                                        // nb_long;
    0,                                        // nb_float;
    0,                                        // nb_oct;
    0,                                        // nb_hex;
    /* Added in release 2.0 */
    0,                                        // nb_inplace_add;
    0,                                        // nb_inplace_subtract;
    (binaryfunc)Shape_inplace_multiply,       // nb_inplace_multiply;
    0,                                        // nb_inplace_divide;
    0,                                        // nb_inplace_remainder;
    0,                                        // nb_inplace_power;
    0,                                        // nb_inplace_lshift;
    0,                                        // nb_inplace_rshift;
    0,                                        // nb_inplace_and;
    0,                                        // nb_inplace_xor;
    0,                                        // nb_inplace_or;

};

static PySequenceMethods Shape_sequence = {
        (lenfunc)Shape_sq_length, // sq_length;
        0, // sq_concat;
        0, // sq_repeat;
        0, // sq_item;
        0, // sq_slice;
        0, // sq_ass_item;
        0, // sq_ass_slice;
        0, // sq_contains;
        /* Added in release 2.0 */
        0, // sq_inplace_concat;
        0, // sq_inplace_repeat;
};

// FIXME Check if inheritance will pick up sq_length from above
static PySequenceMethods Area_sequence = {
        (lenfunc)Shape_sq_length, // sq_length;
        0, // sq_concat;
        0, // sq_repeat;
        (ssizeargfunc)Area_sq_item, // sq_item;
        0, // sq_slice;
        0, // sq_ass_item;
        0, // sq_ass_slice;
        0, // sq_contains;
        /* Added in release 2.0 */
        0, // sq_inplace_concat;
        0, // sq_inplace_repeat;
};

static PySequenceMethods Body_sequence = {
        (lenfunc)Shape_sq_length, // sq_length;
        0, // sq_concat;
        0, // sq_repeat;
        (ssizeargfunc)Body_sq_item, // sq_item;
        0, // sq_slice;
        0, // sq_ass_item;
        0, // sq_ass_slice;
        0, // sq_contains;
        /* Added in release 2.0 */
        0, // sq_inplace_concat;
        0, // sq_inplace_repeat;
};

PyTypeObject PyShape_Type = {
        PyVarObject_HEAD_INIT(&PyType_Type, 0)
        "physics.Shape",                /*tp_name*/
        sizeof(PyShape),                /*tp_basicsize*/
        0,                              /*tp_itemsize*/
        /* methods */
        (destructor)Shape_dealloc,      /*tp_dealloc*/
        0,                              /*tp_print*/
        0,                              /*tp_getattr*/
        0,                              /*tp_setattr*/
        0,                              /*tp_compare*/
        (reprfunc)Shape_repr,           /*tp_repr*/
        &Shape_number,                  /*tp_as_number*/
        &Shape_sequence,                /*tp_as_sequence*/
        0,                              /*tp_as_mapping*/
        0,                              /*tp_hash*/
        0,                              // tp_call
        0,                              // tp_str
        (getattrofunc)Shape_getattro,   // tp_getattro
        (setattrofunc)Shape_setattro,   // tp_setattro
        0,                              // tp_as_buffer
        Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,             // tp_flags
        "Shape objects",                // tp_doc
        0,                              // tp_travers
        0,                              // tp_clear
        0,                              // tp_richcompare
        0,                              // tp_weaklistoffset
        0,                              // tp_iter
        0,                              // tp_iternext
        Shape_methods,                  // tp_methods
        0,                              // tp_members
        0,                              // tp_getset
        0,                              // tp_base
        0,                              // tp_dict
        0,                              // tp_descr_get
        0,                              // tp_descr_set
        0,                              // tp_dictoffset
        (initproc)Shape_init,           // tp_init
        0,                              // tp_alloc
        Shape_new,                      // tp_new
};

PyTypeObject PyArea_Type = {
        PyVarObject_HEAD_INIT(&PyType_Type, 0)
        "physics.Area",                 /*tp_name*/
        sizeof(PyShape),                /*tp_basicsize*/
        0,                              /*tp_itemsize*/
        /* methods */
        0,                              /*tp_dealloc*/
        0,                              /*tp_print*/
        0,                              /*tp_getattr*/
        0,                              /*tp_setattr*/
        0,                              /*tp_compare*/
        0,                              /*tp_repr*/
        0,                              /*tp_as_number*/
        &Area_sequence,                 /*tp_as_sequence*/
        0,                              /*tp_as_mapping*/
        0,                              /*tp_hash*/
        0,                              // tp_call
        0,                              // tp_str
        0,                              // tp_getattro
        0,                              // tp_setattro
        0,                              // tp_as_buffer
        Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,             // tp_flags
        "Area objects",                 // tp_doc
        0,                              // tp_travers
        0,                              // tp_clear
        0,                              // tp_richcompare
        0,                              // tp_weaklistoffset
        0,                              // tp_iter
        0,                              // tp_iternext
        Area_methods,                   // tp_methods
        0,                              // tp_members
        0,                              // tp_getset
        &PyShape_Type,                  // tp_base
        0,                              // tp_dict
        0,                              // tp_descr_get
        0,                              // tp_descr_set
        0,                              // tp_dictoffset
        (initproc)Box_init,             // tp_init
        0,                              // tp_alloc
        Shape_new,                      // tp_new
};

PyTypeObject PyBody_Type = {
        PyVarObject_HEAD_INIT(&PyType_Type, 0)
        "physics.Body",                 /*tp_name*/
        sizeof(PyShape),                /*tp_basicsize*/
        0,                              /*tp_itemsize*/
        /* methods */
        0,                              /*tp_dealloc*/
        0,                              /*tp_print*/
        0,                              /*tp_getattr*/
        0,                              /*tp_setattr*/
        0,                              /*tp_compare*/
        0,                              /*tp_repr*/
        0,                              /*tp_as_number*/
        &Body_sequence,                 /*tp_as_sequence*/
        0,                              /*tp_as_mapping*/
        0,                              /*tp_hash*/
        0,                              // tp_call
        0,                              // tp_str
        0,                              // tp_getattro
        0,                              // tp_setattro
        0,                              // tp_as_buffer
        Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,             // tp_flags
        "Body objects",                 // tp_doc
        0,                              // tp_travers
        0,                              // tp_clear
        0,                              // tp_richcompare
        0,                              // tp_weaklistoffset
        0,                              // tp_iter
        0,                              // tp_iternext
        Body_methods,                   // tp_methods
        0,                              // tp_members
        0,                              // tp_getset
        &PyShape_Type,                  // tp_base
        0,                              // tp_dict
        0,                              // tp_descr_get
        0,                              // tp_descr_set
        0,                              // tp_dictoffset
        (initproc)Box_init,             // tp_init
        0,                              // tp_alloc
        Shape_new,                      // tp_new
};

PyTypeObject PyBox_Type = {
        PyVarObject_HEAD_INIT(&PyType_Type, 0)
        "physics.Box",                  /*tp_name*/
        sizeof(PyShape),                /*tp_basicsize*/
        0,                              /*tp_itemsize*/
        /* methods */
        0,                              /*tp_dealloc*/
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
        0,                              // tp_getattro
        0,                              // tp_setattro
        0,                              // tp_as_buffer
        Py_TPFLAGS_DEFAULT,             // tp_flags
        "Box objects",                  // tp_doc
        0,                              // tp_travers
        0,                              // tp_clear
        0,                              // tp_richcompare
        0,                              // tp_weaklistoffset
        0,                              // tp_iter
        0,                              // tp_iternext
        Box_methods,                    // tp_methods
        0,                              // tp_members
        0,                              // tp_getset
        &PyArea_Type,                   // tp_base
        0,                              // tp_dict
        0,                              // tp_descr_get
        0,                              // tp_descr_set
        0,                              // tp_dictoffset
        (initproc)Box_init,             // tp_init
        0,                              // tp_alloc
        Shape_new,                      // tp_new
};

PyTypeObject PyCourse_Type = {
        PyVarObject_HEAD_INIT(&PyType_Type, 0)

        "physics.Course",               /*tp_name*/
        sizeof(PyShape),                /*tp_basicsize*/
        0,                              /*tp_itemsize*/
        /* methods */
        0,                              /*tp_dealloc*/
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
        0,                              // tp_getattro
        0,                              // tp_setattro
        0,                              // tp_as_buffer
        Py_TPFLAGS_DEFAULT,             // tp_flags
        "Course objects",               // tp_doc
        0,                              // tp_travers
        0,                              // tp_clear
        0,                              // tp_richcompare
        0,                              // tp_weaklistoffset
        0,                              // tp_iter
        0,                              // tp_iternext
        Course_methods,                 // tp_methods
        0,                              // tp_members
        0,                              // tp_getset
        &PyArea_Type,                   // tp_base
        0,                              // tp_dict
        0,                              // tp_descr_get
        0,                              // tp_descr_set
        0,                              // tp_dictoffset
        (initproc)Course_init,          // tp_init
        0,                              // tp_alloc
        Shape_new,                      // tp_new
};

PyTypeObject PyLine_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        
        "physics.Line",                 /*tp_name*/
        sizeof(PyShape),                /*tp_basicsize*/
        0,                              /*tp_itemsize*/
        /* methods */
        0,                              /*tp_dealloc*/
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
        0,                              // tp_getattro
        0,                              // tp_setattro
        0,                              // tp_as_buffer
        Py_TPFLAGS_DEFAULT,             // tp_flags
        "Line objects",                 // tp_doc
        0,                              // tp_travers
        0,                              // tp_clear
        0,                              // tp_richcompare
        0,                              // tp_weaklistoffset
        0,                              // tp_iter
        0,                              // tp_iternext
        0,                              // tp_methods
        0,                              // tp_members
        0,                              // tp_getset
        &PyArea_Type,                   // tp_base
        0,                              // tp_dict
        0,                              // tp_descr_get
        0,                              // tp_descr_set
        0,                              // tp_dictoffset
        (initproc)Line_init,            // tp_init
        0,                              // tp_alloc
        Shape_new,                      // tp_new
};

PyTypeObject PyPolygon_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        
        "physics.Polygon",              /*tp_name*/
        sizeof(PyShape),                /*tp_basicsize*/
        0,                              /*tp_itemsize*/
        /* methods */
        0,                              /*tp_dealloc*/
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
        (getattrofunc)Polygon_getattro, // tp_getattro
        (setattrofunc)Polygon_setattro, // tp_setattro
        0,                              // tp_as_buffer
        Py_TPFLAGS_DEFAULT,             // tp_flags
        "Polygon objects",              // tp_doc
        0,                              // tp_travers
        0,                              // tp_clear
        0,                              // tp_richcompare
        0,                              // tp_weaklistoffset
        0,                              // tp_iter
        0,                              // tp_iternext
        0,                              // tp_methods
        0,                              // tp_members
        0,                              // tp_getset
        &PyArea_Type,                   // tp_base
        0,                              // tp_dict
        0,                              // tp_descr_get
        0,                              // tp_descr_set
        0,                              // tp_dictoffset
        (initproc)Polygon_init,         // tp_init
        0,                              // tp_alloc
        Shape_new,                      // tp_new
};

PyShape * wrapShape(Shape * shape)
{
    PyShape * wrapper = nullptr;
    Form<2> * plan = dynamic_cast<Form<2> *>(shape);
    if (plan != 0) {
        wrapper = newPyArea();
        if (wrapper != 0) {
            wrapper->shape.p = plan;
        }
    } else {
        Form<3> * body = dynamic_cast<Form<3> *>(shape);
        if (body != 0) {
            wrapper = newPyBody();
            if (wrapper != 0) {
                wrapper->shape.b = body;
            }
        }
    }
    return wrapper;
}

PyShape * newPyShape()
{
    return (PyShape *)PyShape_Type.tp_new(&PyShape_Type, 0, 0);
}

PyShape * newPyArea()
{
    return (PyShape *)PyArea_Type.tp_new(&PyArea_Type, 0, 0);
}

PyShape * newPyBody()
{
    return (PyShape *)PyBody_Type.tp_new(&PyBody_Type, 0, 0);
}

PyShape * newPyBox()
{
    return (PyShape *)PyBox_Type.tp_new(&PyBox_Type, 0, 0);
}
