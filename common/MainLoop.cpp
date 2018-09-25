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

#include "MainLoop.h"

#include "common/system.h"
#include "globals.h"
#include "OperationsDispatcher.h"
#include "compose.hpp"
#include "log.h"
#include <boost/asio/signal_set.hpp>
#include <boost/asio/deadline_timer.hpp>

void interactiveSignalsHandler(boost::asio::signal_set& this_, boost::system::error_code error, int signal_number)
{
    if (!error) {
        switch (signal_number) {
            case SIGINT:
            case SIGTERM:
            case SIGHUP:
                //If we've already received one call to shut down softly we should elevate
                //it to a hard shutdown.
                //This also happens if "soft" exit isn't enabled.
                if (exit_flag_soft || !exit_soft_enabled) {
                    exit_flag = true;
                } else {
                    exit_flag_soft = true;
                }
                break;
            case SIGQUIT:
                exit_flag = true;
                break;
            default:
                break;
        }
        this_.async_wait(std::bind(interactiveSignalsHandler, std::ref(this_), std::placeholders::_1, std::placeholders::_2));
    }
}

void daemonSignalsHandler(boost::asio::signal_set& this_, boost::system::error_code error, int signal_number)
{
    if (!error) {
        switch (signal_number) {
            case SIGTERM:
                //If we've already received one call to shut down softly we should elevate
                //it to a hard shutdown.
                //This also happens if "soft" exit isn't enabled.
                if (exit_flag_soft || !exit_soft_enabled) {
                    exit_flag = true;
                } else {
                    exit_flag_soft = true;
                }
                break;
            default:
                break;
        }
        this_.async_wait(std::bind(daemonSignalsHandler, std::ref(this_), std::placeholders::_1, std::placeholders::_2));
    }
}

void MainLoop::run(bool daemon, boost::asio::io_service& io_service, OperationsHandler& operationsHandler, const Callbacks& callbacks)
{

    boost::asio::signal_set signalSet(io_service);
    //If we're not running as a daemon we should use the interactive signal handler.
    if (!daemon) {
        signalSet.add(SIGINT);
        signalSet.add(SIGTERM);
        signalSet.add(SIGHUP);
        signalSet.add(SIGQUIT);

        signalSet.async_wait(std::bind(interactiveSignalsHandler, std::ref(signalSet), std::placeholders::_1, std::placeholders::_2));
    } else {
        signalSet.add(SIGTERM);

        signalSet.async_wait(std::bind(daemonSignalsHandler, std::ref(signalSet), std::placeholders::_1, std::placeholders::_2));
    }


    bool soft_exit_in_progress = false;


    //Make sure that the io_service never runs out of work.
    boost::asio::io_service::work work(io_service);
    //This timer is used to wake the io_service when next op needs to be handled.
    boost::asio::deadline_timer nextOpTimer(io_service);
    //This timer will set a deadline for any mind persistence during soft exits.
    boost::asio::deadline_timer softExitTimer(io_service);
    // Loop until the exit flag is set. The exit flag can be set anywhere in
    // the code easily.
    while (!exit_flag) {
        try {
            bool busy = operationsHandler.idle(10);
            operationsHandler.markQueueAsClean();
            //If the world is busy we should just poll.
            if (busy) {
                io_service.poll();
            } else {
                //If it's not busy however we should run until we get a task.
                //We will either get an io task, or we will be triggered by the timer
                //which is set to expire when the next op should be dispatched.
                double secondsUntilNextOp = operationsHandler.secondsUntilNextOp();
                if (secondsUntilNextOp <= 0.0) {
                    io_service.poll();
                } else {
                    bool nextOpTimeExpired = false;
                    boost::posix_time::microseconds waitTime((int64_t)(secondsUntilNextOp * 1000000L));
                    nextOpTimer.expires_from_now(waitTime);
                    nextOpTimer.async_wait([&](boost::system::error_code ec) {
                        if (ec != boost::asio::error::operation_aborted) {
                            nextOpTimeExpired = true;
                        }
                    });
                    //Keep on running IO handlers until either the queue is dirty (i.e. we need to handle
                    //any new operation) or the timer has expired.
                    do {
                        io_service.run_one();
                    } while (!operationsHandler.isQueueDirty() && !nextOpTimeExpired &&
                             !exit_flag_soft && !exit_flag && !soft_exit_in_progress);
                    nextOpTimer.cancel();
                }
            }
            if (soft_exit_in_progress) {
                //If we're in soft exit mode and either the deadline has been exceeded
                //or we've persisted all minds we should shut down normally.
                if (!callbacks.softExitPoll || callbacks.softExitPoll()) {
                    exit_flag = true;
                    softExitTimer.cancel();
                }
            } else if (exit_flag_soft) {
                exit_flag_soft = false;
                soft_exit_in_progress = true;
                if (callbacks.softExitStart) {
                    auto duration = callbacks.softExitStart();
                    softExitTimer.expires_from_now(duration);
                    softExitTimer.async_wait([&](boost::system::error_code ec) {
                        if (!ec) {
                            if (callbacks.softExitTimeout) {
                                callbacks.softExitTimeout();
                            }
                            exit_flag = true;
                        }
                    });
                }
            }
            // It is hoped that commonly thrown exception, particularly
            // exceptions that can be caused  by external influences
            // should be caught close to where they are thrown. If
            // an exception makes it here then it should be debugged.
        } catch (const std::exception& e) {
            log(ERROR, String::compose("Exception caught in main loop: %1", e.what()));
        } catch (...) {
            log(ERROR, "Exception caught in main()");
        }
    }
    // exit flag has been set so we close down the databases, and indicate
    // to the metaserver (if we are using one) that this server is going down.
    // It is assumed that any preparation for the shutdown that is required
    // by the game has been done before exit flag was set.
    log(NOTICE, "Performing clean shutdown...");



    signalSet.clear();


}
