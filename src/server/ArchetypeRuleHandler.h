// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2013 Erik Ogenvik
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


#ifndef SERVER_ARCHETYPE_RULE_HANDLER_H
#define SERVER_ARCHETYPE_RULE_HANDLER_H

#include "RuleHandler.h"

class EntityBuilder;

class EntityKit;

class ArchetypeFactory;

/// \brief Handle processing and updating of archtype rules.
///
/// An archetype is a rule which will create and populate a new entity of a
/// certain type
class ArchetypeRuleHandler : public RuleHandler
{
    protected:
        EntityBuilder& m_builder;
        const PropertyManager& m_propertyManager;

        int populateArchetypeFactory(const std::string& class_name,
                                     ArchetypeFactory* factory,
                                     const Atlas::Message::MapType& class_desc,
                                     std::string& dependent,
                                     std::string& reason);

        int installArchetypeClass(const std::string& class_name,
                                  const std::string& parent,
                                  const Atlas::Objects::Root& class_desc,
                                  std::string& dependent,
                                  std::string& reason);

        int modifyArchetypeClass(const std::string& class_name,
                                 const Atlas::Objects::Root& class_desc,
                                 std::map<const TypeNode*, TypeNode::PropertiesUpdate>& changes);

    public:
        explicit ArchetypeRuleHandler(EntityBuilder& eb, const PropertyManager& propertyManager)
                : m_builder(eb), m_propertyManager(propertyManager)
        {}

        int check(const Atlas::Objects::Root& desc) override;

        int install(const std::string& name,
                    const std::string& parent,
                    const Atlas::Objects::Root& description,
                    std::string& dependent,
                    std::string& reason,
                    std::map<const TypeNode*, TypeNode::PropertiesUpdate>& changes) override;

        int update(const std::string& name,
                   const Atlas::Objects::Root& desc,
                   std::map<const TypeNode*, TypeNode::PropertiesUpdate>& changes) override;
};

#endif // SERVER_ARCHETYPE_RULE_HANDLER_H
