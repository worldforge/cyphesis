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

// $Id$

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "globals.h"

#include "prefix.h"
#include "const.h"
#include "log.h"
#include "compose.hpp"

#include "modules/DateTime.h"

#include <varconf/config.h>

#include <algorithm>

#include <sys/stat.h>

#include <cstring>

#include <cassert>

const char * CYPHESIS = "cyphesis";
const char * CLIENT = "client";
const char * SLAVE = "slave";

static const char * DEFAULT_RULESET = "mason";
static const char * DEFAULT_CLIENT_SOCKET = "cyphesis.sock";
static const char * DEFAULT_PYTHON_SOCKET = "cypython.sock";
static const char * DEFAULT_SLAVE_SOCKET = "cyslave.sock";
static const char * DEFAULT_INSTANCE = "cyphesis";

varconf::Config * global_conf = NULL;
std::string instance(DEFAULT_INSTANCE);
std::string share_directory(DATADIR);
std::string etc_directory(SYSCONFDIR);
std::string var_directory(LOCALSTATEDIR);
std::string ruleset(DEFAULT_RULESET);
bool exit_flag = false;
bool daemon_flag = false;
bool restricted_flag = false;
bool database_flag = true;
int timeoffset = DateTime::spm() * DateTime::mph() * 9; // Morning
int client_port_num = 6767;
int slave_port_num = 6768;
int dynamic_port_start = 6800;
int dynamic_port_end = 6899;

static const char * FALLBACK_LOCALSTATEDIR = "/var";

static const int S = USAGE_SERVER;
static const int C = USAGE_CLIENT;
static const int M = USAGE_CYCMD;
static const int D = USAGE_DBASE;
static const int P = USAGE_CYPYTHON;

typedef struct {
    const char * section;
    const char * option;
    const char * value;
    const char * dflt;
    const char * description;
    int flags;
} usage_data;

static const usage_data usage[] = {
    { "", "help", "", "", "Display usage information and exit", S|C|M|D },
    { "", "version", "", "", "Display the version information and exit", S|C|M|D },
    { "", "instance", "<short_name>", "\"cyphesis\"", "Unique short name for the server instance", S|C|M|D },
    { CYPHESIS, "directory", "<directory>", "", "Directory where server data and scripts can be found", S|C },
    { CYPHESIS, "confdir", "<directory>", "", "Directory where server config can be found", S|C|M|D },
    { CYPHESIS, "vardir", "<directory>", "", "Directory where temporary files can be stored", S|C|M },
    { CYPHESIS, "ruleset", "<name>", DEFAULT_RULESET, "Ruleset name", S|C|D },
    { CYPHESIS, "servername", "<name>", "<hostname>", "Published name of the server", S|C },
    { CYPHESIS, "tcpport", "<portnumber>", "6767", "Network listen port for client connections", S|C|M },
    { CYPHESIS, "dynamic_port_start", "<portnumber>", "6800", "Lowest port to try and used for dyanmic ports", S },
    { CYPHESIS, "dynamic_port_end", "<portnumber>", "6899", "Highest port to try and used for dyanmic ports", S },
    { CYPHESIS, "restricted", "true|false", "false", "Flag to control restricted mode", S },
    { CYPHESIS, "usemetaserver", "true|false", "true", "Flag to control registration with the metaserver", S },
    { CYPHESIS, "usedatabase", "true|false", "true", "Flag to control whether to use a database for persistent storage", S },
    { CYPHESIS, "metaserver", "<hostname>", "metaserver.worldforge.org", "Hostname to use as the metaserver", S },
    { CYPHESIS, "daemon", "true|false", "false", "Flag to control running the server in daemon mode", S },
    { CYPHESIS, "nice", "<level>", "1", "Reduce the priority level of the server", S },
    { CYPHESIS, "useaiclient", "true|false", "false", "Flag to control whether AI is to be driven by a client", S },
    { CYPHESIS, "dbserver", "<hostname>", "", "Hostname for the PostgreSQL RDBMS", S|D },
    { CYPHESIS, "dbname", "<name>", "\"cyphesis\"", "Name of the database to use", S|D },
    { CYPHESIS, "dbuser", "<dbusername>", "<username>", "Database user name for access", S|D },
    { CYPHESIS, "dbpasswd", "<dbusername>", "", "Database password for access", S|D },
    { CLIENT, "package", "<package_name>", "define_world", "Python package which contains the world initialisation code", C },
    { CLIENT, "function", "<function_name>", "default", "Python function to initialise the world", C },
    { CLIENT, "serverhost", "<hostname>", "localhost", "Hostname of the server to connect to", S|C|M },
    { CLIENT, "account", "<username>", "admin", "Account name to use to authenticate to the server", S|C },
    { CLIENT, "password", "<password>", "", "Password to use to authenticate to the server", S|C },
    { CLIENT, "useslave", "true|false", "false", "Flag to control connecting to an AI slave server, not master world server" , S|M },
    { SLAVE, "tcpport", "<portnumber>", "6768", "Network listen port for client connections to the AI slave server", M },
    { SLAVE, "server", "<hostname>", "localhost", "Master server to connect the slave to", M },
    { 0, 0, 0, 0 }
};

