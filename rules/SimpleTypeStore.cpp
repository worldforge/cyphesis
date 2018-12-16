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
    const std::string& child = obj->getId();
    std::string parent;
    if (!obj->isDefaultParent()) {
        parent = obj->getParent();
    }
    auto I = m_types.find(child);
    auto Iend = m_types.end();
    if (I != Iend) {

        const TypeNode* existingParent = I->second->parent();
        log(ERROR, String::compose("Installing %1 \"%2\"(parent \"%3\") "
                                   "which was already installed as a %4 with parent \"%5\"",
                                   obj->getObjtype(), child, parent,
                                   I->second->description(Visibility::PRIVATE)->getObjtype(),
                                   existingParent ? existingParent->name() : "NON"));
        return nullptr;
    }
    auto type = std::make_unique<TypeNode>(child, obj);
    if (!parent.empty()) {
        I = m_types.find(parent);
        if (I == Iend) {
            log(ERROR, String::compose("Installing %1 \"%2\" "
                                       "which has unknown parent \"%3\".",
                                       obj->getObjtype(), child, parent));;
            return nullptr;
        }
        Atlas::Message::Element children(Atlas::Message::ListType(1, child));

        auto description = I->second->description(Visibility::PRIVATE);

        if (description->copyAttr("children", children) == 0) {
            assert(children.isList());
            children.asList().push_back(child);
        }
        description->setAttr("children", children);
        I->second->setDescription(description);

        type->setParent(I->second.get());
    }


    auto result = m_types.insert(std::make_pair(child, std::move(type)));
    return result.first->second.get();
}

SimpleTypeStore::SimpleTypeStore() = default;
