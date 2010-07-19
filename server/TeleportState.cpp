#include "server/TeleportState.h"

TeleportState::TeleportState(const std::string &id, bool valid) :
                                m_valid(valid),
                                m_entityid(id),
                                m_state(TELEPORT_NONE)
{
}

bool TeleportState::isValid()
{
    return m_valid;
}

const std::string & TeleportState::getId()
{
    return m_entityid;
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
