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


#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "../../TestBase.h"
#include "../../TestEntity.h"

#include "rules/simulation/ContainedVisibilityProperty.h"

#include "common/TypeNode.h"

int main()
{

    struct Test : public Cyphesis::TestBase
    {
        Ref<TestEntity> m_char1;

        Test()
        {
            ADD_TEST(Test::test_copy)
            ADD_TEST(Test::test_apply)
        }

        void setup()
        {
            m_char1 = new TestEntity(1);
        }

        void teardown()
        {
            m_char1.reset();
        }


        void test_copy()
        {
            auto prop = std::make_unique<ContainedVisibilityProperty>();

            auto copy = prop->copy();
            ASSERT_NOT_NULL(copy);
            ASSERT_NOT_NULL(dynamic_cast<ContainedVisibilityProperty*>(copy));
            delete copy;
        }

        void test_apply()
        {
            auto prop = std::make_unique<ContainedVisibilityProperty>();
            prop->set(1);
            auto setProp = m_char1->setProperty(ContainedVisibilityProperty::property_name, std::move(prop));
            m_char1->applyProperty(ContainedVisibilityProperty::property_name, *setProp);

            ASSERT_TRUE(m_char1->hasFlags(entity_contained_visible));
        }

    } t;

    return t.run();
}

// stubs

#include "../../stubs/common/stubProperty.h"

#include "rules/AtlasProperties.h"
#include "rules/Domain.h"
#include "rules/Script.h"

#include "common/log.h"
#include "common/PropertyManager.h"

#include "rules/simulation/DomainProperty.h"
#include "../../stubs/common/stubcustom.h"
#include "../../stubs/rules/stubDomain.h"
#include "../../stubs/rules/simulation/stubDomainProperty.h"
#include "../../stubs/common/stubVariable.h"
#include "../../stubs/common/stubMonitors.h"
#include "../../stubs/common/stubTypeNode.h"


void addToEntity(const Point3D& p, std::vector<double>& vd)
{
    vd.resize(3);
    vd[0] = p[0];
    vd[1] = p[1];
    vd[2] = p[2];
}

#include "../../stubs/rules/simulation/stubBaseWorld.h"

#include "../../stubs/rules/stubScript.h"
#include "../../stubs/rules/stubLocation.h"
#include "../../stubs/rules/stubAtlasProperties.h"
#include "../../stubs/common/stubPropertyManager.h"
#include "../../stubs/common/stubLink.h"
#include "../../stubs/common/stubRouter.h"
#include "../../stubs/common/stubid.h"
#include "../../stubs/common/stublog.h"
#include "../../stubs/rules/stubModifier.h"
#include "../../stubs/rules/stubPhysicalProperties.h"
