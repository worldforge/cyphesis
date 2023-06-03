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

#ifndef STEERING_H_
#define STEERING_H_

#include <wfmath/point.h>
#include <wfmath/vector.h>
#include <wfmath/axisbox.h>

#include <vector>

#include <sigc++/trackable.h>
#include <sigc++/signal.h>
#include <sigc++/connection.h>

#include <boost/optional.hpp>
#include <rules/Location.h>
#include "rules/ai/MemEntity.h"


class Awareness;

class MemEntity;

/**
 * @brief Results of a steering update.
 *
 * A new update should only be sent if direction is a valid vector.
 */
struct SteeringResult
{

    /**
     * Next destination, if set.
     * An invalid destination means that no specific destination should be set for next update.
     */
    WFMath::Point<3> destination;

    /**
     * The direction of movement
     */
    WFMath::Vector<3> direction;

    /**
     * Time until the next waypoint is reached.
     * This is used for scheduling the next update tick.
     */
    boost::optional<double> timeToNextWaypoint;
};


struct EntityLocation;

/**
 * @brief Handles steering of an avatar, with path finding and obstacle avoidance.
 *
 * To enable steering, first set the destination through setDestination() and then call startSteering().
 */
class Steering : public virtual sigc::trackable
{
    public:

        enum class MeasureType
        {
                /**
                 * Measure from the center.
                 */
                        CENTER,

                /**
                 * Measure from the edge. This only applies if a Location/LocatedEntity is being used.
                 */
                        EDGE
        };

        struct SteeringDestination
        {
            /**
             * The destination.
             * Interpretation depends on whether position or parent or both are set.
             * 1) If there's no parent, or the parent is the domain entity, then only position is used. This ignores the measureToDestination value (since we can't use EDGE).
             * 2) If the parent is set to something else than the domain entity we'll walk upwards until we find the entity which belongs to the domain. The used position will be
             *    of that entity, and the radius used if measureToDestination is set to EDGE will be the radius of that entity. Position will be ignored (for now)
             *
             * The effect of this is that there are basically two main different ways of setting a destination.
             * Either you set a specific point in the domain.
             * Or you set it to track a certain entity. In this case you can also use measureToDestination in EDGE mode.
             *
             */
            EntityLocation location;

            /**
             * How to measure from the avatar.
             */
            MeasureType measureFromAvatar;

            /**
             * How to measure to the destination. EDGE can only be used if the location is set to an Entity.
             */
            MeasureType measureToDestination;

            /**
             * How close we need to be, minus any edges (depending on the MeasureTypes set).
             */
            double distance;

//            bool operator==(const SteeringDestination& rhs) const
//            {
//                return destination.location == rhs.destination.location
//                       && measureFromAvatar == rhs.measureFromAvatar
//                       && measureToDestination == rhs.measureToDestination
//                       && rhs.distance == rhs.distance;
//            }
//
//            bool operator!=(const SteeringDestination& rhs) const
//            {
//                return !(*this == rhs);
//            }
        };

        struct ResolvedPosition
        {
            /**
             * The position relative to our own parent.
             */
            WFMath::Point<3> position;

            /**
             * The radius of the entity which is a direct child of our own parent. This is mainly of use when trying to
             * reach an entity which is contained in another entity, and we really need to know if we can reach
             * the other entity.
             */
            double radius;
        };

        explicit Steering(MemEntity& avatar);

        virtual ~Steering() = default;

        void setAwareness(Awareness* awareness);

        int queryDestination(const EntityLocation& destination, double currentServerTimestamp);

        void setDestination(SteeringDestination destination, double currentServerTimestamp);

        /**
         * @brief Updates the path.
         * @param currentAvatarPosition The current position of the avatar entity.
         * @return If >= 0, a path was found. If <0 and >= -3 we couldn't find a path currently because we don't have all data yet. If <= -4 we couldn't find a path.
         */
        int updatePath(double currentTimestamp, const WFMath::Point<3>& currentAvatarPosition);

        /**
         * @brief Updates the path.
         * @return If >= 0, a path was found. If <0 and >= -3 we couldn't find a path currently because we don't have all data yet. If <= -4 we couldn't find a path.
         */
        int updatePath(double currentTimestamp);

        /**
         * @brief Requests an update of the path.
         *
         * The actual update will be deferred to when updatePath() is called, which normally happens
         * with a call to update()
         */
        void requestUpdate();

        /**
         * @brief Starts the steering.
         */
        void startSteering();

        /**
         * @brief Stops the steering.
         */
        void stopSteering();

