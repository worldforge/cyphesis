// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2001 Alistair Riddoch
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


/// \page cydb_index
///
/// \section Introduction
///
/// cydb is a commandline tool to administrate the server database. For
/// information on the commands available, please see the unix manual page.
/// The manual page is generated from docbook sources, so can
/// also be converted into other formats.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "common/log.h"
#include "common/globals.h"
#include "common/system.h"
#include "common/Storage.h"

#include <varconf/config.h>

#include "common/compose.hpp"
#include "DatabaseCreation.h"


#define USE_VARARGS
#define PREFER_STDARG
#include <readline/readline.h>

#include <cstring>
#include <common/DatabaseSQLite.h>

namespace {
    typedef int (* dbcmd_function)(Storage& ab, struct dbsys* system,
                                   int argc, char** argv);

/// \brief Entry in the global command table for cycmd
    struct dbsys
    {
        const char* sys_name;
        const char* sys_description;
        dbcmd_function sys_function;
        struct dbsys* sys_subsys;
    };

    int dbs_help(Storage& ab, struct dbsys* system, int argc, char** argv)
    {
        size_t max_length = 0;

        for (struct dbsys* I = system->sys_subsys; I->sys_name != nullptr; ++I) {
            max_length = std::max(max_length, strlen(I->sys_name));
        }
        max_length += 2;

        std::cout << "Cyphesis database tool." << std::endl
                  << system->sys_name << " commands:"
                  << std::endl << std::endl;

        for (struct dbsys* I = system->sys_subsys; I->sys_name != nullptr; ++I) {
            std::cout << "    " << I->sys_name
                      << std::string(max_length - strlen(I->sys_name), ' ')
                      << I->sys_description << std::endl;
        }
        std::cout << std::endl << std::flush;
        return 0;
    }


    static int world_purge(Storage& ab, struct dbsys* system,
                           int argc, char** argv)
    {
        std::string cmd = "DELETE FROM entities WHERE loc IS NOT null";
        if (Database::instance().runCommandQuery(cmd) != 0) {
            std::cout << "Entity purge fail" << std::endl << std::flush;
            return 1;
        }
        cmd = "DELETE FROM properties";
        if (Database::instance().runCommandQuery(cmd) != 0) {
            std::cout << "Property purge fail" << std::endl << std::flush;
            return 1;
        }
        return 0;
    }

    static int users_purge(Storage& ab, struct dbsys* system,
                           int argc, char** argv)
    {
        std::string cmd = "DELETE FROM accounts WHERE username != 'admin'";
        if (Database::instance().runCommandQuery(cmd) != 0) {
            std::cout << "User purge fail" << std::endl << std::flush;
            return 1;
        }
        return 0;
    }

    static int users_list(Storage& ab, struct dbsys* system,
                          int argc, char** argv)
    {
        std::string cmd = "SELECT username, type FROM accounts";
        DatabaseResult res = Database::instance().runSimpleSelectQuery(cmd);
        DatabaseResult::const_iterator I = res.begin();
        DatabaseResult::const_iterator Iend = res.end();
        for (; I != Iend; ++I) {
            std::string name = I.column("username");
            std::string type = I.column("type");
            std::cout << (type == "admin" ? "*" : " ") << name
                      << std::endl << std::flush;
        }

        return 0;
    }

    static int users_del(Storage& ab, struct dbsys* system,
                         int argc, char** argv)
    {
        if (argc != 2) {
            std::cout << "usage: " << system->sys_name
                      << " <username>" << std::endl << std::flush;
            return 1;
        }
        std::string id = argv[1];
        std::string cmd = String::compose("SELECT username FROM accounts "
                                          "WHERE username='%1'", id);
        DatabaseResult res = Database::instance().runSimpleSelectQuery(cmd);
        if (res.size() == 0) {
            std::cout << "User account " << id << " not found"
                      << std::endl << std::flush;
            return 1;
        }
        if (res.size() != 1) {
            std::cout << "ERROR: Multiple accounts match " << id
                      << std::endl << std::flush;
            return 1;
        }
        cmd = String::compose("DELETE FROM accounts WHERE "
                              "username = '%1'", id);
        if (Database::instance().runCommandQuery(cmd) != 0) {
            std::cout << "User delete fail" << std::endl << std::flush;
            return 1;
        }

        return 0;
    }

