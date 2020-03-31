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

#include <rules/BBoxProperty.h>
#include <rules/python/CyPy_Axisbox.h>
#include <rules/python/CyPy_Element.h>
#include "CyPy_EntityProps.h"
#include "CyPy_TerrainProperty.h"

CyPy_EntityProps::CyPy_EntityProps(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
    : WrapperBase(self, args, kwds)
{
    throw Py::TypeError("Can not create instances from Python.");
}


CyPy_EntityProps::CyPy_EntityProps(Py::PythonClassInstance* self, Ref<Entity> value)
    : WrapperBase(self, std::move(value))
{
}


void CyPy_EntityProps::init_type()
{
    behaviors().name("Entity Properties");
    behaviors().doc("");

    behaviors().supportMappingType(Py::PythonType::support_mapping_subscript);
    behaviors().readyType();
}

Py::Object CyPy_EntityProps::getattro(const Py::String& name)
{
    auto nameStr = name.as_string();

    if (nameStr == "bbox") {
        auto bboxProperty = m_value->getPropertyClassFixed<BBoxProperty>();
        if (bboxProperty) {
            return CyPy_Axisbox::wrap(bboxProperty->data());
        }
        return Py::None();
    }
    if (nameStr == "terrain") {
        return CyPy_TerrainProperty::wrap(m_value);
    }

    auto prop = m_value->getProperty(nameStr);
    if (prop) {

        Atlas::Message::Element element;
        // If this property is not set with a value, return none.
        if (prop->get(element) == 0) {
            if (element.isNone()) {
                return Py::None();
            } else {
                return CyPy_Element::wrap(element);
            }
        }
    }
    return Py::None();
}

int CyPy_EntityProps::setattro(const Py::String& name, const Py::Object& attr)
{
    Atlas::Message::Element obj = CyPy_Element::asElement(attr);
    m_value->setAttrValue(name, std::move(obj));
    return 0;
}

Py::Object CyPy_EntityProps::mapping_subscript(const Py::Object& key)
{
    if (key.isString()) {
        return getattro(Py::String(key));
    }
    return Py::None();
}
