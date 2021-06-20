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

#include "MemEntityProperties.h"
#include "common/log.h"
#include <wfmath/atlasconv.h>

ReadPositionProperty::ReadPositionProperty(WFMath::Point<3>& data)
: m_data(data)
{
}

int ReadPositionProperty::get(Atlas::Message::Element& val) const
{
    if (m_data.isValid()) {
        val = m_data.toAtlas();
        return 0;
    }
    return 1;

}

void ReadPositionProperty::set(const Atlas::Message::Element& val)
{
    if (val.isList()) {
        try {
            m_data.fromAtlas(val.List());
        } catch (...) {
            m_data = {};
            log(ERROR, "ReadPositionProperty::set: Data was not in format which could be parsed into 3d point.");
        }
    } else if (val.isNone()) {
        m_data = {};
    } else {
        m_data = {};
        log(ERROR, "ReadPositionProperty::set: Data was not in format which could be parsed into 3d point.");
    }
}

ReadPositionProperty* ReadPositionProperty::copy() const
{
    return nullptr;
}






ReadOrientationProperty::ReadOrientationProperty(WFMath::Quaternion& data)
        : m_data(data)
{
}


int ReadOrientationProperty::get(Atlas::Message::Element& val) const
{
    if (m_data.isValid()) {
        val = m_data.toAtlas();
        return 0;
    }
    return 1;

}

void ReadOrientationProperty::set(const Atlas::Message::Element& val)
{
    if (val.isList()) {
        try {
            m_data.fromAtlas(val.List());
        } catch (...) {
            m_data = {};
            log(ERROR, "ReadOrientationProperty::set: Data was not in format which could be parsed into 3d point.");
        }
    } else if (val.isNone()) {
        m_data = {};
    } else {
        m_data = {};
        log(ERROR, "ReadOrientationProperty::set: Data was not in format which could be parsed into 3d point.");
    }
}

ReadOrientationProperty* ReadOrientationProperty::copy() const
{
    return nullptr;
}




ReadBboxProperty::ReadBboxProperty(WFMath::AxisBox<3>& data)
        : m_data(data)
{
}

int ReadBboxProperty::get(Atlas::Message::Element& val) const
{
    if (m_data.isValid()) {
        val = m_data.toAtlas();
        return 0;
    }
    return 1;
}

void ReadBboxProperty::set(const Atlas::Message::Element& val)
{
    if (val.isList()) {
        try {
            m_data.fromAtlas(val.List());
        } catch (...) {
            m_data = {};
            log(ERROR, "ReadBboxProperty::set: Data was not in format which could be parsed into 3d point.");
        }
    } else if (val.isNone()) {
        m_data = {};
    } else {
        m_data = {};
        log(ERROR, "ReadBboxProperty::set: Data was not in format which could be parsed into 3d point.");
    }
}

ReadBboxProperty* ReadBboxProperty::copy() const
{
    return nullptr;
}



ReadVelocityProperty::ReadVelocityProperty(VelocityData& data)
        : m_data(data)
{
}

int ReadVelocityProperty::get(Atlas::Message::Element& val) const
{
    if (m_data.data.isValid()) {
        val = m_data.data.toAtlas();
        return 0;
    }
    return 1;
}

void ReadVelocityProperty::set(const Atlas::Message::Element& val)
{
    if (val.isList()) {
        try {
            m_data.data.fromAtlas(val.List());
        } catch (...) {
            m_data = {};
            log(ERROR, "ReadBboxProperty::set: Data was not in format which could be parsed into 3d point.");
        }
    } else if (val.isNone()) {
        m_data = {};
    } else {
        m_data = {};
        log(ERROR, "ReadBboxProperty::set: Data was not in format which could be parsed into 3d point.");
    }
}

ReadVelocityProperty* ReadVelocityProperty::copy() const
{
    return nullptr;
}






ReadAngularProperty::ReadAngularProperty(AngularData& data)
        : m_data(data)
{
}

int ReadAngularProperty::get(Atlas::Message::Element& val) const
{
    if (m_data.data.isValid()) {
        val = m_data.data.toAtlas();
        return 0;
    }
    return 1;
}

void ReadAngularProperty::set(const Atlas::Message::Element& val)
{
    if (val.isList()) {
        try {
            m_data.data.fromAtlas(val.List());
        } catch (...) {
            m_data = {};
            log(ERROR, "ReadBboxProperty::set: Data was not in format which could be parsed into 3d point.");
        }
    } else if (val.isNone()) {
        m_data = {};
    } else {
        m_data = {};
        log(ERROR, "ReadBboxProperty::set: Data was not in format which could be parsed into 3d point.");
    }
}

ReadAngularProperty* ReadAngularProperty::copy() const
{
    return nullptr;
}