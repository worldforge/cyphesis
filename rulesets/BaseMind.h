// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef RULESETS_BASE_MIND_H
#define RULESETS_BASE_MIND_H

#include "MemEntity.h"
#include "MemMap.h"

#include "modules/WorldTime.h"

class BaseMind : public MemEntity {
  protected:
    MemMap m_map;
    bool m_isAwake;
    WorldTime m_time;
    int m_world;
  protected:
    OpNoDict opSightLookup;
    OpNoDict opSoundLookup;

    void sightSubscribe(const std::string& op, OpNo no) {
        opSightLookup[op] = no;
    }

    void soundSubscribe(const std::string& op, OpNo no) {
        opSoundLookup[op] = no;
    }
  public:
    BaseMind(const std::string &, const std::string &);
    virtual ~BaseMind();

    MemMap * getMap() { return &m_map; }
    WorldTime * getTime() { return &m_time; }

    void sleep() { m_isAwake = false; }
    void awake() { m_isAwake = true; }

    void scriptSubscribe(const std::string & op);

    virtual OpVector sightLoginOperation(const Sight & op, Login & sub_op);
    virtual OpVector sightActionOperation(const Sight & op, Action & sub_op);
    virtual OpVector sightChopOperation(const Sight & op, Chop & sub_op);
    virtual OpVector sightCombineOperation(const Sight & op, Combine & sub_op);
    virtual OpVector sightCreateOperation(const Sight & op, Create & sub_op);
    virtual OpVector sightCutOperation(const Sight & op, Cut & sub_op);
    virtual OpVector sightDeleteOperation(const Sight & op, Delete & sub_op);
    virtual OpVector sightDivideOperation(const Sight & op, Divide & sub_op);
    virtual OpVector sightEatOperation(const Sight & op, Eat & sub_op);
    virtual OpVector sightBurnOperation(const Sight & op, Burn & sub_op);
    virtual OpVector sightImaginaryOperation(const Sight & op, Imaginary& sub_op);
    virtual OpVector sightMoveOperation(const Sight & op, Move & sub_op);
    virtual OpVector sightSetOperation(const Sight & op, Set & sub_op);
    virtual OpVector sightTalkOperation(const Sight & op, Talk & sub_op);
    virtual OpVector sightTouchOperation(const Sight & op, Touch & sub_op);
    virtual OpVector sightOtherOperation(const Sight & op,RootOperation & sub_op);

    virtual OpVector soundLoginOperation(const Sound & op, Login & sub_op);
    virtual OpVector soundActionOperation(const Sound & op, Action & sub_op);
    virtual OpVector soundChopOperation(const Sound & op, Chop & sub_op);
    virtual OpVector soundCombineOperation(const Sound & op, Combine & sub_op);
    virtual OpVector soundCreateOperation(const Sound & op, Create & sub_op);
    virtual OpVector soundCutOperation(const Sound & op, Cut & sub_op);
    virtual OpVector soundDeleteOperation(const Sound & op, Delete & sub_op);
    virtual OpVector soundDivideOperation(const Sound & op, Divide & sub_op);
    virtual OpVector soundEatOperation(const Sound & op, Eat & sub_op);
    virtual OpVector soundBurnOperation(const Sound & op, Burn & sub_op);
    virtual OpVector soundImaginaryOperation(const Sound & op, Imaginary& sub_op);
    virtual OpVector soundMoveOperation(const Sound & op, Move & sub_op);
    virtual OpVector soundSetOperation(const Sound & op, Set & sub_op);
    virtual OpVector soundTalkOperation(const Sound & op, Talk & sub_op);
    virtual OpVector soundTouchOperation(const Sound & op, Touch & sub_op);
    virtual OpVector soundOtherOperation(const Sound & op,RootOperation & sub_op);

    virtual OpVector SightOperation(const Sight & op);
    virtual OpVector SoundOperation(const Sound & op);
    virtual OpVector AppearanceOperation(const Appearance & op);
    virtual OpVector DisappearanceOperation(const Disappearance & op);

    virtual OpVector operation(const RootOperation & op);

    OpVector callSightOperation(const Sight & op, RootOperation & sub_op);
    OpVector callSoundOperation(const Sound & op, RootOperation & sub_op);

    friend class PythonMindScript;
};

#endif // RULESETS_BASE_MIND_H
