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

    i2 mloc = MouseLocation(z->controller, z->viewport);
    r2 mpos = CamToPos(mloc, z->viewport);


    i32 menu_action = TickMenu(z->menus[MENU_TITLE], mloc, z->controller, z->input);

    if (menu_action >= 0)
    {
        switch (menu_action)
        {
            case BTN_PLAY:
            {
                z->game->current_level_number = 0;
                CloseActiveLevel(z->game);
                OpenLevel(z->game, z->game->current_level_number, z->menus, z->assets);
                SetCursor(z->viewport, z->assets, CUR_POINT);
                return GAMESTATE_PLAY;
                break;
            }
	        case BTN_OPTS:
                SetCursor(z->viewport, z->assets, CUR_POINT);
                return GAMESTATE_OPTS;
            break;
	        case BTN_QUIT:
                return GAMESTATE_EXIT;
            break;
        }
    }
    

    if (ActionPressed(z->controller, A_MB_L))
    {
        SetCursor(z->viewport, z->assets, CUR_CLICK);
        r32 r = RNG()*3;
        Mix_PlayChannel(SFX_TAP_01 + (i32)r, z->assets->sfx[SFX_TAP_01 + (i32)r], 0);
    }

    if (ActionReleased(z->controller, A_MB_L))
        SetCursor(z->viewport, z->assets, CUR_POINT);
        

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
                SetCursor(z->viewport, z->assets, CUR_POINT);
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

    if (ActionPressed(z->controller, A_MB_L))
    {
        SetCursor(z->viewport, z->assets, CUR_CLICK);
        r32 r = RNG()*3;
        Mix_PlayChannel(SFX_TAP_01 + (i32)r, z->assets->sfx[SFX_TAP_01 + (i32)r], 0);
    }

    if (ActionReleased(z->controller, A_MB_L))
        SetCursor(z->viewport, z->assets, CUR_POINT);
    
    return GAMESTATE_OPTS;

}

