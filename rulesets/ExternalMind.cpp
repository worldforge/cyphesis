#include <Atlas/Message/Object.h>
#include <Atlas/Net/Stream.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Objects/Decoder.h>
#include <Atlas/Objects/Operation/Login.h>

#include "ExternalMind.h"

#include <server/Connection.h>

ExternalMind::ExternalMind(Connection * connection, string & id, string & name):
		BaseMind(id, name), connection(connection)
{
}

oplist ExternalMind::message(const RootOperation & msg)
{
    connection->send(&msg);
    oplist res;
    return(res);
}
