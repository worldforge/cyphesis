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

// $Id: PropertyExerciser.h,v 1.2 2007-01-13 20:29:17 alriddoch Exp $

#ifndef TESTS_PROPERTY_EXERCISER_H
#define TESTS_PROPERTY_EXERCISER_H

#include <Atlas/Message/Element.h>

#include <list>

class PropertyBase;

class PropertyExerciser {
  private:
    Atlas::Message::IntType integer_ptr_target;
    Atlas::Message::FloatType float_ptr_target;
    Atlas::Message::PtrType ptr_ptr_target;
    Atlas::Message::StringType string_ptr_target;
    Atlas::Message::MapType map_ptr_target;
    Atlas::Message::ListType list_ptr_target;
    
    std::list<Atlas::Message::IntType> integer_values;
    std::list<Atlas::Message::FloatType> float_values;
    std::list<Atlas::Message::PtrType> ptr_values;
    std::list<Atlas::Message::StringType> string_values;
    std::list<Atlas::Message::MapType> map_values;
    std::list<Atlas::Message::ListType> list_values;

    template <typename T>
    void testSetByType(PropertyBase & property,
                       Atlas::Message::Element::Type element_type,
                       const std::list<T> & values);
    
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
