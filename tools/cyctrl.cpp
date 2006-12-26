// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

// $Id: cyctrl.cpp,v 1.6 2006-12-26 18:24:25 alriddoch Exp $

#error This file has been removed from the build
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <string>

void usage(char ** argv)
{
    fprintf(stderr, "Usage: %s {start|stop|kill|restart|reinit}\n",
        argv[0]);
    fprintf(stderr, "   or: %s --help\n", argv[0]);
    
}

typedef enum ctask {CY_START, CY_STOP, CY_KILL, CY_RESTART, CY_REINIT } ctask_t;

std::string bin_dir = BINDIR;
std::string var_dir = VARDIR;


void daemon_kill(int pid, int signo)
{
    int ret = kill(pid, signo);
    std::string pid_filename = var_dir + "/tmp/cywatchdog.pid";

    if (ret == 0) {
        return;
    }
    if (errno == ESRCH) {
        fprintf(stderr, "Server pid %d does not exist. Remving pid file.\n", pid);
        unlink(pid_filename.c_str());
        exit(1);
    }
    fprintf(stderr, "Unable to kill server pid %d.\n", pid);
    perror("kill");
}

int daemon_start()
{
    int new_pid = fork();
    if (new_pid < 0) {
        perror("fork");
        return -1;
    } else if (new_pid > 0) {
        return new_pid;
    }
    std::string binary = bin_dir + "/cywatchdog";
    execlp(binary.c_str(), binary.c_str(), NULL);
    perror("exec");
    exit(-1);
}

int main(int argc, char ** argv)
{
    FILE * pid_file;
    int ret;
    ctask_t job;
    int server_pid = -1;
    if ((argc != 2) || (strcmp(argv[1], "--help") == 0)) {
        usage(argv);
        return 1;
    }
    if (strcmp(argv[1], "start") == 0) {
        job = CY_START;
    } else if (strcmp(argv[1], "stop") == 0) {
        job = CY_STOP;
    } else if (strcmp(argv[1], "kill") == 0) {
        job = CY_KILL;
    } else if (strcmp(argv[1], "restart") == 0) {
        job = CY_RESTART;
    } else if (strcmp(argv[1], "reinit") == 0) {
        job = CY_REINIT;
    } else {
        usage(argv);
        return 1;
    }
    std::string pid_filename = var_dir + "/tmp/cywatchdog.pid";

    pid_file = fopen(pid_filename.c_str(), "r");
    if (pid_file == NULL) {
        if (job != CY_START) {
            fprintf(stderr,"%s: Could not open cyphesis daemon pid file.\n",
                argv[0]);
            fprintf(stderr,"Either cyphesis daemon is not running, or an error occured.\n");
            return 1;
        }
    } else {
        if (job == CY_START) {
            fprintf(stderr,"%s: Pid file detected. daemon seems to be already running\n",
                argv[0]);
            return 1;
        }
        ret = fscanf(pid_file, "%d", & server_pid);
        if (ret != 1) {
            fprintf(stderr,"%s: cyphesis daemon pid file corrupt.\n", argv[0]);
            return 1;
        }
    }
    if (job == CY_START) {
        daemon_start();
    } else if (job == CY_STOP) {
        daemon_kill(server_pid, SIGTERM);
    } else if (job == CY_KILL) {
        daemon_kill(server_pid, SIGKILL);
    } else if (job == CY_RESTART) {
        daemon_kill(server_pid, SIGHUP);
    } else if (job == CY_REINIT) {
        daemon_kill(server_pid, SIGUSR1);
    } else {
        return 1;
    }
    return 0;
}
