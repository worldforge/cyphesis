//
// Created by erik on 2018-07-02.
//

#include "ScriptsProperty.h"
#include "PythonScriptFactory.h"
#include "LocatedEntity.h"
#include "rulesets/Script.h"
#include "common/log.h"
#include "common/compose.hpp"

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;

using String::compose;

std::map<std::pair<std::string, std::string>, std::shared_ptr<ScriptKit<LocatedEntity>>> ScriptsProperty::sScriptFactories;

void ScriptsProperty::set(const Atlas::Message::Element &element) {
    Property::set(element);

    m_scripts.clear();
    for (auto &entry : m_data) {
        if (entry.isMap()) {
            std::string script_package;
            std::string script_class;
            if (getScriptDetails(entry.Map(), "<none>", "Entity",
                                              script_package, script_class) == 0) {
                auto key = std::make_pair(script_package, script_class);
                auto I = sScriptFactories.find(key);
                if (I != sScriptFactories.end()) {
                    m_scripts.push_back(I->second);
                } else {
                    auto psf = std::make_shared<PythonScriptFactory<LocatedEntity>>(script_package, script_class);
                    if (psf->setup() == 0) {
                        sScriptFactories.emplace(key, psf);
                        m_scripts.push_back(std::move(psf));
                    } else {
                        log(ERROR, String::compose("Python class \"%1.%2\" failed to load",
                                                   script_package, script_class));
                    }
                }
            }
        }
    }
}

void ScriptsProperty::apply(LocatedEntity *entity) {

    applyScripts(entity);
}

void ScriptsProperty::applyScripts(LocatedEntity *entity) const {

    for (auto script : entity->m_scripts) {
        delete script;
    }
    entity->m_scripts.clear();
    for (auto &scriptFactory : m_scripts) {
        scriptFactory->addScript(entity);
    }
}

void ScriptsProperty::reloadAllScriptFactories() {
    for (auto& entry : sScriptFactories) {
        entry.second->refreshClass();
    }

}


int ScriptsProperty::getScriptDetails(const Atlas::Message::MapType & script,
                                  const std::string & class_name,
                                  const std::string & context,
                                  std::string & script_package,
                                  std::string & script_class)
{
    auto J = script.find("name");
    auto Jend = script.end();

    if (J == Jend || !J->second.isString()) {
        log(ERROR, compose("%1 \"%2\" script has no name.",
                           context, class_name));
        return -1;
    }
    const std::string & script_name = J->second.String();
    J = script.find("language");
    if (J == Jend || !J->second.isString()) {
        log(ERROR, compose("%1 \"%2\" script has no language.",
                           context, class_name));
        return -1;
    }
    const std::string & script_language = J->second.String();
    if (script_language != "python") {
        log(ERROR, compose(R"(%1 "%2" script has unknown language "%3".)",
                           context, class_name, script_language));
        return -1;
    }
    std::string::size_type ptr = script_name.rfind('.');
    if (ptr == std::string::npos) {
        log(ERROR, compose(R"(%1 "%2" python script has bad class name "%3".)",
                           context, class_name, script_name));
        return -1;
    }
    script_package = script_name.substr(0, ptr);
    script_class = script_name.substr(ptr + 1);

    return 0;
}

