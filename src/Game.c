#include "Game.h"
#include <libdragon.h>
#include "ECS/Entities/AF_ECS.h"
#include "AF_Input.h"

// ECS system
AF_Entity* cube;
AF_Entity* plane;
AF_Entity* camera;

// define some const values
#define STICK_DEAD_ZONE 0.01
#define PLAYER_SPEED 2 


// forward decalred functions
void HandleInput(AF_Input* _input, AF_ECS* _ecs);

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
	// add a rigidbody to our cube
	*cube->rigidbody = AF_C3DRigidbody_ADD();
	
	// Create Plane
	plane = AF_ECS_CreateEntity(_ecs);
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
	Vec3 pos = _ecs->rigidbodies[1].velocity;
}

void HandleInput(AF_Input* _input, AF_ECS* _ecs){
	AF_FLOAT x = _input->stick_x;
	AF_FLOAT y = _input->stick_y;
	int vecX = 0;
	int vecY = 0;
	if (y > STICK_DEAD_ZONE){
		vecY = -1;
		debugf("Stick y %f:\n",y);
	}
	if(y < -STICK_DEAD_ZONE){
		vecY = 1;
		debugf("Stick y %f:\n",y);
	}


	if(x > STICK_DEAD_ZONE){
		debugf("Stick y %f:\n",y);
		vecX = 1;
	}
	if(x < -STICK_DEAD_ZONE ){
		debugf("Stick x %f:\n",x);
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
	_ecs->rigidbodies[1].velocity = newVelocity;//newVelocity; 
}

void Game_Shutdown(void){
	debugf("Game_Shutdown");

}

