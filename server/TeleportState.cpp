#include "server/TeleportState.h"

TeleportState::TeleportState() : m_state(TELEPORT_NONE)
{
}

void TeleportState::setRequested()
{
    m_state = TELEPORT_REQUESTED;
}

void TeleportState::setCreated()
{
    m_state = TELEPORT_CREATED;
}

bool TeleportState::isCreated()
{
    return (m_state == TELEPORT_CREATED);
}

bool TeleportState::isRequested()
{
    return (m_state == TELEPORT_REQUESTED);
}
