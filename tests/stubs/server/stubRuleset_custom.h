//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
template<> Ruleset* Singleton<Ruleset>::ms_Singleton = nullptr;

#ifndef STUB_Ruleset_Ruleset
#define STUB_Ruleset_Ruleset
Ruleset::Ruleset(EntityBuilder * eb, boost::asio::io_service& io_service)
: m_io_service(io_service)
{

}
#endif //STUB_Ruleset_Ruleset