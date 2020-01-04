Minds      {#minds}
=====

The code for simulating minds in Cyphesis, AI if you so will, uses some concepts which needs to be explained.

## Out of process

The main concept is that all mind code handling is done outside of the main server process. Thus the server process is only responsible for handling of the world simulation, message routing as well as bookkeeping tasks related to entities, rules, accounts and so on. There are a couple of advantages to this approach:

* It lets us better take advantage of multi core system (which is the norm since a couple of years) without having to rewrite the single threaded nature of the Cyphesis server. Each AI client is single threaded, but multiple clients can run in parallel.
* It makes the server less complex, since AI execution is handled separately. This makes it easier to reason about and work with.
* It allows for an improved workflow when developing AI code. The AI code can now be altered and restarted without the server being touched. This allows for more a more efficient iterable approach.
* More avenues for alternative AI clients are now open. The current AI code uses mainly Python. It's now much easier to implement an AI client in a separate language (Prolog anyone?).
* The door is now open for letting AI clients run on separate machines. Say, in a cloud. While the current code requires the AI client to connect using a local socket, implementing support for remote connections should be trivial.

The main downside is that the protocols needed for letting outside minds communicate with the server are now more complex.

## cyaiclient

The command line tool "cyaiclient" launches a new AI client and connects to a locally running Cyphesis server, over the local socket found at PREFIX/var/cyphesis_cyphesis.sock. Multiple clients can be connected at the same time, although currently only the last connected one will receive mind requests. When a client is disconnected the server will reassign the minds that were handled by that client to the latest connected client, if any such exists. This scheme is designed to make iterative development of ai clients easier.
Cyphesis will by default create one AI client instance at startup, as a child process.

### AI client registration

When a client that wants to act like an AI client is connected it needs to inform the server about this. This is done by setting the attribute "possessive" on the client itself. Once this is done the server will mark the client as a possession client and start to route mind requests to it.

### Possession requests

As soon as an entity with a mind is created the server will look for any registered "possessive" clients, i.e. client that can handle the mind of the entity. If any such is registered, one will be selected (currently the last registered one) and a Possess op will be sent to the client. This op will contain both the id of the entity, as well as a secret key. The client then extracts the entity id along with the key and uses these to take possession of the entity. This is done using the regular method for taking control of a character, i.e. through a Look op, albeit with the possession key attached.

From this moment on the AI client then acts as any other client.

## AI properties

The AI is controlled by a couple of different properties on the entity. These are described below.

### _goals property

The "_goals" property defined which goals are used for the entity. It's a list where each entry is a map. The goals are created in sequence, and checked in sequence (so that earlier goals gets checked first).
Each map entry has a "class" string property which defines the Python class to use for the goal. In addition there can also be a "params" map property containing various parameters. These parameters needs to match the named parameters of the Python class' constructor.

For example, here's a definition for a goal which makes the entity welcome all new entities that are human.
```xml
<list name="_goals">
    <map>
        <string name="class">mind.goals.humanoid.mason.Welcome</string>
        <map name="params">
            <string name="message">Welcome traveller to the island of Braga! You look tired and hungry. Come closer and let me introduce this place.</string>
            <string name="what">entity instance_of types.human</string>
        </map>
    </map>
</list>
```

### _knowledge property

The "_knowledge" property contains various things the entity "knows". It's a map where each key is in the format of "<predicate>:<subject>". The AI currently understands the predicates

* about: a string describing something
* location: a list of either three floats, or an addition string (to hold an id of an entity)

An example of two bits of knowledge, with the first being an "about" referring to "this place", and the second being a "location" by the name of "w1". 

```xml
<map name="_knowledge">
   <string name="about:this place">This is the main docks of this small island. Most visitors arrive here by boat.</string>
   <list name="location:w1">
       <float>0</float>
       <float>0</float>
       <float>0</float>
   </list>
</map> 
```

### Relations

To handle relations between different entities a system is in place using the two related concepts of "disposition" and "threat". Together these two determines whether an entity should be ignored, fought or fleed from.

#### disposition

"Disposition" determines how well a character likes another entity. It's a float value, where > 0 means that the character is in favour. It it's < -1 it means that the character hates the entity, and will either fight of flee from it (depending on the "threat" value).

#### threat

"Threat" determines how the character should behave when the disposition for an entity is < -1 (i.e. it's an "enemy"). For entities that have a positive threat it means that the character should try to flee from them. For negative values the character should instead attack.

#### _relations property

The "_relations" property defines the rules for how these values are computed per entity. It's a list containing "rules" entries. Each entry is a map, containing a required string with the key "filter" which defines an Entity Filter.
In addition there can be float attributes for both "disposition" and "threat".

When a new entity is encountered all rules are checked in order, and the total sum for disposition and threat is calculated by adding up all matching entries.
An example of a rule which makes the character flee from adversaries.

```xml
<list name="_relations">
    <map>
        <string name="filter">entity.type=types.wolf|types.goblin|types.bear|types.skeleton|types.lych</string>
        <float name="disposition">-10</float>
        <float name="threat">10</float>
    </map>
</list>
```  

## Thoughts protocol

Whenever the server or any external authoring client needs to interact with the mind of an entity the Thoughts protocol is used. This is a simple protocol which allows for inspection of thoughts.
At the base is the Thought op, which wraps other ops.
To interact with the mind of an entity a Thoughts op must be sent both to and from the entity. This means that if an external client needs to do this the Relay mechanism must be used to "fake" that the op is coming from the entity itself.
The inner operation, which the Thoughts operation wraps, determines the action. These actions are supported:

* Look: inspect a specific thought/goal

### Look
A Look op is used to inspect thoughts. This is meant to be used by world authors and those that are interested in observing the world.
The current implementation only allows for inspection of goals, using their ids.
Each thought to be inspected is supplied as an args element.
For each thought a "report" is returned in an Info op.

Outgoing
```
{
	"parent": "thought",
	"to": "1",
	"from": "1",
	"objtype": "op",
	"args": [
		{
			"parent": "look",
			"args": [
				{
					"id": "goal1"
				}
			]
		}
	]
}
```
Incoming
```
{
	"parent": "thought",
	"to": "1",
	"from": "1",
	"objtype": "op",
	"args": [
		{
			"parent": "info",
			"objtype": "op",
			"args": [
				{
					"id": "goal1",
					"report": {
						"description": "a description of the goal",
						"fulfilled": 1,
						"variables": {
							"a var": "a value"
						}
					}
				} 
			]
		}
	]
}
```
