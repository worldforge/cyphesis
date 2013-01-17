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

// $Id$

#ifndef RULESETS_TERRAIN_MOD_PROPERTY_H
#define RULESETS_TERRAIN_MOD_PROPERTY_H

#include "rulesets/TerrainEffectorProperty.h"

#include "physics/Vector3D.h"

namespace Mercator {
    class TerrainMod;
}

class TerrainProperty;
class TerrainModTranslator;

typedef HandlerResult (*Handler)(LocatedEntity *,
                                 const Operation &,
                                 OpVector &);
typedef std::map<int, Handler> HandlerMap;

/// \brief Class to handle Entity terrain modifier property
/// \ingroup PropertyClasses
class TerrainModProperty : public TerrainEffectorProperty {
  protected:

    /// \brief A pointer to the modifier returned by a call to Terrain::addMod()
    Mercator::TerrainMod *m_modptr;

    /// \brief the handlers this property should install
    HandlerMap m_handlers;

    /**
     * @brief The inner terrain mod instance which holds the actual Mercator::TerrainMod instance and handles the parsing of it.
     * In order to be able to better support different types of mods the actual instance will be any of the subclasses of InnerTerrainMod, depending on the type of the mod.
     */
    TerrainModTranslator* m_innerMod;

  public:

    explicit TerrainModProperty(const HandlerMap &);
    ~TerrainModProperty();

    TerrainModProperty * copy() const;

    virtual void install(LocatedEntity *);
    virtual void apply(LocatedEntity *);

    Mercator::TerrainMod * getModifier() {
        return m_modptr;
    }

    /// \brief Constructs a Mercator::TerrainMod from Atlas data
    Mercator::TerrainMod * parseModData(LocatedEntity * owner,
                                        const Atlas::Message::MapType &);

    /// \brief Changes a modifier's position
    void move(LocatedEntity*);

    /// \brief Removes the modifier from the terrain
    void remove(LocatedEntity*);

    /// \brief Retrieve a sub attribute of the property
    int getAttr(const std::string &,
                 Atlas::Message::Element &);
    /// \brief Modify a sub attribute of the property
    void setAttr(const std::string &,
                 const Atlas::Message::Element &);

    static HandlerResult move_handler(LocatedEntity * e,
                                      const Operation & op,
                                      OpVector & res);
    static HandlerResult delete_handler(LocatedEntity * e,
                                        const Operation & op,
                                        OpVector & res);
};


#endif // RULESETS_TERRAIN_MOD_PROPERTY_H
