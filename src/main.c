#include <stdio.h>
#define RCAMERA_IMPLEMENTATION
#define RL_CULL_DISTANCE_FAR 1000
#define RL_CULL_DISTANCE_NEAR 0.01 
#define RCAMERA_IMPLEMENTATION

#include <raylib.h>
#include "rcamera.h"
#include <raymath.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

#define GLSL_VERSION 330

#define CUBE_SIZE 0.5
#define WORLD_WIDTH 500
#define WORLD_HEIGHT 500
#define WORLD_DEPTH 500 
#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 900

Vector3 get_camera_look_direction(Camera3D *cam3d){
	Vector3 camera_direction = Vector3Subtract(cam3d->target, cam3d->position);
	camera_direction = Vector3Normalize(camera_direction);

	return camera_direction;
}

typedef enum ELEMENT {
	stone,
	grass,
	shore,
	building
} ELEMENT;

Color element_color[] = {GRAY, GREEN, YELLOW, PURPLE};

typedef struct Block_Element {
	Vector3 position;
	ELEMENT element_type;
	struct Block_Element *next;
} Block_Element;

void InitBlockElements(Block_Element **be){
	*be = malloc(sizeof(Block_Element));
	(*be)->position = (Vector3){0};
	(*be)->element_type = stone;
	(*be)->next = NULL;
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

void PrintAllBlocks(Block_Element *be[100]){
	for(int i = 0; i < 100; i++){
		TraceLog(LOG_INFO, "HEAD BLOCK %d", i);
		PrintBlocks(be[i]);
	}
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

void surround_sand(Block_Element *head_block, Block_Element *be){
	if(be->position.y != CUBE_SIZE/2) return;
	Vector3 tmp_pos = be->position;
	for(int y = -1; y <= 1; y++){
		for(int x = -1; x <= 1; x++){
			if(y == 0 && x == 0) continue;
			Vector3 pos = {
				be->position.x + x * CUBE_SIZE,
				CUBE_SIZE/2,
				be->position.z + y * CUBE_SIZE
			};
			Block_Element *b = GetBlock(head_block, pos);
			if(b == NULL) AddBlock(head_block, pos, shore);
		}
	}
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

/*
TODO:
0. go in one of three directions (<-^->)
1. go through left to right (<-, ^, ->)
2. if find neighbor at direction
3. count for that direction
4. keep going for that direction
5. if no block any more for that go left or right from that direction (<-, ->)
6. follow 3.
7. if you end up at position of the start position than you done
8. calculate from the start position + direction count the closing area

Block_Element* GetAreaOfWithinLoop(Block_Element *be, Vector3 start_pos){
	int direction_count[4] = {0};
	Vector3 prev_pos = start_pos;
	Vector3 next_pos = start_pos;
	int direction = 0;

	do{
		prev_pos = next_pos;
		switch(direction){
			case 0:
				next_pos.x += CUBE_SIZE;
			break;
			case 1:
				next_pos.z += CUBE_SIZE;
			break;
			case 2:
				next_pos.x -= CUBE_SIZE;
			break;
			case 3:
				next_pos.z -= CUBE_SIZE;
			break;
		}
		Block_Element *next_block = GetBlock(be, next_pos);
		if(next_block != NULL){
			direction_count[direction]++;
		}else{
			next_pos = prev_pos;
			if(direction % 2){
			}else{
			}
		}
	}while(next_pos == start_pos);
		
}*/

int go_dir(Block_Element *be_head, Vector3 start, Vector3 pos, int dir_count[4], int dirs[3], Block_Element *gather_circle){
	//__asm__("int3");
	Vector3 next_pos = pos;
	for(size_t i = 0; i < 3; i++){
		switch(dirs[i]){
			case 0:
				next_pos.x += CUBE_SIZE;
			break;
			case 1:
				next_pos.z += CUBE_SIZE;
			break;
			case 2:
				next_pos.x -= CUBE_SIZE;
			break;
			case 3:
				next_pos.z -= CUBE_SIZE;
			break;
			default:
				continue;
		}
		Block_Element *next_block = GetBlock(be_head, next_pos);
		if(next_block == NULL || next_block->element_type != building){
			dirs[i] = -1;
			continue;
		}
		dir_count[dirs[i]]++;

		//TODO: If we have a direction than stay with that direction
		int new_dirs[3] = {(((dirs[i]-1)+4)%4), dirs[i], (((dirs[i]+1)+4)%4)};
		//ROTATE_RIGHT	->	{-1, dirs[i], (((dirs[i]+1)+4)%4)}
		//ROTATE_LEFT	->	{(((dirs[i]-1)+4)%4), dirs[i], -1}

		dirs[i] = -1;
		
		int sum = 0;
		for(size_t j = 0; j < 4; j++){
			sum += dir_count[j];
		}

		if(sum > 14){
			return -2;
		}

		Block_Element *tmp_gather_circle = gather_circle->next;
		gather_circle->next = malloc(sizeof(Block_Element));
		gather_circle->next->position = next_block->position;
		gather_circle->next->element_type = next_block->element_type;
		gather_circle->next->next = tmp_gather_circle;

		if(next_pos.x == start.x && next_pos.y == start.y && next_pos.z == start.z){
			return 1;
		}

		int ret_val = go_dir(be_head, start, next_pos, dir_count, new_dirs, gather_circle);
		if(ret_val > 0){
			return ret_val;
		}
	}
	if(next_pos.x == start.x && next_pos.y == start.y && next_pos.z == start.z){
		return 1;
	}else{
		return -1;
	}
}

static inline void draw_on_plain(RayCollision rc, Block_Element *head_block, int region_index_x, int region_index_y){
	if(rc.hit){
		Vector3 snap_grid = {
			(int)(rc.point.x/CUBE_SIZE) * CUBE_SIZE,
			CUBE_SIZE/2,
			(int)(rc.point.z/CUBE_SIZE) * CUBE_SIZE,
		};

		if(IsKeyPressed(KEY_I) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
			int snap_index_x = (snap_grid.x / 100);
			int snap_index_y = (snap_grid.z / 100);
			Block_Element *be = GetBlock(head_block, snap_grid);
			if( snap_index_x > region_index_x - 2 &&
				snap_index_x < region_index_x + 2 &&
				snap_index_y > region_index_y - 2 &&
				snap_index_y < region_index_y + 2 ){
				if(be == NULL){
					AddBlock(head_block, snap_grid, stone);
					surround_sand(head_block, head_block->next);
				}
			}
		}
		DrawCubeV(snap_grid, (Vector3){CUBE_SIZE, CUBE_SIZE, CUBE_SIZE}, RED);
	}
}

int main(int argc, char **argv){
	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "WaveFunctionColapse Test");
	Block_Element *head_block;
	InitBlockElements(&head_block);

	Camera3D cam3d = {0};
	cam3d.position = (Vector3){CUBE_SIZE * 2, CUBE_SIZE * 2, CUBE_SIZE * 2};
	cam3d.target = (Vector3){2, 2, 0};
	cam3d.up = (Vector3){0, 1, 0};
	cam3d.fovy = 60.0;
	cam3d.projection = CAMERA_PERSPECTIVE;

	Ray camera_ray = {
		cam3d.position,
		get_camera_look_direction(&cam3d)
	};
	Matrix floor_transform = (Matrix) { WORLD_WIDTH/CUBE_SIZE, 0, 0, 0,
										0, 1/CUBE_SIZE, 0, -CUBE_SIZE,
										0, 0, WORLD_HEIGHT/CUBE_SIZE, 0,
										0, 0, 0, 1 };
	//Shader fog_shader = LoadShader(0, TextFormat("/home/tapir/Projects/libs/raylib/examples/shaders/resources/shaders/glsl%i/predator.fs", GLSL_VERSION));
	Shader fog_shader = LoadShader(0, TextFormat("test.fs", GLSL_VERSION));
	Mesh cube_mesh = (GenMeshCube(CUBE_SIZE, CUBE_SIZE, CUBE_SIZE));
    Material cube_material = LoadMaterialDefault();
    cube_material.maps[MATERIAL_MAP_DIFFUSE].color = GRAY;

    Material floor_material = LoadMaterialDefault();
    floor_material.maps[MATERIAL_MAP_DIFFUSE].color = BLUE;
	
	HideCursor();

	SetTargetFPS(60);

	while(!WindowShouldClose()){
		UpdateCamera(&cam3d, CAMERA_FREE);

		BeginDrawing();
		ClearBackground(BLACK);
		BeginMode3D(cam3d);

		camera_ray.position = cam3d.position;
		camera_ray.direction = get_camera_look_direction(&cam3d);

		int region_index_x = (int)(cam3d.position.x / 100);
		int region_index_y = (int)(cam3d.position.z / 100);

		RayCollision rc_block = {0};
		RayCollision rc_floor = GetRayCollisionMesh(camera_ray, cube_mesh, floor_transform);
		int draw_hit_block = 0;

		//--------------------------------------
		Block_Element *be = head_block->next;
		// https://www.raylib.com/examples/shaders/loader.html?name=shaders_fog
		// Later to make some fog so we dont have to show as much

		while(be != NULL){
			//DrawSphere(be->position, CUBE_SIZE/2, PURPLE);
			//DrawCubeV(be->position, (Vector3){CUBE_SIZE, CUBE_SIZE, CUBE_SIZE}, element_color[be->element_type]);
			Matrix cube_transform = MatrixTranslate(be->position.x, be->position.y, be->position.z);
			cube_material.maps[MATERIAL_MAP_DIFFUSE].color = element_color[be->element_type];
			DrawMesh(cube_mesh, cube_material, cube_transform);
			DrawCubeWiresV(be->position, (Vector3){CUBE_SIZE, CUBE_SIZE, CUBE_SIZE}, BLACK);

			rc_block = GetRayCollisionMesh(camera_ray, cube_mesh, cube_transform);
			if(rc_block.hit && !draw_hit_block){
				rc_floor.hit = 0;
				draw_hit_block = 1;
				Vector3 new_pos = (Vector3){be->position.x + (rc_block.normal.x * CUBE_SIZE),
											be->position.y + (rc_block.normal.y * CUBE_SIZE),
											be->position.z + (rc_block.normal.z * CUBE_SIZE)};
				DrawCubeWiresV(new_pos, (Vector3){CUBE_SIZE, CUBE_SIZE, CUBE_SIZE}, BLACK);
				if(IsKeyPressed(KEY_O) || IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)){
					DeleteBlock(head_block, be->position);
				}
				if(IsKeyPressed(KEY_I) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
					Block_Element *be_tmp = GetBlock(head_block, new_pos);
					if(be_tmp == NULL){
						switch(be->element_type){
							case shore:
								DeleteBlock(head_block, be->position);
								new_pos.y = CUBE_SIZE/2;
								AddBlock(head_block, new_pos, stone);
							break;
							case stone:
							case building:
								AddBlock(head_block, new_pos, building);

							break;
							default:
								AddBlock(head_block, new_pos, stone);
								TraceLog(LOG_INFO, "RAY BLOCK: {%.2f, %.2f, %.2f}", rc_block.point.x, rc_block.point.y, rc_block.point.z);
								TraceLog(LOG_INFO, "RAY NORMAL: {%.2f, %.2f, %.2f}", rc_block.normal.x, rc_block.normal.y, rc_block.normal.z);
							break;
						}
						surround_sand(head_block, head_block->next);
					}
				}
			}
			be = be->next;
		}
		draw_hit_block = 0;
		//---------------------------------

		draw_on_plain(rc_floor, head_block, region_index_x, region_index_y);
		DrawMesh(cube_mesh, floor_material, floor_transform);
		DrawGrid(WORLD_WIDTH, CUBE_SIZE * 4);

		EndMode3D();

		DrawLine(WINDOW_WIDTH/2 - 5, WINDOW_HEIGHT/2, WINDOW_WIDTH/2 + 5, WINDOW_HEIGHT/2, WHITE);
		DrawLine(WINDOW_WIDTH/2, WINDOW_HEIGHT/2 - 5, WINDOW_WIDTH/2, WINDOW_HEIGHT/2 + 5, WHITE);
		DrawFPS(10, 10);

		EndDrawing();
		Vector2 mouse_pos = GetMousePosition();
		if(mouse_pos.x < 100 || mouse_pos.y < 100 || mouse_pos.x > WINDOW_WIDTH - 100 || mouse_pos.y > WINDOW_HEIGHT - 100){
			SetMousePosition(WINDOW_WIDTH/2, WINDOW_HEIGHT/2);
		}
	}
	return 0;
}
