// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Operation/Login.h>

#include "MindFactory.h"
#include "Python_API.h"

#include <common/const.h>
#include <common/debug.h>

#include "BaseMind.h"

static const bool debug_flag = false;

using Atlas::Message::Object;

MindFactory * MindFactory::m_instance = NULL;

MindFactory::MindFactory()
{
}

void MindFactory::readRuleset(const std::string & setname)
{
    global_conf->readFromFile(setname+".vconf");
}

BaseMind * MindFactory::newMind(const std::string & id,
                                const std::string & name,
                                const std::string & type,
                                const Object & ent, Routing * svr)
{
    BaseMind * mind = new BaseMind(id, name);
    std::string mind_class("NPCMind"), mind_package("mind.NPCMind");
    if (global_conf->findItem("mind", type)) {
        mind_package = global_conf->getItem("mind", type);
        mind_class = type + "Mind";
    }
    Create_PyMind(mind, mind_package, mind_class);
    return mind;
}
