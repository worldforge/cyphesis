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

#ifndef CYPHESIS_CYPY_LOCATEDENTITY_IMPL_H
#define CYPHESIS_CYPY_LOCATEDENTITY_IMPL_H

#include "rules/PhysicalProperties.h"
#include <rules/BBoxProperty.h>
#include "CyPy_LocatedEntity.h"
#include "CyPy_Point3D.h"
#include "CyPy_Operation.h"
#include "CyPy_EntityLocation.h"
#include "CyPy_Props.h"
#include "CyPy_Location.h"
#include "CyPy_Element.h"
#include "common/Inheritance.h"

template<typename TValue, typename TPythonClass>
CyPy_LocatedEntityBase<TValue, TPythonClass>::CyPy_LocatedEntityBase(Py::PythonClassInstanceWeak* self, const Py::Tuple& args, const Py::Dict& kwds)
        : WrapperBase<TValue, TPythonClass, Py::PythonClassInstanceWeak>::WrapperBase(self, args, kwds)
{
}

template<typename TValue, typename TPythonClass>
CyPy_LocatedEntityBase<TValue, TPythonClass>::CyPy_LocatedEntityBase(Py::PythonClassInstanceWeak* self, TValue value)
        : WrapperBase<TValue, TPythonClass, Py::PythonClassInstanceWeak>::WrapperBase(self, std::move(value))
{
    if (!this->m_value) {
        log(WARNING, "Created a Python Entity wrapper with null entity, this should not happen.");
    }
}


template<typename TValue, typename TPythonClass>
CyPy_LocatedEntityBase<TValue, TPythonClass>::~CyPy_LocatedEntityBase() = default;


template<typename TValue, typename TPythonClass>
Py::Object CyPy_LocatedEntityBase<TValue, TPythonClass>::getattro(const Py::String& name)
{
    auto nameStr = name.as_string();
    if (nameStr == "id") {
        return Py::String(this->m_value->getId());
    }

    if (nameStr == "parent") {
        if (this->m_value->m_parent) {
            return TPythonClass::wrap(this->m_value->m_parent);
        } else {
            return Py::None();
        }
    }

    if (nameStr == "props") {
        return CyPy_Props::wrap(this->m_value);
    }

    if (nameStr == "type") {
        if (!this->m_value->getType()) {
            return Py::String("");
        }
        return Py::String(this->m_value->getType()->name());
    }


    //TODO: Perhaps move only to MemEntity? Or remove altogether?
    if (nameStr == "location") {
        Location location;
        if (this->m_value->m_parent != nullptr) {
            location.m_parent = this->m_value->m_parent;
        }
        if (auto prop = this->m_value->template getPropertyClassFixed<PositionProperty>()) {
            location.m_pos = prop->data();
        }
        if (auto prop = this->m_value->template getPropertyClassFixed<VelocityProperty>()) {
            location.m_velocity = prop->data();
        }
        if (auto prop = this->m_value->template getPropertyClassFixed<OrientationProperty>()) {
            location.m_orientation = prop->data();
        }
        if (auto prop = this->m_value->template getPropertyClassFixed<AngularVelocityProperty>()) {
            location.m_angularVelocity = prop->data();
        }
        if (auto prop = this->m_value->template getPropertyClassFixed<BBoxProperty>()) {
            location.setBBox(prop->data());
        }

        return CyPy_Location::wrap(location);
    }

    if (nameStr == "contains") {
        Py::List list;
        if (this->m_value->m_contains) {
            for (auto& child : *this->m_value->m_contains) {
                list.append(CyPy_LocatedEntity::wrap(child));
            }
        }
        return list;
    }

    if (nameStr == "visible") {
        return Py::Boolean(this->m_value->isVisible());
    }

    if (nameStr == "is_destroyed") {
        return Py::Boolean(this->m_value->isDestroyed());
    }

    return Py::PythonExtensionBase::getattro(name);
}

template<typename TValue, typename TPythonClass>
int CyPy_LocatedEntityBase<TValue, TPythonClass>::setattro(const Py::String& name, const Py::Object& attr)
{
    auto nameStr = name.as_string();

    if (nameStr == "type") {
        if (this->m_value->getType()) {
            throw Py::RuntimeError("Cannot mutate entity type");
        }

        const TypeNode* type = Inheritance::instance().getType(verifyString(attr));
        if (!type) {
            throw Py::ValueError("Entity type unknown");
        }
        this->m_value->setType(type);
        return 0;
    }
    return Py::PythonExtensionBase::setattro(name, attr);
}

