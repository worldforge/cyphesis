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

### Proxy mind

On the server side each entity with a mind also has an instance of ProxyMind attached to it. The purpose of the ProxyMind is to keep tabs on the state of the mind, both when an external AI client is attached as well as when it's completely detached. All thoughts are stored in the proxy mind.

Any authoring client can interact with the proxy mind by inspecting its thoughts and altering them, even though they will in this state only be static. As soon as an AI client connects the stored thoughts will be sent to the client. The AI client will at regular intervals also send its current thoughts to the server, thus making sure they are persisted in the event of a crash or shutdown.

### AI client registration

When a client that wants to act like an AI client is connected it needs to inform the server about this. This is done by setting the attribute "possessive" on the client itself. Once this is done the server will mark the client as a possession client and start to route mind requests to it.

### Possession requests

As soon as an entity with a mind is created the server will look for any registered "possessive" clients, i.e. client that can handle the mind of the entity. If any such is registered, one will be selected (currently the last registered one) and a Possess op will be sent to the client. This op will contain both the id of the entity, as well as a secret key. The client then extracts the entity id along with the key and uses these to take possession of the entity. This is done using the regular method for taking control of a character, i.e. through a Look op, albeit with the possession key attached.

Once the ai client has taken possession of the entity, the server will send all of the stored thoughts. This is done using the Thoughts protocol.

From this moment on the AI client then acts as any other client.

## Thoughts protocol

Whenever the server or any external authoring client needs to interact with the mind of an entity the Thoughts protocol is used. This is a simple protocol which allows for editing and inspection of thoughts.
At the base is the Thought op, which wraps other ops. The usage is very much similar to the standard protocol for setting and inspecting entity protocol.
To interact with the mind of an entity a Thoughts op must be sent both to and from the entity. This means that if an external client needs to do this the Relay mechanism must be used to "fake" that the op is coming from the entity itself.
The inner operation, which the Thoughts operation wraps, determines the action. These actions are supported:

* Set: insert or update a thought
* Delete: delete one or many thoughts
* Look: inspect a specific thought/goal
* Get: get one or many thoughts

### Set
A Set op is used to insert or update a thought. Each argument element represents on thought. If an argument element has an "id" set it will update an existing op with the same id, or create a new one. An argument element without "id" will just add a new thought. The following will set two thoughts, where the first one is a goal with the identifier "goal1" and the second one is a standard knowledge entry.

Outgoing
<pre>
{
	"parents": ["thought"],
	"to": "1",
	"from": "1",
	"args": [
		{
			"parents": ["set"],
			"objtype": "op",
			"args": [
				{
					"id": "goal1",
					"goal": "do_something()",
				},
				{
					"predicate": "about",
					"subject": "village",
					"object": "This is the village."
				} 
			]
		}
	]
}
</pre>

### Get
A Get op is used to get thoughts.
An empty Get op will return all thoughts, both knowledge and goals.
The thoughts will be returned as a Set op. The idea here is that this should be idempotent; if the Set op is then sent back to the AI client it should restore the thought. As such, it can be used for persistence.

Outgoing
<pre>
{
	"parents": ["thought"],
	"to": "1",
	"from": "1",
	"objtype": "op",
	"args": [
		{
			"parents": ["get"]
		}
	]
}
</pre>
Incoming
<pre>
{
	"parents": ["thought"],
	"to": "1",
	"from": "1",
	"objtype": "op",
	"args": [
		{
			"parents": ["set"],
			"objtype": "op",
			"args": [
				{
					"id": "goal1",
					"goal": "do_something()",
				},
				{
					"predicate": "about",
					"subject": "village",
					"object": "This is the village."
				} 
			]
		}
	]
}
</pre>

Alternatively thoughts can also be filtered. This is done by submitting an argument to the Get operation. Currently a match will only be done on the existence of keys in the element, but its expected that this should be extended with regexp search on the value (which currently are ignored).

Outgoing
<pre>
{
	"parents": ["thought"],
	"to": "1",
	"from": "1",
	"objtype": "op",
	"args": [
		{
			"parents": ["get"]
			"objtype": "op",
			"args": [
				{
					"goal": "this text is currently ignored",
				} 
			]
		}
	]
}
</pre>
Incoming
<pre>
{
	"parents": ["thought"],
	"to": "1",
	"from": "1",
	"objtype": "op",
	"args": [
		{
			"parents": ["set"],
			"objtype": "op",
			"args": [
				{
					"id": "goal1",
					"goal": "do_something()"
				}
			]
		}
	]
}
</pre>

### Delete
A Delete op is used to delete thoughts.
An empty Delete op will delete all thoughts, both knowledge and goals.
A delete op with arguments will delete matching thoughts, per argument.

Outgoing to delete all
<pre>
{
	"parents": ["thought"],
	"to": "1",
	"from": "1",
	"objtype": "op",
	"args": [
		{
			"parents": ["delete"]
		}
	]
}
</pre>


Outgoing to delete single
<pre>
{
	"parents": ["thought"],
	"to": "1",
	"from": "1",
	"objtype": "op",
	"args": [
		{
			"parents": ["delete"],
			"args": [
				{
					"id": "goal1"
				}
			]
		}
	]
}
</pre>

### Look
A Look op is used to inspect thoughts. This is meant to be used by world authors and those that are interested in observing the world.
The current implementation only allows for inspection of goals, using their ids.
Each thought to be inspected is supplied as an args element.
For each thought a "report" is returned in an Info op.

Outgoing
<pre>
{
	"parents": ["thought"],
	"to": "1",
	"from": "1",
	"objtype": "op",
	"args": [
		{
			"parents": ["look"],
			"args": [
				{
					"id": "goal1"
				}
			]
		}
	]
}
</pre>
Incoming
<pre>
{
	"parents": ["thought"],
	"to": "1",
	"from": "1",
	"objtype": "op",
	"args": [
		{
			"parents": ["info"],
			"objtype": "op",
			"args": [
				{
					"id": "goal1",
					"report": {
						"description": "a description of the goal"
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
</pre>
