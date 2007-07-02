// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2004 Alistair Riddoch
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

// $Id: op_switch.h,v 1.35 2007-07-02 21:11:08 alriddoch Exp $

#ifndef COMMON_OP_SWITCH_H
#define COMMON_OP_SWITCH_H

#define OP_SWITCH(_op, _op_no, _result, _prefix) \
    switch (_op_no) { \
        case OP_APPEARANCE: \
            _prefix ## AppearanceOperation(_op, _result); \
            break; \
        case OP_COMBINE: \
            _prefix ## CombineOperation(_op, _result); \
            break; \
        case OP_CREATE: \
            _prefix ## CreateOperation(_op, _result); \
            break; \
        case OP_DELETE: \
            _prefix ## DeleteOperation(_op, _result); \
            break; \
        case OP_DISAPPEARANCE: \
            _prefix ## DisappearanceOperation(_op, _result); \
            break; \
        case OP_DIVIDE: \
            _prefix ## DivideOperation(_op, _result); \
            break; \
        case OP_ERROR: \
            _prefix ## ErrorOperation(_op, _result); \
            break; \
        case OP_GET: \
            _prefix ## GetOperation(_op, _result); \
            break; \
        case OP_IMAGINARY: \
            _prefix ## ImaginaryOperation(_op, _result); \
            break; \
        case OP_INFO: \
            _prefix ## InfoOperation(_op, _result); \
            break; \
        case OP_LOGIN: \
            _prefix ## LoginOperation(_op, _result); \
            break; \
        case OP_LOGOUT: \
            _prefix ## LogoutOperation(_op, _result); \
            break; \
        case OP_LOOK: \
            _prefix ## LookOperation(_op, _result); \
            break; \
        case OP_MOVE: \
            _prefix ## MoveOperation(_op, _result); \
            break; \
        case OP_SET: \
            _prefix ## SetOperation(_op, _result); \
            break; \
        case OP_SIGHT: \
            _prefix ## SightOperation(_op, _result); \
            break; \
        case OP_SOUND: \
            _prefix ## SoundOperation(_op, _result); \
            break; \
        case OP_TALK: \
            _prefix ## TalkOperation(_op, _result); \
            break; \
        case OP_TOUCH: \
            _prefix ## TouchOperation(_op, _result); \
            break; \
        case OP_USE: \
            _prefix ## UseOperation(_op, _result); \
            break; \
        case OP_WIELD: \
            _prefix ## WieldOperation(_op, _result); \
            break; \
        case OP_INVALID: \
            break; \
        default: \
            if (_op_no == Atlas::Objects::Operation::ADD_NO) { \
                _prefix ## AddOperation(_op, _result); \
            } else if (_op_no == Atlas::Objects::Operation::ATTACK_NO) { \
                _prefix ## AttackOperation(_op, _result); \
            } else if (_op_no == Atlas::Objects::Operation::EAT_NO) { \
                _prefix ## EatOperation(_op, _result); \
            } else if (_op_no == Atlas::Objects::Operation::NOURISH_NO) { \
                _prefix ## NourishOperation(_op, _result); \
            } else if (_op_no == Atlas::Objects::Operation::SETUP_NO) { \
                _prefix ## SetupOperation(_op, _result); \
            } else if (_op_no == Atlas::Objects::Operation::TICK_NO) { \
                _prefix ## TickOperation(_op, _result); \
            } else if (_op_no == Atlas::Objects::Operation::UPDATE_NO) { \
                _prefix ## UpdateOperation(_op, _result); \
            } else { \
                _prefix ## OtherOperation(_op, _result); \
            } \
            break; \
    }

#define POLL_OP_SWITCH(_op, _op_no, _prefix) \
    switch (_op_no) { \
        case OP_APPEARANCE: \
            return _prefix ## AppearanceOperation(_op); \
            break; \
        case OP_DISAPPEARANCE: \
            return _prefix ## DisappearanceOperation(_op); \
            break; \
        case OP_SIGHT: \
            return _prefix ## SightOperation(_op); \
            break; \
        case OP_SOUND: \
            return _prefix ## SoundOperation(_op); \
            break; \
        case OP_TOUCH: \
            return _prefix ## TouchOperation(_op); \
            break; \
        case OP_ERROR: \
            return _prefix ## ErrorOperation(_op); \
            break; \
        case OP_INVALID: \
            break; \
        default: \
            if (_op_no == Atlas::Objects::Operation::SETUP_NO) { \
                return _prefix ## SetupOperation(_op); \
            } else if (_op_no == Atlas::Objects::Operation::TICK_NO) { \
                return _prefix ## TickOperation(_op); \
            } else if (_op_no == Atlas::Objects::Operation::UNSEEN_NO) { \
                return _prefix ## UnseenOperation(_op); \
            } \
            break; \
    }


#define SUB_OP_SWITCH(_op, _sub_op_no, _result, _prefix, _sub_op) \
    switch (_sub_op_no) { \
        case OP_CREATE: \
            _prefix ## CreateOperation(_op, _sub_op, _result); \
            break; \
        case OP_DELETE: \
            _prefix ## DeleteOperation(_op, _sub_op, _result); \
            break; \
        case OP_MOVE: \
            _prefix ## MoveOperation(_op, _sub_op, _result); \
            break; \
        case OP_SET: \
            _prefix ## SetOperation(_op, _sub_op, _result); \
            break; \
        case OP_INVALID: \
            break; \
        default: \
            break; \
    }

#endif // COMMON_OP_SWITCH_H
