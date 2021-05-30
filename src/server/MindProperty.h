// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2008 Alistair Riddoch
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


#ifndef RULESETS_MIND_PROPERTY_H
#define RULESETS_MIND_PROPERTY_H

#include "common/Property.h"

class MindKit;

/// \brief Class to handle assigning a mind object to an entity
/// \ingroup PropertyClasses
class MindProperty : public PropertyBase
{
    protected:

        /**
         * The preferred language for this mind. For example "python".
         */
        std::string m_language;

        /**
         * The preferred script to be run for this mind.
         */
        std::string m_script;

        MindProperty(const MindProperty&);

    public:
        static constexpr const char* property_name = "mind";
        static constexpr const char* property_atlastype = "map";

        MindProperty() = default;

        ~MindProperty() override = default;

        /// \brief Returns true if mind scripting is enabled.
        bool isMindEnabled() const;

        void set(const Atlas::Message::Element& val) override;

        int get(Atlas::Message::Element& val) const override;

        MindProperty* copy() const override;

        void apply(LocatedEntity& entity) override;

        void remove(LocatedEntity& entity, const std::string& name) override;
};

#endif // RULESETS_MIND_PROPERTY_H
