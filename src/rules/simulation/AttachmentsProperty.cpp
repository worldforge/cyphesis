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

#include "AttachmentsProperty.h"
#include "BaseWorld.h"
#include "ModeDataProperty.h"
#include "common/operations/Update.h"
#include "common/Inheritance.h"
#include "rules/LocatedEntity.h"
#include "rules/entityfilter/ProviderFactory.h"
#include <Atlas/Objects/Operation.h>

AttachmentsProperty::AttachmentsProperty(uint32_t flags)
        : PropertyBase(flags)
{

}

void AttachmentsProperty::install(LocatedEntity& entity, const std::string& name)
{
    entity.installDelegate(Atlas::Objects::Operation::WIELD_NO, name);
}

void AttachmentsProperty::remove(LocatedEntity& entity, const std::string& name)
{
    entity.removeDelegate(Atlas::Objects::Operation::WIELD_NO, name);
}

HandlerResult AttachmentsProperty::operation(LocatedEntity& entity, const Operation& op, OpVector& res)
{
    if (!op->getArgs().empty()) {
        auto& arg = op->getArgs().front();
        //Check if it's this attach point that's being affected
        Atlas::Message::Element attachment_name;
        if (arg->copyAttr("attachment", attachment_name) == 0 && attachment_name.isString()) {
            auto attachmentI = m_data.find(attachment_name.String());

            if (attachmentI != m_data.end()) {
                std::string entity_id;
                if (!arg->isDefaultId()) {
                    entity_id = arg->getId();
                }

                //Check if something already is attached, and detach it if so

                std::string attached_prop_name = std::string("attached_") + attachment_name.String();

                Ref<LocatedEntity> existing_entity;

                auto attachedProp = entity.getPropertyClass<SoftProperty>(attached_prop_name);
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

                auto resetExistingEntityPlantedOn = [&]() {
                    if (existing_entity) {
                        auto existing_entity_mode_data_prop = existing_entity->modPropertyClassFixed<ModeDataProperty>();
                        if (existing_entity_mode_data_prop) {
                            existing_entity_mode_data_prop->clearData();
                            existing_entity->applyProperty(*existing_entity_mode_data_prop);
                            existing_entity->enqueueUpdateOp();
                        }
                    }
                };

                if (entity_id.empty()) {
                    //Unwielding

                    entity.setAttr(attached_prop_name, {});

                    //Check if there was another entity attached to the attachment, and if so reset it's attachment.
                    resetExistingEntityPlantedOn();

                    entity.enqueueUpdateOp();

                } else {

                    auto new_entity = BaseWorld::instance().getEntity(entity_id);
                    if (new_entity) {
                        auto& attachment = attachmentI->second;

                        //Check that the attached entity matches the constraint filter
                        if (attachment.filter) {
                            EntityFilter::QueryContext queryContext{entity, &entity, new_entity.get()};
                            std::vector<std::string> errors;
                            queryContext.report_error_fn = [&](const std::string& error) { errors.push_back(error); };
                            queryContext.entity_lookup_fn = [](const std::string& id) { return BaseWorld::instance().getEntity(id); };
                            queryContext.type_lookup_fn = [](const std::string& id) { return Inheritance::instance().getType(id); };
                            if (!attachment.filter->match(queryContext)) {
                                if (errors.empty()) {
                                    entity.clientError(op, String::compose("Attached entity failed the constraint '%1'.", attachment.contraint), res, entity.getId());
                                } else {
                                    entity.clientError(op, errors.front(), res, entity.getId());
                                }
                                return OPERATION_BLOCKED;
                            }
                        }
                        //Check if the entity already is attached, and if so abort. The client needs to first send a detach/unwield op in this case.
                        auto& modeDataProp = new_entity->requirePropertyClassFixed<ModeDataProperty>();
                        if (modeDataProp.getMode() == ModeProperty::Mode::Planted) {
                            auto& plantedOnData = modeDataProp.getPlantedOnData();
                            //Check if the entity is attached to ourselves; if so we can just detach it from ourselves.
                            //Otherwise we need to abort, since we don't allow ourselves to detach it from another entity.
                            if (plantedOnData.entityId && plantedOnData.entityId == entity.getIntId()) {
                                if (plantedOnData.attachment) {
                                    //We need to reset the old attached value for the attached entity
                                    auto old_attached_prop_name = std::string("attached_") + *plantedOnData.attachment;
                                    auto oldAttachedProp = entity.modPropertyClass<SoftProperty>(old_attached_prop_name);
                                    if (oldAttachedProp) {
                                        oldAttachedProp->data() = Atlas::Message::Element();
                                        entity.applyProperty(old_attached_prop_name, *oldAttachedProp);
                                    }
                                }
                            } else {
                                entity.clientError(op, "The entity is already attached to another entity.", res, entity.getId());
                                return OPERATION_BLOCKED;
                            }
                        }

                        modeDataProp.setPlantedData(ModeDataProperty::PlantedOnData{entity.getIntId(), attachment_name.String()});

                        new_entity->applyProperty(modeDataProp);
                        new_entity->enqueueUpdateOp();

                        entity.setAttrValue(attached_prop_name, Atlas::Message::MapType{{"$eid", new_entity->getId()}});

                        //Check if there was another entity attached to the attachment, and if so reset it's attachment.
                        resetExistingEntityPlantedOn();

                        entity.enqueueUpdateOp();

                    } else {
                        entity.clientError(op, "Could not find wielded entity.", res, entity.getId());
                    }
                }
                return OPERATION_BLOCKED;
            }
        }
    }
    return OPERATION_IGNORED;
}

AttachmentsProperty* AttachmentsProperty::copy() const
{
    //Don't copy values.
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
                    Attachment attachment{
                            entry.second.String(),
                            std::make_unique<EntityFilter::Filter>(entry.second.String(), factory)
                    };
                    m_data.emplace(entry.first, std::move(attachment));
                } catch (const std::invalid_argument& e) {
                    log(WARNING, String::compose(
                            "Error when creating entity filter for attachment with constraint '%1'.: \n%2",
                            entry.second.String(),
                            e.what())
                    );
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
