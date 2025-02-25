# TownBuilderFirstPerson
This is my first try to build a little game that is similar to "Townscaper",
but with additional features priority from high to low:
* walk in the town you build in first person
* walk into buildings
* add furniture and other stuff into buildings
* make NPCs walking around and interacting with each other and stuff 
* weather effects

## Put blocks in world
To be able to set buildings, stone ground, etc. we need to be able to
have some kind of mechanism to put things down on the 3D plane on the
floor.

To achive this I am thinking about just shooting out a vector from the center of
the camera you are looking from (camera position) and just get the 2D point there it
hits the 2D plane. In that position of the plane hover a ghost object over it like a
cube so the user can see where the object can placed to.

### Snap block into grid
What we could do is just make multiple quads that are the grid and just check if any of these
quads has a ray collision with the ray of the camera and then just use that quads general
data fields.

## Wave Function Collapse Building blocks
We have following elements:
* (Water)
* Stone Ground
* Shore (Sand)
* Normal Building
* Garden (Grass)
* ...

### Rules
* Water -> Stone
* Stone -> Building
* Stone -> Corners -> Shore (Sand)
* Building -> Ring formation -> Grass (Garden) in the middle
* Building -> On top -> Tower

#### Ring of buildings -> Grass
A valid idea would be to introduce multithreading at this point right here to
let the algorithm run to detect circular buildings and plant the grass according to that.

##### Multithreading for all Wave Function Collapse rules
We could even bring this idea a little bit further and introduce multithreading for
all the rule calculations and algorithms. This might be better after all. (I am just scared of race conditions and deadlocks).
