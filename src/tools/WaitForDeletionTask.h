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
#ifndef WAITFORDELETIONTASK_H_
#define WAITFORDELETIONTASK_H_

#include "common/ClientTask.h"

/**
 * A task which waits until we've received a sight of the specified entity being deleted.
 */
class WaitForDeletionTask: public ClientTask
{
    public:
        explicit WaitForDeletionTask(const std::string& entityId);

        /// \brief Set up the task processing user arguments
        void setup(const std::string & arg, OpVector &) override;
        /// \brief Handle an operation from the server
        void operation(const Operation &, OpVector &) override;

    protected:

        const std::string m_entityId;

};

#endif /* WAITFORDELETIONTASK_H_ */
