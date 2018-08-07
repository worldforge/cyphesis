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

#ifndef CYPHESIS_MINDSPROPERTY_H
#define CYPHESIS_MINDSPROPERTY_H

#include "common/Property.h"

class Router;

class MindsProperty : public PropertyBase
{
    public:
        static constexpr const char* property_name = "_minds";
        static constexpr const char* property_atlastype = "list";

        MindsProperty();

        ~MindsProperty() override;

        int get(Atlas::Message::Element& val) const override;

        void set(const Atlas::Message::Element& val) override;

        void add(const std::string& val,
                 Atlas::Message::MapType& map) const override;

        void add(const std::string& val,
                 const Atlas::Objects::Entity::RootEntity& ent) const override;

        MindsProperty* copy() const override;

        void install(LocatedEntity*, const std::string& name) override;

        void remove(LocatedEntity*, const std::string& name) override;

        void addMind(Router* mind);
        void removeMind(Router* mind, LocatedEntity* entity);

        const std::vector<Router*>& getMinds() const;

        HandlerResult operation(LocatedEntity*,
                                const Operation&,
                                OpVector&) override;

    protected:

        std::vector<Router*> m_data;

        bool world2mind(const Operation & op);

        bool w2mSightOperation(const Operation &);
        bool w2mSoundOperation(const Operation &);
        bool w2mTouchOperation(const Operation &);
        bool w2mTickOperation(const Operation &);
        bool w2mUnseenOperation(const Operation &);
        bool w2mSetupOperation(const Operation &);
        bool w2mAppearanceOperation(const Operation &);
        bool w2mDisappearanceOperation(const Operation &);
        bool w2mErrorOperation(const Operation &);
        bool w2mThoughtOperation(const Operation & op);
        bool w2mThinkOperation(const Operation & op);
        bool w2mCommuneOperation(const Operation & op);
        bool w2mRelayOperation(const Operation & op);


};


#endif //CYPHESIS_MINDSPROPERTY_H
