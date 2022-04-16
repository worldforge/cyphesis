// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2004 Alistair Riddoch
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


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "system.h"

#include "log.h"
#include "globals.h"
#include "compose.hpp"

#include <wfmath/MersenneTwister.h>
#include <gcrypt.h>

#include <iostream>

#include <sys/stat.h>

#include <cassert>
#include <vector>

extern "C" {
#include <sys/wait.h>
#include <fcntl.h>
}


#ifdef ERROR
#undef ERROR
#endif

unsigned int security_init()
{
    gcry_check_version(nullptr);

    return 0;
}

static int security_new_key(const std::string & key_filename)
{
    FILE * key_file = ::fopen(key_filename.c_str(), "wx");

    if (key_file == nullptr) {
        log(CRITICAL, String::compose("Unable to open file %1 to store server"
                                      " identity", key_filename));
        return -1;
    }

    gcry_control( GCRYCTL_INIT_SECMEM, 16384, 0 );

    gcry_sexp_t key_parameters, key;

    gcry_error_t ret = gcry_sexp_build(&key_parameters, nullptr,
                                       "(genkey(dsa(nbits %d)))", 1024);

    if (gcry_err_code(ret) != GPG_ERR_NO_ERROR) {
        std::cout << "SEXP FAIL" << std::endl << std::flush;
    }

    ret = gcry_pk_genkey(&key, key_parameters);

    if (gcry_err_code(ret) != GPG_ERR_NO_ERROR) {
        std::cout << "GENKEY FAIL" << std::endl << std::flush;
        ::fclose(key_file);
        return -1;
    }

    gcry_sexp_release(key_parameters);

    ret = gcry_pk_testkey(key);

    if (gcry_err_code(ret) != GPG_ERR_NO_ERROR) {
        std::cout << "TESTKEY FAIL" << std::endl << std::flush;
        ::fclose(key_file);
        return -1;
    }

    size_t ktxtlen = gcry_sexp_sprint(key, GCRYSEXP_FMT_CANON, nullptr, 0);
    char * key_text = new char[ktxtlen];
    gcry_sexp_sprint(key, GCRYSEXP_FMT_CANON, key_text, ktxtlen);

    if (fwrite(key_text, ktxtlen, 1, key_file) != ktxtlen) {
        log(ERROR, String::compose("Unable to write key to %1.", key_filename));
    }

    fclose(key_file);

    // gcry_sexp_dump(key);
    gcry_sexp_release(key);

    return 0;
}

static int security_load_key(const std::string & key_filename, size_t len)
{
    FILE * key_file = ::fopen(key_filename.c_str(), "r");

    if (key_file == nullptr) {
        log(CRITICAL, String::compose("Unable to open file %1 to read server"
                                      " identity", key_filename));
        perror("ARSE!");
        return -1;
    }

    std::vector<char> key_text;
    key_text.resize(len);

    size_t records = fread(key_text.data(), len, 1, key_file);
    if (records != 1) {
        log(CRITICAL, String::compose("Unable to load identity information"
                                      " from file %1", key_filename));
        return -1;
    }

    gcry_sexp_t key;

    gcry_error_t ret = gcry_sexp_new(&key, key_text.data(), len, 0);

    if (gcry_err_code(ret) != GPG_ERR_NO_ERROR) {
        log(CRITICAL, String::compose("Malformed identity information"
                                      " from file %1", key_filename));
        return -1;
    }

    ret = gcry_pk_testkey(key);

    if (gcry_err_code(ret) != GPG_ERR_NO_ERROR) {
        std::cout << "TESTKEY FAIL" << std::endl << std::flush;
        return -1;
    }

    return 0;
}

unsigned int security_setup()
{
    std::string key_filename;
    char * home = getenv("HOME");
    if (home == nullptr) {
        std::cout << "No home" << std::endl << std::flush;
        key_filename = String::compose("%1/tmp/cyphesis_%2_id_dsa",
                                       var_directory, instance);
    } else {
        std::cout << "home" << std::endl << std::flush;
        key_filename = String::compose("%1/.cyphesis_%2_id_dsa",
                                       home, instance);
    }

    std::cout << "KEY: " << key_filename << std::endl << std::flush;

    struct stat key_stat{};

    if (::stat(key_filename.c_str(), &key_stat) != 0) {
        std::cout << "not yet" << std::endl << std::flush;
        security_new_key(key_filename);
    } else {
        std::cout << "loading" << std::endl << std::flush;
        security_load_key(key_filename, static_cast<size_t>(key_stat.st_size));
    }

    return SECURITY_OKAY;
}


