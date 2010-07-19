#ifndef SERVER_TELEPORTSTATE_H
#define SERVER_TELEPORTSTATE_H

#include <string>

class TeleportState
{
    bool m_valid;
    std::string m_entityid;
    enum {  TELEPORT_NONE,
            TELEPORT_REQUESTED,
            TELEPORT_CREATED } m_state;

    public:
    TeleportState(const std::string &, bool);
    
    const std::string & getId();
    bool isValid();

    void setRequested();
    void setCreated();

    bool isCreated();
    bool isRequested();
};

#endif
