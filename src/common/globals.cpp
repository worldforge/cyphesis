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
#endif

#include "globals.h"

#include "const.h"
#include "log.h"
#include "compose.hpp"
#include "system.h"

#include <varconf/config.h>

#include <algorithm>

#include <cstring>
#include <memory>


#include <cassert>
#include <boost/filesystem/operations.hpp>
#include <basedir.h>

const char* const CYPHESIS = "cyphesis";
const char* const SLAVE = "slave";

static const char* const DEFAULT_RULESET = "deeds";
static const char* const DEFAULT_INSTANCE = "cyphesis";

varconf::Config* global_conf = nullptr;
std::string instance(DEFAULT_INSTANCE);
std::string bin_directory(BINDIR);
std::string share_directory(DATADIR);
std::string etc_directory(SYSCONFDIR);
std::string var_directory(LOCALSTATEDIR);
std::string assets_directory("");
std::string ruleset_name(DEFAULT_RULESET);
std::string server_uuid("");
std::string server_key("");

bool exit_flag = false;
bool exit_flag_soft = false;
bool exit_soft_enabled = false;
bool daemon_flag = false;
bool database_flag = true;
int timeoffset = SPM * MPH * 9; // Morning
int client_port_num = 6767;
int dynamic_port_start = 6800;
int dynamic_port_end = 6899;

static const char* const FALLBACK_LOCALSTATEDIR = "/var";

static const unsigned int S = USAGE_SERVER;
static const unsigned int C = USAGE_CLIENT;
static const unsigned int M = USAGE_CYCMD;
static const unsigned int D = USAGE_DBASE;
static const unsigned int P = USAGE_CYPYTHON;
static const unsigned int A = USAGE_AICLIENT;

/// \brief Structure for holding data about varconf options
typedef struct
{
    const char* section;
    const char* option;
    const char* value;
    const char* dflt;
    const char* description;
    unsigned int flags;
} usage_data;

static const usage_data usage_options[] = {
        {"",       "help",               "",             "",              "Display usage information and exit",                                                                             S | C | M |
                                                                                                                                                                                            D | A},
        {"",       "version",            "",             "",              "Display the version information and exit",                                                                       S | C | M |
                                                                                                                                                                                            D | A},
        {"",       "instance",           "<short_name>", "\"cyphesis\"",  "Unique short name for the server instance",                                                                      S | C | M |
                                                                                                                                                                                            D},
        {"",       "interactive",        "",             "",              "Run in interactive mode, giving a Python prompt",                                                                C},
        {CYPHESIS, "dynamicpaths",       "true|false",   "false",         "Enables dynamic paths, calculated from the current executable",                                                  S | D | C |
                                                                                                                                                                                            M | A},
        {CYPHESIS, "directory",          "<directory>",  "",              "Directory where server data and scripts can be found",                                                           S | C},
        {CYPHESIS, "confdir",            "<directory>",  "",              "Directory where server config can be found",                                                                     S | C | M |
                                                                                                                                                                                            D | A},
        {CYPHESIS, "bindir",             "<directory>",  "",              "Directory where Cyphesis binaries can be found",                                                                 S | C | M |
                                                                                                                                                                                            D | A},
        {CYPHESIS, "vardir",             "<directory>",  "",              "Directory where temporary files can be stored",                                                                  S | C | M |
                                                                                                                                                                                            A},
        {CYPHESIS, "assetsdir",          "<directory>",  "",              "Directory where media assets are stored, if nothing is specified Cyphesis will look under the shared directory", S},
        {CYPHESIS, "ruleset",            "<name>",       DEFAULT_RULESET,       "Ruleset name",                                                                                                   S | C | D |
                                                                                                                                                                                            A},
        {CYPHESIS, "autoimport",         "<path>",       "",              "Path to world file to import if world is empty",                                                                 S},
        {CYPHESIS, "remotery",           "true|false",   "false",         "Set to true to enable Remotery",                                                                                 S},

        {CYPHESIS, "servername",         "<name>",       "<hostname>",    "Published name of the server",                                                                                   S | C},
        {CYPHESIS, "tcpport",            "<portnumber>", "6767",          "Network listen port for client connections",                                                                     S | C | M},
        {CYPHESIS, "dynamic_port_start", "<portnumber>", "6800",          "Lowest port to try and used for dyanmic ports",                                                                  S},
        {CYPHESIS, "dynamic_port_end",   "<portnumber>", "6899",          "Highest port to try and used for dyanmic ports",                                                                 S},
        {CYPHESIS, "usedatabase",        "true|false",   "true",          "Flag to control whether to use a database for persistent storage",                                               S},
        {CYPHESIS, "daemon",             "true|false",   "false",         "Flag to control running the server in daemon mode",                                                              S},
        {CYPHESIS, "nice",               "<level>",      "1",             "Reduce the priority level of the server",                                                                        S},
        {CYPHESIS, "useaiclient",        "true|false",   "false",         "Flag to control whether AI is to be driven by a client",                                                         S},
        {CYPHESIS, "dbserver",           "<hostname>",   "",              "Hostname for the PostgreSQL RDBMS",                                                                              S | D},
        {CYPHESIS, "dbname",             "<name>",       "\"cyphesis\"",  "Name of the database to use",                                                                                    S | D},
        {CYPHESIS, "dbuser",             "<dbusername>", "<username>",    "Database user name for access",                                                                                  S | D},
        {CYPHESIS, "dbpasswd",           "<dbusername>", "",              "Database password for access",                                                                                   S | D},
        {SLAVE,    "tcpport",            "<portnumber>", "6768",          "Network listen port for client connections to the AI slave server",                                              M},
        {SLAVE,    "server",             "<hostname>",   "localhost",     "Master server to connect the slave to",                                                                          M},
        {nullptr,  nullptr,              nullptr,        nullptr}
};


