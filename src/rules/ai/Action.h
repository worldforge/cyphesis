/*
 Copyright (C) 2018 Erik Ogenvik

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

#ifndef CYPHESIS_ACTION_H
#define CYPHESIS_ACTION_H

#include <vector>
#include <memory>
#include "pycxx/CXX/Objects.hxx"

class DecisionContext
{
    public:
};


struct InputProvider
{
    virtual float getValue(const DecisionContext& context) const = 0;
};

struct ScriptedInputProvider {
    Py::Object m_script;
    float getValue(const DecisionContext& context) const;

};


class ConsiderationInput
{
    public:
        std::unique_ptr<InputProvider> m_provider;

        virtual float getValue(const DecisionContext& context) const = 0;
};

class InputFunction
{
    public:
        virtual float mapInput(float value) const = 0;
};

class CurveInputFunction : public InputFunction
{
    public:
        enum class CurveType
        {
                LINEAR
        };

        CurveInputFunction(CurveType type, float m, float k, float b, float c);

        float mapInput(float value) const override;

};

class Consideration
{
    public:

        std::unique_ptr<ConsiderationInput> m_input;
        std::unique_ptr<InputFunction> m_function;

};


class Action
{
    public:
        std::string description;
        std::vector<Consideration> m_conditionals;

};

class Decision {
    public:
        std::vector<Action> m_actions;


};


#endif //CYPHESIS_ACTION_H
