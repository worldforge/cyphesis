Movements and sights      {#movement_sights}
=====

Movement and sights are determined through a couple of rules and concepts. This document aims to explain how it works.

## Domain

Each entity can have a Domain instance attached to it. The Domain determines both how contained things are allowed to move, and how sights are calculated.
Only the entities directly contained in the Entity are considered.

### Contained entities

When calculating movement and collisions for an entity, only entities belonging to the same Domain (i.e. those that have the same parent entity) are considered. Consider this example:

A World entity contains a Human and a Chest. The Chest in turn contains a Helmet.
When the Human moves, collision detection is only carried out between the Human and the Chest, and never between the Human and the Helmet.

When calculating sights, each Domain must be queried in order. So, building on our previous example, if the Human sends a Look op to the Helmet, sight is determined by first checking with the World's domain if the Human can see the Chest. If this is allowed, the Chest's Domain is then queried if the Human is allowed to see the Helmet. If the Chest didn't have a Domain attached any child entities are visible is the parent entity is visible. If it has a Domain this can for example be a "chest" domain which first checks the open/closed state of the Chest, and then checks the distance of the Human from itself.

Note that the movement rules means that any entities that are expected to interact with one other physically must belong to the same world. So, consider an open crate entity. If items are to freely be put into the crate and then be able to interact with the world (i.e. falling out if the open crate is tipped) they must all belong to the same parent as the open crate itself. However, if items are to be stuck to the open crate, and not be able to interact with the world in any way, they should instead be entities contained by the crate.

### Physical domain

The PhysicalDomain class is used for top level worlds. This kind of domain does all of the heavy lifting in the simulation. It knows how to handle the Terrain property and how to apply physical rules to the entities.

The standard physical rules that applies to the real world also applies to the Physical Domain.

### Inventory domain

Any character which has an inventory can expose this through the Inventory Domain. No physical rules are applied to contained entities: velocity is ignored and no collision detection occurs. When an outside entity looks as an entity contained in the Inventory Domain sight is determined by checking if the entity is outiftted. I.e. if the entity in question has been registered in the Outfit property. Only those entities that are outfitted are visible to outside entities.

In addition, any item that is added is checked if it's stackable, and if so an attempt to stack it with existing similar entities is performed.

### Container domain

Used for any container such as a chest, barrel or crate. Entities are physically constrained to the size of the container, and physics applies, but velocity is ignored. I.e. things are arranged, but nothing moves.
Sights are determined both by checking the closed/open state of the container, and by checking the distance to the observing entity. Only entities standing close to the container should be able to look into it.

### Stackable domain

Attached to entities that stack. Whenever an item is moved into this domain, a check is performed if it's identical to the entity itself. If so, it's "stacked", which means that the "amount" of the entity is increased while the entity that was moved is destroyed.

Conversely, when a stacked entity is moved an extra "amount" attribute can be specified, which allows the "stack" to be split. 

### Void domain

Used whenever an entity needs to be placed in a void where they can't act. All attempts at movement or sights are ignored while in the void domain.

This type of domain can be used for temporarily placing entities that have died and are waiting for respawning.

### Custom domains

Apart from these domains there's room for more specialized ones, such as an "endless bag".

## Movement

The following only applies to Physical Domains.

Each entity has a "position", an "orientation", a "velocity" and an "angular velocity". The values determine how the entity moves through the world, and are communicated to clients.

However, when a character needs to move, this is done by setting the "_propel" property. This property determines how much the character is moving continously on its own. The resulting "velocity" differs from the "_propel" value, since collision detection and other physical constraints are taken into account.

Furthermore, the final velocity is calculated by multiplying the "_propel" value with one of the "speed_ground", "speed_water" and "speed_flight" values. Which value to use should be decided by the domain depending on where the character is situated and in what state. An absence of such values, or 0, means that the entity can't move throughout the specified medium.

The "_propel" property is therefore normalized, in the sense that a value with magnitude of 1 means "full speed". Checks are done so that normal characters shouldn't be allowed to set values with larger magnitude than 1.









