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

#include "common/Inheritance.h"
#include "rules/entityfilter/ProviderFactory.h"
#include "UsageInstance.h"
#include "rules/LocatedEntity.h"
#include "rules/simulation/BaseWorld.h"
#include "modules/Variant.h"
#include "common/AtlasQuery.h"

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::Action;
using Atlas::Objects::Operation::Use;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;

std::function<Py::Object(UsageInstance&& usageInstance)> UsageInstance::scriptCreator;

UsageParameter UsageParameter::parse(const Atlas::Message::Element& element)
{
    UsageParameter parameter{};

    if (!element.isMap()) {
        throw std::invalid_argument("Parameter must be a map.");
    }
    auto& paramMap = element.Map();

    auto I = paramMap.find("type");
    if (I == paramMap.end() || !I->second.isString()) {
        throw std::invalid_argument("Parameter must define a string 'type'.");
    }
    if (I->second.String() == "entity") {
        parameter.type = UsageParameter::Type::ENTITY;
    } else if (I->second.String() == "entity_location") {
        parameter.type = UsageParameter::Type::ENTITYLOCATION;
    } else if (I->second.String() == "direction") {
        parameter.type = UsageParameter::Type::DIRECTION;
    } else if (I->second.String() == "position") {
        parameter.type = UsageParameter::Type::POSITION;
    } else {
        throw std::invalid_argument(String::compose("Parameter type not recognized: %1.", I->second.String()));
    }


    AtlasQuery::find<std::string>(paramMap, "constraint", [&](const std::string& constraint) {
        //TODO: should be a usage constraint provider factory
        parameter.constraint.reset(new EntityFilter::Filter(constraint, EntityFilter::ProviderFactory()));
    });
    AtlasQuery::find<Atlas::Message::IntType>(paramMap, "min", [&](const Atlas::Message::IntType& min) {
        parameter.min = static_cast<int>(min);
    });
    AtlasQuery::find<Atlas::Message::IntType>(paramMap, "max", [&](const Atlas::Message::IntType& max) {
        parameter.max = static_cast<int>(max);
    });

    return parameter;
}

int UsageParameter::countValidArgs(const std::vector<UsageArg>& args, const Ref<LocatedEntity>& actor, const Ref<LocatedEntity>& tool, std::vector<std::string>& errorMessages) const
{
    int count = 0;
    for (auto& arg : args) {
        bool is_valid = false;

        switch (type) {
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
                                if (constraint) {
                                    EntityFilter::QueryContext queryContext{*value.m_parent, actor.get(), tool.get()};
                                    queryContext.entityLoc.pos = &value.m_pos;
                                    queryContext.entity_lookup_fn = [](const std::string& id) { return BaseWorld::instance().getEntity(id); };
                                    queryContext.type_lookup_fn = [](const std::string& id) { return Inheritance::instance().getType(id); };
                                    queryContext.report_error_fn = [&errorMessages](const std::string& message) { errorMessages.emplace_back(message); };
                                    is_valid = constraint->match(queryContext);
                                } else {
                                    is_valid = true;
                                }
                            } else {
                                is_valid = false;
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
                                if (constraint) {
                                    EntityFilter::QueryContext queryContext{*value.m_parent, actor.get(), tool.get()};
                                    queryContext.entityLoc.pos = &value.m_pos;
                                    queryContext.entity_lookup_fn = [](const std::string& id) { return BaseWorld::instance().getEntity(id); };
                                    queryContext.type_lookup_fn = [](const std::string& id) { return Inheritance::instance().getType(id); };
                                    queryContext.report_error_fn = [&errorMessages](const std::string& message) { errorMessages.emplace_back(message); };
                                    is_valid = constraint->match(queryContext);
                                } else {
                                    is_valid = true;
                                }
                            } else {
                                is_valid = false;
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
    return count;
}

std::pair<bool, std::string> UsageInstance::isValid() const
{

    if (definition.constraint) {
        std::vector<std::string> errors;
        EntityFilter::QueryContext queryContext{*tool, actor.get(), tool.get()};
        queryContext.report_error_fn = [&](const std::string& error) { errors.push_back(error); };
        queryContext.entity_lookup_fn = [](const std::string& id) { return BaseWorld::instance().getEntity(id); };
        queryContext.type_lookup_fn = [](const std::string& id) { return Inheritance::instance().getType(id); };

        if (!definition.constraint->match(queryContext)) {
            return {false, errors.empty() ? "Constraint does not match." : errors.front()};
        }
    }

    for (auto& param : definition.params) {
        auto I = args.find(param.first);
        if (I == args.end()) {
            return {false, String::compose("Could not find required '%1' argument.", param.first)};
        }
        std::vector<std::string> errorMessages;
        int count = param.second.countValidArgs(I->second, actor, tool, errorMessages);

        if (count < param.second.min) {
            if (!errorMessages.empty()) {
                return {false, *errorMessages.begin()};
            } else {
                return {false, String::compose("Too few '%1' arguments. Should be minimum %2, got %3.", param.first, param.second.min, count)};
            }
        }
        if (count > param.second.max) {
            return {false, String::compose("Too many '%1' arguments. Should be maximum %2, got %3.", param.first, param.second.max, count)};
        }

    }


    return {true, ""};
}
