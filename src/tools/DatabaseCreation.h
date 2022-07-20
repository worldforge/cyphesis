//
// Created by erik on 2022-07-20.
//

#ifndef CYPHESIS_DATABASECREATION_H
#define CYPHESIS_DATABASECREATION_H

#include <memory>
#include "common/Database.h"

std::unique_ptr<Database> createDatabase();

#endif //CYPHESIS_DATABASECREATION_H
