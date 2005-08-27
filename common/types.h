// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2004 Alistair Riddoch

#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

#include <Atlas/Objects/ObjectsFwd.h>

#include <set>
#include <map>
#include <vector>

class BaseEntity;
class Entity;

typedef enum op_no {
	OP_ACTION,
	OP_ADD,
	OP_APPEARANCE,
	OP_BURN,
	OP_CHOP,
	OP_COMBINE,
	OP_CONNECT,
	OP_CREATE,
	OP_CUT,
	OP_DELETE,
	OP_DISAPPEARANCE,
	OP_DIVIDE,
	OP_EAT,
	OP_ERROR,
	OP_GET,
	OP_IMAGINARY,
	OP_INFO,
	OP_LOGIN,
	OP_LOGOUT,
	OP_LOOK,
	OP_MONITOR,
	OP_MOVE,
	OP_NOURISH,
	OP_SET,
	OP_SETUP,
	OP_SIGHT,
	OP_SOUND,
	OP_TALK,
	OP_TICK,
	OP_TOUCH,
	OP_UNSEEN,
	OP_UPDATE,
	OP_USE,
	OP_WIELD,
	OP_OTHER,
	OP_INVALID
} OpNo;

typedef Atlas::Objects::Operation::RootOperation Operation;

typedef std::map<std::string, BaseEntity *> BaseDict;
typedef std::vector<std::string> IdList;

typedef std::map<std::string, Entity *> EntityDict;
typedef std::set<Entity *> EntitySet;

typedef std::vector<Operation> OpVector;
typedef std::map<std::string,OpNo> OpNoDict;

#endif // TYPES_H
