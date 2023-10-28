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

#include "GeometryProperty.h"
#include "OgreMeshDeserializer.h"
#include "rules/BBoxProperty.h"
#include "physics/Convert.h"
#include "common/log.h"
#include "common/globals.h"
#include "common/TypeNode.h"
#include "common/debug.h"
#include "common/compose.hpp"
#include "common/AtlasQuery.h"
#include "common/AssetsManager.h"
#include "common/Inheritance.h"

#include <wfmath/atlasconv.h>

#include <BulletCollision/CollisionShapes/btSphereShape.h>
#include <BulletCollision/CollisionShapes/btBoxShape.h>
#include <BulletCollision/CollisionShapes/btCylinderShape.h>
#include <BulletCollision/CollisionShapes/btTriangleIndexVertexArray.h>
#include <BulletCollision/CollisionShapes/btCapsuleShape.h>
#include <BulletCollision/CollisionShapes/btScaledBvhTriangleMeshShape.h>
#include <BulletCollision/CollisionShapes/btCompoundShape.h>
#include <BulletCollision/CollisionShapes/btConvexTriangleMeshShape.h>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/fstream.hpp>
#include "rules/LocatedEntity.h"

auto createBoxFn = [](const WFMath::AxisBox<3>& bbox, const WFMath::Vector<3>& size, btVector3& centerOfMassOffset, float)
        -> std::shared_ptr<btCollisionShape> {
    auto btSize = Convert::toBullet(size * 0.5).absolute();
    centerOfMassOffset = -Convert::toBullet(bbox.getCenter());
    return std::make_shared<btBoxShape>(btSize);
};

void GeometryProperty::set(const Atlas::Message::Element& data)
{
    Property<Atlas::Message::MapType>::set(data);


    std::shared_ptr<OgreMeshDeserializer> deserializer;
    AtlasQuery::find<std::string>(data, "path", [&](const std::string& path) {
        try {
            if (boost::algorithm::ends_with(path, ".mesh")) {
                auto fullpath = AssetsManager::instance().getAssetsPath() / path;
                AssetsManager::instance().observeFile(fullpath, [this, fullpath](const boost::filesystem::path& changedPath) {

                    log(NOTICE, String::compose("Reloading geometry from %1.", fullpath));
                    boost::filesystem::ifstream fileStream(fullpath);
                    if (fileStream) {
                        auto innerDeserializer = std::make_shared<OgreMeshDeserializer>(fileStream);
                        innerDeserializer->deserialize();
                        m_meshBounds = innerDeserializer->m_bounds;
                        parseData(std::move(innerDeserializer));


                        struct my_visitor : public boost::static_visitor<>
                        {
                            GeometryProperty* prop;

                            void operator()(LocatedEntity*) const
                            {
                            }

                            void operator()(TypeNode* typeNode) const
                            {
                                prop->install(*typeNode, "");
                            }
                        };

                        my_visitor visitor{};
                        visitor.prop = this;
                        m_owner.apply_visitor(visitor);
                    } else {
                        log(ERROR, "Could not reload geometry file at " + fullpath.string());
                    }
                });


                boost::filesystem::ifstream fileStream(fullpath);
                if (fileStream) {
                    deserializer.reset(new OgreMeshDeserializer(fileStream));
                    deserializer->deserialize();
                    m_meshBounds = deserializer->m_bounds;
                } else {
                    log(ERROR, "Could not find geometry file at " + fullpath.string());
                }
            } else {
                log(ERROR, "Could not recognize geometry file type: " + path);
            }
        } catch (const std::exception& ex) {
            log(ERROR, "Exception when trying to parse geometry at " + path);
        }
    });

    parseData(std::move(deserializer));
}