static int check_tmp_path(const std::string & dir)
{
    std::string tmp_directory = dir + "/tmp";
    struct stat tmp_stat;

    if (::stat(tmp_directory.c_str(), &tmp_stat) != 0) {
        return -1;
    }

    if (!S_ISDIR(tmp_stat.st_mode)) {
        return -1;
    }

    if (::access(tmp_directory.c_str(), W_OK) != 0) {
        return -1;
    }

    return 0;
}

static int force_simple_name(const std::string & in, std::string & out)
{
    out = std::string(in.size(), ' ');

    for (unsigned int i = 0; i < in.size(); ++i) {
        int c = in[i];
        if (islower(c) || isdigit(c)) {
            out[i] = c;
        } else if (isalpha(c)) {
            out[i] = ::tolower(c);
        } else if (isspace(c) || c == '_') {
            out[i] = '_';
        } else {
            return -1;
        }
    }
    return 0;
}

template <typename T>
int readConfigItem(const std::string & section, const std::string & key, T & storage)
{
    if (global_conf->findItem(section, key)) {
        storage = global_conf->getItem(section, key);
        return 0;
    }
    return -1;
}

template<>
int readConfigItem<std::string>(const std::string & section, const std::string & key, std::string & storage)
{
    if (global_conf->findItem(section, key)) {
        storage = global_conf->getItem(section, key).as_string();
        return 0;
    }
    return -1;
}

class Option {
  protected:
    std::string m_value;
    std::string m_description;

    Option(const std::string & val, const std::string & descr);
  public:
    virtual ~Option() = 0;

    const std::string & value() { return m_value; }
    const std::string & description() { return m_description; }

    virtual void read(varconf::Variable var) = 0;

    virtual const std::string repr() const = 0;

    virtual size_t size() const = 0;

    virtual void missing() { }

    virtual void postProcess() { }
};

class DumbOption : public Option {
  protected:
    std::string m_default;
  public:
    DumbOption(const std::string & val, const std::string & descr,
               const std::string & deflt);

    virtual ~DumbOption() { }

    virtual void read(varconf::Variable var) { }
  
    virtual const std::string repr() const;

    virtual size_t size() const;
};

template<typename ValueT>
class StaticOption : public Option {
  protected:
    ValueT & m_data;
    const ValueT m_default;

  public:
    StaticOption(const std::string & val,
                 const std::string & descr,
                 ValueT & data) :
        Option(val, descr), m_data(data), m_default(data) { }

    virtual void read(varconf::Variable var);

    virtual const std::string repr() const;

    virtual size_t size() const;
    
};

Option::Option(const std::string & val, const std::string & descr) :
        m_value(val), m_description(descr)
{
}

Option::~Option()
{
}

DumbOption::DumbOption(const std::string & val,
                       const std::string & descr,
                       const std::string & deflt) :
            Option(val, descr), m_default(deflt)
{
}

