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
#ifndef RULESETS_MIND_AWAREMIND_H_
#define RULESETS_MIND_AWAREMIND_H_

#include "BaseMind.h"
#include "MemMap.h"

class Awareness;

class AwarenessStore;

class AwarenessStoreProvider;

class Steering;

class SharedTerrain;

/**
 * @brief A Mind which is aware of its surroundings and thus can steer and navigate in the world.
 */
class AwareMind : public BaseMind
{
    public:
        AwareMind(RouterId mind_id,
                  std::string entity_id,
                  TypeStore& typeStore,
                  SharedTerrain& sharedTerrain,
                  AwarenessStoreProvider& awarenessStoreProvider);

        ~AwareMind() override;

        void entityAdded(MemEntity& entity) override;

        void entityUpdated(MemEntity& entity, const Atlas::Objects::Entity::RootEntity& ent, LocatedEntity* oldLocation) override;

        void entityDeleted(MemEntity& entity) override;

        int updatePath();

        Steering* getSteering();

        const std::shared_ptr<Awareness>& getAwareness() const;

        double getCurrentServerTime() const;

    protected:

        SharedTerrain& mSharedTerrain;
        AwarenessStoreProvider& mAwarenessStoreProvider;

        AwarenessStore* mAwarenessStore;
        std::shared_ptr<Awareness> mAwareness;
        std::unique_ptr<Steering> mSteering;

        /**
         * Keep track of the last queued move tick, so that we only ever act on the latest.
         * This helps with when new steering goals are added, and old scheduled ticks should be ignored.
         */
        Atlas::Message::IntType mMoveTickSerialNumber;

        void setOwnEntity(OpVector& res, Ref<MemEntity> ownEntity) override;

        void processMove(OpVector& res) override;

        void processNavmesh() override;

        void requestAwareness(const MemEntity& entity);

        void parseTerrain(const Atlas::Message::Element& terrainElement);
};

#endif /* RULESETS_MIND_AWAREMIND_H_ */
