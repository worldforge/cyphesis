// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef RULESETS_BASE_MIND_H
#define RULESETS_BASE_MIND_H

#include "MemEntity.h"
#include "MemMap.h"

#include "modules/WorldTime.h"

/// \brief This is core class for representing the mind of an AI entity.
///
/// Instances of this class almost always have an associated script
/// handles most of the AI. The main purpose of this class is to handle
/// operations and interface to the MemMap used as the core of
/// the entity's memory.
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

    virtual void sightLoginOperation(const Sight & op, Login & sub_op, OpVector &);
    virtual void sightActionOperation(const Sight & op, Action & sub_op, OpVector &);
    virtual void sightChopOperation(const Sight & op, Chop & sub_op, OpVector &);
    virtual void sightCombineOperation(const Sight & op, Combine & sub_op, OpVector &);
    virtual void sightCreateOperation(const Sight & op, Create & sub_op, OpVector &);
    virtual void sightCutOperation(const Sight & op, Cut & sub_op, OpVector &);
    virtual void sightDeleteOperation(const Sight & op, Delete & sub_op, OpVector &);
    virtual void sightDivideOperation(const Sight & op, Divide & sub_op, OpVector &);
    virtual void sightEatOperation(const Sight & op, Eat & sub_op, OpVector &);
    virtual void sightBurnOperation(const Sight & op, Burn & sub_op, OpVector &);
    virtual void sightImaginaryOperation(const Sight & op, Imaginary& sub_op, OpVector &);
    virtual void sightMoveOperation(const Sight & op, Move & sub_op, OpVector &);
    virtual void sightSetOperation(const Sight & op, Set & sub_op, OpVector &);
    virtual void sightTalkOperation(const Sight & op, Talk & sub_op, OpVector &);
    virtual void sightTouchOperation(const Sight & op, Touch & sub_op, OpVector &);
    virtual void sightOtherOperation(const Sight & op,RootOperation & sub_op, OpVector &);

    virtual void soundLoginOperation(const Sound & op, Login & sub_op, OpVector &);
    virtual void soundActionOperation(const Sound & op, Action & sub_op, OpVector &);
    virtual void soundChopOperation(const Sound & op, Chop & sub_op, OpVector &);
    virtual void soundCombineOperation(const Sound & op, Combine & sub_op, OpVector &);
    virtual void soundCreateOperation(const Sound & op, Create & sub_op, OpVector &);
    virtual void soundCutOperation(const Sound & op, Cut & sub_op, OpVector &);
    virtual void soundDeleteOperation(const Sound & op, Delete & sub_op, OpVector &);
    virtual void soundDivideOperation(const Sound & op, Divide & sub_op, OpVector &);
    virtual void soundEatOperation(const Sound & op, Eat & sub_op, OpVector &);
    virtual void soundBurnOperation(const Sound & op, Burn & sub_op, OpVector &);
    virtual void soundImaginaryOperation(const Sound & op, Imaginary& sub_op, OpVector &);
    virtual void soundMoveOperation(const Sound & op, Move & sub_op, OpVector &);
    virtual void soundSetOperation(const Sound & op, Set & sub_op, OpVector &);
    virtual void soundTalkOperation(const Sound & op, Talk & sub_op, OpVector &);
    virtual void soundTouchOperation(const Sound & op, Touch & sub_op, OpVector &);
    virtual void soundOtherOperation(const Sound & op,RootOperation & sub_op, OpVector &);

    virtual void SightOperation(const Sight & op, OpVector &);
    virtual void SoundOperation(const Sound & op, OpVector &);
    virtual void AppearanceOperation(const Appearance & op, OpVector &);
    virtual void DisappearanceOperation(const Disappearance & op, OpVector &);

    virtual void operation(const RootOperation & op, OpVector &);

    void callSightOperation(const Sight & op, RootOperation & sub_op, OpVector &);
    void callSoundOperation(const Sound & op, RootOperation & sub_op, OpVector &);

    friend class PythonMindScript;
};

#endif // RULESETS_BASE_MIND_H
