#ifndef BASE_MIND_H
#define BASE_MIND_H

#include "Thing.h"

class BaseMind : public Thing {
  public:
    BaseMind(string &);
    virtual ~BaseMind() { }

    //bad_type sight_create_operation(bad_type original_op, bad_type op)
    //bad_type sight_delete_operation(bad_type original_op, bad_type op)
    //bad_type sight_set_operation(bad_type original_op, bad_type op)
    //bad_type sight_move_operation(bad_type original_op, bad_type op)
    //bad_type sight_undefined_operation(bad_type original_op, bad_type op)
    //bad_type sound_undefined_operation(bad_type original_op, bad_type op)
    virtual oplist Sight_Operation(const Sight & op, Login & sub_op);
    virtual oplist Sight_Operation(const Sight & op, Create & sub_op);
    virtual oplist Sight_Operation(const Sight & op, Delete & sub_op);
    virtual oplist Sight_Operation(const Sight & op, Move & sub_op);
    virtual oplist Sight_Operation(const Sight & op, Set & sub_op);
    virtual oplist Sight_Operation(const Sight & op, Touch & sub_op);
    virtual oplist Sight_Operation(const Sight & op, RootOperation & sub_op);

    oplist call_sight_operation(const Sight & op, RootOperation & sub_op);
    virtual oplist Operation(const Sight & op);
    virtual oplist Operation(const Sound & op);
    //RootOperation * get_op_name_and_sub(RootOperation & op, string & name);
    //virtual int call_triggers(RootOperation & op);
    virtual oplist operation(RootOperation & op);

};

#endif /* BASE_MIND_H */
