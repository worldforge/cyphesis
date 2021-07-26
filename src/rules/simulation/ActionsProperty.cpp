/*
 Copyright (C) 2020 Erik Ogenvik

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

#include "ActionsProperty.h"
#include "rules/LocatedEntity.h"
#include "common/operations/Tick.h"
#include "common/operations/Update.h"

#include <Atlas/Objects/Entity.h>

ActionsProperty::ActionsProperty()
        : PropertyBase(prop_flag_instance | prop_flag_persistence_ephem)
{

}

void ActionsProperty::install(LocatedEntity& entity, const std::string& name)
{
    entity.installDelegate(Atlas::Objects::Operation::TICK_NO, name);
}

void ActionsProperty::remove(LocatedEntity& entity, const std::string& name)
{
    entity.removeDelegate(Atlas::Objects::Operation::TICK_NO, name);
}

HandlerResult ActionsProperty::TickOperation(LocatedEntity& owner, const Operation& op, OpVector& res)
{
    m_tickOutstanding = boost::none;

    //Remove any actions that are at end_time.
    auto now = op->getSeconds();
    bool hadChanges = false;

    for (auto I = m_data.begin(); I != m_data.end();) {
        auto& entry = I->second;
        if (entry.endTime && *entry.endTime <= now) {
            I = m_data.erase(I);
            hadChanges = true;
        } else {
            ++I;
        }
    }

    if (hadChanges) {
        removeFlags(prop_flag_persistence_clean);
        addFlags(prop_flag_unsent);
        owner.enqueueUpdateOp(res);
    }

    enqueueTickOp(owner, res);

    return OPERATION_BLOCKED;
}


HandlerResult ActionsProperty::operation(LocatedEntity& entity, const Operation& op, OpVector& res)
{
    if (op->getClassNo() == Atlas::Objects::Operation::TICK_NO) {
        auto& args = op->getArgs();
        if (!args.empty()) {
            auto& arg = args.front();
            if (arg->getName() == "actions") {
                TickOperation(entity, op, res);
                return OPERATION_BLOCKED;
            }
        }
    }
    return OPERATION_IGNORED;
}

void ActionsProperty::enqueueTickOp(const LocatedEntity& entity, OpVector& res)
{
    boost::optional<double> nearestExpiry;
    for (auto& entry : m_data) {
        if (entry.second.endTime) {
            if (!nearestExpiry || *nearestExpiry > *entry.second.endTime) {
                nearestExpiry = *entry.second.endTime;
            }
        }
    }

    if (nearestExpiry) {
        //Check if there's either no tick enqueued, or if the tick that is enqueued is later than the nearest expiry.
      //  if (!m_tickOutstanding || nearestExpiry < *m_tickOutstanding) {
            Atlas::Objects::Operation::Tick tick;
            Atlas::Objects::Entity::Anonymous anon;
            anon->setName("actions");
            tick->setArgs1(std::move(anon));
            tick->setTo(entity.getId());
            tick->setFrom(entity.getId());
            tick->setSeconds(*nearestExpiry);
            res.emplace_back(std::move(tick));

            m_tickOutstanding = nearestExpiry;
        //}
    }

}


ActionsProperty* ActionsProperty::copy() const
{
    return new ActionsProperty(*this);
}

int ActionsProperty::get(Atlas::Message::Element& val) const
{
    Atlas::Message::MapType map;
    for (auto& entry : m_data) {
        Atlas::Message::MapType mapEntry{{"start_time", entry.second.startTime}};
        if (entry.second.endTime) {
            mapEntry.emplace("end_time", *entry.second.endTime);
        }

        map.emplace(entry.first, std::move(mapEntry));
    }
    val = std::move(map);
    return 0;
}

void ActionsProperty::set(const Atlas::Message::Element& val)
{
//    m_data.clear();
//    if (val.isMap()) {
//        for (auto& entry : val.Map()) {
//            if (entry.second.isString() && !entry.second.String().empty()) {
//                try {
//                    EntityFilter::ProviderFactory factory{};
//                    Attachment attachment{
//                            entry.second.String(),
//                            std::make_unique<EntityFilter::Filter>(entry.second.String(), factory)
//                    };
//                    m_data.emplace(entry.first, std::move(attachment));
//                } catch (const std::invalid_argument& e) {
//                    log(WARNING, String::compose(
//                            "Error when creating entity filter for attachment with constraint '%1'.: \n%2",
//                            entry.second.String(),
//                            e.what())
//                    );
//                }
//            }
//        }
//    }

}

void ActionsProperty::addAction(LocatedEntity& entity, OpVector& res, std::string actionName, ActionsProperty::Action action)
{
    m_data[actionName] = action;

    enqueueTickOp(entity, res);

    entity.applyProperty(*this);
    entity.enqueueUpdateOp(res);
}

void ActionsProperty::removeAction(LocatedEntity& entity, OpVector& res, std::string actionName)
{
    auto result = m_data.erase(actionName);
    if (result) {
        entity.applyProperty(*this);
        entity.enqueueUpdateOp(res);
    }
}
