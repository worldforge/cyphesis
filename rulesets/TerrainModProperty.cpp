// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2004 Alistair Riddoch
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

// $Id: TerrainModProperty.cpp,v 1.1 2008-08-14 00:03:12 alriddoch Exp $

#include "TerrainModProperty.h"

#include "World.h"

#include "common/log.h"
#include "common/debug.h"

#include "modules/Location.h"

#include <Mercator/Terrain.h>
#include <Mercator/Segment.h>
#include <Mercator/TerrainMod.h>

#include "TerrainProperty.h"

#include <sstream>

#include <cassert>

static const bool debug_flag = false;

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Message::FloatType;

typedef Mercator::Terrain::Pointstore Pointstore;
typedef Mercator::Terrain::Pointcolumn Pointcolumn;

/// \brief TerrainModProperty constructor
///
TerrainModProperty::TerrainModProperty() : PropertyBase(0), m_owner(0)
{
}

void TerrainModProperty::setup(Entity* owner)
{
    log(INFO, "setting up terrain mods");
    m_owner = owner;
    Element ent;
    owner->getAttr("terrainmod", ent);
    m_terrainmods = ent.asMap();

    SetTerrainModifiers(m_owner, ent);
}

bool TerrainModProperty::get(Element & ent) const
{
    MapType & mod = (ent = MapType()).Map();
    mod = m_terrainmods;
    return true;
}

Mercator::TerrainMod * TerrainModProperty::getModifier(Entity* owner)
{
    return parseModData(owner, m_terrainmods);
}

Mercator::TerrainMod * TerrainModProperty::getModifier()
{
    return parseModData(m_owner, m_terrainmods);
}

void TerrainModProperty::set(const Element & ent)
{
    if (ent.isMap()) {
        const MapType & mod = ent.Map();
        m_terrainmods = mod;
    }

    if (m_owner == NULL) {
//         log(INFO, "Yeah, it's NULL");
    } else {
        SetTerrainModifiers(m_owner, ent);
    }
}

void TerrainModProperty::set(const Entity* owner, const Element & ent)
{
    log(INFO, "YEAS");
    if (ent.isMap()) {
        const MapType & mod = ent.Map();
        m_terrainmods = mod;
    }

    SetTerrainModifiers(owner, ent);
}

void TerrainModProperty::add(const std::string & s, MapType & ent) const
{
    get(ent[s]);
}

