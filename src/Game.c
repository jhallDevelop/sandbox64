#include "Game.h"
#include <libdragon.h>
#include <inttypes.h>
#include "ECS/Entities/AF_ECS.h"
#include "AF_Input.h"
#include "AF_Debug.h"
#include "AF_Util.h"
#include "AF_Ray.h"
#include "AF_Physics.h"
#include "AF_Audio.h"
#include "AF_UI.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/gl_integration.h>

// Sounds


// ECS system
AF_Entity* cube;
AF_Entity* cube2;
AF_Entity* trigger1;
AF_Entity* plane;
AF_Entity* camera;
AF_Entity* animatedSprite;
AF_Entity* textBlock;
AF_Entity* laserSoundEntity;
AF_Entity* cannonSoundEntity;
AF_Entity* musicTrackSoundEntity;



// define some const values
#define STICK_DEAD_ZONE 0.01
#define PLAYER_SPEED 20

// Text / Font
const char* fontPath = "rom:/Pacifico.font64";
const char* fontPath2 = "rom:/FerriteCoreDX.font64";
const char* musicPath = "rom:/monosample8.wav64";

// sprites
const char* animatedSpritePath = "";

// Sound
// data type to hold special n64 data
wav64_t sfx_cannon, sfx_laser, sfx_music;
// Mixer channel allocation
#define CHANNEL_SFX1    0
#define CHANNEL_SFX2    1
#define CHANNEL_MUSIC   2
const char* soundFX1Path = "rom:/cannon.wav64";
const char* soundFX2Path = "rom:/laser.wav64";

// Text
const char *textMessage = "SANDBOX64 - 0.01\n";
const char *testText = 
            "Two $02households$01, both alike in dignity,\n"
            "In $02fair Verona$01, where we lay our scene,\n"
            "From ancient grudge break to new $02mutiny$01,\n"
            "Where $02civil blood$01 makes civil hands unclean.\n"
            "From forth the fatal loins of these $02two foes$01\n"
            "A pair of $02star-cross'd lovers$01 take their life;\n";


// Sprite
//Animation frame size defines
#define ANIM_FRAME_W 120
#define ANIM_FRAME_H 80

//Animation frame timing defines
#define ANIM_FRAME_DELAY 3
#define ANIM_FRAME_MAX 6

static sprite_t *sheet_knight;
const char* sheet_knightPath = "rom:/knight.sprite";

// forward decalred functions
void HandleInput(AF_Input* _input, AF_ECS* _ecs);
void Game_DrawPhysics(AF_ECS* _ecs);
void Game_SetupEntities(AF_ECS* _ecs);
AF_Entity* CreateCube(AF_ECS* _ecs);
AF_Entity* CreateAudioEntity(AF_ECS* _ecs, AF_AudioClip _audioClip, uint8_t _channel, void* _waveData, BOOL _isLooping);
AF_Entity* CreateSprite(AF_ECS* _ecs, const char* _spritePath, Vec2 _screenPos, Vec2 _size, uint8_t _color[4], char _animationFrames, Vec2 _spriteSheetSize, void* _spriteData);
void Game_OnTrigger(AF_Collision* _collision);


AF_Entity* CreateSprite(AF_ECS* _ecs, const char* _spritePath, Vec2 _screenPos, Vec2 _size, uint8_t _color[4], char _animationFrames, Vec2 _spriteSheetSize, void* _spriteData){
	AF_Entity* entity = AF_ECS_CreateEntity(_ecs);

	AF_CSprite* sprite = entity->sprite;
	*sprite = AF_CSprite_ADD();
	sprite->spritePath = _spritePath;
	sprite->pos = _screenPos;
	sprite->size = _size;
	// TODO: fix this
	sprite->spriteColor[0] = _color[0];
	sprite->spriteColor[1] = _color[1];
	sprite->spriteColor[2] = _color[2];
	sprite->spriteColor[3] = _color[3];
	sprite->animationFrames = _animationFrames;
	sprite->spriteSheetSize = _spriteSheetSize;

	
	//Load Sprite Sheet
    sprite->spriteData = sprite_load(sprite->spritePath);
	if(sprite->spriteData == NULL){
		debugf("Game: CreateSprite: Failed to load sprite");
		return NULL;
	}
	return entity;
}

