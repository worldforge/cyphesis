# Visibility

__Status__: implemented

Visibility of both entities and properties is handled by three levels: public, protected and private.

Apart from these general levels the domains can apply additional checks. The physical domain will for example hide things that are too distant.

Entities with extra admin rights can override the visibility rules.

Visibility on properties is determined by their names. Visibility of entities is determined by the "visibility" property.

## Public

A public property can be viewed by all entities.

A public entity can be viewed by all entities.

## Protected

A protected property can only be seen by the owning entity. Such properties are prefixed with "_", for example "_usages".

A protected entity can only be seen by the parent entity.

# Private

A private property can only be seen by the simulation. Such properties are prefixed with "__", for example "__scripts".

A private entity can only be seen by the simulation.