void GeometryProperty::parseData(std::shared_ptr<OgreMeshDeserializer> deserializer)
{

    auto sphereCreator = [](float radius, const WFMath::AxisBox<3>& bbox, const WFMath::Vector<3>& size, btVector3& centerOfMassOffset)
            -> std::shared_ptr<btCollisionShape> {
        float xOffset = bbox.lowCorner().x() + (size.x() / 2.0f);
        float yOffset = bbox.lowCorner().y() + (size.y() / 2.0f);
        float zOffset = bbox.lowCorner().z() + (size.z() / 2.0f);

        centerOfMassOffset = -btVector3(xOffset, yOffset, zOffset);
        return std::make_shared<btSphereShape>(radius);
    };

    auto scalerType = parseScalerType();

    auto I = m_data.find("type");
    if (I != m_data.end() && I->second.isString()) {
        const std::string& shapeType = I->second.String();
        if (shapeType == "sphere") {
            mShapeCreator = [sphereCreator, scalerType](const WFMath::AxisBox<3>& bbox, const WFMath::Vector<3>& size, btVector3& centerOfMassOffset, float)
                    -> std::shared_ptr<btCollisionShape> {
                double radius = 0;
                switch (scalerType) {
                    case ScalerType::Min:
                        radius = std::min(size.x(), std::min(size.y(), size.z())) * 0.5;
                        break;
                    case ScalerType::Max:
                        radius = std::max(size.x(), std::max(size.y(), size.z())) * 0.5;
                        break;
                    case ScalerType::XAxis:
                        radius = size.x() * 0.5;
                        break;
                    case ScalerType::YAxis:
                        radius = size.y() * 0.5;
                        break;
                    case ScalerType::ZAxis:
                        radius = size.z() * 0.5;
                        break;
                }
                return sphereCreator(radius, bbox, size, centerOfMassOffset);
            };
        } else if (shapeType == "capsule-y") {
            mShapeCreator = [sphereCreator, scalerType](const WFMath::AxisBox<3>& bbox, const WFMath::Vector<3>& size, btVector3& centerOfMassOffset, float)
                    -> std::shared_ptr<btCollisionShape> {
                centerOfMassOffset = -Convert::toBullet(bbox.getCenter());

                double radius = 0;
                switch (scalerType) {
                    case ScalerType::Min:
                        radius = std::min(size.x(), size.z()) * 0.5;
                        break;
                    case ScalerType::Max:
                        radius = std::max(size.x(), size.z()) * 0.5;
                        break;
                    case ScalerType::XAxis:
                        radius = size.x() * 0.5;
                        break;
                    case ScalerType::YAxis:
                        radius = size.y() * 0.5;
                        break;
                    case ScalerType::ZAxis:
                        radius = size.z() * 0.5;
                        break;
                }

                //subtract the radius times 2 from the height
                float height = size.y() - (radius * 2.0f);
                //If the resulting height is negative we need to use a sphere instead.
                if (height > 0) {
                    return std::make_shared<btCapsuleShape>(radius, height);
                } else {
                    return sphereCreator(size.y() * 0.5f, bbox, size, centerOfMassOffset);
                }
            };

        } else if (shapeType == "capsule-x") {
            mShapeCreator = [sphereCreator, scalerType](const WFMath::AxisBox<3>& bbox, const WFMath::Vector<3>& size, btVector3& centerOfMassOffset, float)
                    -> std::shared_ptr<btCollisionShape> {
                centerOfMassOffset = -Convert::toBullet(bbox.getCenter());
                float radius = 0;
                switch (scalerType) {
                    case ScalerType::Min:
                        radius = std::min(size.y(), size.z()) * 0.5f;
                        break;
                    case ScalerType::Max:
                        radius = std::max(size.y(), size.z()) * 0.5f;
                        break;
                    case ScalerType::XAxis:
                        radius = size.x() * 0.5f;
                        break;
                    case ScalerType::YAxis:
                        radius = size.y() * 0.5f;
                        break;
                    case ScalerType::ZAxis:
                        radius = size.z() * 0.5f;
                        break;
                }
                //subtract the radius times 2 from the height
                float height = size.x() - (radius * 2.0f);
                //If the resulting height is negative we need to use a sphere instead.
                if (height > 0) {
                    return std::make_shared<btCapsuleShapeX>(radius, height);
                } else {
                    return sphereCreator(size.x() * 0.5f, bbox, size, centerOfMassOffset);
                }
            };
        } else if (shapeType == "capsule-z") {
            mShapeCreator = [sphereCreator, scalerType](const WFMath::AxisBox<3>& bbox, const WFMath::Vector<3>& size, btVector3& centerOfMassOffset, float)
                    -> std::shared_ptr<btCollisionShape> {
                centerOfMassOffset = -Convert::toBullet(bbox.getCenter());
                float radius = 0;
                switch (scalerType) {
                    case ScalerType::Min:
                        radius = std::min(size.y(), size.x()) * 0.5f;
                        break;
                    case ScalerType::Max:
                        radius = std::max(size.y(), size.x()) * 0.5f;
                        break;
                    case ScalerType::XAxis:
                        radius = size.x() * 0.5f;
                        break;
                    case ScalerType::YAxis:
                        radius = size.y() * 0.5f;
                        break;
                    case ScalerType::ZAxis:
                        radius = size.z() * 0.5f;
                        break;
                }
                //subtract the radius times 2 from the height
                float height = size.z() - (radius * 2.0f);
                //If the resulting height is negative we need to use a sphere instead.
                if (height > 0) {
                    return std::make_shared<btCapsuleShapeZ>(radius, height);
                } else {
                    return sphereCreator(size.z() * 0.5f, bbox, size, centerOfMassOffset);
                }
            };
        } else if (shapeType == "box") {
            mShapeCreator = createBoxFn;
        } else if (shapeType == "cylinder-y") {
            mShapeCreator = [](const WFMath::AxisBox<3>& bbox, const WFMath::Vector<3>& size, btVector3& centerOfMassOffset, float)
                    -> std::shared_ptr<btCollisionShape> {
                centerOfMassOffset = -Convert::toBullet(bbox.getCenter());
                auto shape = std::make_shared<btCylinderShape>(btVector3(1, 1, 1));
                shape->setLocalScaling(Convert::toBullet(size * 0.5f));
                return shape;
            };
        } else if (shapeType == "cylinder-x") {
            mShapeCreator = [](const WFMath::AxisBox<3>& bbox, const WFMath::Vector<3>& size, btVector3& centerOfMassOffset, float)
                    -> std::shared_ptr<btCollisionShape> {
                centerOfMassOffset = -Convert::toBullet(bbox.getCenter());
                auto shape = std::make_shared<btCylinderShapeX>(btVector3(1, 1, 1));
                shape->setLocalScaling(Convert::toBullet(size * 0.5f));
                return shape;
            };
        } else if (shapeType == "cylinder-z") {
            mShapeCreator = [](const WFMath::AxisBox<3>& bbox, const WFMath::Vector<3>& size, btVector3& centerOfMassOffset, float)
                    -> std::shared_ptr<btCollisionShape> {
                centerOfMassOffset = -Convert::toBullet(bbox.getCenter());
                auto shape = std::make_shared<btCylinderShapeZ>(btVector3(1, 1, 1));
                shape->setLocalScaling(Convert::toBullet(size * 0.5f));
                return shape;
            };
        } else if (shapeType == "mesh") {
            buildMeshCreator(std::move(deserializer));
        } else if (shapeType == "compound") {
            buildCompoundCreator();
        }
    } else {
        log(WARNING, "Geometry property without 'type' attribute set. Property value: " + debug_tostring(m_data));
    }
}

