Usage and tasks      {#usage_and_tasks}
========

In order for entities to interact with the world they need to use other entities, or themselves. The "usage" system is the primary method for driving player interaction and gameplay. This document describes how it, along with the "task" framework works. 

An example could be a player which uses a shovel to dig in the ground, something which will spawn a five second task which upon completion will result in hole being dug, and a pile of dirt being created.

## Usages

Entities that can be used contain a specific "usage" property. This is a core property contained in the simulation core of the server.

The "usage" property describes the various way a certain entity can be used, and the constraints for the usage. Whenever a client wants to perform a usage they need to send a "use" operation to the entity, containing information about the usage. The simulation will take care of making sure that all the constraints are fulfilled, and then call a specified Python script.

The Python script contains the game specific logic for the usage. In most cases however a "task" will be spawned.

### "usages" property

The "usages" property (and also "_usages" for usages only available to the entity itself) contains all data needed for both inspection and execution of usages.

It's a map, where the top level keys are the operations used to trigger the various usages. Note that it's required that these are varlid operations, defined previously in the rules.

An example of a usage for a digging action attached to a shovel is shown here.

```xml
<map>
    <map name="dig">
        <string name="name">Dig</string>
        <map name="params">
            <map name="targets">
                <string name="type">entity_location</string>
                <string name="constraint">entity instance_of types.game_entity &amp;&amp; actor can_reach entity_location with tool</string>
                <int name="min">1</int>
                <int name="max">1</int>
            </map>
        </map>
        <string name="handler">world.objects.tools.GenericTool.use</string>
        <string name="constraint">get_entity(actor.attached_hand_primary) = tool</string>
        <string name="description">Dig with shovel.</string>
    </map>
</map>
``` 

The properties for each usage is described below:

**name** A name for the action, to show in client UIs. Optional.

**description** A longer description to show in client UIs. Optional.

**handler** A Python function which will be invoked when the usage is activated. The Python function takes one parameter, a UsageInstance object, which is described below. Required.

**constraint** An Entity Filter constraint which will be verified first. In the above example the usage can only be activated if the actor has the shovel entity attached to their primary hand. Optional.

**params** A map of parameters that the client needs to send. The format of these is described below.

#### Parameters

A usage can contain optional parameters. These are defined in a map, where the key denotes the parameter name. In the above example there's only one parameter called "targets".

**type** The type of the parameter. Can be one of "entity", "entitylocation", "position" or "direction". "entity" is an entity id, "entitylocation" is both an entity id as well as position, "position" is a 3d position and "direction" is a 3d direction. Required. 
 
**constraint** An Entity Filter constraint which will be verified first. In the above example the parameter needs to be something which is a game_entity and can be reached by the actor with the tool. Optional.

**min** The minimum number of arguments that the client must provide. For optional parameters set this to 0. If omitted this will be 1 by default.     
     
**max** The maximum number of arguments that the client must provide. If omitted this will be 1 by default.     

### Python handler

The Python function which acts as the "handler" takes one argument, which is a UsageInstance object. This object has these properties and methods:

**is_valid()** Performs validation and returns True if the usage was valid. All usages have already been validated to be valid when the handler is called, but if the handler performs changes to the state it can be called again to verify things. This is also useful when the UsageInstance is used within a Task script.

**get_arg(key, index)** Gets one of the supplied arguments to the usage. The "key" is a string and denotes the argument name, and index is an integer and denotes the index of the argument (in those cases where an argument can have multiple values).

**actor** Gets the entity that performs the usage.

**tool** Gets the entity that contains the usage.

**definition** Get the definition of the usage. This is an object which contains two properties, "description"  and "constraint". The latter returns an Entity Filter instance.  
 
# Invoking usage

To invoke a usage the client needs to send a "use" op. An example for an entity with id 2 to use a shovel with id 3 to dig in the earth of an entity with id 1 would look like this.

```
from: 2
args
  args
      id: 3
      parent: root_entity
      objtype: obj
      targets
          id: 1
          objtype: obj
          parent: root_entity
          pos
            : -1
            : 10
            : 4
  parent: dig
  objtype: op
parent: use
objtype: op
``` 

## Protected usages

Some usages can only be activated by the entity itself. These are placed in the "_usages" property. A typical such usage is a human entity can can "punch" or "kick" enemies.

## Tasks

Any long running action in the world is represented by a "task". The task is in most cases placed on the entity that invoked the usage. Active tasks for an entity are handled by the specific "tasks" property. This is a core simulation property, which in turn offloads any logic to game play specific Python scripts.

Tasks can in most cases be stopped, and can in some cases run indefinitely. Some tasks can also have additional "usages" which can be invoked on them while they are active.

### "tasks" property

The "tasks" property contains a map of active tasks.

### Invoking task usage

Clients can invoke usages for tasks by sending a "use" operation. The example belows show an entity with id 2 invoking the "stop" usage on the "dig" task.

```
from: 2
args
  args
    id: stop
  id: dig
  objtype: task
parent: use
objtype: op
``` 

### Task script

The logic that drives Tasks is defined in Python scripts. These scripts must inherit from the ```server.Task``` base class. This class has these properties and methods:

**irrelevant(message)** Marks the task as completed/irrelevant. An optional message can be submitted.

**get_arg(name)** Gets one of the arg from the usage instance. This a short cut for "self.usage.get_arg".

**obsolete()** Gets whether the task is marked as irrelevant already.

**usage** The UsageInstance which triggered this task.

**actor** Gets the actor performing the task. Short cut for "self.usage.actor".

**tool** Gets the tool being used. Short cut for "self.usage.tool".

**definition** Gets the definition for the usage. Short cut for "self.usage.definition".

**op** Gets the operation that triggered the usage. Short cut for "self.usage.op".

**progress** Gets or sets the progress of the task, a float [0..1].

**duration** Gets or sets the duration, as a float, if defined. Else None.

**tick_interval** Gets or sets the tick interval, as a float.

**name** Gets or sets the name of the task.

**usages** Gets or sets the task usages definitions as a list of strings.

These methods are called from the simulation on the Task script instance:

**setup()** Called when the task is first created.

**tick()** Called at tick_interval.

**completed()** Called after the task has been completed.

