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


#ifndef COMMON_TYPE_NODE_H
#define COMMON_TYPE_NODE_H

#include "Visibility.h"
#include "PropertyManager.h"

#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/SmartPtr.h>

#include <iostream>
#include <map>
#include <set>
#include <string>
#include <memory>


/// \brief Entry in the type hierarchy for in-game entity classes.
class TypeNode
{

    protected:
        /// \brief name
        const std::string m_name;

        /// \brief property defaults
        std::map<std::string, std::unique_ptr<PropertyBase>> m_defaults;

        /// \brief type description, complete
        Atlas::Objects::Root m_privateDescription;
        /**
         * \brief type description, with private fields (i.e. starting with "__") removed.
         * Also "children" and "attributes" have been removed.
         */
        Atlas::Objects::Root m_protectedDescription;
        /**
         * \brief type description, with private fields (i.e. starting with "__") and
         * protected fields (i.e. starting with "_") removed.
         * Also "children" and "attributes" have been removed.
         */
        Atlas::Objects::Root m_publicDescription;

        /// \brief parent node
        const TypeNode* m_parent;
    public:

        struct PropertiesUpdate
        {
            std::set<std::string> newProps;
            std::set<std::string> removedProps;
            std::set<std::string> changedProps;
        };


        explicit TypeNode(std::string name);

        TypeNode(std::string name, const Atlas::Objects::Root& description);

        ~TypeNode();

        /// \brief injects a new property and updated the m_description
        TypeNode::PropertiesUpdate injectProperty(const std::string&, std::unique_ptr<PropertyBase>);

        /// \brief add the class properties for this type from Atlas attributes
        void addProperties(const Atlas::Message::MapType& attributes, const PropertyManager& propertyManager);

        /// \brief update the class properties for this type from Atlas attributes
        TypeNode::PropertiesUpdate updateProperties(const Atlas::Message::MapType& attributes, const PropertyManager& propertyManager);

        /// \brief check if this type inherits from another
        bool isTypeOf(const std::string& base_type) const;

        /// \brief check if this type inherits from another
        bool isTypeOf(const TypeNode* base_type) const;

        /// \brief const accessor for name
        const std::string& name() const
        {
            return m_name;
        }

        /// \brief const accessor for property defaults
        const std::map<std::string, std::unique_ptr<PropertyBase>>& defaults() const
        {
            return m_defaults;
        }

        void setDescription(const Atlas::Objects::Root& description);

        /// \brief accessor for type description
        Atlas::Objects::Root& description(Visibility visibility);

        /// \brief const accessor for type description
        const Atlas::Objects::Root& description(Visibility visibility) const;

        /// \brief const accessor for parent node
        const TypeNode* parent() const
        {
            return m_parent;
        }

        /// \brief set the parent node
        void setParent(const TypeNode* parent)
        {
            m_parent = parent;
        }
};

#endif // COMMON_TYPE_NODE_H