std::shared_ptr<btCollisionShape> GeometryProperty::createShape(const WFMath::AxisBox<3>& bbox, btVector3& centerOfMassOffset, float mass) const
{
    auto size = bbox.highCorner() - bbox.lowCorner();
    if (mShapeCreator) {
        return mShapeCreator(bbox, size, centerOfMassOffset, mass);
    } else {
        auto btSize = Convert::toBullet(size * 0.5).absolute();
        centerOfMassOffset = -Convert::toBullet(bbox.getCenter());
        return std::make_shared<btBoxShape>(btSize);
    }
}


void GeometryProperty::buildMeshCreator(std::shared_ptr<OgreMeshDeserializer> meshDeserializer)
{
    //Shared pointers since we want these values to survive as long as "meshShape" is alive.
    auto verts = std::make_shared<std::vector<btScalar>>();
    auto indices = std::make_shared<std::vector<unsigned int>>();

    if (!meshDeserializer) {

        auto vertsI = m_data.find("vertices");
        if (vertsI != m_data.end() && vertsI->second.isList()) {
            auto trisI = m_data.find("indices");
            if (trisI != m_data.end() && trisI->second.isList()) {
                auto& vertsList = vertsI->second.List();
                auto& trisList = trisI->second.List();

                if (vertsList.empty()) {
                    log(ERROR, "Vertices is empty for mesh.");
                    return;
                }

                if (vertsList.size() % 3 != 0) {
                    log(ERROR, "Vertices is not even with 3.");
                    return;
                }

                if (trisList.empty()) {
                    log(ERROR, "Triangles is empty for mesh.");
                    return;
                }

                if (trisList.size() % 3 != 0) {
                    log(ERROR, "Triangles is not even with 3.");
                    return;
                }

                int numberOfVertices = static_cast<int>(vertsList.size() / 3);

                auto& local_verts = *verts.get();
                auto& local_indices = *indices.get();
                local_verts.resize(vertsList.size());

                for (size_t i = 0; i < vertsList.size(); i += 3) {
                    if (!vertsList[i].isFloat() || !vertsList[i + 1].isFloat() || !vertsList[i + 2].isFloat()) {
                        log(ERROR, "Vertex data was not a float for mesh.");
                        return;
                    }
                    local_verts[i] = (float) vertsList[i].Float();
                    local_verts[i + 1] = (float) vertsList[i + 1].Float();
                    local_verts[i + 2] = (float) vertsList[i + 2].Float();
                }

                local_indices.resize(trisList.size());
                for (size_t i = 0; i < trisList.size(); i += 3) {
                    if (!trisList[i].isInt() || !trisList[i + 1].isInt() || !trisList[i + 2].isInt()) {
                        log(ERROR, "Index data was not an int for mesh.");
                        return;
                    }
                    if (trisList[i].Int() >= numberOfVertices || trisList[i + 1].Int() >= numberOfVertices || trisList[i + 2].Int() >= numberOfVertices) {
                        log(ERROR, "Index data was out of bounds for vertices for mesh.");
                        return;
                    }
                    local_indices[i] = (unsigned int) trisList[i].Int();
                    local_indices[i + 1] = (unsigned int) trisList[i + 1].Int();
                    local_indices[i + 2] = (unsigned int) trisList[i + 2].Int();
                }


            } else {
                log(ERROR, "Could not find list of triangles for mesh.");
            }
        } else {
            log(ERROR, "Could not find list of vertices for mesh.");
        }
    } else {
        *indices = std::move(meshDeserializer->m_indices);
        *verts = std::move(meshDeserializer->m_vertices);
    }


    if (indices->empty() || verts->empty()) {
        log(ERROR, "Vertices or indices were empty.");
        return;
    }

    for (auto index : *indices) {
        if (index >= verts->size() / 3) {
            log(ERROR, "Index out of bounds.");
            return;
        }
    }

    int vertStride = sizeof(btScalar) * 3;
    int indexStride = sizeof(unsigned int) * 3;

    int indicesCount = static_cast<int>(indices->size() / 3);
    int vertexCount = static_cast<int>(verts->size() / 3);

    //Make sure to capture "verts" and "indices" so that they are kept around.
    std::shared_ptr<btTriangleIndexVertexArray> triangleVertexArray(new btTriangleIndexVertexArray(indicesCount, reinterpret_cast<int*>(indices->data()), indexStride,
                                                                                                   vertexCount, verts->data(), vertStride),
                                                                    [verts, indices](btTriangleIndexVertexArray* p) {
                                                                        delete p;
                                                                    });

    btVector3 aabbMin, aabbMax;
    triangleVertexArray->calculateAabbBruteForce(aabbMin, aabbMax);
    triangleVertexArray->setPremadeAabb(aabbMin, aabbMax);

    std::shared_ptr<btBvhTriangleMeshShape> meshShape(new btBvhTriangleMeshShape(triangleVertexArray.get(), true, true),
                                                      [triangleVertexArray](btBvhTriangleMeshShape* p) {
                                                          delete p;
                                                      });
    meshShape->setLocalScaling(btVector3(1, 1, 1));
    //Store the bounds, so that the "bbox" property can be updated when this is applied to a TypeNode
    m_meshBounds = WFMath::AxisBox<3>(Convert::toWF<WFMath::Point<3>>(meshShape->getLocalAabbMin()),
                                      Convert::toWF<WFMath::Point<3>>(meshShape->getLocalAabbMax()));

    mShapeCreator = [meshShape, verts, triangleVertexArray](const WFMath::AxisBox<3>& bbox, const WFMath::Vector<3>& size,
                                                            btVector3& centerOfMassOffset, float mass) -> std::shared_ptr<btCollisionShape> {
        //In contrast to other shapes there's no centerOfMassOffset for mesh shapes
        centerOfMassOffset = btVector3(0, 0, 0);
        btVector3 meshSize = meshShape->getLocalAabbMax() - meshShape->getLocalAabbMin();
        btVector3 scaling(size.x() / meshSize.x(), size.y() / meshSize.y(), size.z() / meshSize.z());

        //Due to performance reasons we should use different shapes depending on whether it's static (i.e. mass == 0) or not
        if (mass == 0) {
            //Hold on to meshShape as long as the scaled mesh exists.
            return std::shared_ptr<btScaledBvhTriangleMeshShape>(new btScaledBvhTriangleMeshShape(meshShape.get(), scaling), [meshShape](btScaledBvhTriangleMeshShape* p) {
                delete p;
            });
        } else {

            auto shape = new btConvexTriangleMeshShape(triangleVertexArray.get(), true);
/**
            auto shape = new btConvexHullShape(verts.get()->data(), verts.get()->size() / 3, sizeof(float) * 3);

            //btConvexHullShape::optimizeConvexHull was introduced in 2.84. It's useful, but not necessary.
            //version number 285 corresponds to version 2.84...
            #if BT_BULLET_VERSION > 284
            shape->optimizeConvexHull();
            #endif
            shape->recalcLocalAabb();
            */
            shape->setLocalScaling(scaling);
            return std::shared_ptr<btConvexTriangleMeshShape>(shape);
        }

    };


}

