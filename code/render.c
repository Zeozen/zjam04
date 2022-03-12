#include <stdio.h>
#include "render.h"
#include "zengine.h"



/*-------------------------------------------*/
/*-------------------------------------------*/
/*-------------------------------------------*/
void RenderInit    (u32 t, r32 a, void* engine)
{
    // zEngine* z = (zEngine*)engine;
}

/*-------------------------------------------*/
/*-------------------------------------------*/
/*-------------------------------------------*/
void RenderMain    (u32 t, r32 a, void* engine)
{
    zEngine* z = (zEngine*)engine;

    SDL_SetRenderTarget(z->viewport->renderer, z->viewport->render_layer[ZSDL_RENDERLAYER_BACKGROUND]);
    SDL_RenderCopy(z->viewport->renderer, z->assets->tex[T_BG_ATLAS], NULL, NULL);

//origo grid lines
    // i2 origo_to_screen = PosToCam(ZERO_R2, 1.f, z->viewport);
    // SDL_SetRenderDrawColor(z->viewport->renderer, 0xcc, 0xaa, 0xaa, 0x33);
    // SDL_RenderDrawLine(z->viewport->renderer, origo_to_screen.x, 0, origo_to_screen.x, ZSDL_INTERNAL_HEIGHT);
    // SDL_RenderDrawLine(z->viewport->renderer, 0, origo_to_screen.y, ZSDL_INTERNAL_WIDTH, origo_to_screen.y);
    
    SDL_SetRenderTarget(z->viewport->renderer, z->viewport->render_layer[ZSDL_RENDERLAYER_ENTITIES]);

    SDL_SetRenderTarget(z->viewport->renderer, z->viewport->render_layer[ZSDL_RENDERLAYER_FOREGROUND]);

    SDL_SetRenderTarget(z->viewport->renderer, z->viewport->render_layer[ZSDL_RENDERLAYER_UI]);
    DrawMenu(z->menus[MENU_TITLE], z->viewport, z->assets);

    SDL_Rect tit_src = {0, 0, 224, 112};
    SDL_Rect tit_dst = {ZSDL_INTERNAL_HALFWIDTH - tit_src.w * 0.5, 16, tit_src.w, tit_src.h};
    SDL_RenderCopy(z->viewport->renderer, z->assets->tex[T_UI_ATLAS], &tit_src, &tit_dst);

// //draw player cursors
//     for (i32 i = 0; i < MAX_PLAYERS; i++)
//     {
//         if (z->input->pcon[i]->active)
//         {
//             SDL_Rect src = {0, 0, 8, 8};
//             SDL_Rect dst = {z->input->pcon[i]->cursor_loc.x, z->input->pcon[i]->cursor_loc.y, 8, 8};
//             SDL_RenderCopy(z->viewport->renderer, z->assets->tex[T_PLAYER_CURSOR], &src, &dst);
//             char nav_val[50];
//             sprintf(nav_val, "nav: (%f, %f)", z->input->pcon[i]->nav.x, z->input->pcon[i]->nav.y);
//             DrawTextScreen(z->viewport, z->assets->fon[FONT_ID_ZSYS], COLOR_WHITE, ZERO_I2, nav_val);
//         }
//     }


}

/*-------------------------------------------*/
/*-------------------------------------------*/
/*-------------------------------------------*/
void RenderOptions (u32 t, r32 a, void* engine)
{
    zEngine* z = (zEngine*)engine;

        SDL_SetRenderTarget(z->viewport->renderer, z->viewport->render_layer[ZSDL_RENDERLAYER_BACKGROUND]);
    SDL_RenderCopy(z->viewport->renderer, z->assets->tex[T_BG_ATLAS], NULL, NULL);

    SDL_SetRenderTarget(z->viewport->renderer, z->viewport->render_layer[ZSDL_RENDERLAYER_UI]);
    DrawMenu(z->menus[MENU_OPTIONS], z->viewport, z->assets);
    DrawMenu(z->menus[MENU_OPTIONS_VIDEO], z->viewport, z->assets);
    DrawMenu(z->menus[MENU_OPTIONS_AUDIO], z->viewport, z->assets);
    DrawMenu(z->menus[MENU_OPTIONS_INPUT], z->viewport, z->assets);
}

