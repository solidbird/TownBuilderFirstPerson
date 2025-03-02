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
#define WORLD_WIDTH 1000
#define WORLD_HEIGHT 1000
#define WORLD_DEPTH 1000
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
	Block_Element *next;
} Block_Element;

void InitBlockElements(Block_Element *be[100]){
	for(int i = 0; i < 100; i++){
		be[i] = malloc(sizeof(Block_Element));
		be[i]->next = NULL;
	}
}

void AddBlock(Block_Element *be[100], Vector3 pos, ELEMENT element_type){
	int index_x = pos.x / 100;
	int index_y = pos.y / 100;

	Block_Element *tmp_be = be[index_x + index_y * 100]->next;
	be[index_x + index_y * 100]->next = malloc(sizeof(Block_Element));
	be[index_x + index_y * 100]->position = pos;
	be[index_x + index_y * 100]->element_type = element_type;
	be[index_x + index_y * 100]->next = tmp_be;
}

int DeleteBlock(Block_Element *be[100], Vector3 pos){
	int index_x = pos.x / 100;
	int index_y = pos.y / 100;

	Block_Element *tmp_be = be[index_x + index_y * 100]->next;
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

int main(int argc, char **argv){
	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "WaveFunctionColapse Test");
	Block_Element *head_block;

	Camera3D cam3d = {0};
	cam3d.position = (Vector3){CUBE_SIZE * 2,CUBE_SIZE * 2, CUBE_SIZE * 2};
	cam3d.target = (Vector3){2, 2, 0};
	cam3d.up = (Vector3){0, 1, 0};
	cam3d.fovy = 60.0;
	cam3d.projection = CAMERA_PERSPECTIVE;

	Ray camera_ray = {
		cam3d.position,
		get_camera_look_direction(&cam3d)
	};

	if(block == NULL){
		printf("NOPE");
		return -1;
	}
	HideCursor();

	SetTargetFPS(60);

	while(!WindowShouldClose()){
		UpdateCamera(&cam3d, CAMERA_FREE);

		BeginDrawing();
		ClearBackground(BLACK);
		BeginMode3D(cam3d);

		camera_ray.position = cam3d.position;
		camera_ray.direction = get_camera_look_direction(&cam3d);

		//We only need to draw the cubes that are close to you
		//TODO: This doesnt really work how I wanted it. I just put the barriers -250/+250 up.
		/*int index_low = translate_vector3_to_array_coordinates((Vector3){cam3d.position.x - 50, cam3d.position.y - 50, cam3d.position.z - 50});
		int index_high = translate_vector3_to_array_coordinates((Vector3){cam3d.position.x + 250, cam3d.position.y + 250, cam3d.position.z + 250});
		
		for(int index = index_low; index < index_high; index++){
			if(block[index].set){
				DrawCubeV(block[index].position, (Vector3){CUBE_SIZE, CUBE_SIZE, CUBE_SIZE}, element_color[block[index].element_type]);
				DrawCubeWiresV(block[index].position, (Vector3){CUBE_SIZE, CUBE_SIZE, CUBE_SIZE}, BLACK);
			}
		}*/
	
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

			//int block_pos_index = translate_vector3_to_array_coordinates((Vector3){snap_grid.x, snap_grid.y, snap_grid.z});
			if(IsKeyPressed(KEY_I) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){

				TraceLog(LOG_INFO, "KEY_I: %d", block_pos_index);
				if(!block[block_pos_index].set || block[block_pos_index].element_type == shore){
					block[block_pos_index].position = snap_grid;
					block[block_pos_index].element_type = stone;
					block[block_pos_index].set = 1;
				}else if(block[block_pos_index].element_type == stone){
					snap_grid.y = block[block_pos_index].position.y + CUBE_SIZE;
					block[block_pos_index].position = snap_grid;
					block[block_pos_index].element_type = building;
					block[block_pos_index].set = 1;
				}

				if(block[block_pos_index].element_type == stone){
					for(int y = -1; y <= 1; y++){
						for(int x = -1; x <= 1; x++){
							if(y == 0 && x == 0) continue;
							int index = block_pos_index + WORLD_WIDTH * y + x;
							if(block[index].set && (block[index].element_type == stone || block[index].element_type == building)) continue;

							block[index].position = (Vector3){
								((int)((rc.point.x / CUBE_SIZE)) + x) * CUBE_SIZE,
								0,
								((int)((rc.point.z / CUBE_SIZE)) + y) * CUBE_SIZE,
							};
							block[index].element_type = shore;
							block[index].set = 1;
							TraceLog(LOG_INFO, "index: %d, vector: (%.2f, %.2f)", index, block[index].position.x, block[index].position.z);
						}
					}
				}
			}
			if(IsKeyDown(KEY_O) || IsMouseButtonDown(MOUSE_BUTTON_RIGHT)){
				block[block_pos_index] = (Block_Element){0};
			}
			DrawCubeV(snap_grid, (Vector3){CUBE_SIZE, CUBE_SIZE, CUBE_SIZE}, RED);
			//DrawCubeWiresV(snap_grid, (Vector3){CUBE_SIZE, CUBE_SIZE, CUBE_SIZE}, PURPLE);
		}

		DrawPlane((Vector3){0,0,0}, (Vector2){WORLD_WIDTH, WORLD_HEIGHT}, BLUE);
		DrawGrid(WORLD_WIDTH, CUBE_SIZE * 4);
		EndMode3D();

		DrawFPS(10, 10);

		EndDrawing();
		Vector2 mouse_pos = GetMousePosition();
		TraceLog(LOG_INFO, "MOUSE: (%f, %f)", mouse_pos.x, mouse_pos.y);
		if(mouse_pos.x < 100 || mouse_pos.y < 100 || mouse_pos.x > WINDOW_WIDTH - 100 || mouse_pos.y > WINDOW_HEIGHT - 100){
			SetMousePosition(WINDOW_WIDTH/2, WINDOW_HEIGHT/2);
		}
	}
	return 0;
}
