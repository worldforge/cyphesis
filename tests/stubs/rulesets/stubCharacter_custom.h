//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
#ifndef STUB_Character_Character
#define STUB_Character_Character
Character::Character(const std::string & id, long intId)
    : Thing(id, intId), m_movement(*(Movement*)(nullptr)), m_externalMind(nullptr)
{

}

#endif
