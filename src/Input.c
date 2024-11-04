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
        // Player 1
		joypad_buttons_t pressed = joypad_get_buttons_pressed(JOYPAD_PORT_1);
        joypad_buttons_t held = joypad_get_buttons_held(JOYPAD_PORT_1);
        joypad_inputs_t inputs = joypad_get_inputs(JOYPAD_PORT_1);

		// Player 2
		joypad_buttons_t pressed2 = joypad_get_buttons_pressed(JOYPAD_PORT_2);
        joypad_buttons_t held2 = joypad_get_buttons_held(JOYPAD_PORT_2);
        joypad_inputs_t inputs2 = joypad_get_inputs(JOYPAD_PORT_2);

		// Player 3
		joypad_buttons_t pressed3 = joypad_get_buttons_pressed(JOYPAD_PORT_3);
        joypad_buttons_t held3 = joypad_get_buttons_held(JOYPAD_PORT_3);
        joypad_inputs_t inputs3 = joypad_get_inputs(JOYPAD_PORT_3);

		// Player 4
		joypad_buttons_t pressed4 = joypad_get_buttons_pressed(JOYPAD_PORT_4);
        joypad_buttons_t held4 = joypad_get_buttons_held(JOYPAD_PORT_4);
        joypad_inputs_t inputs4 = joypad_get_inputs(JOYPAD_PORT_4);


		// flush the keys
		for(int i = 0; i < AF_INPUT_KEYS_MAPPED; ++i){
			_input->keys[i].pressed = FALSE;
		}
	// Held
        if (held.a) {
		//debugf("Input A Button Held\n");
		_input->keys[0].pressed = TRUE;
            //animation++;
        }

        if (held.b) {
        
		//debugf("Input B Button Held\n");
		_input->keys[1].pressed = TRUE;
	}
	
	if (held.r) {
		//debugf("Input R Button Held\n");
            //animation++;
        }

        if (held.z) {
        
		//debugf("Input z Button Held\n");
	}

	// Pressed
	if (pressed.a) {
		//debugf("Input A Button Pressed\n");
        }

        if (pressed.b) {
        
		//debugf("Input B Button Pressed\n");
	}


        if (pressed.start) {
		//debugf("Input Start Button pressed\n");
        }


        if (pressed.r) {
		//debugf("Input R Button pressed\n");
        }

		// Player 1
        float y = inputs.stick_y / 128.0f;
        float x = inputs.stick_x / 128.0f;

		// Player 2
		float x2 = inputs2.stick_x / 128.0f;
		float y2 = inputs2.stick_y / 128.0f;

		// Player 2
		float x3 = inputs3.stick_x / 128.0f;
		float y3 = inputs3.stick_y / 128.0f;

		// Player 2
		float x4 = inputs4.stick_x / 128.0f;
		float y4 = inputs4.stick_y / 128.0f;

		//player 1
		_input->stick_x = x;
		_input->stick_y = y;	

		// Player 2
		_input->stick_x2 = x2;
		_input->stick_y2 = y2;

		// Player 3
		_input->stick_x3 = x3;
		_input->stick_y3 = y3;

		// Player 4
		_input->stick_x4 = x4;
		_input->stick_y4 = y4;


}

void AF_Input_Shutdown(void){
	debugf("Input Shutdown\n");
}
