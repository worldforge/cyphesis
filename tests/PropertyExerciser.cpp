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


#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "PropertyExerciser.h"

#include "common/random.h"
#include "common/Property.h"
#include "common/Inheritance.h"

#include <Atlas/Objects/Anonymous.h>

#include <iostream>
#include <limits>

#include <cassert>
#include <Atlas/Objects/Factories.h>

using Atlas::Message::Element;
using Atlas::Message::IntType;
using Atlas::Message::PtrType;
using Atlas::Message::FloatType;
using Atlas::Message::StringType;
using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Objects::Entity::Anonymous;

Atlas::Objects::Factories factories;
PropertyExerciser::PropertyExerciser()
: m_inheritance(std::make_unique<Inheritance>(factories))
{
    integer_values.push_back(0);
    integer_values.push_back(-1);
    integer_values.push_back(23);
    integer_values.push_back(42);
    integer_values.push_back(std::numeric_limits<int>::max());
    integer_values.push_back(std::numeric_limits<int>::min());
    integer_values.push_back(std::numeric_limits<unsigned int>::max());
    integer_values.push_back(std::numeric_limits<long>::max());
    integer_values.push_back(std::numeric_limits<long>::min());

    float_values.push_back(0.f);
    float_values.push_back(-0.f);
    float_values.push_back(1.f);
    float_values.push_back(-1.f);
    float_values.push_back(23.f);
    float_values.push_back(42.f);
    float_values.push_back(-23.f);
    float_values.push_back(-42.f);
    float_values.push_back(__FLT_MIN__);
    float_values.push_back(__FLT_MAX__);
    float_values.push_back(__DBL_MIN__);
    float_values.push_back(__DBL_MAX__);

    ptr_values.push_back(0);
    ptr_values.push_back(&integer_ptr_target);
    ptr_values.push_back(&float_ptr_target);
    ptr_values.push_back(&ptr_ptr_target);
    ptr_values.push_back(&string_ptr_target);
    ptr_values.push_back(&map_ptr_target);
    ptr_values.push_back(&list_ptr_target);

    string_values.push_back("");
    string_values.push_back("hello world");
    string_values.push_back("goodbye, cruel world");
    string_values.push_back(std::string(32768, ' '));
    string_values.push_back(std::string(32768, 'a'));
    string_values.push_back(std::string(32768, 'B'));
    string_values.push_back(std::string(32768, '0'));
    string_values.push_back("!£$%^&*()_+}{[]:@~#';<>?/.,l\\|");
    string_values.push_back("pwu3dc5012cw*/-+3+Q£%$\"q%2");

    // Add all the standard class names to the string values list
    auto& allTypes = Inheritance::instance().getAllObjects();
    auto J = allTypes.begin();
    auto Jend = allTypes.end();
    for (; J != Jend; ++J) {
        string_values.push_back(J->first);
    }
    // FIXME Add all the common property names to the string values

    // empty list
    list_values.push_back(ListType());

    // A number of lists of ints
    std::vector<IntType>::const_iterator I = integer_values.begin();
    std::vector<IntType>::const_iterator Iend = integer_values.end();
    for (; I != Iend; ++I) {
        list_values.push_back(ListType(1, *I));
    }

    // A number of lists of floats
    std::vector<FloatType>::const_iterator F = float_values.begin();
    std::vector<FloatType>::const_iterator Fend = float_values.end();
    for (; F != Fend; ++F) {
        list_values.push_back(ListType(1, *F));
    }

    // A number of lists of strings
    std::vector<StringType>::const_iterator S = string_values.begin();
    std::vector<StringType>::const_iterator Send = string_values.end();
    for (; S != Send; ++S) {
        list_values.push_back(ListType(1, *S));
    }

    // A number of lists of pointers
    std::vector<PtrType>::const_iterator P = ptr_values.begin();
    std::vector<PtrType>::const_iterator Pend = ptr_values.end();
    for (; P != Pend; ++P) {
        list_values.push_back(ListType(1, *P));
    }

    // A list of all the ints
    list_values.push_back(ListType());
    I = integer_values.begin();
    for (; I != Iend; ++I) {
        list_values.back().push_back(*I);
    }

    // A list of all the floats
    list_values.push_back(ListType());
    F = float_values.begin();
    for (; F != Fend; ++F) {
        list_values.back().push_back(*F);
    }

    // A list of all the strings
    list_values.push_back(ListType());
    S = string_values.begin();
    for (; S != Send; ++S) {
        list_values.back().push_back(*S);
    }

    // A list of all the pointers
    list_values.push_back(ListType());
    P = ptr_values.begin();
    for (; P != Pend; ++P) {
        list_values.back().push_back(*P);
    }

    // A heterogenous list
    list_values.push_back(ListType());
    list_values.back().push_back(integer_values.front());
    list_values.back().push_back(float_values.front());
    list_values.back().push_back(ptr_values.front());
    list_values.back().push_back(string_values.front());

    for (int i = 0; i < 64; ++i) {
        list_values.push_back(ListType());
        for (int j = 0; j < 64; ++j) {
            list_values.back().push_back(randomAtlasValue());
        }
    }

    // empty list
    map_values.push_back(MapType());

    std::vector<std::string>::const_iterator K = string_values.begin();
    std::vector<std::string>::const_iterator Kend = string_values.end();
    for (; K != Kend; ++K) {
    
        // A number of maps of ints
        I = integer_values.begin();
        Iend = integer_values.end();
        for (; I != Iend; ++I) {
            map_values.push_back(MapType());
            map_values.back().emplace(*K, *I);
        }

        // A number of maps of floats
        F = float_values.begin();
        Fend = float_values.end();
        for (; F != Fend; ++F) {
            map_values.back().emplace(*K, *F);
        }

        // A number of maps of strings
        S = string_values.begin();
        Send = string_values.end();
        for (; S != Send; ++S) {
            map_values.back().emplace(*K, *S);
        }

        // A number of maps of pointers
        P = ptr_values.begin();
        Pend = ptr_values.end();
        for (; P != Pend; ++P) {
            map_values.back().emplace(*K, *P);
        }

    }

    // A list of all the ints
    map_values.push_back(MapType());
    I = integer_values.begin();
    for (; I != Iend; ++I) {
        map_values.back().emplace(randomString(), *I);
    }

    // A list of all the floats
    map_values.push_back(MapType());
    F = float_values.begin();
    for (; F != Fend; ++F) {
        map_values.back().emplace(randomString(), *F);
    }

    // A list of all the strings
    map_values.push_back(MapType());
    S = string_values.begin();
    for (; S != Send; ++S) {
        map_values.back().emplace(randomString(), *S);
    }

    // A list of all the pointers
    map_values.push_back(MapType());
    P = ptr_values.begin();
    for (; P != Pend; ++P) {
        map_values.back().emplace(randomString(), *P);
    }

    // A map full of random crap
    for (int i = 0; i < 64; ++i) {
        map_values.push_back(MapType());
        for (int j = 0; j < 64; ++j) {
            map_values.back().emplace(randomString(), randomAtlasValue());
        }
    }

    // Now that map_values is populated, use it to put some random stuff in
    // list_values

    // A number of lists of maps
    std::vector<MapType>::const_iterator M = map_values.begin();
    std::vector<MapType>::const_iterator Mend = map_values.end();
    for (; M != Mend; ++M) {
        list_values.push_back(ListType(1, *M));
    }

    // A list of all the maps
    list_values.push_back(ListType());
    M = map_values.begin();
    for (; M != Mend; ++M) {
        list_values.back().push_back(*M);
    }

}

