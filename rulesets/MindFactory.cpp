// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

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

void MindFactory::readRuleset(const string & setname)
{
    global_conf->readFromFile(setname+".vconf");
}

BaseMind * MindFactory::newMind(const string & id, const string & name, const string & type,const Object & ent, Routing * svr)
{
    if (!ent.IsMap()) {
         debug( cout << "Entity is not a map" << endl << flush;);
    }
    Object::MapType entmap = ent.AsMap();
    BaseMind * mind = new BaseMind(id, name);
    string mind_class("NPCMind"), mind_package("mind.NPCMind");
    if (global_conf->findItem("mind", type)) {
        mind_package = global_conf->getItem("mind", type);
        mind_class = type + "Mind";
    }
    Create_PyMind(mind, mind_package, mind_class);
    return mind;
}
