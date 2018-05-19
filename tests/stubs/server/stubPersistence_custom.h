//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
template<> Persistence* Singleton<Persistence>::ms_Singleton = nullptr;

#ifndef STUB_Persistence_Persistence
#define STUB_Persistence_Persistence
Persistence::Persistence(Database& database)
    : m_db(database)
{

}
#endif //STUB_Persistence_Persistence