template<typename TValue, typename TPythonClass>
Py::Object CyPy_LocatedEntityBase<TValue, TPythonClass>::rich_compare(const Py::Object& other, int op)
{

    if (CyPy_LocatedEntity::check(other)) {
        auto otherEntity = CyPy_LocatedEntity::value(other);
        if (op == Py_EQ) {
            return (this->m_value == otherEntity) ? Py::True() : Py::False();
        } else if (op == Py_NE) {
            return (this->m_value != otherEntity) ? Py::True() : Py::False();
        }

    } else {
        if (op == Py_EQ) {
            return Py::False();
        } else if (op == Py_NE) {
            return Py::True();
        }
    }

    throw Py::NotImplementedError("Not implemented");

}

template<typename TValue, typename TPythonClass>
Py::Object CyPy_LocatedEntityBase<TValue, TPythonClass>::get_child(const Py::Tuple& args)
{
    args.verify_length(1);
    auto child_id = verifyString(args.front());
    if (this->m_value->m_contains) {
        for (auto& child : *this->m_value->m_contains) {
            if (child_id == child->getId()) {
                return CyPy_LocatedEntity::wrap(child);
            }
        }
    }
    return Py::None();
}


template<typename TValue, typename TPythonClass>
Py::Object CyPy_LocatedEntityBase<TValue, TPythonClass>::as_entity()
{
    Atlas::Message::MapType element;
    this->m_value->addToMessage(element);
    return CyPy_Element::wrap(element);
}

template<typename TValue, typename TPythonClass>
Py::Object CyPy_LocatedEntityBase<TValue, TPythonClass>::can_reach(const Py::Tuple& args)
{

    args.verify_length(1, 2);
    float extraReach = 0;
    if (args.length() == 2) {
        extraReach = verifyNumeric(args[1]);
    }

    return Py::Boolean(this->m_value->canReach(verifyObject<CyPy_EntityLocation>(args.front()), extraReach));
}

template<typename TValue, typename TPythonClass>
Py::Object CyPy_LocatedEntityBase<TValue, TPythonClass>::describe_entity()
{
    return Py::String(this->m_value->describeEntity());
}


template<typename TValue, typename TPythonClass>
Py::Object CyPy_LocatedEntityBase<TValue, TPythonClass>::client_error(const Py::Tuple& args)
{
    args.verify_length(2);

    OpVector res;
    this->m_value->clientError(verifyObject<CyPy_Operation>(args[0]), verifyString(args[1]), res, this->m_value->getId());
    if (!res.empty()) {
        return CyPy_Operation::wrap(res.front());
    }
    return Py::None();
}


template<typename TValue, typename TPythonClass>
Py::Object CyPy_LocatedEntityBase<TValue, TPythonClass>::is_type(const Py::Tuple& args)
{
    args.verify_length(1);

    const TypeNode* type = this->m_value->getType();
    if (!type) {
        return Py::False();
    }
    return Py::Boolean(type->isTypeOf(verifyString(args[0])));
}


template<typename TValue, typename TPythonClass>
Py::Object CyPy_LocatedEntityBase<TValue, TPythonClass>::getPropertyFromFirstArg(const Py::Tuple& args, const std::function<Py::Object(const Atlas::Message::Element&, Py::Object)>& checkFn) const
{
    args.verify_length(1, 2);
    Py::Object defaultValue = Py::None();
    if (args.length() > 1) {
        defaultValue = args.getItem(1);
    }
    const PropertyBase* prop = this->m_value->getProperty(verifyString(args.front()));
    if (prop) {
        Atlas::Message::Element element;
        prop->get(element);

        return checkFn(element, defaultValue);
    }

    return defaultValue;
}

template<typename TValue, typename TPythonClass>
Py::Object CyPy_LocatedEntityBase<TValue, TPythonClass>::hasPropertyFromFirstArg(const Py::Tuple& args, const std::function<bool(const Atlas::Message::Element&)>& checkFn) const
{

    args.verify_length(1);
    const PropertyBase* prop = this->m_value->getProperty(verifyString(args.front()));
    if (prop) {
        Atlas::Message::Element element;
        prop->get(element);
        return Py::Boolean(checkFn(element));
    }

    return Py::False();
}


template<typename TValue, typename TPythonClass>
Py::Object CyPy_LocatedEntityBase<TValue, TPythonClass>::get_prop_num(const Py::Tuple& args)
{
    return this->getPropertyFromFirstArg(args, [](const Atlas::Message::Element& element, Py::Object defaultValue) -> Py::Object {
        if (element.isInt()) {
            return Py::Long(element.Int());
        } else if (element.isFloat()) {
            return Py::Float(element.Float());
        } else {
            return defaultValue;
        }
    });
}

template<typename TValue, typename TPythonClass>
Py::Object CyPy_LocatedEntityBase<TValue, TPythonClass>::has_prop_num(const Py::Tuple& args)
{
    return this->hasPropertyFromFirstArg(args, [](const Atlas::Message::Element& element) {
        return element.isNum();
    });
}


