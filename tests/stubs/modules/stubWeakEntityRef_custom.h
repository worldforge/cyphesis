//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.

#ifndef STUB_WeakEntityRef_WeakEntityRef
#define STUB_WeakEntityRef_WeakEntityRef

WeakEntityRef::WeakEntityRef(const Ref<LocatedEntity>& e)
    : m_inner(e.get())
{

}

WeakEntityRef::WeakEntityRef(LocatedEntity* e) : m_inner(e)
{
}

WeakEntityRef::WeakEntityRef(const WeakEntityRef& ref) : m_inner(ref.m_inner)
{
}

#endif //STUB_WeakEntityRef_WeakEntityRef

WeakEntityRef& WeakEntityRef::operator=(const WeakEntityRef& ref)
{
    m_inner = ref.m_inner;

    return *this;
}
