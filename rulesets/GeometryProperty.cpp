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
#include "physics/Convert.h"
#include "common/log.h"
#include "common/globals.h"
#include "OgreMeshDeserializer.h"

#include <BulletCollision/CollisionShapes/btSphereShape.h>
#include <BulletCollision/CollisionShapes/btBoxShape.h>
#include <BulletCollision/CollisionShapes/btCylinderShape.h>
#include <BulletCollision/CollisionShapes/btTriangleIndexVertexArray.h>
#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>
#include <BulletCollision/CollisionShapes/btCapsuleShape.h>
#include <BulletCollision/CollisionShapes/btScaledBvhTriangleMeshShape.h>
#include <boost/algorithm/string.hpp>
#include <common/AtlasQuery.h>

const std::string GeometryProperty::property_name = "geometry";
const std::string GeometryProperty::property_atlastype = "map";

void GeometryProperty::set(const Atlas::Message::Element& data)
{
    Property<Atlas::Message::MapType>::set(data);

    auto createBoxFn = [&](const WFMath::AxisBox<3>& bbox, const WFMath::Vector<3>& size, btVector3& centerOfMassOffset) -> std::pair<btCollisionShape*, std::shared_ptr<btCollisionShape>> {
        auto btSize = Convert::toBullet(size * 0.5).absolute();
        centerOfMassOffset = -Convert::toBullet(bbox.getCenter());
        return std::make_pair(new btBoxShape(btSize), std::shared_ptr<btCollisionShape>());
    };

    auto I = m_data.find("shape");
    if (I != m_data.end() && I->second.isString()) {
        const std::string& shapeType = I->second.String();
        if (shapeType == "sphere") {
            mShapeCreator = [&](const WFMath::AxisBox<3>& bbox, const WFMath::Vector<3>& size, btVector3& centerOfMassOffset) -> std::pair<btCollisionShape*, std::shared_ptr<btCollisionShape>> {
                float minRadius = std::min(size.x(), std::min(size.y(), size.z())) * 0.5f;
                float xOffset = bbox.lowCorner().x() + minRadius;
                float yOffset = bbox.lowCorner().y() + minRadius;
                float zOffset = bbox.lowCorner().z() + minRadius;

                centerOfMassOffset = -btVector3(xOffset, yOffset, zOffset);
                return std::make_pair(new btSphereShape(minRadius), std::shared_ptr<btCollisionShape>());
            };
        } else if (shapeType == "capsule-y") {
            mShapeCreator = [&](const WFMath::AxisBox<3>& bbox, const WFMath::Vector<3>& size, btVector3& centerOfMassOffset) -> std::pair<btCollisionShape*, std::shared_ptr<btCollisionShape>> {
                centerOfMassOffset = -Convert::toBullet(bbox.getCenter());
                float minRadius = std::min(size.x(), size.z()) * 0.5f;
                //subtract the radius times 2 from the height
                float height = size.y() - (minRadius * 2.0f);
                //If the resulting height is negative we need to use a sphere instead.
                if (height > 0) {
                    return std::make_pair(new btCapsuleShape(minRadius, height), std::shared_ptr<btCollisionShape>());
                } else {
                    return std::make_pair(new btSphereShape(minRadius), std::shared_ptr<btCollisionShape>());
                }
            };

        } else if (shapeType == "capsule-x") {
            mShapeCreator = [&](const WFMath::AxisBox<3>& bbox, const WFMath::Vector<3>& size, btVector3& centerOfMassOffset) -> std::pair<btCollisionShape*, std::shared_ptr<btCollisionShape>> {
                centerOfMassOffset = -Convert::toBullet(bbox.getCenter());
                float minRadius = std::min(size.z(), size.y()) * 0.5f;
                //subtract the radius times 2 from the height
                float height = size.x() - (minRadius * 2.0f);
                //If the resulting height is negative we need to use a sphere instead.
                if (height > 0) {
                    return std::make_pair(new btCapsuleShapeX(minRadius, height), std::shared_ptr<btCollisionShape>());
                } else {
                    return std::make_pair(new btSphereShape(minRadius), std::shared_ptr<btCollisionShape>());
                }
            };
        } else if (shapeType == "capsule-z") {
            mShapeCreator = [&](const WFMath::AxisBox<3>& bbox, const WFMath::Vector<3>& size, btVector3& centerOfMassOffset) -> std::pair<btCollisionShape*, std::shared_ptr<btCollisionShape>> {
                centerOfMassOffset = -Convert::toBullet(bbox.getCenter());
                float minRadius = std::min(size.x(), size.y()) * 0.5f;
                //subtract the radius times 2 from the height
                float height = size.z() - (minRadius * 2.0f);
                //If the resulting height is negative we need to use a sphere instead.
                if (height > 0) {
                    return std::make_pair(new btCapsuleShapeZ(minRadius, height), std::shared_ptr<btCollisionShape>());
                } else {
                    return std::make_pair(new btSphereShape(minRadius), std::shared_ptr<btCollisionShape>());
                }
            };

        } else if (shapeType == "box") {
            mShapeCreator = createBoxFn;
        } else if (shapeType == "cylinder-y") {
            mShapeCreator = [&](const WFMath::AxisBox<3>& bbox, const WFMath::Vector<3>& size, btVector3& centerOfMassOffset) -> std::pair<btCollisionShape*, std::shared_ptr<btCollisionShape>> {
                centerOfMassOffset = -Convert::toBullet(bbox.getCenter());
                btCylinderShape* shape = new btCylinderShape(btVector3(1, 1, 1));
                shape->setLocalScaling(Convert::toBullet(size * 0.5f));
                return std::make_pair(shape, std::shared_ptr<btCollisionShape>());
            };


        } else if (shapeType == "cylinder-x") {
            mShapeCreator = [&](const WFMath::AxisBox<3>& bbox, const WFMath::Vector<3>& size, btVector3& centerOfMassOffset) -> std::pair<btCollisionShape*, std::shared_ptr<btCollisionShape>> {
                centerOfMassOffset = -Convert::toBullet(bbox.getCenter());
                btCylinderShape* shape = new btCylinderShapeX(btVector3(1, 1, 1));
                shape->setLocalScaling(Convert::toBullet(size * 0.5f));
                return std::make_pair(shape, std::shared_ptr<btCollisionShape>());
            };
        } else if (shapeType == "cylinder-z") {
            mShapeCreator = [&](const WFMath::AxisBox<3>& bbox, const WFMath::Vector<3>& size, btVector3& centerOfMassOffset) -> std::pair<btCollisionShape*, std::shared_ptr<btCollisionShape>> {
                centerOfMassOffset = -Convert::toBullet(bbox.getCenter());
                btCylinderShape* shape = new btCylinderShapeZ(btVector3(1, 1, 1));
                shape->setLocalScaling(Convert::toBullet(size * 0.5f));
                return std::make_pair(shape, std::shared_ptr<btCollisionShape>());
            };
        } else if (shapeType == "mesh") {
            buildMeshCreator();
        } else if (shapeType == "asset") {
            parseMeshFile();
        }
    }
}

