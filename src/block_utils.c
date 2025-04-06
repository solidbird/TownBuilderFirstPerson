#include "block_utils.h"

void InitBlockElements(Block_Element **be){
	*be = malloc(sizeof(Block_Element));
	(*be)->position = (Vector3){0};
	(*be)->element_type = stone;
	(*be)->next = NULL;
}

void AddBlock(Block_Element *be, Vector3 pos, ELEMENT element_type){
	Block_Element *tmp_be = be->next;
	be->next = malloc(sizeof(Block_Element));
	be->next->position = pos;
	be->next->element_type = element_type;
	be->next->next = tmp_be;

	switch(be->next->element_type){
		case building:
			if(be->next->position.y == (3*CUBE_SIZE/2)){
				int dirs_count[4] = {0,0,0,0};
				int dirs[3] = {0,1,2};
				Block_Element *gather_circle;
				InitBlockElements(&gather_circle);
				int a = go_dir(be, be->next->position, be->next->position, dirs_count, dirs, gather_circle);
				Block_Element *tmp = gather_circle->next;
				while(tmp != NULL){
					TraceLog(LOG_INFO, "CIRCLE POS: {%.2f, %.2f, %.2f}", tmp->position.x, tmp->position.y, tmp->position.z);
					tmp = tmp->next;
				}
				TraceLog(LOG_INFO, "GODIR %d", a);
			}
			//TODO if stone block is added on Y = CUBE_SIZE then try to run recursive function
		break;
	}
}

Block_Element* GetBlock(Block_Element *be, Vector3 pos){
	Block_Element *tmp_be = be->next;
	while(tmp_be != NULL){
		if( tmp_be->position.x == pos.x &&
			tmp_be->position.y == pos.y &&
			tmp_be->position.z == pos.z ){
				return tmp_be;
		}
		tmp_be = tmp_be->next;
	}

	return NULL;
}

int DeleteBlock(Block_Element *be, Vector3 pos){
	Block_Element *tmp_be = be->next;
	Block_Element *prev_be = be;
	while(tmp_be != NULL){
		if( tmp_be->position.x == pos.x &&
			tmp_be->position.y == pos.y &&
			tmp_be->position.z == pos.z ){
				prev_be->next = tmp_be->next;
				free(tmp_be);

				return 1;
		}
		prev_be = tmp_be;
		tmp_be = tmp_be->next;
	}

	return 0;
}

void PrintBlocks(Block_Element *be){
	Block_Element *tmp_be = be;
	TraceLog(LOG_INFO, "BLOCK INFO {");
	int i = 0;
	while(tmp_be != NULL){
		TraceLog(LOG_INFO, "\t%d: {%.2f, %.2f, %.2f}", i, tmp_be->position.x, tmp_be->position.y, tmp_be->position.z);
		tmp_be = tmp_be->next;
		i++;
	}
	TraceLog(LOG_INFO, "}");	
}