void reduce_priority(int p)
{
    if (nice(p) < 0) {
        log(ERROR, "Unable to increase nice level to reduce priority");
    }
}

namespace {

    extern "C" void shutdown_on_signal(int signo)
    {
        exit_flag = true;

        struct sigaction action{};
        sigemptyset(&action.sa_mask);
        action.sa_flags = 0;
        action.sa_handler = SIG_IGN;
        sigaction(signo, &action, nullptr);
    }

    extern "C" void soft_shutdown_on_signal(int signo)
    {
        //If we've already received one call to shut down softly we should elevate
        //it to a hard shutdown.
        //This also happens if "soft" exit isn't enabled.
        if (exit_flag_soft || !exit_soft_enabled) {
            exit_flag = true;
        } else {
            exit_flag_soft = true;
        }

        struct sigaction action{};
        sigemptyset(&action.sa_mask);
        action.sa_flags = 0;
        action.sa_handler = SIG_IGN;
        sigaction(signo, &action, nullptr);
    }

    extern "C" void report_segfault(int)
    {
        //Don't print to the log at segfault, as that involves memory allocation.
        //And with a segfault we might have gotten a tainted stack, which might cause that call to hang.
        fprintf(stderr, "Segmentation fault");
        fprintf(stderr, "Please report this bug to " PACKAGE_BUGREPORT);
    }

    extern "C" void report_abort(int)
    {
        //Don't print to the log in signal handler, as that involves memory allocation.
        fprintf(stderr, "Aborted");
        fprintf(stderr, "Please report this bug to " PACKAGE_BUGREPORT);
    }

    extern "C" void report_status(int)
    {
        if (exit_flag) {
            log(NOTICE, "Shutting down");
        } else {
            log(NOTICE, "Running");
        }
    }

    extern "C" void rotate_logs(int)
    {
        rotateLogger();
    }
}

void interactive_signals()
{
    struct sigaction action{};

    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    action.sa_handler = soft_shutdown_on_signal;
    sigaction(SIGINT, &action, nullptr);

    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    action.sa_handler = soft_shutdown_on_signal;
    sigaction(SIGTERM, &action, nullptr);

    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    action.sa_handler = shutdown_on_signal;
    sigaction(SIGQUIT, &action, nullptr);

    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    action.sa_handler = soft_shutdown_on_signal;
    sigaction(SIGHUP, &action, nullptr);

    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    action.sa_handler = SIG_IGN;
    sigaction(SIGPIPE, &action, nullptr);

    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_RESETHAND;
    action.sa_handler = report_segfault;
    sigaction(SIGSEGV, &action, nullptr);

    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_RESETHAND;
    action.sa_handler = report_abort;
    sigaction(SIGABRT, &action, nullptr);

#ifdef __APPLE__
#warning Apple
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    action.sa_handler = report_status;
    sigaction(SIGINFO, &action, nullptr);
#endif // __APPLE__
}

void daemon_signals()
{
    struct sigaction action{};

    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    action.sa_handler = SIG_IGN;
    sigaction(SIGINT, &action, nullptr);

    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    action.sa_handler = soft_shutdown_on_signal;
    sigaction(SIGTERM, &action, nullptr);

    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    action.sa_handler = SIG_IGN;
    sigaction(SIGQUIT, &action, nullptr);

    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    action.sa_handler = rotate_logs;
    sigaction(SIGHUP, &action, nullptr);

    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    action.sa_handler = SIG_IGN;
    sigaction(SIGPIPE, &action, nullptr);

    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_RESETHAND;
    action.sa_handler = report_segfault;
    sigaction(SIGSEGV, &action, nullptr);

    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_RESETHAND;
    action.sa_handler = report_abort;
    sigaction(SIGABRT, &action, nullptr);

}