std::pair<btCollisionShape*, std::shared_ptr<btCollisionShape>> GeometryProperty::createShape(const WFMath::AxisBox<3>& bbox, btVector3& centerOfMassOffset) const
{
    auto size = bbox.highCorner() - bbox.lowCorner();
    if (mShapeCreator) {
        return mShapeCreator(bbox, size, centerOfMassOffset);
    } else {
        auto btSize = Convert::toBullet(size * 0.5).absolute();
        centerOfMassOffset = -Convert::toBullet(bbox.getCenter());
        return std::make_pair(new btBoxShape(btSize), std::shared_ptr<btCollisionShape>());
    }
}


void GeometryProperty::buildMeshCreator()
{
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

            int numberOfVertices = (int) (vertsList.size() / 3);
            int numberOfTriangles = (int) (trisList.size() / 3);

            float* verts = new float[vertsList.size()];

            for (size_t i = 0; i < vertsList.size(); i += 3) {
                if (!vertsList[i].isFloat() || !vertsList[i + 1].isFloat() || !vertsList[i + 2].isFloat()) {
                    log(ERROR, "Vertex data was not a float for mesh.");
                    delete[] verts;
                    return;
                }
                verts[i] = (float) vertsList[i].Float();
                verts[i + 1] = (float) vertsList[i + 1].Float();
                verts[i + 2] = (float) vertsList[i + 2].Float();
            }

            int* indices = new int[trisList.size()];
            for (size_t i = 0; i < trisList.size(); i += 3) {
                if (!trisList[i].isInt() || !trisList[i + 1].isInt() || !trisList[i + 2].isInt()) {
                    log(ERROR, "Index data was not an int for mesh.");
                    delete[] verts;
                    delete[] indices;
                    return;
                }
                if (trisList[i].Int() >= numberOfVertices || trisList[i + 1].Int() >= numberOfVertices || trisList[i + 2].Int() >= numberOfVertices) {
                    log(ERROR, "Index data was out of bounds for vertices for mesh.");
                    delete[] verts;
                    delete[] indices;
                    return;
                }
                indices[i] = (int) trisList[i].Int();
                indices[i + 1] = (int) trisList[i + 1].Int();
                indices[i + 2] = (int) trisList[i + 2].Int();
            }

            int vertStride = sizeof(float) * 3;
            int indexStride = sizeof(int) * 3;

            btTriangleIndexVertexArray* triangleVertexArray = new btTriangleIndexVertexArray(numberOfTriangles, indices, indexStride, numberOfVertices, verts, vertStride);

            std::shared_ptr<btBvhTriangleMeshShape> meshShape(new btBvhTriangleMeshShape(triangleVertexArray, true, true), [triangleVertexArray, verts, indices](btBvhTriangleMeshShape* p) {
                delete triangleVertexArray;
                delete p;
                delete[] verts;
                delete[] indices;
            });
            meshShape->setLocalScaling(btVector3(1, 1, 1));

            mShapeCreator = [meshShape](const WFMath::AxisBox<3>& bbox, const WFMath::Vector<3>& size,
                                        btVector3& centerOfMassOffset) -> std::pair<btCollisionShape*, std::shared_ptr<btCollisionShape>> {
                //In contrast to other shapes there's no centerOfMassOffset for mesh shapes
                centerOfMassOffset = btVector3(0, 0, 0);
                btVector3 meshSize = meshShape->getLocalAabbMax() - meshShape->getLocalAabbMin();
                btVector3 scaling(size.x() / meshSize.x(), size.y() / meshSize.y(), size.z() / meshSize.z());
                btScaledBvhTriangleMeshShape* scaledShape = new btScaledBvhTriangleMeshShape(meshShape.get(), scaling);
                return std::make_pair(scaledShape, meshShape);
            };
        } else {
            log(ERROR, "Could not find list of triangles for mesh.");
        }
    } else {
        log(ERROR, "Could not find list of vertices for mesh.");
    }
}

