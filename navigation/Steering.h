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


class Awareness;
class MemEntity;
class Loitering;

/**
 * @brief Results of a steering update.
 *
 * A new update should only be sent if direction is a valid vector.
 */
struct SteeringResult {

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


class EntityLocation;

/**
 * @brief Handles steering of an avatar, with path finding and obstacle avoidance.
 *
 * To enable steering, first set the destination through setDestination() and then call startSteering().
 */
class Steering: public virtual sigc::trackable
{
public:
	explicit Steering(MemEntity& avatar);
	virtual ~Steering() = default;

	void setAwareness(Awareness* awareness);

	int queryDestination(const EntityLocation& destination, double currentServerTimestamp);

	/**
	 * @brief Sets a new destination, in view position.
	 * Note that this won't start steering; you need to call startSteering() separately.
	 * @param entityRelativePosition The position, as relative to the referred entity.
	 * @param radius The radius around the destination where it's acceptable to end up if we couldn't reach the destination precisely.
	 */
	void setDestination(long entityId, const WFMath::Point<3>& entityRelativePosition, float radius, double currentServerTimestamp);

	/**
	 * @brief Updates the path.
	 * @param currentAvatarPosition The current position of the avatar entity.
	 * @return If >= 0, a path was found. If <0 and >= -3 we couldn't find a path currently because we don't have all data yet. If <= -4 we couldn't find a path.
	 */
	int updatePath(const WFMath::Point<3>& currentAvatarPosition);

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

	WFMath::Point<3> getCurrentAvatarPosition(double currentTimestamp);

	size_t unawareAreaCount() const;

	int getPathResult() const;

	/**
	 * @brief Emitted when the path has been updated.
	 */
	sigc::signal<void> EventPathUpdated;

private:

	Awareness* mAwareness;
	MemEntity& mAvatar;

	sigc::connection mTileListenerConnection;


	/**
	 * The id of the entity to which the destination is relative.
	 * This is either the domain entity, for which the final destination then would be static.
	 * Or it's the id of an entity contained in the domain. The final destination is then dynamic, and updated as the entity moves.
	 */
    long mDestinationEntityId;

	/**
	 * The destination as relative to the mDestinationEntityId.
	 */
    WFMath::Point<3> mEntityRelativeDestination;


	/**
	 * @brief The destination, in view coordinates.
	 */
	WFMath::Point<3> mViewDestination;

	float mDestinationRadius;

	/**
	 * @brief The calculated path to the destination.
	 * The waypoints is in order.
	 */
	std::vector<WFMath::Point<3>> mPath;

	/**
	 * The current active position in the mPath.
	 */
	size_t mPathPosition;

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
	float mAvatarHorizRadius;

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
	void setAwarenessArea();

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
