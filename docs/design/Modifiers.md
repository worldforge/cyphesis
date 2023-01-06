# Ways of handling modifiers.

__Status__: implemented

A "modifier" is something which affects the properties of the entity.

Every property has a "base" value.

All properties can be modified, not only numerical. But the way non-numerical properties can be modified is limited compared to numerical.

Some properties can set flags which disallows them being modified.

Should the modifiers be handled through a new system, or can we use an ModifiersProperty? Probably a new system, "modifiers". But perhaps exposed through an ModifiersProperty.

For numerical properties we only allow additions and subtractions. But these can be based on a ratio of the base value. I.e. an addition could be for a certain number, like "10". But it could also be
a percentage, like "20%". In the latter case the percentage must be based on the "base" value of the property, since mixed additions and multiplications don't stack.

We already have a system in place for combining properties in type nodes. Should we reuse that? Would allow altering of maps and lists too.

Should we also allow modifiers whose value is calculated on the fly through a script?

Requirements:

* An entity could have an item in its inventory that changes its properties.
* An entity could have an item attached which changes its properties.
* An entity can have modifiers (curses, blessings, spells etc.) which changes over time. Some should be visible even when not attached.
* Every modifier must be attributed to another property or entity. They must be accountable. When removing the "owner" property/entity the modifier is removed automatically.

# Suggested solution

Add a new "m_modifiers" field to Entity. This contains C++ logic for handling all modifiers. It's exposed through a read only ModifiersProperty, exposed through "modifiers" or "_modifiers" or "__
modifiers".

Change code so that "set" calls on properties take modifiers into consideration. When persisting entities only store the base value. When sending properties to clients always send the final value.
When handling properties in rules always use the final value.

"get" calls on properties always return the final value. But there must be ways for rules to get access to the base value (for example when we want to increase a base value we must know it).

This will make the system more complex since there will now be a disconnect between setting a value and getting it, with different values being produced.

A new Modifiers class will handle all modifiers. It needs to make sure that properties are changed when modifiers are added, altered and removed.

Modifiers are added to an entity through other entities, which are added as child entities. So, for example a sword might have a modifier which is only applied when it's wielded. A "curse" entity can
also have modifiers, which always are applied. (Note: add extra properties which will allow curses and blessings to be seen when in inventory even if they are not attached).

Some modifiers are governed by scripts (for example a helmet which only confers a blessing during night, or a curse which only is active when in proximity to something). This can be done through
standard Tick handlers on the owning entity.

Entities that are only used for modifiers should have logic to disallow them ever being moved. When the parent entity is deleted the entity is deleted along with it.

Entity filters can be used to limit the kind of entities they can be children of.

Should modifier entities be observers of their parents?

Possible situation: a player wields a sword. They also have a magic hat which confers a modifier on the sword (flammability). As long as the hat is worn a modifier entity is added as a child to the
sword. If any of the sword or hat is moved the modifier is removed.

Modifiers need to be recognized by the client and rendered accordingly. New property, or reuse of visualization prop?

## Modify self

When doing leveling we want a modifier to be applied to the entity as a result of a script run on the entity itself. The modifier should be applied through a "modify_self" property. Each entry here is
tagged with a string, to allow for multiple scripts to add such modifiers. An alternative approach would be to create a hidden child entity which contained the modifiers, but this would probably be
too complex.

Modifiers

* Append \
  Adds something to the end of the value. For numbers this is just an addition, but for strings and lists the new entries are added to the back.
* Prepend \
  Adds something to the start of the value. For numbers this is just an addition, but for strings and lists the new entries are added to the front.
* Subtract \
  Removes something. For numbers this is a subtraction.
* Default \
  The value is replaced.
* Add fraction  \
  For numbers, the base value is multiplied, and the result added.
