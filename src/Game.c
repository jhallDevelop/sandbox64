#include "Game.h"
#include <libdragon.h>
#include "ECS/Entities/AF_ECS.h"
#include "AF_Input.h"
#include "AF_Debug.h"
#include "AF_Util.h"
#include "AF_Ray.h"
#include "AF_Physics.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/gl_integration.h>

// ECS system
AF_Entity* cube;
AF_Entity* plane;
AF_Entity* camera;

// define some const values
#define STICK_DEAD_ZONE 0.01
#define PLAYER_SPEED .25


// forward decalred functions
void HandleInput(AF_Input* _input, AF_ECS* _ecs);
float lightBlue[4] = {0,255, 255, 1};

void Game_Awake(AF_ECS* _ecs){
	if(_ecs == NULL){
		debugf("Game_Awake: passed null ecs reference \n");
		return;
	}

	debugf("Game Awake\n");
	// initialise the ecs system
	// Create Camera
	camera = AF_ECS_CreateEntity(_ecs);

	// Create Cube
	cube = AF_ECS_CreateEntity(_ecs);
	//move the position up a little
	Vec3 cubePosition = {cube->transform->pos.x, cube->transform->pos.y +5 , cube->transform->pos.z};
	cube->transform->pos = cubePosition;
	// add a rigidbody to our cube
	*cube->rigidbody = AF_C3DRigidbody_ADD();
	*cube->collider = AF_CCollider_Plane_ADD();
	Vec3 cubeBounds = {6, 6, 6};
	cube->collider->boundingVolume = cubeBounds;
	//cube->collider->showDebug = TRUE;
	_ecs->colliders[2].showDebug = TRUE;
	

	

	
	// Create Plane
	plane = AF_ECS_CreateEntity(_ecs);
	*plane->collider = AF_CCollider_Plane_ADD();
	Vec3 planeBounds = {20, 0, 20};
	plane->collider->boundingVolume = planeBounds;
	plane->collider->showDebug = TRUE;
	//plane->transform->pos = planePos;
	//plane->transform->pos = planePos;
}
void Game_Start(AF_ECS* _ecs){
	debugf("Game_Start\n");
	if(_ecs == NULL){
		debugf("Game_Start: passed null ecs reference \n");
		return;
	}

}
void Game_Update(AF_Input* _input, AF_ECS* _ecs)
{
	if(_ecs == NULL){
		debugf("Game_Update: passed null ecs reference \n");
		return;
	}
	// game update

	// update cube position based on button press
	// get input
	// if stick pressed left, right, up, down then adjust velocity
	// add velocity to cube component
	HandleInput(_input, _ecs);
}




