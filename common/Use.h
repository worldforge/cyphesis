// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef COMMON_USE_H
#define COMMON_USE_H

#include <Atlas/Objects/Operation/Action.h>

namespace Atlas { namespace Objects { namespace Operation {

/// \brief Operation class to use a tool by a character
///
/// Sent by clients or minds to tell a character to use a tool. Not dispatched
/// within the world. One or more other operations are typically the result
/// of this being sent by the client.
class Use : public Action {
  protected:
    Use(const char *, const char *);
  public:
    Use();
    virtual ~Use();
    static Use Class();
};

} } }

#endif // COMMON_USE_H
