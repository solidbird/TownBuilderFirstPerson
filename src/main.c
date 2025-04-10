#define RCAMERA_IMPLEMENTATION
#define RL_CULL_DISTANCE_FAR 1000
#define RL_CULL_DISTANCE_NEAR 0.01 

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

#include <raylib.h>
#include <raymath.h>

#include "rcamera.h"
#include "block_utils.h"

#define GLSL_VERSION 330

#define WORLD_WIDTH 500
#define WORLD_HEIGHT 500
#define WORLD_DEPTH 500 
#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 900


Color element_color[] = {GRAY, GREEN, YELLOW, PURPLE};

Vector3 get_camera_look_direction(Camera3D *cam3d){
	Vector3 camera_direction = Vector3Subtract(cam3d->target, cam3d->position);
	camera_direction = Vector3Normalize(camera_direction);

	return camera_direction;
}

void surround_sand(Block_Element *head_block, Block_Element *be){
	if(be->position.y != CUBE_SIZE/2) return;
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

void draw_on_plain(RayCollision rc, Block_Element *head_block, int region_index_x, int region_index_y){
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

void draw_on_blocks(Block_Element *head_block, Mesh *cube_mesh, Material *cube_material, Ray *camera_ray, RayCollision *rc_floor){
	//--------------------------------------
	Block_Element *be = head_block->next;
	int draw_hit_block = 0;
	// https://www.raylib.com/examples/shaders/loader.html?name=shaders_fog
	// Later to make some fog so we dont have to show as much

	while(be != NULL){
		Matrix cube_transform = MatrixTranslate(be->position.x, be->position.y, be->position.z);
		cube_material->maps[MATERIAL_MAP_DIFFUSE].color = element_color[be->element_type];
		DrawMesh(*cube_mesh, *cube_material, cube_transform);
		DrawCubeWiresV(be->position, (Vector3){CUBE_SIZE, CUBE_SIZE, CUBE_SIZE}, BLACK);

		RayCollision rc_block = GetRayCollisionMesh(*camera_ray, *cube_mesh, cube_transform);
		if(rc_block.hit && !draw_hit_block){
			rc_floor->hit = 0;
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

		RayCollision rc_floor = GetRayCollisionMesh(camera_ray, cube_mesh, floor_transform);

		draw_on_blocks(head_block, &cube_mesh, &cube_material, &camera_ray, &rc_floor);

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
