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

#ifndef CYPHESIS_REPEATEDTASK_H
#define CYPHESIS_REPEATEDTASK_H


#include <boost/asio/io_context.hpp>
#include <boost/asio/deadline_timer.hpp>

/**
 * Repeats a task with a specified interval.
 */
class RepeatedTask
{
    public:
        RepeatedTask(boost::asio::io_context& io_context, boost::posix_time::time_duration interval, std::function<void()> function);

    private:
        boost::posix_time::time_duration mInterval;
        boost::asio::deadline_timer mTimer;
        std::function<void()> mFunction;

        void executeTask();

};


#endif //CYPHESIS_REPEATEDTASK_H
