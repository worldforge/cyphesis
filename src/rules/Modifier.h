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

#include "common/ModifierType.h"
#include <Atlas/Message/Element.h>
#include <memory>

/**
 * Allows modifications of arbitrary values.
 *
 * Note that there is some overlap in some of the modifiers depending on the type.
 *
 * For example, a Subtract(2) modifier can also be expressed with a Prepend(-2) or Append(-2) modifier.
 */
struct Modifier
{
    
    Atlas::Message::Element mValue;

    explicit Modifier(Atlas::Message::Element value) : mValue(std::move(value))
    {}

    virtual ~Modifier() = default;
    /**
     * Modifies the submitted value in place.
     * @param element The value to be changed.
     * @param baseValue The base value, which is taken into account in some modifiers.
     */
    virtual void process(Atlas::Message::Element& element, const Atlas::Message::Element& baseValue) const = 0;

    virtual ModifierType getType() const = 0;
    
    static std::unique_ptr<Modifier> createModifier(ModifierType modificationType, Atlas::Message::Element attr);

};

struct DefaultModifier : public Modifier
{

    explicit DefaultModifier(Atlas::Message::Element value) : Modifier(std::move(value))
    {}

    void process(Atlas::Message::Element& element, const Atlas::Message::Element& baseValue) const override;

    ModifierType getType() const override
    {
        return ModifierType::Default;
    }
};

/**
 * This value will be prepended.
 * For numerical values it will result in an addition.
 * For strings it will result in the value being added to the start of the existing string.
 * For maps it will result in the values being inserted.
 * For lists it will result in the values being added to the start of the list.
 */
struct PrependModifier : public Modifier
{

    explicit PrependModifier(Atlas::Message::Element value) : Modifier(std::move(value))
    {}

    void process(Atlas::Message::Element& element, const Atlas::Message::Element& baseValue) const override;

    ModifierType getType() const override
    {
        return ModifierType::Prepend;
    }
};

/**
 * This value will be appended.
 * For numerical values it will result in an addition.
 * For strings it will result in the value being added to the end of the existing string.
 * For maps it will result in the values being inserted.
 * For lists it will result in the values being added to the back of the list.
 */
struct AppendModifier : public Modifier
{
    explicit AppendModifier(Atlas::Message::Element value) : Modifier(std::move(value))
    {}

    void process(Atlas::Message::Element& element, const Atlas::Message::Element& baseValue) const override;

    ModifierType getType() const override
    {
        return ModifierType::Append;
    }
};

/**
 * This value will be subtracted.
 * For numerical values it will result in a subtraction.
 * For strings nothing will happen, since it's not obvious how one subtracts one string from another.
 * For maps it will result in the keys being removed (any values are ignored).
 * For lists it will result in the values being removed to the start of the list.
 */
struct SubtractModifier : public Modifier
{
    explicit SubtractModifier(Atlas::Message::Element value) : Modifier(std::move(value))
    {}

    void process(Atlas::Message::Element& element, const Atlas::Message::Element& baseValue) const override;

    ModifierType getType() const override
    {
        return ModifierType::Subtract;
    }

};

/**
 * This value will be multiplied with the base value, and then added.
 * For numerical values it will result in a multiplication and then an addition.
 * The reason for doing this instead of just regular multiplication is because we want to allow for these values to stack.
 * For strings nothing will happen.
 * For maps nothing will happen.
 * For lists nothing will happen.
 */
struct AddFractionModifier : public Modifier
{

    explicit AddFractionModifier(Atlas::Message::Element value) : Modifier(std::move(value))
    {}

    void process(Atlas::Message::Element& element, const Atlas::Message::Element& baseValue) const override;

    ModifierType getType() const override
    {
        return ModifierType::AddFraction;
    }

};

#endif //CYPHESIS_MODIFIER_H
