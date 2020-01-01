/*
 Copyright (C) 2019 Erik Ogenvik

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

#ifndef CYPHESIS_MODIFIER_H
#define CYPHESIS_MODIFIER_H

#include <Atlas/Message/Element.h>

struct Modifier
{
    virtual void process(Atlas::Message::Element& element, const Atlas::Message::Element& baseValue) = 0;
};

struct PrependModifier : public Modifier
{

    Atlas::Message::Element mValue;

    explicit PrependModifier(Atlas::Message::Element value) : mValue(std::move(value))
    {}

    void process(Atlas::Message::Element& element, const Atlas::Message::Element& baseValue) override;
};

struct AppendModifier : public Modifier
{

    Atlas::Message::Element mValue;

    explicit AppendModifier(Atlas::Message::Element value) : mValue(std::move(value))
    {}

    void process(Atlas::Message::Element& element, const Atlas::Message::Element& baseValue) override;
};

struct SubtractModifier : public Modifier
{
    Atlas::Message::Element mValue;

    explicit SubtractModifier(Atlas::Message::Element value) : mValue(std::move(value))
    {}

    void process(Atlas::Message::Element& element, const Atlas::Message::Element& baseValue) override;
};

struct MultiplyModifier : public Modifier
{
    Atlas::Message::Element mValue;

    explicit MultiplyModifier(Atlas::Message::Element value) : mValue(std::move(value))
    {}

    void process(Atlas::Message::Element& element, const Atlas::Message::Element& baseValue) override;
};

#endif //CYPHESIS_MODIFIER_H
