/*
 Copyright (C) 2021 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef CYPHESIS_MEMENTITYPROPERTIES_H
#define CYPHESIS_MEMENTITYPROPERTIES_H


#include "common/Property.h"

#include <wfmath/vector.h>
#include <wfmath/point.h>
#include <wfmath/quaternion.h>
#include <wfmath/axisbox.h>


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

struct VelocityData
{
    WFMath::Vector<3> data;
    double timestamp = 0;
};
struct AngularData
{
    WFMath::Vector<3> data;
    double timestamp = 0;
};

struct MovementData
{
    VelocityData velocity;
    AngularData angular;

    bool operator!=(const MovementData& rhs) const
    {
        return velocity.timestamp != rhs.velocity.timestamp
               || velocity.data != rhs.velocity.data
               || angular.data != rhs.angular.data
               || angular.timestamp != rhs.angular.timestamp;
    }
};


class ReadPositionProperty : public PropertyBase
{
    public:
        static constexpr const char* property_atlastype = "list";
        static constexpr const char* property_name = "pos";

        WFMath::Point<3>& m_data;

        ReadPositionProperty(WFMath::Point<3>& data);

        int get(Atlas::Message::Element& val) const override;

        void set(const Atlas::Message::Element&) override;

        ReadPositionProperty* copy() const override;

};

class ReadOrientationProperty : public PropertyBase
{
    public:
        static constexpr const char* property_atlastype = "list";
        static constexpr const char* property_name = "orientation";

        WFMath::Quaternion& m_data;

        ReadOrientationProperty(WFMath::Quaternion& data);

        int get(Atlas::Message::Element& val) const override;

        void set(const Atlas::Message::Element&) override;

        ReadOrientationProperty* copy() const override;

};

class ReadBboxProperty : public PropertyBase
{
    public:
        static constexpr const char* property_atlastype = "list";
        static constexpr const char* property_name = "bbox";

        WFMath::AxisBox<3>& m_data;

        ReadBboxProperty(WFMath::AxisBox<3>& data);

        int get(Atlas::Message::Element& val) const override;

        void set(const Atlas::Message::Element&) override;

        ReadBboxProperty* copy() const override;

};

class ReadVelocityProperty : public PropertyBase
{
    public:
        static constexpr const char* property_atlastype = "list";
        static constexpr const char* property_name = "velocity";

        VelocityData& m_data;

        ReadVelocityProperty(VelocityData& data);

        int get(Atlas::Message::Element& val) const override;

        void set(const Atlas::Message::Element&) override;

        ReadVelocityProperty* copy() const override;

};

class ReadAngularProperty : public PropertyBase
{
    public:
        static constexpr const char* property_atlastype = "list";
        static constexpr const char* property_name = "angular";

        AngularData& m_data;

        ReadAngularProperty(AngularData& data);

        int get(Atlas::Message::Element& val) const override;

        void set(const Atlas::Message::Element&) override;

        ReadAngularProperty* copy() const override;

};


#endif //CYPHESIS_MEMENTITYPROPERTIES_H
