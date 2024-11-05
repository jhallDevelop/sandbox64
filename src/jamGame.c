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
#include "AF_Renderer.h"

//We need to show lots of internal details of the module which are not
// exposed via public API, so include the internal header file.
//#include "./libdragon/src/audio/libxm/xm_internal.h"
//#include "libdragon/src/audio/audio/libxm/xm_internal.h" //"libdragon/src/audio/libxm/xm_internal.h"
//#include "xm_internal.h"

// Sounds
#define COUNT_DOWN_TIME 120
#define GODS_EAT_COUNT 15

// ECS system
AF_Entity* camera;
AF_Entity* player1Entity;
AF_Entity* player2Entity;
AF_Entity* player3Entity;
AF_Entity* player4Entity;

// God
AF_Entity* godEntity;
AF_Entity* godEye1;
AF_Entity* godEye2;
AF_Entity* godEye3;
AF_Entity* godEye4;
AF_Entity* godEyeInner1;
AF_Entity* godEyeInner2;
AF_Entity* godMouth;

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

// Player Counters
AF_Entity* playersCountUIEntity;

// player counter char buffers
char playerCountCharBuff[320] = "0                 0                  0                  0";

// Pickup
AF_Entity* bucket1;
AF_Entity* bucket2;
AF_Entity* bucket3;
AF_Entity* bucket4;

// villigers
AF_Entity* villager1;
AF_Entity* villager2;
AF_Entity* villager3;
AF_Entity* villager4;

// Main Menu
AF_Entity* mainMenuTitleEntity;
AF_Entity* mainMenuSubTitleEntity;
char mainMenuTitleCharBuffer[20] = "OLD GODS";
char mainMenuSubTitleCharBuffer[40] = "Press Start to begin";

// GameOverScreen
AF_Entity* gameOverTitleEntity;
AF_Entity* gameOverSubTitleEntity;
char gameOverTitleCharBuffer[20] = "Game Over";
char gameOverSubTitleLoseCharBuffer[80] = "You have failed to contain the the god\nPress Start to restart";
char gameOverSubTitleWinCharBuffer[80] = "You have succeded to contain the the god\nPress Start to restart";

// LoseScreen



// define some const values


// Textures
//const char* circleTexturePath = "rom:/circle0.sprite";


// Text / Font
const char* fontPath = "rom:/Electrolize-Regular.font64";//"rom:/Pacifico.font64";
const char* fontPath2 = "rom:/ZenDots-Regular.font64";//"rom:/FerriteCoreDX.font64";
//const char* titleFontPath = "rom:/ZenDots-Regular.font64";

// sprites
const char* animatedSpritePath = "";

// Sound
// data type to hold special n64 data
AF_Entity* laserSoundEntity;
AF_Entity* cannonSoundEntity;
AF_Entity* musicSoundEntity;
wav64_t sfx_cannon, sfx_laser, sfx_music;
// Mixer channel allocation
#define CHANNEL_SFX1    0
#define CHANNEL_SFX2    1
#define CHANNEL_MUSIC   2

const char* cannonFXPath = "rom:/cannon.wav64";
const char* laserFXPath = "rom:/laser.wav64";
const char* musicFXPath = "rom:/monosample8.wav64";

// Text
const char *titleText = "og64 0.1\n";
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
    GAME_STATE_MAIN_MENU = 0,
    GAME_STATE_PLAYING = 1,
    GAME_STATE_GAME_OVER_LOSE = 2,
    GAME_STATE_GAME_OVER_WIN = 3,
};

enum GAME_STATE gameState = GAME_STATE_PLAYING;

// SpriteSheet
//static sprite_t *sheet_knight;
const char* player1SpriteSheetPath = "rom:/knight.sprite";

int currentBucket = 0;

static xm64player_t xm;
static char *cur_rom = "rom:/Arcade_S900.xm64";

