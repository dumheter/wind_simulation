# Scene Format
This document describes the scene format used for the prototype engine to 
structure scenes. JSON is the format chosen to store the scene data.

# Scene
The top level JSON object contains two tags, one for the name of the scene and 
another which is an array of all objects in the scene

## Tags
Possible tags
- 'name': Name of the scene
- 'objects': Array of top-level objects in the scene

# Objects
Objects represents things in the scenes. These are defined either in the 
top-level object array or in sub-object arrays. 

Each object has a type that is used to setup some predefined components on the 
objects. An object of type 'empty' does not have any predefined components.

## Tags
Possible tags

- "name": Name of the object.
- "type": Type of the object, can be left out which defaults it to an empty 
          object.
          - **cube** Object gets a cube model.
          - **sphere** Object gets a cube model.
          - **cylinder** Object gets a cube model.
- "position": Position of the object.
- "scale": Scale of the object.
- "rotation": Rotation of the object in degrees.
- "material": Describes the material of the object (more details in the material 
              section).
- "physics": Describes the physics properties of the object (more details in the 
             physics section).
- "wind": Describes the wind-related properties of the obejct (more details in 
          the wind section).

# Material


# Physics


# Wind
