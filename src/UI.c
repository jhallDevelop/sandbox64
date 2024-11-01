#include <libdragon.h>
#include "AF_UI.h"
#include "ECS/Components/AF_CText.h"
#include "AF_Vec2.h"
#include "AF_Util.h"


/*
====================
AF_UI_INIT
Init for UI components like text
====================
*/
void AF_UI_Init(AF_ECS* _ecs){

    if(_ecs == NULL){
		debugf("Renderer: Renderer_Init has null ecs referenced passed in \n");
		return;
	} 

	debugf("InitTextRendering\n");

    for(int i = 0; i < _ecs->entitiesCount; i++){
        // find components that are text components and initialise them
        // Register the font
        AF_CText* text = &_ecs->texts[i];

        int hasFlag = AF_Component_GetHas(text->enabled);
        int enabledFlag = AF_Component_GetEnabled(text->enabled);
        // skip components that don't have the text component
        if(AF_Component_GetHas(enabledFlag) == 0){
            continue;
        }

        // don't proceed if no path or null
        if(text->fontPath == NULL || AF_STRING_IS_EMPTY(text->text) == TRUE){
            continue;
        }
        debugf("AF_UI_INIT: loading font %i: %s enabled: %d has: %d \n",text->fontID, text->fontPath, hasFlag, enabledFlag);

        rdpq_font_t *fnt1 = rdpq_font_load(text->fontPath);
        rdpq_font_style(fnt1, 0, &(rdpq_fontstyle_t){
            .color = RGBA32(text->textColor[0],text->textColor[1], text->textColor[2], 0xFF),//text->textColor[0],text->textColor[1], text->textColor[2], text->textColor[3]), //0xED, 0xAE, 0x49, 0xFF),
        });
       
        rdpq_text_register_font(text->fontID, fnt1);
    }
    
}

/*
====================
AF_UI_RENDERER_UPDATE
Render all UI elements like text
====================
*/
void AF_UI_Renderer_Update(AF_CText* _text){
    
    // Find components that are text components
    // skip components that don't have the text component
    int hasFlag = AF_Component_GetHas(_text->enabled);
    int enabledFlag = AF_Component_GetEnabled(_text->enabled);
    // skip components that don't have the text component
    if(AF_Component_GetHas(enabledFlag) == 0){
        return;
    }

    // don't proceed if no path or null
    if(_text->fontPath == NULL || AF_STRING_IS_EMPTY(_text->text) == TRUE){
        return;
    }

    
    int nbytes = strlen(_text->text);
    rdpq_paragraph_t* par = rdpq_paragraph_build(&(rdpq_textparms_t){
        // .line_spacing = -3,
        .align = ALIGN_LEFT,
        .valign = VALIGN_CENTER,
        .width = _text->textBounds.x,
        .height = _text->textBounds.y,
        .wrap = WRAP_WORD,
    }, _text->fontID, _text->text, &nbytes);
    
    rdpq_paragraph_render(par, _text->screenPos.x, _text->screenPos.y);
    rdpq_paragraph_free(par);

}

/*
====================
AF_UI_RENDERER_FINISH
Final render pass
====================
*/
void AF_UI_Renderer_Finish(void){

}

/*
====================
AF_UI_RENDERER_SHUTDOWN
Do shutdown things
====================
*/
void AF_UI_Renderer_Shutdown(void){

}