// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

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
	s_state_t	s_state;
} server_t;

server_t server = { -1, -1, SVR_PRE };

void server_wait()
{
	alarm(30);
	wait4(server.s_pid, NULL, WNOHANG, NULL);
	alarm(0);
}

void server_signal(int signo)
{
	switch(server.s_state) {
		case SVR_START:
		case SVR_SCRIPT:
		case SVR_RUNNING:
		case SVR_OBSOLETE:
		case SVR_FAULT:
			kill(server.s_pid, signo);
		default:
			break;
	}
}

void server_term() { server_signal(SIGTERM); }
void server_kill() { server_signal(SIGKILL); }

void signal_hup(int signo)
{
	server_term();
	/* Verify server down, then restart */
}

void signal_term(int signo)
{
	server_term();
	/* We really want to wait for the server to die here, to see if
	 * it does, rather than waiting blindly
	 */
	sleep(30);
	server_kill();
	exit(0);
}

void signal_alarm(int signo) { }

int main(int argc, char ** argv)
{
	if (fork()) exit(0);
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
        setpgrp();
        signal(SIGHUP, signal_hup);
        signal(SIGTERM, signal_term);
        signal(SIGALRM, signal_alarm);
        for(;;);
}