    static int users_mod(Storage& ab, struct dbsys* system,
                         int argc, char** argv)
    {
        int opt;
        char* type = 0, * password = 0;

        while ((opt = getopt(argc, argv, "t:p:")) != -1) {
            switch (opt) {
                case 't':
                    type = optarg;
                    break;
                case 'p':
                    password = optarg;
                    break;
                default:
                    std::cout << "usage: " << system->sys_name
                              << " [-t TYPE|-p PASSWORD] USERNAME"
                              << std::endl << std::flush;
                    return 1;
            }
        }

        if (argc - optind != 1 || (type == 0 && password == 0)) {
            std::cout << "usage: " << system->sys_name
                      << " [-t TYPE] [-p PASSWORD] USERNAME"
                      << std::endl << std::flush;
            return 1;
        }
        std::string id = argv[optind];
        std::string cmd = String::compose("SELECT username, type FROM accounts "
                                          "WHERE username='%1'", id);
        DatabaseResult res = Database::instance().runSimpleSelectQuery(cmd);
        if (res.size() == 0) {
            std::cout << "User account " << id << " not found"
                      << std::endl << std::flush;
            return 1;
        }
        if (res.size() != 1) {
            std::cout << "ERROR: Multiple accounts match " << id
                      << std::endl << std::flush;
            return 1;
        }
        if (type != 0) {
            std::string new_type = type;
            if (new_type != "admin" && new_type != "player" &&
                new_type != "disabled") {
                std::cout << "ERROR: Account type must be one of "
                             "\"player\", \"admin\" or \"disabled\""
                          << std::endl << std::flush;
            }
            // FIXME Verify the account exists.
            cmd = String::compose("UPDATE accounts SET type = '%1' WHERE "
                                  "username = '%2'", new_type, id);
            if (Database::instance().runCommandQuery(cmd) != 0) {
                std::cout << "User mod type fail" << std::endl << std::flush;
                return 1;
            }
            std::cout << "Account type updated."
                      << std::endl << std::flush;
        }
        if (password != 0) {
            std::string new_pass;
            encrypt_password(password, new_pass);
            cmd = String::compose("UPDATE accounts SET password = '%1' WHERE "
                                  "username = '%2'", new_pass, id);
            if (Database::instance().runCommandQuery(cmd) != 0) {
                std::cout << "User mod password fail" << std::endl << std::flush;
                return 1;
            }
            std::cout << "Account password updated."
                      << std::endl << std::flush;
        }

        return 0;
    }

    int dbs_generic(Storage& ab, struct dbsys* system,
                    int argc, char** argv)
    {
        struct dbsys* subsyss = system->sys_subsys;
        if (subsyss == 0) {
            std::cout << "INTERNAL ERROR" << std::endl << std::flush;
            return 1;
        }
        int nargc = argc - 1;
        if (nargc < 1) {
            dbs_help(ab, system, argc, argv);
            return 1;
        }
        char** nargv = &argv[1];
        for (struct dbsys* I = subsyss; I->sys_name != nullptr; ++I) {
            if (strcmp(nargv[0], I->sys_name) == 0) {
                return I->sys_function(ab, I, nargc, nargv);
            }
        }
        std::cout << "ERROR: No such command: "
                  << system->sys_name << " " << nargv[0]
                  << std::endl << std::endl << std::flush;
        dbs_help(ab, system, argc, argv);
        return 0;
    }

    struct dbsys world_cmds[] = {
        {"purge", "Purge world data", &world_purge, 0},
        {"help",  "Show world help",  &dbs_help,    &world_cmds[0]},
        {nullptr, "Guard",}
    };

    struct dbsys users_cmds[] = {
        {"purge", "Purge users data",      &users_purge, 0},
        {"list",  "List user accounts",    &users_list,  0},
        {"del",   "Delete a user account", &users_del,   0},
        {"mod",   "Modify a user account", &users_mod,   0},
        {"help",  "Show users help",       &dbs_help,    &users_cmds[0]},
        {nullptr, "Guard",}
    };


    struct dbsys systems[] = {
        {"user",  "Modify the user account table",   &dbs_generic, &users_cmds[0]},
        {"world", "Modify the world storage tables", &dbs_generic, &world_cmds[0]},
        {"help",  "Show command help",               &dbs_help,    &systems[0]},
        {nullptr, "Guard",}
    };

    struct dbsys tool = {
        "cydb", "Work with cyphesis databases", &dbs_help, &systems[0]
    };

//    int completion_iterator = 0;
//
//    char* completion_generator(const char* text, int state)
//    {
//        if (state == 0) {
//            completion_iterator = 0;
//        }
//        for (int i = completion_iterator; systems[i].sys_name != 0; ++i) {
//            if (strncmp(text, systems[i].sys_name, strlen(text)) == 0) {
//                completion_iterator = i + 1;
//                return strdup(systems[i].sys_name);
//            }
//        }
//        return 0;
//    }

//    void gotCommand(char* cmd)
//    {
//    }

//// FIXME
//    void setup_completion()
//    {
//        std::string prompt = "foobar>";
//        rl_callback_handler_install(prompt.c_str(), &gotCommand);
//        rl_completion_entry_function = &completion_generator;
//
//        std::cout << std::endl << std::flush;
//        rl_callback_handler_remove();
//    }

#if 0
    // FIXME
    void updatePrompt()
    {
        std::string prompt;
        std::string designation(">");
        if (!username.empty()) {
            prompt = username + "@";
            if (accountType == "admin") {
                designation = "#";
            } else {
                designation = "$";
            }
        } else {
            prompt = "";
        }
        prompt += serverName;
        prompt += " ";
        prompt += systemType;
        prompt += designation;
        prompt += " ";
        rl_set_prompt(prompt.c_str());
    }

