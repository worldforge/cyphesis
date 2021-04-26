#include <memory>

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

#include "SimpleTypeStore.h"
#include "common/log.h"
#include "common/TypeNode.h"
#include "common/compose.hpp"

#include <Atlas/Objects/Entity.h>
#include <Atlas/Objects/Factories.h>
#include <common/Property.h>

const TypeNode* SimpleTypeStore::getType(const std::string& parent) const
{
    auto I = m_types.find(parent);
    if (I == m_types.end()) {
        return nullptr;
    }
    return I->second.get();
}


TypeNode* SimpleTypeStore::addChild(const Atlas::Objects::Root& obj)
{
    assert(obj.isValid());
    TypeNode* parentNode = nullptr;

    const std::string& child = obj->getId();
    std::string parent;
    if (!obj->isDefaultParent()) {
        parent = obj->getParent();
    }

    if (!parent.empty()) {
        auto I = m_types.find(parent);
        if (I == m_types.end()) {
            log(ERROR, String::compose("Installing %1 \"%2\" "
                                       "which has unknown parent \"%3\".",
                                       obj->getObjtype(), child, parent));
            return nullptr;
        }
        parentNode = I->second.get();
    }

    auto I = m_types.find(child);
    if (I != m_types.end()) {

        const TypeNode* existingParent = I->second->parent();
        log(ERROR, String::compose("Installing %1 \"%2\"(parent \"%3\") "
                                   "which was already installed as a %4 with parent \"%5\"",
                                   obj->getObjtype(), child, parent,
                                   I->second->description(Visibility::PRIVATE)->getObjtype(),
                                   existingParent ? existingParent->name() : "NON"));
        return nullptr;
    }
    auto type = std::make_unique<TypeNode>(child, obj);

    //First add all properties from parent type.
    if (parentNode) {
        for (auto& entry : parentNode->defaults()) {
            type->injectProperty(entry.first, std::unique_ptr<PropertyBase>(entry.second->copy()));
        }
    }

    readAttributesIntoType(*type, obj);

    if (parentNode) {

        Atlas::Message::Element children(Atlas::Message::ListType(1, child));

        auto description = parentNode->description(Visibility::PRIVATE);

        if (description->copyAttr("children", children) == 0) {
            assert(children.isList());
            children.asList().push_back(child);
        }
        description->setAttr("children", children);
        parentNode->setDescription(description);

        type->setParent(parentNode);
    }


    auto result = m_types.emplace(child, std::move(type));
    return result.first->second.get();
}

void SimpleTypeStore::readAttributesIntoType(TypeNode& type, const Atlas::Objects::Root& obj)
{
    // Store the default attribute for entities create by this rule.
    if (obj->hasAttr("properties")) {
        auto propertiesElement = obj->getAttr("properties");
        if (propertiesElement.isMap()) {
            type.addProperties(propertiesElement.Map(), m_propertyManager);
        }
    }

}

size_t SimpleTypeStore::getTypeCount() const
{
    return m_types.size();
}

SimpleTypeStore::SimpleTypeStore(const PropertyManager& propertyManager)
        : m_propertyManager(propertyManager),
          m_factories(new Atlas::Objects::Factories())
{

}

Atlas::Objects::Factories& SimpleTypeStore::getFactories()
{
    return *m_factories;
}

const Atlas::Objects::Factories& SimpleTypeStore::getFactories() const
{
    return *m_factories;
}
