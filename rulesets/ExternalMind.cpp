#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Operation/Login.h>

#include "ExternalMind.h"

#include <server/Connection.h>

ExternalMind::ExternalMind(Connection * connection, string & id) :
		BaseMind(id), connection(connection)
{
}

oplist ExternalMind::message(const RootOperation & msg)
{
    connection->send(&msg);
    oplist res;
    return(res);
}