        /**
         * @brief Returns true if steering currently is enabled.
         * @return True if steering is enabled.
         */
        bool isEnabled() const;

        /**
         * @brief Sets the desired speed.
         * @param desiredSpeed The desired speed, as a normalized value.
         */
        void setDesiredSpeed(float desiredSpeed);

        /**
         * @brief Gets the current path.
         * @return The current path.
         */
        const std::vector<WFMath::Point<3>>& getPath() const;

        size_t getCurrentPathIndex() const;

        /**
        * @brief Returns true if we've just sent a movement update to the server and thus expect an update in return.
        *
        * This is useful to know whether any movement update received from the server was instigated by us or not.
        * @return
        */
        bool getIsExpectingServerMovement() const;

        /**
         * @brief Sets if we're expecting an update from the server.
         * @param expected
         */
        void setIsExpectingServerMovement(bool expected);

        /**
         * @brief Updates the steering.
         *
         * Call this often when steering is enabled.
         */
        SteeringResult update(double currentTimestamp);

        WFMath::Point<3> getCurrentAvatarPosition(double currentTimestamp) const;

        size_t unawareAreaCount() const;

        int getPathResult() const;

        bool isAtDestination(double currentTimestamp, const SteeringDestination& destination) const;

        bool isAtCurrentDestination(double currentTimestamp) const;

        boost::optional<double> distanceTo(double currentTimestamp, const EntityLocation& location, MeasureType fromSelf, MeasureType toDestination) const;

        WFMath::Vector<3> directionTo(double currentTimestamp, const EntityLocation& location) const;

        Steering::ResolvedPosition resolvePosition(double currentTimestamp, const EntityLocation& location) const;

        /**
         * @brief Emitted when the path has been updated.
         */
        sigc::signal<void()> EventPathUpdated;

    private:

        Awareness* mAwareness;
        MemEntity& mAvatar;

        sigc::connection mTileListenerConnection;

        SteeringDestination mSteeringDestination;

        /**
         * @brief The calculated path to the destination.
         * The waypoints is in order.
         */
        std::vector<WFMath::Point<3>> mPath;

        /**
         * The current active position in the mPath.
         */
        size_t mCurrentPathIndex;

        /**
         * @brief True if steering currently is enabled.
         */
        bool mSteeringEnabled;

        /**
         * @brief True if the path should be recalculated the next time update() is called.
         */
        bool mUpdateNeeded;

        /**
         * @brief In world units how much padding to expand the awareness area with.
         */
        float mPadding;

        /**
         * @brief The max speed of the character, when moving with a normalized velocity vector.
         */
        double mMaxSpeed;

        /**
         * @brief A normalized value of how much of the max speed we want to keep.
         */
        float mDesiredSpeed;

        /**
         * @brief True if we're expecting a movement response from the server.
         *
         * If we've told the server to that we should move, we'll be expecting a response and need to handle that slightly different
         * than if we got a movement update out of the blue.
         */
        bool mExpectingServerMovement;

        int mPathResult;

        /**
         * Horizontal radius of the avatar, i.e. radius using only x and y.
         * Used to determine how close the avatar should be to things.
         */
        WFMath::CoordType mAvatarHorizRadius;

        /**
         * @brief Keep track of the last sent velocity.
         *
         * This is mainly used to keep track of if we need to send a stop velocity, once we've reached our destination.
         */
        WFMath::Vector<2> mLastSentVelocity;

        /**
         * @brief The position of the avatar when the awareness area was last updated.
         *
         * We keep track of this in order to know when to update the area again.
         */
        WFMath::Point<3> mAvatarPositionLastUpdate;

        /**
         * @brief Sets the awareness to be a corridor between where the avatar currently is and our destination.
         *
         * mPadding determines the width of the corridor.
         */
        void setAwarenessArea(double currentServerTimestamp);

        /**
         * @brief Listen to tiles being updated, and request updates.
         * @param tx
         * @param ty
         */
        void Awareness_TileUpdated(int tx, int ty);

        /**
         * @brief Tells the server to move in a certain direction.
         * @param direction The direction to move in.
         */
        void moveInDirection(const WFMath::Vector<2>& direction);

        /**
         * @brief Tells the server to move towards a certain point.
         *
         * The server will make sure to stop at the specified point, unless we collide with something.
         * This should therefore be used to reach our final destination.
         *
         * @param point The point to move towards.
         */
        void moveToPoint(const WFMath::Point<3>& point);

        void updatePosition(double currentServerTimestamp, long entityId, WFMath::Point<3>& pos) const;

};


#endif /* STEERING_H_ */
