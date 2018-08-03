//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
template<>
Monitors * Singleton<Monitors>::ms_Singleton = nullptr;

//Always just create a singleton
Monitors* monitors = new Monitors();
