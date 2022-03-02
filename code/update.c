#include "update.h"
#include "zengine.h"


/*-------------------------------------------------*/
i32 UpdateInit    (u32 t, r32 dt, void* engine)
{
    // zEngine* z = (zEngine*)engine;

    return GAMESTATE_MAIN;
}

/*-------------------------------------------------*/
i32 UpdateMain    (u32 t, r32 dt, void* engine)
{
    zEngine* z = (zEngine*)engine;

    static u8 cam_active = 0;
    static u8 cross_active = 0;
    i2 mloc = MouseLocation(z->controller, z->viewport);
    r2 mpos = CamToPos(mloc, z->viewport);


    i32 menu_action = TickMenu(z->menus[MENU_TITLE], mloc, z->controller, z->input);

    if (menu_action >= 0)
    {
        switch (menu_action)
        {
            case BTN_PLAY:
            {
                //TODO set active level to appropriate value, copy struct and enter play
                z->game->current_level_number = 2;
                CloseLevel(z->game);
                OpenLevel(z->game, z->game->current_level_number);

                printf("testing %d\n", z->game->level_active->cell[0].sprite_mg);
                return GAMESTATE_PLAY;
                break;
            }
	        case BTN_OPTS:
                return GAMESTATE_OPTS;
            break;
	        case BTN_QUIT:
                return GAMESTATE_EXIT;
            break;
        }
    }


    if (ActionPressed(z->controller, A_WHLU))
    {
        if (z->viewport->camera->zoom < ZSDL_CAMERA_MAX_ZOOM)
            z->viewport->camera->zoom *= 1.1f;
    }
        
    if (ActionPressed(z->controller, A_WHLD))
    {
        if (z->viewport->camera->zoom > ZSDL_CAMERA_MIN_ZOOM)
            z->viewport->camera->zoom *= 0.9f;
    } 
    
    static r2 grab_cam_loc = ZERO_R2;
    if (ActionPressed(z->controller, A_MB_L))
    {
        if (cam_active)
        {
            SetCursor(z->viewport, z->assets, CUR_GRAB);
            grab_cam_loc = CamToPos(MouseLocation(z->controller, z->viewport), z->viewport);
        }
        else
        {
            cross_active = 1;
            SetCursor(z->viewport, z->assets, CUR_CLICK);
            SpawnBubble(z->particles, 16, mpos, ZERO_R2, ZERO_R2, 1.f, 0.f, 4, COLOR_RED, (SDL_Color){0xff, 0xff, 0x00, 0x00});
        }
        
    }

    if (ActionHeld(z->controller, A_MB_L))
    {
        if (cam_active)
        {
            r2 delta = sub_r2(mpos, grab_cam_loc);
            z->viewport->camera->pos = sub_r2(z->viewport->camera->pos, delta);
            grab_cam_loc = CamToPos(MouseLocation(z->controller, z->viewport), z->viewport);
        }
    }

    if (ActionReleased(z->controller, A_MB_L))
    {
        if (cam_active)
        {
            SetCursor(z->viewport, z->assets, CUR_HAND);
        }
        if (cross_active)
        {
            cross_active = 0;
            SetCursor(z->viewport, z->assets, CUR_POINT);
        }
    }

    if (ActionPressed(z->controller, A_JUMP))
    {
        cam_active = 1;
        SetCursor(z->viewport, z->assets, CUR_HAND);
    }
    if (ActionReleased(z->controller, A_JUMP))
    {
        cam_active = 0;
        SetCursor(z->viewport, z->assets, CUR_POINT);
    }


    return GAMESTATE_MAIN;

}

/*-------------------------------------------------*/
i32 UpdateOptions (u32 t, r32 dt, void* engine)
{
    zEngine* z = (zEngine*)engine;
    i2 mloc = MouseLocation(z->controller, z->viewport);
    r2 mpos = CamToPos(mloc, z->viewport);


    //}


    static u32 submenu_active = BTN_OPTS_VIDEO;

    i32 menu_action = TickMenu(z->menus[MENU_OPTIONS], mloc, z->controller, z->input);

    if (menu_action >= 0)
    {
        ToggleMenu(&z->menus[MENU_OPTIONS_AUDIO], ZDISABLED);
        ToggleMenu(&z->menus[MENU_OPTIONS_VIDEO], ZDISABLED);
        ToggleMenu(&z->menus[MENU_OPTIONS_INPUT], ZDISABLED);
        switch (menu_action)
        {
            case BTN_OPTS_VIDEO:
                submenu_active = BTN_OPTS_VIDEO;
                ToggleMenu(&z->menus[MENU_OPTIONS_VIDEO], ZENABLED);
            break;
	        case BTN_OPTS_AUDIO:
                submenu_active = BTN_OPTS_AUDIO;
                ToggleMenu(&z->menus[MENU_OPTIONS_AUDIO], ZENABLED);
            break;
	        case BTN_OPTS_INPUT:
                submenu_active = BTN_OPTS_INPUT;
                ToggleMenu(&z->menus[MENU_OPTIONS_INPUT], ZENABLED);
            break;
	        case BTN_OPTS_RETRN:
                return GAMESTATE_MAIN;
            break;            
        }
    }

    i32 submenu_action;
    switch (submenu_active)
    {
        case BTN_OPTS_VIDEO:
        {
            submenu_action = TickMenu(z->menus[MENU_OPTIONS_VIDEO], mloc, z->controller, z->input);
            switch (submenu_action)
            {
                case BTN_OPTS_VIDEO_FSCREEN:
                    ToggleFullscreen(z->viewport);
                    ComputePixelScale(z->viewport);
                    CalculateScreen(z->viewport);
                    RefreshCursors(z->viewport, z->assets);
                break;
            }

        }
        break;
        case BTN_OPTS_AUDIO:
        {
            submenu_action = TickMenu(z->menus[MENU_OPTIONS_AUDIO], mloc, z->controller, z->input);
            switch (submenu_action)
            {
                case BTN_OPTS_AUDIO_MAIN_VOLUME:
                break;
            }
        break;            
        }
        case BTN_OPTS_INPUT:
        {
            submenu_action = TickMenu(z->menus[MENU_OPTIONS_INPUT], mloc, z->controller, z->input);
            switch (submenu_action)
            {
                case BTN_OPTS_INPUT_REBIND:
                break;            
            }
        break;
        }
    }

    
    return GAMESTATE_OPTS;

}