/*-------------------------------------------------*/
i32 UpdatePlay    (u32 t, r32 dt, void* engine)
{
    zEngine* z = (zEngine*)engine;
    i2 mloc = MouseLocation(z->controller, z->viewport);
    r2 mpos = CamToPos(mloc, z->viewport);
    i2 mcel = PosToCel(mpos, z->game->level_active);
    i32 midx = CelToIdx(mcel, z->game->level_active);

    // char dbug[50];
    // sprintf(dbug, "lvls clear, curr: %d, %d", z->game->levels_cleared, z->game->current_level_number);
    // DrawTextScreen(z->viewport, z->assets->fon[FONT_ID_ZSYS], COLOR_WHITE, ZERO_I2, dbug);


    i32 menu_action_ctrl        = TickMenu(z->menus[MENU_CONTROLS], mloc, z->controller, z->input);
    i32 menu_action_nav_prev    = TickMenu(z->menus[MENU_NAV_PREV], mloc, z->controller, z->input);
    i32 menu_action_nav_next    = TickMenu(z->menus[MENU_NAV_NEXT], mloc, z->controller, z->input);

    if (menu_action_ctrl >= 0)
    {
        switch (menu_action_ctrl)
        {
            case BTN_CTRL_UNDO:
                UndoMove(z->game, z->assets);

            break;
	        case BTN_CTRL_RESTART:
                RestartLevel(z->game, z->menus, z->assets);
            break;
        }
    }
    if (ActionPressed(z->controller, A_UNDO))
    {
        UndoMove(z->game, z->assets);
        
    }
    if (ActionPressed(z->controller, A_RSTRT))
    {
        RestartLevel(z->game, z->menus, z->assets);
        
    }

    if (menu_action_nav_prev >= 0)
    {
        z->game->current_level_number--;
        CloseActiveLevel(z->game);
        OpenLevel(z->game, z->game->current_level_number, z->menus, z->assets);

    }
    if (menu_action_nav_next >= 0)
    {
        if (NextLevel(z->game, z->menus, z->assets))
        {
            return GAMESTATE_PLAY;
        }
        else
        {
            return GAMESTATE_EXIT;
        }
    }

    //TODO add visual indication if you cannot place a unit somewhere, due to cell being conquered or already has a unit
    if (z->game->board_cleared)
    {
        return GAMESTATE_GOAL;
    }
    else
    {

        //only progress if board is not invalid, if invalid we need to undo or restart to progress
        if (z->game->board_invalid == 0)
        {
            if (ValidateCel(mcel, z->game->level_active))
            {
                if (ActionPressed(z->controller, A_MB_L))
                {
                    if (z->game->move_active)
                    {
                        if ((z->game->level_active->cell[midx].type == UNIT_NONE) && (z->game->level_active->cell[midx].collision == CELL_FLOOR) && (z->game->level_active->cell[midx].status == CELL_STATUS_FREE))
                        {
                            if (!i2_equals(z->game->current_move.cell_start, z->game->current_move.cell_end))
                            {
                                PerformMove(z->game, mcel, z->assets);
                                SetCursor(z->viewport, z->assets, CUR_POINT);
                                

                            }
                            else
                            {
                                CancelMove(z->game, z->assets);
                                SetCursor(z->viewport, z->assets, CUR_POINT);
                                
                            }
                        }
                        else
                        {
                            CancelMove(z->game, z->assets);
                            SetCursor(z->viewport, z->assets, CUR_POINT);
                            
                        }
                    }
                    else
                    {
                        if (z->game->level_active->cell[midx].collision == CELL_HAS_UNIT)
                        {
                            StartMove(z->game, mcel, z->assets);
                            SetCursor(z->viewport, z->assets, CUR_GRAB);
                            
                        }
                        else
                        {
                            SetCursor(z->viewport, z->assets, CUR_CLICK);
                            r32 r = RNG()*3;
                            Mix_PlayChannel(SFX_TAP_01 + (i32)r, z->assets->sfx[SFX_TAP_01 + (i32)r], 0);


                        }
                    }
                }



                if (!z->game->move_active)
                {
                    if (z->game->level_active->cell[midx].collision == CELL_HAS_UNIT)
                    {
                        SetCursor(z->viewport, z->assets, CUR_HAND);
                    }
                    else
                    {
                        if (ActionReleased(z->controller, A_MB_L))
                        {
                            SetCursor(z->viewport, z->assets, CUR_POINT);
                        }
                    }
                }
            }
            else
            {
                if (z->game->move_active)
                {
                    if (ActionPressed(z->controller, A_MB_L))
                    {
                        CancelMove(z->game, z->assets);
                        SetCursor(z->viewport, z->assets, CUR_POINT);
                        
                    }
                }
                else
                {
                    if (GET8IN64(z->viewport->settings, ZSDL_SETTINGS_BYTE_ACTIVE_CURSOR) == CUR_HAND)
                    {
                        SetCursor(z->viewport, z->assets, CUR_POINT);
                    }
                    if (ActionPressed(z->controller, A_MB_L))
                    {
                        SetCursor(z->viewport, z->assets, CUR_CLICK);
                        r32 r = RNG()*3;
                        Mix_PlayChannel(SFX_TAP_01 + (i32)r, z->assets->sfx[SFX_TAP_01 + (i32)r], 0);
                    }

                    if (ActionReleased(z->controller, A_MB_L))
                        SetCursor(z->viewport, z->assets, CUR_POINT);
                }
            }
        }
        else
        {
            return GAMESTATE_LOSE;
        }
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
    zEngine* z = (zEngine*)engine;

    i2 mloc = MouseLocation(z->controller, z->viewport);

    i32 menu_action_ctrl        = TickMenu(z->menus[MENU_CONTROLS], mloc, z->controller, z->input);
    i32 menu_action_nav_prev    = TickMenu(z->menus[MENU_NAV_PREV], mloc, z->controller, z->input);
    i32 menu_action_nav_next    = TickMenu(z->menus[MENU_NAV_NEXT], mloc, z->controller, z->input);

    if (menu_action_ctrl >= 0)
    {
        switch (menu_action_ctrl)
        {
            case BTN_CTRL_UNDO:
                UndoMove(z->game, z->assets);
                return GAMESTATE_PLAY;
            break;
	        case BTN_CTRL_RESTART:
                RestartLevel(z->game, z->menus, z->assets);
                return GAMESTATE_PLAY;
            break;
        }
    }

    if (menu_action_nav_prev >= 0)
    {
        z->game->current_level_number--;
        CloseActiveLevel(z->game);
        OpenLevel(z->game, z->game->current_level_number, z->menus, z->assets);
        return GAMESTATE_PLAY;
    }
    if (menu_action_nav_next >= 0)
    {
        if (NextLevel(z->game, z->menus, z->assets))
        {
            return GAMESTATE_PLAY;
        }
        else
        {
            return GAMESTATE_EXIT;
        }
    }

    if (ActionPressed(z->controller, A_MB_L))
    {
        SetCursor(z->viewport, z->assets, CUR_CLICK);
        r32 r = RNG()*3;
        Mix_PlayChannel(SFX_TAP_01 + (i32)r, z->assets->sfx[SFX_TAP_01 + (i32)r], 0);
    }

    if (ActionReleased(z->controller, A_MB_L))
        SetCursor(z->viewport, z->assets, CUR_POINT);

    if (ActionPressed(z->controller, A_UNDO))
    {
        UndoMove(z->game, z->assets);
        return GAMESTATE_PLAY;
    }
    if (ActionPressed(z->controller, A_RSTRT))
    {
        RestartLevel(z->game, z->menus, z->assets);
        return GAMESTATE_PLAY;
    }

    return GAMESTATE_LOSE;
}

/*-------------------------------------------------*/
i32 UpdateGoal    (u32 t, r32 dt, void* engine)
{
    zEngine* z = (zEngine*)engine;

    i2 mloc = MouseLocation(z->controller, z->viewport);

    i32 menu_action_ctrl        = TickMenu(z->menus[MENU_CONTROLS], mloc, z->controller, z->input);
    i32 menu_action_nav_prev    = TickMenu(z->menus[MENU_NAV_PREV], mloc, z->controller, z->input);
    i32 menu_action_nav_next    = TickMenu(z->menus[MENU_NAV_NEXT], mloc, z->controller, z->input);

    if (menu_action_ctrl >= 0)
    {
        switch (menu_action_ctrl)
        {
            case BTN_CTRL_UNDO:
                UndoMove(z->game, z->assets);
                return GAMESTATE_PLAY;
            break;
	        case BTN_CTRL_RESTART:
                RestartLevel(z->game, z->menus, z->assets);
                return GAMESTATE_PLAY;
            break;
        }
    }

    if (menu_action_nav_prev >= 0)
    {
        z->game->current_level_number--;
        CloseActiveLevel(z->game);
        OpenLevel(z->game, z->game->current_level_number, z->menus, z->assets);
        return GAMESTATE_PLAY;
    }
    if (menu_action_nav_next >= 0)
    {
        if (NextLevel(z->game, z->menus, z->assets))
        {
            return GAMESTATE_PLAY;
        }
        else
        {
            return GAMESTATE_EXIT;
        }
    }

    if (ActionPressed(z->controller, A_MB_L))
    {
        SetCursor(z->viewport, z->assets, CUR_CLICK);
        r32 r = RNG()*3;
        Mix_PlayChannel(SFX_TAP_01 + (i32)r, z->assets->sfx[SFX_TAP_01 + (i32)r], 0);
    }

    if (ActionReleased(z->controller, A_MB_L))
        SetCursor(z->viewport, z->assets, CUR_POINT);

    if (ActionPressed(z->controller, A_UNDO))
    {
        UndoMove(z->game, z->assets);
        return GAMESTATE_PLAY;
    }
    if (ActionPressed(z->controller, A_RSTRT))
    {
        RestartLevel(z->game, z->menus, z->assets);
        return GAMESTATE_PLAY;
    }


    return GAMESTATE_GOAL;
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



