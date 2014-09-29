/*
 Copyright (C) 2014 Erik Ogenvik

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
#ifndef STUBBASEMIND_H_
#define STUBBASEMIND_H_


BaseMind::BaseMind(const std::string & id, long intId) :
          MemEntity(id, intId), m_map(m_script)
{
}

BaseMind::~BaseMind()
{
}

void BaseMind::SightOperation(const Operation & op, OpVector & res)
{
}

void BaseMind::SoundOperation(const Operation & op, OpVector & res)
{
}

void BaseMind::AppearanceOperation(const Operation & op, OpVector & res)
{
}

void BaseMind::DisappearanceOperation(const Operation & op, OpVector & res)
{
}

void BaseMind::UnseenOperation(const Operation & op, OpVector & res)
{
}

void BaseMind::operation(const Operation & op, OpVector & res)
{
}


#endif /* STUBBASEMIND_H_ */