AF_Entity* CreateCube(AF_ECS* _ecs){
	AF_Entity* returnedCube = AF_ECS_CreateEntity(_ecs);
	//move the position up a little
	Vec3 cubePosition = {returnedCube->transform->pos.x, returnedCube->transform->pos.y , returnedCube->transform->pos.z};
	returnedCube->transform->pos = cubePosition;
	// add a rigidbody to our cube
	*returnedCube->rigidbody = AF_C3DRigidbody_ADD();
	*returnedCube->collider = AF_CCollider_Box_ADD();
	*returnedCube->mesh = AF_CMesh_ADD();
	returnedCube->mesh->meshType = AF_MESH_TYPE_CUBE;
	Vec3 cubeBounds = {6, 6, 6};
	returnedCube->collider->boundingVolume = cubeBounds;
	void (*onCollisionCallback)(AF_Collision*) = &Game_OnCollision;
	returnedCube->collider->collision.callback = onCollisionCallback;
	//cube->collider->showDebug = TRUE;
	returnedCube->collider->showDebug = TRUE;

	return returnedCube;
}

AF_Entity* CreateAudioEntity(AF_ECS* _ecs, AF_AudioClip _audioClip, uint8_t _channel, void* _wavData, BOOL _isLooping){
	// NULL checks
	if(_ecs == NULL){
		debugf("CreateAudioEntity: passed in a null _ecs\n");
		return NULL;
	}

	if(_wavData == NULL){
		debugf("CreateAudioEntity: passed in a null _wavData\n");
		return NULL;
	}
	AF_Entity* returnEntity = AF_ECS_CreateEntity(_ecs);
	*returnEntity->audioSource = AF_CAudioSource_ADD();


	AF_CAudioSource* audioSource = returnEntity->audioSource;
	audioSource->channel = _channel;
	audioSource->loop = _isLooping;
	audioSource->clipData = _wavData;
	audioSource->clip = _audioClip;
	// Bump maximum frequency of music channel to 128k.
	// The default is the same of the output frequency (44100), but we want to
	// let user increase it.
	mixer_ch_set_limits(audioSource->channel, 0, audioSource->clip.clipFrequency, 0);

	wav64_open((wav64_t*)audioSource->clipData, audioSource->clip.clipPath);
	wav64_set_loop((wav64_t*)audioSource->clipData, audioSource->loop);
	

	return returnEntity;
}

void Game_Awake(AF_ECS* _ecs){
	if(_ecs == NULL){
		debugf("Game_Awake: passed null ecs reference \n");
		return;
	}
	debugf("Game Awake\n");
	
	Game_SetupEntities(_ecs);
	
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
		//rigidbody->velocity = zeroVelocity;
	}
	HandleInput(_input, _ecs);

	// Check whether one audio buffer is ready, otherwise wait for next
	// frame to perform mixing.

	// TODO: re-enable for audio
	/*
	if (audio_can_write()) {    	
		short *buf = audio_write_begin();
		mixer_poll(buf, audio_get_buffer_length());
		audio_write_end();
	}*/
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
		Vec3 jumpVelocity = {0,30, 0};
		//cube->rigidbody->velocity  = Vec3_ADD(cube->rigidbody->velocity, jumpVelocity);
		AF_Physics_ApplyLinearImpulse(cube->rigidbody, jumpVelocity);

		// play sound
		AF_Audio_Play(laserSoundEntity->audioSource, 0.25f, FALSE);

		// play animation
		AF_UI_RendererSprite_Play(animatedSprite->sprite, FALSE);

		
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

