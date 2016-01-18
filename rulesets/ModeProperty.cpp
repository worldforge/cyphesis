/*
 * ModeProperty.cpp
 *
 *  Created on: 10 jul 2015
 *      Author: erik
 */

#include "ModeProperty.h"
#include "LocatedEntity.h"

#include "PlantableProperty.h"

ModeProperty::ModeProperty() {
}

ModeProperty::~ModeProperty() {
}

void ModeProperty::apply(LocatedEntity *entity) {
    if (m_data == "planted") {
        auto plantableProp = entity->getPropertyClass<PlantableProperty>("plantable");
        if (plantableProp) {
            if (plantableProp->getOrientation().isValid()) {
                entity->m_location.m_orientation = plantableProp->getOrientation();
                entity->resetFlags(entity_orient_clean);
            }
        }
    }
}

