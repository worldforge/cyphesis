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

#include "TypeResolver.h"
#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>
#include <common/log.h>
#include <common/compose.hpp>


const TypeStore& TypeResolver::getTypeStore() const
{
    return m_typeStore;
}


std::set<const TypeNode*> TypeResolver::InfoOperation(const Operation& op, OpVector& res)
{
    if (!op->getArgs().empty()) {
        auto& arg = op->getArgs().front();
        if (!arg->isDefaultObjtype()) {
            auto& objType = arg->getObjtype();
            if ((objType == "meta") ||
                (objType == "class") ||
                (objType == "op_definition") ||
                (objType == "archetype")) {
                return processTypeData(arg, res);
            }
        }
    }
    return {};
}

std::set<const TypeNode*> TypeResolver::processTypeData(const Atlas::Objects::Root& data, OpVector& res)
{
    if (!data->isDefaultId()) {
        auto& id = data->getId();
        if (!data->isDefaultParent()) {
            auto& parent = data->getParent();
            auto parentType = m_typeStore.getType(parent);
            if (!parentType) {
                m_pendingTypes[id].ent = data;
                requestType(parent, res);
                m_pendingTypes[parent].childTypes.insert(id);
            } else {
                //Parent was resolved, now we also are resolved
                return resolveType(id, data, res);
            }
        } else {
            //We got a top level entity, resolve it.
            return resolveType(id, data, res);
        }
    }
    return {};
}

const TypeNode* TypeResolver::requestType(const std::string& id, OpVector& res)
{
    auto type = m_typeStore.getType(id);
    if (type) {
        return type;
    }

    auto I = m_pendingTypes.find(id);
    if (I == m_pendingTypes.end()) {
        Atlas::Objects::Entity::Anonymous what;
        what->setId(id);

        Atlas::Objects::Operation::Get get;
        get->setArgs1(what);
        if (m_typeProviderId) {
            get->setFrom(*m_typeProviderId);
        }

        res.emplace_back(std::move(get));

        m_pendingTypes[id] = {};
    }
    return nullptr;
}

std::set<const TypeNode*> TypeResolver::resolveType(const std::string& id, const Atlas::Objects::Root& ent, OpVector& res)
{
    std::set<const TypeNode*> resolved;

    auto typeNode = m_typeStore.getType(id);
    if (!typeNode) {
        typeNode = m_typeStore.addChild(ent);
    }
    if (typeNode) {
        resolved.insert(typeNode);
        auto I = m_pendingTypes.find(id);
        if (I != m_pendingTypes.end()) {


            for (auto& child : I->second.childTypes) {
                auto childI = m_pendingTypes.find(child);
                if (childI != m_pendingTypes.end()) {
                    if (childI->second.ent) {
                        auto resolvedChildren = resolveType(child, childI->second.ent, res);
                        resolved.insert(resolvedChildren.begin(), resolvedChildren.end());
                    }
                }
            }

            m_pendingTypes.erase(I);
        }
    }
    return resolved;
}

TypeResolver::TypeResolver(TypeStore& typeStore)
    : m_typeStore(typeStore)
{

}