    void exec(const std::string & cmd, const std::string & arg)
    {
        bool reply_expected = true;
        reply_flag = false;
        error_flag = false;

        if (cmd == "stat") {
            Get g;
            encoder->streamObjectsMessage(g);
        } else if (cmd == "install") {
            size_t space = arg.find(' ');
            if (space == std::string::npos || space >= (arg.size() - 1)) {
                std::cout << "usage: install <type id> <parent id>"
                          << std::endl << std::flush;
            } else {
                Create c;
                c->setFrom(accountId);
                Anonymous ent;
                ent->setId(std::string(arg, 0, space));
                ent->setObjtype("class");
                ent->setParent(std::string(arg, space + 1));
                c->setArgs1(ent);
                encoder->streamObjectsMessage(c);
            }
            reply_expected = false;
        } else if (cmd == "look") {
            Look l;
            l->setFrom(accountId);
            encoder->streamObjectsMessage(l);
        } else if (cmd == "logout") {
            Logout l;
            l->setFrom(accountId);
            if (!arg.empty()) {
                Anonymous lmap;
                lmap->setId(arg);
                l->setArgs1(lmap);
                reply_expected = false;
            }
            encoder->streamObjectsMessage(l);
        } else if (cmd == "say") {
            Talk t;
            Anonymous ent;
            ent->setAttr("say", arg);
            t->setArgs1(ent);
            t->setFrom(accountId);
            encoder->streamObjectsMessage(t);
        } else if (cmd == "help" || cmd == "?") {
            reply_expected = false;
            help();
        } else if (cmd == "query") {
            Get g;

            Anonymous cmap;
            cmap->setObjtype("obj");
            if (!arg.empty()) {
                cmap->setId(arg);
            }
            g->setArgs1(cmap);
            g->setFrom(accountId);

            encoder->streamObjectsMessage(g);
        } else if (cmd == "reload") {
            if (arg.empty()) {
                reply_expected = false;
                std::cout << "reload: Argument required" << std::endl << std::flush;
            } else {
                Set s;

                Anonymous tmap;
                tmap->setObjtype("class");
                tmap->setId(arg);
                s->setArgs1(tmap);
                s->setFrom(accountId);

                encoder->streamObjectsMessage(s);
            }
        } else if (cmd == "get") {
            Get g;

            Anonymous cmap;
            cmap->setObjtype("class");
            if (!arg.empty()) {
                cmap->setId(arg);
            }
            g->setArgs1(cmap);
            g->setFrom(accountId);

            encoder->streamObjectsMessage(g);
        } else if (cmd == "monitor") {
            AdminTask * task = new OperationMonitor;
            if (runTask(task, arg) == 0) {
                Monitor m;

                m->setArgs1(Anonymous());
                m->setFrom(accountId);

                encoder->streamObjectsMessage(m);
            }

            reply_expected = false;
        } else if (cmd == "unmonitor") {
            OperationMonitor * om = dynamic_cast<OperationMonitor *>(currentTask);

            if (om != 0) {
                Monitor m;

                m->setFrom(accountId);

                encoder->streamObjectsMessage(m);

                reply_expected = false;

                struct timeval tv;
                gettimeofday(&tv, nullptr);
                int monitor_time = tv.tv_sec - om->startTime();

                std::cout << om->count() << " operations monitored in "
                          << monitor_time << " seconds = "
                          << om->count() / monitor_time
                          << " operations per second"
                          << std::endl << std::flush;

                endTask();
            }
        } else if (cmd == "connect") {
            reply_expected = false;
            Connect m;

            Anonymous cmap;
            cmap->setAttr("hostname", arg);
            m->setArgs1(cmap);
            m->setFrom(accountId);

            encoder->streamObjectsMessage(m);
        } else if (cmd == "add_agent") {
            std::string agent_type("creator");

            if (!arg.empty()) {
                agent_type = arg;
            }

            Create c;

            Anonymous cmap;
            cmap->setParent(agent_type);
            cmap->setName("cycmd agent");
            cmap->setObjtype("obj");
            c->setArgs1(cmap);
            c->setFrom(accountId);

            avatar_flag = true;

            encoder->streamObjectsMessage(c);
        } else if (cmd == "delete") {
            if (agentId.empty()) {
                std::cout << "Use add_agent to add an in-game agent first" << std::endl << std::flush;
                reply_expected = false;
            } else if (arg.empty()) {
                std::cout << "Please specify the entity to delete" << std::endl << std::flush;
                reply_expected = false;
            } else {
                Delete del;

                Anonymous del_arg;
                del_arg->setId(arg);
                del->setArgs1(del_arg);
                del->setFrom(agentId);
                del->setTo(arg);

                encoder->streamObjectsMessage(del);

                reply_expected = false;
            }
        } else if (cmd == "find_by_name") {
            if (agentId.empty()) {
                std::cout << "Use add_agent to add an in-game agent first" << std::endl << std::flush;
                reply_expected = false;
            } else if (arg.empty()) {
                std::cout << "Please specify the name to search for" << std::endl << std::flush;
                reply_expected = false;
            } else {
                Look l;

                Anonymous lmap;
                lmap->setName(arg);
                l->setArgs1(lmap);
                l->setFrom(agentId);

                encoder->streamObjectsMessage(l);

                reply_expected = false;
            }
        } else if (cmd == "find_by_type") {
            if (agentId.empty()) {
                std::cout << "Use add_agent to add an in-game agent first" << std::endl << std::flush;
                reply_expected = false;
            } else if (arg.empty()) {
                std::cout << "Please specify the type to search for" << std::endl << std::flush;
                reply_expected = false;
            } else {
                Look l;

                Anonymous lmap;
                lmap->setParent(arg);
                l->setArgs1(lmap);
                l->setFrom(agentId);

                encoder->streamObjectsMessage(l);

                reply_expected = false;
            }
        } else if (cmd == "flush") {
            if (agentId.empty()) {
                std::cout << "Use add_agent to add an in-game agent first" << std::endl << std::flush;
                reply_expected = false;
            } else if (arg.empty()) {
                std::cout << "Please specify the type to flush" << std::endl << std::flush;
                reply_expected = false;
            } else {
                AdminTask * task = new Flusher(agentId);
                runTask(task, arg);
                reply_expected = false;
            }
        } else if (cmd == "creator_create") {
            if (agentId.empty()) {
                std::cout << "Use add_agent to add an in-game agent first" << std::endl << std::flush;
                reply_expected = false;
            } else if (arg.empty()) {
                std::cout << "Use add_agent to add an in-game agent first" << std::endl << std::flush;
                reply_expected = false;
            } else {
                Create c;

                Anonymous thing;
                thing->setParent(arg);
                c->setArgs1(thing);
                c->setFrom(agentId);

                encoder->streamObjectsMessage(c);

                reply_expected = false;
            }
        } else if (cmd == "cancel") {
            if (endTask() != 0) {
                std::cout << "No task currently running" << std::endl << std::flush;
            }
        } else {
            reply_expected = false;
            std::cout << cmd << ": Command not known" << std::endl << std::flush;
        }

        ios << std::flush;

        if (!reply_expected) { return; }
        // Wait for reply
        time_t wait_start_time = time(nullptr);
        while (!reply_flag) {
           if (time(nullptr) - wait_start_time > 5) {
               std::cout << cmd << ": No reply from server" << std::endl << std::flush;
               return;
           }
           poll(false);
        }
    }
#endif

