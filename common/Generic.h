// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2004 Alistair Riddoch

#ifndef COMMON_GENERIC_H
#define COMMON_GENERIC_H

#include <Atlas/Objects/Operation/RootOperation.h>

// This operation cannot be instantiated directly, it can only be given
// a parent string in order to create an operation instance with
// parents that none of the hardcoded types support. Operations of
// this type should only be created and interpreted in scripts. If
// operations must be understood by C++ code, then add a new class.

namespace Atlas { namespace Objects { namespace Operation {

/// \brief Operation class for operations generated from scripts for which
/// there is no hard-coded class
class Generic : public RootOperation {
  public:
    explicit Generic(const std::string & p);
    virtual ~Generic();
    static Generic Class(const std::string & p);
};

} } }

#endif // COMMON_GENERIC_H
