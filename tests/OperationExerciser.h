// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2006 Alistair Riddoch
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


#ifndef TESTS_OPERATION_EXERCISER_H
#define TESTS_OPERATION_EXERCISER_H

#include <string>

#include <cassert>

template <class Op>
class OperationExerciser {
  protected:
    void testPlainConstructor() {
        {
            Op op;
        }

        {
            auto op = new Op();
            delete op;
        }
    }

    void testClassNo(const int class_no) {
        {
            Op op;
            assert(class_no == op->getClassNo());
        }
    }

    void testTypeString(const std::string & type) {
        {
            Op op;
            assert(!op->isDefaultParent());
        }

        {
            Op op;
            assert(type == op->getParent());
        }
    }

  public:
    OperationExerciser() { }

    ~OperationExerciser() { }

    void runTests(const std::string & type, const int class_no) {
        testPlainConstructor();
        testClassNo(class_no);
        testTypeString(type);
    }
};

#endif // TESTS_OPERATION_EXERCISER_H
