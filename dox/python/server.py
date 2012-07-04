## \brief Interface to server core tasks.
##
## This is the interface class for all task scripts.
## It provides the interface to the underlying object in the server core.
class Task:
  def __init__(self, c):
     self.character = c

  ## \var character
  ## A reference to the owner of the task

## \brief Interface to server core objects.
##
## This is the interface class for all entity and mind scripts.
## It provides the interface to the underlying object in the server core.
class Thing:

  ## Thing constructor
  def __init__(self):
    self.type = "thing"
    self.location = physics.Location()

  ## Create an Atlas represenation of this entity
  ##
  ## \return An Atlas represention of this entity
  def as_entity(self): pass
    
  ## \var type
  ## A string giving the Atlas class of this entity

  ## \var location
  ## A structure giving the full location data of the entity