const std::string DumbOption::repr() const
{
    return m_default;
}

size_t DumbOption::size() const
{
    return m_default.size();
}

template<>
void StaticOption<std::string>::read(varconf::Variable var)
{
    m_data = var.as_string();
}

template<typename ValueT>
void StaticOption<ValueT>::read(varconf::Variable var)
{
    m_data = var;
}

template<typename ValueT>
const std::string StaticOption<ValueT>::repr() const
{
    return String::compose("%1", m_default);
}

template<typename ValueT>
size_t StaticOption<ValueT>::size() const
{
    return String::compose("%1", m_default).size();
}

template class StaticOption<int>;

class UnixSockOption : public StaticOption<std::string>
{
  protected:
    const char * const m_format;
  public:
    UnixSockOption(const std::string & val,
                   const std::string & descr,
                   std::string & data,
                   const char * format) :
        StaticOption<std::string>(val, descr, data), m_format(format) { }

    virtual void missing();

    virtual void postProcess();
};

void UnixSockOption::missing()
{
    m_data = String::compose(m_format, instance);
}

void UnixSockOption::postProcess()
{
    if (m_data.find('/') != 0) {
        m_data = String::compose("%1/tmp/%2",
                                 var_directory,
                                 m_data);
    }
}

typedef std::map<std::string, Option *> OptionMap;
typedef std::map<std::string, OptionMap> SectionMap;

class Options {
  protected:
    SectionMap m_sectionMap;

    static Options * m_instance;

    explicit Options();
  public:
    static Options * instance() {
        if (m_instance == 0) {
            m_instance = new Options;
        }
        return m_instance;
    }

    const SectionMap & sectionMap() const {
        return m_sectionMap;
    }

    int check_config(varconf::Config &, int usage_groups = USAGE_SERVER|
                                                           USAGE_CLIENT|
                                                           USAGE_CYCMD|
                                                           USAGE_DBASE) const;

    void addOption(const std::string & section,
                   const std::string & setting,
                   Option *);
};

Options * Options::m_instance = 0;

Options::Options()
{
    const usage_data * ud = &usage[0];
    for (; ud->section != 0; ++ud) {
        m_sectionMap[ud->section].insert(std::make_pair(ud->option,
              new DumbOption(ud->value, ud->description, ud->dflt)));
    }
}

int Options::check_config(varconf::Config & config,
                          int usage_groups) const
{
    SectionMap::const_iterator I = m_sectionMap.begin();
    SectionMap::const_iterator Iend = m_sectionMap.end();
    for (; I != Iend; ++I) {
        const std::string & section_name = I->first;
        const OptionMap & section_help = I->second;
        const varconf::sec_map & section = config.getSection(section_name);

        varconf::sec_map::const_iterator J = section.begin();
        varconf::sec_map::const_iterator Jend = section.end();
        for (; J != Jend; ++J) {
            const std::string & option_name = J->first;
            if (section_help.find(J->first) == section_help.end()) {
                log(WARNING, String::compose("Invalid option -- %1:%2",
                                             section_name, option_name));
            }
        }
    }
    return 0;
}

void Options::addOption(const std::string & section,
                        const std::string & setting,
                        Option * option)
{
    OptionMap & config_section = m_sectionMap[section];
    if (config_section.find(setting) != config_section.end()) {
        log(ERROR, String::compose("Config option %1:%2 already defined",
                                   section, setting));
        return;
    }
    m_sectionMap[section].insert(std::make_pair(setting, option));
}

int_config_register::int_config_register(int & var,
                                         const char * section,
                                         const char * setting,
                                         const char * help)
{
    Options::instance()->addOption(section, setting,
                                   new StaticOption<int>("<foo>", help, var));
}

string_config_register::string_config_register(std::string & var,
                                               const char * section,
                                               const char * setting,
                                               const char * help)
{
    Options::instance()->addOption(section, setting,
                                   new StaticOption<std::string>("<foo>", help, var));
}

