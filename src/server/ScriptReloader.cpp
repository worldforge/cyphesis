/*
 Copyright (C) 2020 Erik Ogenvik

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

#include "ScriptReloader.h"
#include "pythonbase/Python_API.h"
#include "rules/python/ScriptsProperty.h"
#include "rules/simulation/BaseWorld.h"
#include "rules/LocatedEntity.h"

ScriptReloader::ScriptReloader(BaseWorld& world)
{
    python_reload_scripts.connect([&]() {

        ScriptsProperty::reloadAllScriptFactories();

        //TODO: Implement scripts on archetypes and reload them here
//        std::map<const TypeNode*, EntityFactoryBase*> collector;
//        for (auto& entry : m_entityFactories) {
//            auto entityFactory = dynamic_cast<EntityFactoryBase*>(entry.second);
//            if (entityFactory && entityFactory->m_scriptFactory) {
//                log(NOTICE, compose("Reloading scripts for %1", entityFactory->m_type->name()));
//                entityFactory->m_scriptFactory->refreshClass();
//                collector.emplace(entityFactory->m_type, entityFactory);
//            }
//        }

        auto& entities = world.getEntities();


        //Reload all scripts on all entities. This might be improved to only reload affected scripts.
        for (auto& entry : entities) {
            auto scriptsProp = entry.second->getPropertyClass<ScriptsProperty>("__scripts");
            if (scriptsProp) {
                scriptsProp->applyScripts(*entry.second);
            }
            auto scriptsInstanceProp = entry.second->getPropertyClass<ScriptsProperty>("__scripts_instance");
            if (scriptsInstanceProp) {
                scriptsInstanceProp->applyScripts(*entry.second);
            }
        }

    });

}
