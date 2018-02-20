Entities      {#entities}
========

The core of the world are the various entities that makes up it. The behaviour of the entities are primarily driven by the Properties that are attached to them. There's also the ability to attach Python scripts to them, but we encourage a data driven approach whenever possible, to make the system less complex.

## Types

Each entity has a parent Type, with predefined properties. The Types define the base entity classes, and allows for efficient creation of multiple similar entities, with the possibility of individual customization per entity.

If you've used an Object-Oriented system, you can think of the Types as "classes". One class can inherit from another class, and thus gains the properties of all of it parents.

Internally each Type is handled by the TypeNode class.

## Useful properties

While the number of properties are large, there are a couple of base ones that are essential to have knowledge of.

### "geometry"
The "geometry" property determines the geometry used for interaction with the physical world. It's handled by the GeometryProperty class.

For most entities you already have a mesh file available, and you want to refer to that file in the "path" attribute. This will update the "bbox" property to fit that of the mesh.

Often you also want the collision shape to use the mesh data, which is done by setting the "type" attribute to "mesh". 

However, you don't necessarily always want the collision shape to be a mesh. In some cases you instead want to use a more simple shape. Check the GeometryProperty class for examples of other collision shapes. 

### "present"

The "present" property is used by the client to determine how to visualize the entity. There are three levels of control available: "mesh", "modeldef" and "entitymap"

* "mesh"
When you only want to show mesh straight up you can just refer to the .mesh file directly.

* "modeldef"
A "model definition" allows much more control than a single mesh. You can refer to multiple meshes, turn on submeshes and replace materials. You can also define lights and particle effects. Do this by referring to a ".modeldef" file.

* "entitymap"
The most complex and powerful presentation is the "entity mapping". This is a rules engine which allows you to set up a series of rules with corresponding actions. These actions could be the showing of meshes or models, or more abstract things such as determining whether labels are shown or not. Use this when you need more control over complex behaviours, but keep in mind that the client presentation should play closely to the geometry as it's represented on the server.

### "speed-ground"

In m/s the max speed of the entity over ground. If absent the entity can't move over ground.

### "speed-water"

In m/s the max speed of the entity in water. If absent the entity can't move in water.

### "speed-flight"

In m/s the max speed of the entity in in flight. If absent the entity can't fly.

### "speed-jump"

In m/s the max speed of the entity when jumping. If absent the entity can't jump.
