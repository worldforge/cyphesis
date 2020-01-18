/*
 Copyright (C) 2015 erik

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
#ifndef RULESETS_MIND_AWARENESSSTOREPROVIDER_H_
#define RULESETS_MIND_AWARENESSSTOREPROVIDER_H_

#include "AwarenessStore.h"

#include <wfmath/axisbox.h>

#include <unordered_map>
#include <memory>

class TypeNode;

struct IHeightProvider;

class AwarenessStoreProvider
{
    public:
        explicit AwarenessStoreProvider(IHeightProvider& heightProvider);

        virtual ~AwarenessStoreProvider() = default;

        AwarenessStore& getStore(const TypeNode* type, int tileSize = 64);

    protected:
        std::unordered_map<std::string, AwarenessStore> m_awarenessStores;
        IHeightProvider& m_heightProvider;

};

#endif /* RULESETS_MIND_AWARENESSSTOREPROVIDER_H_ */
