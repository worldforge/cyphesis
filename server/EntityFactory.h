// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
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


#ifndef SERVER_ENTITY_FACTORY_H
#define SERVER_ENTITY_FACTORY_H

#include "common/EntityKit.h"

class EntityFactoryBase : public EntityKit {
    protected:

      void initializeEntity(LocatedEntity& thing,
              const Atlas::Objects::Entity::RootEntity & attributes,
              LocatedEntity* location);
    public:

      ScriptKit<LocatedEntity> * m_scriptFactory;
      /// Default attribute values for this class
      Atlas::Message::MapType m_classAttributes;
      /// Default attribute values for instances of this class, including
      /// defaults inherited from parent classes.
      Atlas::Message::MapType m_attributes;
      /// Factory for class from which the class handled by this factory
      /// inherits.
      EntityFactoryBase * m_parent;
      /// Set of factories for classes which inherit from the class handled
      /// by this factory.
      std::set<EntityFactoryBase *> m_children;

      EntityFactoryBase();

      ~EntityFactoryBase() override;

      virtual EntityFactoryBase * duplicateFactory() = 0;

      void addProperties() override;

      void updateProperties(std::map<const TypeNode*, TypeNode::PropertiesUpdate>& changes) override;

};

/// \brief Concrete factory template for creating in-game entity objects.
template <class T>
class EntityFactory : public EntityFactoryBase {
  protected:
    EntityFactory(EntityFactory<T> & o);

  public:

    EntityFactory();

    ~EntityFactory() override;

    LocatedEntity * newEntity(const std::string & id, long intId,
                const Atlas::Objects::Entity::RootEntity & attributes, LocatedEntity* location) override;

    EntityFactoryBase * duplicateFactory() override;

};

#endif // SERVER_ENTITY_FACTORY_H