Mercator::TerrainMod * TerrainModProperty::parseModData(const Entity* owner, const Element & modifier)
{
    if (!modifier.isMap()) {
        log(ERROR, "Invalid terrain mod data");
    }

    const Atlas::Message::MapType & modMap = modifier.asMap();
    m_terrainmods = modMap;
    std::string modType;
    std::string shapeType;
    int shapeDim;
    WFMath::Point<3> pos;
    Atlas::Message::MapType shapeMap;

    Atlas::Message::MapType::const_iterator mod_I;

    // Get modifier type
    mod_I = modMap.find("type");
    if (mod_I != modMap.end()) {
    const Atlas::Message::Element& modTypeElem(mod_I->second);
        if (modTypeElem.isString()) {
            modType = modTypeElem.asString();
        }
    }

    // Get modifier position
    Point3D modPos = owner->m_location.pos();
    pos = WFMath::Point<3>(modPos.x(), modPos.y(), modPos.z());


    // Get modifier's shape
    mod_I = modMap.find("shape");
    if (mod_I != modMap.end()) {
        const Atlas::Message::Element& shapeElem(mod_I->second);
        if (shapeElem.isMap()) {
            shapeMap = shapeElem.asMap();
            Atlas::Message::MapType::const_iterator shape_I;
                // Get shape's type
            shape_I = shapeMap.find("type");
            if (shape_I != shapeMap.end()) {
                const Atlas::Message::Element& shapeTypeElem(shape_I->second);
                if (shapeTypeElem.isString()) {
                shapeType = shapeTypeElem.asString();
                }
            }

            // Get shape's dimension
            shape_I = shapeMap.find("dim");
            if (shape_I != shapeMap.end()) {
                const Atlas::Message::Element& shapeDimElem(shape_I->second);
                if (shapeDimElem.isInt()) {
                    shapeDim = (int)shapeDimElem.asNum();
                }
            }
        } // end shape data

        // Check for additional modifier parameters
        if (modType == "slopemod") {
            float dx, dy, level;
            // Get slopes
            mod_I = modMap.find("slopes");
            if (mod_I != modMap.end()) {
                const Atlas::Message::Element& modSlopeElem = mod_I->second;
                if (modSlopeElem.isList()) {
                    const Atlas::Message::ListType & slopes = modSlopeElem.asList();
                    dx = (int)slopes[0].asNum();
                    dy = (int)slopes[1].asNum();
                }
            }

            // Get level
            mod_I = modMap.find("height");
            if (mod_I != modMap.end()) {
                const Atlas::Message::Element& modHeightElem = mod_I->second;
                level = modHeightElem.asNum();
            }

            if (shapeType == "ball") {
                float shapeRadius;
                // Get sphere's radius
                Atlas::Message::MapType::const_iterator shape_I = shapeMap.find("radius");
                if (shape_I != shapeMap.end()) {
                const Atlas::Message::Element& shapeRadiusElem(shape_I->second);
                shapeRadius = shapeRadiusElem.asNum();
                }

                // Make disc
                WFMath::Point<2> pos_2d(pos.x(),pos.y());
                WFMath::Ball<2> modShape = WFMath::Ball<2>(pos_2d, shapeRadius);

//                  log(INFO, "Successfully parsed a slopemod");

                // Make modifier

                // Apply Modifier

            }

        } else if (modType == "levelmod") {
            float level;
            // Get level
            mod_I = modMap.find("height");
            if (mod_I != modMap.end()) {
                const Atlas::Message::Element& modHeightElem = mod_I->second;
                level = modHeightElem.asNum();
            }

            if (shapeType == "ball") {
                float shapeRadius;
                // Get sphere's radius
                Atlas::Message::MapType::const_iterator shape_I = shapeMap.find("radius");
                if (shape_I != shapeMap.end()) {
                    const Atlas::Message::Element& shapeRadiusElem(shape_I->second);
                    shapeRadius = shapeRadiusElem.asNum();
                }

                // Make disc
                WFMath::Point<2> pos_2d(pos.x(),pos.y());
                WFMath::Ball<2> modShape = WFMath::Ball<2>(pos_2d, shapeRadius); ///FIXME: assumes 2d ball...

//                  log(INFO, "Successfully parsed a levelmod");

                // Make Modifier
                Mercator::LevelTerrainMod<WFMath::Ball<2> > *NewMod;
                NewMod = new Mercator::LevelTerrainMod<WFMath::Ball<2> >(level, modShape);

                return NewMod;

            } else if (shapeType == "rotbox") {
                WFMath::Point<2> shapePoint;
                WFMath::Vector<2> shapeVector;
                // Get rotbox's position
                Atlas::Message::MapType::const_iterator shape_I = shapeMap.find("point");
                if (shape_I != shapeMap.end()) {
                const Atlas::Message::Element& shapePointElem(shape_I->second);
                    if (shapePointElem.isList()) {
                        const Atlas::Message::ListType & pointList = shapePointElem.asList();
                        shapePoint = WFMath::Point<2>((int)pointList[0].asNum(), (int)pointList[1].asNum());
                    }
                }
                // Get rotbox's vector
                shape_I = shapeMap.find("vector");
                if (shape_I != shapeMap.end()) {
                    const Atlas::Message::Element& shapeVectorElem(shape_I->second);
                    if (shapeVectorElem.isList()) {
                        const Atlas::Message::ListType & vectorList = shapeVectorElem.asList(); 
                        shapeVector = WFMath::Vector<2>((int)vectorList[0].asNum(), (int)vectorList[1].asNum());
                    }
                }
                
//                 log(INFO,"Successfully parsed a levelmod");

                // Make rotbox
                    ///FIXME: needs to use shapeDim instead of 2
                WFMath::RotBox<2> modShape = WFMath::RotBox<2>(shapePoint, shapeVector, WFMath::RotMatrix<2>()); 

                // Make modifier
                Mercator::LevelTerrainMod<WFMath::RotBox<2> > *NewMod;
                NewMod = new Mercator::LevelTerrainMod<WFMath::RotBox<2> >(level, modShape);

                return NewMod;
            }

        } else if (modType == "adjustmod") {
            float level;
            // Get level
            mod_I = modMap.find("height");
            if (mod_I != modMap.end()) {
                const Atlas::Message::Element& modHeightElem = mod_I->second;
                level = modHeightElem.asNum();
            }

            if (shapeType == "ball") {
                float shapeRadius;
                // Get sphere's radius
                Atlas::Message::MapType::const_iterator shape_I = shapeMap.find("radius");
                if (shape_I != shapeMap.end()) {
                    const Atlas::Message::Element& shapeRadiusElem(shape_I->second);
                    shapeRadius = shapeRadiusElem.asNum();
                }

                // Make sphere
                WFMath::Point<2> pos_2d(pos.x(), pos.y());
                WFMath::Ball<2> modShape = WFMath::Ball<2>(pos_2d, shapeRadius);

                // Make modifier

                // Apply Modifier

            }
//             log(INFO,"Successfully parsed an adjustmod");

        } else if (modType == "cratermod") {

            // Get other shape parameters
            if (shapeType == "ball" ) {
                float shapeRadius;
                // Get sphere's radius
                Atlas::Message::MapType::const_iterator shape_I = shapeMap.find("radius");
                if (shape_I != shapeMap.end()) {
                    const Atlas::Message::Element& shapeRadiusElem(shape_I->second);
                    shapeRadius = shapeRadiusElem.asNum();
                }

                // Make sphere
                WFMath::Ball<3> modShape = WFMath::Ball<3>(pos, shapeRadius); ///FIXME: assumes 3d ball...

//                 log(INFO,"Successfully parsed a cratermod");
                // Make modifier
                Mercator::CraterTerrainMod *NewMod;
                NewMod = new Mercator::CraterTerrainMod(modShape);

                return NewMod;
            }
        }
    }
}

 void TerrainModProperty::SetTerrainModifiers(const Element & modifier)
 {
    Point3D modPos;
    WFMath::Point<3> pos;
    if (m_owner != NULL) {
        modPos = m_owner->m_location.pos();
        pos = WFMath::Point<3>(modPos.x(), modPos.y(), modPos.z());
    } else {
        std::stringstream ss;
        ss << "Attempting to set a mod with a NULL owner";
        log(INFO, ss.str());
    }

    Mercator::TerrainMod * NewMod = parseModData(m_owner, modifier);


        // Get list of entities in the world and find the one holding the terrain
    const EntityDict & ents = BaseWorld::instance().getEntities();
    EntityDict::const_iterator eI = ents.begin();
    PropertyBase * terr; // terrain property?
    TerrainProperty * r_terr; // real terrain property
        // Search for an entity with the terrain property
    for (; eI != ents.end(); eI++)
    {
        terr = eI->second->getProperty("terrain");
        if (terr != NULL) {
            break;
        }
    }

    if (terr != NULL) {
        // Find other entities w/ modifiers
        std::list<TerrainModProperty*> mod_ents;
        PropertyBase * prop;
        eI = ents.begin();
        for (; eI != ents.end(); eI++)
        {
            prop = eI->second->getProperty("terrainmod");
            if (prop != NULL) {
                mod_ents.push_back(dynamic_cast<TerrainModProperty*>(prop));
            }
        }
        // Add them to list if they are not THIS entity
        // run r_terr->setMod(entity's mod) for each
        r_terr = dynamic_cast<TerrainProperty*>(terr);
        r_terr->clearMods(pos.x(), pos.y());
        float mx,my;
        for (std::list<TerrainModProperty *>::const_iterator I = mod_ents.begin(); I != mod_ents.end(); I++)
        {
            if ( (*I)->m_owner != NULL) {
                mx = (*I)->m_owner->m_location.pos().x();
                my = (*I)->m_owner->m_location.pos().y();
                std::stringstream ss;
                ss << "Modifier being applied at: " << mx << "," << my;
                log(INFO, ss.str());
                Mercator::TerrainMod *nm = (*I)->getModifier();
                r_terr->setMod(nm, mx, my);
            }
        }
        r_terr->setMod(NewMod, pos.x(), pos.y());
        std::stringstream ss;
        ss << "Applied last mod at: " << pos.x() << "," << pos.y();
        log(INFO, ss.str());
    } else {
        log(ERROR, "Could not find TerrainProperty to apply modifier.");
    }
 }

