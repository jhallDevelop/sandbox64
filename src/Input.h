/*================
Input contains everything needed to collect input from the system and feed it back into the game
=================*/
#ifndef INPUT_H
#define INPUT_H
#include <libdragon.h>
#include "ECS/Entities/AF_ECS.h"
#define STICK_DEAD_ZONE 0.01
static inline void Input_Init(AF_ECS* _ecs){
	if(_ecs == NULL){
		debugf("Input: Input_Init pass in a null reference to _ecs\n");
		return;
	}
	debugf("Input Init\n");
	joypad_init();

}

static inline void Input_Update(AF_ECS* _ecs){
	if(_ecs == NULL){
		debugf("Input: Input_Update: passed in a null reference to _ecs\n");
		return;
	}
	joypad_poll();
        joypad_buttons_t pressed = joypad_get_buttons_pressed(JOYPAD_PORT_1);
        joypad_buttons_t held = joypad_get_buttons_held(JOYPAD_PORT_1);
        joypad_inputs_t inputs = joypad_get_inputs(JOYPAD_PORT_1);

	// Held
        if (held.a) {
		debugf("Input A Button Held\n");
            //animation++;
        }

        if (held.b) {
        
		debugf("Input B Button Held\n");
	}
	
	if (held.r) {
		debugf("Input R Button Held\n");
            //animation++;
        }

        if (held.z) {
        
		debugf("Input z Button Held\n");
	}

	// Pressed
	if (pressed.a) {
		debugf("Input A Button Pressed\n");
        }

        if (pressed.b) {
        
		debugf("Input B Button Pressed\n");
	}


        if (pressed.start) {
		debugf("Input Start Button pressed\n");
        }


        if (pressed.r) {
		debugf("Input R Button pressed\n");
        }


        float y = inputs.stick_y / 128.f;
        float x = inputs.stick_x / 128.f;
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

	// update the transform
	//float updateX = _transform->position[0];
	//float updateY = _transform->position[2];

	// speed
	//float speed = 0.1;
	// apply velocity
	//updateX += speed * vecX;
	//updateY += speed * vecY;

	//update the transform
	//_transform->position[0] = updateX;
	//_transform->position[2] = updateY;
}

static inline void Input_Shutdown(void){
	debugf("Input Shutdown\n");
}
#endif
