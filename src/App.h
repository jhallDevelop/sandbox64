/*================
App to kick of the rest of the game and game functions
App will handle communication between the main entry point, update, input, rendering, sound, physics etc.
==================*/
#ifndef APP_H
#define APP_H
#include <stdint.h>
typedef struct {
    uint16_t windowWidth;
    uint16_t windowHeight;
    
}AppData;
void App_Init(uint16_t _windowWidth,uint16_t _windowHeight);
void App_Update(void);
void App_Shutdown(void);
#endif
