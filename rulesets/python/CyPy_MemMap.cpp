/*
 Copyright (C) 2018 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "CyPy_MemMap.h"
#include "CyPy_LocatedEntity.h"
#include "CyPy_Location.h"
#include "CyPy_Element.h"
#include "CyPy_RootEntity.h"

#include <Atlas/Objects/objectFactory.h>
#include <rulesets/Py_Message.h>
#include <Python-ast.h>
#include <rulesets/Py_Thing.h>
#include <rulesets/Py_RootEntity.h>

using Atlas::Objects::Root;
using Atlas::Objects::Factories;
using Atlas::Objects::Entity::RootEntity;

CyPy_MemMap::CyPy_MemMap(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
    : WrapperBase(self, args, kwds)
{
    args.verify_length(1);
    //verifyObject<CyPy_LocatedEntity>(args.front());
}

CyPy_MemMap::CyPy_MemMap(Py::PythonClassInstance* self, MemMap* value)
    : WrapperBase(self, value)
{

}


Py::Object CyPy_MemMap::find_by_location(const Py::Tuple& args)
{
    Py::List list;
    args.verify_length(3);
    auto& location = verifyObject<CyPy_Location>(args[0]);
    if (!location.isValid()) {
        throw Py::TypeError("Location is incomplete");
    }
    auto radius = verifyNumeric(args[1]);
    auto type = verifyString(args[2]);
    auto res = m_value->findByLocation(location, radius, type);

    for (auto& entry : res) {
        list.append(CyPy_LocatedEntity::wrap(entry));
    }
    return list;
}

Py::Object CyPy_MemMap::find_by_type(const Py::Tuple& args)
{
    Py::List list;

    args.verify_length(1);
    auto what = verifyString(args.front());
    auto res = m_value->findByType(what);

    for (auto& entry : res) {
        list.append(CyPy_LocatedEntity::wrap(entry));
    }
    return list;
}

Py::Object CyPy_MemMap::updateAdd(const Py::Tuple& args)
{
//    args.verify_length(2);
//
//    double time = verifyNumeric(args[1]);
//    if (CyPy_Element::check(args[0])) {
//        auto& element = CyPy_Element::value(args[0]);
//        if (!element.isMap()) {
//            throw Py::TypeError("arg is a Message that is not a map");
//        }
//        try {
//            Root obj = Factories::instance()->createObject(element.Map());
//
//            RootEntity ent = Atlas::Objects::smart_dynamic_cast<RootEntity>(obj);
//            if (!ent.isValid()) {
//                throw Py::TypeError("arg is a Message that does not represent an entity");
//            }
//            auto* ret = m_value->updateAdd(ent, time);
//            if (!ret) {
//                throw Py::TypeError("arg is a Message that does not have an ID");
//            }
//            return CyPy_MemEntity::wrap(ret);
//        }
//        catch (Atlas::Message::WrongTypeException&) {
//            throw Py::TypeError("arg is a Message that contains malformed attributes");
//        }
//    } else if (CyPy_RootEntity::check(args[0])) {
//        auto& ent = CyPy_RootEntity::value(args[0]);
//        auto* ret = m_value->updateAdd(ent, time);
//        if (!ret) {
//            throw Py::TypeError("arg is a Message that does not have an ID");
//        }
//        return CyPy_MemEntity::wrap(ret);
//    } else {
//        throw Py::TypeError("arg is not an Atlas Entity or Message");
//    }
}

//Py::Object CyPy_MemMap::delete_(const Py::Tuple& args)
//{
//#ifndef NDEBUG
//    if (self->m_map == nullptr) {
//        PyErr_SetString(PyExc_AssertionError, "nullptr MemMap in MemMap.delete");
//        return nullptr;
//    }
//#endif // NDEBUG
//    if (!PyUnicode_CheckExact(py_id)) {
//        throw Py::TypeError("MemMap.delete must be string");
//        return nullptr;
//    }
//    char * id = PyUnicode_AsUTF8(py_id);
//    self->m_map->del(id);
//
//    Py_INCREF(Py_None);
//    return Py_None;
//}
//
//Py::Object CyPy_MemMap::get(const Py::Tuple& args)
//{
//#ifndef NDEBUG
//    if (self->m_map == nullptr) {
//        PyErr_SetString(PyExc_AssertionError, "nullptr MemMap in MemMap.get");
//        return nullptr;
//    }
//#endif // NDEBUG
//    if (!PyUnicode_CheckExact(py_id)) {
//        throw Py::TypeError("MemMap.get must be string");
//        return nullptr;
//    }
//    char * id = PyUnicode_AsUTF8(py_id);
//    MemEntity * ret = self->m_map->get(id);
//    if (ret == nullptr) {
//        Py_INCREF(Py_None);
//        return Py_None;
//    }
//    PyObject * thing = wrapEntity(ret);
//    return thing;
//}
//
//Py::Object CyPy_MemMap::get_add(const Py::Tuple& args)
//{
//#ifndef NDEBUG
//    if (self->m_map == nullptr) {
//        PyErr_SetString(PyExc_AssertionError, "nullptr MemMap in MemMap.get_add");
//        return nullptr;
//    }
//#endif // NDEBUG
//    if (!PyUnicode_CheckExact(py_id)) {
//        throw Py::TypeError("MemMap.get_add must be string");
//        return nullptr;
//    }
//    char * id = PyUnicode_AsUTF8(py_id);
//    MemEntity * ret = self->m_map->getAdd(id);
//    assert(ret != 0);
//    PyObject * thing = wrapEntity(ret);
//    return thing;
//}
//
//Py::Object CyPy_MemMap::add_hooks_append(const Py::Tuple& args)
//{
//#ifndef NDEBUG
//    if (self->m_map == nullptr) {
//        PyErr_SetString(PyExc_AssertionError, "nullptr MemMap in MemMap.add_hooks_append");
//        return nullptr;
//    }
//#endif // NDEBUG
//    if (!PyUnicode_CheckExact(py_method)) {
//        throw Py::TypeError("MemMap.add_hooks_append must be string");
//        return nullptr;
//    }
//    char * method = PyUnicode_AsUTF8(py_method);
//    self->m_map->getAddHooks().push_back(std::string(method));
//
//    Py_INCREF(Py_None);
//    return Py_None;
//}
//
//Py::Object CyPy_MemMap::update_hooks_append(const Py::Tuple& args)
//{
//#ifndef NDEBUG
//    if (self->m_map == nullptr) {
//        PyErr_SetString(PyExc_AssertionError, "nullptr MemMap in MemMap.update_hooks_append");
//        return nullptr;
//    }
//#endif // NDEBUG
//    if (!PyUnicode_CheckExact(py_method)) {
//        throw Py::TypeError("MemMap.update_hooks_append must be string");
//        return nullptr;
//    }
//    char * method = PyUnicode_AsUTF8(py_method);
//    self->m_map->getUpdateHooks().push_back(std::string(method));
//
//    Py_INCREF(Py_None);
//    return Py_None;
//}
//
//Py::Object CyPy_MemMap::delete_hooks_append(const Py::Tuple& args)
//{
//#ifndef NDEBUG
//    if (self->m_map == nullptr) {
//        PyErr_SetString(PyExc_AssertionError, "nullptr MemMap in MemMap.delete_hooks_append");
//        return nullptr;
//    }
//#endif // NDEBUG
//    if (!PyUnicode_CheckExact(py_method)) {
//        throw Py::TypeError("MemMap.delete_hooks_append must be string");
//        return nullptr;
//    }
//    char * method = PyUnicode_AsUTF8(py_method);
//    self->m_map->getDeleteHooks().push_back(std::string(method));
//
//    Py_INCREF(Py_None);
//    return Py_None;
//}
//
/////\brief Return Python list of entities that match a given Filter
//Py::Object CyPy_MemMap::find_by_filter(const Py::Tuple& args){
//#ifndef NDEBUG
//    if (self->m_map == nullptr) {
//        PyErr_SetString(PyExc_AssertionError, "nullptr MemMap in MemMap.find_by_filter");
//        return nullptr;
//    }
//#endif // NDEBUG
//
//    EntityVector res;
//    if(!PyFilter_Check(filter)){
//        return nullptr;
//    }
//    PyFilter* f = (PyFilter*)filter;
//
//    auto iter_begin = self->m_map->getEntities().begin();
//    auto iter_end = self->m_map->getEntities().end();
//    for (; iter_begin != iter_end; ++iter_begin){
//        if (f->m_filter->match(*iter_begin->second)){
//            res.push_back(iter_begin->second);
//        }
//    }
//    PyObject * list = PyList_New(res.size());
//    if (list == nullptr) {
//        return nullptr;
//    }
//    EntityVector::const_iterator Iend = res.end();
//    int i = 0;
//    for (EntityVector::const_iterator I = res.begin(); I != Iend; ++I, ++i) {
//        PyObject * thing = wrapEntity(*I);
//        if (thing == nullptr) {
//            Py_DECREF(list);
//            return nullptr;
//        }
//        PyList_SetItem(list, i, thing);
//    }
//    return list;
//}
//
/////\brief find entities using a query in a specified location
//Py::Object CyPy_MemMap::find_by_location_query(const Py::Tuple& args){
//#ifndef NDEBUG
//    if (self->m_map == nullptr) {
//        PyErr_SetString(PyExc_AssertionError, "nullptr MemMap in MemMap.find_by_location_query");
//        return nullptr;
//    }
//#endif // NDEBUG
//
//    PyObject * where_obj;
//    double radius;
//    PyObject* filter;
//    if (!PyArg_ParseTuple(args, "OdO", &where_obj, &radius, &filter)) {
//        return nullptr;
//    }
//
//    if (!PyFilter_Check(filter)) {
//        return nullptr;
//    }
//    PyFilter* f = (PyFilter*)filter;
//
//
//    if (!PyLocation_Check(where_obj)) {
//        throw Py::TypeError("Argument must be a location");
//        return nullptr;
//    }
//    PyLocation * where = (PyLocation *)where_obj;
//    if (!where->location->isValid()) {
//        PyErr_SetString(PyExc_RuntimeError, "Location is incomplete");
//        return nullptr;
//    }
//
//    //Create a vector and fill it with entities that match the given filter and are in range
//    float square_range = radius * radius;
//    EntityVector res;
//    LocatedEntity* place = where->location->m_loc;
//    if (place != 0) {
//
//        auto iter = place->m_contains->begin();
//        auto iter_end = place->m_contains->end();
//
//        for (; iter != iter_end; ++iter) {
//            LocatedEntity* item = *iter;
//            if (item == 0) {
//                continue;
//            }
//            if (!item->isVisible() || !f->m_filter->match(*item)) {
//                continue;
//            }
//            if (squareDistance(where->location->pos(), item->m_location.pos()) < square_range) {
//                res.push_back(item);
//            }
//        }
//    }
//
//    //Create a python list an fill it with the entities we got
//    PyObject * list = PyList_New(res.size());
//    if (list == nullptr) {
//        return nullptr;
//    }
//    EntityVector::const_iterator Iend = res.end();
//    int i = 0;
//    for (EntityVector::const_iterator I = res.begin(); I != Iend; ++I, ++i) {
//        PyObject * thing = wrapEntity(*I);
//        if (thing == nullptr) {
//            Py_DECREF(list);
//            return nullptr;
//        }
//        PyList_SetItem(list, i, thing);
//    }
//    return list;
//}
//
//PyObject* MemMap_add_entity_memory(const Py::Tuple& args){
//#ifndef NDEBUG
//    if (self->m_map == nullptr) {
//        PyErr_SetString(PyExc_AssertionError, "nullptr MemMap in MemMap.add_entity_memory");
//        return nullptr;
//    }
//#endif // NDEBUG
//
//    char *id, *memory_name;
//    PyObject *val;
//    if (!PyArg_ParseTuple(args, "ssO", &id, &memory_name, &val)) {
//        return nullptr;
//    }
//
//    Atlas::Message::Element element_val;
//    PyObject_asMessageElement(val, element_val, false);
//
//    self->m_map->addEntityMemory(id, memory_name, element_val);
//
//    return Py_None;
//}
//
//PyObject* MemMap_recall_entity_memory(const Py::Tuple& args){
//#ifndef NDEBUG
//    if (self->m_map == nullptr) {
//        PyErr_SetString(PyExc_AssertionError, "nullptr MemMap in MemMap.recall_entity_memory");
//        return nullptr;
//    }
//#endif // NDEBUG
//
//    char *id, *memory_name;
//    if (!PyArg_ParseTuple(args, "ss", &id, &memory_name)) {
//        return nullptr;
//    }
//
//    Atlas::Message::Element element_val;
//    self->m_map->recallEntityMemory(std::string(id), std::string(memory_name), element_val);
//    PyObject* ret = MessageElement_asPyObject(element_val);
//
//    if (ret) {
//        return ret;
//    } else {
//        return nullptr;
//    }
//
//}
//
//static PyMethodDef MemMap_methods[] = {
//    {"find_by_location",    (PyCFunction)MemMap_find_by_location,    METH_VARARGS},
//    {"find_by_type",        (PyCFunction)MemMap_find_by_type,        METH_O},
//    {"find_by_filter",      (PyCFunction)MemMap_find_by_filter,      METH_O},
//    {"find_by_location_query",(PyCFunction)MemMap_find_by_location_query,  METH_VARARGS},
//    {"add_entity_memory",   (PyCFunction)MemMap_add_entity_memory,   METH_VARARGS},
//    {"recall_entity_memory",(PyCFunction)MemMap_recall_entity_memory,METH_VARARGS},
//    {"add",                 (PyCFunction)MemMap_updateAdd,           METH_VARARGS},
//    {"delete",              (PyCFunction)MemMap_delete,              METH_O},
//    {"get",                 (PyCFunction)MemMap_get,                 METH_O},
//    {"get_add",             (PyCFunction)MemMap_get_add,             METH_O},
//    {"update",              (PyCFunction)MemMap_updateAdd,           METH_VARARGS},
//    {"add_hooks_append",    (PyCFunction)MemMap_add_hooks_append,    METH_O},
//    {"update_hooks_append", (PyCFunction)MemMap_update_hooks_append, METH_O},
//    {"delete_hooks_append", (PyCFunction)MemMap_delete_hooks_append, METH_O},
//    {nullptr,                  nullptr}           // sentinel
//};
