// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2009 Alistair Riddoch
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


#ifndef TOOLS_FLUSHER_H
#define TOOLS_FLUSHER_H

#include "common/ClientTask.h"

#include <memory>

class ObjectContext;

/// \brief Task class for flushing the server of character entities
class Flusher : public ClientTask {
  protected:
    std::weak_ptr<ObjectContext> m_context;
    
    std::string type;
  public:
    explicit Flusher(const std::shared_ptr<ObjectContext> & context);

    ~Flusher() override;

    void setup(const std::string & arg, OpVector & ret) override;

    void operation(const Operation & op, OpVector & res) override;
};

#endif // TOOLS_FLUSHER_H
