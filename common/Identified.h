// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2008 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

// $Id: Identified.h,v 1.1 2008-01-17 23:31:08 alriddoch Exp $

#ifndef COMMON_IDENTIFIED_H
#define COMMON_IDENTIFIED_H

#include "OperationRouter.h"

#include <string>

/// \brief This is the base class for any entity which has an Atlas
/// compatible indentifier.
///
class Identified {
  private:
    /// \brief String identifier
    const std::string m_id;
    /// \brief Integer identifier
    const long m_intId;
  protected:
    explicit Identified(const std::string & id, long intId);
  public:

    /// \brief Read only accessor for string identity
    const std::string & getId() const {
        return m_id;
    }

    /// \brief Read only accessor for Integer identity
    long getIntId() const {
        return m_intId;
    }

    void error(const Operation &, const std::string & errstring, OpVector &,
               const std::string & to = "") const;
    void clientError(const Operation &, const std::string & errstring,
                     OpVector &, const std::string & to = "") const;
};

#endif // COMMON_IDENTIFIED_H
