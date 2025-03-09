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

/*int translate_vector3_to_array_coordinates(Vector3 src){
	return ((int)(src.x / CUBE_SIZE) + WORLD_WIDTH/2) +
			(int)(((src.y / CUBE_SIZE) + WORLD_HEIGHT/2) * WORLD_WIDTH) +
			(int)(((src.z / CUBE_SIZE) + WORLD_DEPTH/2) * WORLD_WIDTH * WORLD_HEIGHT);
}*/

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

void InitBlockElements(Block_Element *be[100]){
	for(int i = 0; i < 100; i++){
		be[i] = malloc(sizeof(Block_Element));
		be[i]->position = (Vector3){0};
		be[i]->element_type = stone;
		be[i]->next = NULL;
	}
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
	int index_x = pos.x / 100;
	int index_y = pos.z / 100;

	Block_Element *tmp_be = be->next;
	be->next = malloc(sizeof(Block_Element));
	be->next->position = pos;
	be->next->element_type = element_type;
	be->next->next = tmp_be;
	TraceLog(LOG_INFO, "ADDED BLOCK (%d) @ {%.2f, %.2f, %.2f}",(index_x+5) + (index_y+5) * 10 ,be->next->position.x, be->next->position.y, be->next->position.z);
}

int DeleteBlock(Block_Element *be, Vector3 pos){
	int index_x = pos.x / 100;
	int index_y = pos.y / 100;

	Block_Element *tmp_be = be[index_x + index_y * 100].next;
	Block_Element *prev_be = NULL;
	while(tmp_be->next != NULL){
		prev_be = tmp_be;
		tmp_be = tmp_be->next;
		if( tmp_be->position.x == pos.x &&
			tmp_be->position.y == pos.y &&
			tmp_be->position.z == pos.z ){
				prev_be->next = tmp_be->next;
				free(tmp_be);

				return 1;
		}
	}

	return 0;
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

int main(int argc, char **argv){
	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "WaveFunctionColapse Test");
	Block_Element *head_block[100];
	InitBlockElements(head_block);

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
		//TraceLog(LOG_INFO, "REGION: {%.2f, %.2f, %.2f}", cam3d.position.x, cam3d.position.y, cam3d.position.z);

		for(int y = region_index_y - 1; y < region_index_y + 1; y++){
			for(int x = region_index_x - 1; x < region_index_x + 1; x++){
				Block_Element *be = head_block[(x+5) + (y+5) * 10]->next;
				while(be != NULL){
					DrawCubeV(be->position, (Vector3){CUBE_SIZE, CUBE_SIZE, CUBE_SIZE}, element_color[be->element_type]);
					DrawCubeWiresV(be->position, (Vector3){CUBE_SIZE, CUBE_SIZE, CUBE_SIZE}, BLACK);
					be = be->next;
				}
			}
		}
	
		RayCollision rc = GetRayCollisionQuad(camera_ray, 
			(Vector3){WORLD_WIDTH, 0, WORLD_HEIGHT},
			(Vector3){WORLD_WIDTH, 0, -WORLD_HEIGHT},
			(Vector3){-WORLD_WIDTH, 0, -WORLD_HEIGHT},
			(Vector3){-WORLD_WIDTH, 0, WORLD_HEIGHT});
		//TraceLog(LOG_INFO, "%d, (%f, %f, %f)",block_pos_index, rc.point.x, rc.point.y, rc.point.z);
		if(rc.hit){
			Vector3 snap_grid = {
				(int)(rc.point.x/CUBE_SIZE) * CUBE_SIZE,
				0,
				(int)(rc.point.z/CUBE_SIZE) * CUBE_SIZE,
			};

			if(IsKeyPressed(KEY_I) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
				/*Block_Element *be = GetBlock(head_block[(region_index_x + 5) + (region_index_y + 5) * 10], snap_grid);
				if(be != NULL && be->element_type == shore){
					AddBlock(head_block[(region_index_x + 5) + (region_index_y + 5) * 10], snap_grid, stone);
				}else if(be == NULL){
					AddBlock(head_block[(region_index_x + 5) + (region_index_y + 5) * 10], snap_grid, stone);
				}*/
				//PrintAllBlocks(head_block);
				int index = (region_index_x + 5) + (region_index_y + 5) * 10;
				int snap_index_x = (snap_grid.x / 100);
				int snap_index_y = (snap_grid.z / 100);
				Block_Element *be = GetBlock(head_block[index], snap_grid);
				if( snap_index_x > region_index_x - 2 &&
					snap_index_x < region_index_x + 2 &&
					snap_index_y > region_index_y - 2 &&
					snap_index_y < region_index_y + 2 ){
					if(be == NULL){
						AddBlock(head_block[(((int)(snap_grid.x/100))+5) + (((int)(snap_grid.z/100))+5) * 10], snap_grid, stone);
					}else{
						//WAVE FUNCTION COLLAPSE THAT BLOCK ACCORDING TO RULES
						PrintAllBlocks(head_block);
						TraceLog(LOG_INFO, "BLOCK: %d @ {%.2f, %.2f}", index, be->position.x, be->position.y);
					}
				}
			}
			DrawCubeV(snap_grid, (Vector3){CUBE_SIZE, CUBE_SIZE, CUBE_SIZE}, RED);
		}

		DrawPlane((Vector3){0,0,0}, (Vector2){WORLD_WIDTH, WORLD_HEIGHT}, BLUE);
		DrawGrid(WORLD_WIDTH, CUBE_SIZE * 4);
		EndMode3D();

		DrawFPS(10, 10);

		EndDrawing();
		Vector2 mouse_pos = GetMousePosition();
		if(mouse_pos.x < 100 || mouse_pos.y < 100 || mouse_pos.x > WINDOW_WIDTH - 100 || mouse_pos.y > WINDOW_HEIGHT - 100){
			SetMousePosition(WINDOW_WIDTH/2, WINDOW_HEIGHT/2);
		}
	}
	return 0;
}
