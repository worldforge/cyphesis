// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef BASE_MIND_H
#define BASE_MIND_H

#include "Entity.h"
#include "MemMap.h"

class BaseMind : public Entity {
  protected:
    MemMap map;
  public:
    BaseMind(const string &, const string &);
    virtual ~BaseMind();

    virtual MemMap * getMap();

    virtual oplist sightOperation(const Sight & op, Login & sub_op);
    virtual oplist sightOperation(const Sight & op, Chop & sub_op);
    virtual oplist sightOperation(const Sight & op, Create & sub_op);
    virtual oplist sightOperation(const Sight & op, Cut & sub_op);
    virtual oplist sightOperation(const Sight & op, Delete & sub_op);
    virtual oplist sightOperation(const Sight & op, Eat & sub_op);
    virtual oplist sightOperation(const Sight & op, Fire & sub_op);
    virtual oplist sightOperation(const Sight & op, Move & sub_op);
    virtual oplist sightOperation(const Sight & op, Set & sub_op);
    virtual oplist sightOperation(const Sight & op, Touch & sub_op);
    virtual oplist sightOperation(const Sight & op, RootOperation & sub_op);

    virtual oplist soundOperation(const Sound & op, Talk & sub_op);
    virtual oplist soundOperation(const Sound & op, RootOperation & sub_op);

    virtual oplist Operation(const Sight & op);
    virtual oplist Operation(const Sound & op);
    virtual oplist Operation(const Appearance & op);
    virtual oplist Operation(const Disappearance & op);
    //RootOperation * get_op_name_and_sub(RootOperation & op, string & name);
    //virtual int call_triggers(RootOperation & op);
    //virtual oplist message(const RootOperation & op);
    virtual oplist operation(const RootOperation & op);

    oplist callSightOperation(const Sight & op, RootOperation & sub_op);
    oplist callSoundOperation(const Sound & op, RootOperation & sub_op);

    friend class PythonMindScript;
};

#endif // BASE_MIND_H
