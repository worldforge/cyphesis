/* This file may be redistributed and modified only under the terms of
 * the GNU General Public License (See COPYING for details).
 * Copyright (C) 2000 Alistair Riddoch */

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <syslog.h>
#include <sys/wait.h>
#include <errno.h>

#include <common/config.h>

/* #define _NO_DAEMON */

char * inst_dir = INSTALLDIR;

typedef enum server_state {
	SVR_PRE,		/* Server not started yet	*/
	SVR_START,		/* Server started		*/
	SVR_SCRIPT,		/* Server init script running	*/
	SVR_RUNNING,		/* Server init script finished	*/
	SVR_OBSOLETE,		/* Server internal state dead	*/
	SVR_FAULT,		/* Server internal state faulty	*/
	SVR_TERM,		/* Server terminated		*/
	SVR_KILLED		/* Server gone			*/
} s_state_t;

typedef struct server_info {
	int		s_pid;
	int		s_spid;
	int		s_stime;
	s_state_t	s_state;
	int		s_uptime;
} server_t;

int last_signal;

server_t server = { -1, -1, SVR_PRE, 0 };

void finish(int val)
{
	closelog();
	unlink("/var/tmp/cyphesisd.pid");
	exit(val);
}

int wait30(int pid)
{
	int ret;
#ifdef USE_ALARM
	/* I can't get this to work. The alarm signal does not seem to make
	 * the wait4() system call return */
	alarm(30);
	printf("Alarm set\n"); fflush(stdout);
	ret = wait4(pid, NULL, 0, NULL);
	printf("Done waiting\n"); fflush(stdout);
	alarm(0);
	printf("Alarm cleared\n"); fflush(stdout);
#else
	/* This is a poor substitute as it frequently blocks for 30
	 * seconds on a process which has not quite exited when the
	 * first wait4() call is made */
	ret = wait4(pid, NULL, WNOHANG, NULL);
	if (ret != 0) {
		return ret;
	}
	sleep(30);
	ret = wait4(pid, NULL, WNOHANG, NULL);
#endif
	return ret;
}

int server_wait()
{
	return wait30(server.s_pid);
}

int script_wait()
{
	return wait30(server.s_spid);
}

void script_signal(int signo)
{
	printf("Killing script %d with %d\n", server.s_spid, signo); fflush(stdout);
	kill(server.s_spid, signo);
}

void server_signal(int signo)
{
	switch(server.s_state) {
		case SVR_START:
		case SVR_SCRIPT:
		case SVR_RUNNING:
		case SVR_OBSOLETE:
		case SVR_FAULT:
		case SVR_TERM:
			printf("Killing server %d with %d\n", server.s_pid, signo); fflush(stdout);
			kill(server.s_pid, signo);
		default:
			break;
	}
}

void server_term() { server_signal(SIGTERM); }
void server_kill() { server_signal(SIGKILL); }

int script_start()
{
	int new_pid = fork();
	char * binary;
	if (new_pid < 0) {
		syslog(LOG_CRIT, "fork(2) failed, sleeping for 30 seconds");
		sleep(30);
		return -1;
	} else if (new_pid > 0) {
		server.s_spid = new_pid;
		server.s_state = SVR_SCRIPT;
		syslog(LOG_INFO, "Script started with with pid %d", new_pid);
		server.s_stime = 0;
		return new_pid;
	}
	binary = malloc(strlen(inst_dir) + 15);
	if (binary == NULL) {
		syslog(LOG_CRIT, "malloc failed");
		exit(1);
	}
	strcpy(binary, inst_dir);
	strcat(binary, "/bin/acorn");
	execlp(binary, binary, NULL);
	perror("exec");
	syslog(LOG_CRIT, "script exec failed");
	exit(-1);
}

int server_start()
{
	int new_pid = fork();
	char * binary;
	char * args[2];
	if (new_pid < 0) {
		syslog(LOG_CRIT, "fork(2) failed, sleeping for 30 seconds");
		sleep(30);
		return -1;
	} else if (new_pid > 0) {
		server.s_pid = new_pid;
		server.s_state = SVR_START;
		syslog(LOG_INFO, "Server started with with pid %d", new_pid);
		server.s_uptime = 0;
		return new_pid;
	}
	binary = malloc(strlen(inst_dir) + 15);
	if (binary == NULL) {
		syslog(LOG_CRIT, "malloc failed");
		exit(1);
	}
	strcpy(binary, inst_dir);
	strcat(binary, "/bin/cyphesis");
	args[0] = binary;
	args[1] = NULL;
	execve(binary, args, NULL);
	syslog(LOG_CRIT, "server exec failed");
	exit(-1);
}

