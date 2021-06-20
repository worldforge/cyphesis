/*
 Copyright (C) 2018 Erik Ogenvik

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

#include "CyPy_Rules.h"
#include "CyPy_Props.h"
#include "CyPy_Location.h"
#include "CyPy_EntityLocation.h"
#include "CyPy_LocatedEntity.h"
#include "CyPy_RootEntity.h"
#include "rules/PhysicalProperties.h"
#include <wfmath/atlasconv.h>

CyPy_Rules::CyPy_Rules() : ExtensionModule("rules")
{
    CyPy_Props::init_type();
    CyPy_Location::init_type();
    CyPy_EntityLocation::init_type();

    add_varargs_method("isLocation", &CyPy_Rules::is_location, "");
    add_varargs_method("extract_location", &CyPy_Rules::extract_location, "Extracts all location data from the entity into the root entity message.");

    initialize("Rules");

    Py::Dict d(moduleDictionary());

    d["Location"] = CyPy_Location::type();
    d["EntityLocation"] = CyPy_EntityLocation::type();

}

Py::Object CyPy_Rules::is_location(const Py::Tuple& args)
{
    args.verify_length(1, 1);
    return Py::Boolean(CyPy_Location::check(args[0]));
}

Py::Object CyPy_Rules::extract_location(const Py::Tuple& args)
{
    args.verify_length(2);
    auto& entity = verifyObject<CyPy_LocatedEntity>(args[0]);
    auto& rootEntity = verifyObject<CyPy_RootEntity>(args[1]);

    if (entity->m_parent != nullptr) {
        rootEntity->setLoc(entity->m_parent->getId());
    }
    if (auto prop = entity->getPropertyClassFixed<PositionProperty>()) {
        ::addToEntity(prop->data(), rootEntity->modifyPos());
    }
    if (auto prop = entity->getPropertyClassFixed<VelocityProperty>()) {
        ::addToEntity(prop->data(), rootEntity->modifyVelocity());
    }
    if (auto prop = entity->getPropertyClassFixed<OrientationProperty>()) {
        rootEntity->setAttr("orientation", prop->data().toAtlas());
    }
    if (auto prop = entity->getPropertyClassFixed<AngularVelocityProperty>()) {
        rootEntity->setAttr("angular", prop->data().toAtlas());
    }

    return args[1];
}

std::string CyPy_Rules::init()
{
    PyImport_AppendInittab("rules", []() {
        static CyPy_Rules module;
        return module.module().ptr();
    });
    return "rules";
}
