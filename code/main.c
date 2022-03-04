#include <stdio.h>
// #include "assets.h"
// #include "game.h"
#include "update.h"
#include "render.h"
#include "zengine.h"
#include "ini.h"
// #include "zsdl.h"

#ifdef __EMSCRIPTEN__
#include "emscripten.h"
#endif

#define DT_MS 10
#define DT_SEC 0.01f



void mainloop(void *arg)
{
	zEngine* z = (zEngine*)(arg);
	static u32 current_time = 0;
	static u32 time_accumulator = 0;
	static u32 t = 0;
	static u32 t_r = 0; //separate tick counter for counting rendered frames

	//*vvvvvvvvvvvvvvvvvvvvvvvvvv GAMELOOP BEGIN vvvvvvvvvvvvvvvvvvvvvvvvvv*/
		u32 new_time   = SDL_GetTicks();
		u32 frame_time = new_time - current_time;
		if (frame_time > 250)
			frame_time = 250; // avoid spiral of death scenario
		current_time = new_time;
		time_accumulator += frame_time;



/* LOGIC UPDATE IN FIXED TIMESTEPS */
		while (time_accumulator >= DT_MS)
		{
			z->gamestate_old = z->gamestate_now;

/* INPUT COLLECTION */
			CollectInput(z->controller);
			TickInput(z->input);

			for (i32 i = 0; i < MAX_PLAYERS; i++)
			{
				if (z->input->pcon[i]->active)
				{
					z->input->pcon[i]->cursor_loc = add_r2(z->input->pcon[i]->cursor_loc, z->input->pcon[i]->nav);
					z->input->pcon[i]->cursor_loc.x = ClampR32(z->input->pcon[i]->cursor_loc.x, 0, ZSDL_INTERNAL_WIDTH - 8);
					z->input->pcon[i]->cursor_loc.y = ClampR32(z->input->pcon[i]->cursor_loc.y, 0, ZSDL_INTERNAL_HEIGHT - 8);
				}
			}

			const u8* keystate = SDL_GetKeyboardState(NULL);
			SDL_Event e;
			while (SDL_PollEvent(&e))
			{
				if (e.type == SDL_QUIT)
					z->gamestate_new = GAMESTATE_EXIT;
				if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN)
				{
					if (keystate[SDL_SCANCODE_LALT])
					{
						ToggleFullscreen(z->viewport);
						ComputePixelScale(z->viewport);
						CalculateScreen(z->viewport);
						RefreshCursors(z->viewport, z->assets);
					}
				}
				if (e.type == SDL_WINDOWEVENT)
				{
					switch(e.window.event)
					{
						case SDL_WINDOWEVENT_RESIZED:
						{
							ComputePixelScale(z->viewport);
							CalculateScreen(z->viewport);
							RefreshCursors(z->viewport, z->assets);
							break;
						}
						default:
						break;
					}
				}
			}

//activate debug rendering target
			SDL_SetRenderTarget(z->viewport->renderer, z->viewport->render_layer[ZSDL_RENDERLAYER_DBUG]);
			SDL_SetRenderDrawColor(z->viewport->renderer, 0x00, 0x00, 0x00, 0x00);
			SDL_RenderClear(z->viewport->renderer);
			
/* TRANSITION GAMESTATE BEGIN */
		    if (z->gamestate_now != z->gamestate_new)
    		{
   				static b8 transition_allowed[NUMBER_OF_GAMESTATES*NUMBER_OF_GAMESTATES] = 
    			{ //FROM	init    main    opts	play	event,  lose	goal	edit	exit	  TO
                			1,      0,      0,		0,  	0,      0,      0,		0,		0,		//init
                			1,      1,      1,		1,  	0,      1,      1,      1,		0,		//main
                			0,      1,      1,		0,  	0,      0,      0,      0,		0,		//opts
                			0,      1,      0,		1,  	1,      1,      1,      1,		0,		//play
                			0,      1,      0,		1,  	1,      1,      1,      0,		0,		//event
                			0,      0,      0,		1,  	1,      1,      0,      0,		0,		//lose
                			0,      0,      0,		1,  	1,      0,      1,      0,		0,		//goal
                			0,      1,      0,		1,  	0,      0,      0,      1,		0,		//edit
                			0,      1,      1,		1,  	1,      1,      1,      1,		1		//exit
    			};

    			if (transition_allowed[z->gamestate_now + z->gamestate_new * NUMBER_OF_GAMESTATES])
	    		{

/*	exit and cleanup current state	*/
#if DEBUGPRNT
printf("Game exiting state \t%s...\n", GetGamestateName(z->gamestate_now));
#endif
	        		switch (z->gamestate_now) 
	        		{
	            		case GAMESTATE_INIT:
						break;
	            		case GAMESTATE_MAIN:
							ToggleMenu(&z->menus[MENU_TITLE], ZDISABLED);
						break;
	            		case GAMESTATE_OPTS:
							ToggleMenu(&z->menus[MENU_OPTIONS], ZDISABLED);
						break;
	            		case GAMESTATE_PLAY:
						break;
	            		case GAMESTATE_EVNT:
						break;
	            		case GAMESTATE_LOSE:
						break;
	            		case GAMESTATE_GOAL:
						break;
	            		case GAMESTATE_EDIT:
						break;
	            		case GAMESTATE_EXIT:
						break;
	        		}
					z->gamestate_old = z->gamestate_now;

/*	enter and setup next state	*/
#if DEBUGPRNT
printf("Game entering state \t%s...\n", GetGamestateName(z->gamestate_new));
#endif
	        		switch (z->gamestate_new) 
	        		{
	            		case GAMESTATE_INIT:
							z->viewport->camera->zoom = 1.f;
						break;
	            		case GAMESTATE_MAIN:
							ToggleMenu(&z->menus[MENU_TITLE], ZENABLED);
						break;
	            		case GAMESTATE_OPTS:
							ToggleMenu(&z->menus[MENU_OPTIONS], ZENABLED);
						break;						
	            		case GAMESTATE_PLAY:
							ToggleMenu(&z->menus[MENU_CONTROLS], ZENABLED);

						break;
	            		case GAMESTATE_EVNT:
							ToggleMenu(&z->menus[MENU_CONTROLS], ZDISABLED);
						break;
	            		case GAMESTATE_LOSE:
					
						break;
	            		case GAMESTATE_GOAL:
							if (z->game->current_level_number > 0)
								ToggleMenu(&z->menus[MENU_NAV_PREV], ZENABLED);
							else
								ToggleMenu(&z->menus[MENU_NAV_PREV], ZDISABLED);
							if (z->game->levels_cleared > z->game->current_level_number)
								ToggleMenu(&z->menus[MENU_NAV_NEXT], ZENABLED);
							else
								ToggleMenu(&z->menus[MENU_NAV_NEXT], ZDISABLED);
						break;
	            		case GAMESTATE_EDIT:
						break;
	            		case GAMESTATE_EXIT:
						break;
	        		}
#if DEBUGPRNT
printf("Gamestate change complete.\n");
#endif
					z->gamestate_now = z->gamestate_new;
					z->t_0_gamestate_change = t;
	    		} // end if transition allowed
	    		else //keep current state, but push back and update old state
	    		{
					z->gamestate_old = z->gamestate_now;
#if DEBUGPRNT
printf("Gamestate change from %s \tto %s was deemed illegal!\n", GetGamestateName(z->gamestate_now), GetGamestateName(z->gamestate_new));
#endif
	    		}
			}
/* TRANSITION GAMESTATE END */

/* PERFORM STATE LOGIC UPDATE */

//perform update for current gamestate
			z->gamestate_new = z->update[z->gamestate_now](t, DT_SEC, z);

// tick things that always tick
			TickParticles(z->particles, t, DT_SEC);

// advance time
			t++;
			time_accumulator -= DT_MS;

// allow quit whenever
			if (ActionPressed(z->controller, A_QUIT))
        		z->gamestate_new = GAMESTATE_EXIT;
		} //logic update end
		
