//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
#ifndef STUB_Modifier_createModifier
#define STUB_Modifier_createModifier
std::unique_ptr<Modifier> Modifier::createModifier(ModifierType modificationType, Atlas::Message::Element attr)
{
    return std::make_unique<DefaultModifier>(std::move(attr));
}
#endif //STUB_Modifier_createModifier

#ifndef STUB_DefaultModifier_process
#define STUB_DefaultModifier_process
void DefaultModifier::process(Atlas::Message::Element& element, const Atlas::Message::Element& baseValue) const
{
    element = mValue;
}
#endif //STUB_DefaultModifier_process
