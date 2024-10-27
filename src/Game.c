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
AF_Entity* cube2;
AF_Entity* plane;
AF_Entity* camera;

// define some const values
#define STICK_DEAD_ZONE 0.01
#define PLAYER_SPEED .25


// forward decalred functions
void HandleInput(AF_Input* _input, AF_ECS* _ecs);
void Game_DrawPhysics(AF_ECS* _ecs);

AF_Entity* CreateCube(AF_ECS* _ecs);

AF_Entity* CreateCube(AF_ECS* _ecs){
	AF_Entity* returnedCube = AF_ECS_CreateEntity(_ecs);
	//move the position up a little
	Vec3 cubePosition = {returnedCube->transform->pos.x, returnedCube->transform->pos.y , returnedCube->transform->pos.z};
	returnedCube->transform->pos = cubePosition;
	// add a rigidbody to our cube
	*returnedCube->rigidbody = AF_C3DRigidbody_ADD();
	*returnedCube->collider = AF_CCollider_Box_ADD();
	*returnedCube->mesh = AF_CMesh_ADD();
	Vec3 cubeBounds = {6, 6, 6};
	returnedCube->collider->boundingVolume = cubeBounds;
	void (*onCollisionCallback)(AF_Collision*) = &Game_OnCollision;
	returnedCube->collider->collision.callback = onCollisionCallback;
	//cube->collider->showDebug = TRUE;
	returnedCube->collider->showDebug = TRUE;
	return returnedCube;
}

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
	cube = CreateCube(_ecs);
	Vec3 cube1Pos = {2.5, 0, -5};
	cube->transform->pos = cube1Pos;
	cube->collider->showDebug = FALSE;
	
	cube2 = CreateCube(_ecs);
	Vec3 cube2Pos = {-5, 0, -5};
	cube2->transform->pos  = cube2Pos;
	cube2->collider->showDebug = TRUE;

	

	
	// Create Plane
	plane = AF_ECS_CreateEntity(_ecs);
	*plane->collider = AF_CCollider_Plane_ADD();
	*plane->mesh = AF_CMesh_ADD();
	Vec3 planePos = {0,-3, 0};
	plane->transform->pos = planePos;
	Vec3 planeBounds = {40, 0.1f, 40};
	plane->collider->boundingVolume = planeBounds;
	plane->collider->showDebug = FALSE;
	plane->rigidbody->inverseMass = 0;
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

	for(int i = 0; i < _ecs->entitiesCount; ++i){
		// clear velocities
		AF_C3DRigidbody* rigidbody =  &_ecs->rigidbodies[i];
		Vec3 zeroVelocity = {0,0,0};
		rigidbody->velocity = zeroVelocity;
	}
	HandleInput(_input, _ecs);
}




// used to run in-between render start and render end. 
// Good for rendering debug data.
void Game_LateUpdate(AF_ECS* _ecs){
	
	//Game_DrawPhysics(_ecs);
	//AF_Physics_DrawCollisions(_ecs);
       
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

	BOOL meshIsEnabled = FALSE;
	BOOL debugIsEnabled = FALSE;
	// update the cube rigidbody velocity
	Vec3 newVelocity = {PLAYER_SPEED * vecX, 0, PLAYER_SPEED * vecY};
	cube->rigidbody->velocity = newVelocity;//newVelocity; 

	// toggle the visibility of objects
	if(_input->keys[0].pressed == TRUE){
		// turn off the visibility of meshes
		//meshIsEnabled = FALSE;
		Vec3 jumpVelocity = {0,1, 0};
		cube->rigidbody->velocity  = Vec3_ADD(cube->rigidbody->velocity, jumpVelocity);
	}else{
		// turn off the visibility of meshes
		meshIsEnabled = TRUE;
	}

	if(_input->keys[1].pressed == TRUE){
		// turn off the visibility of meshes
		debugIsEnabled = FALSE;
	}else{
		debugIsEnabled = TRUE;
	}

	for(int i = 0; i < _ecs->entitiesCount; ++i){
		AF_Component_SetEnabled(_ecs->meshes[i].enabled, meshIsEnabled);
		AF_Component_SetEnabled(_ecs->colliders[i].showDebug, debugIsEnabled);
	}
}



void Game_DrawPhysics(AF_ECS* _ecs){
	float green[4] = {0,255, 0, 1};
	float red[4] = {255,0, 0, 1};
	float lightBlue[4] = {0,255, 255, 1};
	
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
		AF_Debug_DrawLineWorld(&startPoint, &justInFront, green, TRUE);
	}else{
		AF_Debug_DrawLineWorld(&startPoint, &justInFront, red, TRUE);
	}

	for(int i = 0; i < _ecs->entitiesCount; ++i){
		if(_ecs->colliders[i].showDebug == TRUE){
				if(_ecs->colliders[i].collision.collided == TRUE){
					
				}
				
		}
	}
	
}

void Game_OnCollision(AF_Collision* _collision){
	if(_collision == NULL){
		debugf("Game: Game_OnCollision: passed null collision object\n");
		return;
	}
	if(_collision->entity1 == NULL){
		debugf("Game: Game_OnCollision: entity 1 is null\n");
		return;
	}
	AF_Entity* entity = (AF_Entity*)_collision->entity1;
	if(entity == NULL){
		debugf("Game: Game_OnCollision: entity is null\n");
		return;
	}
	AF_CCollider* collider = entity->collider;
	if(collider == NULL){
		debugf("Game: Game_OnCollision: collider is null\n");
		return;
	}
	/*if(AF_Component_GetHas(collider->enabled) != FALSE){
		debugf("Game: Game_OnCollision: entity 1 has no collider component\n");
		return;
	}*/
	
	// do collision things
}



void Game_Shutdown(void){
	debugf("Game_Shutdown");

}

