#ifndef SERVER_PENDING_TELEPORT_H
#define SERVER_PENDING_TELEPORT_H

#include <string>

class PendingTeleport
{
    std::string m_entity_id;
    std::string m_possess_key;

    public:

    PendingTeleport(const std::string &, const std::string &);
    const std::string & getPossessKey();
    const std::string & getEntityID();
};

#endif
