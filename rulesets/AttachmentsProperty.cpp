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

#include "AttachmentsProperty.h"
#include "BaseWorld.h"
#include "common/Update.h"
#include "LocatedEntity.h"
#include "PlantedOnProperty.h"
#include <Atlas/Objects/Operation.h>
#include <rulesets/entityfilter/Providers.h>

AttachmentsProperty::AttachmentsProperty(uint32_t flags)
    : PropertyBase(flags)
{

}

void AttachmentsProperty::install(LocatedEntity* entity, const std::string& name)
{
    entity->installDelegate(Atlas::Objects::Operation::WIELD_NO, name);
}

void AttachmentsProperty::remove(LocatedEntity* entity, const std::string& name)
{
    entity->removeDelegate(Atlas::Objects::Operation::WIELD_NO, name);
}

HandlerResult AttachmentsProperty::operation(LocatedEntity* entity, const Operation& op, OpVector& res)
{
    if (!op->getArgs().empty()) {
        auto& arg = op->getArgs().front();
        //Check if it's this attach point that's being affected
        Atlas::Message::Element attachment_name;
        if (arg->copyAttr("attachment", attachment_name) == 0 && attachment_name.isString()) {
            auto attachmentI = m_data.find(attachment_name.String());

            if (attachmentI != m_data.end()) {
                auto& attachment = attachmentI->second;
                std::string entity_id;
                if (!arg->isDefaultId()) {
                    entity_id = arg->getId();
                }


                //std::set<const LocatedEntity*> prevObserving, newObserving;

                //Check if something already is attached, and detach it if so

                std::string attached_prop_name = std::string("attached_") + attachment_name.String();

                Ref<LocatedEntity> existing_entity;

                auto attachedProp = entity->getPropertyClass<SoftProperty>(attached_prop_name);
                if (attachedProp) {
                    existing_entity = extractEntityRef(attachedProp->data());
                }

                //If we're attaching an entity which is already attached at the same attachment just bail now.
                if (existing_entity) {
                    if (entity_id == existing_entity->getId()) {
                        //Entity is already attached, do nothing
                        return OPERATION_BLOCKED;
                    }
                }
                //Get the entities currently observing the attached entity
                // existing_entity->collectObservers(prevObserving);


                auto new_entity = BaseWorld::instance().getEntity(entity_id);
                if (new_entity) {
                    //Check that the attached entity matches the constraint filter
                    if (attachment.filter) {
                        EntityFilter::QueryContext queryContext{*entity, entity, new_entity.get()};
                        if (!attachment.filter->match(queryContext)) {
                            entity->clientError(op, String::compose("Attached entity failed the constraint '%1'.", attachment.contraint), res, entity->getId());
                            return OPERATION_BLOCKED;
                        }
                    }
                    //Check if the entity already is attached, and if so abort. The client needs to first send a detach/unwield op in this case.
                    auto plantedOnProp = new_entity->requirePropertyClassFixed<PlantedOnProperty>();
                    if (plantedOnProp->data().entity) {
                        //Check if the entity is attached to ourselves; if so we can just detach it from ourselves.
                        //Otherwise we need to abort, since we don't allow ourselves to detach it from another entity.
                        if (plantedOnProp->data().entity.get() == entity) {
                            if (plantedOnProp->data().attachment) {
                                //We need to reset the old attached value for the attached entity
                                auto old_attached_prop_name = std::string("attached_") + *plantedOnProp->data().attachment;
                                auto oldAttachedProp = entity->modPropertyClass<SoftProperty>(old_attached_prop_name);
                                if (oldAttachedProp) {
                                    oldAttachedProp->data() = Atlas::Message::Element();
                                    entity->applyProperty(old_attached_prop_name, oldAttachedProp);
                                }
                            }
                        } else {
                            entity->clientError(op, "The entity is already attached to another entity.", res, entity->getId());
                            return OPERATION_BLOCKED;
                        }
                    }

                    plantedOnProp->data().entity = WeakEntityRef(entity);
                    plantedOnProp->data().attachment = attachment_name.String();

                    new_entity->applyProperty(PlantedOnProperty::property_name, plantedOnProp);
                    {
                        Atlas::Objects::Operation::Update update;
                        update->setTo(new_entity->getId());
                        new_entity->sendWorld(update);
                    }

                    entity->setAttr(attached_prop_name, Atlas::Message::MapType{{"$eid", new_entity->getId()}});

                    //Check if there was another entity attached to the attachment, and if so reset it's attachment.
                    if (existing_entity) {
                        auto existing_entity_planted_on_prop = existing_entity->modPropertyClassFixed<PlantedOnProperty>();
                        if (existing_entity_planted_on_prop) {
                            existing_entity_planted_on_prop->data() = PlantedOnProperty::Data{};
                            existing_entity->applyProperty(PlantedOnProperty::property_name, existing_entity_planted_on_prop);
                            {
                                Atlas::Objects::Operation::Update update;
                                update->setTo(existing_entity->getId());
                                new_entity->sendWorld(update);
                            }
                        }
                    }

                    Atlas::Objects::Operation::Update update;
                    update->setTo(entity->getId());
                    entity->sendWorld(update);

                } else {
                    entity->clientError(op, "Could not find wielded entity.", res, entity->getId());
                }
                return OPERATION_BLOCKED;
            }
        }
    }
    return OPERATION_IGNORED;
}

AttachmentsProperty* AttachmentsProperty::copy() const
{
    return new AttachmentsProperty();
}

int AttachmentsProperty::get(Atlas::Message::Element& val) const
{
    Atlas::Message::MapType map;
    for (auto& entry : m_data) {
        map.emplace(entry.first, entry.second.contraint);
    }
    val = std::move(map);
    return 0;
}

void AttachmentsProperty::set(const Atlas::Message::Element& val)
{
    m_data.clear();
    if (val.isMap()) {
        for (auto& entry : val.Map()) {
            if (entry.second.isString() && !entry.second.String().empty()) {
                try {
                    EntityFilter::ProviderFactory factory{};
                    Attachment attachment{entry.second.String(),
                                          std::unique_ptr<EntityFilter::Filter>(new EntityFilter::Filter(entry.second.String(), factory))};
                    m_data.emplace(entry.first, std::move(attachment));
                } catch (const std::invalid_argument& e) {
                    log(WARNING, String::compose("Error when creating entity filter for attachment with constraint '%1'.: \n%2", entry.second.String(), e.what()));
                }
            }
        }
    }

}

Ref<LocatedEntity> AttachmentsProperty::extractEntityRef(const Atlas::Message::Element& val)
{
    if (val.isMap()) {
        auto I = val.Map().find("$eid");
        if (I != val.Map().end() && I->second.isString()) {
            return BaseWorld::instance().getEntity(I->second.String());
        }
    }
    return Ref<LocatedEntity>();
}
