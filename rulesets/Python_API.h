// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef RULESETS_PYTHON_API_H
#define RULESETS_PYTHON_API_H

#include <string>

class Entity;
class BaseMind;

void Create_PyThing(Entity * thing, const std::string & package,
                                    const std::string & type);
void Create_PyMind(BaseMind * mind, const std::string & package,
                                    const std::string & type);

void init_python_api();
void shutdown_python_api();

#endif // RULESETS_PYTHON_API_H
