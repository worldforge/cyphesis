// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef OP_SWITCH_H
#define OP_SWITCH_H

#define OP_SWITCH(_op, _op_no, _prefix) \
    switch (_op_no) { \
        case OP_LOGIN: \
            return _prefix ## Operation((const Login &)_op); \
            break; \
        case OP_ACTION: \
            return _prefix ## Operation((const Action &)_op); \
            break; \
        case OP_CHOP: \
            return _prefix ## Operation((const Chop &)_op); \
            break; \
        case OP_COMBINE: \
            return _prefix ## Operation((const Combine &)_op); \
            break; \
        case OP_CREATE: \
            return _prefix ## Operation((const Create &)_op); \
            break; \
        case OP_CUT: \
            return _prefix ## Operation((const Cut &)_op); \
            break; \
        case OP_DELETE: \
            return _prefix ## Operation((const Delete &)_op); \
            break; \
        case OP_DIVIDE: \
            return _prefix ## Operation((const Divide &)_op); \
            break; \
        case OP_EAT: \
            return _prefix ## Operation((const Eat &)_op); \
            break; \
        case OP_FIRE: \
            return _prefix ## Operation((const Fire &)_op); \
            break; \
        case OP_INFO: \
            return _prefix ## Operation((const Info &)_op); \
            break; \
        case OP_MOVE: \
            return _prefix ## Operation((const Move &)_op); \
            break; \
        case OP_NOURISH: \
            return _prefix ## Operation((const Nourish &)_op); \
            break; \
        case OP_SET: \
            return _prefix ## Operation((const Set &)_op); \
            break; \
        case OP_GET: \
            return _prefix ## Operation((const Get &)_op); \
            break; \
        case OP_SIGHT: \
            return _prefix ## Operation((const Sight &)_op); \
            break; \
        case OP_SOUND: \
            return _prefix ## Operation((const Sound &)_op); \
            break; \
        case OP_TALK: \
            return _prefix ## Operation((const Talk &)_op); \
            break; \
        case OP_TOUCH: \
            return _prefix ## Operation((const Touch &)_op); \
            break; \
        case OP_TICK: \
            return _prefix ## Operation((const Tick &)_op); \
            break; \
        case OP_LOOK: \
            return _prefix ## Operation((const Look &)_op); \
            break; \
        case OP_LOAD: \
            return _prefix ## Operation((const Load &)_op); \
            break; \
        case OP_SAVE: \
            return _prefix ## Operation((const Save &)_op); \
            break; \
        case OP_SETUP: \
            return _prefix ## Operation((const Setup &)_op); \
            break; \
        case OP_APPEARANCE: \
            return _prefix ## Operation((const Appearance &)_op); \
            break; \
        case OP_DISAPPEARANCE: \
            return _prefix ## Operation((const Disappearance &)_op); \
            break; \
        case OP_ERROR: \
            return _prefix ## Operation((const Error &)_op); \
            break; \
        default: \
            cout << "nothing doing here {" << _op.GetParents().front().AsString() << "}" << endl; \
            return _prefix ## Operation(_op); \
            break; \
    }

#define SUB_OP_SWITCH(_op, _sub_op_no, _prefix, _sub_op) \
    switch (_sub_op_no) { \
        case OP_LOGIN: \
            return _prefix ## Operation(_op, (Login &)_sub_op); \
            break; \
        case OP_ACTION: \
            return _prefix ## Operation(_op, (Action &)_sub_op); \
            break; \
        case OP_CHOP: \
            return _prefix ## Operation(_op, (Chop &)_sub_op); \
            break; \
        case OP_COMBINE: \
            return _prefix ## Operation(_op, (Combine &)_sub_op); \
            break; \
        case OP_CREATE: \
            return _prefix ## Operation(_op, (Create &)_sub_op); \
            break; \
        case OP_CUT: \
            return _prefix ## Operation(_op, (Cut &)_sub_op); \
            break; \
        case OP_DELETE: \
            return _prefix ## Operation(_op, (Delete &)_sub_op); \
            break; \
        case OP_DIVIDE: \
            return _prefix ## Operation(_op, (Divide &)_sub_op); \
            break; \
        case OP_EAT: \
            return _prefix ## Operation(_op, (Eat &)_sub_op); \
            break; \
        case OP_FIRE: \
            return _prefix ## Operation(_op, (Fire &)_sub_op); \
            break; \
        case OP_MOVE: \
            return _prefix ## Operation(_op, (Move &)_sub_op); \
            break; \
        case OP_SET: \
            return _prefix ## Operation(_op, (Set &)_sub_op); \
            break; \
        case OP_TALK: \
            return _prefix ## Operation(_op, (Talk &)_sub_op); \
            break; \
        case OP_TOUCH: \
            return _prefix ## Operation(_op, (Touch &)_sub_op); \
            break; \
        default: \
            cout << "nothing doing here {" << _op.GetParents().front().AsString() << "_" << _sub_op.GetParents().front().AsString() << "}" << endl; \
            return _prefix ## Operation(_op, _sub_op); \
            break; \
    }

#endif // OP_SWITCH_H