/* RENDER UPDATE */

		CleanRenderTargets(z->viewport);

	//TODO: calculate interpolation value from last render to smooth rendering: prev_state * (1-t) + curr_state * t
		r32 alpha = 1.f;

	//perform rendering for current state
		z->render[z->gamestate_now](t_r, alpha, z);

	//tick render time
		t_r++;

	// draw things that are always drawn
		SDL_SetRenderTarget(z->viewport->renderer, z->viewport->render_layer[ZSDL_RENDERLAYER_ENTITIES]);
		DrawParticles(z->viewport, t, z->particles);

	//render end
		FinalizeRenderAndPresent(z->viewport);
	//main loop end
}


int main(int argc, char* argv[])
{
/*vvvvvvvvvvvvvvvvvvvvvvvvvv INIT vvvvvvvvvvvvvvvvvvvvvvvvvv*/
	SetupSDL();
	Viewport* viewport = CreateViewport("ZENGINE");
	Game* game = CreateGame();
	Controller* controller = CreateController();
	Input* input = CreateInputManager();
	Assets* assets = CreateAssets(viewport);
	viewport->camera = CreateCamera(ZERO_R2);
	Particles* particles = InitParticles();
	Menu* menus = malloc(sizeof(Menu) * MAX_MENUS);
	menus[MENU_TITLE] 			= CreateMenu("main");
	menus[MENU_OPTIONS] 		= CreateMenu("options");
	menus[MENU_OPTIONS_VIDEO] 	= CreateMenu("options_video");
	menus[MENU_OPTIONS_AUDIO] 	= CreateMenu("options_audio");
	menus[MENU_OPTIONS_INPUT] 	= CreateMenu("options_input");
	menus[MENU_CONTROLS] 		= CreateMenu("controls");
	menus[MENU_NAV_PREV] 		= CreateMenu("nav_prev");
	menus[MENU_NAV_NEXT] 		= CreateMenu("nav_next");

	zEngine* z = (zEngine*)malloc(sizeof(zEngine));
	z->viewport = viewport;
	z->game = game;
	z->controller = controller;
	z->input = input;
	z->assets = assets;
	z->particles = particles;
	z->gamestate_now = GAMESTATE_INIT;
	z->gamestate_new = GAMESTATE_INIT;
	z->menus = menus;

    z->update[GAMESTATE_INIT] = &UpdateInit;
    z->update[GAMESTATE_MAIN] = &UpdateMain;
    z->update[GAMESTATE_OPTS] = &UpdateOptions;
    z->update[GAMESTATE_PLAY] = &UpdatePlay;
    z->update[GAMESTATE_EVNT] = &UpdateEvent;
    z->update[GAMESTATE_LOSE] = &UpdateLose;
    z->update[GAMESTATE_GOAL] = &UpdateGoal;
    z->update[GAMESTATE_EDIT] = &UpdateEdit;
    z->update[GAMESTATE_EXIT] = &UpdateExit;

    z->render[GAMESTATE_INIT] = &RenderInit;
    z->render[GAMESTATE_MAIN] = &RenderMain;
    z->render[GAMESTATE_OPTS] = &RenderOptions;
    z->render[GAMESTATE_PLAY] = &RenderPlay;
    z->render[GAMESTATE_EVNT] = &RenderEvent;
    z->render[GAMESTATE_LOSE] = &RenderLose;
    z->render[GAMESTATE_GOAL] = &RenderGoal;
    z->render[GAMESTATE_EDIT] = &RenderEdit;
    z->render[GAMESTATE_EXIT] = &RenderExit;


/*^^^^^^^^^^^^^^^^^^^^^^^^^^ INIT ^^^^^^^^^^^^^^^^^^^^^^^^^^*/

/*vvvvvvvvvvvvvvvvvvvvvvvvvv LOAD ASSETS vvvvvvvvvvvvvvvvvvvvvvvvvv*/
LoadTexture(z->assets, T_TILE_ATLAS, viewport->renderer, T_TILE_ATLAS_PATH);
LoadTexture(z->assets, T_UI_ATLAS, viewport->renderer, T_UI_ATLAS_PATH);
LoadTexture(z->assets, T_PLAYER_CURSOR, viewport->renderer, T_PLAYER_CURSOR_PATH);
LoadTexture(z->assets, T_BG_ATLAS, viewport->renderer, T_BG_ATLAS_PATH);

LoadFont(z->assets, FONT_ID_ZSYS, viewport->renderer, FONT_PATH_ZSYS);

LoadCursor(z->assets, CUR_POINT, CUR_PATH_POINT);
LoadCursor(z->assets, CUR_CLICK, CUR_PATH_CLICK);
LoadCursor(z->assets, CUR_HAND, CUR_PATH_HAND);
LoadCursor(z->assets, CUR_GRAB, CUR_PATH_GRAB);
LoadCursor(z->assets, CUR_CROSS, CUR_PATH_CROSS);

LoadSound(z->assets, SFX_HOVER, 			SFX_PATH_HOVER);
LoadSound(z->assets, SFX_SELECT, 			SFX_PATH_SELECT);
LoadSound(z->assets, SFX_TAP_01, 			SFX_PATH_TAP_01);
LoadSound(z->assets, SFX_TAP_02, 			SFX_PATH_TAP_02);
LoadSound(z->assets, SFX_TAP_03, 			SFX_PATH_TAP_03);
LoadSound(z->assets, SFX_UNDO_01, 			SFX_PATH_UNDO_01);
LoadSound(z->assets, SFX_UNDO_02, 			SFX_PATH_UNDO_02);
LoadSound(z->assets, SFX_UNDO_03, 			SFX_PATH_UNDO_03);
LoadSound(z->assets, SFX_OPEN_LEVEL, 		SFX_PATH_OPEN_LEVEL);
LoadSound(z->assets, SFX_LEVEL_COMPLETE, 	SFX_PATH_LEVEL_COMPLETE);
LoadSound(z->assets, SFX_GOOD_MOVE, 		SFX_PATH_GOOD_MOVE);
LoadSound(z->assets, SFX_BAD_MOVE, 			SFX_PATH_BAD_MOVE);
LoadSound(z->assets, SFX_PICKUP, 			SFX_PATH_PICKUP);
LoadSound(z->assets, SFX_DROP, 				SFX_PATH_DROP);

//load levels
ini_t* level_file = ini_load(LEVEL_DESIGN_PATH);

if (!ini_sget(level_file, "meta", "num_levels", "%d", &z->game->num_levels))
{
	printf("couldn't determine number of levels from level design file! exiting.. \n");
	z->game->num_levels = 1;
	z->gamestate_new = GAMESTATE_EXIT;
}
else
{
	if (z->game->num_levels > MAX_LEVELS)
	{
		printf("level design file has level count %d, which exceeds the max level count of %d\n", z->game->num_levels, MAX_LEVELS);
		z->gamestate_new = GAMESTATE_EXIT;
	}
	else
	{
		for (i32 i = 0; i < z->game->num_levels; i++)
		{
			char lvl_string[9];
			sprintf(lvl_string, "level_%d", i);
			GenerateString(z->assets, STR_LEVEL_IDX_0 + i, lvl_string);
			LoadLevel(z->game, i, z->assets->str[STR_LEVEL_IDX_0 + i]);
		}
	}
}



/*^^^^^^^^^^^^^^^^^^^^^^^^^^ LOAD ASSETS ^^^^^^^^^^^^^^^^^^^^^^^^^^*/

SetCursor(z->viewport, z->assets, CUR_POINT);
//START IN FULLSCREEN
// ToggleFullscreen(z->viewport);
ComputePixelScale(z->viewport);
CalculateScreen(z->viewport);
RefreshCursors(z->viewport, z->assets);





/*vvvvvvvvvvvvvvvvvvvvvvvvvv MAIN LOOP vvvvvvvvvvvvvvvvvvvvvvvvvv*/
#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop_arg(mainloop, z, -1, 1);
#else
	while (z->gamestate_now != GAMESTATE_EXIT)
		mainloop(z);
#endif
/*^^^^^^^^^^^^^^^^^^^^^^^^^^ MAIN LOOP ^^^^^^^^^^^^^^^^^^^^^^^^^^*/

#if DEBUGPRNT
printf("\n~~~Exiting game!~~~\n");
#endif		
	// free all things
	FreeParticles(particles);
	FreeMenus(menus);
	FreeController(controller);
	FreeInputManager(input);
	FreeAssets(assets);
	FreeViewport(viewport);
	FreeGame(game);
	free(z);

	Mix_Quit();
	IMG_Quit();
	SDL_Quit();

	return 0;
}