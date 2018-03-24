// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2016 Erik Ogenvik
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

#include "ModeProperty.h"
#include "LocatedEntity.h"

#include "QuaternionProperty.h"

#include "common/BaseWorld.h"

#include <Atlas/Objects/Entity.h>
#include <Atlas/Objects/Operation.h>
#include <wfmath/atlasconv.h>

const std::string ModeProperty::property_name = "mode";
const std::string ModeProperty::property_atlastype = "string";

ModeProperty::ModeProperty()
: m_mode(Mode::Free)
{
}

void ModeProperty::apply(LocatedEntity *entity)
{

    if (m_data == "planted") {
        //See if there's a rotation we should apply
        const QuaternionProperty* plantedRotation = entity->getPropertyClass<QuaternionProperty>("planted-rotation");
        if (plantedRotation && plantedRotation->data().isValid()) {
            //Check that the rotation is applied already, otherwise apply it.
            QuaternionProperty* activeRotationProp = entity->requirePropertyClass<QuaternionProperty>("active-rotation");
            if (activeRotationProp->data() != plantedRotation->data()) {
                WFMath::Quaternion currentOrientation = entity->m_location.orientation();

                if (activeRotationProp->data().isValid() && activeRotationProp->data() != WFMath::Quaternion::Identity()) {
                    WFMath::Quaternion rotation = activeRotationProp->data().inverse();
                    //normalize to avoid drift
                    rotation.normalize();
                    currentOrientation = rotation * currentOrientation;
                }

                WFMath::Quaternion rotation = plantedRotation->data();
                //normalize to avoid drift
                rotation.normalize();
                currentOrientation = rotation * currentOrientation;

                activeRotationProp->data() = plantedRotation->data();
                activeRotationProp->apply(entity);
                activeRotationProp->resetFlags(per_clean);
                activeRotationProp->setFlags(flag_unsent);

                Atlas::Objects::Entity::Anonymous move_arg;
                move_arg->setId(entity->getId());
                move_arg->setAttr("orientation", currentOrientation.toAtlas());

                Atlas::Objects::Operation::Move moveOp;
                moveOp->setTo(entity->getId());
                moveOp->setSeconds(BaseWorld::instance().getTime());
                moveOp->setArgs1(move_arg);
                entity->sendWorld(moveOp);
            }
        }
    } else {
        if (entity->hasAttr("active-rotation")) {
            QuaternionProperty* activeRotationProp = entity->modPropertyClass<QuaternionProperty>("active-rotation");
            if (activeRotationProp->data().isValid()) {
                WFMath::Quaternion currentOrientation = entity->m_location.orientation();

                WFMath::Quaternion rotation = activeRotationProp->data().inverse();
                //normalize to avoid drift
                rotation.normalize();
                currentOrientation = rotation * currentOrientation;

                activeRotationProp->data() = WFMath::Quaternion::Identity();
                activeRotationProp->apply(entity);
                activeRotationProp->resetFlags(per_clean);
                activeRotationProp->setFlags(flag_unsent);

                Atlas::Objects::Entity::Anonymous move_arg;
                move_arg->setId(entity->getId());
                move_arg->setAttr("orientation", currentOrientation.toAtlas());

                Atlas::Objects::Operation::Move moveOp;
                moveOp->setTo(entity->getId());
                moveOp->setSeconds(BaseWorld::instance().getTime());
                moveOp->setArgs1(move_arg);
                entity->sendWorld(moveOp);

            }
        }
    }
}

ModeProperty * ModeProperty::copy() const
{
    return new ModeProperty(*this);
}

void ModeProperty::set(const Atlas::Message::Element & val)
{
    Property<std::string>::set(val);
    if (m_data == "free" || m_data.empty()) {
        m_mode = Mode::Free;
    } else if (m_data == "planted") {
        m_mode = Mode::Planted;
    } else if (m_data == "fixed") {
        m_mode = Mode::Fixed;
    } else if (m_data == "submerged") {
        m_mode = Mode::Submerged;
    } else {
        m_mode = Mode::Unknown;
    }
}

