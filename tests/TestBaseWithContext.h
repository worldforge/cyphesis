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

#include <Atlas/Message/Element.h>

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include <iostream>
#include <cmath>

//debug.cpp is linked to all tests, but we don't want to include debug.h since the "debug" macro creates conflicts.
template<typename T>
void debug_dump(const T& t, std::ostream&);

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


namespace Cyphesis {

    template<typename ContextT>
    struct Test
    {
        const char* name;
        std::function<void(ContextT&)> method;
    };

    /**
     * Base class for tests. The template parameter ContextT declares a "context" class.
     * Before each test is invoked an instance of ContextT will be created, and destroyed after invokation.
     * The intent is that ContextT should contain all structures that are relevant to the tests.
     * @tparam ContextT
     */
    template<typename ContextT>
    class TestBaseWithContext
    {
        protected:
            int m_error_count;
            // Need to track not just counts, but also details of the failures,
            // which basically requires macros.
            std::list<std::string> m_errorReports;

            std::list<Test<ContextT>> m_tests;

            void addFailure(std::string s)
            {
                m_errorReports.emplace_back(std::move(s));
            }

        public:
            virtual ~TestBaseWithContext();

            std::size_t errorCount() const;

            const std::list<std::string>& errorReports() const;

            void addTest(Test<ContextT> t);

            void addTest(const char* test_name, std::function<void(ContextT&)> method);

            int run();

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

    template<typename ContextT>
    inline TestBaseWithContext<ContextT>::~TestBaseWithContext()
    {
    }

    template<typename ContextT>
    std::size_t TestBaseWithContext<ContextT>::errorCount() const
    {
        return m_errorReports.size();
    }

    template<typename ContextT>
    const std::list<std::string>& TestBaseWithContext<ContextT>::errorReports() const
    {
        return m_errorReports;
    }

    template<typename ContextT>
    void TestBaseWithContext<ContextT>::addTest(Test<ContextT> t)
    {
        m_tests.emplace_back(t);
    }

    template<typename ContextT>
    void TestBaseWithContext<ContextT>::addTest(const char* test_name, std::function<void(ContextT&)> method)
    {
        m_tests.emplace_back(Test<ContextT>{test_name, method});
    }


    template<typename ContextT>
    int TestBaseWithContext<ContextT>::run()
    {
        int error_count = 0;

        for (auto& test : m_tests) {
            std::cerr << "Starting test " << test.name << std::endl << std::flush;
            {
                ContextT context{};
                test.method(context);
            }
            std::cerr << "Completed test " << test.name << std::endl << std::flush;

            if (!m_errorReports.empty()) {
                ++error_count;

                std::cerr << "Test \"" << test.name << "\" failed:" << std::endl;

                for (auto& report : m_errorReports) {
                    std::cerr << report << std::endl;
                }

                m_errorReports.clear();
            }
        }

        return error_count;
    }

    template<typename ContextT>
    template<typename V>
    int TestBaseWithContext<ContextT>::assertTrue(const char* n, const V& val,
                                                  const char* func, const char* file, int line)
    {
        if (!val) {
            addFailure(String::compose("%1:%2: %3: Assertion '%4' failed.",
                                       file, line, func, n));
            return -1;
        }
        return 0;
    }

    template<typename ContextT>
    template<typename V>
    int TestBaseWithContext<ContextT>::assertFalse(const char* n, const V& val,
                                                   const char* func, const char* file, int line)
    {
        if (val) {
            addFailure(String::compose("%1:%2: %3: Assertion '%4' failed.",
                                       file, line, func, n));
            return -1;
        }
        return 0;
    }

    template<typename ContextT>
    template<typename L, typename R>
    int TestBaseWithContext<ContextT>::assertEqual(const char* l, const L& lval,
                                                   const char* r, const R& rval,
                                                   const char* func, const char* file, int line)
    {
        if (lval != rval) {
            addFailure(String::compose("%1:%2: %3: Assertion '%4 == %5' failed. "
                                       "%6 != %7",
                                       file, line, func, l, r, lval, rval));
            return -1;
        }
        return 0;
    }

    template<typename ContextT>
    template<typename L, typename R, typename E>
    int TestBaseWithContext<ContextT>::assertFuzzyEqual(const char* l, const L& lval,
                                                        const char* r, const R& rval,
                                                        const char* e, const E& epsilon,
                                                        const char* func, const char* file, int line)
    {
        if (std::abs(lval - rval) > epsilon) {
            addFailure(String::compose("%1:%2: %3: Assertion '%4 ~= %5' failed. "
                                       "%6 != %7",
                                       file, line, func, l, r, lval, rval));
            return -1;
        }
        return 0;
    }

    template<typename ContextT>
    template<typename L, typename R>
    int TestBaseWithContext<ContextT>::assertNotEqual(const char* l, const L& lval,
                                                      const char* r, const R& rval,
                                                      const char* func, const char* file, int line)
    {
        if (lval == rval) {
            addFailure(String::compose("%1:%2: %3: Assertion '%4 != %5' failed. "
                                       "%6 == %7",
                                       file, line, func, l, r, lval, rval));
            return -1;
        }
        return 0;
    }

    template<typename ContextT>
    template<typename L, typename R>
    int TestBaseWithContext<ContextT>::assertGreater(const char* l, const L& lval,
                                                     const char* r, const R& rval,
                                                     const char* func, const char* file, int line)
    {
        if (lval <= rval) {
            addFailure(String::compose("%1:%2: %3: Assertion '%4 > %5' failed. "
                                       "%6 <= %7",
                                       file, line, func, l, r, lval, rval));
            return -1;
        }
        return 0;
    }

    template<typename ContextT>
    template<typename L, typename R>
    int TestBaseWithContext<ContextT>::assertLess(const char* l, const L& lval,
                                                  const char* r, const R& rval,
                                                  const char* func, const char* file, int line)
    {
        if (lval >= rval) {
            addFailure(String::compose("%1:%2: %3: Assertion '%4 < %5' failed. "
                                       "%6 >= %7",
                                       file, line, func, l, r, lval, rval));
            return -1;
        }
        return 0;
    }

    template<typename ContextT>
    template<typename T>
    int TestBaseWithContext<ContextT>::assertNull(const char* n, const T* ptr,
                                                  const char* func, const char* file, int line)
    {
        if (ptr != 0) {
            addFailure(String::compose("%1:%2: %3: Assertion '%4' null failed.",
                                       file, line, func, n));
            return -1;
        }
        return 0;
    }

    template<typename ContextT>
    template<typename T>
    int TestBaseWithContext<ContextT>::assertNotNull(const char* n, const T* ptr,
                                                     const char* func, const char* file, int line)
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
    this->addTest(#_function, [&](auto& context){_function(context);});\
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