// Setup the games entities
void Game_SetupEntities(AF_ECS* _ecs){
	// initialise the ecs system
	// Create Camera
	camera = AF_ECS_CreateEntity(_ecs);

	// Create Player
	cube = CreateCube(_ecs);
	Vec3 cube1Pos = {2.5, 0, -5};
	cube->transform->pos = cube1Pos;
	cube->collider->showDebug = FALSE;
	cube->rigidbody->inverseMass = 1;
	
	// Create Enemy
	cube2 = CreateCube(_ecs);
	Vec3 cube2Pos = {-5, 0, -5};
	cube2->transform->pos  = cube2Pos;
	cube2->collider->showDebug = TRUE;
	cube2->rigidbody->inverseMass = 1;

	// Create Trigger
	trigger1 = CreateCube(_ecs);
	Vec3 trigger1Pos = {-10, 0, 0};
	cube2->transform->pos  = trigger1Pos;
	cube2->rigidbody->inverseMass = 0;
	cube2->collider->collision.callback = Game_OnTrigger;

	
	// Create Plane
	plane = AF_ECS_CreateEntity(_ecs);
	*plane->collider = AF_CCollider_Plane_ADD();
	*plane->mesh = AF_CMesh_ADD();
	plane->mesh->meshType = AF_MESH_TYPE_PLANE;
	Vec3 planePos = {0,-5, 0};
	plane->transform->pos = planePos;
	Vec3 planeBounds = {40, 1.0f, 40};
	plane->collider->boundingVolume = planeBounds;
	plane->collider->showDebug = FALSE;
	plane->rigidbody->inverseMass = 0;
	//plane->transform->pos = planePos;
	//plane->transform->pos = planePos;

	// Setup Audio
	// TODO: put into audio function
	audio_init(44100, 4);
	mixer_init(16);  // Initialize up to 16 channels

	AF_AudioClip musicAudioClip = {0, musicPath, 12800};
	laserSoundEntity = CreateAudioEntity(_ecs, musicAudioClip, CHANNEL_MUSIC, (void*)&sfx_music, TRUE);

	AF_AudioClip sfx1AudioClip = {0, soundFX1Path, 128000};
	laserSoundEntity = CreateAudioEntity(_ecs, sfx1AudioClip, CHANNEL_SFX1, (void*)&sfx_laser, FALSE);

	AF_AudioClip sfx2AudioClip = {0, soundFX2Path, 128000};
	cannonSoundEntity = CreateAudioEntity(_ecs, sfx2AudioClip, CHANNEL_SFX2, (void*)&sfx_cannon, FALSE);

	

	bool music = false;
	int music_frequency = sfx_music.wave.frequency;

	music = !music;
	if (music) {
		//wav64_play(&sfx_music, CHANNEL_MUSIC);
		//music_frequency = sfx_music.wave.frequency;
	}

	
	// Create test Text
	textBlock = AF_ECS_CreateEntity(_ecs);
	*textBlock->text = AF_CText_ADD();

	textBlock->text->text = textMessage;
	textBlock->text->fontID = 1;
	textBlock->text->fontPath = fontPath2;

	// Text Color
	textBlock->text->textColor[0] = 255;
	textBlock->text->textColor[1] = 255;
	textBlock->text->textColor[2] = 255;
	textBlock->text->textColor[3] = 255;

	// Text position
    int box_width = 262;
    int box_height = 0;//150;
    int x0 = 10;//(320-box_width);///2;
	int y0 = 20;//(240-box_height);///2; 

	Vec2 textScreenPos = {x0, y0};
	Vec2 textBounds = {box_width, box_height};
    textBlock->text->screenPos = textScreenPos;
	textBlock->text->textBounds = textBounds;

	// Create sprites
	Vec2 spritePos = {10, 20};
	Vec2 spriteSize = {120, 80};
	uint8_t spriteColor[4] = {255, 0, 0, 255};
	Vec2 spriteSheetSize = {spriteSize.x *6,spriteSize.y * 1};
	animatedSprite = CreateSprite(_ecs, sheet_knightPath, spritePos, spriteSize, spriteColor, 1, spriteSheetSize, (void*)sheet_knight);
	animatedSprite->sprite->animationFrames = 6;
	animatedSprite->sprite->animationSpeed = (1.0f) * 1000000; // Convert to microseconds if timer_ticks() is in microseconds ;		// about 33 milliseconds / 30fps
	animatedSprite->sprite->loop = FALSE;
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

void Game_OnTrigger(AF_Collision* _collision){
	AF_Entity* entity1 =  _collision->entity1;
	AF_Entity* entity2 =  _collision->entity2;
	uint32_t entity1ID = AF_ECS_GetID(entity1->id_tag);
	uint32_t entity2ID = AF_ECS_GetID(entity2->id_tag);
	PACKED_UINT32 entity1Tag = AF_ECS_GetTag(entity1->id_tag);
	PACKED_UINT32 entity2Tag = AF_ECS_GetTag(entity2->id_tag);
	debugf("Game_OnTrigger: Entity %lu hit Entity %lu \n", entity1ID, entity2ID);
	
	// Play sound
	AF_Audio_Play(cannonSoundEntity->audioSource, 1.0f, FALSE);
	//wav64_play(&sfx_cannon, CHANNEL_SFX1);
}



void Game_Shutdown(void){
	debugf("Game_Shutdown");

}


//// TODO: get rid of this
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

