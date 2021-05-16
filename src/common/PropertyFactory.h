// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2006 Alistair Riddoch
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


#ifndef COMMON_PROPERTY_FACTORY_H
#define COMMON_PROPERTY_FACTORY_H

#include <cinttypes>
#include <memory>
#include "Property.h"

/// \brief Kit interface for factories to create Property objects.
class PropertyKit {
  public:
    virtual ~PropertyKit() = default;

    /// \brief Create a new Property instance
    virtual std::unique_ptr<PropertyBase> newProperty() = 0;

    /// \brief Create a copy of this factory.
    virtual std::unique_ptr<PropertyKit> duplicateFactory() const = 0;
};

/// \brief Factory template to create standard Property objects.
template <class T>
class PropertyFactory : public PropertyKit {
  public:
    std::uint32_t m_flags;
    PropertyFactory() : m_flags(0) {}
    std::unique_ptr<PropertyBase> newProperty() override;
    std::unique_ptr<PropertyKit> duplicateFactory() const override;
};

#endif // COMMON_PROPERTY_FACTORY_H