// forward decalred functions
void HandleInput(AF_Input* _input, AF_ECS* _ecs);
void Game_DrawPhysics(AF_ECS* _ecs);
void Game_SetupEntities(AF_ECS* _ecs);
AF_Entity* CreateCube(AF_ECS* _ecs);
AF_Entity* CreateAudioEntity(AF_ECS* _ecs, AF_AudioClip _audioClip, uint8_t _channel, void* _waveData, BOOL _isLooping);
AF_Entity* CreateSprite(AF_ECS* _ecs, const char* _spritePath, Vec2 _screenPos, Vec2 _size, uint8_t _color[4], char _animationFrames, Vec2 _spriteSheetSize, void* _spriteData);
AF_Entity* CreatePrimative(AF_ECS* _ecs, Vec3 _pos, Vec3 _bounds, enum AF_MESH_TYPE _meshType, enum CollisionVolumeType _collisionType, void* _collisionCallback);
AF_Entity* Game_UI_CreatePlayerCountLabel(AF_ECS* _ecs, char* _textBuff, int _fontID, const char* _fontPath, float _color[4], Vec2 _pos, Vec2 _size);
void Game_UpdatePlayerScoreText();
void Game_OnTrigger(AF_Collision* _collision);
void Game_OnGodTrigger(AF_Collision* _collision);
void Game_OnBucket1Trigger(AF_Collision* _collision);
void Game_OnBucket2Trigger(AF_Collision* _collision);
void Game_OnBucket3Trigger(AF_Collision* _collision);
void Game_OnBucket4Trigger(AF_Collision* _collision);
void Game_BucketCollisionBehaviour(int _currentBucket, int _bucketID, AF_Collision* _collision, AF_Entity* _villager, AF_Entity* _godEntity);
void RenderGameOverScreen(AF_Input* _input);
void UpdateText(AF_ECS* _ecs);
void RenderMainMenu(AF_Input* _input, AF_Time* _time);
void SpawnBucket();
void PlayMusic();

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
	mixer_ch_set_limits(audioSource->channel, 0, 128000, 0);//audioSource->clip.clipFrequency, 0);

	wav64_open((wav64_t*)audioSource->clipData, audioSource->clip.clipPath);
	wav64_set_loop((wav64_t*)audioSource->clipData, audioSource->loop);
	

	return returnEntity;
}

