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

#include "globals.h"
#include "OperationsDispatcher.h"
#include "compose.hpp"
#include "log.h"
#include <boost/asio/signal_set.hpp>
#include <boost/asio/steady_timer.hpp>
#include "Remotery.h"

namespace {
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
            this_.async_wait([&this_](boost::system::error_code error_in, int signal_nbr) { interactiveSignalsHandler(this_, error_in, signal_nbr); });
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
            this_.async_wait([&this_](boost::system::error_code error_in, int signal_nbr) { daemonSignalsHandler(this_, error_in, signal_nbr); });
        }
    }
}

void MainLoop::run(bool daemon,
                   boost::asio::io_context& io_context,
                   OperationsHandler& operationsHandler,
                   const Callbacks& callbacks,
                   std::chrono::steady_clock::duration& time)
{

    boost::asio::signal_set signalSet(io_context);
    //If we're not running as a daemon we should use the interactive signal handler.
    if (!daemon) {
        //signalSet.add(SIGINT);
        signalSet.add(SIGTERM);
        signalSet.add(SIGHUP);
        signalSet.add(SIGQUIT);

        signalSet.async_wait([&signalSet](boost::system::error_code error, int signal_number) { interactiveSignalsHandler(signalSet, error, signal_number); });
    } else {
        signalSet.add(SIGTERM);

        signalSet.async_wait([&signalSet](boost::system::error_code error, int signal_number) { daemonSignalsHandler(signalSet, error, signal_number); });
    }


    bool soft_exit_in_progress = false;


    //Make sure that the io_context never runs out of work.
    boost::asio::io_context::work work(io_context);
    //This timer is used to wake the io_context when next op needs to be handled.
    boost::asio::steady_timer nextOpTimer(io_context);
    //This timer will set a deadline for any mind persistence during soft exits.
    boost::asio::steady_timer softExitTimer(io_context);

    std::chrono::steady_clock::duration tick_size = std::chrono::milliseconds(10);
    // Loop until the exit flag is set. The exit flag can be set anywhere in
    // the code easily.
    while (!exit_flag) {

        rmt_ScopedCPUSample(MainLoop, 0)

        auto frameStartTime = std::chrono::steady_clock::now();
        auto max_wall_time = std::chrono::milliseconds(8);
        auto op_handling_expiry_time = frameStartTime + tick_size;
        bool nextOpTimeExpired = false;
#if BOOST_VERSION >= 106600
        nextOpTimer.expires_after(tick_size);
#else
        nextOpTimer.expires_from_now(tick_size);
#endif
        nextOpTimer.async_wait([&nextOpTimeExpired](boost::system::error_code ec) {
            if (ec != boost::asio::error::operation_aborted) {
                rmt_ScopedCPUSample(nextOpTimeExpired, 0)
                nextOpTimeExpired = true;
            }
        });

        time += tick_size;

        //Dispatch any incoming messages first
        {
            rmt_ScopedCPUSample(dispatchOperations, 0)
            callbacks.dispatchOperations();
        }
        {
            rmt_ScopedCPUSample(processOps, 0)
            operationsHandler.processUntil(time, max_wall_time);
        }
        {
            rmt_ScopedCPUSample(runIO, 0)

            do {
                try {
                    rmt_ScopedCPUSample(runIO_one, 0)
                    io_context.run_one();
                } catch (const std::exception& ex) {
                    log(ERROR, String::compose("Exception caught in main loop: %1", ex.what()));
                }
            } while (!nextOpTimeExpired && std::chrono::steady_clock::now() < op_handling_expiry_time);
        }
        nextOpTimer.cancel();
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
#if BOOST_VERSION >= 106600
                softExitTimer.expires_after(duration);
#else
                softExitTimer.expires_from_now(duration);
#endif
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
    }
    // exit flag has been set so we close down the databases, and indicate
    // to the metaserver (if we are using one) that this server is going down.
    // It is assumed that any preparation for the shutdown that is required
    // by the game has been done before exit flag was set.
    log(NOTICE, "Performing clean shutdown...");


    signalSet.cancel();
    signalSet.clear();


}
