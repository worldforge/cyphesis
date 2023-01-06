# Combat

__Status__: implemented

## Requirements

* An actor should be able to try to hit any entity.
* Only some entities should react to hits.
* Combat can be either through melee or projectile weapons.
* An entity could be hit by many different things, such as explosions or runaway carts.
* Clients should get relevant information to see who was hit, for how much damage and by whom.
* Combat should be handled by the game rules, outside of C++. The C++ code should only handle the physics parts.
* When determining the effects of the combat both the actors skills and effects as well as the target's skills and effects should be taken into account.

# Melee combat

A character has a tool attached which allows for a combat task. If no tool the character could have an intrinsic task for punching etc.. when the task is invoked a script handler will be invoked. It
will query the domain whether the strike connected. It will also apply any modifiers if available. This allows the rules to apply things such as skills. Typically this would be handled by the Effects
system. For example if the user has a ring equipped which increases damage.

A Hit op will be generated and sent to the Hit entity. The op contains info about the kind of action, the damage and from where it came.

# Projectile combat

A projectile entity is either removed from the user's inventory (like an arrow) or created (like a fireball). The entity is set to "projectile" mode and given velocity. Extra information about who
fired it and the damage it could do is attached to it through a property (TBD, "projectile_data"). When the entity hits something a Hit op is generated and sent to the entity it hit, as well to the
entity itself. The Hit op contains info of which entity fired the projectile.

When the mode of the projectile changes the "projectile_data" prop is removed.

# Entities that are hit

Entities that are hit are responsible for handling the Hit op. If there's no handler then they aren't affected.

The hit handler will look at the op and apply any damages, after applying any modifiers. This step mirrors the way damage was calculated when the Hit op was created. In this way anything equipped by
the target can affect the damage, for example any armor.

The handler is also responsible for sending out a Sight op containing the Hit op. This allows clients to react.

The handler will also send out Set ops for changing the target's props (typically decreasing status).

The Hit operation sent to entities contain these fields:

from: The entity that caused the hit. Could be a sword, or an arrow, or a character in the case of unarmed combat. Note that this is just the entity that physically hit the character; not the one that
“caused” the hit.

to: The receiving entity, as standard.

The first args is an Entity containing:

id: The entity which caused the hit. This is the entity which caused the hit.

damage: A value to be applied to cause damage.

type: A string representing the type of hit. The code can then run extra logic depending on the type (like “slash”, or “explosion”).

# Projectiles

When a projectile hits something a Hit op will be sent both to the entity that was hit as well as to the projectile itself. Different projectiles will handle this in different ways. For arrows and
quarrels we want the projectile to embed itself in whatever it hits. For bullets we want the projectile to just disappear. For a fireball we want it to spawn an explosion and then disappear. All this
should be handled by scripts attached to the projectile.
