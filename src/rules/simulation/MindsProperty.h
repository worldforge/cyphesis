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
#include <Atlas/Objects/Operation.h>

class Router;

/**
 * Keeps tab of the Mind instances that currently are controlling a specific entity.
 *
 * Operations sent to the entity will be relayed to the Minds (especially Sights), and
 * operations coming from the minds will be filtered and wrapped into Thought ops and
 * sent to the simulation.
 *
 * This is the main entry point for allowing an external client to control an entity.
 *
 * \ingroup PropertyClasses
 */
class MindsProperty : public PropertyBase
{
    public:
        static constexpr const char* property_name = "_minds";
        static constexpr const char* property_atlastype = "list";


        MindsProperty();

        ~MindsProperty() override;

        MindsProperty(const MindsProperty&) = delete;

        int get(Atlas::Message::Element& val) const override;

        void set(const Atlas::Message::Element& val) override;

        void add(const std::string& val,
                 Atlas::Message::MapType& map) const override;

        void add(const std::string& val,
                 const Atlas::Objects::Entity::RootEntity& ent) const override;

        MindsProperty* copy() const override;

        void install(LocatedEntity&, const std::string& name) override;

        void remove(LocatedEntity&, const std::string& name) override;

        void addMind(Router* mind);

        void removeMind(Router* mind, LocatedEntity& entity);

        const std::vector<Router*>& getMinds() const;

        HandlerResult operation(LocatedEntity&,
                                const Operation&,
                                OpVector&) override;

        void sendToMinds(const Operation& op, OpVector& res) const;


    protected:

        std::vector<Router*> m_data;


        bool world2mind(const Operation& op) const;


        HandlerResult ThoughtOperation(LocatedEntity& ent, const Operation& op, OpVector& res) const;

        HandlerResult RelayOperation(LocatedEntity& ent, const Operation& op, OpVector& res);


        void mindCreateOperation(LocatedEntity&, const Operation&, OpVector&) const;

        void mindDeleteOperation(LocatedEntity&, const Operation&, OpVector&) const;

        void mindGoalInfoOperation(LocatedEntity&, const Operation&, OpVector&) const;

        void mindImaginaryOperation(LocatedEntity&, const Operation&, OpVector&) const;

        void mindLookOperation(LocatedEntity&, const Operation&, OpVector&) const;

        void mindMoveOperation(LocatedEntity&, const Operation&, OpVector&) const;

        void mindSetOperation(LocatedEntity&, const Operation&, OpVector&) const;

        void mindTalkOperation(LocatedEntity&, const Operation&, OpVector&) const;

        void mindThoughtOperation(LocatedEntity&, const Operation&, OpVector&) const;

        void mindThinkOperation(LocatedEntity&, const Operation&, OpVector&) const;

        void mindTickOperation(LocatedEntity&, const Operation&, OpVector&) const;

        void mindTouchOperation(LocatedEntity&, const Operation&, OpVector&) const;

        void mindUpdateOperation(LocatedEntity&, const Operation&, OpVector&) const;

        void mindUseOperation(LocatedEntity&, const Operation&, OpVector&) const;

        void mindWieldOperation(LocatedEntity&, const Operation&, OpVector&) const;

        void mindOtherOperation(LocatedEntity&, const Operation&, OpVector&) const;

        void mind2body(LocatedEntity&, const Operation& op, OpVector&) const;

        void moveOtherEntity(LocatedEntity& ent, const Operation& op, OpVector& res, const Atlas::Objects::Entity::RootEntity& arg, const std::string& other_id) const;

};


#endif //CYPHESIS_MINDSPROPERTY_H
