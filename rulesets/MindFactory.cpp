// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "MindFactory.h"

#include "BaseMind.h"
#include "Python_API.h"

#include "common/debug.h"

static const bool debug_flag = false;

MindFactory * MindFactory::m_instance = NULL;

MindFactory::MindFactory()
{
}

BaseMind * MindFactory::newMind(const std::string & id,
                                const std::string & name,
                                const std::string & type)
{
    BaseMind * mind = new BaseMind(id, name);
    std::string mind_class("NPCMind"), mind_package("mind.NPCMind");
    MindFactory::mindmap_t::const_iterator I = m_mindTypes.find(type);
    if (I != m_mindTypes.end()) {
        mind_package = I->second;
        mind_class = type + "Mind";
        debug(std::cout << "Got custom mind of type " << mind_package << " for "
                        << type << std::endl << std::flush;);
    }
    Create_PyMind(mind, mind_package, mind_class);
    return mind;
}
