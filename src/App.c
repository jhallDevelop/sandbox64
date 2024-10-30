#include "App.h"
#include <stdio.h>
#include <libdragon.h>
#include "AF_Renderer.h"
#include "Game.h"
#include "AF_Input.h"
#include "AF_Physics.h"
// UFNLoader for n64
#include "debug.h"

AppData appData;
AF_ECS ecs;
AF_Input input;

#define FRAME_BUFFER_SIZE 320*240*2
display_context_t disp;
char buffer[FRAME_BUFFER_SIZE];// = {0};
const BOOL isDebug = TRUE;
float deltaTime;
int32_t curTick = 0;
int32_t curFrame = 0;
uint64_t lastTime =0;
float deltaTime = 0.0f;
uint64_t currentTime = 0;

// forward declare
void App_Update_Wrapper(int _ovfl);

void App_Init(const uint16_t _windowWidth, const uint16_t _windowHeight){
    debug_initialize();
    debug_printf("USB UFNLoader Enabled!\n");

    debugf("App_Init\n");

    //Initialise the app data structure
    appData.windowWidth = _windowWidth;
    appData.windowHeight = _windowHeight;

    // Basic  setup for libdragon
    debug_init_isviewer(); // this enables the ISViewer debug channel
    console_init();
    debug_init_usblog();
    console_set_debug(true);
    timer_init();
    
    

    // Init the ECS system
    AF_ECS_Init(&ecs);

    // Init Input
    AF_Input_Init();
    AF_Physics_Init(&ecs);
    // Init Rendering
    
    AF_Renderer_Init(&ecs); 
    
    Game_Awake(&ecs);
    Game_Start(&ecs);

    // set framerate to target 60fp and call the app update function
    new_timer(TIMER_TICKS(1000000 / 60), TF_CONTINUOUS, App_Update_Wrapper);
}

void App_Update_Wrapper(int _ovfl){
    App_Update(&input, &ecs);
}


void App_Update(AF_Input* _input, AF_ECS* _ecs){
    currentTime = timer_ticks();
    deltaTime = (float)(currentTime - lastTime)/ TICKS_PER_SECOND;
    lastTime = currentTime;
    //debugf("App_Update\n");
    //print to the screen
    // TODO: get input to retrun a struct of buttons pressed/held
    AF_Input_Update(_input);

    //AF_Physics_EarlyUpdate(&ecs);

    // TODO: pass input and ECS structs to the game to apply game logic
    Game_Update(_input, _ecs);

    // Physics
    AF_Physics_Update(_ecs, deltaTime);

    // late update for physics
    AF_Physics_LateUpdate(_ecs);

    // TODO: Pass ECS entities to renderer to render them
    //AF_Renderer_Update(&ecs);
    //AF_Renderer_Debug();
    // render debug physics if enabled
    /*if(isDebug == TRUE){
        AF_Physics_LateRenderUpdate(&ecs);
    }*/
    

    Game_LateUpdate(_ecs);
    //AF_Renderer_Finish(); 
    // update the tick
    curTick++;
}

void App_Render_Update(){
    AF_Renderer_Update(&ecs);
    if(isDebug == TRUE){
        //AF_Physics_LateRenderUpdate(&ecs);
    }
     AF_Renderer_Finish(); 
     curFrame++;
}


void App_Shutdown(void){
	debugf("App_Shutdown\n");
    Game_Shutdown();
	AF_Renderer_Shutdown();
	AF_Physics_Shutdown();
	AF_Input_Shutdown();	
	//AF_ECS_Shutdown();

}