/*-------------------------------------------------*/
i32 UpdatePlay    (u32 t, r32 dt, void* engine)
{
    zEngine* z = (zEngine*)engine;
    i2 mloc = MouseLocation(z->controller, z->viewport);
    r2 mpos = CamToPos(mloc, z->viewport);
    i2 mcel = PosToCel(mpos, z->game->level_active);

    //TODO add visual indication if you cannot place a unit somewhere, due to cell being conquered or already has a unit

    //only progress if board is not invalid, if invalid we need to undo or restart to progress
    if (z->game->board_invalid == 0)
    {
        if (ValidateCel(mcel, z->game->level_active))
        {
            if (ActionPressed(z->controller, A_MB_L))
            {
                if (z->game->move_active)
                {
                    if ((z->game->level_active->cell[CelToIdx(mcel, z->game->level_active)].type == UNIT_NONE) && 
                    (z->game->level_active->cell[CelToIdx(mcel, z->game->level_active)].collision == CELL_STATUS_EMPTY))
                    {
                        if (!i2_equals(z->game->current_move.cell_start, z->game->current_move.cell_end))
                        {
                            PerformMove(z->game, mcel);
                            SetCursor(z->viewport, z->assets, CUR_POINT);
                        }
                        else
                        {
                            CancelMove(z->game);
                            SetCursor(z->viewport, z->assets, CUR_POINT);    
                        }
                    }
                    else
                    {
                        CancelMove(z->game);
                        SetCursor(z->viewport, z->assets, CUR_POINT);
                    }
                }
                else
                {
                    if ((z->game->level_active->cell[CelToIdx(mcel, z->game->level_active)].type > UNIT_NONE) && 
                    (z->game->level_active->cell[CelToIdx(mcel, z->game->level_active)].collision == CELL_STATUS_HAS_UNIT))
                    {
                        StartMove(z->game, mcel);
                        SetCursor(z->viewport, z->assets, CUR_GRAB);
                    }
                }
            }
            if (!z->game->move_active)
            {
                if ((z->game->level_active->cell[CelToIdx(mcel, z->game->level_active)].type > UNIT_NONE) && 
                (z->game->level_active->cell[CelToIdx(mcel, z->game->level_active)].collision == CELL_STATUS_HAS_UNIT))
                {
                    SetCursor(z->viewport, z->assets, CUR_HAND);
                }
                else
                {
                    SetCursor(z->viewport, z->assets, CUR_POINT);
                }
            }
        }
        else
        {
            if (z->game->move_active)
            {
                if (ActionPressed(z->controller, A_MB_L))
                {
                    CancelMove(z->game);
                    SetCursor(z->viewport, z->assets, CUR_POINT);
                }
            }
            else
            {
                SetCursor(z->viewport, z->assets, CUR_POINT);
            }
        }
    }


    if (ActionPressed(z->controller, A_RSTRT))
    {
        RestartLevel(z->game);
    }

    if (ActionPressed(z->controller, A_UNDO))
    {
        UndoMove(z->game);
    }

    return GAMESTATE_PLAY;
}

/*-------------------------------------------------*/
i32 UpdateEvent   (u32 t, r32 dt, void* engine)
{
    // zEngine* z = (zEngine*)engine;

    return GAMESTATE_EXIT;
}

/*-------------------------------------------------*/
i32 UpdateLose    (u32 t, r32 dt, void* engine)
{
    // zEngine* z = (zEngine*)engine;

    return GAMESTATE_EXIT;
}

/*-------------------------------------------------*/
i32 UpdateGoal    (u32 t, r32 dt, void* engine)
{
    // zEngine* z = (zEngine*)engine;

    return GAMESTATE_EXIT;
}

/*-------------------------------------------------*/
i32 UpdateEdit    (u32 t, r32 dt, void* engine)
{
    // zEngine* z = (zEngine*)engine;

    return GAMESTATE_EXIT;
}

/*-------------------------------------------------*/
i32 UpdateExit    (u32 t, r32 dt, void* engine)
{
    // zEngine* z = (zEngine*)engine;

    return GAMESTATE_EXIT;
}



