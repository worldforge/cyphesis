// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2004 Alistair Riddoch

#ifndef COMMON_UNSEEN_H
#define COMMON_UNSEEN_H

#include <Atlas/Objects/Operation/Perception.h>

namespace Atlas { namespace Objects { namespace Operation {

/// \brief Operation class to indicate that the target of a Look op is not
/// visible
///
/// Set to an entity which has sent a Look or other operation to which a
/// response cannot be sent. This occurs of the target of the look does not
/// exist or if it cannot be seen by the looking entity. Mind scripts or
/// clients can quite frequently send ops to entities which have been deleted
/// but for some reason the client did not get notified. This should allow
/// the client to remove the entity from its store.
class Unseen : public Perception {
  protected:
    Unseen(const char *, const char *);
  public:
    Unseen();
    virtual ~Unseen();
    static Unseen Class();
};

} } }

#endif // COMMON_UNSEEN_H
