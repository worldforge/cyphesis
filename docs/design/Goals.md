# Goals

__Status__: implemented

Goals are defined in the “_goals” property, as a list of maps.

Format:

```
_goals: [{
	name: 'Feed'
	sub_goals: [


]
class: 'Search',
params: {
	what: 'entity instance_of types.bread'
}

]
```


## Entity relationships

Every mind relates to all other entities through both “disposition” and “threat”.

The disposition determines whether the entity is a friend or foe. 0 means neutral, 1.0 is a “friend”, -1.0 is a “foe”.

How to handle “foes” is determined by the “threat”. If it’s 1.0 the mind should flee, if it’s -1.0 the mind should attack.

## Examples of behaviour

* Patrol between waypoints
* Eat when hungry
* Gather food
* Sleep when night
* Attack foes
* Flee from foes
* Help friends in fighting
* Cut down wood, bring wood to woodpile
* Plant trees
* Each acorns
* Hunt pigs

### Eat acorn

“Move To food”:

1. “Food entity available”: 100 : -1000
2. “Closest food entity distance = [1..40]”: 100
3. “Character status &lt; 0.6”: 100

“Eat food”:

1. “Acorn food available”: 100 : -1000
2. “Closest food entity distance = [0..1]”: 100
3. “Character status &lt; 0.6”: 100

### Patrol between waypoints

“Move To Waypoint”:

1. “Distance to Waypoint > 1”: 100

“Update active Waypoint”:

1. “Distance to Waypoint &lt; 1”: 100

### Gather food

“Move To food”:

1. “Food in inventory”: -1000
2. “Food entity available”: 50 : -1000
3. “Closest food entity distance = [1..40]”: 50
4. Scale with hunger?

“Pick up food”:

1. “Food in inventory”: -1000
2. “Food entity available”: 50 : -1000
3. “Closest food entity distance = [0..1]”: 50
4. Scale with hunger?

### Eat when hungry

“Eat food”:

1. “Food in inventory”: 100
2. “Character Nutrient == 0”: 300

	