void TerrainModProperty::SetTerrainModifiers(const Entity* owner, const Element & modifier)
{
    Point3D modPos = owner->m_location.pos();
    WFMath::Point<3> pos = WFMath::Point<3>(modPos.x(), modPos.y(), modPos.z());

    Mercator::TerrainMod * NewMod = parseModData(owner, modifier);


        // Get list of entities in the world and find the one holding the terrain
    const EntityDict & ents = BaseWorld::instance().getEntities();
    EntityDict::const_iterator eI = ents.begin();
    PropertyBase * terr; // terrain property?
    TerrainProperty * r_terr; // real terrain property
        // Search for an entity with the terrain property
    for (; eI != ents.end(); eI++)
    {
        terr = eI->second->getProperty("terrain");
        if (terr != NULL) {
            break;
        }
    }
    r_terr = dynamic_cast<TerrainProperty*>(terr);
    r_terr->clearMods(pos.x(), pos.y());

    float mx,my;
    if (terr != NULL) {
        // Find other entities w/ modifiers
        std::list<TerrainModProperty*> mod_ents;
        PropertyBase * prop;
        eI = ents.begin();
        for (; eI != ents.end(); eI++)
        {
            prop = eI->second->getProperty("terrainmod");
            if (prop != NULL) {
                mx = eI->second->m_location.pos().x();
                my = eI->second->m_location.pos().y();
                std::stringstream ss;
                ss << "Modifier being applied at: " << mx << "," << my;
                log(INFO, ss.str());
                Mercator::TerrainMod *nm = dynamic_cast<TerrainModProperty*>(eI->second->getProperty("terrainmod"))->getModifier(eI->second);
                r_terr->setMod(nm, mx, my);

            }
        }

        r_terr->setMod(NewMod, pos.x(), pos.y());
        std::stringstream ss;
        ss << "Applied last mod at: " << pos.x() << "," << pos.y();
        log(INFO, ss.str());
    } else {
        log(ERROR, "Could not find TerrainProperty to apply modifier.");
    }
}

void TerrainModProperty::move(Entity* owner)
{

        // Get terrain
    const EntityDict & ents = BaseWorld::instance().getEntities();
    EntityDict::const_iterator eI = ents.begin();
    PropertyBase * prop;
    TerrainProperty * terrain;

    for (; eI != ents.end(); eI++)
    {
        prop = eI->second->getProperty("terrain");
        if (prop != NULL) {
            terrain = dynamic_cast<TerrainProperty*>(prop);
            break; // Found the terrain!
        }
    }
    
        // Clear mods from the old position
    terrain->clearMods(owner->m_location.pos().x(), owner->m_location.pos().y());

    // reapply mods on old segment
    TerrainModProperty * mod;
    for(eI = ents.begin(); eI != ents.end(); eI++) {
        prop = eI->second->getProperty("terrainmod");
        if (prop != NULL) {
            mod = dynamic_cast<TerrainModProperty*>(prop);
            Element mod_data;
            if (mod->get(mod_data)) {
                mod->set(mod_data);
            }
        }
    }

    // Apply this mod at the new position
    set(m_terrainmods);
}
