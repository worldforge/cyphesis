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

#include "../TestBase.h"

#include "common/Shaker.h"

using namespace std;

class Shakertest : public Cyphesis::TestBase
{
  
  
  protected:
    Shaker * m_shaker;
  public:
    Shakertest();
    void setup();
    void teardown();
    void testGenerate();

};

Shakertest::Shakertest() 
{
    ADD_TEST(Shakertest::testGenerate);
}

void Shakertest::setup()
{
    m_shaker = new Shaker;
}

void Shakertest::teardown()
{
    delete m_shaker;
}

void Shakertest::testGenerate()
{
    unsigned int salt_length1 = 0;
    unsigned int salt_length2 = 8;
    unsigned int salt_length3 = 100;    
    std::string s1  = m_shaker->generateSalt(salt_length1);
    ASSERT_NOT_NULL(&s1);
    ASSERT_TRUE(salt_length1*2==s1.length());
    for(unsigned int i =0;i<s1.length();i++)
    {
        ASSERT_TRUE(isxdigit(s1[i]));
    }

    std::string s2 = m_shaker->generateSalt(salt_length2);
    ASSERT_NOT_NULL(&s2);
    ASSERT_TRUE(salt_length2*2==s2.length());
    for(unsigned int i =0;i<s2.length();i++)
    {
        ASSERT_TRUE(isxdigit(s2[i]));
    }

    std::string s3 = m_shaker->generateSalt(salt_length3);
    ASSERT_NOT_NULL(&s3);
    ASSERT_TRUE(salt_length3*2==s3.length());
    for(unsigned int i =0;i<s3.length();i++)
    {
        ASSERT_TRUE(isxdigit(s3[i]));
    }
}



int main()
{
    Shakertest t;
	
    return t.run();
}
