// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "Master.h"

Master::Master(const std::string & id, CommClient & cli, ServerRouting & svr) :
               OOGThing(id), m_commClient(cli), m_server(svr)
{
}

Master::~Master()
{
}
