//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
#ifndef STUB_IdProperty_IdProperty
#define STUB_IdProperty_IdProperty
IdProperty::IdProperty(const std::string & data)
: m_data(data)
{

}
#endif //STUB_IdProperty_IdProperty

#ifndef STUB_ContainsProperty_ContainsProperty
#define STUB_ContainsProperty_ContainsProperty
ContainsProperty::ContainsProperty(LocatedEntitySet & data)
: m_data(data)
{

}
#endif //STUB_ContainsProperty_ContainsProperty


#ifndef STUB_LocationProperty_LocationProperty
#define STUB_LocationProperty_LocationProperty
LocationProperty::LocationProperty(const LocatedEntity& entity)
        : m_data(entity)
{

}
#endif //STUB_LocationProperty_LocationProperty