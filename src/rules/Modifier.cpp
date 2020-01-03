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

#include "Modifier.h"
#include <algorithm>

void PrependModifier::process(Atlas::Message::Element& element, const Atlas::Message::Element& baseValue)
{
    if (element.isNone() || element.getType() != mValue.getType()) {
        element = mValue;
    } else {
        switch (mValue.getType()) {
            case Atlas::Message::Element::TYPE_INT:
                element = mValue.Int() + element.Int();
                break;
            case Atlas::Message::Element::TYPE_FLOAT:
                element = mValue.Float() + element.Float();
                break;
            case Atlas::Message::Element::TYPE_STRING:
                element = mValue.String() + element.String();
                break;
            case Atlas::Message::Element::TYPE_MAP:
                //Overwrite element entries; with C++17 we can use "insert_or_assign".
                for (const auto& entry: mValue.Map()) {
                    element.Map()[entry.first] = entry.second;
                }
                break;
            case Atlas::Message::Element::TYPE_LIST: {
                auto listCopy = std::move(element.List());
                element.List().clear();
                element.List().reserve(listCopy.size() + mValue.List().size());
                for (auto& entry: mValue.List()) {
                    element.List().push_back(entry);
                }
                for (auto&& entry: listCopy) {
                    element.List().push_back(std::move(entry));
                }
            }
                break;
            default:
                break;
        }
    }
}

void AppendModifier::process(Atlas::Message::Element& element, const Atlas::Message::Element& baseValue)
{
    if (element.isNone() || element.getType() != mValue.getType()) {
        element = mValue;
    } else {
        switch (mValue.getType()) {
            case Atlas::Message::Element::TYPE_INT:
                element = element.Int() + mValue.Int();
                break;
            case Atlas::Message::Element::TYPE_FLOAT:
                element = element.Float() + mValue.Float();
                break;
            case Atlas::Message::Element::TYPE_STRING:
                element = element.String() + mValue.String();
                break;
            case Atlas::Message::Element::TYPE_MAP:
                //Overwrite element entries; with C++17 we can use "insert_or_assign".
                for (const auto& entry: mValue.Map()) {
                    element.Map()[entry.first] = entry.second;
                }
                break;
            case Atlas::Message::Element::TYPE_LIST:
                element.List().reserve(element.List().size() + mValue.List().size());
                for (auto& entry: mValue.List()) {
                    element.List().push_back(entry);
                }
                break;
            default:
                break;
        }

    }
}

void SubtractModifier::process(Atlas::Message::Element& element, const Atlas::Message::Element& baseValue)
{
    switch (mValue.getType()) {
        case Atlas::Message::Element::TYPE_INT:
            if (element.isNone() || element.getType() != mValue.getType()) {
                element = 0 - mValue.Int();
            } else {
                element = element.Int() - mValue.Int();
            }
            break;
        case Atlas::Message::Element::TYPE_FLOAT:
            if (element.isNone() || element.getType() != mValue.getType()) {
                element = 0 - mValue.Float();
            } else {
                element = element.Float() - mValue.Float();
            }
            break;
        case Atlas::Message::Element::TYPE_STRING:
            //There's no one obvious way of effecting one string from another, so we'll just skip this
            break;
        case Atlas::Message::Element::TYPE_MAP:
            if (element.isNone() || element.getType() != mValue.getType()) {
                element = Atlas::Message::MapType();
            } else {
                //Only act on the map keys
                for (const auto& entry: mValue.Map()) {
                    element.Map().erase(entry.first);
                }
            }
            break;
        case Atlas::Message::Element::TYPE_LIST: {
            if (element.isNone() || element.getType() != mValue.getType()) {
                element = Atlas::Message::ListType();
            } else {
                for (auto& entry: mValue.List()) {
                    Atlas::Message::ListType::iterator I;
                    //Delete all instances from the list.
                    while (true) {
                        I = std::find(std::begin(element.List()), std::end(element.List()), entry);
                        if (I != element.List().end()) {
                            element.List().erase(I);
                        } else {
                            break;
                        }
                    }
                }
            }
            break;
        }
        default:
            break;
    }
}

void MultiplyModifier::process(Atlas::Message::Element& element, const Atlas::Message::Element& baseValue)
{
    switch (mValue.getType()) {
        case Atlas::Message::Element::TYPE_INT:
            if (element.getType() == mValue.getType() && element.getType() == baseValue.getType()) {
                element = element.Int() + (baseValue.Int() * mValue.Int());
            }
            break;
        case Atlas::Message::Element::TYPE_FLOAT:
            if (element.getType() == mValue.getType() && element.getType() == baseValue.getType()) {
                element = element.Float() + (baseValue.Float() * mValue.Float());
            }
            break;
        case Atlas::Message::Element::TYPE_STRING:
        case Atlas::Message::Element::TYPE_MAP:
        case Atlas::Message::Element::TYPE_LIST:
        default:
            break;
    }
}
