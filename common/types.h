// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2004 Alistair Riddoch

#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

#include "operations.h"

#include <list>
#include <set>
#include <map>
#include <vector>

class BaseEntity;
class Entity;
class Account;

typedef enum op_no {
	OP_LOGIN,
	OP_LOGOUT,
	OP_ACTION,
	OP_CHOP,
	OP_COMBINE,
	OP_CREATE,
	OP_CUT,
	OP_DELETE,
	OP_DIVIDE,
	OP_EAT,
	OP_ERROR,
	OP_BURN,
	OP_IMAGINARY,
	OP_INFO,
	OP_MONITOR,
	OP_MOVE,
	OP_NOURISH,
	OP_SET,
	OP_GET,
	OP_SIGHT,
	OP_SOUND,
	OP_TOUCH,
	OP_TALK,
	OP_TICK,
	OP_LOOK,
	OP_SETUP,
	OP_APPEARANCE,
	OP_DISAPPEARANCE,
	OP_USE,
	OP_WIELD,
	OP_OTHER,
	OP_INVALID
} OpNo;

typedef std::map<std::string, BaseEntity *> BaseDict;
typedef std::list<BaseEntity *> BaseList;
typedef std::pair<std::string, BaseEntity *> BasePair;
typedef std::vector<std::string> IdList;

typedef std::map<std::string, Entity *> EntityDict;
typedef std::list<Entity *> EntityList;
typedef std::vector<Entity *> EntityVector;
typedef std::set<Entity *> EntitySet;

typedef std::map<std::string, Account *> AccountDict;
typedef std::list<Account *> AccountList;

typedef std::vector<Atlas::Objects::Operation::RootOperation *> OpVector;
typedef std::list<Atlas::Objects::Operation::RootOperation *> OpQueue;
typedef std::map<std::string,OpNo> OpNoDict;

#endif // TYPES_H