GeometryProperty* GeometryProperty::copy() const
{
    return new GeometryProperty(*this);
}

void GeometryProperty::parseMeshFile()
{
    AtlasQuery::find<Atlas::Message::StringType>(data(), "path", [&](const auto& path) {
        try {
            if (boost::algorithm::ends_with(path, ".mesh")) {
                std::string fullpath = share_directory + "/cyphesis/assets/" + path;
                std::fstream fileStream(fullpath);
                if (fileStream) {
                    auto verts = new std::vector<float>();
                    auto indices = new std::vector<int>();

                    {
                        OgreMeshDeserializer deserializer(fileStream);
                        deserializer.deserialize();
                        *verts = std::move(deserializer.m_vertices);
                        *indices = std::move(deserializer.m_indices);
                    }


                    int vertStride = sizeof(float) * 3;
                    int indexStride = sizeof(int) * 3;

                    int numberOfVertices = (int) (verts->size() / 3);
                    int numberOfTriangles = (int) (indices->size() / 3);

                    btTriangleIndexVertexArray* triangleVertexArray = new btTriangleIndexVertexArray(numberOfTriangles, indices->data(), indexStride, numberOfVertices, verts->data(), vertStride);

                    std::shared_ptr<btBvhTriangleMeshShape> meshShape(new btBvhTriangleMeshShape(triangleVertexArray, true, true), [triangleVertexArray, verts, indices](btBvhTriangleMeshShape* p) {
                        delete triangleVertexArray;
                        delete p;
                        delete verts;
                        delete indices;
                    });
                    meshShape->setLocalScaling(btVector3(1, 1, 1));

                    mShapeCreator = [meshShape](const WFMath::AxisBox<3>& bbox, const WFMath::Vector<3>& size,
                                                btVector3& centerOfMassOffset) -> std::pair<btCollisionShape*, std::shared_ptr<btCollisionShape>> {
                        //In contrast to other shapes there's no centerOfMassOffset for mesh shapes
                        centerOfMassOffset = btVector3(0, 0, 0);
                        btVector3 meshSize = meshShape->getLocalAabbMax() - meshShape->getLocalAabbMin();
                        btVector3 scaling(size.x() / meshSize.x(), size.y() / meshSize.y(), size.z() / meshSize.z());
                        btScaledBvhTriangleMeshShape* scaledShape = new btScaledBvhTriangleMeshShape(meshShape.get(), scaling);
                        return std::make_pair(scaledShape, meshShape);
                    };

                } else {
                    log(ERROR, "Could not find geometry file at " + path);
                }
            } else {
                log(ERROR, "Could not recognize geometry file type: " + path);
            }
        } catch (const std::exception& ex) {
            log(ERROR, "Exception when trying to parse geometry at " + path);
        }
    });

}

