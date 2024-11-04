#include "Game.h"
#include <libdragon.h>

#include <inttypes.h>
#include "ECS/Entities/AF_ECS.h"
#include "AF_Input.h"
#include "AF_Debug.h"
#include "AF_Util.h"
//#include "AF_Ray.h"
#include "AF_Physics.h"
#include "AF_Audio.h"
#include "AF_UI.h"
#include "ECS/Components/AF_CMesh.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/gl_integration.h>

#include "PlayerController.h"
// Sounds


// ECS system
AF_Entity* camera;
AF_Entity* player1Entity;
AF_Entity* player2Entity;
AF_Entity* player3Entity;
AF_Entity* player4Entity;

// God
AF_Entity* godEntity;

// Environment
AF_Entity* leftWall;
AF_Entity* rightWall;
AF_Entity* backWall;
AF_Entity* frontWall;
AF_Entity* groundPlaneEntity;
AF_Entity* gameTitleEntity;

// Timer
AF_Entity* godEatCountLabelEntity;
AF_Entity* countdownTimerLabelEntity;

// Pickup
AF_Entity* bucket1;
AF_Entity* bucket2;
AF_Entity* bucket3;
AF_Entity* bucket4;


// define some const values


// Textures
//const char* circleTexturePath = "rom:/circle0.sprite";


// Text / Font
const char* fontPath = "rom:/Pacifico.font64";
const char* fontPath2 = "rom:/FerriteCoreDX.font64";

// sprites
const char* animatedSpritePath = "";

// Sound
// data type to hold special n64 data
wav64_t sfx_cannon, sfx_laser, sfx_music;
// Mixer channel allocation
#define CHANNEL_SFX1    0
#define CHANNEL_SFX2    1
#define CHANNEL_MUSIC   2
//const char* soundFX1Path = "rom:/cannon.wav64";

// Text
const char *titleText = "oldGods64 - 0.00003\n";
char godsCountLabelText[20] = "666";
char countdownTimerLabelText[20] = "6666";

// Sprite
//Animation frame size defines
#define ANIM_FRAME_W 120
#define ANIM_FRAME_H 80

//Animation frame timing defines
#define ANIM_FRAME_DELAY 3
#define ANIM_FRAME_MAX 6

// Gameplay Vars
int godEatCount = 0;

float countdownTimer = 120;

enum GAME_STATE{
    GAME_STATE_GAME_OVER = 0,
    GAME_STATE_PLAYING = 1
};

enum GAME_STATE gameState = GAME_STATE_PLAYING;

//static sprite_t *sheet_knight;
//const char* sheet_knightPath = "rom:/knight.sprite";

// forward decalred functions
void HandleInput(AF_Input* _input, AF_ECS* _ecs);
void Game_DrawPhysics(AF_ECS* _ecs);
void Game_SetupEntities(AF_ECS* _ecs);
AF_Entity* CreateCube(AF_ECS* _ecs);
AF_Entity* CreateAudioEntity(AF_ECS* _ecs, AF_AudioClip _audioClip, uint8_t _channel, void* _waveData, BOOL _isLooping);
AF_Entity* CreateSprite(AF_ECS* _ecs, const char* _spritePath, Vec2 _screenPos, Vec2 _size, uint8_t _color[4], char _animationFrames, Vec2 _spriteSheetSize, void* _spriteData);
AF_Entity* CreatePrimative(AF_ECS* _ecs, Vec3 _pos, Vec3 _bounds, enum AF_MESH_TYPE _meshType, enum CollisionVolumeType _collisionType, void* _collisionCallback);
void Game_OnTrigger(AF_Collision* _collision);
void Game_OnGodTrigger(AF_Collision* _collision);
void Game_OnBucketTrigger(AF_Collision* _collision);
void UpdateText(AF_ECS* _ecs);


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

AF_Entity* CreatePrimative(AF_ECS* _ecs, Vec3 _pos, Vec3 _bounds, enum AF_MESH_TYPE _meshType, enum CollisionVolumeType _collisionType, void* _collisionCallback){
	AF_Entity* entity = AF_ECS_CreateEntity(_ecs);
	//move the position up a little
	entity->transform->pos = _pos;
	// add a rigidbody to our cube
	*entity->rigidbody = AF_C3DRigidbody_ADD();
	
	*entity->collider = AF_CCollider_ADD_TYPE(_collisionType);//AF_CCollider_Box_ADD();
	*entity->mesh = AF_CMesh_ADD();
	entity->mesh->meshType = _meshType;
	entity->collider->boundingVolume = Vec3_MULT_SCALAR(_bounds, 2);;
	//void (*onCollisionCallback)(AF_Collision*) = &Game_OnCollision;
	entity->collider->collision.callback = _collisionCallback;
	entity->collider->showDebug = FALSE;

	return entity;
}

