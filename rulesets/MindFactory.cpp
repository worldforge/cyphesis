// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

#include <Python.h>

#include "MindFactory.h"

#include "BaseMind.h"
#include "Python_Script_Utils.h"

#include "common/debug.h"

static const bool debug_flag = false;

MindFactory * MindFactory::m_instance = NULL;

MindFactory::MindFactory()
{
}

BaseMind * MindFactory::newMind(const std::string & id, long intId,
                                const std::string & name,
                                const std::string & type)
{
    BaseMind * mind = new BaseMind(id, intId, name);
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
