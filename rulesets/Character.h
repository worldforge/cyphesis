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
    RootOperation * gen_move_operation(Location *,Location &);
    RootOperation * gen_move_operation(Location *);
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


    virtual RootOperation * Operation(const Setup & op);
    virtual RootOperation * Operation(const Tick & op);
    virtual RootOperation * Operation(const Talk & op);
    virtual RootOperation * Mind_Operation(const Login & op) { return(NULL); }
    virtual RootOperation * Mind_Operation(const Create & op);
    virtual RootOperation * Mind_Operation(const Cut & op);
    virtual RootOperation * Mind_Operation(const Delete & op);
    virtual RootOperation * Mind_Operation(const Eat & op);
    virtual RootOperation * Mind_Operation(const Move & op);
    virtual RootOperation * Mind_Operation(const Set & op);
    virtual RootOperation * Mind_Operation(const Sight & op) { return(NULL); }
    virtual RootOperation * Mind_Operation(const Sound & op) { return(NULL); }
    virtual RootOperation * Mind_Operation(const Talk & op);
    virtual RootOperation * Mind_Operation(const Tick & op);
    virtual RootOperation * Mind_Operation(const Touch & op);
    virtual RootOperation * Mind_Operation(const Look & op);
    virtual RootOperation * Mind_Operation(const Load & op) { return(NULL); }
    virtual RootOperation * Mind_Operation(const Save & op) { return(NULL); }
    virtual RootOperation * Mind_Operation(const Setup & op);
    virtual RootOperation * Mind_Operation(const RootOperation & op) { return(NULL); }
    virtual RootOperation * W2m_Operation(const Login & op) { return(NULL); }
    virtual RootOperation * W2m_Operation(const Create & op) { return(NULL); }
    virtual RootOperation * W2m_Operation(const Delete & op) { return(NULL); }
    virtual RootOperation * W2m_Operation(const Move & op) { return(NULL); }
    virtual RootOperation * W2m_Operation(const Set & op) { return(NULL); }
    virtual RootOperation * W2m_Operation(const Sight & op);
    virtual RootOperation * W2m_Operation(const Sound & op);
    virtual RootOperation * W2m_Operation(const Touch & op);
    virtual RootOperation * W2m_Operation(const Tick & op);
    virtual RootOperation * W2m_Operation(const Look & op) { return(NULL); }
    virtual RootOperation * W2m_Operation(const Load & op) { return(NULL); }
    virtual RootOperation * W2m_Operation(const Save & op) { return(NULL); }
    virtual RootOperation * W2m_Operation(const Setup & op);
    virtual RootOperation * W2m_Operation(const RootOperation & op) { return(NULL); }
    virtual RootOperation * send_mind(RootOperation & msg);
    virtual RootOperation * mind2body(const RootOperation & op);
    virtual RootOperation * world2body(const RootOperation & op);
    virtual RootOperation * world2mind(const RootOperation & op);
    virtual RootOperation * external_message(const RootOperation & op);
    virtual RootOperation * operation(const RootOperation & op);
    virtual RootOperation * external_operation(const RootOperation & op);
};

#endif /* CHARACTER_H */