AF_Entity* CreateCube(AF_ECS* _ecs){
	AF_Entity* returnedCube = AF_ECS_CreateEntity(_ecs);
	//move the position up a little
	Vec3 cubePosition = {returnedCube->transform->pos.x, returnedCube->transform->pos.y , returnedCube->transform->pos.z};
	Vec3 cubeScale = {returnedCube->transform->scale.x, returnedCube->transform->scale.y , returnedCube->transform->scale.z};
	returnedCube->transform->pos = cubePosition;
	// add a rigidbody to our cube
	*returnedCube->rigidbody = AF_C3DRigidbody_ADD();
	*returnedCube->collider = AF_CCollider_Box_ADD();
	*returnedCube->mesh = AF_CMesh_ADD();
	returnedCube->mesh->meshType = AF_MESH_TYPE_CUBE;
	returnedCube->collider->boundingVolume = cubeScale;
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
void Game_Update(AF_Input* _input, AF_ECS* _ecs, AF_Time* _time)
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
    //debugf("timer tick %li countdown %f\n", _time->currentTick, _time->timeSinceLastFrame*.01);
    countdownTimer -= _time->timeSinceLastFrame;
    if(countdownTimer <= 0){
        gameState = GAME_STATE_GAME_OVER;
        countdownTimer = 120;
    }
    UpdateText(_ecs);
}

// used to run in-between render start and render end. 
// Good for rendering debug data.
void Game_LateUpdate(AF_ECS* _ecs){
	
	//Game_DrawPhysics(_ecs);
	//AF_Physics_DrawCollisions(_ecs);

}


void HandleInput(AF_Input* _input, AF_ECS* _ecs){

    // player 1
    Vec2 player1Stick = {_input->stick_x, _input->stick_y};
    UpdatePlayerMovement(player1Stick, player1Entity);
    // Player 2
    Vec2 player2Stick = {_input->stick_x2, _input->stick_y2};
    UpdatePlayerMovement(player2Stick, player2Entity);
    // Player 3
    Vec2 player3Stick = {_input->stick_x3, _input->stick_y3};
    UpdatePlayerMovement(player3Stick, player3Entity);
    // Player 4
    Vec2 player4Stick = {_input->stick_x4, _input->stick_y4};
    UpdatePlayerMovement(player4Stick, player4Entity);
}



