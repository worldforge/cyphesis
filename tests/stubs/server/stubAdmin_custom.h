//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
#include <Atlas/Objects/Operation.h>
#ifndef STUB_Admin_Admin
#define STUB_Admin_Admin
Admin::Admin(Connection * conn, const std::string & username, const std::string & passwd, RouterId id)
    : Account(conn, username, passwd, id)
{

}
#endif //STUB_Admin_Admin

#ifndef STUB_Admin_getType
#define STUB_Admin_getType
const char* Admin::getType() const
{
    return "admin";
}
#endif //STUB_Admin_getType

#ifndef STUB_Admin_createMind
#define STUB_Admin_createMind
std::unique_ptr<ExternalMind> Admin::createMind(const Ref<LocatedEntity>& entity) const
{
    return {};
}
#endif //STUB_Admin_createMind
