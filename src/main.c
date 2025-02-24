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

#define CUBE_SIZE 0.5

Vector3 get_camera_look_direction(Camera3D *cam3d){
	Vector3 camera_direction = Vector3Subtract(cam3d->target, cam3d->position);
	camera_direction = Vector3Normalize(camera_direction);

	return camera_direction;
}

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

	Vector3 *block_positions = (Vector3*) malloc(sizeof(Vector3) * 500);
	int block_pos_index = 0;

	SetTargetFPS(60);

	while(!WindowShouldClose()){
		BeginDrawing();
		ClearBackground(BLACK);
		BeginMode3D(cam3d);
		UpdateCamera(&cam3d, CAMERA_FREE);

		/*for(int i = 0; i < 10; i++){
			DrawCubeV((Vector3){0, CUBE_SIZE * i, 0}, (Vector3){CUBE_SIZE, CUBE_SIZE, CUBE_SIZE}, RED);
		}*/

		camera_ray.position = cam3d.position;
		camera_ray.direction = get_camera_look_direction(&cam3d);

		RayCollision rc = GetRayCollisionQuad(camera_ray, 
			(Vector3){10, 0, 10},
			(Vector3){10, 0, -10},
			(Vector3){-10, 0, -10},
			(Vector3){-10, 0, 10});
		TraceLog(LOG_INFO, "%d, (%f, %f, %f)",block_pos_index, rc.point.x, rc.point.y, rc.point.z);
		if(rc.hit){
			if(IsKeyPressed(KEY_I)) block_positions[block_pos_index++] = rc.point; 
			DrawCubeV(rc.point, (Vector3){CUBE_SIZE*0.5, CUBE_SIZE*0.5, CUBE_SIZE*0.5}, GRAY);
		}

		for(int i = 0; i < block_pos_index; i++){
			DrawCubeV(block_positions[i], (Vector3){CUBE_SIZE*0.5, CUBE_SIZE*0.5, CUBE_SIZE*0.5}, RED);
		}
		
		DrawPlane((Vector3){0,0,0}, (Vector2){1000, 1000}, BLUE);
		DrawGrid(1000, CUBE_SIZE * 4);
		EndMode3D();

		DrawFPS(10, 10);

		EndDrawing();
	}
	return 0;
}
