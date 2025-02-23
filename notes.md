# TownBuilderFirstPerson
This is my first try to build a little game that is similar to "Townscaper",
but with additional features priority from high to low:
* walk in the town you build in first person
* walk into buildings
* add furniture and other stuff into buildings
* make NPCs walking around and interacting with each other and stuff 
* weather effects

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

