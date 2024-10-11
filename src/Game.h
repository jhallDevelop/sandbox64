/*================
Game is used to run all gameplay logic e.g. take in inputs and then process them for specific game functionality.
Game also will take in ECS Entities and then apply gameplay to those.
*/

#ifndef GAME_H
#define GAME_H

void Game_Awake(void);
void Game_Start(void);
void Game_Update(void);
void Game_Shutdown(void);

#endif
