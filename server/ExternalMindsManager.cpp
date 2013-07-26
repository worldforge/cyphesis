/*
 Copyright (C) 2013 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "ExternalMindsManager.h"

#include "common/Router.h"
#include "common/Possess.h"

#include <Atlas/Objects/Entity.h>

ExternalMindsManager * ExternalMindsManager::m_instance = nullptr;

ExternalMindsManager::ExternalMindsManager()
{

}

ExternalMindsManager::~ExternalMindsManager()
{
}

ExternalMindsManager * ExternalMindsManager::instance()
{
    if (m_instance == NULL) {
        m_instance = new ExternalMindsManager();
    }
    return m_instance;
}
void ExternalMindsManager::addConnection(
        const ExternalMindsConnection& connection)
{
    m_connections.push_back(connection);
}

int ExternalMindsManager::requestPossession(LocatedEntity& entity,
        const std::string& possession_key)
{
    if (!m_connections.empty()) {
        ExternalMindsConnection& connection = *m_connections.begin();

        Atlas::Objects::Operation::Possess possessOp;

        Atlas::Objects::Entity::Anonymous possess_args;
        possess_args->setAttr("possess_key", possession_key);

        possessOp->setArgs1(possess_args);
        possessOp->setTo(connection.getRouter()->getId());

        OpVector res;
        connection.getRouter()->operation(possessOp, res);
        return 0;
    }
    return -1;
}

