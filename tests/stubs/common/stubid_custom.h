//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
namespace {
    long _id = 0;
}

long integerId(const std::string & id)
{
    long intId = strtol(id.c_str(), 0, 10);
    if (intId == 0 && id != "0") {
        intId = -1L;
    }

    return intId;
}

long forceIntegerId(const std::string & id)
{
    long intId = strtol(id.c_str(), 0, 10);
    return intId;
}

int integerIdCheck(const std::string & id)
{
    long intId = strtol(id.c_str(), 0, 10);
    if (intId == 0 && id != "0") {
        return -1;
    }
    return 0;
}

RouterId newId()
{
    return RouterId(++_id);
}

RouterId idFromString(const std::string& id)
{
    return {integerId(id)};
}

RouterId forceIdFromString(const std::string& id) {
    return {forceIntegerId(id)};
}