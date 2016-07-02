#include "navigation/Steering.h"

Steering::Steering(MemEntity& avatar)
: mAvatar(avatar)
{
}

Steering::~Steering()
{
}

void Steering::setAwareness(Awareness* awareness)
{
}

void Steering::setDestination(int entityId, const WFMath::Point<3>& entityRelativePosition, float radius, double currentServerTimestamp)
{
}

void Steering::setAwarenessArea()
{
}

size_t Steering::unawareAreaCount() const {
    return 0;
}

void Steering::setSpeed(float speed)
{
}

int Steering::getPathResult() const
{
    return 0;
}

void Steering::updateDestination(double currentServerTimestamp, int entityId, WFMath::Point<3>& pos) {
}


int Steering::updatePath(const WFMath::Point<3>& currentAvatarPosition)
{
    return 0;
}

int Steering::updatePath(double currentTimestamp)
{
    return 0;
}

void Steering::requestUpdate()
{
}

void Steering::startSteering()
{
}

void Steering::stopSteering()
{
}

bool Steering::isEnabled() const
{
    return false;
}

const std::list<WFMath::Point<3>>& Steering::getPath() const
{
    return mPath;
}

WFMath::Point<3> Steering::getCurrentAvatarPosition(double currentTimestamp)
{
    return WFMath::Point<3>();
}

SteeringResult Steering::update(double currentTimestamp)
{
    return SteeringResult();
}

void Steering::Awareness_TileUpdated(int tx, int ty)
{
}

bool Steering::getIsExpectingServerMovement() const
{
    return false;
}

void Steering::setIsExpectingServerMovement(bool expected)
{
}

