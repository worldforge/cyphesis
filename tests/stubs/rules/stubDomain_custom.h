//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
#ifndef STUB_Domain_Domain
#define STUB_Domain_Domain
Domain::Domain(LocatedEntity& entity)
: m_entity(entity)
{

}
#endif //STUB_Domain_Domain

#ifndef STUB_Domain_observeCloseness
#define STUB_Domain_observeCloseness
boost::optional<std::function<void()>> Domain::observeCloseness(LocatedEntity& reacher, LocatedEntity& target, double reach, std::function<void()> callback)
{
    return boost::none;
}
#endif //STUB_Domain_observeCloseness
