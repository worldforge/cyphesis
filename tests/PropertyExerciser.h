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


#ifndef TESTS_PROPERTY_EXERCISER_H
#define TESTS_PROPERTY_EXERCISER_H

#include <Atlas/Message/Element.h>

#include <list>
#include <memory>
#include <common/Inheritance.h>


class PropertyExerciser {
  private:
    Atlas::Message::IntType integer_ptr_target;
    Atlas::Message::FloatType float_ptr_target;
    Atlas::Message::PtrType ptr_ptr_target;
    Atlas::Message::StringType string_ptr_target;
    Atlas::Message::MapType map_ptr_target;
    Atlas::Message::ListType list_ptr_target;
    
    std::vector<Atlas::Message::IntType> integer_values;
    std::vector<Atlas::Message::FloatType> float_values;
    std::vector<Atlas::Message::PtrType> ptr_values;
    std::vector<Atlas::Message::StringType> string_values;
    std::vector<Atlas::Message::MapType> map_values;
    std::vector<Atlas::Message::ListType> list_values;

    Atlas::Message::Element randomAtlasValue();
    const std::string & randomString() const;

    std::unique_ptr<Inheritance> m_inheritance;

    template <typename T>
    void testSetByType(PropertyBase & property,
                       Atlas::Message::Element::Type element_type,
                       const std::vector<T> & values);
    
    void testGet(PropertyBase & property,
                 Atlas::Message::Element::Type element_type);
    void testAdd(PropertyBase & property,
                 Atlas::Message::Element::Type element_type);
    void testSet(PropertyBase & property,
                 Atlas::Message::Element::Type element_type);
  public:
    PropertyExerciser();

    int exerciseProperty(PropertyBase & property,
                         Atlas::Message::Element::Type element_type);
};


#endif // TESTS_PROPERTY_EXERCISER_H
