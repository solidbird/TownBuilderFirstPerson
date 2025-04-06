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
		case shore: break;
		case stone: break;
		case grass: break;
		case building:
			if(be->next->position.y == (3*CUBE_SIZE/2)){
				int dirs_count[4] = {0,0,0,0};
				int dirs[3] = {0,1,2};
				Block_Element *gather_circle;
				InitBlockElements(&gather_circle);
				int ret_loop = LoopDetection(be, be->next->position, be->next->position, dirs_count, dirs, gather_circle);
				Block_Element *tmp = gather_circle->next;
				while(tmp != NULL){
					TraceLog(LOG_INFO, "CIRCLE POS: {%.2f, %.2f, %.2f}", tmp->position.x, tmp->position.y, tmp->position.z);
					tmp = tmp->next;
				}
				TraceLog(LOG_INFO, "Loop detected %d", ret_loop);
			}
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

int LoopDetection(Block_Element *be_head, Vector3 start, Vector3 pos, int dir_count[4], int dirs[3], Block_Element*gather_circle){
	//TODO: remember first turn to keep close corners on loop detection
	 Vector3 next_pos = pos;
	 Vector3 prev_pos = {0};
	 for(int i = 0; i < 3; i++){
		 prev_pos = next_pos;
		 switch(dirs[i]){
			 case 0: next_pos.x += CUBE_SIZE; break;
			 case 1: next_pos.z += CUBE_SIZE; break;
			 case 2: next_pos.x -= CUBE_SIZE; break;
			 case 3: next_pos.z -= CUBE_SIZE; break;
			 default: continue;
		 }
		 Block_Element *next_block = GetBlock(be_head, next_pos);
		 if(next_block == NULL || next_block->element_type != building){
			 next_pos = prev_pos;
			 dirs[i] = -1;
			 TraceLog(LOG_INFO, "PREV POS {%.2f, %.2f}", prev_pos.x, prev_pos.z);
			 continue;
		 }
		 dir_count[dirs[i]]++;

		 int new_dirs[3] = {(((dirs[i]-1)+4)%4), dirs[i], (((dirs[i]+1)+4)%4)};
		 dirs[i] = -1;

		 int sum = 0;
		 for(size_t j = 0; j < 4; j++){
			 sum += dir_count[j];
		 }

		 Block_Element *tmp_gather_circle = gather_circle->next;
		 gather_circle->next = malloc(sizeof(Block_Element));
		 gather_circle->next->position = next_block->position;
		 gather_circle->next->element_type = next_block->element_type;
		 gather_circle->next->next = tmp_gather_circle;

		 if(next_pos.x == start.x && next_pos.y == start.y && next_pos.z == start.z){
			return 1;
		 } 
		 if(sum > 15) return -2;		 

		 return LoopDetection(be_head, start, next_pos, dir_count, new_dirs, gather_circle);
	 }
	 return -1;
}