// Setup the games entities
void Game_SetupEntities(AF_ECS* _ecs){
	// initialise the ecs system
	// Create Camera
	camera = AF_ECS_CreateEntity(_ecs);
	
    // Create God
	
	Vec3 godPos = {0, 5, -12};
	Vec3 godScale = {5,5,5};
    godEntity = CreatePrimative(_ecs, godPos, godScale, AF_MESH_TYPE_SPHERE, AABB, Game_OnGodTrigger);
	//godEntity->transform->pos = godPos;
	//godEntity->transform->scale = godScale;
	//godEntity->collider->boundingVolume = Vec3_MULT_SCALAR(godScale, 2);
    godEntity->mesh->material.textureID = 0;
	godEntity->collider->showDebug = TRUE;
	godEntity->rigidbody->inverseMass = 0;
	godEntity->rigidbody->isKinematic = TRUE;

    
	// Create Player1
	player1Entity = CreateCube(_ecs);
	Vec3 player1Pos = {2.5, 1.5, -5};
	Vec3 player1Scale = {1,1,1};
	player1Entity->transform->pos = player1Pos;
	player1Entity->transform->scale = player1Scale;
	player1Entity->collider->boundingVolume = Vec3_MULT_SCALAR(player1Scale, 2);
	player1Entity->collider->showDebug = FALSE;
    player1Entity->mesh->material.textureID = 0;
	player1Entity->rigidbody->inverseMass = 1;
	player1Entity->rigidbody->isKinematic = TRUE;

    // Create Player2
	player2Entity = CreateCube(_ecs);
	Vec3 player2Pos = {-2.5, 1.5, -5};
	Vec3 player2Scale = {1,1,1};
	player2Entity->transform->pos = player2Pos;
	player2Entity->transform->scale = player2Scale;
	player2Entity->collider->boundingVolume = Vec3_MULT_SCALAR(player2Scale, 2);
	player2Entity->collider->showDebug = FALSE;
    player2Entity->mesh->material.textureID = 0;
	player2Entity->rigidbody->inverseMass = 1;
	player2Entity->rigidbody->isKinematic = TRUE;

    // Create Player3
	player3Entity = CreateCube(_ecs);
	Vec3 player3Pos = {-2.5, 1.5, 5};
	Vec3 player3Scale = {1,1,1};
	player3Entity->transform->pos = player3Pos;
	player3Entity->transform->scale = player3Scale;
	player3Entity->collider->boundingVolume = Vec3_MULT_SCALAR(player3Scale, 2);
	player3Entity->collider->showDebug = FALSE;
    player3Entity->mesh->material.textureID = 0;
	player3Entity->rigidbody->inverseMass = 1;
	player3Entity->rigidbody->isKinematic = TRUE;

    // Create Player4
	player4Entity = CreateCube(_ecs);
	Vec3 player4Pos = {2.5, 1.5, 5};
	Vec3 player4Scale = {1,1,1};
	player4Entity->transform->pos = player4Pos;
	player4Entity->transform->scale = player4Scale;
	player4Entity->collider->boundingVolume = Vec3_MULT_SCALAR(player4Scale, 2);
	player4Entity->collider->showDebug = FALSE;
    player4Entity->mesh->material.textureID = 0;
	player4Entity->rigidbody->inverseMass = 1;
	player4Entity->rigidbody->isKinematic = TRUE;
	

	//====ENVIRONMENT========
	// Create Plane
	groundPlaneEntity = CreateCube(_ecs);
	Vec3 planePos = {0, -2, 0};
	Vec3 planeScale = {20,1,20};
	groundPlaneEntity->transform->pos = planePos;
	groundPlaneEntity->transform->scale = planeScale;
	groundPlaneEntity->collider->boundingVolume = Vec3_MULT_SCALAR(planeScale, 2);
	groundPlaneEntity->collider->showDebug = FALSE;
    groundPlaneEntity->mesh->material.textureID = 0;
	groundPlaneEntity->rigidbody->inverseMass = 0;

    // Create Left Wall
    float wallHeight = 3;
	leftWall = CreateCube(_ecs);
	Vec3 leftWallPos = {-20, 0, 0};
	Vec3 leftWallScale = {1,wallHeight,20};
	leftWall->transform->pos = leftWallPos;
	leftWall->transform->scale = leftWallScale;
	leftWall->collider->boundingVolume = Vec3_MULT_SCALAR(leftWallScale, 2);
	leftWall->collider->showDebug = FALSE;
    leftWall->mesh->material.textureID = 0;
	leftWall->rigidbody->inverseMass = 0;

    // Create Right Wall
	rightWall = CreateCube(_ecs);
	Vec3 rightWallPos = {20, 0, 0};
	Vec3 rightWallScale = {1,wallHeight,20};
    rightWall->transform->pos = rightWallPos;
	rightWall->transform->scale = rightWallScale;
	rightWall->collider->boundingVolume = Vec3_MULT_SCALAR(rightWallScale, 2);
	rightWall->collider->showDebug = FALSE;
    rightWall->mesh->material.textureID = 0;
	rightWall->rigidbody->inverseMass = 0;

    // Create Back Wall
	backWall = CreateCube(_ecs);
	Vec3 backWallPos = {0, 0, -20};
	Vec3 backWallScale = {20,wallHeight,1};
    backWall->transform->pos = backWallPos;
	backWall->transform->scale = backWallScale;
	backWall->collider->boundingVolume = Vec3_MULT_SCALAR(backWallScale, 2);
	backWall->collider->showDebug = FALSE;
    rightWall->mesh->material.textureID = 0;
	backWall->rigidbody->inverseMass = 0;

    // Create Front Wall
	frontWall = CreateCube(_ecs);
	Vec3 frontWallPos = {0, 0, 20};
	Vec3 frontWallScale = {20,wallHeight,1};
    frontWall->transform->pos = backWallPos;
	frontWall->transform->scale = frontWallScale;
	frontWall->collider->boundingVolume = Vec3_MULT_SCALAR(frontWallScale, 2);
	frontWall->collider->showDebug = FALSE;
    frontWall->mesh->material.textureID = 0;
	frontWall->rigidbody->inverseMass = 0;


    // Bucket 1
    bucket1 = CreateCube(_ecs);
	Vec3 bucket1Pos = {-10, 2.5, -10};
	Vec3 bucket1Scale = {1,1,1};
	bucket1->transform->pos = bucket1Pos;
	bucket1->transform->scale = bucket1Scale;
	bucket1->collider->boundingVolume = Vec3_MULT_SCALAR(bucket1Scale, 2);
	bucket1->collider->showDebug = TRUE;
    bucket1->mesh->material.textureID = 0;
	bucket1->rigidbody->inverseMass = 0;
	bucket1->rigidbody->isKinematic = TRUE;
    bucket1->collider->collision.callback = Game_OnBucketTrigger;

    // Bucket 2
    bucket2 = CreateCube(_ecs);
	Vec3 bucket2Pos = {10, 2.5, -10};
	Vec3 bucket2Scale = {1,1,1};
	bucket2->transform->pos = bucket2Pos;
	bucket2->transform->scale = bucket2Scale;
	bucket2->collider->boundingVolume = Vec3_MULT_SCALAR(bucket2Scale, 2);
	bucket2->collider->showDebug = TRUE;
    bucket2->mesh->material.textureID = 0;
	bucket2->rigidbody->inverseMass = 0;
	bucket2->rigidbody->isKinematic = TRUE;
    bucket2->collider->collision.callback = Game_OnBucketTrigger;


    // Bucket 3
    bucket3 = CreateCube(_ecs);
	Vec3 bucket3Pos = {-10, 2.5, 10};
	Vec3 bucket3Scale = {1,1,1};
	bucket3->transform->pos = bucket3Pos;
	bucket3->transform->scale = bucket3Scale;
	bucket3->collider->boundingVolume = Vec3_MULT_SCALAR(bucket3Scale, 2);
	bucket3->collider->showDebug = TRUE;
    bucket3->mesh->material.textureID = 0;
	bucket3->rigidbody->inverseMass = 0;
	bucket3->rigidbody->isKinematic = TRUE;
    bucket3->collider->collision.callback = Game_OnBucketTrigger;

    // Bucket 4
    bucket4 = CreateCube(_ecs);
	Vec3 bucket4Pos = {10, 2.5, 10};
	Vec3 bucket4Scale = {1,1,1};
	bucket4->transform->pos = bucket4Pos;
	bucket4->transform->scale = bucket4Scale;
	bucket4->collider->boundingVolume = Vec3_MULT_SCALAR(bucket4Scale, 2);
	bucket4->collider->showDebug = TRUE;
    bucket4->mesh->material.textureID = 0;
	bucket4->rigidbody->inverseMass = 0;
	bucket4->rigidbody->isKinematic = TRUE;
    bucket4->collider->collision.callback = Game_OnBucketTrigger;


	// Setup Audio
	// TODO: put into audio function
	//audio_init(44100, 4);
	//mixer_init(16);  // Initialize up to 16 channels

	//AF_AudioClip musicAudioClip = {0, musicPath, 12800};
	//laserSoundEntity = CreateAudioEntity(_ecs, musicAudioClip, CHANNEL_MUSIC, (void*)&sfx_music, TRUE);


	
	
	// Create test Text
    int font1 = 1;
    int font2 = 2;
    float whiteColor[4];
    whiteColor[0] = 255;
    whiteColor[1] = 255;
    whiteColor[2] = 255;
    whiteColor[3] = 255;

    AF_LoadFont(font1, fontPath, whiteColor);
    AF_LoadFont(font2, fontPath2, whiteColor);
	gameTitleEntity = AF_ECS_CreateEntity(_ecs);
	*gameTitleEntity->text = AF_CText_ADD();

	gameTitleEntity->text->text = titleText;
	gameTitleEntity->text->fontID = 1;
	gameTitleEntity->text->fontPath = fontPath;

	// Text Color
	//gameTitleEntity->text->textColor = whiteColor;
    gameTitleEntity->text->fontID = font1;

	// Title Text position
    int box_width = 262;
    int box_height = 0;//150;
    int x0 = 10;//(320-box_width);///2;
	int y0 = 20;//(240-box_height);///2; 

	Vec2 textScreenPos = {x0, y0};
	Vec2 textBounds = {box_width, box_height};
    gameTitleEntity->text->screenPos = textScreenPos;
	gameTitleEntity->text->textBounds = textBounds;

    // ======God Eat Label Text position
    godEatCountLabelEntity = AF_ECS_CreateEntity(_ecs);
	*godEatCountLabelEntity->text = AF_CText_ADD();

	godEatCountLabelEntity->text->text = godsCountLabelText;//godsCountLabelText;
	godEatCountLabelEntity->text->fontID = 2;
	godEatCountLabelEntity->text->fontPath = fontPath2;

	// Text Color
	//godEatCountLabelEntity->text->textColor = whiteColor;
    godEatCountLabelEntity->text->fontID = font2;

    int godLabelBox_width = 262;
    int godLabelBox_height = 0;//150;
    int godLabelBoxPosX = 250;//(320-box_width);///2;
	int godLabelBoxPosY = 20;//(240-box_height);///2; 

	Vec2 godLabelTextScreenPos = {godLabelBoxPosX, godLabelBoxPosY};
	Vec2 godLabelTextBounds = {godLabelBox_width, godLabelBox_height};
    godEatCountLabelEntity->text->screenPos = godLabelTextScreenPos;
	godEatCountLabelEntity->text->textBounds = godLabelTextBounds;

    // ======Count down timer Text position
    countdownTimerLabelEntity = AF_ECS_CreateEntity(_ecs);
	*countdownTimerLabelEntity->text = AF_CText_ADD();

	countdownTimerLabelEntity->text->text = countdownTimerLabelText;//godsCountLabelText;
	countdownTimerLabelEntity->text->fontID = 2;
	countdownTimerLabelEntity->text->fontPath = fontPath2;

	// Text Color
	//countdownTimerLabelEntity->text->textColor = whiteColor;
    countdownTimerLabelEntity->text->fontID = font1;

    int countdownTimerBox_width = 262;
    int countdownTimerBox_height = 0;//150;
    int countdownTimerBoxPosX = 150;//(320-box_width);///2;
	int countdownTimerBoxPosY = 20;//(240-box_height);///2; 

	Vec2 countdownTimerLabelTextScreenPos = {countdownTimerBoxPosX, countdownTimerBoxPosY};
	Vec2 countdownTimerLabelTextBounds = {countdownTimerBox_width, countdownTimerBox_height};
    countdownTimerLabelEntity->text->screenPos = countdownTimerLabelTextScreenPos;
	countdownTimerLabelEntity->text->textBounds = countdownTimerLabelTextBounds;

	// Create sprites
    /*
	Vec2 spritePos = {10, 20};
	Vec2 spriteSize = {120, 80};
	uint8_t spriteColor[4] = {255, 0, 0, 255};
	Vec2 spriteSheetSize = {spriteSize.x *6,spriteSize.y * 1};
	animatedSprite = CreateSprite(_ecs, sheet_knightPath, spritePos, spriteSize, spriteColor, 1, spriteSheetSize, (void*)sheet_knight);
	animatedSprite->sprite->animationFrames = 6;
	animatedSprite->sprite->animationSpeed = (1.0f) * 1000000; // Convert to microseconds if timer_ticks() is in microseconds ;		// about 33 milliseconds / 30fps
	animatedSprite->sprite->loop = FALSE;
    */
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
	//AF_Audio_Play(cannonSoundEntity->audioSource, 1.0f, FALSE);
	//wav64_play(&sfx_cannon, CHANNEL_SFX1);
}


