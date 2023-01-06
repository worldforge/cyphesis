# Container

__Status__: implemented

A container domain has the effect of only allowing inspection of it's contents if the inspector is close enough to reach it.

A client needs to know that it's close enough.

Should we introduce a new kind of property, a per-entity property? I.e. "reachable", with values unique for each entity.

The physics system can handle updating it.

But this would be a new kind of property, with more complex logic.
