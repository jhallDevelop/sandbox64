/*
===============================================================================
PHYSICS Implementation 
n64 implementation of AF_Physics
===============================================================================
*/
#include <libdragon.h>
#include "AF_Physics.h"
#include "ECS/Entities/AF_ECS.h"
// Physics Init
void AF_Physics_Init(AF_ECS* _ecs){
	debugf("Physics_Init: \n");
	if(_ecs == NULL){
		debugf("Physics: Physics_Init pass in a null reference to _ecs\n");
		return;
	}
}


// Physics Update
void AF_Physics_Update(AF_ECS* _ecs){
	if(_ecs == NULL){
		debugf("Physics: Physics_Update pass in a null reference to _ecs\n");
		return;
	}
	// loop through and update all transforms based on their velocities
	for(int i = 0; i < _ecs->entitiesCount; i++){
		//if(_ecs->rigidbodies[i].enabled == TRUE){
			// update the transform based on the rigidbody
			Vec3 vel = _ecs->rigidbodies[i].velocity;
			Vec3 pos = _ecs->transforms[i].pos; 
			Vec3 updatedPos = {pos.x + vel.x, pos.y + vel.y, pos.z + vel.z}; 
			_ecs->transforms[i].pos = updatedPos;
		//}
	}

}

//Physics Shutdown
void AF_Physics_Shutdown(void){
	debugf("Physics: Shutdown\n");
}

