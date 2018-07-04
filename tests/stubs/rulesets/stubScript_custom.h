//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
#ifndef STUB_Script_operation
#define STUB_Script_operation
HandlerResult Script::operation(const std::string & opname,
                                const Atlas::Objects::Operation::RootOperation & op,
                                OpVector & res)
{
    return HandlerResult::OPERATION_IGNORED;
}
#endif