Element PropertyExerciser::randomAtlasValue()
{
    switch (randint(0, 6)) {
      case 0:
        if (integer_values.empty()) {
            integer_values.push_back(0);
        }
        return integer_values[randint(0, integer_values.size())];
        break;
      case 1:
        if (float_values.empty()) {
            float_values.push_back(0.1f);
        }
        return float_values[randint(0, float_values.size())];
        break;
      case 2:
        if (ptr_values.empty()) {
            ptr_values.push_back(nullptr);
        }
        return ptr_values[randint(0, ptr_values.size())];
        break;
      case 3:
        if (string_values.empty()) {
            string_values.push_back("");
        }
        return string_values[randint(0, string_values.size())];
        break;
      case 4:
        if (list_values.empty()) {
            list_values.push_back(ListType());
        }
        return list_values[randint(0, list_values.size())];
        break;
      case 5:
        if (map_values.empty()) {
            map_values.push_back(MapType());
        }
        return map_values[randint(0, map_values.size())];
        break;
      case 6:
        std::cout << "NNOON 6" << std::endl << std::flush;
      default:
        return Element();
        break;
    }
}

const std::string & PropertyExerciser::randomString() const
{
    static const std::string empty_string;
    if (string_values.empty()) {
        return empty_string;
    }
    return string_values[randint(0, string_values.size())];
}

void PropertyExerciser::testGet(PropertyBase & property, 
                                Element::Type element_type)
{
    Element get_target;
    if (property.get(get_target) == 0) {
        assert(get_target.getType() == element_type);
    } else {
        assert(get_target.getType() == Element::TYPE_NONE);
    }
}

void PropertyExerciser::testAdd(PropertyBase & property,
                                Element::Type element_type)
{
    Anonymous add_target;
    MapType add_target2;
    std::vector<StringType>::const_iterator I = string_values.begin();
    std::vector<StringType>::const_iterator Iend = string_values.end();
    for (; I != Iend; ++I) {
        property.add(*I, add_target);
        property.add(*I, add_target2);
    }

}

template <typename T>
void PropertyExerciser::testSetByType(PropertyBase & property,
                                      Element::Type element_type,
                                      const std::vector<T> & values)
{
    typename std::vector<T>::const_iterator I = values.begin();
    typename std::vector<T>::const_iterator Iend = values.end();
    for (; I != Iend; ++I) {
        property.set(*I);
        testGet(property, element_type);
        testAdd(property, element_type);
    }
}

void PropertyExerciser::testSet(PropertyBase & property,
                                Element::Type element_type)
{
    testSetByType(property, element_type, integer_values);
    testSetByType(property, element_type, float_values);
    testSetByType(property, element_type, ptr_values);
    testSetByType(property, element_type, string_values);
    testSetByType(property, element_type, map_values);
    testSetByType(property, element_type, list_values);
}

int PropertyExerciser::exerciseProperty(PropertyBase & property,
                                        Element::Type element_type)
{
    testGet(property, element_type);
    testAdd(property, element_type);
    testSet(property, element_type);
    return 0;
}
