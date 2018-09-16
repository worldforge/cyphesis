/*
 Copyright (C) 2018 Erik Ogenvik

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

#ifndef CYPHESIS_TYPERESOLVER_H
#define CYPHESIS_TYPERESOLVER_H

#include "common/OperationRouter.h"
#include <map>
#include <string>
#include <set>
#include <Atlas/Objects/RootEntity.h>
#include <boost/optional.hpp>

class TypeNode;

class TypeResolver
{
    public:

        boost::optional<std::string> m_typeProviderId;

        std::set<TypeNode*> InfoOperation(const Operation& op, OpVector& res);

        void requestType(const std::string& id, OpVector& res);

    private:

        struct PendingType {
            Atlas::Objects::Root ent;
            std::set<std::string> childTypes;
        };

        std::map<std::string, PendingType> m_pendingTypes;

        std::set<TypeNode*> processTypeData(const Atlas::Objects::Root& data, OpVector& res);


        std::set<TypeNode*> resolveType(const std::string& id, const Atlas::Objects::Root& ent, OpVector& res);
};


#endif //CYPHESIS_TYPERESOLVER_H
