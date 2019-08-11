Entities      {#entities}
========

The core of the world are the various entities that makes up it. The behaviour of the entities are primarily driven by the Properties that are attached to them. There's also the ability to attach Python scripts to them, but we encourage a data driven approach whenever possible, to make the system less complex.

## Types

Each entity has a parent Type, with predefined properties. The Types define the base entity classes, and allows for efficient creation of multiple similar entities, with the possibility of individual customization per entity.

If you've used an Object-Oriented system, you can think of the Types as "classes". One class can inherit from another class, and thus gains the properties of all of it parents.

Internally each Type is handled by the TypeNode class.

## Properties

The behaviour of entities is mainly driven by various properties. Properties are named, with the only allowed characters being [a-z0-9_]. There are three visibility levels for properties. These are determined by the names of the properties.

### Private properties

Properties that starts with "__" (for example "__curse") are "private". These can only be seen by the simulation and admin clients. The entity to which they belong can't see them itself.
These properties are meant to be used for internal and secret things, which would ruin the game if they were known by players.

### Protected properties

Properties that starts with "_" (for example "_thoughts") are "protected". These can only be seen by the simulation, admin clients and the entity to which they belong. Typically these would be properties that should be hidden to other players.

## Defining properties in types

The properties available for a type is defined in an element named "attributes". The keys define the name of the property while the value is a map. This map in turn allows for a simple composition of properties, which is useful when having types in a inheritance tree.

These various type of "composition" are allowed:

* **default** The value will be used as it is, overwriting any other value in a parent type.

* **append** The value will be appended to any existing value in a parent type. The result is slightly different depending on the property type.
 
    For numerical values it will result in an addition.
    For strings it will result in the value being added to the end of the existing string.
    For maps it will result in the values being inserted.
    For lists it will result in the values being added to the back of the list.  

* **prepend** The value will be prepended to any existing value in a parent type. The result is slightly different depending on the property type.
 
    For numerical values it will result in an addition.
    For strings it will result in the value being added to the start of the existing string.
    For maps it will result in the values being inserted.
    For lists it will result in the values being added to the start of the list.
     
* **subtract** The value will be subtracted from any existing value in a parent type. The result is slightly different depending on the property type.
    
    For numerical values it will result in a subtraction.
    For strings nothing will happen, since it's not obvious how one subtracts one string from another.
    For maps it will result in the keys being removed (any values are ignored).
    For lists it will result in the values being removed to the start of the list.
    
An example would be a parent type with a __scripts property, which is of list type.

```xml
<map>
    <map name="attributes">
        <map name="__scripts">
            <list name="default">
                <map>
                    <string name="language">python</string>
                    <string name="name">world.objects.A</string>
                </map>
            </list>
        </map>
    </map>
    <string name="id">parent_type</string>
    <string name="objtype">class</string>
</map>
```

A child entity can define a __scripts property which appends to the existing property, as such:

```xml
<map>
    <map name="attributes">
        <map name="__scripts">
            <list name="append">
                <map>
                    <string name="language">python</string>
                    <string name="name">world.objects.B</string>
                </map>
            </list>
        </map>
    </map>
    <string name="id">child_type</string>
    <string name="objtype">class</string>
    <string name="parent">parent_type</string>
</map>
```

And instance of ```child_type``` would then get a __scripts property as such:

```xml
<list name="__scripts">
    <map>
        <string name="language">python</string>
        <string name="name">world.objects.A</string>
    </map>
    <map>
        <string name="language">python</string>
        <string name="name">world.objects.B</string>
    </map>
</list>
```


## Useful properties

While the number of properties are large, there are a couple of base ones that are essential to have knowledge of.

### "geometry"
The "geometry" property determines the geometry used for interaction with the physical world. It's handled by the GeometryProperty class.

For most entities you already have a mesh file available, and you want to refer to that file in the "path" attribute. This will update the "bbox" property to fit that of the mesh.

Often you also want the collision shape to use the mesh data, which is done by setting the "type" attribute to "mesh". 

However, you don't necessarily always want the collision shape to be a mesh. In some cases you instead want to use a more simple shape. Check the GeometryProperty class for examples of other collision shapes. 

### "present"

The "present" property is used by the client to determine how to visualize the entity. There are three levels of control available: "mesh", "modeldef" and "entitymap"

* **mesh**
When you only want to show mesh straight up you can just refer to the .mesh file directly.

* **modeldef**
A "model definition" allows much more control than a single mesh. You can refer to multiple meshes, turn on submeshes and replace materials. You can also define lights and particle effects. Do this by referring to a ".modeldef" file.

* **entitymap**
The most complex and powerful presentation is the "entity mapping". This is a rules engine which allows you to set up a series of rules with corresponding actions. These actions could be the showing of meshes or models, or more abstract things such as determining whether labels are shown or not. Use this when you need more control over complex behaviours, but keep in mind that the client presentation should play closely to the geometry as it's represented on the server.

### "speed_ground"

In m/s the max speed of the entity over ground. If absent the entity can't move over ground.

### "speed_water"

In m/s the max speed of the entity in water. If absent the entity can't move in water.

### "speed_flight"

In m/s the max speed of the entity in in flight. If absent the entity can't fly.

### "speed_jump"

In m/s the max speed of the entity when jumping. If absent the entity can't jump.
