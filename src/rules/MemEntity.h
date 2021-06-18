// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
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


#ifndef RULESETS_MEM_ENTITY_H
#define RULESETS_MEM_ENTITY_H

#include "rules/LocatedEntity.h"

struct TransformData
{
    WFMath::Point<3> pos;
    WFMath::Quaternion orientation;

    bool operator!=(const TransformData& rhs) const
    {
        return pos != rhs.pos
               || orientation != rhs.orientation;
    }
};

struct MovementData
{
    struct
    {
        WFMath::Vector<3> data;
        double timestamp = 0;
    } velocity;
    struct
    {
        WFMath::Vector<3> data;
        double timestamp = 0;
    } angular;

    bool operator!=(const MovementData& rhs) const
    {
        return velocity.timestamp != rhs.velocity.timestamp
               || velocity.data != rhs.velocity.data
               || angular.data != rhs.angular.data
               || angular.timestamp != rhs.angular.timestamp;
    }
};

/// \brief This class is used to represent entities inside MemMap used
/// by the mind of an AI.
///
/// It adds a flag to indicate if this entity is currently visible, and
/// a means of tracking when it was last seen, so garbage entities can
/// be cleaned up.
class MemEntity : public LocatedEntity
{
    protected:
        double m_lastSeen;
    public:
        explicit MemEntity(const std::string& id, long intId);

        ~MemEntity() override = default;

        void setVisible(bool v = true)
        {
            if (v) {
                m_flags.addFlags(entity_visible);
            } else {
                m_flags.removeFlags(entity_visible);
            }
        }

        const double& lastSeen() const
        {
            return m_lastSeen;
        }

        void update(const double& d)
        {
            if (d >= m_lastSeen) {
                m_lastSeen = d;
            }
        }

        void externalOperation(const Operation& op, Link&) override;

        void operation(const Operation&, OpVector&) override;

        void destroy() override;

        TransformData m_transform;
        MovementData m_movement;
        WFMath::AxisBox<3> m_bbox;
        //Location m_location;

};

#endif // RULESETS_MEM_ENTITY_H
