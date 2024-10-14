#include "Game.h"
#include <libdragon.h>
#include "ECS/Entities/AF_ECS.h"

// ECS system
AF_Entity cube;
AF_Entity plane;
AF_Entity camera;

void Game_Awake(AF_ECS* _ecs){
	if(_ecs == NULL){
		debugf("Game_Awake: passed null ecs reference \n");
		return;
	}

	debugf("Game Awake\n");
	// initialise the ecs system
	// Create Camera

	// Create Cube

	// Create Plane
}
void Game_Start(AF_ECS* _ecs){
	debugf("Game_Start\n");
	if(_ecs == NULL){
		debugf("Game_Start: passed null ecs reference \n");
		return;
	}

}
void Game_Update(AF_ECS* _ecs)
{
	if(_ecs == NULL){
		debugf("Game_Update: passed null ecs reference \n");
		return;
	}
	// game update

	// update cube position based on button press


}

void Game_Shutdown(void){
	debugf("Game_Shutdown");

}

