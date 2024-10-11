/*================
Input contains everything needed to collect input from the system and feed it back into the game
=================*/
#ifndef INPUT_H
#define INPUT_H
#include <libdragon.h>
#define STICK_DEAD_ZONE 0.01
static inline void Input_Init(void){
	debugf("Input Init\n");
	joypad_init();

}

static inline void Input_Update(void){

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
	if (y > STICK_DEAD_ZONE || y < -STICK_DEAD_ZONE){
		debugf("Stick y %f:\n",y);
	}

	if(x > STICK_DEAD_ZONE || x < -STICK_DEAD_ZONE ){
		debugf("Stick x %f:\n",x);
	}
}

static inline void Input_Shutdown(void){
	debugf("Input Shutdown\n");
}
#endif
