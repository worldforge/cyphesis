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

// $Id: Droptest.cpp,v 1.1 2006-12-05 09:40:01 alriddoch Exp $

#include "OperationExerciser.h"

#include "common/Drop.h"
#include "common/random.h"

int main()
{
    OperationExerciser<Atlas::Objects::Operation::Drop> op_tester;

    Atlas::Objects::Operation::DROP_NO = randint(1, 100);

    op_tester.runTests("drop", Atlas::Objects::Operation::DROP_NO);

    return 0;
}
