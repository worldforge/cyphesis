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
class AwareMind : public BaseMind, public MemMap::MapListener
{
    public:
        AwareMind(const std::string& mind_id,
                  std::string entity_id,
                  SharedTerrain& sharedTerrain,
                  AwarenessStoreProvider& awarenessStoreProvider);

        ~AwareMind() override;

        void entityAdded(const MemEntity& entity) override;

        void entityUpdated(const MemEntity& entity, const Atlas::Objects::Entity::RootEntity& ent, LocatedEntity* oldLocation) override;

        void entityDeleted(const MemEntity& entity) override;

        void operation(const Operation& op, OpVector& res) override;

        int updatePath();

        Steering* getSteering();

        const std::shared_ptr<Awareness>& getAwareness() const;

        double getServerTimeDiff() const;

        double getCurrentLocalTime() const;

        double getCurrentServerTime() const;

    protected:

        SharedTerrain& mSharedTerrain;
        AwarenessStoreProvider& mAwarenessStoreProvider;

        AwarenessStore* mAwarenessStore;
        std::shared_ptr<Awareness> mAwareness;
        std::unique_ptr<Steering> mSteering;

        /**
         * @brief Difference in time between server time and local time.
         *
         * This is used to correctly calculate position of entities when handling steering.
         *
         * The value is calculated by subtracting the timestamp received from the server from the current local time.
         */
        double mServerTimeDiff;

        void setOwnEntity(OpVector& res, Ref<MemEntity> ownEntity) override;

        void processMoveTick(const Operation& op, OpVector& res);

        void requestAwareness(const MemEntity& entity);

        void parseTerrain(const Atlas::Message::Element& terrainElement);

};

#endif /* RULESETS_MIND_AWAREMIND_H_ */
