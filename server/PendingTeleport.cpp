#include "PendingTeleport.h"

PendingTeleport::PendingTeleport(const std::string &id, const std::string &key) 
                                            :   m_entity_id(id),
                                                m_possess_key(key)
{
}

const std::string & PendingTeleport::getPossessKey()
{
    return m_possess_key;
}

const std::string & PendingTeleport::getEntityID()
{
    return m_entity_id;
}
