// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef COMMON_OP_SWITCH_H
#define COMMON_OP_SWITCH_H

#define OP_SWITCH(_op, _op_no, _prefix) \
    switch (_op_no) { \
        case OP_LOGIN: \
            return _prefix ## LoginOperation((const Login &)_op); \
            break; \
        case OP_LOGOUT: \
            return _prefix ## LogoutOperation((const Logout &)_op); \
            break; \
        case OP_ACTION: \
            return _prefix ## ActionOperation((const Action &)_op); \
            break; \
        case OP_CHOP: \
            return _prefix ## ChopOperation((const Chop &)_op); \
            break; \
        case OP_COMBINE: \
            return _prefix ## CombineOperation((const Combine &)_op); \
            break; \
        case OP_CREATE: \
            return _prefix ## CreateOperation((const Create &)_op); \
            break; \
        case OP_CUT: \
            return _prefix ## CutOperation((const Cut &)_op); \
            break; \
        case OP_DELETE: \
            return _prefix ## DeleteOperation((const Delete &)_op); \
            break; \
        case OP_DIVIDE: \
            return _prefix ## DivideOperation((const Divide &)_op); \
            break; \
        case OP_EAT: \
            return _prefix ## EatOperation((const Eat &)_op); \
            break; \
        case OP_BURN: \
            return _prefix ## BurnOperation((const Burn &)_op); \
            break; \
        case OP_IMAGINARY: \
            return _prefix ## ImaginaryOperation((const Imaginary &)_op); \
            break; \
        case OP_INFO: \
            return _prefix ## InfoOperation((const Info &)_op); \
            break; \
        case OP_MOVE: \
            return _prefix ## MoveOperation((const Move &)_op); \
            break; \
        case OP_NOURISH: \
            return _prefix ## NourishOperation((const Nourish &)_op); \
            break; \
        case OP_SET: \
            return _prefix ## SetOperation((const Set &)_op); \
            break; \
        case OP_GET: \
            return _prefix ## GetOperation((const Get &)_op); \
            break; \
        case OP_SIGHT: \
            return _prefix ## SightOperation((const Sight &)_op); \
            break; \
        case OP_SOUND: \
            return _prefix ## SoundOperation((const Sound &)_op); \
            break; \
        case OP_TALK: \
            return _prefix ## TalkOperation((const Talk &)_op); \
            break; \
        case OP_TOUCH: \
            return _prefix ## TouchOperation((const Touch &)_op); \
            break; \
        case OP_TICK: \
            return _prefix ## TickOperation((const Tick &)_op); \
            break; \
        case OP_LOOK: \
            return _prefix ## LookOperation((const Look &)_op); \
            break; \
        case OP_SETUP: \
            return _prefix ## SetupOperation((const Setup &)_op); \
            break; \
        case OP_APPEARANCE: \
            return _prefix ## AppearanceOperation((const Appearance &)_op); \
            break; \
        case OP_DISAPPEARANCE: \
            return _prefix ## DisappearanceOperation((const Disappearance &)_op); \
            break; \
        case OP_USE: \
            return _prefix ## UseOperation((const Use &)_op); \
            break; \
        case OP_WIELD: \
            return _prefix ## WieldOperation((const Wield &)_op); \
            break; \
        case OP_ERROR: \
            return _prefix ## ErrorOperation((const Error &)_op); \
            break; \
        case OP_INVALID: \
            break; \
        default: \
            return _prefix ## OtherOperation((const RootOperation &)_op); \
            break; \
    }

#define SUB_OP_SWITCH(_op, _sub_op_no, _prefix, _sub_op) \
    switch (_sub_op_no) { \
        case OP_LOGIN: \
            return _prefix ## LoginOperation(_op, (Login &)_sub_op); \
            break; \
        case OP_ACTION: \
            return _prefix ## ActionOperation(_op, (Action &)_sub_op); \
            break; \
        case OP_CHOP: \
            return _prefix ## ChopOperation(_op, (Chop &)_sub_op); \
            break; \
        case OP_COMBINE: \
            return _prefix ## CombineOperation(_op, (Combine &)_sub_op); \
            break; \
        case OP_CREATE: \
            return _prefix ## CreateOperation(_op, (Create &)_sub_op); \
            break; \
        case OP_CUT: \
            return _prefix ## CutOperation(_op, (Cut &)_sub_op); \
            break; \
        case OP_DELETE: \
            return _prefix ## DeleteOperation(_op, (Delete &)_sub_op); \
            break; \
        case OP_DIVIDE: \
            return _prefix ## DivideOperation(_op, (Divide &)_sub_op); \
            break; \
        case OP_EAT: \
            return _prefix ## EatOperation(_op, (Eat &)_sub_op); \
            break; \
        case OP_BURN: \
            return _prefix ## BurnOperation(_op, (Burn &)_sub_op); \
            break; \
        case OP_IMAGINARY: \
            return _prefix ## ImaginaryOperation(_op, (Imaginary &)_sub_op); \
            break; \
        case OP_MOVE: \
            return _prefix ## MoveOperation(_op, (Move &)_sub_op); \
            break; \
        case OP_SET: \
            return _prefix ## SetOperation(_op, (Set &)_sub_op); \
            break; \
        case OP_TALK: \
            return _prefix ## TalkOperation(_op, (Talk &)_sub_op); \
            break; \
        case OP_TOUCH: \
            return _prefix ## TouchOperation(_op, (Touch &)_sub_op); \
            break; \
        case OP_INVALID: \
            break; \
        default: \
            return _prefix ## OtherOperation(_op, _sub_op); \
            break; \
    }

#endif // COMMON_OP_SWITCH_H
