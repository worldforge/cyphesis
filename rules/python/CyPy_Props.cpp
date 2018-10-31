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

#include "CyPy_Props.h"
#include "CyPy_Element.h"
#include "CyPy_Axisbox.h"

CyPy_Props::CyPy_Props(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
    : WrapperBase(self, args, kwds)
{

}


CyPy_Props::CyPy_Props(Py::PythonClassInstance* self, Ref<LocatedEntity> value)
    : WrapperBase(self, std::move(value))
{

}


void CyPy_Props::init_type()
{
    behaviors().name("Properties");
    behaviors().doc("");

    behaviors().supportMappingType(Py::PythonType::support_mapping_subscript);
    behaviors().readyType();
}

Py::Object CyPy_Props::getattro(const Py::String& name)
{
    auto nameStr = name.as_string();

//    if (nameStr == "bbox") {
//        auto bboxProperty = m_value->getPropertyClassFixed<BBoxProperty>();
//        if (bboxProperty) {
//            return CyPy_Axisbox::wrap(bboxProperty->data());
//        }
//        return Py::None();
//    }

    auto prop = m_value->getProperty(nameStr);
    if (prop) {
//        if (nameStr == "terrain") {
//            return CyPy_TerrainProperty::wrap(m_value);
//        } else {


            //Check if it's a special prop
//        if (dynamic_cast<const StatisticsProperty*>(prop) || dynamic_cast<const TerrainProperty*>(prop)
//            || dynamic_cast<const TerrainModProperty*>(prop)) {
//            PyObject* ret = Property_asPyObject(prop, locatedEntity);
//            if (ret) {
//                return ret;
//            }
//        } else {
            Atlas::Message::Element element;
            // If this property is not set with a value, return none.
            if (prop->get(element) == 0) {
                if (element.isNone()) {
                    return Py::None();
                } else {
                    return CyPy_Element::wrap(element);
                }
            }
//        }
    }
    return Py::None();
}

int CyPy_Props::setattro(const Py::String& name, const Py::Object& attr)
{
    auto nameStr = name.as_string();


    Atlas::Message::Element obj = CyPy_Element::asElement(attr);
    m_value->setAttr(name, obj);
    return 0;
}

Py::Object CyPy_Props::mapping_subscript(const Py::Object& key)
{
    if (key.isString()) {
        return getattro(Py::String(key));
    }
    return Py::None();
}
