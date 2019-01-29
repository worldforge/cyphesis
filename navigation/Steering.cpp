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

#include "common/debug.h"

#include <wfmath/point.h>
#include <wfmath/vector.h>
#include <wfmath/rotbox.h>
#include <wfmath/segment.h>

#include <iostream>

static const bool debug_flag = true;

Steering::Steering(MemEntity& avatar) :
        mAwareness(nullptr),
        mAvatar(avatar),
        mDestinationEntityId(-1),
        mDestinationRadius(1.0),
        mCurrentPathIndex(0),
        mSteeringEnabled(false),
        mUpdateNeeded(false),
        mPadding(16),
        mMaxSpeed(5),
        mDesiredSpeed(0.5),
        mExpectingServerMovement(false),
        mPathResult(0),
        mAvatarHorizRadius(0.4)
{
    auto speedGroundProp = avatar.getPropertyType<double>("speed_ground");
    if (speedGroundProp) {
        mMaxSpeed = speedGroundProp->data();
    }

}

void Steering::setAwareness(Awareness* awareness)
{
    auto& bbox = mAvatar.m_location.bBox();
    if (bbox.isValid()) {
        WFMath::CoordType squareHorizRadius = std::max(square(bbox.lowCorner().x()) +
                                  square(bbox.lowCorner().z()),
                                  square(bbox.highCorner().x()) +
                                  square(bbox.highCorner().z()));
        mAvatarHorizRadius = std::sqrt(squareHorizRadius);
    }
    mAwareness = awareness;
    mTileListenerConnection.disconnect();
    if (mAwareness) {
        mTileListenerConnection = mAwareness->EventTileUpdated.connect(sigc::mem_fun(*this, &Steering::Awareness_TileUpdated));
        setAwarenessArea();
    }
}


int Steering::queryDestination(const EntityLocation& destination, double currentServerTimestamp)
{
    if (mAwareness) {

        auto currentAvatarPos = getCurrentAvatarPosition(currentServerTimestamp);

        WFMath::Point<3> finalPosition = WFMath::Point<3>::ZERO();
        if (destination.m_pos.isValid()) {
            finalPosition = destination.m_pos;
        }
        //TODO: handle other entities as destinations
        if (destination.m_parent) {
            updatePosition(currentServerTimestamp, mAvatar.getIntId(), currentAvatarPos);
        }
        std::vector<WFMath::Point<3>> path;
        auto pathFindResult = mAwareness->findPath(currentAvatarPos, finalPosition, mDestinationRadius, path);
        return pathFindResult;


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
    }
    return -10;
}



void Steering::setDestination(long entityId, const WFMath::Point<3>& entityRelativePosition, float radius, double currentServerTimestamp)
{
    if (mAwareness) {
        mDestinationEntityId = entityId;
        mEntityRelativeDestination = entityRelativePosition;

        auto currentAvatarPos = getCurrentAvatarPosition(currentServerTimestamp);

        WFMath::Point<3> finalPosition = entityRelativePosition;
        updatePosition(currentServerTimestamp, entityId, finalPosition);

        float distanceAvatarDestination = WFMath::Distance(currentAvatarPos, finalPosition);

        float finalRadius = radius;

        //Check if the destination is too far away. If so we should adjust it closer, and increase the radius.
        //This depends on the AI updating the destination at regular intervals.
        if (distanceAvatarDestination > (mAwareness->getTileSizeInMeters() * 10)) {
            WFMath::Vector<3> vector = finalPosition - currentAvatarPos;

            finalPosition = currentAvatarPos + (vector.normalize() * mAwareness->getTileSizeInMeters() * 10);
            finalRadius = (radius * 10.f);
        }

        //Only update if destination or radius has changed, or if the current tile of the avatar isn't known.
        if (mViewDestination != finalPosition || mDestinationRadius != finalRadius
                || !mAwareness->isPositionAware(currentAvatarPos.x(), currentAvatarPos.z())) {
            mViewDestination = finalPosition;
            mDestinationRadius = finalRadius;
            mUpdateNeeded = true;

            setAwarenessArea();
            mAvatarPositionLastUpdate = currentAvatarPos;
        }
    }
}

