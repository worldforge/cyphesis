//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
#ifndef STUB_TypeResolver_TypeResolver
#define STUB_TypeResolver_TypeResolver
TypeResolver::TypeResolver(TypeStore& typeStore)
: m_typeStore(typeStore)
{

}
#endif //STUB_TypeResolver_TypeResolver


#ifndef STUB_TypeResolver_InfoOperation
#define STUB_TypeResolver_InfoOperation
std::set<const TypeNode*> TypeResolver::InfoOperation(const Operation& op, OpVector& res)
{
    return std::set<const TypeNode*>();
}
#endif //STUB_TypeResolver_InfoOperation

#ifndef STUB_TypeResolver_processTypeData
#define STUB_TypeResolver_processTypeData
std::set<const TypeNode*> TypeResolver::processTypeData(const Atlas::Objects::Root& data, OpVector& res)
{
    return std::set<const TypeNode*>();
}
#endif //STUB_TypeResolver_processTypeData

#ifndef STUB_TypeResolver_resolveType
#define STUB_TypeResolver_resolveType
std::set<const TypeNode*> TypeResolver::resolveType(const std::string& id, const Atlas::Objects::Root& ent, OpVector& res)
{
    return std::set<const TypeNode*>();
}
#endif //STUB_TypeResolver_resolveType
