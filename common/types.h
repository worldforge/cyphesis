// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

#include <list>
#include <vector>

#include <Atlas/Objects/Operation/RootOperation.h>

class BaseEntity;

typedef enum op_no {
	OP_LOGIN,
	OP_LOGOUT,
	OP_CHOP,
	OP_COMBINE,
	OP_CREATE,
	OP_CUT,
	OP_DELETE,
	OP_DIVIDE,
	OP_EAT,
	OP_ERROR,
	OP_FIRE,
	OP_INFO,
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
	OP_LOAD,
	OP_SAVE,
	OP_SETUP,
	OP_APPEARANCE,
	OP_DISAPPEARANCE,
	OP_INVALID
} op_no_t;

typedef std::map<string, BaseEntity *> dict_t;
typedef std::list<BaseEntity *> list_t;
typedef std::pair<string, BaseEntity *> idpair_t;

typedef std::vector<Atlas::Objects::Operation::RootOperation *> oplist;

#endif /* TYPES_H */