/*-------------------------------------------*/
/*-------------------------------------------*/
/*-------------------------------------------*/
void RenderPlay    (u32 t, r32 a, void* engine)
{
    zEngine* z = (zEngine*)engine;

    SDL_SetRenderTarget(z->viewport->renderer, z->viewport->render_layer[ZSDL_RENDERLAYER_BACKGROUND]);
    SDL_RenderCopy(z->viewport->renderer, z->assets->tex[T_BG_ATLAS], NULL, NULL);    

    DrawGrid(z->game->level_active, z->viewport, z->assets, 0);

    if (z->game->move_active)
    {
        SDL_Rect src = CelSpriteSource(CelToIdx(z->game->current_move.cell_start, z->game->level_active), z->game->level_active, SPRITELAYER_MG);
        src.y = src.y - (6 * 32);
        i2 mloc = MouseLocation(z->controller, z->viewport);
        SDL_Rect dst = {mloc.x - 16, mloc.y - 16, 32, 32};
        SDL_RenderCopy(z->viewport->renderer, z->assets->tex[T_TILE_ATLAS], &src, &dst);
    }

    SDL_SetRenderTarget(z->viewport->renderer, z->viewport->render_layer[ZSDL_RENDERLAYER_UI]);
    if (!z->game->menu_top_active)
    {
        SDL_Rect menu_top_tab = {ZSDL_INTERNAL_HALFWIDTH - 20, 0, 40, 10};
        SDL_SetRenderDrawColor(z->viewport->renderer, 156, 200, 198, 255);
        SDL_RenderFillRect(z->viewport->renderer, &menu_top_tab);
    }
    if (!z->game->menu_bot_active)
    {
        SDL_Rect menu_bot_tab = {ZSDL_INTERNAL_HALFWIDTH - 20, ZSDL_INTERNAL_HEIGHT - 10, 40, 10};
        SDL_SetRenderDrawColor(z->viewport->renderer, 156, 200, 198, 255);
        SDL_RenderFillRect(z->viewport->renderer, &menu_bot_tab);
    }
    DrawMenu(z->menus[MENU_CONTROL_TOP], z->viewport, z->assets);
    DrawMenu(z->menus[MENU_CONTROL_BOT], z->viewport, z->assets);

    SDL_Rect src_complete = {256, 48, 32, 32};
    SDL_Rect dst_complete = {ZSDL_INTERNAL_WIDTH - 38, ZSDL_INTERNAL_HEIGHT - 60, 32, 32};
    if (z->game->levels_cleared & LEVEL_NUMBER(z->game->current_level_number))
    {
        SDL_RenderCopy(z->viewport->renderer, z->assets->tex[T_UI_ATLAS], &src_complete, &dst_complete);
    }
}

/*-------------------------------------------*/
/*-------------------------------------------*/
/*-------------------------------------------*/
void RenderEvent   (u32 t, r32 a, void* engine)
{
    // zEngine* z = (zEngine*)engine;

    
}

/*-------------------------------------------*/
/*-------------------------------------------*/
/*-------------------------------------------*/
void RenderLose    (u32 t, r32 a, void* engine)
{
    zEngine* z = (zEngine*)engine;

    SDL_SetRenderTarget(z->viewport->renderer, z->viewport->render_layer[ZSDL_RENDERLAYER_BACKGROUND]);
    SDL_RenderCopy(z->viewport->renderer, z->assets->tex[T_BG_ATLAS], NULL, NULL);    

    DrawGrid(z->game->level_active, z->viewport, z->assets, 0);

    SDL_SetRenderTarget(z->viewport->renderer, z->viewport->render_layer[ZSDL_RENDERLAYER_UI]);
    //DrawTextScreen(z->viewport, z->assets->fon[FONT_ID_ZSYS], COLOR_BLACK, ZERO_I2, "BAD BOARD");
    if (!z->game->menu_top_active)
    {
        SDL_Rect menu_top_tab = {ZSDL_INTERNAL_HALFWIDTH - 20, 0, 40, 10};
        SDL_SetRenderDrawColor(z->viewport->renderer, 156, 200, 198, 255);
        SDL_RenderFillRect(z->viewport->renderer, &menu_top_tab);
    }
    if (!z->game->menu_bot_active)
    {
        SDL_Rect menu_bot_tab = {ZSDL_INTERNAL_HALFWIDTH - 20, ZSDL_INTERNAL_HEIGHT - 10, 40, 10};
        SDL_SetRenderDrawColor(z->viewport->renderer, 156, 200, 198, 255);
        SDL_RenderFillRect(z->viewport->renderer, &menu_bot_tab);
        SDL_Rect src_prompt = {240, 48, 16, 16};
        SDL_Rect dst_prompt = {ZSDL_INTERNAL_HALFWIDTH - 8, ZSDL_INTERNAL_HEIGHT - 26, 16, 16};
        SDL_RenderCopy(z->viewport->renderer, z->assets->tex[T_UI_ATLAS], &src_prompt, &dst_prompt);        
    }
    DrawMenu(z->menus[MENU_CONTROL_TOP], z->viewport, z->assets);
    DrawMenu(z->menus[MENU_CONTROL_BOT], z->viewport, z->assets);
    SDL_Rect src_complete = {256, 48, 32, 32};
    SDL_Rect dst_complete = {ZSDL_INTERNAL_WIDTH - 38, ZSDL_INTERNAL_HEIGHT - 60, 32, 32};
    if (z->game->levels_cleared & LEVEL_NUMBER(z->game->current_level_number))
    {
        SDL_RenderCopy(z->viewport->renderer, z->assets->tex[T_UI_ATLAS], &src_complete, &dst_complete);
    }
}

