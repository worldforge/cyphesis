# Spawning

__Status__: implemented with Archetypes and Spawners, but lacking scripts

Creating a new player entity should be configurable from the rules, without having to alter any C++ code.

In addition, the values that the user has to enter should be configurable, so that they don’t need to be hard coded into the client.

The current solution requires a certain set of values, such as “sex” and “name”. This should instead be dynamically communicated from the server to the client.

At the same time we want to support the use case of spawning in a “creator” instance. The “creator” instance could be created at any place on the server, but we want to be able to support it being
created at one of the spawn points.

Upon logging in the configuration options for new entities should be sent to the client.

Upon creation of new entities the client should send a Spawn op which should be handled by script code put on an entity in game. This requires that extra checks are put in place to make sure that it’s
routed to the right place, without any possibility of hacking etc.

Creation of a new player entity should be handled by scripts on the Creator Entity.

Client sends a Spawn op directed to the Spawn Entity. The Account code will make sure to verify that the Spawn Entity is registered as a Spawner, and send it on. The Spawn op looks like a regular
Create op, but the Spawn Entity’s script code will make sure that the arguments provided are matched with what is required and allowed, before in turn creating an actual Create op from it.

For the “creator” entity creation (i.e. when spawning in as an admin character) we need to support the following flow:

1. Use logs in as Admin
2. User gets a list of available spawns, along with entity id.
3. User performs a Get on the spawn, thus getting its location as well as parent.
4. User creates a new “creator” instance directly on the server, using the same location as the spawn.

This approach bypasses the Spawn Entity’s spawning code, while at the same time letting the Admin entity be created at the same place as regular entities would be.
