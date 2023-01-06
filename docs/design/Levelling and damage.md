# Level and Xp

__Status__: implemented, with base functionality and examples

Add a concept of “level” and “xp” (experience).

A world could be created without these concepts, but it’s hard to balance it. Other options would be to have a TES/Fallout style system where you gain points which you can distribute as you like. This
can also be combined with levels (i.e. allowing the player to distribute points gained from levelling).

Each character has a “level” property and a “Levelled” script.

The script will take the level into account and apply various modifiers to the entity to boost its stats.

The experience amount determines the level. Typically there would be some curved function determining the next experience level.

When fighting monsters an entity would only get xp gains if the monster is of a level higher, or within the vicinity of the entity’s level. This makes it so that there’s not much to gain from fighting
too low entities.

Monsters that give experience when killed have an “__xp_yield” property.

Weapons are levelled, so that they can only be used by characters that are of the same level or higher.

# Damage model

Initially we’ll use a damage system where the status of entities is [0..1]. At 0 they are killed.

Damage taken is calculated by looking at the “damage” value of the Hit op and subtracting any “armor” value that the entity has.

The effect is that if the entity has a very high “armor” value no damage will ever be taken.
