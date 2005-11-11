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

#define OP_ACTION Atlas::Objects::Operation::ACTION_NO
#define OP_ADD Atlas::Objects::Operation::ADD_NO
#define OP_APPEARANCE Atlas::Objects::Operation::APPEARANCE_NO
#define OP_ATTACK Atlas::Objects::Operation::ATTACK_NO
#define OP_BURN Atlas::Objects::Operation::BURN_NO
#define OP_CHOP Atlas::Objects::Operation::CHOP_NO
#define OP_COMBINE Atlas::Objects::Operation::COMBINE_NO
#define OP_CONNECT Atlas::Objects::Operation::CONNECT_NO
#define OP_CREATE Atlas::Objects::Operation::CREATE_NO
#define OP_CUT Atlas::Objects::Operation::CUT_NO
#define OP_DELETE Atlas::Objects::Operation::DELETE_NO
#define OP_DISAPPEARANCE Atlas::Objects::Operation::DISAPPEARANCE_NO
#define OP_DIVIDE Atlas::Objects::Operation::DIVIDE_NO
#define OP_EAT Atlas::Objects::Operation::EAT_NO
#define OP_ERROR Atlas::Objects::Operation::ERROR_NO
#define OP_GET Atlas::Objects::Operation::GET_NO
#define OP_IMAGINARY Atlas::Objects::Operation::IMAGINARY_NO
#define OP_INFO Atlas::Objects::Operation::INFO_NO
#define OP_LOGIN Atlas::Objects::Operation::LOGIN_NO
#define OP_LOGOUT Atlas::Objects::Operation::LOGOUT_NO
#define OP_LOOK Atlas::Objects::Operation::LOOK_NO
#define OP_MONITOR Atlas::Objects::Operation::MONITOR_NO
#define OP_MOVE Atlas::Objects::Operation::MOVE_NO
#define OP_NOURISH Atlas::Objects::Operation::NOURISH_NO
#define OP_SET Atlas::Objects::Operation::SET_NO
#define OP_SETUP Atlas::Objects::Operation::SETUP_NO
#define OP_SIGHT Atlas::Objects::Operation::SIGHT_NO
#define OP_SOUND Atlas::Objects::Operation::SOUND_NO
#define OP_TALK Atlas::Objects::Operation::TALK_NO
#define OP_TICK Atlas::Objects::Operation::TICK_NO
#define OP_TOUCH Atlas::Objects::Operation::TOUCH_NO
#define OP_UNSEEN Atlas::Objects::Operation::UNSEEN_NO
#define OP_UPDATE Atlas::Objects::Operation::UPDATE_NO
#define OP_USE Atlas::Objects::Operation::USE_NO
#define OP_WIELD Atlas::Objects::Operation::WIELD_NO
#define OP_DELVE Atlas::Objects::Operation::DELVE_NO
#define OP_DIG Atlas::Objects::Operation::DIG_NO
#define OP_MOW Atlas::Objects::Operation::MOW_NO
// #define OP_OTHER Atlas::Objects::Operation::OTHER_NO
#define OP_INVALID (-1)

typedef int OpNo;

typedef Atlas::Objects::Operation::RootOperation Operation;

typedef std::map<std::string, BaseEntity *> BaseDict;
typedef std::vector<std::string> IdList;

typedef std::map<long, Entity *> EntityDict;
typedef std::set<Entity *> EntitySet;

typedef std::vector<Operation> OpVector;
typedef std::map<std::string,OpNo> OpNoDict;

#endif // TYPES_H