// used to run in-between render start and render end. 
// Good for rendering debug data.
void Game_LateUpdate(AF_ECS* _ecs){
	float green[4] = {0,255, 0, 1};
	float red[4] = {255,0, 0, 1};
	// Convert the position of the cube in world pos into screen pos


	float distanceInfront = 10.0f;
	Vec3 startPoint = {cube->transform->pos.x,cube->transform->pos.y - 2.5f,cube->transform->pos.z};
	Vec3 justInFront = {startPoint.x, startPoint.y - distanceInfront, startPoint.z};
	Ray ray;
	ray.position = startPoint;
	Vec3 down = {0, -1, 0};
	ray.direction = down;
	AF_Collision collision;
	BOOL result = AF_Physics_RayIntersection(&ray, plane , &collision);
	if(result == TRUE){
		debugf("collision detected\n");
		AF_Debug_DrawLineWorld(&startPoint, &justInFront, green, TRUE);
	}else{
		AF_Debug_DrawLineWorld(&startPoint, &justInFront, red, TRUE);
	}

	for(int i = 0; i < _ecs->entitiesCount; ++i){
		AF_CCollider* collider = &_ecs->colliders[i];
        //if((AF_Component_GetHas(collider->enabled) == TRUE) && (AF_Component_GetEnabled(collider->enabled) == TRUE)){
            // render debug collider?
            if(collider->showDebug == TRUE){
                // render debug collider
                //draw all edges
                //if(collider->type == Plane){
                    Vec3 pos = collider->pos;//_ecs[i].transforms->pos;
                    Vec3 bounds = collider->boundingVolume;
					// Top
					/*
                    Vec3 top_bottomLeft = {pos.x - bounds.x/2, pos.x + bounds.x/2, pos.z - bounds.z/2};
                    Vec3 top_topLeft =  {pos.x - bounds.x/2, pos.x + bounds.x/2, pos.z + bounds.z/2};
                    Vec3 top_topRight =  {pos.x + bounds.x/2, pos.x + bounds.x/2, pos.z + bounds.z/2};
                    Vec3 top_bottomRight =  {pos.x + bounds.x/2, pos.x + bounds.x/2, pos.z - bounds.z/2};
					*/
					
                    const int verticesCount = 24;  // Total lines: 12 edges * 2 vertices per edge
					Vec3 vertices[verticesCount];

					// Top face vertices
					vertices[0] = (Vec3){pos.x - bounds.x/2, pos.y + bounds.y/2, pos.z - bounds.z/2};  // top-bottomLeft
					vertices[1] = (Vec3){pos.x - bounds.x/2, pos.y + bounds.y/2, pos.z + bounds.z/2};  // top-topLeft

					vertices[2] = (Vec3){pos.x - bounds.x/2, pos.y + bounds.y/2, pos.z + bounds.z/2};  // top-topLeft
					vertices[3] = (Vec3){pos.x + bounds.x/2, pos.y + bounds.y/2, pos.z + bounds.z/2};  // top-topRight

					vertices[4] = (Vec3){pos.x + bounds.x/2, pos.y + bounds.y/2, pos.z + bounds.z/2};  // top-topRight
					vertices[5] = (Vec3){pos.x + bounds.x/2, pos.y + bounds.y/2, pos.z - bounds.z/2};  // top-bottomRight

					vertices[6] = (Vec3){pos.x + bounds.x/2, pos.y + bounds.y/2, pos.z - bounds.z/2};  // top-bottomRight
					vertices[7] = (Vec3){pos.x - bounds.x/2, pos.y + bounds.y/2, pos.z - bounds.z/2};  // top-bottomLeft

					// Bottom face vertices
					vertices[8]  = (Vec3){pos.x - bounds.x/2, pos.y - bounds.y/2, pos.z - bounds.z/2};  // bottom-bottomLeft
					vertices[9]  = (Vec3){pos.x - bounds.x/2, pos.y - bounds.y/2, pos.z + bounds.z/2};  // bottom-topLeft

					vertices[10] = (Vec3){pos.x - bounds.x/2, pos.y - bounds.y/2, pos.z + bounds.z/2};  // bottom-topLeft
					vertices[11] = (Vec3){pos.x + bounds.x/2, pos.y - bounds.y/2, pos.z + bounds.z/2};  // bottom-topRight

					vertices[12] = (Vec3){pos.x + bounds.x/2, pos.y - bounds.y/2, pos.z + bounds.z/2};  // bottom-topRight
					vertices[13] = (Vec3){pos.x + bounds.x/2, pos.y - bounds.y/2, pos.z - bounds.z/2};  // bottom-bottomRight

					vertices[14] = (Vec3){pos.x + bounds.x/2, pos.y - bounds.y/2, pos.z - bounds.z/2};  // bottom-bottomRight
					vertices[15] = (Vec3){pos.x - bounds.x/2, pos.y - bounds.y/2, pos.z - bounds.z/2};  // bottom-bottomLeft

					// Vertical edges connecting top and bottom faces
					vertices[16] = (Vec3){pos.x - bounds.x/2, pos.y + bounds.y/2, pos.z - bounds.z/2};  // top-bottomLeft
					vertices[17] = (Vec3){pos.x - bounds.x/2, pos.y - bounds.y/2, pos.z - bounds.z/2};  // bottom-bottomLeft

					vertices[18] = (Vec3){pos.x - bounds.x/2, pos.y + bounds.y/2, pos.z + bounds.z/2};  // top-topLeft
					vertices[19] = (Vec3){pos.x - bounds.x/2, pos.y - bounds.y/2, pos.z + bounds.z/2};  // bottom-topLeft

					vertices[20] = (Vec3){pos.x + bounds.x/2, pos.y + bounds.y/2, pos.z + bounds.z/2};  // top-topRight
					vertices[21] = (Vec3){pos.x + bounds.x/2, pos.y - bounds.y/2, pos.z + bounds.z/2};  // bottom-topRight

					vertices[22] = (Vec3){pos.x + bounds.x/2, pos.y + bounds.y/2, pos.z - bounds.z/2};  // top-bottomRight
					vertices[23] = (Vec3){pos.x + bounds.x/2, pos.y - bounds.y/2, pos.z - bounds.z/2};  // bottom-bottomRight


					

                    AF_Debug_DrawLineArrayWorld(vertices, verticesCount, lightBlue, FALSE);
                }
            }
       //}
	
	
}

