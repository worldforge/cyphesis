/*
 Copyright (C) 2013 Erik Ogenvik

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

#ifndef PROXYMIND_H_
#define PROXYMIND_H_

#include "BaseMind.h"

#include <Atlas/Objects/ObjectsFwd.h>

#include <vector>

/**
 * Connected to a Character which has or should have an external mind.
 *
 * The proxy mind is responsible for keeping tab of any thoughts sent to the character.
 * This is mainly of importance when the character haven't yet obtained an external mind.
 * In that case it's important that the proxy mind keeps tab of the thoughts specified, so that
 * these can be sent to the external mind once it's connected.
 *
 * That way any world author can send thoughts to entities for which there aren't any external mind
 * connected yet.
 *
 * The ProxyMind is an instance of BaseMind. That means that by default it will register everything that
 * happens around the character. The idea is that any external mind then could upon connection ask the
 * proxy mind about what it knows (i.e. thus given knowledge about recent events rather than just what's
 * just in the current vicinity).
 */
class ProxyMind : public BaseMind
{
    public:
        ProxyMind(const std::string & id, long intId);
        virtual ~ProxyMind();

        virtual void operation(const Operation & op, OpVector & res);

        /**
         * Gets all registered thoughts.
         * @return All registered thoughts.
         */
        const std::vector<Atlas::Objects::Operation::RootOperation> getThoughts() const;

        /**
         * Clear all registered thoughts.
         */
        void clearThoughts();

    private:
        std::vector<Atlas::Objects::Operation::RootOperation> m_thoughts;
};

#endif /* PROXYMIND_H_ */
