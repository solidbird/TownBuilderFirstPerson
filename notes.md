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

### Memory usage for blocks in world
I noticed that if I just reserve 1000 * 1000 * 1000 * 20 Bytes memory for potential
blocks in the world that could be set we really quickly get into memory size issues.

My idea is to index multiple regions and set the blocks that are set in these regions
into a dynamic linked list. For example if we have the current world:
```
------------------------- 
|		|		|		|
|	0	|	1	|	2	|
|		|		|		|
------------------------- 
|		|		|		|
|	3	|	4	|	5	|
|		|		|		|
------------------------- 
|		|		|		|
|	6	|	7	|	8	|
|		|		|		|
-------------------------
```

Now if we get into certain regains which in most case would get into
4 regions at the same time we would render only those regions buy
going through the linked list each. Maybe we can take advantage of caching
by trying to lay them close by each other but other than that there not much
of a draw back compare to directly going through a preset array of blocks.

The only issue might be that setting the block might take some time.
To metigate this we can just set the new set block at the beginning of the
list so setting doesn't take too much time.

Also we need helper functions for the dynamic list so we can detect if we ran
out of space that we allocated and translate the indexes of the map into proper
blocks. Here a small example:

```
BlockHeader *bh[9];

for(size_t i = 0; i < 9; i++){
	BlockHeader *bh[i] = malloc(sizeof(BlockHeader));
	*bh[i] = {0};
}
...

bh[0] = Block{10,11,23} -> Block{10,11,23};
bh[1] = Block{340,454,12};
bh[2] = NULL;
bh[3] = NULL;
bh[4] = Block{556,435,545} -> Bock{222,33,345} -> Block{...} -> ...
bh[5] = ...
bh[6] = ...
bh[7] = ...
bh[8] = ...
```

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
