/*================
EntityFactory
Used to create entities depending on the component needs/intentions.
Will find helper factory functions that create an AF_Enity and initialise key variables based on passed in params
================*/
#ifndef ENTITYFACTORY_H
#define ENTITYFACTORY_H

#include <libdragon.h>
#include "ECS/Entities/AF_ECS.h"

static inline AF_Entity* Entity_Factory_CreatePrimative(AF_ECS* _ecs, Vec3 _pos, Vec3 _scale, enum AF_MESH_TYPE _meshType, enum CollisionVolumeType _collisionType){
	AF_Entity* entity = AF_ECS_CreateEntity(_ecs);
	//move the position up a little
	entity->transform->pos = _pos;
    entity->transform->scale = _scale;
	// add a rigidbody to our cube
	*entity->rigidbody = AF_C3DRigidbody_ADD();
	*entity->collider = AF_CCollider_ADD_TYPE(_collisionType);//AF_CCollider_Box_ADD();
	*entity->mesh = AF_CMesh_ADD();
	entity->mesh->meshType = _meshType;
	entity->collider->boundingVolume = Vec3_MULT_SCALAR(_scale, 2);
	return entity;
}

static inline AF_Entity* Entity_Factory_CreateSprite(AF_ECS* _ecs, const char* _spritePath, Vec2 _screenPos, Vec2 _size, uint8_t _color[4], char _animationFrames, Vec2 _spriteSheetSize, void* _spriteData){
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


static inline AF_Entity* Entity_Factory_CreateAudio(AF_ECS* _ecs, AF_AudioClip _audioClip, uint8_t _channel, void* _wavData, BOOL _isLooping){
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

static inline AF_Entity* Entity_Factory_CreateUILabel(AF_ECS* _ecs, char* _textBuff, int _fontID, const char* _fontPath, float _color[4], Vec2 _pos, Vec2 _size){
   
    AF_Entity* entity = AF_ECS_CreateEntity(_ecs);
    *entity->text = AF_CText_ADD();
	entity->text->text = _textBuff;
	entity->text->fontID = 2;
	entity->text->fontPath = _fontPath;
    entity->text->fontID = _fontID;
    entity->text->screenPos = _pos;
	entity->text->textBounds = _size;
    
    return entity;
}


#endif