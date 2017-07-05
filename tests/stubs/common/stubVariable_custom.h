//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
template class Variable<int>;
template class Variable<std::string>;
template class Variable<const char *>;

#ifndef STUB_Variable_Variable
#define STUB_Variable_Variable
template <typename T>
Variable<T>::Variable(const T & variable)
    : m_variable(variable)
{

}
#endif //STUB_Variable_Variable

