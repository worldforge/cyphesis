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

    void scriptSubscribe(const std::string &);

    virtual void sightLoginOperation(const RootOperation &, RootOperation &, OpVector &);
    virtual void sightActionOperation(const RootOperation &, RootOperation &, OpVector &);
    virtual void sightChopOperation(const RootOperation &, RootOperation &, OpVector &);
    virtual void sightCombineOperation(const RootOperation &, RootOperation &, OpVector &);
    virtual void sightCreateOperation(const RootOperation &, RootOperation &, OpVector &);
    virtual void sightCutOperation(const RootOperation &, RootOperation &, OpVector &);
    virtual void sightDeleteOperation(const RootOperation &, RootOperation &, OpVector &);
    virtual void sightDivideOperation(const RootOperation &, RootOperation &, OpVector &);
    virtual void sightEatOperation(const RootOperation &, RootOperation &, OpVector &);
    virtual void sightBurnOperation(const RootOperation &, RootOperation &, OpVector &);
    virtual void sightImaginaryOperation(const RootOperation &, RootOperation &, OpVector &);
    virtual void sightMoveOperation(const RootOperation &, RootOperation &, OpVector &);
    virtual void sightSetOperation(const RootOperation &, RootOperation &, OpVector &);
    virtual void sightTalkOperation(const RootOperation &, RootOperation &, OpVector &);
    virtual void sightTouchOperation(const RootOperation &, RootOperation &, OpVector &);
    virtual void sightOtherOperation(const RootOperation &,RootOperation &, OpVector &);

    virtual void soundLoginOperation(const RootOperation &, RootOperation &, OpVector &);
    virtual void soundActionOperation(const RootOperation &, RootOperation &, OpVector &);
    virtual void soundChopOperation(const RootOperation &, RootOperation &, OpVector &);
    virtual void soundCombineOperation(const RootOperation &, RootOperation &, OpVector &);
    virtual void soundCreateOperation(const RootOperation &, RootOperation &, OpVector &);
    virtual void soundCutOperation(const RootOperation &, RootOperation &, OpVector &);
    virtual void soundDeleteOperation(const RootOperation &, RootOperation &, OpVector &);
    virtual void soundDivideOperation(const RootOperation &, RootOperation &, OpVector &);
    virtual void soundEatOperation(const RootOperation &, RootOperation &, OpVector &);
    virtual void soundBurnOperation(const RootOperation &, RootOperation &, OpVector &);
    virtual void soundImaginaryOperation(const RootOperation &, RootOperation &, OpVector &);
    virtual void soundMoveOperation(const RootOperation &, RootOperation &, OpVector &);
    virtual void soundSetOperation(const RootOperation &, RootOperation &, OpVector &);
    virtual void soundTalkOperation(const RootOperation &, RootOperation &, OpVector &);
    virtual void soundTouchOperation(const RootOperation &, RootOperation &, OpVector &);
    virtual void soundOtherOperation(const RootOperation &,RootOperation &, OpVector &);

    virtual void SightOperation(const RootOperation &, OpVector &);
    virtual void SoundOperation(const RootOperation &, OpVector &);
    virtual void AppearanceOperation(const RootOperation &, OpVector &);
    virtual void DisappearanceOperation(const RootOperation &, OpVector &);

    virtual void operation(const RootOperation &, OpVector &);

    void callSightOperation(const RootOperation &, RootOperation &, OpVector &);
    void callSoundOperation(const RootOperation &, RootOperation &, OpVector &);

    friend class PythonMindScript;
};

#endif // RULESETS_BASE_MIND_H
