//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
#ifndef STUB_StatisticsProperty_StatisticsProperty
#define STUB_StatisticsProperty_StatisticsProperty
StatisticsProperty::StatisticsProperty(const StatisticsProperty & other) :
    m_data(other.m_data),
    m_script(0)
{
}

/// \brief StatisticsProperty constructor
///
/// @param data variable that holds the Property value
/// @param flags flags to indicate how this property is stored
StatisticsProperty::StatisticsProperty() : m_script(0)
{
}

#endif //STUB_StatisticsProperty_StatisticsProperty
