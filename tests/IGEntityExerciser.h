// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2003 Alistair Riddoch
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


#ifndef TESTS_IG_ENTITY_EXERCISER_H
#define TESTS_IG_ENTITY_EXERCISER_H

#include "common/EntityExerciser.h"

#include "TestPropertyManager.h"
#include "TestWorld.h"

#include "rules/simulation/Entity.h"

#include <Atlas/Message/Element.h>

class IGEntityExerciser : public EntityExerciser {
  protected:
        TestPropertyManager m_propertyManager;
        std::unique_ptr<TestWorld> m_testWorld;
    Ref<Entity> m_ent;
  public:
    explicit IGEntityExerciser(const Ref<Entity>& e);
    ~IGEntityExerciser() override;

    bool checkProperties(const std::set<std::string> & prop_names) override;

    void runOperations() override;
};

#endif // TESTS_IG_ENTITY_EXERCISER_H
