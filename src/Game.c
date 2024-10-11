#include "Game.h"
#include <libdragon.h>


void Game_Awake(void){
	debugf("Game Awake\n");
}
void Game_Start(void){
	debugf("Game_Start\n");
}
void Game_Update(const CTransform* _transform){
	// game update
}

void Game_Shutdown(void){
	debugf("Game_Shutdown");

}

