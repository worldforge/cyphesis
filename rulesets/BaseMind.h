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

    void sightSubscribe(const std::string & op, OpNo no) {
        opSightLookup[op] = no;
    }

    void soundSubscribe(const std::string & op, OpNo no) {
        opSoundLookup[op] = no;
    }
  public:
    BaseMind(const std::string &, const std::string &);
    virtual ~BaseMind();

    MemMap * getMap() { return &m_map; }
    WorldTime * getTime() { return &m_time; }

    void sleep() { m_isAwake = false; }
    void awake() { m_isAwake = true; }

    virtual void scriptSubscribe(const std::string &);

    virtual void sightLoginOperation(const Operation &, const Operation &, OpVector &);
    virtual void sightActionOperation(const Operation &, const Operation &, OpVector &);
    virtual void sightChopOperation(const Operation &, const Operation &, OpVector &);
    virtual void sightCombineOperation(const Operation &, const Operation &, OpVector &);
    virtual void sightCreateOperation(const Operation &, const Operation &, OpVector &);
    virtual void sightCutOperation(const Operation &, const Operation &, OpVector &);
    virtual void sightDeleteOperation(const Operation &, const Operation &, OpVector &);
    virtual void sightDivideOperation(const Operation &, const Operation &, OpVector &);
    virtual void sightEatOperation(const Operation &, const Operation &, OpVector &);
    virtual void sightBurnOperation(const Operation &, const Operation &, OpVector &);
    virtual void sightImaginaryOperation(const Operation &, const Operation &, OpVector &);
    virtual void sightMoveOperation(const Operation &, const Operation &, OpVector &);
    virtual void sightSetOperation(const Operation &, const Operation &, OpVector &);
    virtual void sightTalkOperation(const Operation &, const Operation &, OpVector &);
    virtual void sightTouchOperation(const Operation &, const Operation &, OpVector &);
    virtual void sightOtherOperation(const Operation &, const Operation &, OpVector &);

    virtual void soundLoginOperation(const Operation &, const Operation &, OpVector &);
    virtual void soundActionOperation(const Operation &, const Operation &, OpVector &);
    virtual void soundChopOperation(const Operation &, const Operation &, OpVector &);
    virtual void soundCombineOperation(const Operation &, const Operation &, OpVector &);
    virtual void soundCreateOperation(const Operation &, const Operation &, OpVector &);
    virtual void soundCutOperation(const Operation &, const Operation &, OpVector &);
    virtual void soundDeleteOperation(const Operation &, const Operation &, OpVector &);
    virtual void soundDivideOperation(const Operation &, const Operation &, OpVector &);
    virtual void soundEatOperation(const Operation &, const Operation &, OpVector &);
    virtual void soundBurnOperation(const Operation &, const Operation &, OpVector &);
    virtual void soundImaginaryOperation(const Operation &, const Operation &, OpVector &);
    virtual void soundMoveOperation(const Operation &, const Operation &, OpVector &);
    virtual void soundSetOperation(const Operation &, const Operation &, OpVector &);
    virtual void soundTalkOperation(const Operation &, const Operation &, OpVector &);
    virtual void soundTouchOperation(const Operation &, const Operation &, OpVector &);
    virtual void soundOtherOperation(const Operation &, const Operation &, OpVector &);

    virtual void SightOperation(const Operation &, OpVector &);
    virtual void SoundOperation(const Operation &, OpVector &);
    virtual void AppearanceOperation(const Operation &, OpVector &);
    virtual void DisappearanceOperation(const Operation &, OpVector &);

    virtual void operation(const Operation &, OpVector &);

    void callSightOperation(const Operation &, const Operation &, OpVector &);
    void callSoundOperation(const Operation &, const Operation &, OpVector &);

    friend class PythonMindScript;
};

#endif // RULESETS_BASE_MIND_H
