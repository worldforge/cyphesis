/* This file may be redistributed and modified only under the terms of
 * the GNU General Public License (See COPYING for details).
 * Copyright (C) 2000 Alistair Riddoch */

#include <string>
#include <fstream>

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <syslog.h>
#include <sys/wait.h>
#include <errno.h>

// #define _NO_DAEMON

std::string bin_dir = BINDIR;
std::string var_dir = VARDIR;

typedef enum server_state {
    SVR_PRE,			// Server not started yet
    SVR_INIT,			// Server initialising
    SVR_START,			// Server started
    SVR_CLIENT,			// Server init client running
    SVR_CLIENT_STALLED,		// Server init client defunct
    SVR_RUNNING,		// Server init client finished
    SVR_OBSOLETE_CLIENT,	// Server not required, client still running
    SVR_OBSOLETE,		// Server not required
    SVR_TERM,			// Server terminated
    SVR_KILLED			// Server gone
} s_state_t;

typedef struct server_info {
    int        s_pid;
    int        s_stime;
    int        s_cpid;
    int        s_ctime;
    s_state_t  s_state;
    int        s_uptime;
    bool       s_restart;
} server_t;

int last_signal;

server_t server = { -1, 0, -1, 0, SVR_PRE, 0, false };

void finish(int val)
{
    closelog();
    unlink((var_dir + "/tmp/cywatchdog.pid").c_str());
    exit(val);
}

int wait30(int pid)
{
    int ret;
    /* This is a poor substitute as it frequently blocks for 30
     * seconds on a process which has not quite exited when the
     * first wait4() call is made */
    ret = wait4(pid, NULL, WNOHANG, NULL);
    if (ret != 0) {
        return ret;
    }
    sleep(30);
    ret = wait4(pid, NULL, WNOHANG, NULL);
    return ret;
}

int server_wait()
{
    return wait30(server.s_pid);
}

int client_wait()
{
    return wait30(server.s_cpid);
}

void client_signal(int signo)
{
#ifdef _NO_DAEMON
    printf("Killing client %d with %d\n", server.s_cpid, signo);
    fstd::flush(stdout);
#endif
    kill(server.s_cpid, signo);
}

void server_signal(int signo)
{
    switch(server.s_state) {
        case SVR_START:
        case SVR_CLIENT:
        case SVR_CLIENT_STALLED:
        case SVR_RUNNING:
        case SVR_OBSOLETE_CLIENT:
        case SVR_OBSOLETE:
        case SVR_TERM:
#ifdef _NO_DAEMON
            printf("Killing server %d with %d\n", server.s_pid, signo); fstd::flush(stdout);
#endif
            kill(server.s_pid, signo);
        default:
            break;
    }
}

void server_term() { server_signal(SIGTERM); }
void server_kill() { server_signal(SIGKILL); }

int client_start()
{
    int new_pid = fork();
    if (new_pid < 0) {
        syslog(LOG_CRIT, "fork(2) failed, sleeping for 30 seconds");
        sleep(30);
        return -1;
    } else if (new_pid > 0) {
        server.s_cpid = new_pid;
        server.s_state = SVR_CLIENT;
        syslog(LOG_INFO, "Client started with with pid %d", new_pid);
        server.s_ctime = 0;
        return new_pid;
    }
    std::string binary = bin_dir + "/cyclient";
    execlp(binary.c_str(), binary.c_str(), NULL);
    perror("exec");
    syslog(LOG_CRIT, "Client exec failed");
    exit(-1);
}

int server_start()
{
    int new_pid = fork();
    char * args[2];
    if (new_pid < 0) {
        syslog(LOG_CRIT, "fork(2) failed, sleeping for 30 seconds");
        sleep(30);
        return -1;
    } else if (new_pid > 0) {
        server.s_pid = new_pid;
        server.s_state = SVR_START;
        syslog(LOG_INFO, "Server started with with pid %d", new_pid);
        server.s_stime = 0;
        server.s_uptime = 0;
        return new_pid;
    }

    std::string binary = bin_dir + "/cyphesis";
    args[0] = (char *)binary.c_str();
    args[1] = "--cyphesis:daemon=false";
    args[2] = NULL;
    execve(binary.c_str(), args, NULL);
    syslog(LOG_CRIT, "server exec failed");
    exit(-1);
}