unixsock_config_register::unixsock_config_register(std::string & var,
                                                   const char * section,
                                                   const char * setting,
                                                   const char * help,
                                                   const char * format)
{
    Options::instance()->addOption(section, setting,
                                   new UnixSockOption("<foo>", help, var, format));
}

void readInstanceConfiguration(const std::string & section);

int loadConfig(int argc, char ** argv, int usage)
{
    global_conf = varconf::Config::inst();

    global_conf->setParameterLookup('h', "help");
    global_conf->setParameterLookup('?', "help");

    global_conf->setParameterLookup('v', "version");

    // Check the commmand line config doesn't contain any unknown or
    // inappropriate options.
    varconf::Config test_cmdline;
    test_cmdline.getCmdline(argc, argv);
    Options::instance()->check_config(test_cmdline, usage);

    // See if the user has set the install directory on the command line
    bool home_dir_config = false;
    char * home = getenv("HOME");

    // Read in only the users settings, and the commandline settings.
    if (home != NULL) {
        home_dir_config = global_conf->readFromFile(std::string(home) + "/.cyphesis.vconf");
    }

    global_conf->getCmdline(argc, argv);

    if (global_conf->findItem("", "version")) {
        return CONFIG_VERSION;
    }

    if (global_conf->findItem("", "help")) {
        return CONFIG_HELP;
    }

    // Check if the config directory has been overriden at this point, as if
    // it has, that will affect loading the main config.
    readConfigItem("cyphesis", "confdir", etc_directory);

    // Load up the rest of the system config file, and then ensure that
    // settings are overridden in the users config file, and the command line
    bool main_config = global_conf->readFromFile(etc_directory +
                                                 "/cyphesis/cyphesis.vconf",
                                                 varconf::GLOBAL);
    if (!main_config) {
        log(ERROR, String::compose("Unable to read main config file \"%1\"",
                                      etc_directory +
                                      "/cyphesis/cyphesis.vconf"));
        if (home_dir_config) {
            log(INFO, "Try removing .cyphesis.vconf from your home directory as it may specify an invalid installation directory, and then restart cyphesis.");
        } else {
            log(INFO, "Please ensure that cyphesis has been installed correctly.");
        }
    }
    if (home_dir_config) {
        global_conf->readFromFile(std::string(home) + "/.cyphesis.vconf");
    }

    int optind = global_conf->getCmdline(argc, argv);

    Options::instance()->check_config(*global_conf);

    assert(optind > 0);

    std::string raw_instance;

    if (readConfigItem("", "instance", raw_instance) == 0) {
        if (force_simple_name(raw_instance, instance) != 0) {
            log(ERROR, "Invalid instance name.");
            return CONFIG_ERROR;
        }
        if (raw_instance != instance) {
            log(INFO, String::compose("Using instance name \"%1\".", instance));
        }
    }

    readConfigItem("cyphesis", "dynamic_port_start", dynamic_port_start);
    readConfigItem("cyphesis", "dynamic_port_end", dynamic_port_end);

    Options * options = Options::instance();

    SectionMap::const_iterator I = options->sectionMap().begin();
    SectionMap::const_iterator Iend = options->sectionMap().end();
    OptionMap::const_iterator J;
    OptionMap::const_iterator Jend;
    for (; I != Iend; ++I) {
        J = I->second.begin();
        Jend = I->second.end();
        for (; J != Jend; ++J) {
            if (global_conf->findItem(I->first, J->first)) {
                std::cout << "FOUND " << I->first << ":" << J->first
                          << std::endl << std::flush;
                J->second->read(global_conf->getItem(I->first, J->first));
            } else {
                J->second->missing();
            }
            J->second->postProcess();
        }
    }

    readInstanceConfiguration(instance);

    return optind;
}

void updateUserConfiguration()
{
    char * home = getenv("HOME");

    // Write out any changes that have been overriden at user scope. It
    // may be a good idea to do this at shutdown.
    if (home != NULL) {
        global_conf->writeToFile(std::string(home) + "/.cyphesis.vconf", varconf::USER);
    }

}

