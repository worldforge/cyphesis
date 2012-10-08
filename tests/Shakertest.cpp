// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2012 Anthony Pesce
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

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "TestBase.h"
#include <string>
#include "common/Shaker.h"

class Shakertest : public Cyphesis::TestBase
{
  
  
  protected:
    Shaker * m_shaker;
  public:
    Shakertest();
    void setup();
    void teardown();
    void test_generate();

};

Shakertest::Shakertest() 
{
	ADD_TEST(Shakertest::test_generate);
}

void Shakertest::setup()
{
	m_shaker = new Shaker;
}

void Shakertest::teardown()
{
	delete m_shaker;
}

void Shakertest::test_generate()
{
	std::string s = m_shaker->generate_salt(1000);
    ASSERT_NOT_NULL(&s);
}



int main()
{
	Shakertest t;
	
	return t.run();
}



