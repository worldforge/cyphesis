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


#ifndef COMMON_OP_SWITCH_H
#define COMMON_OP_SWITCH_H

#define OP_SWITCH(_op, _op_no, _result, _prefix) \
    switch (_op_no) { \
        case Atlas::Objects::Operation::DELETE_NO: \
            _prefix ## DeleteOperation(_op, _result); \
            break; \
        case Atlas::Objects::Operation::IMAGINARY_NO: \
            _prefix ## ImaginaryOperation(_op, _result); \
            break; \
        case Atlas::Objects::Operation::LOOK_NO: \
            _prefix ## LookOperation(_op, _result); \
            break; \
        case Atlas::Objects::Operation::MOVE_NO: \
            _prefix ## MoveOperation(_op, _result); \
            break; \
        case Atlas::Objects::Operation::SET_NO: \
            _prefix ## SetOperation(_op, _result); \
            break; \
        case Atlas::Objects::Operation::TALK_NO: \
            _prefix ## TalkOperation(_op, _result); \
            break; \
        case OP_INVALID: \
            break; \
        default: \
            if ((_op_no) == Atlas::Objects::Operation::UPDATE_NO) { \
                _prefix ## UpdateOperation(_op, _result); \
            } else if ((_op_no) == Atlas::Objects::Operation::RELAY_NO) { \
                _prefix ## RelayOperation(_op, _result); \
            } else { \
                /* ERROR */ \
            } \
            break; \
    }

#define POLL_OP_SWITCH(_op, _op_no, _prefix) \
    switch (_op_no) { \
        case Atlas::Objects::Operation::APPEARANCE_NO: \
            return _prefix ## AppearanceOperation(_op); \
            break; \
        case Atlas::Objects::Operation::DISAPPEARANCE_NO: \
            return _prefix ## DisappearanceOperation(_op); \
            break; \
        case Atlas::Objects::Operation::SIGHT_NO: \
            return _prefix ## SightOperation(_op); \
            break; \
        case Atlas::Objects::Operation::SOUND_NO: \
            return _prefix ## SoundOperation(_op); \
            break; \
        case Atlas::Objects::Operation::TOUCH_NO: \
            return _prefix ## TouchOperation(_op); \
            break; \
        case Atlas::Objects::Operation::ERROR_NO: \
            return _prefix ## ErrorOperation(_op); \
            break; \
        case OP_INVALID: \
            break; \
        default: \
            if ((_op_no) == Atlas::Objects::Operation::SETUP_NO) { \
                return _prefix ## SetupOperation(_op); \
            } else if ((_op_no) == Atlas::Objects::Operation::TICK_NO) { \
                return _prefix ## TickOperation(_op); \
            } else if ((_op_no) == Atlas::Objects::Operation::UNSEEN_NO) { \
                return _prefix ## UnseenOperation(_op); \
            } else if ((_op_no) == Atlas::Objects::Operation::THOUGHT_NO) { \
                return _prefix ## ThoughtOperation(_op); \
            } else if ((_op_no) == Atlas::Objects::Operation::THINK_NO) { \
                return _prefix ## ThinkOperation(_op); \
            } else if ((_op_no) == Atlas::Objects::Operation::COMMUNE_NO) { \
                return _prefix ## CommuneOperation(_op); \
            } else if ((_op_no) == Atlas::Objects::Operation::RELAY_NO) { \
                return _prefix ## RelayOperation(_op); \
            } \
            break; \
    }


#define SUB_OP_SWITCH(_op, _sub_op_no, _result, _prefix) \
    switch (_sub_op_no) { \
        case Atlas::Objects::Operation::CREATE_NO: \
            _prefix ## CreateOperation(_op, _result); \
            break; \
        case Atlas::Objects::Operation::DELETE_NO: \
            _prefix ## DeleteOperation(_op, _result); \
            break; \
        case Atlas::Objects::Operation::MOVE_NO: \
            _prefix ## MoveOperation(_op, _result); \
            break; \
        case Atlas::Objects::Operation::SET_NO: \
            _prefix ## SetOperation(_op, _result); \
            break; \
        case OP_INVALID: \
            break; \
        default: \
            break; \
    }

#endif // COMMON_OP_SWITCH_H