int daemonise()
{
    int pid = fork();
    int new_stdio;
    int status = 0;
    int running = false;

    switch (pid) {
        case 0:
            // Child
            // Get rid of controlling tty, and start new session
            setsid();
            // Switch signal behavoir
            daemon_signals();
            // Change current working directory to /
            if (chdir("/") != 0) {
                log(ERROR, "Unable to change current working directory to /");
            }
            // Get rid if stdio
            close(STDIN_FILENO);
            close(STDOUT_FILENO);
            close(STDERR_FILENO);
            // Open /dev/null on the stdio file descriptors to avoid problems
            new_stdio = open("/dev/null", O_RDWR);
            dup2(new_stdio, STDIN_FILENO);
            dup2(new_stdio, STDOUT_FILENO);
            dup2(new_stdio, STDERR_FILENO);
            break;
        case -1:
            // Error

            // We are not the daemon process
            daemon_flag = false;

            log(ERROR, "Failed to fork() to go to the background.");

            break;
        default:
            // Parent

            // We are not the daemon process
            daemon_flag = false;

            // Install handler for SIGUSR1
            struct sigaction action{};
            sigemptyset(&action.sa_mask);
            action.sa_flags = 0;
            action.sa_handler = soft_shutdown_on_signal;
            sigaction(SIGUSR1, &action, nullptr);

            if (waitpid(pid, &status, 0) < 0) {
                running = true;
            } else {
                pid = -1;
            }

            if (running) {
                log(INFO, "Running");
            } else {
                int estatus = WEXITSTATUS(status);
                if (estatus == EXIT_SUCCESS) {
                    log(ERROR, "Cyphesis exited normally at initialization.");
                } else if (estatus == EXIT_DATABASE_ERROR) {
                    log(ERROR, "Cyphesis was unable to connect to the database.");
                } else if (estatus == EXIT_SOCKET_ERROR) {
                    log(ERROR, "Cyphesis was unable to open a listen socket.");
                } else if (estatus == EXIT_PORT_ERROR) {
                    log(ERROR, "Could not find free client listen socket. "
                               "Init failed.");
                    log(INFO, String::compose("To allocate 8 more ports please"
                                              " run:\n\n    cyconfig "
                                              "--cyphesis:dynamic_port_end=%1"
                                              "\n\n", dynamic_port_end + 8));
                } else {
                    log(ERROR, "Cyphesis exited unexpectedly at initialization.");
                }
                log(ERROR, "See syslog for details.");
            }

            break;
    }
    return pid;

}

void running()
{
    if (daemon_flag) {
        kill(getppid(), SIGUSR1);
    }
}

static const char hex_table[16] = { '0', '1', '2', '3', '4', '5', '6', '7',
                                    '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

static const int hash_algorithm = GCRY_MD_MD5;
static const int hash_salt_size = 8;

void hash_password(const std::string & pwd, const std::string & salt,
                   std::string & hash)
{
    unsigned int digest_length = gcry_md_get_algo_dlen(hash_algorithm);
    std::vector<unsigned char> buf;
    buf.resize(digest_length);

    std::string passwd_and_salt = pwd + salt;

    // Generate an MD% hash of the password and salt concatenated
    gcry_md_hash_buffer(hash_algorithm, buf.data(),
                        (const unsigned char *)passwd_and_salt.c_str(),
                        passwd_and_salt.size());
    // Build a string containing the salt and hash together
    // hash = String::compose("$1$%1$", salt);
    if (!salt.empty()) {
        hash = "$1$";
        hash += salt;
        hash += "$";
    } else {
        hash.clear();
    }
    for(unsigned int i = 0; i < digest_length; ++i) {
        hash.push_back(hex_table[buf[i] & 0xf]);
        hash.push_back(hex_table[(buf[i] & 0xf0) >> 4]);
    }
}

void encrypt_password(const std::string & pwd, std::string & hash)
{
    std::string salt;
    WFMath::MTRand rng;

    // Generate 8 bytes of salt
    for (int i = 0; i < hash_salt_size; ++i) {
        auto b = static_cast<unsigned char>(rng.randInt() & 0xff);
        salt.push_back(hex_table[b & 0xf]);
        salt.push_back(hex_table[(b & 0xf0) >> 4]);
    }

    // Get a hash of password and salt
    hash_password(pwd, salt, hash);
}

int check_password(const std::string & pwd, const std::string & hash)
{
    unsigned int digest_length = gcry_md_get_algo_dlen(hash_algorithm);
    std::string new_hash;
    size_t hash_size = hash.size();

    if (hash_size < (digest_length * 2 + 5) || hash.substr(0, 3) != "$1$") {
        // Get a hash of password with no salt
        hash_password(pwd, "", new_hash);
    } else {
        // Extract the salt from the hash string
        size_t dp = hash.find('$', 3);
        if (dp == std::string::npos) {
            log(CYLOG_ERROR, "Password hash has no $ symbol after the salt.");
            return -1;
        }
        assert(dp > 3);
        std::string salt = hash.substr(3, dp - 3);
        // Get a has of password and salt
        hash_password(pwd, salt, new_hash);
    }
    // Check if the generated hash matches the reference hash
    return hash == new_hash ? 0 : -1;
}