void signal_hup(int signo)
{
	/* Terminate server and re-run */
	syslog(LOG_NOTICE, "Re-starting server");
	last_signal = signo;
	if (server.s_state==SVR_SCRIPT) {
		script_signal(SIGTERM);
		if (script_wait() < 1) {
			script_signal(SIGKILL);
			script_wait();
		}
	}
	server_term();
	if (server_wait() == -1) {
		server_kill();
		server_wait();
	}
	server.s_state = SVR_PRE;
        signal(SIGHUP, signal_hup);
	/* Verify server down, then restart */
}

void signal_term(int signo)
{
	/* Terminate server and exit */
	syslog(LOG_NOTICE, "Shutting server down");
	last_signal = signo;
	if (server.s_state==SVR_SCRIPT) {
		script_signal(SIGTERM);
		if (script_wait() < 1) {
			script_signal(SIGKILL);
			script_wait();
		}
	}
	server_term();
	server.s_state = SVR_TERM;
	server_wait();
	server_kill();
	server.s_state = SVR_KILLED;
	finish(0);
}

void signal_user(int signo)
{
	/* Restart the script */
	syslog(LOG_NOTICE, "Re-initialising server");
	last_signal = signo;
	if (server.s_state==SVR_SCRIPT) {
		script_signal(SIGTERM);
		if (script_wait() < 1) {
			script_signal(SIGKILL);
			script_wait();
		}
	}
	if ((server.s_state==SVR_SCRIPT)||(server.s_state==SVR_RUNNING)) {
		server.s_state = SVR_START;
	}
        signal(SIGUSR1, signal_user);
}

void signal_alarm(int signo)
{
	printf("ALARM"); fflush(stdout);
	last_signal = signo;
        signal(SIGALRM, signal_alarm);
}

int main(int argc, char ** argv)
{
	int pid;
	sig_t ret;
	FILE * pid_file;
#ifndef _NO_DAEMON
	if (fork()) exit(0);
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
        setpgrp();
#endif
	if (strcmp(inst_dir, "NONE") == 0) {
		inst_dir = "/usr/local";
	}
	openlog("WorldForge Cyphesis", LOG_PID, LOG_DAEMON);
	syslog(LOG_INFO, "Server monitor started");
	pid_file = fopen("/var/tmp/cyphesisd.pid", "w");
	if (pid_file != NULL) {
		fprintf(pid_file, "%d", getpid());
		fclose(pid_file);
	} else {
		syslog(LOG_ERR, "Failed to write pid file: %s. control program won't work", strerror(errno));
	}
        signal(SIGHUP, signal_hup);
        signal(SIGTERM, signal_term);
        ret = signal(SIGALRM, signal_alarm);
	printf("Alarm %s\n", (ret == SIG_ERR) ? "error" : "set");
        signal(SIGUSR1, signal_user);
	/* pid = server_start(); */
        for(;;) {
		printf("TICK %d\n", server.s_state); fflush(stdout);
		switch(server.s_state) {
			case SVR_PRE:		/* not started yet	*/
				server_start();
				break;
			case SVR_START:		/* started		*/
				script_start();
				break;
			case SVR_SCRIPT:	/* init script running	*/
				server.s_stime++;
				if (server.s_stime > 5) {
					script_signal(SIGTERM);
					if (script_wait() < 1) {
						script_signal(SIGKILL);
						script_wait();
					}
					server.s_state = SVR_RUNNING;
				}
				break;
			case SVR_RUNNING:	/* init script finished	*/
				if (server.s_uptime > 2880) {
					signal_hup(-1);
				}
			case SVR_OBSOLETE:	/* internal state dead	*/
			case SVR_FAULT:		/* internal state fault	*/
			default:
				break;
			case SVR_TERM:		/* terminated		*/
			case SVR_KILLED:	/* gone			*/
				finish(0);
				break;
		}
		pid = wait30(-1);
		if (pid == server.s_pid) {
			/* It is actually possible to report why */
			syslog(LOG_ERR, "Server terminated unexpectedly");
			syslog(LOG_NOTICE, "Server re-starting");
			server.s_state = SVR_PRE;
			sleep(30);
		} else if (pid == server.s_spid) {
			syslog(LOG_ERR, "Script terminated unexpectedly");
			syslog(LOG_NOTICE, "Script re-starting");
			server.s_state = SVR_START;
			sleep(30);
		} else {
			server.s_uptime++;
		}
	}
}
