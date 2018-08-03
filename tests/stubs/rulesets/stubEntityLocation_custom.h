//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.

#ifndef STUB_EntityLocation_EntityLocation
#define STUB_EntityLocation_EntityLocation
EntityLocation::EntityLocation(Ref<LocatedEntity> loc)
:m_loc(loc)
{

}

EntityLocation::EntityLocation(Ref<LocatedEntity> loc, const WFMath::Point<3>& pos)
: m_loc(loc), m_pos(pos)
{

}
#endif //STUB_EntityLocation_EntityLocation

#ifndef STUB_EntityLocation_pos
#define STUB_EntityLocation_pos
const WFMath::Point<3>& EntityLocation::pos() const
{
    return m_pos;
}
#endif //STUB_EntityLocation_pos

#ifndef STUB_EntityLocation_isValid
#define STUB_EntityLocation_isValid
bool EntityLocation::isValid() const
{
    return (m_loc && m_pos.isValid());
}
#endif //STUB_EntityLocation_isValid