template<typename TValue, typename TPythonClass>
Py::Object CyPy_LocatedEntityBase<TValue, TPythonClass>::get_prop_float(const Py::Tuple& args)
{
    return this->getPropertyFromFirstArg(args, [](const Atlas::Message::Element& element, Py::Object defaultValue) -> Py::Object {
        if (element.isFloat()) {
            return Py::Float(element.Float());
        } else {
            return defaultValue;
        }
    });
}

template<typename TValue, typename TPythonClass>
Py::Object CyPy_LocatedEntityBase<TValue, TPythonClass>::has_prop_float(const Py::Tuple& args)
{
    return this->hasPropertyFromFirstArg(args, [](const Atlas::Message::Element& element) {
        return element.isFloat();
    });
}


template<typename TValue, typename TPythonClass>
Py::Object CyPy_LocatedEntityBase<TValue, TPythonClass>::get_prop_int(const Py::Tuple& args)
{
    return this->getPropertyFromFirstArg(args, [](const Atlas::Message::Element& element, Py::Object defaultValue) -> Py::Object {
        if (element.isInt()) {
            return Py::Long(element.Int());
        } else {
            return defaultValue;
        }
    });
}

template<typename TValue, typename TPythonClass>
Py::Object CyPy_LocatedEntityBase<TValue, TPythonClass>::has_prop_int(const Py::Tuple& args)
{
    return this->hasPropertyFromFirstArg(args, [](const Atlas::Message::Element& element) {
        return element.isInt();
    });
}


template<typename TValue, typename TPythonClass>
Py::Object CyPy_LocatedEntityBase<TValue, TPythonClass>::get_prop_string(const Py::Tuple& args)
{
    return this->getPropertyFromFirstArg(args, [](const Atlas::Message::Element& element, Py::Object defaultValue) -> Py::Object {
        if (element.isString()) {
            return Py::String(element.String());
        } else {
            return defaultValue;
        }
    });
}

template<typename TValue, typename TPythonClass>
Py::Object CyPy_LocatedEntityBase<TValue, TPythonClass>::has_prop_string(const Py::Tuple& args)
{
    return this->hasPropertyFromFirstArg(args, [](const Atlas::Message::Element& element) {
        return element.isString();
    });
}


template<typename TValue, typename TPythonClass>
Py::Object CyPy_LocatedEntityBase<TValue, TPythonClass>::get_prop_bool(const Py::Tuple& args)
{
    return this->getPropertyFromFirstArg(args, [](const Atlas::Message::Element& element, Py::Object defaultValue) -> Py::Object {
        if (element.isInt()) {
            return Py::Boolean(element.Int() != 0);
        } else {
            return defaultValue;
        }
    });
}

template<typename TValue, typename TPythonClass>
Py::Object CyPy_LocatedEntityBase<TValue, TPythonClass>::has_prop_bool(const Py::Tuple& args)
{
    return this->has_prop_int(args);
}


template<typename TValue, typename TPythonClass>
Py::Object CyPy_LocatedEntityBase<TValue, TPythonClass>::get_prop_map(const Py::Tuple& args)
{
    return this->getPropertyFromFirstArg(args, [](const Atlas::Message::Element& element, Py::Object defaultValue) -> Py::Object {
        if (element.isMap()) {
            return CyPy_ElementMap::wrap(element.Map());
//            return Py::Dict(element.Map());
        } else {
            return defaultValue;
        }
    });
}

template<typename TValue, typename TPythonClass>
Py::Object CyPy_LocatedEntityBase<TValue, TPythonClass>::has_prop_map(const Py::Tuple& args)
{
    return this->hasPropertyFromFirstArg(args, [](const Atlas::Message::Element& element) {
        return element.isMap();
    });
}


template<typename TValue, typename TPythonClass>
Py::Object CyPy_LocatedEntityBase<TValue, TPythonClass>::get_prop_list(const Py::Tuple& args)
{
    return this->getPropertyFromFirstArg(args, [](const Atlas::Message::Element& element, Py::Object defaultValue) -> Py::Object {
        if (element.isList()) {
            return CyPy_ElementList::wrap(element.List());
//            return Py::Dict(element.Map());
        } else {
            return defaultValue;
        }
    });
}

template<typename TValue, typename TPythonClass>
Py::Object CyPy_LocatedEntityBase<TValue, TPythonClass>::has_prop_list(const Py::Tuple& args)
{
    return this->hasPropertyFromFirstArg(args, [](const Atlas::Message::Element& element) {
        return element.isList();
    });
}

template<typename TValue, typename TPythonClass>
Py::Object CyPy_LocatedEntityBase<TValue, TPythonClass>::wrap(LocatedEntity* value)
{
    return wrapLocatedEntity(value);
}

template<typename TValue, typename TPythonClass>
Py::Object CyPy_LocatedEntityBase<TValue, TPythonClass>::str()
{
    return describe_entity();
}

#endif //CYPHESIS_CYPY_LOCATEDENTITY_IMPL_H