void signal_hup(int signo)
{
    /* Terminate server and re-run */
    syslog(LOG_NOTICE, "Re-starting server");
    server.s_restart = true;
    last_signal = signo;
    if (server.s_state == SVR_CLIENT) {
        server.s_state = SVR_OBSOLETE_CLIENT;
    } else {
        server.s_state = SVR_OBSOLETE;
    }
    signal(SIGHUP, signal_hup);
    /* Verify server down, then restart */
}

void signal_term(int signo)
{
    /* Terminate server and exit */
    signal(SIGTERM, SIG_IGN);
    server.s_restart = false;
    syslog(LOG_NOTICE, "Shutting server down");
    if (server.s_state == SVR_CLIENT) {
        server.s_state = SVR_OBSOLETE_CLIENT;
    } else {
        server.s_state = SVR_OBSOLETE;
    }
    signal(SIGTERM, signal_term);
}

void signal_user(int signo)
{
    /* Restart the client */
    signal(SIGUSR1, SIG_IGN);
    syslog(LOG_NOTICE, "Re-initialising server");
    last_signal = signo;
    if (server.s_state == SVR_CLIENT) {
        server.s_state = SVR_CLIENT_STALLED;
    } else if (server.s_state == SVR_RUNNING) {
        server.s_state = SVR_START;
    }
    signal(SIGUSR1, signal_user);
}

int main(int argc, char ** argv)
{
    int pid;
#ifndef _NO_DAEMON
    if (fork()) exit(0);
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    setsid();
#endif
    openlog("WorldForge Cyphesis Watchdog", LOG_PID, LOG_DAEMON);
    syslog(LOG_INFO, "Server monitor started");
    std::ofstream pid_file((var_dir + "/tmp/cywatchdog.pid").c_str());
    pid_file << getpid() << std::flush;
    pid_file.close();
    if (false) {
        syslog(LOG_ERR, "Failed to write pid file: %s. control program won't work", strerror(errno));
    }
    signal(SIGHUP, signal_hup);
    signal(SIGTERM, signal_term);
    signal(SIGUSR1, signal_user);
    for(;;) {
#ifdef _NO_DAEMON
        printf("TICK %d\n", server.s_state); fflush(stdout);
#endif
        switch(server.s_state) {
            case SVR_PRE:		// not started yet    */
                server_start();
                break;
            case SVR_INIT:		// initialising        */
                server.s_stime++;
                if (server.s_stime > 2) {
                    syslog(LOG_WARNING, "Server failed to go into background");
                    server.s_state = SVR_START;
                }
                break;
            case SVR_START:		// started        */
                client_start();
                break;
            case SVR_CLIENT:		// init client running    */
                server.s_ctime++;
                if (server.s_ctime > 5) {
                    syslog(LOG_WARNING, "Client failed to terminate");
                    client_signal(SIGTERM);
                    if (client_wait() < 1) {
                        client_signal(SIGKILL);
                        client_wait();
                    }
                    server.s_state = SVR_RUNNING;
                }
                break;
            case SVR_CLIENT_STALLED:		// init client stalled
                client_signal(SIGTERM);
                if (client_wait() < 1) {
                    client_signal(SIGKILL);
                    client_wait();
                }
                server.s_state = SVR_START;
                break;
            case SVR_RUNNING:		// init client finished
                if (server.s_uptime > 2880) {
                    server.s_restart = true;
                    server.s_state = SVR_OBSOLETE;
                }
                break;
            case SVR_OBSOLETE_CLIENT:
                client_signal(SIGTERM);
                if (client_wait() < 1) {
                    client_signal(SIGKILL);
                    client_wait();
                }
                server.s_state = SVR_OBSOLETE;
                break;
            case SVR_OBSOLETE:		// server needs to be killed
                server_term();
                server_wait();
                server.s_state = SVR_TERM;
                break;
            case SVR_TERM:		// terminated
                server_kill();
                server_wait();
                if (server.s_restart) {
                    server.s_state = SVR_PRE;
                } else {
                    server.s_state = SVR_KILLED;
                }
                break;
            case SVR_KILLED:		// gone
                finish(0);
                break;
            default:
                break;
        }
        pid = wait30(-1);
        if (pid == server.s_pid) {
            /* It is actually possible to report why */
            syslog(LOG_ERR, "Server terminated unexpectedly");
            syslog(LOG_NOTICE, "Server re-starting");
            server.s_state = SVR_PRE;
            sleep(30);
        } else if (pid == server.s_cpid) {
            server.s_state = SVR_RUNNING;
            sleep(30);
        } else {
            server.s_uptime++;
        }
    }
}
