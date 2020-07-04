//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.

#ifndef STUB_OperationsDispatcher_OperationsDispatcher
#define STUB_OperationsDispatcher_OperationsDispatcher
template <typename T>
OperationsDispatcher<T>::OperationsDispatcher(std::function<void(const Operation&, Ref<T>)> operationProcessor, TimeProviderFnType timeProviderFn)
    : m_operationProcessor(operationProcessor), m_timeProviderFn(timeProviderFn)
{

}
#endif //STUB_OperationsDispatcher_OperationsDispatcher


#ifndef STUB_OpQueEntry_OpQueEntry
#define STUB_OpQueEntry_OpQueEntry
template <typename T>
OpQueEntry<T>::OpQueEntry(OpQueEntry && o) noexcept
{

}
#endif //STUB_OpQueEntry_OpQueEntry