GeometryProperty* GeometryProperty::copy() const
{
    return new GeometryProperty(*this);
}


void GeometryProperty::install(TypeNode& typeNode, const std::string&)
{
    m_owner = &typeNode;

    //If there are valid mesh bounds read, and there's no bbox property already, add one.
    if (m_meshBounds.isValid()) {
        auto I = typeNode.defaults().find(BBoxProperty::property_name);
        //Create a new property if either there isn't one, or the existing one is calculated from geometry (which we detect by checking if it's ephemeral).
        if (I == typeNode.defaults().end() || I->second->flags().hasFlags(prop_flag_persistence_ephem)) {
            //Update the bbox property of the type if there are valid bounds from the mesh.
            auto bBoxProperty = std::make_unique<BBoxProperty>();
            bBoxProperty->data() = m_meshBounds;
            //Mark the property as ephemeral since it's calculated.
            bBoxProperty->addFlags(prop_flag_class | prop_flag_persistence_ephem);
            bBoxProperty->flags().addFlags(PropertyUtil::flagsForPropertyName(BBoxProperty::property_name));
            bBoxProperty->install(typeNode, BBoxProperty::property_name);
            auto update = typeNode.injectProperty(BBoxProperty::property_name, std::move(bBoxProperty));

            Inheritance::instance().typesUpdated({{&typeNode, update}});
        }
    }
}

