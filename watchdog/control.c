// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include <stdio.h>
#include <signal.h>
#include <errno.h>

#include "config.h"

usage(char ** argv)
{
	fprintf(stderr, "Usage: %s {start|stop|kill|restart|reinit}\n",
		argv[0]);
	fprintf(stderr, "   or: %s --help\n", argv[0]);
	
}

typedef enum ctask {CY_START, CY_STOP, CY_KILL, CY_RESTART, CY_REINIT } ctask_t;

char * inst_dir = INSTALLDIR;
char * pid_filename = "/var/tmp/cyphesisd.pid";

void daemon_kill(int pid, int signo)
{
	int ret = kill(pid, signo);
	if (ret == 0) {
		return;
	}
	if (errno == ESRCH) {
		fprintf(stderr, "Server pid %d does not exist. Remving pid file.\n", pid);
		unlink(pid_filename);
		exit(1);
	}
	fprintf(stderr, "Unable to kill server pid %d.\n", pid);
	perror("kill");
}

void daemon_start()
{
}

int main(int argc, char ** argv)
{
	FILE * pid_file;
	int ret;
	ctask_t job;
	int server_pid = -1;
	if ((argc != 2) || (strcmp(argv[1], "--help") == 0)) {
		usage(argv);
		exit(1);
	}
        if (strcmp(inst_dir, "NONE") == 0) {
                inst_dir = "/usr/local";
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
		exit(1);
	}
	pid_file = fopen(pid_filename, "r");
	if (pid_file == NULL) {
		if (job != CY_START) {
			fprintf(stderr,"%s: Could not open cyphesis daemon pid file.\n",
				argv[0]);
			fprintf(stderr,"Either cyphesis daemon is not running, or an error occured.\n");
			exit(1);
		}
	} else {
		if (job == CY_START) {
			fprintf(stderr,"%s: Pid file detected. daemon seems to be already running\n",
				argv[0]);
			exit(1);
		}
		ret = fscanf(pid_file, "%d", & server_pid);
		if (ret != 1) {
			fprintf(stderr,"%s: cyphesis daemon pid file corrupt.\n",
				argv[0]);
			exit(1);
		}
	}
	if (job == CY_START) {
		daemon_start();
		exit(0);
	} else if (job == CY_STOP) {
		daemon_kill(server_pid, SIGTERM);
		exit(0);
	} else if (job == CY_KILL) {
		daemon_kill(server_pid, SIGKILL);
		exit(0);
	} else if (job == CY_RESTART) {
		daemon_kill(server_pid, SIGHUP);
		exit(0);
	} else if (job == CY_REINIT) {
		daemon_kill(server_pid, SIGUSR1);
		exit(0);
	}
}
