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


#ifndef TESTS_PYTHON_TESTERS_H
#define TESTS_PYTHON_TESTERS_H

#define run_python_string(_s) { int pyret = CyPyRun_SimpleString(_s); \
                                assert(pyret == 0); }

#define fail_python_string(_s) { int pyret = CyPyRun_SimpleString(_s); \
                                 assert(pyret == -1); }

#define expect_python_error(_s, _e) { int pyret = CyPyRun_SimpleString(_s, _e); \
                                      assert(pyret == -3); }

int CyPyRun_SimpleString(const char * s,
                         struct _object * exception = 0);

#endif // TESTS_PYTHON_TESTERS_H
