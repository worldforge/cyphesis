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


#ifndef SERVER_ENTITY_RULE_HANDLER_H
#define SERVER_ENTITY_RULE_HANDLER_H

#include "RuleHandler.h"
#include <string>
#include <map>
#include <functional>

class EntityBuilder;
class EntityKit;
class EntityFactoryBase;

/// \brief Handle processing and updating of entity type rules
class EntityRuleHandler : public RuleHandler {
  protected:
    EntityBuilder * const m_builder;

    int populateEntityFactory(const std::string & class_name,
                              EntityFactoryBase * factory,
                              const Atlas::Message::MapType & class_desc,
                              std::string & dependent,
                              std::string & reason);

    int installEntityClass(const std::string & class_name,
                           const std::string & parent,
                           const Atlas::Objects::Root & class_desc,
                           std::string & dependent,
                           std::string & reason,
                           std::map<const TypeNode*, TypeNode::PropertiesUpdate>& changes);

    int installEntityClass(const std::string & class_name,
                           const std::string & parent,
                           const Atlas::Objects::Root & class_desc,
                           std::string & dependent,
                           std::string & reason,
                           EntityFactoryBase* factory,
                           std::map<const TypeNode*, TypeNode::PropertiesUpdate>& changes);

    int modifyEntityClass(const std::string & class_name,
                          const Atlas::Objects::Root & class_desc,
                          std::map<const TypeNode*, TypeNode::PropertiesUpdate>& changes);

    std::map<std::string, std::function<EntityFactoryBase*(EntityFactoryBase*)>> mFactories;

  public:
    explicit EntityRuleHandler(EntityBuilder * eb);

    int check(const Atlas::Objects::Root & desc) override;
    int install(const std::string &,
                        const std::string &,
                        const Atlas::Objects::Root & desc,
                        std::string &,
                        std::string &,
                        std::map<const TypeNode*, TypeNode::PropertiesUpdate>& changes) override;
    int update(const std::string &,
                    const Atlas::Objects::Root & desc,
                    std::map<const TypeNode*, TypeNode::PropertiesUpdate>& changes) override;


};

#endif // SERVER_ENTITY_RULE_HANDLER_H
