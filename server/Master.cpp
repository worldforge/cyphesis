// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "Master.h"

#include "common/id.h"

Master::Master(CommClient & cli, ServerRouting & svr, const std::string & id) :
        OOGThing(id, forceIntegerId(id)), m_commClient(cli), m_server(svr)
{
}

Master::~Master()
{
}
