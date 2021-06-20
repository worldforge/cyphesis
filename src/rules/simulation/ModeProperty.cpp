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
#include "rules/LocatedEntity.h"

#include "rules/QuaternionProperty.h"

#include "rules/simulation/BaseWorld.h"
#include "ModeDataProperty.h"

#include <Atlas/Objects/Entity.h>
#include <Atlas/Objects/Operation.h>
#include <wfmath/atlasconv.h>
#include "rules/AtlasProperties.h"
#include "rules/PhysicalProperties.h"


ModeProperty::ModeProperty()
        : m_mode(Mode::Free)
{
}

void ModeProperty::apply(LocatedEntity& entity)
{
    auto modeDataProp = entity.getPropertyClassFixed<ModeDataProperty>();
    if (modeDataProp) {
        if (modeDataProp->getMode() != m_mode && modeDataProp->getMode() != Mode::Unknown) {
            auto mutableModeDataProp = entity.modPropertyClassFixed<ModeDataProperty>();
            mutableModeDataProp->clearData();
            entity.applyProperty(*mutableModeDataProp);
            mutableModeDataProp->removeFlags(prop_flag_persistence_clean);
        }
    }

    auto orientationProperty = entity.getPropertyClassFixed<OrientationProperty>();

    if (m_mode == Mode::Planted) {
        //See if there's a rotation we should apply
        const auto* plantedRotation = entity.getPropertyClass<QuaternionProperty>("planted_rotation");
        if (plantedRotation && plantedRotation->data().isValid()) {
            //Check that the rotation is applied already, otherwise apply it.
            auto& activeRotationProp = entity.requirePropertyClass<QuaternionProperty>("active_rotation");
            if (activeRotationProp.data() != plantedRotation->data()) {
                //TODO: is this right? perhaps we should just bail out if there's no valid orientation?
                WFMath::Quaternion currentOrientation = (orientationProperty && orientationProperty->data().isValid()) ? orientationProperty->data() : WFMath::Quaternion::IDENTITY();

                if (activeRotationProp.data().isValid() && activeRotationProp.data() != WFMath::Quaternion::Identity()) {
                    WFMath::Quaternion rotation = activeRotationProp.data().inverse();
                    //normalize to avoid drift
                    rotation.normalize();
                    currentOrientation = rotation * currentOrientation;
                }

                WFMath::Quaternion rotation = plantedRotation->data();
                //normalize to avoid drift
                rotation.normalize();
                currentOrientation = rotation * currentOrientation;

                activeRotationProp.data() = plantedRotation->data();
                entity.applyProperty("active_rotation", activeRotationProp);
                activeRotationProp.removeFlags(prop_flag_persistence_clean);

                Atlas::Objects::Entity::Anonymous move_arg;
                move_arg->setId(entity.getId());
                move_arg->setAttr("orientation", currentOrientation.toAtlas());

                Atlas::Objects::Operation::Move moveOp;
                moveOp->setTo(entity.getId());
                moveOp->setSeconds(BaseWorld::instance().getTimeAsSeconds());
                moveOp->setArgs1(move_arg);
                entity.sendWorld(moveOp);
            }
        }
    } else {
        auto* activeRotationProp = entity.modPropertyClass<QuaternionProperty>("active_rotation");
        if (activeRotationProp && activeRotationProp->data().isValid()) {
            //TODO: is this right? perhaps we should just bail out if there's no valid orientation?
            WFMath::Quaternion currentOrientation = (orientationProperty && orientationProperty->data().isValid()) ? orientationProperty->data() : WFMath::Quaternion::IDENTITY();

            WFMath::Quaternion rotation = activeRotationProp->data().inverse();
            //normalize to avoid drift
            rotation.normalize();
            currentOrientation = rotation * currentOrientation;

            activeRotationProp->data() = WFMath::Quaternion::Identity();
            activeRotationProp->apply(entity);
            activeRotationProp->removeFlags(prop_flag_persistence_clean);
            activeRotationProp->addFlags(prop_flag_unsent);

            Atlas::Objects::Entity::Anonymous move_arg;
            move_arg->setId(entity.getId());
            move_arg->setAttr("orientation", currentOrientation.toAtlas());

            Atlas::Objects::Operation::Move moveOp;
            moveOp->setTo(entity.getId());
            moveOp->setSeconds(BaseWorld::instance().getTimeAsSeconds());
            moveOp->setArgs1(move_arg);
            entity.sendWorld(moveOp);

        }

    }

}

ModeProperty* ModeProperty::copy() const
{
    return new ModeProperty(*this);
}

int ModeProperty::get(Atlas::Message::Element& val) const
{
    val = encodeMode(m_mode);
    return 0;
}

std::string ModeProperty::encodeMode(ModeProperty::Mode mode)
{
    switch (mode) {
        case Mode::Planted:
            return "planted";
        case Mode::Fixed:
            return "fixed";
        case Mode::Free:
            return "free";
        case Mode::Submerged:
            return "submerged";
        case Mode::Projectile:
            return "projectile";
        case Mode::Unknown:
            return "";
    }
    return "";
}


ModeProperty::Mode ModeProperty::parseMode(const std::string& mode)
{
    if (mode == "free" || mode.empty()) {
        return Mode::Free;
    } else if (mode == "planted") {
        return Mode::Planted;
    } else if (mode == "fixed") {
        return Mode::Fixed;
    } else if (mode == "submerged") {
        return Mode::Submerged;
    } else if (mode == "projectile") {
        return Mode::Projectile;
    } else {
        return Mode::Unknown;
    }
}

void ModeProperty::set(const Atlas::Message::Element& val)
{
    if (val.isString()) {
        auto& data = val.asString();
        m_mode = parseMode(data);
    }
}

std::string ModeProperty::data() const
{
    return encodeMode(m_mode);
}

