/*
 Copyright (C) 2019 Erik Ogenvik

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

#include "ModeDataProperty.h"
#include "BaseWorld.h"
#include "rules/LocatedEntity.h"

ModeDataProperty::ModeDataProperty() : mMode(ModeProperty::Mode::Unknown)
{}

ModeDataProperty* ModeDataProperty::copy() const
{
    return new ModeDataProperty(*this);
}

void ModeDataProperty::set(const Atlas::Message::Element& val)
{
    if (val.isMap()) {
        const auto& map = val.Map();
        auto modeI = map.find("mode");
        if (modeI != map.end() && modeI->second.isString()) {
            auto mode = ModeProperty::parseMode(modeI->second.String());
            switch (mode) {
                case ModeProperty::Mode::Planted: {
                    setPlantedData(map);
                    return;
                }
                case ModeProperty::Mode::Projectile: {
                    setProjectileData(map);
                    return;
                }
                default: {
                }
            }
        }
    }
    mMode = ModeProperty::Mode::Unknown;
    mData = NullData{};
}

int ModeDataProperty::get(Atlas::Message::Element& val) const
{
    Atlas::Message::MapType map;
    switch (mMode) {
        case ModeProperty::Mode::Planted: {
            auto& plantedData = boost::get<PlantedOnData>(mData);
            if (plantedData.entityId) {
                map["$eid"] = std::to_string(*plantedData.entityId);
            }
            if (plantedData.attachment) {
                map["attachment"] = *plantedData.attachment;
            }
            map["mode"] = "planted";
        }
            break;
        case ModeProperty::Mode::Fixed:
        case ModeProperty::Mode::Free:
        case ModeProperty::Mode::Submerged:
            break;
        case ModeProperty::Mode::Projectile: {
            auto& projectileData = boost::get<ProjectileData>(mData);
            if (projectileData.entity) {
                map["$eid"] = projectileData.entity->getId();
            }
            map["extra"] = projectileData.extra;
            map["mode"] = "projectile";
        }
            break;
        case ModeProperty::Mode::Unknown: {
        }
            break;
    }
    val = map;

    return 0;
}

void ModeDataProperty::setPlantedData(const Atlas::Message::MapType& map)
{
    auto data = parsePlantedData(map);
    setPlantedData(std::move(data));
}

void ModeDataProperty::setPlantedData(PlantedOnData data)
{
    mData = std::move(data);
    mMode = ModeProperty::Mode::Planted;
}


void ModeDataProperty::setProjectileData(const Atlas::Message::MapType& map)
{
    ProjectileData data{};

    auto eidI = map.find("$eid");
    if (eidI != map.end() && eidI->second.isString()) {
        data.entity = BaseWorld::instance().getEntity(eidI->second.String());
    }

    auto extraI = map.find("extra");
    if (extraI != map.end() && extraI->second.isMap()) {
        data.extra = extraI->second.Map();
    }

    setProjectileData(std::move(data));
}

void ModeDataProperty::setProjectileData(ProjectileData data)
{
    mData = std::move(data);
    mMode = ModeProperty::Mode::Projectile;
}

ModeDataProperty::PlantedOnData& ModeDataProperty::getPlantedOnData()
{
    return boost::get<PlantedOnData>(mData);
}

const ModeDataProperty::PlantedOnData& ModeDataProperty::getPlantedOnData() const
{
    return boost::get<PlantedOnData>(mData);
}

ModeDataProperty::ProjectileData& ModeDataProperty::getProjectileData()
{
    return boost::get<ProjectileData>(mData);
}

const ModeDataProperty::ProjectileData& ModeDataProperty::getProjectileData() const
{
    return boost::get<ProjectileData>(mData);
}

void ModeDataProperty::clearData()
{
    mMode = ModeProperty::Mode::Unknown;
    mData = NullData{};
}

ModeProperty::Mode ModeDataProperty::getMode() const
{
    return mMode;
}

ModeDataProperty::PlantedOnData ModeDataProperty::parsePlantedData(const Atlas::Message::MapType& map)
{
    PlantedOnData data{};
    auto eidI = map.find("$eid");
    if (eidI != map.end() && eidI->second.isString()) {
        data.entityId = std::stol(eidI->second.String());
    }
    auto pointI = map.find("attachment");
    if (pointI != map.end() && pointI->second.isString()) {
        data.attachment = pointI->second.String();
    }
    return data;
}
