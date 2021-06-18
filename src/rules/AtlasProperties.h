// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2007 Alistair Riddoch
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


#ifndef RULESETS_ATLAS_PROPERTIES_H
#define RULESETS_ATLAS_PROPERTIES_H

#include "common/Property.h"

#include "modules/Ref.h"

#include <set>

/// \brief Class to handle Entity id property
/// \ingroup PropertyClasses
class IdProperty : public PropertyBase
{
    protected:
        const std::string& m_data;
    public:
        static constexpr const char* property_name = "id";
        static constexpr const char* property_atlastype = "string";

        explicit IdProperty(const std::string& data);

        int get(Atlas::Message::Element& val) const override;

        void set(const Atlas::Message::Element& val) override;

        void add(const std::string& key, Atlas::Message::MapType& map) const override;

        void add(const std::string& key, const Atlas::Objects::Entity::RootEntity& ent) const override;

        IdProperty* copy() const override;
};

class LocationProperty : public PropertyBase
{
    protected:
        const LocatedEntity& m_data;
    public:
        static constexpr const char* property_name = "loc";
        static constexpr const char* property_atlastype = "string";

        explicit LocationProperty(const LocatedEntity& entity);

        int get(Atlas::Message::Element& val) const override;

        void set(const Atlas::Message::Element& val) override;

        void add(const std::string& key, Atlas::Message::MapType& map) const override;

        void add(const std::string& key, const Atlas::Objects::Entity::RootEntity& ent) const override;

        LocationProperty* copy() const override;

        const std::string& data() const;
};

/// \brief Class to handle Entity name property
/// \ingroup PropertyClasses
class NameProperty : public Property<std::string>
{
    public:
        static constexpr const char* property_name = "name";
        static constexpr const char* property_atlastype = "string";

        explicit NameProperty(unsigned int flags);

        void add(const std::string& key, const Atlas::Objects::Entity::RootEntity& ent) const override;
};

class LocatedEntity;

typedef std::set<Ref<LocatedEntity>> LocatedEntitySet;

/// \brief Class to handle Entity contains property
/// \ingroup PropertyClasses
class ContainsProperty : public PropertyBase
{
    protected:
        LocatedEntitySet& m_data;
    public:
        static constexpr const char* property_name = "contains";
        static constexpr const char* property_atlastype = "list";

        explicit ContainsProperty(LocatedEntitySet& data);

        int get(Atlas::Message::Element& val) const override;

        void set(const Atlas::Message::Element& val) override;

        void add(const std::string& key, const Atlas::Objects::Entity::RootEntity& ent) const override;

        ContainsProperty* copy() const override;
};

#endif // RULESETS_ATLAS_PROPERTIES_H
