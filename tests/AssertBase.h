/*
 Copyright (C) 2020 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef TESTS_ASSERT_BASE_H
#define TESTS_ASSERT_BASE_H

#include <cmath>


namespace Cyphesis {

    struct AssertBase
    {
        int m_error_count;
        // Need to track not just counts, but also details of the failures,
        // which basically requires macros.
        std::list <std::string> m_errorReports;

        void addFailure(const std::string& s)
        {
            m_errorReports.push_back(s);
        }

        std::size_t errorCount() const;

        const std::list <std::string>& errorReports() const;

        template<typename V>
        int assertTrue(const char* n, const V& val,
                       const char* func, const char* file, int line);

        template<typename V>
        int assertFalse(const char* n, const V& val,
                        const char* func, const char* file, int line);

        template<typename L, typename R>
        int assertEqual(const char* l, const L& lval,
                        const char* r, const R& rval,
                        const char* func, const char* file, int line);

        template<typename L, typename R, typename E>
        int assertFuzzyEqual(const char* l, const L& lval,
                             const char* r, const R& rval,
                             const char* e, const E& epsilon,
                             const char* func, const char* file, int line);

        template<typename L, typename R>
        int assertNotEqual(const char* l, const L& lval,
                           const char* r, const R& rval,
                           const char* func, const char* file, int line);

        template<typename L, typename R>
        int assertGreater(const char* l, const L& lval,
                          const char* r, const R& rval,
                          const char* func, const char* file, int line);

        template<typename L, typename R>
        int assertLess(const char* l, const L& lval,
                       const char* r, const R& rval,
                       const char* func, const char* file, int line);

        template<typename T>
        int assertNull(const char* n, const T* ptr,
                       const char* func, const char* file, int line);

        template<typename T>
        int assertNotNull(const char* n, const T* ptr,
                          const char* func, const char* file, int line);
    };


    std::size_t AssertBase::errorCount() const
    {
        return m_errorReports.size();
    }

    const std::list<std::string> & AssertBase::errorReports() const
    {
        return m_errorReports;
    }


    template <typename V>
    int AssertBase::assertTrue(const char * n, const V & val,
                               const char * func, const char * file, int line)
    {
        if (!val) {
            addFailure(String::compose("%1:%2: %3: Assertion '%4' failed.",
                                       file, line, func, n));
            return -1;
        }
        return 0;
    }

    template <typename V>
    int AssertBase::assertFalse(const char * n, const V & val,
                                const char * func, const char * file, int line)
    {
        if (val) {
            addFailure(String::compose("%1:%2: %3: Assertion '%4' failed.",
                                       file, line, func, n));
            return -1;
        }
        return 0;
    }

    template <typename L, typename R>
    int AssertBase::assertEqual(const char * l, const L & lval,
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

    template <typename L, typename R, typename E>
    int AssertBase::assertFuzzyEqual(const char * l, const L & lval,
                                     const char * r, const R & rval,
                                     const char * e, const E & epsilon,
                                     const char * func, const char * file, int line)
    {
        if (std::abs(lval - rval) > epsilon) {
            addFailure(String::compose("%1:%2: %3: Assertion '%4 ~= %5' failed. "
                                       "%6 != %7",
                                       file, line, func, l, r, lval, rval));
            return -1;
        }
        return 0;
    }

    template <typename L, typename R>
    int AssertBase::assertNotEqual(const char * l, const L & lval,
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
    int AssertBase::assertGreater(const char * l, const L & lval,
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
    int AssertBase::assertLess(const char * l, const L & lval,
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
    int AssertBase::assertNull(const char * n, const T * ptr,
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
    int AssertBase::assertNotNull(const char * n, const T * ptr,
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


#define ASSERT_TRUE(_expr) {\
    if (this->assertTrue(#_expr, _expr, __PRETTY_FUNCTION__,\
                         __FILE__, __LINE__) != 0) return;\
}

#define ASSERT_FALSE(_expr) {\
    if (this->assertFalse(#_expr, _expr, __PRETTY_FUNCTION__,\
                         __FILE__, __LINE__) != 0) return;\
}

#define ASSERT_EQUAL(_lval, _rval) {\
    if (this->assertEqual(#_lval, _lval, #_rval, _rval, __PRETTY_FUNCTION__,\
                          __FILE__, __LINE__) != 0) return;\
}

#define ASSERT_FUZZY_EQUAL(_lval, _rval, _epsilon) {\
    if (this->assertFuzzyEqual(#_lval, _lval, #_rval, _rval, #_epsilon, _epsilon, __PRETTY_FUNCTION__,\
                          __FILE__, __LINE__) != 0) return;\
}

#define ASSERT_FUZZY_EQUAL_FN(_lval, _rval, _epsilon, _fn) {\
    if (this->assertFuzzyEqual(#_lval, _lval, #_rval, _rval, #_epsilon, _epsilon, __PRETTY_FUNCTION__,\
                          __FILE__, __LINE__) != 0) {_fn(); return;}\
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

#endif //TESTS_ASSERT_BASE_H
