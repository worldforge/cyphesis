// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef BASE_ENTITY_H
#define BASE_ENTITY_H

#include <Atlas/Objects/Operation/Error.h>

#include <common/types.h>
#include <common/operations.h>

class WorldRouter;

class BaseEntity {
  public:
    int seq;			// Sequence number
    string fullid;		// String id
    bool inGame;		// true if in game object

    BaseEntity();
    virtual ~BaseEntity();

    virtual void destroy();

    Atlas::Message::Object asObject() const;
    virtual void addToObject(Atlas::Message::Object &) const;

    virtual oplist message(const RootOperation & op);
    virtual oplist operation(const RootOperation & op);
    virtual oplist externalOperation(const RootOperation & op);
    virtual oplist externalMessage(const RootOperation & op);

    virtual oplist Operation(const Login & op);
    virtual oplist Operation(const Logout & op);
    virtual oplist Operation(const Action & op);
    virtual oplist Operation(const Chop & op);
    virtual oplist Operation(const Combine & op);
    virtual oplist Operation(const Create & op);
    virtual oplist Operation(const Cut & op);
    virtual oplist Operation(const Delete & op);
    virtual oplist Operation(const Divide & op);
    virtual oplist Operation(const Eat & op);
    virtual oplist Operation(const Fire & op);
    virtual oplist Operation(const Get & op);
    virtual oplist Operation(const Info & op);
    virtual oplist Operation(const Move & op);
    virtual oplist Operation(const Nourish & op);
    virtual oplist Operation(const Set & op);
    virtual oplist Operation(const Sight & op);
    virtual oplist Operation(const Sound & op);
    virtual oplist Operation(const Talk & op);
    virtual oplist Operation(const Touch & op);
    virtual oplist Operation(const Tick & op);
    virtual oplist Operation(const Look & op);
    virtual oplist Operation(const Load & op);
    virtual oplist Operation(const Save & op);
    virtual oplist Operation(const Setup & op);
    virtual oplist Operation(const Appearance & op);
    virtual oplist Operation(const Disappearance & op);
    virtual oplist Operation(const RootOperation & op);

    void setRefno(const oplist & ret, const RootOperation & ref_op) const;
    op_no_t opEnumerate(const RootOperation & op) const;
    oplist callOperation(const RootOperation & op);
    oplist error(const RootOperation & op, const char * errstring) const;

    void setRefnoOp(RootOperation * op, const RootOperation & ref_op) const {
        op->SetRefno(ref_op.GetSerialno());
    }

};

#endif // BASE_ENTITY_H
