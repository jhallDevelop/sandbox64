#include "App.h"
#include <stdio.h>
#include <libdragon.h>
#include "Renderer.h"
#include "Game.h"
#include "Input.h"
AppData appData;
AF_ECS ecs;

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
    Input_Init(&ecs);

    // Init Rendering
    Renderer_Init(&ecs); 
    Game_Awake(&ecs);
    Game_Start(&ecs);

}


void App_Update(void){
    //debugf("App_Update\n");
    //print to the screen
    // TODO: get input to retrun a struct of buttons pressed/held
    Input_Update(&ecs);

    // TODO: pass input and ECS structs to the game to apply game logic
    Game_Update(&ecs);

    // TODO: Pass ECS entities to renderer to render them
    Renderer_Update(&ecs);
}

void App_Shutdown(void){
        debugf("App_Shutdown\n");
	Game_Shutdown();
	Renderer_Shutdown();
	Input_Shutdown();	
}
