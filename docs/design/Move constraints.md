# Move constraints

__Status__: implemented

This document describes the way that physically moving entities will be constrained in Worldforge.

Whenever an entity tries to move another entity there are constraints that need to be checked. This needs to be done for all entities, with the ability for world authors to provide custom rules and
overrides.

When an entity tries to move another entity there are a couple of checks that need to be made.

1. Can the mover reach the entity? This is determined by all the domains that exist between the entity and the mover.
2. Does the entity belong to a domain? Only entities that are direct children of domains are allowed to be moved.
3. Does the domain allow the entity to be moved?
4. Does the entity itself allow itself to be moved?
5. Is the mover allowed to move entities?
6. Can the mover reach the destination, where the entity is to be placed?
7. Does the domain to where the entity is moved allow it?

The protocol needs to handle both movement ops coming from the simulation as well as from normal entities. For movement ops coming from the simulation there also needs to be methods by which normal
rules can be bypassed. For most simulation cases the domain rules should apply, such as preventing a too large entity being moved into a small sack. But if there’s a use case for forcing that to
happen anyway there should be mechanisms by which the simulation can force it.

Moving entities should also honour “amount” if they are stackable.

Currently movement of the entity itself is also handled by Move ops. It needs to be investigated whether this is a good solution in the long run. It’s possible that a new op such as Propel might be
used for those cases instead.

Question: which entity should receive the Move op? The Thing that’s moved? It’s parent domain entity? The entity does the moving?

Probably to all involved, depending on the amount of checks needed.

When the Move op is received by the Thing, the movement will be carried out.

If the Move op is sent to the entity being moved into, it will perform checks, and then send it on to the Thing if the checks pass.

If the Move op is sent to the entity being moved out of, it will perform check, and then send it on to the entity being moved to.

If the Move op is sent to the entity performing the moving it will perform checks and then send it on the entity being moved from.

In this way there’s a possibility to override some checks by sending the Move op directly to one of the entities in the chain directly.

# Possible flows

These are a couple of possible flows.

## Simulation moves entity between non-containers

The simulation will move entity “Thing” from its current parent “Parent 1” to another entity “Parent 2”.

A Move() op is sent from “Thing” to “Thing”. First arg is an Entity with id “Thing”, “loc” “Container 2”. Movement will happen without any extra checks.

## Simulation moves entity between containers

The simulation will move entity “Thing” from its current parent “Container 1” to another entity “Container 2”. The containers both have domains attached. Movement should only succeed if “Container 2”
has space available. No distance checks should occur.

A Move() op is sent from “Thing” to “Container 2”. First arg is an Entity with id “Thing”, “loc” “Container 2”. “Container 2” will check if movement into it is allowed, and then send the op to “Thing”
which will do the movement.

## Simulation force moves entity between containers

The simulation will move entity “Thing” from its current parent “Container 1” to another entity “Container 2”. The containers both have domains attached. No checks should occur.

A Move() op is sent from “Thing” to “Thing”. First arg is an Entity with id “Thing”, “loc” “Container 2”.

## Simulation moves entity within domain

The simulation will move entity “Thing” from its current position in “Container 1” to anotherposition. The container has a domain attached. No distance checks should occur.

A Move() op is sent from “Thing” to “Thing”. “Thing” will check if “Container 1” allows the movement.

## Actor moves entity between containers

An entity “Actor” tries to move another entity “Thing” from its current parent “Container 1” to another entity “Container 2”. The containers both have domains attached. Movement should only be allowed
if “Actor” can reach both “Thing” and “Container 2” and “Container 2” has space available.

The Mind of the Actor sends a Thought(Move) op. The inner argument contains an Entity with id “Thing”, “loc” “Container 2”.

The “minds” property will receive the Thought and perform checks.

First it needs to check if the Actor can reach both “Thing” as well as the location in “Container 2”.

The Move op will first be sent from Actor to “Container 1”, which will check if movement away from it is allowed. The op will then be sent from “Container 1” to “Container 2” which will check if
movement into it is allowed. And finally the op will be sent to “Thing” which will perform the move.

## Actor moves entity between within domain

An entity “Actor” tries to move another entity “Thing” from its current position in “Container 1” to another position. The container has a domain attached. Movement should only be allowed if “Actor”
can reach both “Thing” and the destination location, and Thing can be moved.

The Mind of the Actor sends a Thought(Move) op. The inner argument contains an Entity with id “Thing”, and a new “pos”.

The “minds” property will receive the Thought and perform checks.

First it needs to check if the Actor can reach both “Thing” as well as the new location in “Container 1”.

The Move op will first be sent from Actor to “Container 1”, which will check if movement within it is allowed. The op will then be sent from “Container 1” to “Thing” which will perform the move.

## Constraints

It should be possible to set constraints on how an entity can be moved. Either on the entity being moved or the destination or the current location.

This can either be done by Python scripts, or by more efficient entity filtering done through the C++ code.

Typical constraints:

* Only allow children of a certain type.
* Only allow a certain number of children.
* Only allow children of some size.
* Don’t allow moving an entity if it’s a certain weight.
* Don’t allow moving an entity if it’s a certain weight and the mover lacks strength

When a movement occurs there are four involved entities.

The Mover, i.e. the entity which performs the movement.

The Thing, the entity being moved.

The Container, i.e. the Thing’s current location..

The Destination, the new location of the Thing.

In some cases some of these will be the same entity.

These properties are introduced as constraints:

* “mover_constraint”: placed on the Mover
* “move_constraint”: placed on the Thing
* “contain_constraint”: placed on the Container
* “destination_constraint”: placed on the Destination

These constraints are checked whenever an entity is moved. 
