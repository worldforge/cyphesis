# Using entities

__Status__: implemented

Entities must have a “usages” property. This is a map of entries. The key for each entry defines the operation used, and the value is a map of the format:

```
targets: entity filters for possible targets (optional)(removed)
consumes: a list of filters for consumed entities involved (optional)(removed)
description: (optional)
args: optional arguments (optional)
handler: a script reference
constraint: an entity filter constraining the entities that can use the tool (optional)
name: (optional) what to show to the client, in a menu etc.
```

Clients send a Use op to the used tool.

“from” is the user.

First argument is the action operation.

The number of arguments of the op must match the combined “targets” and “consumes” lists. First the targets are specified, and then the consumables. For optional empty Entity values are sent.

Property listens to Use ops and calls handler.

# Combat

For melee weapons, a "strike", "cut" or similar op is used. Combat handler checks with Domain if the attempted strike hits, and sends a Hit op to target.

Target has a script ("Hittable.py" for example) which handles the Hit operation and applies the calculations for how much damage is taken. ~~The Hit op is broadcasted, allowing for clients to react.~~

For ranged weapons a "shoot" op is issued to the weapon. The weapon fires an Entity (either getting one from inventory as with bows, or creating one as with wands).

For tools that use other entities the “args” op arguments can contain a reference to the entity to be used (like a bow which uses arrows). Otherwise the consumption should be defined in the handler (
i.e. that a bow automatically uses arrows).

The projectile is handled by the physical domain. Whatever it hits gets issued a Hit op, as with melee.

## Unarmed combat

For entities with intrinsic abilities these are defined in a “_usages” property. These can only be seen and activated by the entity itself. For example a human entity might have a "punch" op.

## Combining entities

For crafting one or many entities are combined. ~~This is defined by the “consumes” attribute.~~

# Constraints

The “constraints” attribute can contain an entity filter which is applied when performing the action. “entity” and "tool" refers to the used tool, and “actor” to the entity performing the action.
Constraints are used for two reasons:

1. To remove the need to perform the same checks in the Python code, even if this is possible.
2. To inform the clients of constraints so that they can provide a better user experience.

# Cooldowns

When using items there’s often a “cooldown” period afterwards, where one cannot perform any other action. A cooldown can either be attached to an entity, or to the actor.

For example, when swinging a sword the actor cannot do anything with the hand that was used to swing the sword for some seconds. This works since the sword can only be used if it’s wielded.

But if there’s a magic item in the inventory which can be invoked without being wielded we instead want the cooldown to be specified for the entity itself.

For the first instance the property “_ready_at_attached” is used. This is a map, where the keys define various attach points (hands mainly), and the values define the time when the cooldown is over (
as float).

For example:

```
_ready_at_attached: {
	hand_primary: 1435.23,
	hand_secondary: 1634.32
}
```

For the second instance the property “ready_at” is used instead. The value is a float defining the time when cooldown is over.

For example:

```
ready_at: 453.43
```

The property “cooldown” describes in seconds how long the cooldown period is.

The server will make sure that the cooldown properties are respected.

If the client tries to perform an action before it's possible a task should be spawned which will perform the task when it's possible.

# Handler

The actual logic for the usage is contained in a Python handler. This is a function which accepts one argument of the type UsageInstance (Python server.UsageInstance). The UsageInstance objects
contain all of the instances that are involved in the usage, as well as methods for checking that all filters and constraints are valid.

As with other Python calls the function can return zero or many Operations. Typically you would want to send some Operations to both other entities as well as back to the user.

# Tasks

Some usages take a little bit longer. For these instances one needs to create a Task instance and attach it to the actor. The task instance must inherit from server.Task.

A task can have a “duration” value set which determines how long it takes until completion.

A task can have a “tick_interval” value set which determines how often it will get callbacks.

One of these, or both, must be set for the task to function. If only the duration is set the tick_interval will be set to match, so that a Tick operation is sent once right before the task completes.

If no duration is set a tick interval must be set to allow the Task to manually update its progress in the Tick callback.

The main way the Python task instance interacts with the server is through the methods “setup(self)”, “tick(self)” and “completed(self)”. All of the methods can return zero or many operations.

“setup(self)” is called when the task has been correctly registered with the actor.

“tick(self)” is called for every tick. This is called at least once. If only the “duration” value is set the tick call will happen once and right before the task completes. A typical use would be to
continuously check that the usage is still valid, for long running tasks.

“completed(self)” is called when the task has completed.

# Examples

A sword. Can hit other things and must be wielded.

```
strike: {
    targets: [ "entity.type instanceof Thing"]
    handler: world.tools.Sword.strike
    constraint: "actor.attached.hand_primary = entity || actor.attached.hand_secondary = entity"
}
```

A bow. Can hit other things, must be wielded and needs arrows which must be in the actor’s inventory.

```
fire: {
    targets: [ "entity.type instanceof Thing"]
    consumes: [ "entity.type instanceof Arrow && actor contains entity"]
    handler: world.tools.Bow.fire
    constraint: "actor.attached.hand_primary = entity || actor.attached.hand_secondary = entity"
}
```

A rope which can be used to tie two entities together.

```
tie: {
    targets: [ "entity.type instanceof Thing","entity.type instanceof Thing"]
    handler: world.tools.Rope.tie
    constraint: "actor contains entity"
}
```

An usage attached to a character, allowing it to hit with fists. Either as “usages” or “_usages”. Only the character itself can perform it.

```
strike: {
    targets: [ "entity.type instanceof Thing"]
    handler: world.creatures.Biped.fist_strike
    constraint: "actor == entity && actor.attached.hand_primary = none"
}
```

A button which can be toggled. In this case we’ll check for reachability in the constraint already.

```
toggle: {
    handler: world.tools.Button.toggle
    constraint: "actor canReach entity"
}
```

A magic scroll which can be invoked multiple times, with cooldowns.

```
cast: {
	handler: world.tools.Scroll.cast
	constraint: "actor contains entity"
}
```

A recipe for baking bread. The recipe will produce different breads depending on the qualities of the supplied entities.

```
bake: {
	handler: world.recipes.Bread.bake
	constraint: "actor contains entity"
	targets: ["entity.type instanceof Bowl"]
	consumes: ["entity.type instanceof Flour && actor contains entity", "entity.type instanceof Liquid && actor contains entity", "entity.type instanceof Salt && actor contains entity"]
} 
