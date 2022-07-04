// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2012 Alistair Riddoch
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


#ifndef TESTS_TEST_BASE_H
#define TESTS_TEST_BASE_H

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "common/compose.hpp"
#include "AssertBase.h"

#include <Atlas/Message/Element.h>

#include <boost/bind/bind.hpp>
#include <boost/function.hpp>

#include <iostream>
#include <cmath>

//debug.cpp is linked to all tests, but we don't want to include debug.h since the "debug" macro creates conflicts.
template <typename T>
void debug_dump(const T & t, std::ostream &);

// If tests fail, and print out the message below, you'll have to actually
// implement this function to find out the details.
std::ostream& operator<<(std::ostream& os, const Atlas::Message::MapType& v)
{
    os << "[ATLAS_MAP]";
    return os;
}

std::ostream& operator<<(std::ostream& os,
                         const Atlas::Message::Element& e)
{
    debug_dump(e, os);
    return os;
}

namespace Atlas
{
namespace Message
{
std::ostream& operator<<(std::ostream& os, const Atlas::Message::MapType& v)
{
    os << "[ATLAS_MAP]";
    return os;
}

std::ostream& operator<<(std::ostream& os, const Atlas::Message::Element& e)
{
    debug_dump(e, os);
    return os;
}
}
}

namespace Cyphesis {

class Test
{
  public:
    const char * name;
    boost::function<void()> method;
};

class TestBase : public AssertBase
{
  protected:

    std::list<Test> m_tests;


  public:
    virtual ~TestBase();

    virtual void setup() = 0;
    virtual void teardown() = 0;

    void addTest(Test t);

    int run();


};

inline
TestBase::~TestBase()
{
}

void TestBase::addTest(Test t)
{
    m_tests.push_back(t);
}

int TestBase::run()
{
    int error_count = 0;

    std::list<Test>::const_iterator Iend = m_tests.end();
    std::list<Test>::const_iterator I = m_tests.begin();
    for (; I != Iend; ++I) {
        setup();
        std::cerr << "Starting test " << I->name << std::endl << std::flush;
        (*I).method();
        teardown();
        std::cerr << "Completed test " << I->name << std::endl << std::flush;

        if (!m_errorReports.empty()) {
            ++error_count;

            std::cerr << "Test \"" << (*I).name << "\" failed:" << std::endl;

            std::list<std::string>::const_iterator I = m_errorReports.begin();
            std::list<std::string>::const_iterator Iend = m_errorReports.end();
            for (; I != Iend; ++I) {
                std::cerr << *I << std::endl;
            }

            m_errorReports.clear();
        }
    }

    return error_count;
}


}

#define ADD_TEST(_function) {\
    Cyphesis::Test _function_test = { #_function,\
                                      boost::bind(&_function, this) };\
    this->addTest(_function_test);\
}

#endif // TESTS_TEST_BASE_H
