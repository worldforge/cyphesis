#ifndef SERVER_TELEPORTSTATE_H
#define SERVER_TELEPORTSTATE_H

#include <string>

class TeleportState
{
    enum {  TELEPORT_NONE,
            TELEPORT_REQUESTED,
            TELEPORT_CREATED } m_state;

    public:
    TeleportState();
    
    void setRequested();
    void setCreated();

    bool isCreated();
    bool isRequested();
};

#endif
