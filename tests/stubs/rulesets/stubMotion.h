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
#ifndef STUBMOTION_H_
#define STUBMOTION_H_


Motion::Motion(LocatedEntity & body) : m_entity(body), m_serialno(0),
                                m_collision(false)
{
}

Motion::~Motion()
{
}

float Motion::checkCollisions(Domain&)
{
    return consts::move_tick;
}

bool Motion::resolveCollision()
{
    return true;
}

void Motion::setMode(const std::string & mode)
{
}

void Motion::adjustPostion()
{
}

Operation * Motion::genUpdateOperation()
{
    return 0;
}

Operation * Motion::genMoveOperation()
{
    return 0;
}


#endif /* STUBMOTION_H_ */
