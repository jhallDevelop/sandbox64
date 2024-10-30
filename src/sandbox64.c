#include "App.h"

#define WINDOW_WIDTH 320
#define WINDOW_HEIGHT 240
int main(void){
    // Initialise the app
    App_Init(WINDOW_WIDTH, WINDOW_HEIGHT);

    // Main game loop
    while(1) {
        // render stuff as fast as possible, interdependent from other code
        App_Render_Update();
    }

    // Do Shutdown things
    App_Shutdown();

}
