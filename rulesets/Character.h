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
    double last_movement_time;

  public:
    Vector3D target_location;
    Vector3D velocity;
    int serialno;

    MovementInfo(Character * body);

    bool update_needed(const Location & location);
    double get_tick_addition(const Vector3D & coordinates);
    void reset();
    Move * gen_move_operation(Location *,Location &);
    Move * gen_move_operation(Location *);
};

class Character : public Thing {
  protected:
    MovementInfo movement;
    string sex;
    int autom;
  public:
    BaseMind * mind;
    ExternalMind * external_mind;
    Account * player;
    double drunkness;

    Character();
    virtual ~Character() { }


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
    virtual oplist Mind_Operation(const Error & op) { oplist res; return(res); }
    virtual oplist Mind_Operation(const RootOperation & op) { oplist res; return(res); }
    virtual oplist W2m_Operation(const Login & op) { oplist res; return(res); }
    virtual oplist W2m_Operation(const Create & op) { oplist res; return(res); }
    virtual oplist W2m_Operation(const Delete & op) { oplist res; return(res); }
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
    virtual oplist W2m_Operation(const Error & op);
    virtual oplist W2m_Operation(const RootOperation & op) { oplist res; return(res); }
    virtual oplist send_mind(const RootOperation & msg);
    virtual oplist mind2body(const RootOperation & op);
    virtual oplist world2body(const RootOperation & op);
    virtual oplist world2mind(const RootOperation & op);
    virtual oplist external_message(const RootOperation & op);
    virtual oplist operation(const RootOperation & op);
    virtual oplist external_operation(const RootOperation & op);
};

#endif /* CHARACTER_H */
