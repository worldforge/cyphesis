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

#ifndef EXTERNALMINDSMANAGER_H_
#define EXTERNALMINDSMANAGER_H_

#include "ExternalMindsConnection.h"

#include <vector>
#include <string>

class LocatedEntity;

class ExternalMindsManager
{
    public:
        explicit ExternalMindsManager();
        ~ExternalMindsManager();

        static ExternalMindsManager * instance();

        void addConnection(const ExternalMindsConnection& connection);

        int requestPossession(LocatedEntity& entity, const std::string& possession_key);

    private:
        std::vector<ExternalMindsConnection> m_connections;
        static ExternalMindsManager * m_instance;


};

#endif /* EXTERNALMINDSMANAGER_H_ */
