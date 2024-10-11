#include "App.h"
#include <stdio.h>
#include <libdragon.h>
#include "Renderer.h"
#include "Game.h"
#include "Input.h"
#include "CTransform.h"
AppData appData;
CTransform cubeTransform;

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


    // Init Input
    Input_Init();

    // Init Rendering
    Renderer_Init(); 
    Game_Awake();
    Game_Start();

    // Create a default initialised transform
    CTransform defaultCube = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};

    // Copy over the data
    cubeTransform = defaultCube;
}


void App_Update(void){
    //debugf("App_Update\n");
    //print to the screen
    // TODO: get input to retrun a struct of buttons pressed/held
    Input_Update(&cubeTransform);

    // TODO: pass input and ECS structs to the game to apply game logic
    Game_Update(&cubeTransform);

    // TODO: Pass ECS entities to renderer to render them
    Renderer_Update(&cubeTransform);
}

void App_Shutdown(void){
        debugf("App_Shutdown\n");
	Game_Shutdown();
	Renderer_Shutdown();
	Input_Shutdown();	
}