void HandleInput(AF_Input* _input, AF_ECS* _ecs){
	AF_FLOAT x = _input->stick_x;
	AF_FLOAT y = _input->stick_y;
	int vecX = 0;
	int vecY = 0;
	if (y > STICK_DEAD_ZONE){
		vecY = -1;
		//debugf("Stick y %f:\n",y);
	}
	if(y < -STICK_DEAD_ZONE){
		vecY = 1;
		//debugf("Stick y %f:\n",y);
	}


	if(x > STICK_DEAD_ZONE){
		//debugf("Stick y %f:\n",y);
		vecX = 1;
	}
	if(x < -STICK_DEAD_ZONE ){
		//debugf("Stick x %f:\n",x);
		vecX = -1;
	}

	if(x == 0){
		vecX = 0;
	}

	if(y == 0){
		vecY = 0;
	}


	// update the cube rigidbody velocity
	Vec3 newVelocity = {PLAYER_SPEED * vecX, 0, PLAYER_SPEED * vecY};
	cube->rigidbody->velocity = newVelocity;//newVelocity; 

	// toggle the visibility of objects
	if(_input->keys[0].pressed == TRUE){
		// turn off the visibility of meshes
		_ecs->meshes[2].enabled  = FALSE;
		_ecs->meshes[3].enabled  = FALSE;
	}else{
		// turn off the visibility of meshes
		_ecs->meshes[2].enabled  = TRUE;
		_ecs->meshes[3].enabled  = TRUE;
	}

	if(_input->keys[1].pressed == TRUE){
		// turn off the visibility of meshes
		_ecs->colliders[2].showDebug  = FALSE;
		_ecs->colliders[3].showDebug  = FALSE;
	}else{
		_ecs->colliders[2].showDebug  = TRUE;
		_ecs->colliders[3].showDebug  = TRUE;
	}
	//debugf("Upate cube rigid: x: %f y: %f z: %f\n", cube->rigidbody->velocity.x, cube->rigidbody->velocity.y, cube->rigidbody->velocity.z);
	//debugf("Update[1]  rigid: x: %f y: %f z: %f\n", _ecs->rigidbodies[0].velocity.x, _ecs->rigidbodies[0].velocity.y, _ecs->rigidbodies[0].velocity.z);
	//debugf("Update[1]  rigid: x: %f y: %f z: %f\n", _ecs->rigidbodies[1].velocity.x, _ecs->rigidbodies[1].velocity.y, _ecs->rigidbodies[1].velocity.z);
	//debugf("Update[1]  rigid: x: %f y: %f z: %f\n", _ecs->rigidbodies[2].velocity.x, _ecs->rigidbodies[2].velocity.y, _ecs->rigidbodies[2].velocity.z);
}





void Game_Shutdown(void){
	debugf("Game_Shutdown");

}