static int check_tmp_path(const std::string& dir)
{
    std::string tmp_directory = dir + "/tmp";
    if (!boost::filesystem::exists(tmp_directory)) {
        auto createResult = boost::filesystem::create_directories(tmp_directory);
        if (!createResult) {
            return -1;
        }
        log(INFO, String::compose("Created tmp directory at %1", tmp_directory));
    } else {
        if (!boost::filesystem::is_directory(tmp_directory)) {
            return -1;
        }
        if (::access(tmp_directory.c_str(), W_OK) != 0) {
            return -1;
        }
    }

    return 0;
}

static int force_simple_name(const std::string& in, std::string& out)
{
    out = std::string(in.size(), ' ');

    for (size_t i = 0; i < in.size(); ++i) {
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

template<typename T>
int readConfigItem(const std::string& section, const std::string& key, T& storage)
{
    if (global_conf->findItem(section, key)) {
        storage = (T) global_conf->getItem(section, key);
        return 0;
    }
    return -1;
}

template<>
int readConfigItem<std::string>(const std::string& section, const std::string& key, std::string& storage)
{
    if (global_conf->findItem(section, key)) {
        storage = global_conf->getItem(section, key).as_string();
        return 0;
    }
    return -1;
}

template int readConfigItem<bool>(const std::string& section, const std::string& key, bool& storage);

template int readConfigItem<int>(const std::string& section, const std::string& key, int& storage);

/// \brief Base class for handling varconf options declared inline.
class Option
{
    protected:
        const std::string m_value;
        const std::string m_description;

        Option(std::string val, std::string descr);

    public:
        virtual ~Option() = 0;

        const std::string& value() const
        { return m_value; }

        const std::string& description() const
        { return m_description; }

        virtual void read(varconf::Variable var) = 0;

        virtual std::string repr() const = 0;

        virtual size_t size() const = 0;

        virtual void missing()
        {}

        virtual void postProcess()
        {}
};

/// \brief Basic varconf option which does not require any processing
class DumbOption : public Option
{
    protected:
        const std::string m_default;
    public:
        DumbOption(const std::string& val, const std::string& descr,
                   std::string deflt);

        ~DumbOption() override = default;

        void read(varconf::Variable var) override
        {}

        std::string repr() const override;

        size_t size() const override;
};

/// \brief Basic varconf option declared as a variable inline
template<typename ValueT>
class StaticOption : public Option
{
    protected:
        ValueT& m_data;
        const ValueT m_default;

    public:
        StaticOption(const std::string& val,
                     const std::string& descr,
                     ValueT& data) :
                Option(val, descr), m_data(data), m_default(data)
        {}

        void read(varconf::Variable var) override;

        std::string repr() const override;

        size_t size() const override;

};

Option::Option(std::string val, std::string descr) :
        m_value(std::move(val)), m_description(std::move(descr))
{
}

Option::~Option() = default;

DumbOption::DumbOption(const std::string& val,
                       const std::string& descr,
                       std::string deflt) :
        Option(val, descr), m_default(std::move(deflt))
{
}

std::string DumbOption::repr() const
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
    m_data = (ValueT) var;
}

template<typename ValueT>
std::string StaticOption<ValueT>::repr() const
{
    return String::compose("%1", m_default);
}

template<typename ValueT>
size_t StaticOption<ValueT>::size() const
{
    return String::compose("%1", m_default).size();
}

template
class StaticOption<int>;

/// \brief Handle the processing required for a unix socket option
class UnixSockOption : public StaticOption<std::string>
{
    protected:
        const char* const m_format;
    public:
        UnixSockOption(const std::string& val,
                       const std::string& descr,
                       std::string& data,
                       const char* format) :
                StaticOption<std::string>(val, descr, data), m_format(format)
        {}

        void missing() override;

        void postProcess() override;
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

typedef std::map<std::string, std::unique_ptr<Option> > OptionMap;
typedef std::map<std::string, OptionMap> SectionMap;

/// \brief Singleton to manage all information about varconf options.
class Options
{
    protected:
        SectionMap m_sectionMap;

        static Options* m_instance;

        explicit Options();

    public:
        static Options* instance()
        {
            if (m_instance == nullptr) {
                m_instance = new Options;
            }
            return m_instance;
        }

        const SectionMap& sectionMap() const
        {
            return m_sectionMap;
        }

        int check_config(varconf::Config&, unsigned int usage_groups = USAGE_SERVER |
                                                                       USAGE_CLIENT |
                                                                       USAGE_CYCMD |
                                                                       USAGE_DBASE) const;

        void addOption(const std::string& section,
                       const std::string& setting,
                       std::unique_ptr<Option> option);
};

Options* Options::m_instance = nullptr;

Options::Options()
{
    const usage_data* ud = &usage_options[0];
    for (; ud->section != nullptr; ++ud) {
        m_sectionMap[ud->section].emplace(ud->option,
                                          std::make_unique<DumbOption>(ud->value, ud->description, ud->dflt));
    }
}

int Options::check_config(varconf::Config& config,
                          unsigned int) const
{
    for (auto& section_entry : m_sectionMap) {
        const std::string& section_name = section_entry.first;
        const OptionMap& section_help = section_entry.second;
        const varconf::sec_map& section = config.getSection(section_name);

        for (auto& entry : section) {
            const std::string& option_name = entry.first;
            if (section_help.find(option_name) == section_help.end() &&
                entry.second->scope() == varconf::INSTANCE) {
                log(WARNING, String::compose("Invalid option -- %1:%2",
                                             section_name, option_name));
            }
        }
    }
    return 0;
}

void Options::addOption(const std::string& section,
                        const std::string& setting,
                        std::unique_ptr<Option> option)
{
    OptionMap& config_section = m_sectionMap[section];
    if (config_section.find(setting) != config_section.end()) {
        log(ERROR, String::compose("Config option %1:%2 already defined",
                                   section, setting));
        return;
    }
    m_sectionMap[section].emplace(setting, std::move(option));
}

int_config_register::int_config_register(int& var,
                                         const char* section,
                                         const char* setting,
                                         const char* help)
{
    Options::instance()->addOption(section, setting,
                                   std::make_unique<StaticOption<int>>("<foo>", help, var));
}

bool_config_register::bool_config_register(bool& var,
                                           const char* section,
                                           const char* setting,
                                           const char* help)
{
    Options::instance()->addOption(section, setting,
                                   std::make_unique<StaticOption<bool>>("<foo>", help, var));
}

string_config_register::string_config_register(std::string& var,
                                               const char* section,
                                               const char* setting,
                                               const char* help)
{
    Options::instance()->addOption(section, setting,
                                   std::make_unique<StaticOption<std::string>>("<foo>", help, var));
}

unixsock_config_register::unixsock_config_register(std::string& var,
                                                   const char* section,
                                                   const char* setting,
                                                   const char* help,
                                                   const char* format)
{
    Options::instance()->addOption(section, setting,
                                   std::make_unique<UnixSockOption>("<filename>", help, var, format));
}

void readInstanceConfiguration(const std::string& section);
namespace {
    /**
     * Reads environment variables that start with "WF_" and inject into Varconf.
     *
     * The environment variables uses "_" as a separator between "section" and "key", and are case sensitive.
     * For example, "WF_cyphesis_bindir=/foo/bar" would set the item "bindir" in the "cyphesis" section to "/foo/bar".
     */
    void set_config_from_environment() {
        const std::string prefix = "WF_";

        //We're here accessing the magical variable "environ" that is globally available in all POSIX processes.
        for (size_t i = 0; environ[i] != nullptr; i++) {
            const std::string env = environ[i];

            if (env.substr(0, prefix.size()) == prefix) {
                auto eq_pos = env.find('=');


                if (eq_pos != std::string::npos) {
                    auto fullname = env.substr(prefix.size(), (eq_pos - prefix.size()));
                    auto separatorPos = fullname.find('_');
                    if (separatorPos != std::string::npos) {
                        auto section = fullname.substr(0, separatorPos);
                        auto name = fullname.substr(separatorPos + 1);
                        auto value = env.substr((eq_pos + 1), (env.size() - (eq_pos + 1)));
                        global_conf->setItem(section, name, value, varconf::INSTANCE);
                    }
                }

            }
        }
    }
}

int loadConfig(int argc, char** argv, int usage)
{
    global_conf = varconf::Config::inst();

    //Listen for errors from Varconf and write to the log.
    global_conf->sige.connect([](const char* error) {
        log(ERROR, error);
    });

    global_conf->setParameterLookup('h', "help");
    global_conf->setParameterLookup('?', "help");

    global_conf->setParameterLookup('v', "version");
    global_conf->setParameterLookup('i', "interactive");

    // Check the command line config doesn't contain any unknown or
    // inappropriate options.
    varconf::Config test_cmdline;
    test_cmdline.getCmdline(argc, argv);
    Options::instance()->check_config(test_cmdline, usage);

    // See if the user has set the install directory on the command line
    boost::filesystem::path homeConfigPath{};
    const auto* configHome = xdgConfigHome(nullptr);

    // Read in only the users settings, and the commandline settings.
    if (configHome) {
        homeConfigPath = boost::filesystem::path(configHome) / "cyphesis.vconf";
        if (boost::filesystem::exists(homeConfigPath)) {
            log(INFO, String::compose("Reading settings from %1", homeConfigPath.string()));
            auto home_dir_config = global_conf->readFromFile(homeConfigPath.string());
            if (!home_dir_config) {
                // Not being able to read from the local config file, if it exists, should result in fail fast.
                return CONFIG_ERROR;
            }
        }
    }

    //Set the var_directory by default to the XDG Data Home directory.
    //This can be overridden either by setting the $XDG_DATA_HOME environment variable, or the "vardir" config option.
    const auto* dataHome = xdgDataHome(nullptr);
    if (dataHome) {
        var_directory = std::string(dataHome) + "/cyphesis";
    }


    global_conf->getCmdline(argc, argv);

    if (global_conf->findItem("", "version")) {
        return CONFIG_VERSION;
    }

    if (global_conf->findItem("", "help")) {
        return CONFIG_HELP;
    }

    bool dynamicPaths = false;
    readConfigItem("cyphesis", "dynamicpaths", dynamicPaths);
    if (dynamicPaths) {
        log(INFO, "Dynamic paths are now enabled. All paths will be calculated relative to the current executable.");
        getinstallprefix();
    }

    //Read config data from environment, since this might instruct us to read the config file from a specific location.
    //Note that we'll do this once again after we've loaded the config file, since we want environment settings to override
    //config file settings.
    set_config_from_environment();

    // Check if the config directory has been overridden at this point, as if
    // it has, that will affect loading the main config.
    readConfigItem("cyphesis", "confdir", etc_directory);

    // Load up the rest of the system config file, and then ensure that
    // settings are overridden in the users config file, and the command line
    auto main_config = global_conf->readFromFile(etc_directory +
                                                 "/cyphesis/cyphesis.vconf",
                                                 varconf::GLOBAL);
    if (!main_config) {
        log(ERROR, String::compose("Unable to read main config file \"%1\"",
                                   etc_directory +
                                   "/cyphesis/cyphesis.vconf"));
        if (!homeConfigPath.empty()) {
            log(INFO, "Try removing .cyphesis.vconf from your home directory as it may specify an invalid installation directory, and then restart cyphesis.");
        } else {
            log(INFO, "Please ensure that cyphesis has been installed correctly.");
        }
        //We should fail if we can't read the config file.
        return CONFIG_ERROR;
    }

    //Home config settings should override global settings, so we'll read this file again.
    if (!homeConfigPath.empty() && boost::filesystem::exists(homeConfigPath)) {
        global_conf->readFromFile(homeConfigPath.string());
    }

    //Here we're reading config setting from the environment again, just like we did a couple of lines before. This is because we want
    //environment settings to override config file settings.
    set_config_from_environment();


    auto optindex = global_conf->getCmdline(argc, argv);

    Options::instance()->check_config(*global_conf);

    assert(optindex > 0);

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

    Options* options = Options::instance();


    for (const auto& section_entry : options->sectionMap()) {
        for (const auto& entry: section_entry.second) {
            if (global_conf->findItem(section_entry.first, entry.first)) {
                entry.second->read(global_conf->getItem(section_entry.first, entry.first));
            } else {
                entry.second->missing();
            }
        }
    }

    readInstanceConfiguration(instance);

    for (const auto& section_entry : options->sectionMap()) {
        for (const auto& entry: section_entry.second) {
            entry.second->postProcess();
        }
    }

    return optindex;
}

void updateUserConfiguration()
{
    const auto* configHome = xdgConfigHome(nullptr);

    // Write out any changes that have been overridden at user scope. It
    // may be a good idea to do this at shutdown.
    if (configHome != nullptr) {
        global_conf->writeToFile((boost::filesystem::path(configHome) / "cyphesis.vconf").string(), varconf::USER);
    }

}

void readInstanceConfiguration(const std::string& section)
{
    // Config is now loaded. Now set the values of some globals.

    readConfigItem(section, "bindir", bin_directory);

    readConfigItem(section, "directory", share_directory);

    readConfigItem(section, "confdir", etc_directory);

    readConfigItem(section, "vardir", var_directory);

    readConfigItem(section, "daemon", daemon_flag);

    if (readConfigItem(section, "tcpport", client_port_num) != 0) {
        if (section != DEFAULT_INSTANCE) {
            client_port_num = -1;
        }
    }

    // Load up the ruleset.
    if (readConfigItem(section, "ruleset", ruleset_name)) {
        if (section == DEFAULT_INSTANCE) {
            log(ERROR, String::compose("No ruleset specified in config. "
                                       "Using \"%1\" rules.", DEFAULT_RULESET));
        } else {
            log(INFO, String::compose("Auto configuring new instance \"%1\" "
                                      "to use ruleset \"%2\".",
                                      instance, ruleset_name));
            global_conf->setItem(section,
                                 "ruleset",
                                 ruleset_name,
                                 varconf::USER);
        }
    }

    readConfigItem(ruleset_name, "assetsdir", assets_directory);

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

void reportVersion(const char* prgname)
{
    std::cout << prgname << " (cyphesis) " << consts::version
              << " (WorldForge)" << std::endl << std::flush;
}

void showUsage(const char* prgname, unsigned int usage_flags, const char* extras)
{
    //Create a lookup table of the commands and flags in "usage"
    std::map<std::string, unsigned int> default_commands_flags;

    const usage_data* ud = &usage_options[0];
    for (; ud->section != nullptr; ++ud) {
        std::stringstream ss;
        ss << ud->section << ":" << ud->option;
        default_commands_flags[ss.str()] = ud->flags;
    }

    //Check if the section and option is allowed given the usage_flag
    auto isAllowed = [&](const std::string& section, const std::string& option) {
        auto I = default_commands_flags.find(section + ":" + option);
        if (I == default_commands_flags.end()) {
            //If it can't be found in "usage" it's defined for the specific command and is allowed
            return true;
        }
        return ((I->second & usage_flags) != 0u);
    };


    std::cout << "Usage: " << prgname << " [options]";
    if (extras != nullptr) {
        std::cout << " " << extras;
    }
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;

    size_t column_width = 0;

    Options* options = Options::instance();


    for (auto& section_entry : options->sectionMap()) {
        for (auto& entry: section_entry.second) {
            if (isAllowed(section_entry.first, entry.first)) {
                column_width = std::max(column_width, section_entry.first.size() + entry.first.size() + entry.second->value().size() + 2);
            }
        }
    }

    std::cout << "  --help, -h        Display usage information and exit"
              << std::endl;

    std::cout << "  --version, -v     Display the version information and exit"
              << std::endl << std::endl;

    for (auto& section_entry : options->sectionMap()) {
        for (auto& entry: section_entry.second) {
            if (isAllowed(section_entry.first, entry.first)) {
                if (!section_entry.first.empty()) {
                    std::cout << "  --" << section_entry.first << ":" << entry.first;
                } else {
                    std::cout << "  --" << entry.first;
                }
                const Option* opt = entry.second.get();
                if (!opt->value().empty()) {
                    std::cout << "=" << opt->value();
                }
                if (opt->size() != 0) {
                    size_t len = section_entry.first.size() + 1 + entry.first.size();
                    if (!opt->value().empty()) {
                        len += (opt->value().size() + 1);
                    }
                    std::cout << std::string(column_width - len + 2, ' ')
                              << "= " << opt->repr();
                }
                std::cout << std::endl;
                std::cout << "      " << opt->description() << std::endl;
            }
        }
    }

    std::cout << std::flush;
}
