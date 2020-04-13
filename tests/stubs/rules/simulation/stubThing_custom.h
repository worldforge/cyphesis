//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
#ifndef STUB_Thing_Thing
#define STUB_Thing_Thing
Thing::Thing(const std::string& id, long intId)
    : Entity(id, intId)
{

}
Thing::Thing(long intId)
    : Entity(std::to_string(intId), intId)
{

}
#endif //STUB_Thing_Thing