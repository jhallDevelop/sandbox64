/*================
Implementation file for AF_Input
Specific to n64
Input contains everything needed to collect input from the system and feed it back into the game
=================*/
#include <libdragon.h>
#include "AF_Input.h"

void AF_Input_Init(){
	debugf("Input Init\n");
	joypad_init();
}

void AF_Input_Update(AF_Input* _input){
	if(_input == NULL){
		debugf("Input: Input_Update: passed in a null reference to input\n");
		return;
	}
	joypad_poll();
        joypad_buttons_t pressed = joypad_get_buttons_pressed(JOYPAD_PORT_1);
        joypad_buttons_t held = joypad_get_buttons_held(JOYPAD_PORT_1);
        joypad_inputs_t inputs = joypad_get_inputs(JOYPAD_PORT_1);

		// flush the keys
		for(int i = 0; i < AF_INPUT_KEYS_MAPPED; ++i){
			_input->keys[i].pressed = FALSE;
		}
	// Held
        if (held.a) {
		debugf("Input A Button Held\n");
		_input->keys[0].pressed = TRUE;
            //animation++;
        }

        if (held.b) {
        
		debugf("Input B Button Held\n");
		_input->keys[1].pressed = TRUE;
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
	_input->stick_x = x;
	_input->stick_y = y;	
}

void AF_Input_Shutdown(void){
	debugf("Input Shutdown\n");
}
