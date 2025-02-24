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

Vector3 get_camera_look_direction(Camera3D *cam3d){
	Vector3 camera_direction = Vector3Subtract(cam3d->target, cam3d->position);
	camera_direction = Vector3Normalize(camera_direction);

	return camera_direction;
}

int translate_vector2_to_array_coordinates(Vector2 src){
	return ((int)(src.x / CUBE_SIZE) + WORLD_WIDTH/2) + (int)(((src.y / CUBE_SIZE) + WORLD_HEIGHT/2) * WORLD_WIDTH);
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
	int set;
} Block_Element;

int main(int argc, char **argv){
	InitWindow(1600, 900, "WaveFunctionColapse Test");
	DisableCursor();

	Camera3D cam3d = {0};
	cam3d.position = (Vector3){CUBE_SIZE * 2,CUBE_SIZE * 2, CUBE_SIZE * 2};
	cam3d.target = (Vector3){0, 0, 0};
	cam3d.up = (Vector3){0, 1, 0};
	cam3d.fovy = 60.0;
	cam3d.projection = CAMERA_PERSPECTIVE;

	Ray camera_ray = {
		cam3d.position,
		get_camera_look_direction(&cam3d)
	};

	Block_Element *block = (Block_Element*) malloc(sizeof(Block_Element) * WORLD_WIDTH * sizeof(Block_Element) * WORLD_HEIGHT);
	int block_pos_index = 0;

	SetTargetFPS(60);

	while(!WindowShouldClose()){
		BeginDrawing();
		ClearBackground(BLACK);
		BeginMode3D(cam3d);
		UpdateCamera(&cam3d, CAMERA_FREE);

		camera_ray.position = cam3d.position;
		camera_ray.direction = get_camera_look_direction(&cam3d);

		RayCollision rc = GetRayCollisionQuad(camera_ray, 
			(Vector3){WORLD_WIDTH, 0, WORLD_HEIGHT},
			(Vector3){WORLD_WIDTH, 0, -WORLD_HEIGHT},
			(Vector3){-WORLD_WIDTH, 0, -WORLD_HEIGHT},
			(Vector3){-WORLD_WIDTH, 0, WORLD_HEIGHT});
		//TraceLog(LOG_INFO, "%d, (%f, %f, %f)",block_pos_index, rc.point.x, rc.point.y, rc.point.z);
		TraceLog(LOG_INFO, "CAM POS: (%f, %f, %f)", cam3d.position.x, cam3d.position.y, cam3d.position.z);
		if(rc.hit){
			Vector3 snap_grid = {
				(int)(rc.point.x/CUBE_SIZE) * CUBE_SIZE,
				0,
				(int)(rc.point.z/CUBE_SIZE) * CUBE_SIZE,
			};

			if(IsKeyPressed(KEY_I)){
				block_pos_index = translate_vector2_to_array_coordinates((Vector2){snap_grid.x/CUBE_SIZE, snap_grid.z/CUBE_SIZE});
				TraceLog(LOG_INFO, "KEY_I: %d", block_pos_index);
				block[block_pos_index].position = snap_grid;
				block[block_pos_index].element_type = grass;
				block[block_pos_index].set = 1;
			}
			DrawCubeV(snap_grid, (Vector3){CUBE_SIZE, CUBE_SIZE, CUBE_SIZE}, GRAY);
		}

		//We only need to draw the cubes that are close to you
		//TODO: This doesnt really work how I wanted it. I just put the barriers -250/+250 up.
		int index_low = translate_vector2_to_array_coordinates((Vector2){cam3d.position.x - 250, cam3d.position.z - 250});
		int index_high = translate_vector2_to_array_coordinates((Vector2){cam3d.position.x + 250, cam3d.position.z + 250});
		for(int index = index_low; index < index_high; index++){
			if(block[index].set){
				DrawCubeV(block[index].position, (Vector3){CUBE_SIZE, CUBE_SIZE, CUBE_SIZE}, element_color[block[index].element_type]);
			}
		}

		DrawPlane((Vector3){0,0,0}, (Vector2){WORLD_WIDTH, WORLD_HEIGHT}, BLUE);
		DrawGrid(WORLD_WIDTH, CUBE_SIZE * 4);
		EndMode3D();

		DrawFPS(10, 10);

		EndDrawing();
	}
	return 0;
}
