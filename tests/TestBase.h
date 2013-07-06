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

#include "common/compose.hpp"

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include <iostream>

namespace Cyphesis {

class Test
{
  public:
    const char * name;
    boost::function<void()> method;
};

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
    int assertTrue(const char * n, const V & val,
                   const char * func, const char * file, int line);

    template <typename L, typename R>
    int assertEqual(const char * l, const L & lval,
                    const char * r, const R & rval,
                    const char * func, const char * file, int line);

    template <typename L, typename R>
    int assertNotEqual(const char * l, const L & lval,
                       const char * r, const R & rval,
                       const char * func, const char * file, int line);

    template <typename L, typename R>
    int assertGreater(const char * l, const L & lval,
                      const char * r, const R & rval,
                      const char * func, const char * file, int line);

    template <typename L, typename R>
    int assertLess(const char * l, const L & lval,
                   const char * r, const R & rval,
                   const char * func, const char * file, int line);

    template <typename T>
    int assertNull(const char * n, const T * ptr,
                   const char * func, const char * file, int line);

    template <typename T>
    int assertNotNull(const char * n, const T * ptr,
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
    int error_count = 0;

    std::list<Test>::const_iterator Iend = m_tests.end();
    std::list<Test>::const_iterator I = m_tests.begin();
    for (; I != Iend; ++I) {
        setup();
        (*I).method();
        teardown();

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

template <typename V>
int TestBase::assertTrue(const char * n, const V & val,
                         const char * func, const char * file, int line)
{
    if (!val) {
        addFailure(String::compose("%1:%2: %3: Assertion '%4' failed.",
                                   file, line, func, n));
        return -1;
    }
    return 0;
}

template <typename L, typename R>
int TestBase::assertEqual(const char * l, const L & lval,
                          const char * r, const R & rval,
                          const char * func, const char * file, int line)
{
    if (lval != rval) {
        addFailure(String::compose("%1:%2: %3: Assertion '%4 == %5' failed. "
                                   "%6 != %7",
                                   file, line, func, l, r, lval, rval));
        return -1;
    }
    return 0;
}

template <typename L, typename R>
int TestBase::assertNotEqual(const char * l, const L & lval,
                             const char * r, const R & rval,
                             const char * func, const char * file, int line)
{
    if (lval == rval) {
        addFailure(String::compose("%1:%2: %3: Assertion '%4 != %5' failed. "
                                   "%6 == %7",
                                   file, line, func, l, r, lval, rval));
        return -1;
    }
    return 0;
}

template <typename L, typename R>
int TestBase::assertGreater(const char * l, const L & lval,
                            const char * r, const R & rval,
                            const char * func, const char * file, int line)
{
    if (lval <= rval) {
        addFailure(String::compose("%1:%2: %3: Assertion '%4 > %5' failed. "
                                   "%6 <= %7",
                                   file, line, func, l, r, lval, rval));
        return -1;
    }
    return 0;
}

template <typename L, typename R>
int TestBase::assertLess(const char * l, const L & lval,
                         const char * r, const R & rval,
                         const char * func, const char * file, int line)
{
    if (lval >= rval) {
        addFailure(String::compose("%1:%2: %3: Assertion '%4 < %5' failed. "
                                   "%6 >= %7",
                                   file, line, func, l, r, lval, rval));
        return -1;
    }
    return 0;
}

template <typename T>
int TestBase::assertNull(const char * n, const T * ptr,
                         const char * func, const char * file, int line)
{
    if (ptr != 0) {
        addFailure(String::compose("%1:%2: %3: Assertion '%4' null failed.",
                                   file, line, func, n));
        return -1;
    }
    return 0;
}

template <typename T>
int TestBase::assertNotNull(const char * n, const T * ptr,
                            const char * func, const char * file, int line)
{
    if (ptr == 0) {
        addFailure(String::compose("%1:%2: %3: Assertion '%4' not null failed.",
                                   file, line, func, n));
        return -1;
    }
    return 0;
}


}

#define ADD_TEST(_function) {\
    Cyphesis::Test _function_test = { #_function,\
                                      boost::bind(&_function, this) };\
    this->addTest(_function_test);\
}

#define ASSERT_TRUE(_expr) {\
    if (this->assertTrue(#_expr, _expr, __PRETTY_FUNCTION__,\
                         __FILE__, __LINE__) != 0) return;\
}

#define ASSERT_EQUAL(_lval, _rval) {\
    if (this->assertEqual(#_lval, _lval, #_rval, _rval, __PRETTY_FUNCTION__,\
                          __FILE__, __LINE__) != 0) return;\
}

#define ASSERT_NOT_EQUAL(_lval, _rval) {\
    if (this->assertNotEqual(#_lval, _lval, #_rval, _rval, __PRETTY_FUNCTION__,\
                             __FILE__, __LINE__) != 0) return;\
}

#define ASSERT_GREATER(_lval, _rval) {\
    if (this->assertGreater(#_lval, _lval, #_rval, _rval, __PRETTY_FUNCTION__,\
                            __FILE__, __LINE__) != 0) return;\
}

#define ASSERT_LESS(_lval, _rval) {\
    if (this->assertLess(#_lval, _lval, #_rval, _rval, __PRETTY_FUNCTION__,\
                         __FILE__, __LINE__) != 0) return;\
}

#define ASSERT_NULL(_ptr) {\
    if (this->assertNull(#_ptr, _ptr, __PRETTY_FUNCTION__,\
                         __FILE__, __LINE__) != 0) return;\
}

#define ASSERT_NOT_NULL(_ptr) {\
    if (this->assertNotNull(#_ptr, _ptr, __PRETTY_FUNCTION__,\
                            __FILE__, __LINE__) != 0) return;\
}

#endif // TESTS_TEST_BASE_H