/*-------------------------------------------*/
/*-------------------------------------------*/
/*-------------------------------------------*/
void RenderGoal    (u32 t, r32 a, void* engine)
{
    zEngine* z = (zEngine*)engine;

    SDL_SetRenderTarget(z->viewport->renderer, z->viewport->render_layer[ZSDL_RENDERLAYER_BACKGROUND]);
    SDL_RenderCopy(z->viewport->renderer, z->assets->tex[T_BG_ATLAS], NULL, NULL);    

    DrawGrid(z->game->level_active, z->viewport, z->assets, 0);

    SDL_SetRenderTarget(z->viewport->renderer, z->viewport->render_layer[ZSDL_RENDERLAYER_UI]);
    //DrawTextScreen(z->viewport, z->assets->fon[FONT_ID_ZSYS], COLOR_BLACK, ZERO_I2, "GOOD BOARD!");
    if (!z->game->menu_top_active)
    {
        SDL_Rect menu_top_tab = {ZSDL_INTERNAL_HALFWIDTH - 20, 0, 40, 10};
        SDL_SetRenderDrawColor(z->viewport->renderer, 156, 200, 198, 255);
        SDL_RenderFillRect(z->viewport->renderer, &menu_top_tab);
        SDL_Rect src_prompt = {224, 48, 16, 16};
        SDL_Rect dst_prompt = {ZSDL_INTERNAL_HALFWIDTH - 8, 10, 16, 16};
        SDL_RenderCopy(z->viewport->renderer, z->assets->tex[T_UI_ATLAS], &src_prompt, &dst_prompt);
    }
    if (!z->game->menu_bot_active)
    {
        SDL_Rect menu_bot_tab = {ZSDL_INTERNAL_HALFWIDTH - 20, ZSDL_INTERNAL_HEIGHT - 10, 40, 10};
        SDL_SetRenderDrawColor(z->viewport->renderer, 156, 200, 198, 255);
        SDL_RenderFillRect(z->viewport->renderer, &menu_bot_tab);
    }
    DrawMenu(z->menus[MENU_CONTROL_TOP], z->viewport, z->assets);
    DrawMenu(z->menus[MENU_CONTROL_BOT], z->viewport, z->assets);
    SDL_Rect src_complete = {256, 48, 32, 32};
    SDL_Rect dst_complete = {ZSDL_INTERNAL_WIDTH - 38, ZSDL_INTERNAL_HEIGHT - 60, 32, 32};
    if (z->game->levels_cleared & LEVEL_NUMBER(z->game->current_level_number))
    {
        SDL_RenderCopy(z->viewport->renderer, z->assets->tex[T_UI_ATLAS], &src_complete, &dst_complete);
    }    

}

/*-------------------------------------------*/
/*-------------------------------------------*/
/*-------------------------------------------*/
void RenderEdit    (u32 t, r32 a, void* engine)
{
    // zEngine* z = (zEngine*)engine;
}

/*-------------------------------------------*/
/*-------------------------------------------*/
/*-------------------------------------------*/
void RenderExit    (u32 t, r32 a, void* engine)
{
    // zEngine* z = (zEngine*)engine;
}

