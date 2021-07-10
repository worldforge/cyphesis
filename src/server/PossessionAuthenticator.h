// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2010 Alistair Riddoch
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

#ifndef SERVER_POSSESSION_AUTHENTICATOR_H
#define SERVER_POSSESSION_AUTHENTICATOR_H

#include "modules/Ref.h"
#include <string>
#include <unordered_map>
#include <boost/optional.hpp>
#include <common/Singleton.h>
#include "PendingPossession.h"

class LocatedEntity;

/// \brief Map of teleported entity IDs and their PendingState objects
typedef std::unordered_map<std::string, std::unique_ptr<PendingPossession>> PendingPossessionsMap;


/// \brief A class that stores and authenticates possession requests
class PossessionAuthenticator : public Singleton<PossessionAuthenticator>
{
    private:

        /// \brief Map of teleport requests
        PendingPossessionsMap m_possessions;

        void removePossession(PendingPossessionsMap::iterator I);

    public:


        bool isPending(const std::string&) const;

        int addPossession(const std::string&, const std::string&);

        int removePossession(const std::string&);

        boost::optional<std::string> getPossessionKey(const std::string& entity_id);

        Ref<LocatedEntity> authenticatePossession(const std::string&,
                                                  const std::string&);

        friend class TeleportAuthenticatortest;
};

#endif // SERVER_POSSESSION_AUTHENTICATOR_H
