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

#ifndef RULESETS_GEOMETRYPROPERTY_H_
#define RULESETS_GEOMETRYPROPERTY_H_

#include "common/Property.h"
#include <wfmath/axisbox.h>
#include <wfmath/point.h>
#include <wfmath/vector.h>
#include <functional>

class btCollisionShape;

class btVector3;

/**
 * @brief Specifies geometry of an entity.
 *
 * The property "shape" defines the shape of the geometry.
 * Possible values:
 *
 * "sphere"
 * "box"
 * "capsule-z" : A capsule, oriented along the z axis
 * "capsule-x" : A capsule, oriented along the x axis
 * "capsule-y" : A capsule, oriented along the y axis
 * "cylinder-z": A cylinder, oriented along the z axis
 * "cylinder-x": A cylinder, oriented along the x axis
 * "cylinder-y": A cylinder, oriented along the y axis
 * "mesh"      : An arbitrary mesh, using vertices and indices.
 * "asset"     : Generates a mesh from a file. The path to is defined in the "path" entry.
 *
 * @ingroup PropertyClasses
 */
class GeometryProperty : public Property<Atlas::Message::MapType>
{
    public:

        static const std::string property_name;
        static const std::string property_atlastype;

        GeometryProperty() = default;

        ~GeometryProperty() override = default;

        void set(const Atlas::Message::Element&) override;

        GeometryProperty* copy() const override;

        /**
         * Creates a new shape instance for the supplied bounding box, and setting the center of mass offset.
         * @param bbox The bounding box of the entity for which the shape will be used.
         * @param centerOfMassOffset Out parameter for the center of mass offset.
         * @return A pair containing at least a collision shape as first entry. Ownership of this shape is passed to the caller.
         * Optionally there can also be as a second entry a shared pointer to a "backing" shape. Such a shape is shared between multiple instances, and deleted only
         * when all instances are deleted. Calling code needs to retain the shared pointer as long as the first collision shape is in use.
         */
        std::pair<btCollisionShape*, std::shared_ptr<btCollisionShape>> createShape(const WFMath::AxisBox<3>& bbox, btVector3& centerOfMassOffset) const;

    private:

        /**
         * Creator function used for creating a new shape instance.
         */
        std::function<std::pair<btCollisionShape*, std::shared_ptr<btCollisionShape>>(const WFMath::AxisBox<3>& bbox, const WFMath::Vector<3>& size, btVector3& centerOfMassOffset)> mShapeCreator;

        void buildMeshCreator();

        void parseMeshFile();
};

#endif /* RULESETS_GEOMETRYPROPERTY_H_ */
