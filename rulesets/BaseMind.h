// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef RULESETS_BASE_MIND_H
#define RULESETS_BASE_MIND_H

#include <modules/WorldTime.h>

#include "Entity.h"
#include "MemMap.h"

class BaseMind : public Entity {
  protected:
    MemMap map;
    bool isAwake;
    WorldTime time;
    int world;
  public:
    BaseMind(const std::string &, const std::string &);
    virtual ~BaseMind();

    MemMap * getMap() { return &map; }
    WorldTime * getTime() { return &time; }

    void sleep() { isAwake = false; }
    void awake() { isAwake = true; }

    virtual oplist sightLoginOperation(const Sight & op, Login & sub_op);
    virtual oplist sightActionOperation(const Sight & op, Action & sub_op);
    virtual oplist sightChopOperation(const Sight & op, Chop & sub_op);
    virtual oplist sightCombineOperation(const Sight & op, Combine & sub_op);
    virtual oplist sightCreateOperation(const Sight & op, Create & sub_op);
    virtual oplist sightCutOperation(const Sight & op, Cut & sub_op);
    virtual oplist sightDeleteOperation(const Sight & op, Delete & sub_op);
    virtual oplist sightDivideOperation(const Sight & op, Divide & sub_op);
    virtual oplist sightEatOperation(const Sight & op, Eat & sub_op);
    virtual oplist sightFireOperation(const Sight & op, Fire & sub_op);
    virtual oplist sightImaginaryOperation(const Sight & op, Imaginary& sub_op);
    virtual oplist sightMoveOperation(const Sight & op, Move & sub_op);
    virtual oplist sightSetOperation(const Sight & op, Set & sub_op);
    virtual oplist sightTalkOperation(const Sight & op, Talk & sub_op);
    virtual oplist sightTouchOperation(const Sight & op, Touch & sub_op);
    virtual oplist sightOtherOperation(const Sight & op,RootOperation & sub_op);

    //virtual oplist soundTalkOperation(const Sound & op, Talk & sub_op);
    //virtual oplist soundOtherOperation(const Sound & op,RootOperation & sub_op);
    virtual oplist soundLoginOperation(const Sound & op, Login & sub_op);
    virtual oplist soundActionOperation(const Sound & op, Action & sub_op);
    virtual oplist soundChopOperation(const Sound & op, Chop & sub_op);
    virtual oplist soundCombineOperation(const Sound & op, Combine & sub_op);
    virtual oplist soundCreateOperation(const Sound & op, Create & sub_op);
    virtual oplist soundCutOperation(const Sound & op, Cut & sub_op);
    virtual oplist soundDeleteOperation(const Sound & op, Delete & sub_op);
    virtual oplist soundDivideOperation(const Sound & op, Divide & sub_op);
    virtual oplist soundEatOperation(const Sound & op, Eat & sub_op);
    virtual oplist soundFireOperation(const Sound & op, Fire & sub_op);
    virtual oplist soundImaginaryOperation(const Sound & op, Imaginary& sub_op);
    virtual oplist soundMoveOperation(const Sound & op, Move & sub_op);
    virtual oplist soundSetOperation(const Sound & op, Set & sub_op);
    virtual oplist soundTalkOperation(const Sound & op, Talk & sub_op);
    virtual oplist soundTouchOperation(const Sound & op, Touch & sub_op);
    virtual oplist soundOtherOperation(const Sound & op,RootOperation & sub_op);

    virtual oplist SightOperation(const Sight & op);
    virtual oplist SoundOperation(const Sound & op);
    virtual oplist SaveOperation(const Save & op);
    virtual oplist LoadOperation(const Load & op);
    virtual oplist AppearanceOperation(const Appearance & op);
    virtual oplist DisappearanceOperation(const Disappearance & op);
    //RootOperation * get_op_name_and_sub(RootOperation & op, std::string & name);
    //virtual int call_triggers(RootOperation & op);
    //virtual oplist message(const RootOperation & op);
    virtual oplist operation(const RootOperation & op);

    oplist callSightOperation(const Sight & op, RootOperation & sub_op);
    oplist callSoundOperation(const Sound & op, RootOperation & sub_op);

    friend class PythonMindScript;
};

#endif // RULESETS_BASE_MIND_H
