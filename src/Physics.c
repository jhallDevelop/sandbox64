/*
===============================================================================
PHYSICS Implementation 
n64 implementation of AF_Physics
===============================================================================
*/
#include <libdragon.h>
#include "AF_Physics.h"
#include "ECS/Entities/AF_ECS.h"

float collisionColor[4] = {255,0, 0, 1};
// Physics Init
void AF_Physics_Init(AF_ECS* _ecs){
	debugf("Physics_Init: \n");
	if(_ecs == NULL){
		debugf("Physics: Physics_Init pass in a null reference to _ecs\n");
		return;
	}
}


// Physics Update
void AF_Physics_Update(AF_ECS* _ecs, const float _dt){
	if(_ecs == NULL){
		debugf("Physics: Physics_Update pass in a null reference to _ecs\n");
		return;
	}
	// loop through and update all transforms based on their velocities
	for(int i = 0; i < _ecs->entitiesCount; ++i){
		AF_CCollider* collider = &_ecs->colliders[i];
		AF_C3DRigidbody* rigidbody = &_ecs->rigidbodies[i];

		
		//if(_ecs->rigidbodies[i].enabled == TRUE){
			// update the transform based on the rigidbody
		//if(_ecs->rigidbodies[i].inverseMass >0 ){
		//	Vec3 gravityVelocity = {0, _ecs->rigidbodies[i].gravity *0.05, 0};
		//	rigidbody->velocity = Vec3_ADD(_ecs->rigidbodies[i].velocity, gravityVelocity);
		//}
		//debgf("Physics: upate: velocity x: %f y: %f z: %f\n", rigidbody->velocity.x, rigidbody->velocity.y, rigidbody->velocity.z);
		AF_Physics_IntegrateAccell(rigidbody, _dt);
		
		_ecs->transforms[i].pos = Vec3_ADD(_ecs->rigidbodies[i].velocity, _ecs->transforms[i].pos);

		// update the bounds position
		collider->pos = _ecs->transforms[i].pos;

			
		// clear all collsision except keep the callback
		AF_Collision clearedCollision = {FALSE, NULL, NULL, collider->collision.callback, {0,0,0}, 0.0f, {0,0,0}, 0};
		collider->collision = clearedCollision;
	}
}

void AF_Physics_LateUpdate(AF_ECS* _ecs){
	

	// Do collision tests
	AF_Physics_AABB_Test(_ecs);

	// Resolve collision between two objects
}

void AF_Physics_LateRenderUpdate(AF_ECS* _ecs){
	for(int i = 0; i < _ecs->entitiesCount; ++i){
		AF_CCollider* collider = &_ecs->colliders[i];
		if(collider->showDebug != TRUE){
			//debugf("Physics: LateRenderUpate: not showing debug %i\n", i);
			continue;
		}

		if(collider->collision.collided != TRUE){
			//debugf("Physics: LateRenderUpate: not colided\n");
			continue;
		}
		//debugf("Physics: LateRenderUpate: draw debug\n");
		AF_Physics_DrawBox(collider, collisionColor);	
	}
}



//Physics Shutdown
void AF_Physics_Shutdown(void){
	debugf("Physics: Shutdown\n");
}