AF_Entity* Game_UI_CreatePlayerCountLabel(AF_ECS* _ecs, char* _textBuff, int _fontID, const char* _fontPath, float _color[4], Vec2 _pos, Vec2 _size){
   
    AF_Entity* entity = AF_ECS_CreateEntity(_ecs);
    *entity->text = AF_CText_ADD();
	entity->text->text = _textBuff;//godsCountLabelText;
	entity->text->fontID = 2;
	entity->text->fontPath = _fontPath;
    entity->text->fontID = _fontID;
    entity->text->screenPos = _pos;
	entity->text->textBounds = _size;
    
    return entity;
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

    gameState = GAME_STATE_MAIN_MENU;

    // choose the random spawn
    SpawnBucket();
    //PlayMusic();

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


    if(gameState == GAME_STATE_PLAYING){
        HandleInput(_input, _ecs);
    }
	

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
    



    // carry villages
    for(int i = 0; i < _ecs->entitiesCount; ++i){
        AF_Entity* entity = &_ecs->entities[i];
        AF_CPlayerData* playerData = entity->playerData;

        //if((AF_Component_GetHas(playerData->enabled) == TRUE) && (AF_Component_GetEnabled(playerData->enabled) == TRUE)){
        if(playerData->isCarrying == TRUE){
            // make villager match player transform
            Vec3 villagerCarryPos = {entity->transform->pos.x, entity->transform->pos.y+3, entity->transform->pos.z};
            villager1->transform->pos = villagerCarryPos;
            
            //debugf("entity carrying villager: x: %f y: %f x: %f \n", villagerCarryPos.x, villagerCarryPos.y, villagerCarryPos.z);
        }
        //}
     
    }
    UpdateText(_ecs);
    // this will decide how to render depending on game state
    RenderMainMenu(_input,_time);
    RenderGameOverScreen(_input);
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
	Vec3 godScale = {10,10,10};
    godEntity = CreatePrimative(_ecs, godPos, godScale, AF_MESH_TYPE_SPHERE, AABB, Game_OnGodTrigger);
	//godEntity->transform->pos = godPos;
	//godEntity->transform->scale = godScale;
	//godEntity->collider->boundingVolume = Vec3_MULT_SCALAR(godScale, 2);
    godEntity->mesh->material.textureID = 9;
	godEntity->collider->showDebug = TRUE;
	godEntity->rigidbody->inverseMass = 0;
	godEntity->rigidbody->isKinematic = TRUE;


    Vec3 godeye1Pos = {-2.5, 10, -12};
	Vec3 godeye1Scale = {1,1,1};
    godEye1 = CreateCube(_ecs);//CreatePrimative(_ecs, godeye1Pos, godeye1Scale, AF_MESH_TYPE_SPHERE, AABB, Game_OnTrigger);
    godEye1->transform->pos = godeye1Pos;
    godEye1->transform->scale = godeye1Scale;
    godEye1->rigidbody->inverseMass = 0;
    godEye1->mesh->material.textureID = 0;
    godEye1->mesh->isAnimating = TRUE;

    Vec3 godeye2Pos = {2.5, 10, -12};
	Vec3 godeye2Scale = {1,1,1};
    godEye2 = CreateCube(_ecs);//CreatePrimative(_ecs, godeye2Pos, godeye2Scale, AF_MESH_TYPE_SPHERE, AABB, Game_OnTrigger);
    godEye2->transform->pos = godeye2Pos;
    godEye2->transform->scale = godeye2Scale;
    godEye2->rigidbody->inverseMass = 0;
    godEye2->mesh->material.textureID = 1;
    godEye2->mesh->isAnimating = TRUE;

    Vec3 godeye3Pos = {2.5, 5, -12};
	Vec3 godeye3Scale = {1,1,1};
    godEye3 = CreateCube(_ecs);//CreatePrimative(_ecs, godeye2Pos, godeye2Scale, AF_MESH_TYPE_SPHERE, AABB, Game_OnTrigger);
    godEye3->transform->pos = godeye3Pos;
    godEye3->transform->scale = godeye3Scale;
    godEye3->rigidbody->inverseMass = 0;
    godEye3->mesh->material.textureID = 2;
    godEye3->mesh->isAnimating = TRUE;

    Vec3 godeye4Pos = {-2.5, 5, -12};
	Vec3 godeye4Scale = {1,1,1};
    godEye4 = CreateCube(_ecs);//CreatePrimative(_ecs, godeye2Pos, godeye2Scale, AF_MESH_TYPE_SPHERE, AABB, Game_OnTrigger);
    godEye4->transform->pos = godeye4Pos;
    godEye4->transform->scale = godeye4Scale;
    godEye4->rigidbody->inverseMass = 0;
    godEye4->mesh->material.textureID = 3;
    godEye4->mesh->isAnimating = TRUE;

    // God eye inner
    /*
    Vec3 godeyeInner1Pos = {-20.5, 10, 0};
	Vec3 godeyeInner1Scale = {3,3,3};
    godEyeInner1 =  CreateCube(_ecs);
    godEyeInner1->collider->boundingVolume = Vec3_MULT_SCALAR(godeyeInner1Scale, 2);
    godEyeInner1->transform->pos = godeyeInner1Pos;
	godEyeInner1->transform->scale = godeyeInner1Scale;
    
    godEyeInner1->rigidbody->inverseMass = 0;
    godEyeInner1->rigidbody->isKinematic = TRUE;
    godEyeInner1->mesh->material.textureID = 1;

    Vec3 godeyeInner2Pos = {2.5, 10, -15};
	Vec3 godeyeInner2Scale = {4,4,4};
    godEyeInner2 =  CreateCube(_ecs);
    godEyeInner2->transform->pos = godeyeInner2Pos;
	godEyeInner2->transform->scale = godeyeInner2Scale;
    godEyeInner2->rigidbody->inverseMass = 0;
    godEyeInner2->rigidbody->isKinematic = TRUE;
    godEyeInner2->mesh->material.textureID = 8;
   */

    //AF_Entity* godMouth;

    
	// ---------Create Player1------------------
	player1Entity = CreateCube(_ecs);
    player1Entity->mesh->material.textureID = 0;
	Vec3 player1Pos = {2.5, 1.5, -5};
	Vec3 player1Scale = {1,1,1};
	player1Entity->transform->pos = player1Pos;
	player1Entity->transform->scale = player1Scale;
	player1Entity->collider->boundingVolume = Vec3_MULT_SCALAR(player1Scale, 2);
	player1Entity->collider->showDebug = FALSE;
	player1Entity->rigidbody->inverseMass = 1;
	player1Entity->rigidbody->isKinematic = TRUE;
    *player1Entity->playerData = AF_CPlayerData_ADD();

    // Create sprites
     /*
	Vec2 spritePos = {10, 20};
	Vec2 spriteSize = {120, 80};
	uint8_t spriteColor[4] = {255, 0, 0, 255};
	Vec2 spriteSheetSize = {spriteSize.x *3,spriteSize.y * 1};
	//animatedSprite = CreateSprite(_ecs, sheet_knightPath, spritePos, spriteSize, spriteColor, 1, spriteSheetSize, (void*)sheet_knight);
   
    AF_CSprite* player1Sprite = player1Entity->sprite;
	*player1Sprite = AF_CSprite_ADD();
	player1Sprite->spritePath = player1SpriteSheetPath;//_spritePath;
	player1Sprite->pos = spritePos;
	player1Sprite->size = spriteSize;
	// TODO: fix this
	player1Sprite->spriteColor[0] = 255;
	player1Sprite->spriteColor[1] = 0;
	player1Sprite->spriteColor[2] = 0;
	player1Sprite->spriteColor[3] = 255;
    
	player1Sprite->animationFrames = 3;//_animationFrames;
	player1Sprite->spriteSheetSize = spriteSheetSize;

	
	//Load Sprite Sheet
    player1Sprite->spriteData = sprite_load(player1Sprite->spritePath);
	if(player1Sprite->spriteData == NULL){
		debugf("Game: CreateSprite: Failed to load sprite");
	}
	player1Sprite->animationFrames = 3;
	player1Sprite->animationSpeed = (1.0f) * 1000000; // Convert to microseconds if timer_ticks() is in microseconds ;		// about 33 milliseconds / 30fps
	player1Sprite->loop = FALSE;
    */



    // Create Player2
    //uint32_t player2TextureID = AF_LoadTexture("rom:/green.sprite");
	player2Entity = CreateCube(_ecs);
    //player2Entity->mesh->material.textureID = player2TextureID;
	Vec3 player2Pos = {-2.5, 1.5, -5};
	Vec3 player2Scale = {1,1,1};
    player2Entity->mesh->material.textureID = 1;
	player2Entity->transform->pos = player2Pos;
	player2Entity->transform->scale = player2Scale;
	player2Entity->collider->boundingVolume = Vec3_MULT_SCALAR(player2Scale, 2);
	player2Entity->collider->showDebug = FALSE;
    //player2Entity->mesh->material.textureID = 0;
	player2Entity->rigidbody->inverseMass = 1;
	player2Entity->rigidbody->isKinematic = TRUE;
    *player2Entity->playerData = AF_CPlayerData_ADD();

    // Create Player3
	player3Entity = CreateCube(_ecs);
    player3Entity->mesh->material.textureID = 2;
	Vec3 player3Pos = {-2.5, 1.5, 5};
	Vec3 player3Scale = {1,1,1};
	player3Entity->transform->pos = player3Pos;
	player3Entity->transform->scale = player3Scale;
	player3Entity->collider->boundingVolume = Vec3_MULT_SCALAR(player3Scale, 2);
	player3Entity->collider->showDebug = FALSE;
    //player3Entity->mesh->material.textureID = 0;
	player3Entity->rigidbody->inverseMass = 1;
	player3Entity->rigidbody->isKinematic = TRUE;
    *player3Entity->playerData = AF_CPlayerData_ADD();

    // Create Player4
	player4Entity = CreateCube(_ecs);
    player4Entity->mesh->material.textureID = 3;
	Vec3 player4Pos = {2.5, 1.5, 5};
	Vec3 player4Scale = {1,1,1};
	player4Entity->transform->pos = player4Pos;
	player4Entity->transform->scale = player4Scale;
	player4Entity->collider->boundingVolume = Vec3_MULT_SCALAR(player4Scale, 2);
	player4Entity->collider->showDebug = FALSE;
    //player4Entity->mesh->material.textureID = 0;
	player4Entity->rigidbody->inverseMass = 1;
	player4Entity->rigidbody->isKinematic = TRUE;
    *player4Entity->playerData = AF_CPlayerData_ADD();
	

	//=========ENVIRONMENT========
	// Create Plane
    //uint32_t groundPlaneTextureID = AF_LoadTexture("rom:/checker.sprite");
	groundPlaneEntity = CreateCube(_ecs);
    //groundPlaneEntity->mesh->material.textureID = groundPlaneTextureID;
	Vec3 planePos = {0, -2, 0};
	Vec3 planeScale = {40,1,40};
	groundPlaneEntity->transform->pos = planePos;
	groundPlaneEntity->transform->scale = planeScale;
	groundPlaneEntity->collider->boundingVolume = Vec3_MULT_SCALAR(planeScale, 2);
	groundPlaneEntity->collider->showDebug = FALSE;
    groundPlaneEntity->mesh->material.textureID = 7;
	groundPlaneEntity->rigidbody->inverseMass = 0;

    // Create Left Wall
    float wallHeight = 3;
	leftWall = CreateCube(_ecs);
	Vec3 leftWallPos = {-40, 0, 0};
	Vec3 leftWallScale = {1,wallHeight,40};
	leftWall->transform->pos = leftWallPos;
	leftWall->transform->scale = leftWallScale;
	leftWall->collider->boundingVolume = Vec3_MULT_SCALAR(leftWallScale, 2);
	leftWall->collider->showDebug = FALSE;
    leftWall->mesh->material.textureID = 8;
	leftWall->rigidbody->inverseMass = 0;

    // Create Right Wall
	rightWall = CreateCube(_ecs);
	Vec3 rightWallPos = {40, 0, 0};
	Vec3 rightWallScale = {1,wallHeight,40};
    rightWall->transform->pos = rightWallPos;
	rightWall->transform->scale = rightWallScale;
	rightWall->collider->boundingVolume = Vec3_MULT_SCALAR(rightWallScale, 2);
	rightWall->collider->showDebug = FALSE;
    rightWall->mesh->material.textureID = 8;
	rightWall->rigidbody->inverseMass = 0;

    // Create Back Wall
	backWall = CreateCube(_ecs);
	Vec3 backWallPos = {0, 0, -30};
	Vec3 backWallScale = {40,wallHeight,1};
    backWall->transform->pos = backWallPos;
	backWall->transform->scale = backWallScale;
	backWall->collider->boundingVolume = Vec3_MULT_SCALAR(backWallScale, 2);
	backWall->collider->showDebug = FALSE;
    rightWall->mesh->material.textureID = 8;
	backWall->rigidbody->inverseMass = 0;

    // Create Front Wall
	frontWall = CreateCube(_ecs);
	Vec3 frontWallPos = {0, 0, 20};
	Vec3 frontWallScale = {40,wallHeight,1};
    frontWall->transform->pos = frontWallPos;
	frontWall->transform->scale = frontWallScale;
	frontWall->collider->boundingVolume = Vec3_MULT_SCALAR(frontWallScale, 2);
	frontWall->collider->showDebug = FALSE;
    frontWall->mesh->material.textureID = 8;
	frontWall->rigidbody->inverseMass = 0;


    // Bucket 1
    bucket1 = CreateCube(_ecs);
	Vec3 bucket1Pos = {-20, 2, -15};
	Vec3 bucket1Scale = {1,1,1};
	bucket1->transform->pos = bucket1Pos;
	bucket1->transform->scale = bucket1Scale;
	bucket1->collider->boundingVolume = Vec3_MULT_SCALAR(bucket1Scale, 2);
	bucket1->collider->showDebug = TRUE;
    bucket1->mesh->material.textureID = 0;
	bucket1->rigidbody->inverseMass = 0;
	bucket1->rigidbody->isKinematic = TRUE;
    bucket1->collider->collision.callback = Game_OnBucket1Trigger;

    // Bucket 2
    bucket2 = CreateCube(_ecs);
	Vec3 bucket2Pos = {20, 2, -15};
	Vec3 bucket2Scale = {1,1,1};
	bucket2->transform->pos = bucket2Pos;
	bucket2->transform->scale = bucket2Scale;
	bucket2->collider->boundingVolume = Vec3_MULT_SCALAR(bucket2Scale, 2);
	bucket2->collider->showDebug = TRUE;
    bucket2->mesh->material.textureID = 1;
	bucket2->rigidbody->inverseMass = 0;
	bucket2->rigidbody->isKinematic = TRUE;
    bucket2->collider->collision.callback = Game_OnBucket2Trigger;


    // Bucket 3
    bucket3 = CreateCube(_ecs);
	Vec3 bucket3Pos = {-20, 2, 15};
	Vec3 bucket3Scale = {1,1,1};
	bucket3->transform->pos = bucket3Pos;
	bucket3->transform->scale = bucket3Scale;
	bucket3->collider->boundingVolume = Vec3_MULT_SCALAR(bucket3Scale, 2);
	bucket3->collider->showDebug = TRUE;
    bucket3->mesh->material.textureID = 2;
	bucket3->rigidbody->inverseMass = 0;
	bucket3->rigidbody->isKinematic = TRUE;
    bucket3->collider->collision.callback = Game_OnBucket3Trigger;

    // Bucket 4
    bucket4 = CreateCube(_ecs);
	Vec3 bucket4Pos = {20, 2, 15};
	Vec3 bucket4Scale = {1,1,1};
	bucket4->transform->pos = bucket4Pos;
	bucket4->transform->scale = bucket4Scale;
	bucket4->collider->boundingVolume = Vec3_MULT_SCALAR(bucket4Scale, 2);
	bucket4->collider->showDebug = TRUE;
    bucket4->mesh->material.textureID = 3;
	bucket4->rigidbody->inverseMass = 0;
	bucket4->rigidbody->isKinematic = TRUE;
    bucket4->collider->collision.callback = Game_OnBucket4Trigger;



    /// Villages
    
    villager1 = CreateCube(_ecs);
	Vec3 villager1Pos = {-1000, 0, 0};
	Vec3 villager1Scale = {1,1,1};
	villager1->transform->pos = villager1Pos;
	villager1->transform->scale = villager1Scale;
	villager1->collider->boundingVolume = Vec3_MULT_SCALAR(villager1Scale, 2);
	villager1->collider->showDebug = TRUE;
    villager1->mesh->material.textureID = 8;
	villager1->rigidbody->inverseMass = 0;
	villager1->rigidbody->isKinematic = TRUE;
    villager1->collider->collision.callback = Game_OnCollision;


	// Setup Audio
	// TODO: put into audio function
	audio_init(44100, 4);
	mixer_init(32);  // Initialize up to 16 channels

	AF_AudioClip musicAudioClip = {0, musicFXPath, 12800};
	laserSoundEntity = CreateAudioEntity(_ecs, musicAudioClip, CHANNEL_MUSIC, (void*)&sfx_music, TRUE);

	AF_AudioClip sfx1AudioClip = {0, laserFXPath, 128000};
	laserSoundEntity = CreateAudioEntity(_ecs, sfx1AudioClip, CHANNEL_SFX1, (void*)&sfx_laser, FALSE);

	AF_AudioClip sfx2AudioClip = {0, cannonFXPath, 128000};
	cannonSoundEntity = CreateAudioEntity(_ecs, sfx2AudioClip, CHANNEL_SFX2, (void*)&sfx_cannon, FALSE);

	

	bool music = false;
	//int music_frequency = sfx_music.wave.frequency;
	music = !music;
	if (music) {
		wav64_play(&sfx_music, CHANNEL_MUSIC);
		//music_frequency = sfx_music.wave.frequency;
	}

	
	// Create test Text
    int font1 = 1;
    int font2 = 2;
    float whiteColor[4];
    whiteColor[0] = 255;
    whiteColor[1] = 255;
    whiteColor[2] = 255;
    whiteColor[3] = 255;

    AF_LoadFont(font1, fontPath, whiteColor);
    AF_LoadFont(font2, fontPath2, whiteColor); // title font
	gameTitleEntity = AF_ECS_CreateEntity(_ecs);
	*gameTitleEntity->text = AF_CText_ADD();

	gameTitleEntity->text->text = titleText;
	gameTitleEntity->text->fontID = 1;
	gameTitleEntity->text->fontPath = fontPath2;

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
 // Create Player 1 card
    Vec2 playe1CountLabelPos = {20, 180};
    Vec2 playe1CountLabelSize = {320, 50};
    playersCountUIEntity = Game_UI_CreatePlayerCountLabel(_ecs, playerCountCharBuff, font2, fontPath2, whiteColor, playe1CountLabelPos, playe1CountLabelSize);


    // game over
    Vec2 gameOverTitlePos = {120, 100};
    Vec2 gameOverTitleSize = {320, 50};
    Vec2 gameOverSubTitlePos = {20, 140};
    Vec2 gameOverSubTitleSize = {320, 50};
    gameOverTitleEntity = Game_UI_CreatePlayerCountLabel(_ecs, gameOverTitleCharBuffer, font2, fontPath2, whiteColor, gameOverTitlePos, gameOverTitleSize);
    gameOverSubTitleEntity = Game_UI_CreatePlayerCountLabel(_ecs, gameOverSubTitleLoseCharBuffer, font2, fontPath2, whiteColor, gameOverSubTitlePos, gameOverSubTitleSize);
    // disable at the start
	gameOverTitleEntity->text->isShowing = FALSE;
    gameOverSubTitleEntity->text->isShowing = FALSE;

    // Create Main Menu
    Vec2 mainMenuTitlePos = {120, 100};
    Vec2 mainMenuTitleSize = {320, 50};
    Vec2 mainMenuSubTitlePos = {80, 140};
    Vec2 mainMenuSubTitleSize = {320, 50};
    mainMenuTitleEntity = Game_UI_CreatePlayerCountLabel(_ecs, mainMenuTitleCharBuffer, font2, fontPath2, whiteColor, mainMenuTitlePos, mainMenuTitleSize);

    mainMenuSubTitleEntity = Game_UI_CreatePlayerCountLabel(_ecs, mainMenuSubTitleCharBuffer, font2, fontPath2, whiteColor, mainMenuSubTitlePos, mainMenuSubTitleSize);
    // disable at the start
	mainMenuTitleEntity->text->isShowing = FALSE;
    mainMenuSubTitleEntity->text->isShowing = FALSE;
   /**/


}


