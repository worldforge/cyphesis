#ifndef CHARACTER_H
#define CHARACTER_H

typedef int bad_type; // Remove this to get unset type reporting

#define None 0 // Remove this to deal with un-initialied vars

class Mind;

#include "Thing.h"

class MovementInfo {
    bad_type target_location;
    bad_type velocity;
    bad_type serialno;
    bad_type last_movement_time;

    public:
    MovementInfo();
    bad_type update_needed(bad_type location);
    bad_type get_tick_addition(bad_type coordinates);
    bad_type __str__();
    bad_type reset();
    bad_type gen_move_operation(bad_type body, bad_type loc=None);
};

class Character : public Thing {
  protected:
    MovementInfo movement;
    string sex;
  public:
    Mind * mind;
    double drunkness;

    Character();
    virtual ~Character() { }


    virtual RootOperation * Operation(const Setup & op);
    virtual RootOperation * Operation(const Tick & op);
    virtual RootOperation * Operation(const Talk & op);
    virtual RootOperation * Mind_Operation(const Login & op);
    virtual RootOperation * Mind_Operation(const Create & op);
    virtual RootOperation * Mind_Operation(const Cut & op);
    virtual RootOperation * Mind_Operation(const Delete & op);
    virtual RootOperation * Mind_Operation(const Eat & op);
    virtual RootOperation * Mind_Operation(const Move & op);
    virtual RootOperation * Mind_Operation(const Set & op);
    virtual RootOperation * Mind_Operation(const Sight & op);
    virtual RootOperation * Mind_Operation(const Sound & op);
    virtual RootOperation * Mind_Operation(const Talk & op);
    virtual RootOperation * Mind_Operation(const Tick & op);
    virtual RootOperation * Mind_Operation(const Touch & op);
    virtual RootOperation * Mind_Operation(const Look & op);
    virtual RootOperation * Mind_Operation(const Load & op);
    virtual RootOperation * Mind_Operation(const Save & op);
    virtual RootOperation * Mind_Operation(const Setup & op);
    virtual RootOperation * Mind_Operation(const RootOperation & op);
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
    virtual RootOperation * W2m_Operation(const RootOperation & op);
    virtual RootOperation * send_mind(RootOperation & msg);
    virtual RootOperation * mind2body(const RootOperation & op);
    virtual RootOperation * world2body(const RootOperation & op);
    virtual RootOperation * world2mind(const RootOperation & op);
    virtual RootOperation * external_message(const RootOperation & op);
    virtual RootOperation * operation(const RootOperation & op);
    virtual RootOperation * external_operation(const RootOperation & op);
};

#endif /* CHARACTER_H */
