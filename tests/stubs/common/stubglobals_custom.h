//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
const char * const CYPHESIS = "cyphesis";
const char * const SLAVE = "slave";

static const char * const DEFAULT_RULESET = "deeds";
static const char * const DEFAULT_INSTANCE = "cyphesis";

varconf::Config * global_conf = nullptr;
std::string instance("");
std::string bin_directory("");
std::string share_directory("");
std::string etc_directory("");
std::string var_directory("");
std::string assets_directory("");
std::string ruleset_name("");
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

static const char * const FALLBACK_LOCALSTATEDIR = "/var";
