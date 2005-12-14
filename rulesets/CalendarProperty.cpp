// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "CalendarProperty.h"

#include "modules/DateTime.h"

#include "common/debug.h"

#include <cassert>

static const bool debug_flag = false;

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Message::FloatType;

CalendarProperty::CalendarProperty(unsigned int flags) : PropertyBase(flags)
{
}

void CalendarProperty::get(Element & ent) const
{
    MapType & c = (ent = MapType()).asMap();
    c["seconds_per_minute"] = (int)DateTime::spm();
    c["minutes_per_hour"] = (int)DateTime::mph();
    c["hours_per_day"] = (int)DateTime::hpd();
    c["days_per_month"] = (int)DateTime::dpm();
    c["months_per_year"] = (int)DateTime::mpy();
}

void CalendarProperty::set(const Element & ent)
{
    return;
}
