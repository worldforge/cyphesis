// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef CHARACTER_H
#define CHARACTER_H

class BaseMind;
class ExternalMind;
class Account;
class Character;
class Location;

#include "Thing.h"

#include <physics/Vector3D.h>

class MovementInfo {
    Character * body;
    friend class Character;
    double last_movement_time;
    Vector3D target_location;
    Vector3D updated_location;
    Vector3D velocity;
    Vector3D face;
    int serialno;

  public:
    MovementInfo(Character * body);

    bool update_needed(const Location & location) const;
    double get_tick_addition(const Vector3D & coordinates) const;
    void reset();
    void check_collisions(const Location & loc);
    Move * gen_face_operation(const Location &);
    Move * gen_move_operation(Location *,const Location &);
    Move * gen_move_operation(Location *);
};

class Character : public Thing {
  protected:
    MovementInfo movement;
    friend class MovementInfo;
    int autom;
  public:
    BaseMind * mind;
    ExternalMind * external_mind;
    Account * player;
    double drunkness;
    string sex;

    Character();
    virtual ~Character();

    virtual const Object & operator[](const string & aname);
    virtual void set(const string & aname, const Object & attr);

    virtual void addObject(Message::Object *) const;
    virtual oplist Operation(const Setup & op);
    virtual oplist Operation(const Tick & op);
    virtual oplist Operation(const Talk & op);
    virtual oplist Mind_Operation(const Login & op) { oplist res; return(res); }
    virtual oplist Mind_Operation(const Create & op);
    virtual oplist Mind_Operation(const Cut & op);
    virtual oplist Mind_Operation(const Delete & op);
    virtual oplist Mind_Operation(const Eat & op);
    virtual oplist Mind_Operation(const Move & op);
    virtual oplist Mind_Operation(const Set & op);
    virtual oplist Mind_Operation(const Sight & op) { oplist res; return(res); }
    virtual oplist Mind_Operation(const Sound & op) { oplist res; return(res); }
    virtual oplist Mind_Operation(const Talk & op);
    virtual oplist Mind_Operation(const Tick & op);
    virtual oplist Mind_Operation(const Touch & op);
    virtual oplist Mind_Operation(const Look & op);
    virtual oplist Mind_Operation(const Load & op) { oplist res; return(res); }
    virtual oplist Mind_Operation(const Save & op) { oplist res; return(res); }
    virtual oplist Mind_Operation(const Setup & op);
    virtual oplist Mind_Operation(const Appearance & op) { oplist res; return(res); }
    virtual oplist Mind_Operation(const Disappearance & op) { oplist res; return(res); }
    virtual oplist Mind_Operation(const Error & op) { oplist res; return(res); }
    virtual oplist Mind_Operation(const RootOperation & op) { oplist res; return(res); }
    virtual oplist W2m_Operation(const Login & op) { oplist res; return(res); }
    virtual oplist W2m_Operation(const Chop & op) { oplist res; return(res); }
    virtual oplist W2m_Operation(const Create & op) { oplist res; return(res); }
    virtual oplist W2m_Operation(const Cut & op) { oplist res; return(res); }
    virtual oplist W2m_Operation(const Delete & op) { oplist res; return(res); }
    virtual oplist W2m_Operation(const Eat & op) { oplist res; return(res); }
    virtual oplist W2m_Operation(const Fire & op) { oplist res; return(res); }
    virtual oplist W2m_Operation(const Move & op) { oplist res; return(res); }
    virtual oplist W2m_Operation(const Set & op) { oplist res; return(res); }
    virtual oplist W2m_Operation(const Sight & op);
    virtual oplist W2m_Operation(const Sound & op);
    virtual oplist W2m_Operation(const Touch & op);
    virtual oplist W2m_Operation(const Tick & op);
    virtual oplist W2m_Operation(const Look & op) { oplist res; return(res); }
    virtual oplist W2m_Operation(const Load & op) { oplist res; return(res); }
    virtual oplist W2m_Operation(const Save & op) { oplist res; return(res); }
    virtual oplist W2m_Operation(const Setup & op);
    virtual oplist W2m_Operation(const Appearance & op) { oplist res; return(res); }
    virtual oplist W2m_Operation(const Disappearance & op) { oplist res; return(res); }
    virtual oplist W2m_Operation(const Error & op);
    virtual oplist W2m_Operation(const RootOperation & op) { oplist res; return(res); }
    virtual oplist send_mind(const RootOperation & op);
    virtual oplist mind2body(const RootOperation & op);
    virtual oplist world2body(const RootOperation & op);
    virtual oplist world2mind(const RootOperation & op);
    virtual oplist external_message(const RootOperation & op);
    virtual oplist operation(const RootOperation & op);
    virtual oplist external_operation(const RootOperation & op);
};

#endif /* CHARACTER_H */
