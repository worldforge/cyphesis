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

#include <common/Inheritance.h>
#include "rules/entityfilter/Providers.h"
#include "UsageInstance.h"
#include "rules/LocatedEntity.h"
#include "rules/simulation/BaseWorld.h"
#include "modules/Variant.h"

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::Action;
using Atlas::Objects::Operation::Use;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;

std::function<Py::Object(UsageInstance&& usageInstance)> UsageInstance::scriptCreator;

std::pair<bool, std::string> UsageInstance::isValid() const
{

    if (definition.constraint) {
        EntityFilter::QueryContext queryContext{*tool, actor.get(), tool.get()};
        queryContext.entity_lookup_fn = [](const std::string& id) { return BaseWorld::instance().getEntity(id); };
        queryContext.type_lookup_fn = [](const std::string& id) { return Inheritance::instance().getType(id); };

        if (!definition.constraint->match(queryContext)) {
            return {false, "Constraint does not match."};
        }
    }

    for (auto& param : definition.params) {
        auto I = args.find(param.first);
        if (I == args.end()) {
            return {false, String::compose("Could not find required '%1' argument.", param.first)};
        }
        int count = 0;
        for (auto& arg : I->second) {
            bool is_valid = false;

            switch (param.second.type) {
                case UsageParameter::Type::DIRECTION: {

                    auto visitor = compose(
                        [&](const EntityLocation& value) {},
                        [&](const WFMath::Point<3>& value) {},
                        [&](const WFMath::Vector<3>& value) {
                            is_valid = value.isValid();
                        }
                    );

                    boost::apply_visitor(visitor, arg);

                    break;
                }
                case UsageParameter::Type::POSITION: {
                    auto visitor = compose(
                        [&](const EntityLocation& value) {},
                        [&](const WFMath::Point<3>& value) {
                            is_valid = value.isValid();
                        },
                        [&](const WFMath::Vector<3>& value) {}
                    );

                    boost::apply_visitor(visitor, arg);

                    break;
                }
                case UsageParameter::Type::ENTITY: {

                    auto visitor = compose(
                        [&](const EntityLocation& value) -> void {
                            if (value.m_parent && !value.m_parent->isDestroyed()) {
                                if (param.second.constraint) {
                                    EntityFilter::QueryContext queryContext{*value.m_parent, actor.get(), tool.get()};
                                    queryContext.entity_lookup_fn = [](const std::string& id) { return BaseWorld::instance().getEntity(id); };
                                    queryContext.type_lookup_fn = [](const std::string& id) { return Inheritance::instance().getType(id); };
                                    is_valid = param.second.constraint->match(queryContext);
                                } else {
                                    is_valid = true;
                                }
                            }
                        },
                        [&](const WFMath::Point<3>& value) {},
                        [&](const WFMath::Vector<3>& value) {}
                    );
                    boost::apply_visitor(visitor, arg);
                    break;
                }
                case UsageParameter::Type::ENTITYLOCATION: {
                    auto visitor = compose(
                        [&](const EntityLocation& value) -> void {
                            if (value.isValid() && !value.m_parent->isDestroyed()) {
                                if (param.second.constraint) {
                                    EntityFilter::QueryContext queryContext{*value.m_parent, actor.get(), tool.get()};
                                    queryContext.entity_lookup_fn = [](const std::string& id) { return BaseWorld::instance().getEntity(id); };
                                    queryContext.type_lookup_fn = [](const std::string& id) { return Inheritance::instance().getType(id); };
                                    is_valid = param.second.constraint->match(queryContext);
                                } else {
                                    is_valid = true;
                                }
                            }
                        },
                        [&](const WFMath::Point<3>& value) {},
                        [&](const WFMath::Vector<3>& value) {}
                    );
                    boost::apply_visitor(visitor, arg);
                }
            }
            if (is_valid) {
                count++;
            }
        }

        if (count < param.second.min) {
            return {false, String::compose("Too few '%1' arguments. Should be minimum %2, got %3.", param.first, param.second.min, count)};
        }
        if (count > param.second.max) {
            return {false, String::compose("Too many '%1' arguments. Should be maximum %2, got %3.", param.first, param.second.max, count)};
        }

    }


    return {true, ""};
}
