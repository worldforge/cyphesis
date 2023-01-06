# Actions

__Status__: implemented, thought not "pose"

Devise a new way for allowing actions performed by entities to be represented in the clients.
Optionally also move "tasks" into "_tasks" and provide a "pose" property

## History

The current way actions are presented is by having the client react to Action and Task operations being emitted by the server.

There are a couple of issues with this approach however.

1) If the action or task started before the entity was seen by the client, the client will have no way of knowing that the entity is currently performing an action.
2) For some tasks we want to perform different actions depending on the progress of the task. For example, a simple swing with a sword contains of an initial animation where the sword is drawn back,
   and then a subsequent animation when the strike is performed. These represent two different animations.
3) We might want to use the same animation for multiple types of actions, and vice versa use different animations for the same action, depending on some state or setting.

There's therefore a need for an improved system which would allow all of the above features. Work on such a system would need to be synchronized with both
the client and the server.

An additional issue is that the "tasks" property is public. Preferably we would want this protected as "_tasks".
There's also the issue that there's currently no way to communicate the "pose" of an entity. For example, an entity that is sneaking should have a different pose than normally.
We currently let the client handle any pose changes derived from movement. A character with a certain velocity would use the "walking" animation, and a quicker character would use the "running"
animation.
We should consider whether we want to keep this setup of if we instead want this to be completely governed by the server.

One question is whether we want to separate the "action" property from the "pose" property, or whether they in fact are the same.

## Use cases

* Cutting down a tree. A "chopping" action is played throughout the whole task. No movement should be allowed during this action.
* Drawing a bow and releasing it. A "draw" action is first played. Then the task is either abandoned or a "release" action is played. Movement is allowed. The "draw" action has no end, while the "
  release" has an end time.
* Raising a sword, and then swinging it. A "raise" action is first played. Then the task is either abandoned or a "swing" action is played. Movement is allowed. The "raise" action has no end, while
  the "swing" has an end time.
* Pointing at a specific location. Movement should be allowed. This action should have an end time.
* A character looks at an entity or a specific location. This action should be combined with multiple other actions. (this use case indicates that we need to allow for multiple actions)


* An entity is dead and should lie down on the ground in a "dead" pose.
* An entity is sneaking and should move in a "sneaking" pose.
* An entity is winded and should move in an "exhausted" pose.

## The "actions" property

One proposed solution is to add a new property, named "actions". Any actions that are to be performed will be communicated through entries in this property.

It's to be provided either as a map or as a list. Each action should always have a start time, and an optional end time. The "start" time is important as it allows clients that get appearance for
entities to properly show the correct state of the action.

Actions can have extra properties. This for example would allow for an entity to point in a direction, where the direction/position is supplied as parameter.
Likewise, an entity aiming a bow would have the direction communicated.

There are some issues to take into consideration.

1) Should we allow for multiple actions, or just one? The system would be much simpler if only one action is allowed, but we should consider if that allows us to handle all cases.
2) How can we guarantee that no actions are "stuck"? If there's an error in the tasks' logic, how can we make sure that any tasks are abandoned? We need to provide foolproof methods for tying actions
   to tasks, so that the action is guaranteed to be removed if the script code for the task fails, or if the task is abandoned.

## Implementation

The initial implementation will add an "actions" property, which is of map type. The keys define the actions, and each entry is a map. Each entry has a required "start_time" attribute. It might
optionally also have a "end_time" attribute.
For the first iteration we'll skip any extra parameters.

```
actions: {
    punch: {
        start_time: 1243    
    }
}
```
