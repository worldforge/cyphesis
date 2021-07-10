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

#include <sigc++/trackable.h>

#include <map>
#include <unordered_set>
#include <string>
#include <common/Singleton.h>

class LocatedEntity;
class MindsProperty;
class PossessionAuthenticator;

class ExternalMindsManager : public virtual sigc::trackable, public Singleton<ExternalMindsManager>
{
    public:
        explicit ExternalMindsManager(PossessionAuthenticator& possessionAuthenticator);
        ~ExternalMindsManager() override = default;

        int addConnection(const ExternalMindsConnection& connection);
        int removeConnection(const std::string& routerId);

        /**
         * Requests a possession for the supplied character.
         *
         * This means that we want an external AI process to take possession of the character and control its mind.
         * The manager is responsible for finding a suitable external mind connection to ask for possession.
         *
         * The preferred language and script to use can be specified, but these might not be honoured, depending on
         * the external minds client registered.
         *
         * @param character The character which should be possessed.
         * @param language The preferred language to use for the mind.
         * @param script The preferred script to use for the mind.
         * @return
         */
        int requestPossession(LocatedEntity& character);

        void removeRequest(LocatedEntity& character);

    private:
        PossessionAuthenticator& m_possessionAuthenticator;
        std::map<std::string, ExternalMindsConnection> m_connections;
        std::unordered_set<LocatedEntity*> m_unpossessedEntities;
        std::unordered_set<LocatedEntity*> m_possessedEntities;

        void entity_destroyed(LocatedEntity& character);
        void entity_mindsChanged(LocatedEntity& character, const MindsProperty& mindsProp);

        int requestPossessionFromRegisteredClients(const std::string& character_id);

        void addPossessionEntryForCharacter(LocatedEntity& character);


};

#endif /* EXTERNALMINDSMANAGER_H_ */
