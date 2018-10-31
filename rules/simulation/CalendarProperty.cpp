// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2005 Alistair Riddoch
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


#include "CalendarProperty.h"

#include "modules/DateTime.h"

#include "common/debug.h"

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Message::FloatType;

CalendarProperty::CalendarProperty() = default;

int CalendarProperty::get(Element & ent) const
{
    MapType & c = (ent = MapType()).asMap();
    c["seconds_per_minute"] = (int)DateTime::spm();
    c["minutes_per_hour"] = (int)DateTime::mph();
    c["hours_per_day"] = (int)DateTime::hpd();
    c["days_per_month"] = (int)DateTime::dpm();
    c["months_per_year"] = (int)DateTime::mpy();
    return 0;
}

void CalendarProperty::set(const Element & ent)
{
}

CalendarProperty * CalendarProperty::copy() const
{
    return new CalendarProperty(*this);
}