void GeometryProperty::buildCompoundCreator()
{
    mShapeCreator = [&](const WFMath::AxisBox<3>& bbox, const WFMath::Vector<3>& size,
                        btVector3& centerOfMassOffset, float mass) -> std::shared_ptr<btCollisionShape> {
        auto I = m_data.find("shapes");
        if (I != m_data.end() && I->second.isList()) {
            auto shapes = I->second.List();
#if BT_BULLET_VERSION > 283
            auto compoundShape = new btCompoundShape(true, static_cast<int>(shapes.size()));
#else
            auto compoundShape = new btCompoundShape(true);
#endif
            std::vector<btCollisionShape*> childShapes(shapes.size());
            for (auto& shapeElement : shapes) {
                if (shapeElement.isMap()) {
                    auto& shapeMap = shapeElement.Map();
                    AtlasQuery::find<std::string>(shapeMap, "type", [&](const std::string& type) {
                        if (type == "box") {
                            AtlasQuery::find<Atlas::Message::ListType>(shapeMap, "points", [&](const Atlas::Message::ListType& points) {
                                WFMath::AxisBox<3> shapeBox(points);

                                btTransform transform(btQuaternion::getIdentity());
                                transform.setOrigin(Convert::toBullet(shapeBox.getCenter()));

                                AtlasQuery::find<Atlas::Message::ListType>(shapeMap, "orientation", [&](const Atlas::Message::ListType& orientationList) {
                                    transform.setRotation(Convert::toBullet(WFMath::Quaternion(orientationList)));

                                });

                                auto boxSize = shapeBox.highCorner() - shapeBox.lowCorner();

                                btBoxShape* boxShape = new btBoxShape(Convert::toBullet(boxSize / 2.f));
                                childShapes.emplace_back(boxShape);
                                compoundShape->addChildShape(transform, boxShape);
                            });
                        } else {
                            //TODO: implement more shapes when needed. "box" should go a long way though.
                            log(WARNING, String::compose("Unrecognized compound shape type '%1'.", type));
                        }
                    });

                }
            }

            btVector3 aabbMin, aabbMax;
            compoundShape->getAabb(btTransform::getIdentity(), aabbMin, aabbMax);

            centerOfMassOffset = btVector3(0, 0, 0);
            btVector3 meshSize = aabbMax - aabbMin;
            btVector3 scaling(size.x() / meshSize.x(), size.y() / meshSize.y(), size.z() / meshSize.z());
            compoundShape->setLocalScaling(scaling);

            return std::shared_ptr<btCollisionShape>(compoundShape, [childShapes](btCollisionShape* p) {
                for (btCollisionShape* childShape : childShapes) {
                    delete childShape;
                }
                delete p;
            });
        }
        return createBoxFn(bbox, size, centerOfMassOffset, mass);
    };

}

GeometryProperty::ScalerType GeometryProperty::parseScalerType()
{
    auto I = m_data.find("scaler");

    if (I != m_data.end() && I->second.isString()) {
        auto& radiusTypeString = I->second.String();
        if (radiusTypeString == "min") {
            return ScalerType::Min;
        } else if (radiusTypeString == "max") {
            return ScalerType::Max;
        } else if (radiusTypeString == "x") {
            return ScalerType::XAxis;
        } else if (radiusTypeString == "y") {
            return ScalerType::YAxis;
        } else if (radiusTypeString == "z") {
            return ScalerType::ZAxis;
        }
    }

    //Default to minimum size
    return ScalerType::Min;
}