    static int run_command(Storage& ab, int argc, char** argv)
    {
        for (struct dbsys* I = &systems[0]; I->sys_name != nullptr; ++I) {
            if (strcmp(argv[0], I->sys_name) == 0) {
                return I->sys_function(ab, I, argc, argv);
            }
        }
        std::cout << "ERROR: No such command: " << argv[0]
                  << std::endl << std::endl << std::flush;
        dbs_help(ab, &tool, argc, argv);
        return 1;
    }

}


int main(int argc, char ** argv)
{
    setLoggingPrefix("DB");

    int config_status = loadConfig(argc, argv, USAGE_DBASE); 
    if (config_status < 0) {
        if (config_status == CONFIG_VERSION) {
            reportVersion(argv[0]);
            return 0;
        } else if (config_status == CONFIG_HELP) {
            showUsage(argv[0], USAGE_DBASE, "[ cmd ]");
            return 0;
        } else if (config_status != CONFIG_ERROR) {
            log(ERROR, "Unknown error reading configuration.");
        }
        // Fatal error loading config file
        return 1;
    }

    int optindex = config_status;

    Atlas::Objects::Factories factories;

    bool interactive = true;
    int ret = 0;
    if (optindex < argc) {
        interactive = false;
    }

    auto database = createDatabase();
    Storage ab(*database);

    if (!interactive) {
        ret = run_command(ab, argc - optindex, &argv[optindex]);
    } else {
        dbs_help(ab, &tool, argc, argv);
    }

    delete global_conf;
    return ret;
}