void Steering::setAwarenessArea()
{
    if (mAwareness) {
        if (mViewDestination.isValid()) {
            WFMath::Point<2> destination2d(mViewDestination.x(), mViewDestination.z());
            WFMath::Point<2> entityPosition2d(mAvatar.m_location.m_pos.x(), mAvatar.m_location.m_pos.z());

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

size_t Steering::unawareAreaCount() const {
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

void Steering::updatePosition(double currentServerTimestamp, long entityId, WFMath::Point<3>& pos) const {
    if (mAwareness && mAvatar.m_location.m_parent) {
        if (entityId != mAvatar.m_location.m_parent->getIntId()) {
            mAwareness->projectPosition(mDestinationEntityId, pos, currentServerTimestamp);
        }
    }
}


int Steering::updatePath(const WFMath::Point<3>& currentAvatarPosition)
{
    mPath.clear();
    mCurrentPathIndex = 0;
    if (!mAwareness) {
        mPathResult = -7;
        return mPathResult;
    }
    if (!mViewDestination.isValid()) {
        mPathResult = -8;
        return mPathResult;
    }
    mPathResult = mAwareness->findPath(currentAvatarPosition, mViewDestination, mDestinationRadius, mPath);
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
    auto currentEntityPos = mAvatar.m_location.m_pos;
    if (mAvatar.m_location.m_velocity.isValid()) {
        currentEntityPos += (mAvatar.m_location.m_velocity * (currentTimestamp - mAvatar.m_location.timeStamp()));
    }

    updatePosition(currentTimestamp, mDestinationEntityId, mViewDestination);

    return updatePath(currentEntityPos);
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

WFMath::Point<3> Steering::getCurrentAvatarPosition(double currentTimestamp)
{
    auto currentEntityPos = mAvatar.m_location.m_pos;
    if (mAvatar.m_location.m_velocity.isValid()) {
        currentEntityPos += (mAvatar.m_location.m_velocity * (currentTimestamp - mAvatar.m_location.timeStamp()));
    }
    return currentEntityPos;
}

SteeringResult Steering::update(double currentTimestamp)
{
    SteeringResult result{};
    if (mSteeringEnabled && mAwareness) {

        auto currentEntityPos = getCurrentAvatarPosition(currentTimestamp);

        //if (mUpdateNeeded) {
        updatePosition(currentTimestamp, mDestinationEntityId, mViewDestination);
        updatePath(currentEntityPos);
        //}
        if (!mPath.empty()) {
            const auto& finalDestination = mPath.back();

            const WFMath::Point<2> entityPosition(currentEntityPos.x(), currentEntityPos.z());
            //First check if we've arrived at our actual destination.
            if (WFMath::Distance(WFMath::Point<2>(finalDestination.x(), finalDestination.z()), entityPosition) < mAvatarHorizRadius) {
                //We've arrived at our destination. If we're moving we should stop.
                if (mLastSentVelocity != WFMath::Vector<2>::ZERO()) {
                    result.direction = WFMath::Vector<3>::ZERO();
                    mLastSentVelocity = WFMath::Vector<2>::ZERO();
                    mExpectingServerMovement = true;
                }
                stopSteering();
            } else {
                //We should send a move op if we're either not moving, or we've reached a waypoint, or we need to divert a lot.

                WFMath::Point<2> nextWaypoint(mPath[mCurrentPathIndex].x(), mPath[mCurrentPathIndex].z());
                while (WFMath::Distance(nextWaypoint, entityPosition) < mAvatarHorizRadius && mCurrentPathIndex < mPath.size() - 1) {
                    mCurrentPathIndex++;
                    nextWaypoint = WFMath::Point<2>(mPath[mCurrentPathIndex].x(), mPath[mCurrentPathIndex].z());
                }

                WFMath::Vector<2> distance = nextWaypoint - entityPosition;
                WFMath::Vector<2> velocity = distance;
                WFMath::Point<2> destination;
                velocity = velocity.normalize() * mDesiredSpeed;

                result.timeToNextWaypoint = distance.mag() / mMaxSpeed;

                if (mCurrentPathIndex == mPath.size() - 1) {
                    //if the next waypoint is the destination we should send a "move to position" update to the server, to make sure that we stop when we've arrived.
                    //otherwise, if there's too much lag, we might end up overshooting our destination and will have to double back
                    destination = nextWaypoint;
                }

                //Check if we need to divert in order to avoid colliding.
                WFMath::Vector<2> newVelocity;
                bool avoiding = mAwareness->avoidObstacles(mAvatar.getIntId(), entityPosition, velocity * mMaxSpeed, newVelocity, currentTimestamp);
                if (avoiding) {
                    auto newMag = newVelocity.mag();
                    auto relativeMag = mMaxSpeed / newMag;

                    velocity = newVelocity;
                    velocity.normalize();
                    velocity *= relativeMag;

                    //Schedule a new steering op very soon
                    result.timeToNextWaypoint = 0.2f;
                    mUpdateNeeded = true;
                }

                bool shouldSend = false;
                if (velocity.isValid()) {
                    if (mLastSentVelocity.isValid()) {
                        //If the entity has stopped, and we're not waiting for confirmation to a movement request we've made, we need to start moving.
                        if (mAvatar.m_location.velocity() == WFMath::Vector<3>::ZERO() && !mExpectingServerMovement) {
                            shouldSend = true;
                        } else {
                            auto currentTheta = std::atan2(mLastSentVelocity.y(), mLastSentVelocity.x());
                            auto newTheta = std::atan2(velocity.y(), velocity.x());

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
                        result.destination = WFMath::Point<3>(destination.x(), mAvatar.m_location.m_pos.y(), destination.y());
                    }
                    result.direction = WFMath::Vector<3>(velocity.x(), 0, velocity.y());
                    mLastSentVelocity = velocity;
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

