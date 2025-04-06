#ifndef BLOCK_UTILS_H
#define BLOCK_UTILS_H

#include <raylib.h>
#include <stdlib.h>
#define CUBE_SIZE 0.5

typedef enum ELEMENT {
	stone,
	grass,
	shore,
	building
} ELEMENT;


typedef struct Block_Element {
	Vector3 position;
	ELEMENT element_type;
	struct Block_Element *next;
} Block_Element;

void InitBlockElements(Block_Element **be);
void AddBlock(Block_Element *be, Vector3 pos, ELEMENT element_type);
Block_Element* GetBlock(Block_Element *be, Vector3 pos);
int DeleteBlock(Block_Element *be, Vector3 pos);
void PrintBlocks(Block_Element *be);
int LoopDetection(Block_Element *be_head, Vector3 start, Vector3 pos, int dir_count[4], int dirs[3], Block_Element*gather_circle);

#endif
