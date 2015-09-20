/*
 Copyright (C) 2015 erik

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
#ifndef AICLIENT_AWAREMIND_H_
#define AICLIENT_AWAREMIND_H_

#include "rulesets/BaseMind.h"

class Awareness;
class AwarenessStoreProvider;
class Steering;

class AwareMind : public BaseMind
{
    public:
        AwareMind(const std::string &id, long intId, const AwarenessStoreProvider& awarenessStoreProvider);
        virtual ~AwareMind();

    protected:

        const AwarenessStoreProvider& mAwarenessStoreProvider;
        Awareness* mAwareness;
        Steering* mSteering;
};

#endif /* AICLIENT_AWAREMIND_H_ */