void readInstanceConfiguration(const std::string & section)
{
    // Config is now loaded. Now set the values of some globals.

    readConfigItem(section, "directory", share_directory);

    readConfigItem(section, "confdir", etc_directory);

    readConfigItem(section, "vardir", var_directory);

    readConfigItem(section, "daemon", daemon_flag);

    if (readConfigItem(section, "tcpport", client_port_num) != 0) {
        if (section != DEFAULT_INSTANCE) {
            client_port_num = -1;
        }
    }

    readConfigItem("slave", "tcpport", slave_port_num);

    // Load up the ruleset.
    if (readConfigItem(section, "ruleset", ruleset)) {
        if (section == DEFAULT_INSTANCE) {
            log(ERROR, String::compose("No ruleset specified in config. "
                                       "Using \"%1\" rules.", DEFAULT_RULESET));
        } else {
            log(INFO, String::compose("Auto configuring new instance \"%1\" "
                                      "to use ruleset \"%2\".",
                                      instance, ruleset));
            global_conf->setItem(section, "ruleset", ruleset, varconf::USER);
        }
    }

    if (check_tmp_path(var_directory) != 0) {
        if (var_directory != "/usr/var") {
            // Binreloc enabled builds installed system wide have localstatedir
            // set to something that is never writable, so must always fall
            // back to /var/tmp, so we should not display the message.
            log(WARNING,
                String::compose("No temporary directory found at \"%1/tmp\"",
                                var_directory));
        }
        if (check_tmp_path(FALLBACK_LOCALSTATEDIR) != 0) {
            log(CRITICAL, String::compose("No temporary directory available "
                                          "at \"%1/tmp\" or \"%2/tmp\".",
                                          var_directory,
                                          FALLBACK_LOCALSTATEDIR));
        } else {
            if (var_directory != "/usr/var") {
                log(NOTICE,
                    String::compose("Using \"%1/tmp\" as temporary directory",
                                    FALLBACK_LOCALSTATEDIR));
            }
            var_directory = FALLBACK_LOCALSTATEDIR;
        }
    }

}

void reportVersion(const char * prgname)
{
    std::cout << prgname << " (cyphesis) " << consts::version
              << " (WorldForge)" << std::endl << std::flush;
}

void showUsage(const char * prgname, int usage_flags, const char * extras)
{
    std::cout << "Usage: " << prgname << " [options]";
    if (extras != 0) {
        std::cout << " " << extras;
    }
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    
    size_t column_width = 0;

    Options * options = Options::instance();

    SectionMap::const_iterator I = options->sectionMap().begin();
    SectionMap::const_iterator Iend = options->sectionMap().end();
    OptionMap::const_iterator J;
    OptionMap::const_iterator Jend;
    for (; I != Iend; ++I) {
        J = I->second.begin();
        Jend = I->second.end();
        for (; J != Jend; ++J) {
            column_width = std::max(column_width, I->first.size() + J->first.size() + J->second->value().size() + 2);
        }
    }

    std::cout << "  --help, -h        Display usage information and exit"
              << std::endl;

    std::cout << "  --version, -v     Display the version information and exit"
              << std::endl << std::endl;

    I = options->sectionMap().begin();
    for (; I != Iend; ++I) {
        J = I->second.begin();
        Jend = I->second.end();
        for (; J != Jend; ++J) {
            if (I->first.size() != 0) {
                std::cout << "  --" << I->first << ":" << J->first;
            } else {
                std::cout << "  --" << J->first;
            }
            Option * opt = J->second;
            if (opt->value().size() != 0) {
                std::cout << "=" << opt->value();
            }
            if (opt->size() != 0) {
                size_t len = I->first.size() + 1 + J->first.size();
                if (opt->value().size() != 0) {
                    len += (opt->value().size() + 1);
                }
                std::cout << std::string(column_width - len + 2, ' ')
                          << "= " << opt->repr();
            }
            std::cout << std::endl;
            std::cout << "      " << opt->description() << std::endl;
        }
    }
    std::cout << std::flush;
}
