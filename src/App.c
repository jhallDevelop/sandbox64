#include "App.h"
#include <stdio.h>
#include <libdragon.h>
#include "AF_Renderer.h"
#include "Game.h"
#include "AF_Input.h"
#include "AF_Physics.h"

AppData appData;
AF_ECS ecs;
AF_Input input;


#define FRAME_BUFFER_SIZE 320*240*2
display_context_t disp;
char buffer[FRAME_BUFFER_SIZE];// = {0};

void App_Init(const uint16_t _windowWidth, const uint16_t _windowHeight){
    debugf("App_Init\n");

    //Initialise the app data structure
    appData.windowWidth = _windowWidth;
    appData.windowHeight = _windowHeight;

    // Basic debug setup
    debug_init_isviewer(); // this enables the ISViewer debug channel
    console_init();
    debug_init_usblog();
    console_set_debug(true);

    // Init the ECS system
    AF_ECS_Init(&ecs);

    // Init Input
    AF_Input_Init();
    AF_Physics_Init(&ecs);
    // Init Rendering
    
    AF_Renderer_Init(&ecs); 
    
    Game_Awake(&ecs);
    Game_Start(&ecs);
}


void App_Update(void){
    //debugf("App_Update\n");
    //print to the screen
    // TODO: get input to retrun a struct of buttons pressed/held
    AF_Input_Update(&input);

    // TODO: pass input and ECS structs to the game to apply game logic
    Game_Update(&input, &ecs);

    // Physics
    AF_Physics_Update(&ecs);

    // TODO: Pass ECS entities to renderer to render them
    AF_Renderer_Update(&ecs);
    //AF_Renderer_Debug();
    Game_LateUpdate(&ecs);
    AF_Renderer_Finish(); 
}

void App_Shutdown(void){
	debugf("App_Shutdown\n");
       	Game_Shutdown();
	AF_Renderer_Shutdown();
	AF_Physics_Shutdown();
	AF_Input_Shutdown();	
	//AF_ECS_Shutdown();

}