void Game_OnGodTrigger(AF_Collision* _collision){
	AF_Entity* entity1 =  _collision->entity1;
	AF_Entity* entity2 =  _collision->entity2;
	uint32_t entity1ID = AF_ECS_GetID(entity1->id_tag);
	uint32_t entity2ID = AF_ECS_GetID(entity2->id_tag);
	PACKED_UINT32 entity1Tag = AF_ECS_GetTag(entity1->id_tag);
	PACKED_UINT32 entity2Tag = AF_ECS_GetTag(entity2->id_tag);
	debugf("Game_GodTrigger: eat count %i \n", godEatCount);

    godEatCount++;
	
	// Play sound
	//AF_Audio_Play(cannonSoundEntity->audioSource, 1.0f, FALSE);
	//wav64_play(&sfx_cannon, CHANNEL_SFX1);
}

void Game_OnBucketTrigger(AF_Collision* _collision){
	AF_Entity* entity1 =  _collision->entity1;
	AF_Entity* entity2 =  _collision->entity2;
	uint32_t entity1ID = AF_ECS_GetID(entity1->id_tag);
	uint32_t entity2ID = AF_ECS_GetID(entity2->id_tag);
	PACKED_UINT32 entity1Tag = AF_ECS_GetTag(entity1->id_tag);
	PACKED_UINT32 entity2Tag = AF_ECS_GetTag(entity2->id_tag);
	debugf("Game_OnBucketTrigger:  \n");

}

void UpdateText(AF_ECS* _ecs){
    // God eat count
    sprintf(godsCountLabelText, "%i", godEatCount);
    godEatCountLabelEntity->text->text = godsCountLabelText;

    // countdown timer
    sprintf(countdownTimerLabelText, "TIME %i", (int)countdownTimer);
    countdownTimerLabelEntity->text->text = countdownTimerLabelText;
}


void Game_Shutdown(void){
	debugf("Game_Shutdown");

}