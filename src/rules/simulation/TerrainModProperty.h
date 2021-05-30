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


#ifndef RULESETS_TERRAIN_MOD_PROPERTY_H
#define RULESETS_TERRAIN_MOD_PROPERTY_H

#include "TerrainEffectorProperty.h"

#include "physics/Vector3D.h"
#include <memory>

namespace Mercator {
    class TerrainMod;
}

class TerrainProperty;

class TerrainModTranslator;

/// \brief Class to handle Entity terrain modifier property
/// \ingroup PropertyClasses
class TerrainModProperty : public TerrainEffectorProperty
{
    public:
        static constexpr const char* property_name = "terrainmod";

        TerrainModProperty();

        TerrainModProperty(const TerrainModProperty& rhs);

        ~TerrainModProperty() override;

        TerrainModProperty* copy() const override;

        void apply(LocatedEntity&) override;

        /// \brief Constructs a Mercator::TerrainMod from Atlas data
        std::unique_ptr<Mercator::TerrainMod> parseModData(const WFMath::Point<3>& pos,
                                           const WFMath::Quaternion& orientation) const;

        /// \brief Retrieve a sub attribute of the property
        int getAttr(const std::string&,
                    Atlas::Message::Element&) const;

        /// \brief Modify a sub attribute of the property
        void setAttr(const std::string&,
                     const Atlas::Message::Element&);


    protected:

        std::unique_ptr<TerrainModTranslator> m_translator;

};


#endif // RULESETS_TERRAIN_MOD_PROPERTY_H
