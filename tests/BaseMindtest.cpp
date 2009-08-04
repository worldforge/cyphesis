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

// $Id$

#include "rulesets/BaseMind.h"

#include "common/Unseen.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/SmartPtr.h>

#include <cassert>

int main()
{
    BaseMind * bm = new BaseMind("1", 1);

    (void)bm->getMap();
    (void)bm->getTime();

    bm->sleep();
    bm->awake();

    {
        OpVector res;
        Atlas::Objects::Operation::Get g;
        bm->operation(g, res);
    }

    {
        OpVector res;
        Atlas::Objects::Operation::Sight op;
        bm->operation(op, res);

        op->setArgs1(Atlas::Objects::Entity::Anonymous());
        bm->operation(op, res);

        op->setArgs1(Atlas::Objects::Entity::RootEntity(0));
        bm->operation(op, res);

        op->setArgs1(Atlas::Objects::Operation::Get());
        bm->operation(op, res);

        {
            Atlas::Objects::Operation::Create sub_op;
            op->setArgs1(sub_op);
            bm->operation(op, res);

            sub_op->setArgs1(Atlas::Objects::Entity::Anonymous());
            bm->operation(op, res);

            sub_op->setArgs1(Atlas::Objects::Entity::Anonymous(0));
            bm->operation(op, res);
        }

        {
            Atlas::Objects::Operation::Delete sub_op;
            op->setArgs1(sub_op);
            bm->operation(op, res);

            sub_op->setArgs1(Atlas::Objects::Entity::Anonymous());
            bm->operation(op, res);

            sub_op->setArgs1(Atlas::Objects::Entity::Anonymous(0));
            bm->operation(op, res);

            Atlas::Objects::Entity::Anonymous arg;
            arg->setId("2");
            sub_op->setArgs1(arg);
            bm->operation(op, res);
        }

        {
            Atlas::Objects::Operation::Move sub_op;
            op->setArgs1(sub_op);
            bm->operation(op, res);

            sub_op->setArgs1(Atlas::Objects::Entity::Anonymous());
            bm->operation(op, res);

            sub_op->setArgs1(Atlas::Objects::Entity::Anonymous(0));
            bm->operation(op, res);

            Atlas::Objects::Entity::Anonymous arg;
            arg->setId("2");
            sub_op->setArgs1(arg);
            bm->operation(op, res);
        }

        {
            Atlas::Objects::Operation::Set sub_op;
            op->setArgs1(sub_op);
            bm->operation(op, res);

            sub_op->setArgs1(Atlas::Objects::Entity::Anonymous());
            bm->operation(op, res);

            sub_op->setArgs1(Atlas::Objects::Entity::Anonymous(0));
            bm->operation(op, res);

            Atlas::Objects::Entity::Anonymous arg;
            arg->setId("2");
            sub_op->setArgs1(arg);
            bm->operation(op, res);
        }
    }

    {
        OpVector res;
        Atlas::Objects::Operation::Sound op;
        bm->operation(op, res);

        op->setArgs1(Atlas::Objects::Operation::Get());
        bm->operation(op, res);
    }

    {
        OpVector res;
        Atlas::Objects::Operation::Appearance op;
        bm->operation(op, res);

        Atlas::Objects::Entity::Anonymous arg;
        op->setArgs1(arg);
        bm->operation(op, res);

        arg->setId("2");
        bm->operation(op, res);

        arg->setStamp(0);
        bm->operation(op, res);

        arg->setStamp(23);
        bm->operation(op, res);
    }

    {
        OpVector res;
        Atlas::Objects::Operation::Disappearance op;
        bm->operation(op, res);

        Atlas::Objects::Entity::Anonymous arg;
        op->setArgs1(arg);
        bm->operation(op, res);

        arg->setId("2");
        bm->operation(op, res);
    }

    {
        OpVector res;
        Atlas::Objects::Operation::Unseen op;
        bm->operation(op, res);

        Atlas::Objects::Entity::Anonymous arg;
        op->setArgs1(arg);
        bm->operation(op, res);

        arg->setId("2");
        bm->operation(op, res);
    }

    delete bm;
    // The is no code in operations.cpp to execute, but we need coverage.
    return 0;
}
