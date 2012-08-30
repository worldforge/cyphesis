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

namespace Cyphesis {

class Test
{
  protected:
    int m_error_count;
    // Need to track not just counts, but also details of the failures,
    // which basically requires macros.
    std::list<std::string> m_errorReports;

    void addFailure(const std::string & s)
    {
        m_errorReports.push_back(s);
    }
  public:
    virtual ~Test();

    std::size_t errorCount() const;

    const std::list<std::string> & errorReports() const;

    template <typename V>
    void assertTrue(const char * n, const V & val,
                    const char * func, const char * file, int line);

    template <typename L, typename R>
    void assertEqual(const char * l, const L & lval,
                     const char * r, const R & rval,
                     const char * func, const char * file, int line);
};

inline
Test::~Test()
{
}

std::size_t Test::errorCount() const
{
    return m_errorReports.size();
}

const std::list<std::string> & Test::errorReports() const
{
    return m_errorReports;
}

template <typename V>
void Test::assertTrue(const char * n, const V & val,
                      const char * func, const char * file, int line)
{
    if (!val) {
        addFailure(String::compose("%1:%2: %3: Assertion '%4' failed.",
                                   file, line, func, n));
    }
}

template <typename L, typename R>
void Test::assertEqual(const char * l, const L & lval,
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

#define ASSERT_TRUE(_expr) {\
    this->assertTrue(#_expr, _expr, __PRETTY_FUNCTION__, __FILE__, __LINE__);}

#define ASSERT_EQUAL(_lval, _rval) {\
    this->assertEqual(#_lval, _lval, #_rval, _rval, __PRETTY_FUNCTION__,\
                      __FILE__, __LINE__);}

#endif // TEST_TEST_BASE_H
