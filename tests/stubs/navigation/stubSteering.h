// AUTOGENERATED file, created by the tool generate_stub.py, don't edit!
// If you want to add your own functionality, instead edit the stubSteering_custom.h file.

#ifndef STUB_NAVIGATION_STEERING_H
#define STUB_NAVIGATION_STEERING_H

#include "navigation/Steering.h"
#include "stubSteering_custom.h"


#ifndef STUB_Steering_Steering
//#define STUB_Steering_Steering
   Steering::Steering(MemEntity& avatar)
    : sigc::trackable(avatar)
    , mAwareness(nullptr)
  {
    
  }
#endif //STUB_Steering_Steering

#ifndef STUB_Steering_setAwareness
//#define STUB_Steering_setAwareness
  void Steering::setAwareness(Awareness* awareness)
  {
    
  }
#endif //STUB_Steering_setAwareness

#ifndef STUB_Steering_queryDestination
//#define STUB_Steering_queryDestination
  int Steering::queryDestination(const EntityLocation& destination, double currentServerTimestamp)
  {
    return 0;
  }
#endif //STUB_Steering_queryDestination

#ifndef STUB_Steering_setDestination
//#define STUB_Steering_setDestination
  void Steering::setDestination(long entityId, const WFMath::Point<3>& entityRelativePosition, float radius, double currentServerTimestamp)
  {
    
  }
#endif //STUB_Steering_setDestination

#ifndef STUB_Steering_updatePath
//#define STUB_Steering_updatePath
  int Steering::updatePath(const WFMath::Point<3>& currentAvatarPosition)
  {
    return 0;
  }
#endif //STUB_Steering_updatePath

#ifndef STUB_Steering_updatePath
//#define STUB_Steering_updatePath
  int Steering::updatePath(double currentTimestamp)
  {
    return 0;
  }
#endif //STUB_Steering_updatePath

#ifndef STUB_Steering_requestUpdate
//#define STUB_Steering_requestUpdate
  void Steering::requestUpdate()
  {
    
  }
#endif //STUB_Steering_requestUpdate

#ifndef STUB_Steering_startSteering
//#define STUB_Steering_startSteering
  void Steering::startSteering()
  {
    
  }
#endif //STUB_Steering_startSteering

#ifndef STUB_Steering_stopSteering
//#define STUB_Steering_stopSteering
  void Steering::stopSteering()
  {
    
  }
#endif //STUB_Steering_stopSteering

#ifndef STUB_Steering_isEnabled
//#define STUB_Steering_isEnabled
  bool Steering::isEnabled() const
  {
    return false;
  }
#endif //STUB_Steering_isEnabled

#ifndef STUB_Steering_setDesiredSpeed
//#define STUB_Steering_setDesiredSpeed
  void Steering::setDesiredSpeed(float desiredSpeed)
  {
    
  }
#endif //STUB_Steering_setDesiredSpeed

#ifndef STUB_Steering_getPath
//#define STUB_Steering_getPath
  const std::vector<WFMath::Point<3>>& Steering::getPath() const
  {
    static std::vector<WFMath::Point<3>> instance; return instance;
  }
#endif //STUB_Steering_getPath

#ifndef STUB_Steering_getIsExpectingServerMovement
//#define STUB_Steering_getIsExpectingServerMovement
  bool Steering::getIsExpectingServerMovement() const
  {
    return false;
  }
#endif //STUB_Steering_getIsExpectingServerMovement

#ifndef STUB_Steering_setIsExpectingServerMovement
//#define STUB_Steering_setIsExpectingServerMovement
  void Steering::setIsExpectingServerMovement(bool expected)
  {
    
  }
#endif //STUB_Steering_setIsExpectingServerMovement

#ifndef STUB_Steering_update
//#define STUB_Steering_update
  SteeringResult Steering::update(double currentTimestamp)
  {
    return *static_cast<SteeringResult*>(nullptr);
  }
#endif //STUB_Steering_update

#ifndef STUB_Steering_getCurrentAvatarPosition
//#define STUB_Steering_getCurrentAvatarPosition
  WFMath::Point<3> Steering::getCurrentAvatarPosition(double currentTimestamp)
  {
    return *static_cast<WFMath::Point<3>*>(nullptr);
  }
#endif //STUB_Steering_getCurrentAvatarPosition

#ifndef STUB_Steering_unawareAreaCount
//#define STUB_Steering_unawareAreaCount
  size_t Steering::unawareAreaCount() const
  {
    return 0;
  }
#endif //STUB_Steering_unawareAreaCount

#ifndef STUB_Steering_getPathResult
//#define STUB_Steering_getPathResult
  int Steering::getPathResult() const
  {
    return 0;
  }
#endif //STUB_Steering_getPathResult

#ifndef STUB_Steering_setAwarenessArea
//#define STUB_Steering_setAwarenessArea
  void Steering::setAwarenessArea()
  {
    
  }
#endif //STUB_Steering_setAwarenessArea

#ifndef STUB_Steering_Awareness_TileUpdated
//#define STUB_Steering_Awareness_TileUpdated
  void Steering::Awareness_TileUpdated(int tx, int ty)
  {
    
  }
#endif //STUB_Steering_Awareness_TileUpdated

#ifndef STUB_Steering_moveInDirection
//#define STUB_Steering_moveInDirection
  void Steering::moveInDirection(const WFMath::Vector<2>& direction)
  {
    
  }
#endif //STUB_Steering_moveInDirection

#ifndef STUB_Steering_moveToPoint
//#define STUB_Steering_moveToPoint
  void Steering::moveToPoint(const WFMath::Point<3>& point)
  {
    
  }
#endif //STUB_Steering_moveToPoint

#ifndef STUB_Steering_updatePosition
//#define STUB_Steering_updatePosition
  void Steering::updatePosition(double currentServerTimestamp, long entityId, WFMath::Point<3>& pos) const
  {
    
  }
#endif //STUB_Steering_updatePosition


#endif