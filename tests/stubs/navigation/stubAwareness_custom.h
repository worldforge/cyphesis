//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
#ifndef STUB_Awareness_Awareness
#define STUB_Awareness_Awareness
Awareness::Awareness(const LocatedEntity& domainEntity, float agentRadius, float agentHeight, IHeightProvider& heightProvider, const WFMath::AxisBox<3>& extent, int tileSize )
    : mHeightProvider(heightProvider), mDomainEntity(domainEntity)
{

}
#endif //STUB_Awareness_Awareness
