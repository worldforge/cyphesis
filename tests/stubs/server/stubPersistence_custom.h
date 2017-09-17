//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
Persistence * Persistence::m_instance = nullptr;

#ifndef STUB_Persistence_Persistence
#define STUB_Persistence_Persistence
Persistence::Persistence() : m_db(*(Database*)0)
{

}
#endif //STUB_Persistence_Persistence

#ifndef STUB_Persistence_instance
#define STUB_Persistence_instance
Persistence* Persistence::instance()
{
    if (m_instance == nullptr) {
        m_instance = new Persistence();
    }
    return m_instance;
}
#endif //STUB_Persistence_instance