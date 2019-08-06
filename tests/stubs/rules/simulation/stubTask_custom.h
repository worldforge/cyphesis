//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
#ifndef STUB_Task_Task
#define STUB_Task_Task
Task::Task(UsageInstance usageInstance, const Py::Object& script)
:m_script(script), m_usageInstance(std::move(usageInstance))
{

}
#endif //STUB_Task_Task
