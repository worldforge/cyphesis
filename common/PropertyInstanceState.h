/*
 Copyright (C) 2015 Erik Ogenvik

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
#ifndef RULESETS_PROPERTYINSTANCESTATE_H_
#define RULESETS_PROPERTYINSTANCESTATE_H_

#include <unordered_map>
#include <cassert>

class LocatedEntity;

/**
 * @brief Handles entity specific property state.
 *
 * In some cases you want to save instance state per entity. Often in those cases you can store
 * it in some member directly on the entity. But sometimes you don't want to bloat the entity with
 * fields for seldom used properties. In those cases this class becomes useful.
 * An instance is meant to be used as a static field of the property. When the property is installed
 * on the entity addState should be called, and when it's removed removeState must be called.
 * Failing to call removeState properly will lead to an assert error on shutdown.
 */
template <typename T>
class PropertyInstanceState {
    private:
        std::unordered_map<const LocatedEntity*, T*> mStates;

    public:

        ~PropertyInstanceState() {
            //When an instance of this is destroyed, all entities should already have deregistered themselves from it.
            assert(mStates.empty());
        }

        /**
         * Adds a new state for the specified entity.
         * @param entity An entity.
         * @param state A new state instance.
         */
        void addState(const LocatedEntity* entity, T* state) {
            mStates.insert(std::make_pair(entity, state));
        }

        /**
         * Gets the state for the supplied entity.
         * @param entity An entity.
         * @return The registered state, or null.
         */
        T* getState(const LocatedEntity* entity) const {
            auto I = mStates.find(entity);
            if (I != mStates.end()) {
                return I->second;
            }
            return nullptr;
        }

        /**
         * Removes the state for the supplied entity.
         *
         * Calling this will delete the state instance.
         * @param entity An entity.
         */
        void removeState(const LocatedEntity* entity) {
            auto I = mStates.find(entity);
            if (I != mStates.end()) {
                delete I->second;
                mStates.erase(I);
            }
        }

        /**
         * Replaces the existing state registered for the entity with a new state.
         *
         * The existing state instance will be deleted.
         * @param entity An entity.
         * @param state A new state.
         */
        void replaceState(const LocatedEntity* entity, T* state) {
            auto I = mStates.find(entity);
            if (I != mStates.end()) {
                delete I->second;
                I->second = state;
            }
        }
};


#endif /* RULESETS_PROPERTYINSTANCESTATE_H_ */
