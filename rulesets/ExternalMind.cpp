#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Operation/Login.h>

#include "ExternalMind.h"

ExternalMind::ExternalMind(Connection * connection) : connection(connection)
{
}

RootOperation * ExternalMind::message(const RootOperation & msg)
{
    connection->send(&msg);
    return(NULL);
}