void SpawnBucket(){
    int upper = 4;
    int lower = 0;
    int randomNum = (rand() % (upper + - lower) + lower);
    // don't let the random number go above 4, as we count from 0
    if(randomNum >=4){
        randomNum = 3;
    }
    debugf("Random number %i \n", randomNum);
    if(randomNum == 0){
        currentBucket = 0;
        godEntity->mesh->material.textureID = 0;
        bucket1->mesh->material.textureID = 0;
        bucket2->mesh->material.textureID = 5;
        bucket3->mesh->material.textureID = 5;
        bucket4->mesh->material.textureID = 5;
    }else if( randomNum == 1){
        currentBucket = 1;
        godEntity->mesh->material.textureID = 1;
        bucket1->mesh->material.textureID = 5;
        bucket2->mesh->material.textureID = 1;
        bucket3->mesh->material.textureID = 5;
        bucket4->mesh->material.textureID = 5;
    }else if( randomNum == 2){
        currentBucket = 2;
        godEntity->mesh->material.textureID = 2;
        bucket1->mesh->material.textureID = 5;
        bucket2->mesh->material.textureID = 5;
        bucket3->mesh->material.textureID = 2;
        bucket4->mesh->material.textureID = 5;
    }else if( randomNum == 3){
        currentBucket = 3;
        godEntity->mesh->material.textureID = 3;
        bucket1->mesh->material.textureID = 5;
        bucket2->mesh->material.textureID = 5;
        bucket3->mesh->material.textureID = 5;
        bucket4->mesh->material.textureID = 3;
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

/*
Game_OnTrigger
Default collision callback to be used by game entities
*/
void Game_OnTrigger(AF_Collision* _collision){
    
}

/*
Game_OnGodTrigger
Callback Behaviour triggered when the player dropps off a sacrafice to the gods
*/
void Game_OnGodTrigger(AF_Collision* _collision){
	//AF_Entity* entity1 =  _collision->entity1;
	AF_Entity* entity2 =  _collision->entity2;
	//uint32_t entity1ID = AF_ECS_GetID(entity1->id_tag);
	//uint32_t entity2ID = AF_ECS_GetID(entity2->id_tag);
	//PACKED_UINT32 entity1Tag = AF_ECS_GetTag(entity1->id_tag);
	//PACKED_UINT32 entity2Tag = AF_ECS_GetTag(entity2->id_tag);
	

    // if entity is carrying, eat and shift the villager into the distance
    if(entity2->playerData->isCarrying == TRUE){
        debugf("Game_GodTrigger: eat count %i \n", godEatCount);
        godEatCount++;
        
        entity2->playerData->score ++;
        
        Game_UpdatePlayerScoreText(entity2, playerCountCharBuff, &entity2->playerData->score);

        entity2->playerData->isCarrying = FALSE;
        entity2->playerData->carryingEntity = 0;
        Vec3 poolLocation = {100, 0,0};
        villager1->transform->pos = poolLocation;
        // randomly call for a colour bucket
        SpawnBucket();
        // play sound
		//AF_Audio_Play(cannonSoundEntity->audioSource, 0.5f, FALSE);

        // clear the players from carrying
        player1Entity->playerData->isCarrying = FALSE;
        player2Entity->playerData->isCarrying = FALSE;
        player3Entity->playerData->isCarrying = FALSE;
        player4Entity->playerData->isCarrying = FALSE;
    }
    
	
	// Play sound
	//AF_Audio_Play(cannonSoundEntity->audioSource, 1.0f, FALSE);
	//wav64_play(&sfx_cannon, CHANNEL_SFX1);
}

/*
Game_OnBucket1Trigger
Trigger callback assigned to buckets in the game world
*/
void Game_OnBucket1Trigger(AF_Collision* _collision){
    if(currentBucket != 0){
        return;
    }
	Game_BucketCollisionBehaviour(currentBucket, 0, _collision, villager1, godEntity);
}

/*
Game_OnBucket2Trigger
Trigger callback assigned to buckets in the game world
*/
void Game_OnBucket2Trigger(AF_Collision* _collision){
    if(currentBucket != 1){
        return;
    }
	Game_BucketCollisionBehaviour(currentBucket, 1, _collision, villager1, godEntity);
}

/*
Game_OnBucket3Trigger
Trigger callback assigned to buckets in the game world
*/
void Game_OnBucket3Trigger(AF_Collision* _collision){
    if(currentBucket != 2){
        return;
    }
	Game_BucketCollisionBehaviour(currentBucket, 2, _collision, villager1, godEntity);
}

/*
Game_OnBucket4Trigger
Trigger callback assigned to buckets in the game world
*/
void Game_OnBucket4Trigger(AF_Collision* _collision){
    if(currentBucket != 3){
        return;
    }
    Game_BucketCollisionBehaviour(currentBucket, 3, _collision, villager1, godEntity);
}

/*
Game_BucketCollisionBehaviour
Perform gameplay logic if bucket has been collided with by a player character
*/
void Game_BucketCollisionBehaviour(int _currentBucket, int _bucketID, AF_Collision* _collision, AF_Entity* _villager, AF_Entity* _godEntity){
    // Don't react if this bucket isn't activated
    if(_currentBucket != _bucketID){
        return;
    }
	//AF_Entity* entity1 =  _collision->entity1;
	AF_Entity* entity2 =  _collision->entity2;
	//uint32_t entity1ID = AF_ECS_GetID(entity1->id_tag);
	//uint32_t entity2ID = AF_ECS_GetID(entity2->id_tag);
	//PACKED_UINT32 entity1Tag = AF_ECS_GetTag(entity1->id_tag);
	//PACKED_UINT32 entity2Tag = AF_ECS_GetTag(entity2->id_tag);
	//debugf("Game_OnBucketTrigger:  \n");
    // attatch next villager
    //AF_CPlayerData* playerData1 = entity1->playerData;

    // Second collision is the playable character
    AF_CPlayerData* playerData2 = entity2->playerData;
    //if((AF_Component_GetHas(playerData1->enabled) == TRUE) && (AF_Component_GetEnabled(playerData1->enabled) == TRUE)){
        // attatch the villager to this player
        if(_villager->playerData->isCarried == FALSE){
            //debugf("OnBucketTrigger: carry villager \n");
            playerData2->carryingEntity = _villager->id_tag;
            _villager->mesh->material.textureID = _godEntity->mesh->material.textureID;
            playerData2->isCarrying = TRUE;
            // play sound
		    //AF_Audio_Play(laserSoundEntity->audioSource, 0.5f, FALSE);
        }
}



void UpdateText(AF_ECS* _ecs){
    // God eat count
    

    // countdown timer
    sprintf(countdownTimerLabelText, "TIME %i", (int)countdownTimer);
    countdownTimerLabelEntity->text->text = countdownTimerLabelText;
    // update the text
    countdownTimerLabelEntity->text->isDirty = TRUE;

    
    //playersCountUIEntity->text->text = playerCountCharBuff;
    // Update player counters
    // player 1
    
}

/*
Game_UpdatePlayerScoreText
Update the UI score elements
*/
void Game_UpdatePlayerScoreText(){
    sprintf(godsCountLabelText, "%i", godEatCount);
    godEatCountLabelEntity->text->text = godsCountLabelText;
    // our UI text rendering needs to be told an element is dirty so it will rebuild the text paragraph (for performance)
    godEatCountLabelEntity->text->isDirty = TRUE;
    sprintf(playerCountCharBuff, "%i                 %i                  %i                  %i", (int)player1Entity->playerData->score, (int)player2Entity->playerData->score, (int)player3Entity->playerData->score, (int)player4Entity->playerData->score);
    debugf("playerScore %s \n", playerCountCharBuff);
    
    playersCountUIEntity->text->text = playerCountCharBuff;
    playersCountUIEntity->text->isDirty = TRUE;
   
}

void RenderMainMenu(AF_Input* _input, AF_Time* _time){
    switch (gameState)
    {
    case GAME_STATE_MAIN_MENU:
        // Main Menu
        mainMenuTitleEntity->text->isShowing = TRUE;
        mainMenuSubTitleEntity->text->isShowing = TRUE;

        // Game Over
        gameOverTitleEntity->text->isShowing = FALSE;
        gameOverSubTitleEntity->text->isShowing = FALSE;
        gameOverTitleEntity->text->isShowing = FALSE;
        gameOverSubTitleEntity->text->isShowing = FALSE;

        // Player Counts hid
        playersCountUIEntity->text->isShowing = FALSE;

        // reset the visible text
        //Game_UpdatePlayerScoreText();
        // Header bar
        godEatCountLabelEntity->text->isShowing = FALSE;
        // gods count reset
        godEatCount = 0;

        countdownTimerLabelEntity->text->isShowing = FALSE;

        // countdown Time
        countdownTimer = COUNT_DOWN_TIME;

        // detect start button pressed
        if(_input->keys[2].pressed == TRUE){

            
            gameState = GAME_STATE_PLAYING;
            player1Entity->playerData->score = 0;
            player2Entity->playerData->score = 0;
            player3Entity->playerData->score = 0;
            player4Entity->playerData->score = 0;
            playersCountUIEntity->playerData->score = 0;
            Game_UpdatePlayerScoreText();
        }
        
        
    break;

    case GAME_STATE_PLAYING:
        // Update countdown timer
        countdownTimer -= _time->timeSinceLastFrame;
        if(countdownTimer <= 0){
            gameState = GAME_STATE_GAME_OVER_LOSE;
            countdownTimer = COUNT_DOWN_TIME;
        }

        if(godEatCount == GODS_EAT_COUNT){
            gameState = GAME_STATE_GAME_OVER_WIN;
            countdownTimer = COUNT_DOWN_TIME;
        }

    // Player Counts
        playersCountUIEntity->text->isShowing = TRUE;

        // MAin MEnu
        mainMenuTitleEntity->text->isShowing = FALSE;
        mainMenuSubTitleEntity->text->isShowing = FALSE;

        // Header bar
        godEatCountLabelEntity->text->isShowing = TRUE;
        countdownTimerLabelEntity->text->isShowing= TRUE;
    break;

    default:
        break;
    }
}
void RenderGameOverScreen(AF_Input* _input){
    switch (gameState)
    {
    case GAME_STATE_PLAYING:
        
    break;
    case GAME_STATE_GAME_OVER_WIN:
        /* code */
        // render game over win title
        // render game over sub title showing what player won
        gameOverSubTitleEntity->text->text = gameOverSubTitleWinCharBuffer;
        gameOverTitleEntity->text->isShowing = TRUE;
        gameOverSubTitleEntity->text->isShowing = TRUE;

        // detect start button pressed
        if(_input->keys[2].pressed == TRUE){
            gameState = GAME_STATE_MAIN_MENU;
        }
        break;

    case GAME_STATE_GAME_OVER_LOSE:
        /* code */
        // render Game over lose title
        // Render Game Over lose sub title
        gameOverSubTitleEntity->text->text = gameOverSubTitleLoseCharBuffer;
        gameOverTitleEntity->text->isShowing = TRUE;
        gameOverSubTitleEntity->text->isShowing = TRUE;

        // detect start button pressed
        if(_input->keys[2].pressed == TRUE){
            gameState = GAME_STATE_MAIN_MENU;
        }
        break;
    
    default:
        break;
    }

    // if player presses start button. Restart the game
}

void PlayMusic(){
    //xm64player_set_vol (&xm, 1.0f);
    xm64player_open(&xm, cur_rom);
	xm64player_play(&xm, 2);
}

void Game_Shutdown(void){
	debugf("Game_Shutdown");

}





