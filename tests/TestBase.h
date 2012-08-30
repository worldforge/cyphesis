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

// $Id$

#ifndef TEST_TEST_BASE_H
#define TEST_TEST_BASE_H

#include "common/compose.hpp"

#include <boost/bind.hpp>
#include <boost/function.hpp>

namespace Cyphesis {

typedef boost::function<void()> Test;

class TestBase
{
  protected:
    int m_error_count;
    // Need to track not just counts, but also details of the failures,
    // which basically requires macros.
    std::list<std::string> m_errorReports;

    std::list<Test> m_tests;

    void addFailure(const std::string & s)
    {
        m_errorReports.push_back(s);
    }
  public:
    virtual ~TestBase();

    virtual void setup() = 0;
    virtual void teardown() = 0;

    std::size_t errorCount() const;

    const std::list<std::string> & errorReports() const;

    void addTest(Test t);

    int run();

    template <typename V>
    void assertTrue(const char * n, const V & val,
                    const char * func, const char * file, int line);

    template <typename L, typename R>
    void assertEqual(const char * l, const L & lval,
                     const char * r, const R & rval,
                     const char * func, const char * file, int line);
};

inline
TestBase::~TestBase()
{
}

std::size_t TestBase::errorCount() const
{
    return m_errorReports.size();
}

const std::list<std::string> & TestBase::errorReports() const
{
    return m_errorReports;
}

void TestBase::addTest(Test t)
{
    m_tests.push_back(t);
}

int TestBase::run()
{
    std::list<Test>::const_iterator Iend = m_tests.end();
    std::list<Test>::const_iterator I = m_tests.begin();
    for (; I != Iend; ++I) {
        setup();
        (*I)();
        teardown();
    }

    if (m_errorReports.empty()) {
        return 0;
    }
    return -1;
}

template <typename V>
void TestBase::assertTrue(const char * n, const V & val,
                      const char * func, const char * file, int line)
{
    if (!val) {
        addFailure(String::compose("%1:%2: %3: Assertion '%4' failed.",
                                   file, line, func, n));
    }
}

template <typename L, typename R>
void TestBase::assertEqual(const char * l, const L & lval,
                       const char * r, const R & rval,
                       const char * func, const char * file, int line)
{
    if (lval != rval) {
        addFailure(String::compose("%1:%2: %3: Assertion '%4 == %5' failed. "
                                   "%6 != %7",
                                   file, line, func, l, r, lval, rval));
    }
}

}

#define ADD_TEST(_function) { this->addTest(boost::bind(&_function, this)); }

#define ASSERT_TRUE(_expr) {\
    this->assertTrue(#_expr, _expr, __PRETTY_FUNCTION__, __FILE__, __LINE__);}

#define ASSERT_EQUAL(_lval, _rval) {\
    this->assertEqual(#_lval, _lval, #_rval, _rval, __PRETTY_FUNCTION__,\
                      __FILE__, __LINE__);}

#endif // TEST_TEST_BASE_H
