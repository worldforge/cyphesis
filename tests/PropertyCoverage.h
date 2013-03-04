// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2009 Alistair Riddoch
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

// $Id$

#ifndef TESTS_PROPERTY_COVERAGE_H
#define TESTS_PROPERTY_COVERAGE_H

#include <Atlas/Message/Element.h>

class PropertyBase;
class Entity;
class BaseWorld;
class Character;

class PropertyCoverage {
  protected:
    PropertyBase * const m_prop;
    Entity * const m_tlve;
    BaseWorld * const m_wrld;
    Entity * m_ent;

    Atlas::Message::ListType m_testData;

    virtual void interfaceCoverage();
  public:

    explicit PropertyCoverage(PropertyBase * pb);

    ~PropertyCoverage();

    void basicCoverage();

    Character * createCharacterEntity();

    void testDataAppend(const Atlas::Message::Element &);

    Entity * tlve() { return m_tlve; }
};

template<class PropertyT>
class PropertyChecker : public PropertyCoverage
{
  protected:
    PropertyT * m_sub_prop;

    virtual void interfaceCoverage();
  public:
    explicit PropertyChecker(PropertyT * p);
};

template<class PropertyT>
PropertyChecker<PropertyT>::PropertyChecker(PropertyT * p) :
      PropertyCoverage(p), m_sub_prop(p)
{
}

// This test works at compile time, by ensuring the copy method returns the
// subclass type.
template<class PropertyT>
void PropertyChecker<PropertyT>::interfaceCoverage()
{
    PropertyT * copy = m_sub_prop->copy();
    // The above line generates an unused variable warning without the line
    // below. The purpose of this test is not to care about the value, just
    // to ensure the subclass has implemented this method to return its own
    // type, so we cast the result to void to tell the compiler that we
    // know its unused.
    (void)copy;
}

#endif // TESTS_PROPERTY_COVERAGE_H
