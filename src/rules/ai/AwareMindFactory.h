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
#ifndef RULESETS_MIND_AWAREMINDFACTORY_H_

#define RULESETS_MIND_AWAREMINDFACTORY_H_

#include "AwarenessStoreProvider.h"
#include "SharedTerrain.h"

#include "MindFactory.h"
#include <unordered_map>

class AwarenessStore;
class TypeStore;

class AwareMindFactory : public MindKit
{
    public:
        explicit AwareMindFactory(TypeStore& typeStore);

        ~AwareMindFactory() override = default;

        BaseMind* newMind(RouterId id, const std::string& entity_id) const override;

    protected:
        TypeStore& mTypeStore;
        std::unique_ptr<SharedTerrain> mSharedTerrain;
        std::unique_ptr<AwarenessStoreProvider> mAwarenessStoreProvider;

};

#endif /* RULESETS_MIND_AWAREMINDFACTORY_H_ */
