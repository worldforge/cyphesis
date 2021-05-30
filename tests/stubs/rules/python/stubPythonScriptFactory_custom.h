//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.

#ifndef STUB_PythonScriptFactory_PythonScriptFactory
#define STUB_PythonScriptFactory_PythonScriptFactory
template <typename T>
PythonScriptFactory<T>::PythonScriptFactory(const std::string & package, const std::string & type)
: PythonClass(package, type)
{

}
#endif //STUB_PythonScriptFactory_PythonScriptFactory



#ifndef STUB_PythonScriptFactory_setup
#define STUB_PythonScriptFactory_setup
template <typename T>
int PythonScriptFactory<T>::setup()
{
    return 0;
}
#endif //STUB_PythonScriptFactory_setup

#ifndef STUB_PythonScriptFactory_package
#define STUB_PythonScriptFactory_package
template <typename T>
const std::string& PythonScriptFactory<T>::package() const
{
    return m_package;
}
#endif //STUB_PythonScriptFactory_package

#ifndef STUB_PythonScriptFactory_addScript
#define STUB_PythonScriptFactory_addScript
template <typename T>
int PythonScriptFactory<T>::addScript(T & entity) const
{
    return 0;
}
#endif //STUB_PythonScriptFactory_addScript

#ifndef STUB_PythonScriptFactory_refreshClass
#define STUB_PythonScriptFactory_refreshClass
template <typename T>
int PythonScriptFactory<T>::refreshClass()
{
    return 0;
}
#endif //STUB_PythonScriptFactory_refreshClass

#ifndef STUB_PythonScriptFactory_createScript
#define STUB_PythonScriptFactory_createScript
template <typename T>
Py::Object PythonScriptFactory<T>::createScript(T & entity) const
{
    return Py::Object();
}
#endif //STUB_PythonScriptFactory_createScript


