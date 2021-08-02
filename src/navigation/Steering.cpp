/*
 Copyright (C) 2014 Erik Ogenvik

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

#include "Steering.h"
#include "Awareness.h"

#include "rules/ai/MemEntity.h"
#include "rules/ScaleProperty.h"
#include "rules/Vector3Property.h"
#include "rules/PhysicalProperties.h"

#include "Remotery.h"

#include <wfmath/point.h>
#include <wfmath/vector.h>
#include <wfmath/rotbox.h>
#include <wfmath/segment.h>


static const bool debug_flag = true;

Steering::Steering(MemEntity& avatar) :
        mAwareness(nullptr),
        mAvatar(avatar),
        mSteeringDestination{},
        mCurrentPathIndex(0),
        mSteeringEnabled(false),
        mUpdateNeeded(false),
        mPadding(16),
        mMaxSpeed(5),
        mDesiredSpeed(0.5),
        mExpectingServerMovement(false),
        mPathResult(0),
        mAvatarHorizRadius(0)
{
    auto speedGroundProp = avatar.getPropertyType<double>("speed_ground");
    if (speedGroundProp) {
        mMaxSpeed = speedGroundProp->data();
    }


    auto bbox = ScaleProperty::scaledBbox(mAvatar);
    if (bbox.isValid()) {
        mAvatarHorizRadius = std::sqrt(boxSquareHorizontalBoundingRadius(bbox));
    } else {
        log(WARNING, "Created Steering with avatar without any bounding box. This is unusual.");
    }

}

void Steering::setAwareness(Awareness* awareness)
{
    mAwareness = awareness;
    mTileListenerConnection.disconnect();
    if (mAwareness) {
        mTileListenerConnection = mAwareness->EventTileUpdated.connect(sigc::mem_fun(*this, &Steering::Awareness_TileUpdated));
        //setAwarenessArea();
    }
}


int Steering::queryDestination(const EntityLocation& destination, double currentServerTimestamp)
{
//    if (mAwareness) {
//
//        auto currentAvatarPos = getCurrentAvatarPosition(currentServerTimestamp);
//
//        WFMath::Point<3> finalPosition = WFMath::Point<3>::ZERO();
//        if (destination.m_pos.isValid()) {
//            finalPosition = destination.m_pos;
//        }
//        //TODO: handle other entities as destinations
//        if (destination.m_parent) {
//            updatePosition(currentServerTimestamp, mAvatar.getIntId(), currentAvatarPos);
//        }
//        std::vector<WFMath::Point<3>> path;
//        auto pathFindResult = mAwareness->findPath(currentAvatarPos, finalPosition, mDestinationRadius, path);
//        return pathFindResult;
//
//
////
////        float distanceAvatarDestination = WFMath::Distance(currentAvatarPos, finalPosition);
////
////        float finalRadius = radius;
////
////        //Check if the destination is too far away. If so we should adjust it closer, and increase the radius.
////        //This depends on the AI updating the destination at regular intervals.
////        if (distanceAvatarDestination > (mAwareness->getTileSizeInMeters() * 10)) {
////            WFMath::Vector<3> vector = finalPosition - currentAvatarPos;
////
////            finalPosition = currentAvatarPos + (vector.normalize() * mAwareness->getTileSizeInMeters() * 10);
////            finalRadius = (radius * 10.f);
////        }
////
////        //Only update if destination or radius has changed, or if the current tile of the avatar isn't known.
////        if (mViewDestination != finalPosition || mDestinationRadius != finalRadius
////            || !mAwareness->isPositionAware(currentAvatarPos.x(), currentAvatarPos.z())) {
////            mViewDestination = finalPosition;
////            mDestinationRadius = finalRadius;
////            mUpdateNeeded = true;
////
////            setAwarenessArea();
////            mAvatarPositionLastUpdate = currentAvatarPos;
////        }
//    }
    return -10;
}

void Steering::setDestination(SteeringDestination destination, double currentServerTimestamp)
{
    if (destination.location.m_pos != mSteeringDestination.location.m_pos || destination.location.m_parent != mSteeringDestination.location.m_parent) {
        mSteeringDestination = std::move(destination);
        mUpdateNeeded = true;
    }
    setAwarenessArea(currentServerTimestamp);
//    mSteeringDestination = std::move(destination);
//
//    if (mAwareness) {
//
//        auto currentAvatarPos = getCurrentAvatarPosition(currentServerTimestamp);
//
//        WFMath::Point<3> finalPosition = entityRelativePosition;
//        updatePosition(currentServerTimestamp, entityId, finalPosition);
//
//        float distanceAvatarDestination = WFMath::Distance(currentAvatarPos, finalPosition);
//
//        float finalRadius = radius;
//
//        //Check if the destination is too far away. If so we should adjust it closer, and increase the radius.
//        //This depends on the AI updating the destination at regular intervals.
//        if (distanceAvatarDestination > (mAwareness->getTileSizeInMeters() * 10)) {
//            WFMath::Vector<3> vector = finalPosition - currentAvatarPos;
//
//            finalPosition = currentAvatarPos + (vector.normalize() * mAwareness->getTileSizeInMeters() * 10);
//            finalRadius = (radius * 10.f);
//        }
//
//        //Only update if destination or radius has changed, or if the current tile of the avatar isn't known.
//        if (mViewDestination != finalPosition || mDestinationRadius != finalRadius
//            || !mAwareness->isPositionAware(currentAvatarPos.x(), currentAvatarPos.z())) {
//            mViewDestination = finalPosition;
//            mDestinationRadius = finalRadius;
//            mUpdateNeeded = true;
//
//            setAwarenessArea();
//            mAvatarPositionLastUpdate = currentAvatarPos;
//        }
//    }
}


void Steering::setAwarenessArea(double currentServerTimestamp)
{
    if (mAwareness) {
        auto resolvedPosition = resolvePosition(currentServerTimestamp, mSteeringDestination.location);
//        resolvePosition()


        if (resolvedPosition.position.isValid()) {
            WFMath::Point<2> destination2d(resolvedPosition.position.x(), resolvedPosition.position.z());
            auto pos = PositionProperty::extractPosition(mAvatar);
            WFMath::Point<2> entityPosition2d(pos.x(), pos.z());

            WFMath::Vector<2> direction(destination2d - entityPosition2d);
            auto theta = std::atan2(direction.y(), direction.x()); // rotation about Y
            WFMath::RotMatrix<2> rm;
            rm.rotation(theta);

            WFMath::Point<2> start = entityPosition2d;
            start -= WFMath::Vector<2>(mPadding, mPadding);

            WFMath::Vector<2> size(direction.mag() + (mPadding * 2), mPadding * 2);

            WFMath::RotBox<2> area;
            area.size() = size;
            area.corner0() = start;
            area.orientation() = WFMath::RotMatrix<2>().identity();
            area.rotatePoint(rm, entityPosition2d);

            mAwareness->setAwarenessArea(mAvatar.getId(), area, WFMath::Segment<2>(entityPosition2d, destination2d));
        }
    }
}

size_t Steering::unawareAreaCount() const
{
    if (mAwareness) {
        return mAwareness->unawareTilesInArea(mAvatar.getId());
    }
    return 0;
}


void Steering::setDesiredSpeed(float desiredSpeed)
{
    mDesiredSpeed = desiredSpeed;
}

int Steering::getPathResult() const
{
    return mPathResult;
}

int Steering::updatePath(double currentTimestamp, const WFMath::Point<3>& currentAvatarPosition)
{
    rmt_ScopedCPUSample(Steering_updatePath, 0)
    mPath.clear();
    mCurrentPathIndex = 0;
    if (!mAwareness) {
        mPathResult = -7;
        return mPathResult;
    }
    auto resolvedPosition = resolvePosition(currentTimestamp, mSteeringDestination.location);
    if (!resolvedPosition.position.isValid()) {
        mPathResult = -8;
        return mPathResult;
    }
    mPathResult = mAwareness->findPath(currentAvatarPosition, resolvedPosition.position, mSteeringDestination.distance, mPath);
    if (mPathResult == -1) {
        mAwareness->markTilesAsDirty(WFMath::AxisBox<2>(
                {currentAvatarPosition.x() - 5, currentAvatarPosition.z() - 5},
                {currentAvatarPosition.x() + 5, currentAvatarPosition.z() + 5}));
    } else if (mPathResult == -2) {
        mAwareness->markTilesAsDirty(WFMath::AxisBox<2>(
                {resolvedPosition.position.x() - 5, resolvedPosition.position.z() - 5},
                {resolvedPosition.position.x() + 5, resolvedPosition.position.z() + 5}));
    }
    //debug_print("Updating path, size of new path: " << result << ". Pos: " << currentAvatarPosition);
    EventPathUpdated();
    mUpdateNeeded = false;
    return mPathResult;
}

int Steering::updatePath(double currentTimestamp)
{
    if (!mAwareness) {
        return -1;
    }
    auto currentEntityPos = mAwareness->projectPosition(mAvatar.getIntId(), currentTimestamp);

    return updatePath(currentTimestamp, currentEntityPos);
}

void Steering::requestUpdate()
{
    mUpdateNeeded = true;
}

void Steering::startSteering()
{
    mSteeringEnabled = true;
    mExpectingServerMovement = false;
}

void Steering::stopSteering()
{
    if (!mSteeringEnabled) {
        return;
    }
    mSteeringEnabled = false;
    mExpectingServerMovement = false;
    mLastSentVelocity = WFMath::Vector<2>();

    //reset path
    mPath.clear();
    mCurrentPathIndex = 0;
    mPathResult = 0;
    EventPathUpdated();

}

bool Steering::isEnabled() const
{
    return mSteeringEnabled;
}

const std::vector<WFMath::Point<3>>& Steering::getPath() const
{
    return mPath;
}

size_t Steering::getCurrentPathIndex() const
{
    return mCurrentPathIndex;
}

WFMath::Point<3> Steering::getCurrentAvatarPosition(double currentTimestamp) const
{
    return mAwareness ? mAwareness->projectPosition(mAvatar.getIntId(), currentTimestamp) : WFMath::Point<3>{};
}

WFMath::Vector<3> Steering::directionTo(double currentTimestamp, const EntityLocation& location) const
{
    auto currentEntityPos = getCurrentAvatarPosition(currentTimestamp);
    auto resolvedDestination = resolvePosition(currentTimestamp, location);

    if (!currentEntityPos.isValid() || !resolvedDestination.position.isValid()) {
        return {};
    }

    return (resolvedDestination.position - currentEntityPos);
}

boost::optional<double> Steering::distanceTo(double currentTimestamp, const EntityLocation& location, MeasureType fromSelf, MeasureType toDestination) const
{
    auto currentEntityPos = getCurrentAvatarPosition(currentTimestamp);
    auto resolvedDestination = resolvePosition(currentTimestamp, location);

    if (!currentEntityPos.isValid() || !resolvedDestination.position.isValid()) {
        return boost::none;
    }

    auto distance = WFMath::Distance(WFMath::Point<2>(currentEntityPos.x(), currentEntityPos.z()), WFMath::Point<2>(resolvedDestination.position.x(), resolvedDestination.position.z()));
    if (fromSelf == MeasureType::EDGE) {
        distance -= mAvatarHorizRadius;
    }
    if (toDestination == MeasureType::EDGE) {
        distance -= resolvedDestination.radius;
//        if (resolvedDestination.location) {
//            auto& bbox = resolvedDestination.location->bBox();
//            if (bbox.isValid()) {
//                distance -= std::sqrt(boxSquareHorizontalBoundingRadius(bbox));
//            }
//        }
    }
    return std::max(0.0, distance);
}

Steering::ResolvedPosition Steering::resolvePosition(double currentTimestamp, const EntityLocation& location) const
{
    if (!mAwareness) {
        return {};
    }
    //If there's no parent at all we're operating within the same space as the avatar.
    if (!location.m_parent) {
        return {location.m_pos, 0};
    }
    //If the parent is the same we're just operating on position data without any volume.
    if (location.m_parent == mAvatar.m_parent) {
        //Just ignore any extra position supplied.
        return {location.pos(), 0};
    }
    //If the supplied destination doesn't belong to the current domain, walk upwards until we find the entity that does.
    //This entity will then determine the position.
    auto entity = location.m_parent;

    while (entity->m_parent && entity->m_parent != mAvatar.m_parent) {
        entity = entity->m_parent;
    }

    //Could not find the parent domain entity
    if (!entity) {
        return {};
    }

    double distance = 0;
    auto I = mAwareness->getObservedEntities().find(entity->getIntId());
    if (I != mAwareness->getObservedEntities().end()) {
        if (I->second->scaledBbox.isValid()) {
            distance = std::sqrt(boxSquareHorizontalBoundingRadius(I->second->scaledBbox));
        }
    }

    return {mAwareness->projectPosition(entity->getIntId(), currentTimestamp), distance};
}

bool Steering::isAtDestination(double currentTimestamp, const SteeringDestination& destination) const
{
    auto distance = distanceTo(currentTimestamp, destination.location, destination.measureFromAvatar, destination.measureToDestination);
    if (distance) {
        return *distance <= destination.distance;
    }
    return false;
}

bool Steering::isAtCurrentDestination(double currentTimestamp) const
{
    return isAtDestination(currentTimestamp, mSteeringDestination);
}


SteeringResult Steering::update(double currentTimestamp)
{
    rmt_ScopedCPUSample(Steering_update, 0)
    SteeringResult result{};
    if (!mSteeringEnabled) {
        auto propelProperty = mAvatar.getPropertyClass<Vector3Property>("_propel");
        if (propelProperty && propelProperty->data().isValid() && propelProperty->data() != WFMath::Vector<3>::ZERO()) {
            result.direction = WFMath::Vector<3>::ZERO();
        }
    } else if (mAwareness) {

        auto currentEntityPos = getCurrentAvatarPosition(currentTimestamp);

        if (mUpdateNeeded) {
            updatePath(currentTimestamp, currentEntityPos);
        }
        if (!mPath.empty()) {
            //First check if we've arrived at our actual destination.
            if (isAtCurrentDestination(currentTimestamp)) {
                //We've arrived at our destination. If we're moving we should stop.
                if (mLastSentVelocity != WFMath::Vector<2>::ZERO()) {
                    result.direction = WFMath::Vector<3>::ZERO();
                    mLastSentVelocity = WFMath::Vector<2>::ZERO();
                    mExpectingServerMovement = true;
                }
                stopSteering();
            } else {
                //We should send a move op if we're either not moving, or we've reached a waypoint, or we need to divert a lot.
                const WFMath::Point<2> entityPosition(currentEntityPos.x(), currentEntityPos.z());

                WFMath::Point<2> nextWaypoint(mPath[mCurrentPathIndex].x(), mPath[mCurrentPathIndex].z());
                while (WFMath::Distance(nextWaypoint, entityPosition) < mAvatarHorizRadius && mCurrentPathIndex < mPath.size() - 1) {
                    mCurrentPathIndex++;
                    nextWaypoint = WFMath::Point<2>(mPath[mCurrentPathIndex].x(), mPath[mCurrentPathIndex].z());
                }

                WFMath::Vector<2> distance = nextWaypoint - entityPosition;
                WFMath::Vector<2> velocityNorm = distance; //The velocity, normalized
                velocityNorm = velocityNorm.normalize() * mDesiredSpeed;
                WFMath::Point<2> destination;

                result.timeToNextWaypoint = distance.mag() / velocityNorm.mag();

                if (mCurrentPathIndex == mPath.size() - 1) {
                    //if the next waypoint is the destination we should send a "move to position" update to the server, to make sure that we stop when we've arrived.
                    //otherwise, if there's too much lag, we might end up overshooting our destination and will have to double back
                    destination = nextWaypoint;
                }

                //Check if we need to divert in order to avoid colliding.
                WFMath::Vector<2> newVelocity; //The new velocity after avoiding, not normalized.
                bool shouldSend = false;
                bool avoiding = mAwareness->avoidObstacles(mAvatar.getIntId(), entityPosition, velocityNorm * mMaxSpeed, newVelocity, currentTimestamp, &nextWaypoint);
                if (avoiding) {
                    //log(INFO, "Avoiding.");
                    velocityNorm = newVelocity / mMaxSpeed;

                    //Schedule a new steering op very soon
                    result.timeToNextWaypoint = std::min(*result.timeToNextWaypoint, 0.1);
                    mUpdateNeeded = true;
                    shouldSend = true;
                }

                if (velocityNorm.isValid()) {
                    if (mLastSentVelocity.isValid()) {
                        auto velocityProp = mAvatar.getPropertyClassFixed<VelocityProperty>();
                        //If the entity has stopped, and we're not waiting for confirmation to a movement request we've made, we need to start moving.
                        if (velocityProp && velocityProp->data() == WFMath::Vector<3>::ZERO() && !mExpectingServerMovement) {
                            shouldSend = true;
                        } else {
                            auto currentTheta = std::atan2(mLastSentVelocity.y(), mLastSentVelocity.x());
                            auto newTheta = std::atan2(velocityNorm.y(), velocityNorm.x());

                            //If we divert too much from where we need to go we must adjust.
                            if (std::abs(currentTheta - newTheta) > WFMath::numeric_constants<double>::pi() / 20) {
                                shouldSend = true;
                            }
                        }
                    } else {
                        //If we've never sent a movement update before we should do that now.
                        shouldSend = true;
                    }
                }
                if (shouldSend) {
                    //If we're moving to a certain destination and aren't avoiding anything we should tell the server to move to the destination.
                    if (destination.isValid() && !avoiding) {
                        auto posProp = mAvatar.getPropertyClassFixed<PositionProperty>();
                        auto y = posProp ? posProp->data().y() : 0.0;
                        result.destination = WFMath::Point<3>(destination.x(), y, destination.y());
                    }
                    result.direction = WFMath::Vector<3>(velocityNorm.x(), 0, velocityNorm.y());
                    mLastSentVelocity = velocityNorm;
                    mExpectingServerMovement = true;
                }
            }
        } else {
            //We are steering, but the path is empty, which means we can't find any path. If we're moving we should stop movement.
            //But we won't stop steering; perhaps we'll find a path later.
            if (mLastSentVelocity != WFMath::Vector<2>::ZERO()) {
                result.direction = WFMath::Vector<3>::ZERO();
                mLastSentVelocity = WFMath::Vector<2>::ZERO();
                mExpectingServerMovement = true;
            }
        }
    }
    return result;
}

void Steering::Awareness_TileUpdated(int tx, int ty)
{
    mUpdateNeeded = true;
}

bool Steering::getIsExpectingServerMovement() const
{
    return mExpectingServerMovement;
}

void Steering::setIsExpectingServerMovement(bool expected)
{
    mExpectingServerMovement = expected;
}

