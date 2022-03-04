#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"
#include "ini.h"

char* GetGamestateName(Gamestate state)
{
	switch (state)
	{
		case GAMESTATE_INIT:
			return "[Gamestate Init]";
		case GAMESTATE_MAIN:
			return "[Gamestate Main]";
		case GAMESTATE_OPTS:
			return "[Gamestate Options]";
		case GAMESTATE_PLAY:
			return "[Gamestate Play]";
		case GAMESTATE_EVNT:
			return "[Gamestate Event]";
		case GAMESTATE_LOSE:
			return "[Gamestate Lose]";
		case GAMESTATE_GOAL:
			return "[Gamestate Goal]";
		case GAMESTATE_EDIT:
			return "[Gamestate Edit]";
		case GAMESTATE_EXIT:
			return "[Gamestate Exit]";
		default:
			return "[Gamestate Unknown]";
	}
}


Game* CreateGame()
{
    Game* new_game = malloc(sizeof(Game));
	new_game->level_active = NULL;
	for (i32 i = 0; i < MAX_UNDOS; i++)
	{
		new_game->move_stack[i] = NULL;
	}

	RestartGame(new_game);
    return new_game;
}

void FreeGame(Game* game)
{
    if (game != NULL)
	{
		CloseActiveLevel(game);
		for (i32 i = 0; i < game->num_levels; i++)
		{
			if (game->levels[i] != NULL)
				FreeGrid(game->levels[i]);
		}
        free(game);
	}
    printf("Game freed.\n");
}

void RestartGame(Game* game)
{
	game->current_level_number = 0;
	game->move_active = 0;
	game->last_move_idx = 0;
	game->levels_cleared = 0;
	
}

void LoadLevel(Game* game, u32 level_number, const char* level_name)
{
	
	ini_t* level_design = ini_load(LEVEL_DESIGN_PATH);
	i32 width, height;
	ini_sget(level_design, level_name, "width", "%d", &width);
	ini_sget(level_design, level_name, "height", "%d", &height);

	game->levels[level_number] = CreateGrid(width, height, make_r2(width * -0.5f * WORLD_UNIT_F, height * -0.5f * WORLD_UNIT_F));


	for (i32 y = 0; y < height; y++)
	{
		char lvl_row[2];
		sprintf(lvl_row, "%d", y);
		const char* lvl_string_raw = ini_get(level_design, level_name, lvl_row);
		for (i32 x = 0; x < width; x++)
		{
			game->levels[level_number]->cell[x + y * width].status = CELL_STATUS_FREE;
			switch(lvl_string_raw[x])
			{
				case '0':
				{
					game->levels[level_number]->cell[x + y * width].collision 	= CELL_WALL;
					game->levels[level_number]->cell[x + y * width].type 		= UNIT_NONE;
					game->levels[level_number]->cell[x + y * width].sprite_bg 	= CELL_SPR_WALL_TILE;
					game->levels[level_number]->cell[x + y * width].sprite_mg 	= CELL_SPR_UNIT_NONE;
					game->levels[level_number]->cell[x + y * width].sprite_fg 	= CELL_SPR_TRANSPARENT;
					break;
				}
				case '.':
				{
					game->levels[level_number]->cell[x + y * width].collision 	= CELL_FLOOR;
					game->levels[level_number]->cell[x + y * width].type 		= UNIT_NONE;
					game->levels[level_number]->cell[x + y * width].sprite_bg 	= CELL_SPR_EMPTY_TILE;
					game->levels[level_number]->cell[x + y * width].sprite_mg 	= CELL_SPR_UNIT_NONE;
					game->levels[level_number]->cell[x + y * width].sprite_fg 	= CELL_SPR_TRANSPARENT;
					break;
				}
				case 'o':
				{
					game->levels[level_number]->cell[x + y * width].collision 	= CELL_HAS_UNIT;
					game->levels[level_number]->cell[x + y * width].type 		= UNIT_BASE;
					game->levels[level_number]->cell[x + y * width].sprite_bg 	= CELL_SPR_EMPTY_TILE;
					game->levels[level_number]->cell[x + y * width].sprite_mg 	= CELL_SPR_UNIT_BASE;
					game->levels[level_number]->cell[x + y * width].sprite_fg 	= CELL_SPR_TRANSPARENT;
					break;
				}
				case '@':
				{
					game->levels[level_number]->cell[x + y * width].collision 	= CELL_HAS_UNIT;
					game->levels[level_number]->cell[x + y * width].type 		= UNIT_BOMB;
					game->levels[level_number]->cell[x + y * width].sprite_bg 	= CELL_SPR_EMPTY_TILE;
					game->levels[level_number]->cell[x + y * width].sprite_mg 	= CELL_SPR_UNIT_BOMB;
					game->levels[level_number]->cell[x + y * width].sprite_fg 	= CELL_SPR_TRANSPARENT;
					break;
				}
				case '^':
				{
					game->levels[level_number]->cell[x + y * width].collision 	= CELL_HAS_UNIT;
					game->levels[level_number]->cell[x + y * width].type 		= UNIT_UNO_N;
					game->levels[level_number]->cell[x + y * width].sprite_bg 	= CELL_SPR_EMPTY_TILE;
					game->levels[level_number]->cell[x + y * width].sprite_mg 	= CELL_SPR_UNIT_UNO_N;
					game->levels[level_number]->cell[x + y * width].sprite_fg 	= CELL_SPR_TRANSPARENT;
					break;
				}
				case 'n':
				{
					game->levels[level_number]->cell[x + y * width].collision 	= CELL_HAS_UNIT;
					game->levels[level_number]->cell[x + y * width].type 		= UNIT_UNO_NE;
					game->levels[level_number]->cell[x + y * width].sprite_bg 	= CELL_SPR_EMPTY_TILE;
					game->levels[level_number]->cell[x + y * width].sprite_mg 	= CELL_SPR_UNIT_UNO_NE;
					game->levels[level_number]->cell[x + y * width].sprite_fg 	= CELL_SPR_TRANSPARENT;
					break;
				}
				case '>':
				{
					game->levels[level_number]->cell[x + y * width].collision 	= CELL_HAS_UNIT;
					game->levels[level_number]->cell[x + y * width].type 		= UNIT_UNO_E;
					game->levels[level_number]->cell[x + y * width].sprite_bg 	= CELL_SPR_EMPTY_TILE;
					game->levels[level_number]->cell[x + y * width].sprite_mg 	= CELL_SPR_UNIT_UNO_E;
					game->levels[level_number]->cell[x + y * width].sprite_fg 	= CELL_SPR_TRANSPARENT;
					break;
				}
				case 'e':
				{
					game->levels[level_number]->cell[x + y * width].collision 	= CELL_HAS_UNIT;
					game->levels[level_number]->cell[x + y * width].type 		= UNIT_UNO_SE;
					game->levels[level_number]->cell[x + y * width].sprite_bg 	= CELL_SPR_EMPTY_TILE;
					game->levels[level_number]->cell[x + y * width].sprite_mg 	= CELL_SPR_UNIT_UNO_SE;
					game->levels[level_number]->cell[x + y * width].sprite_fg 	= CELL_SPR_TRANSPARENT;
					break;
				}
				case 'v':
				{
					game->levels[level_number]->cell[x + y * width].collision 	= CELL_HAS_UNIT;
					game->levels[level_number]->cell[x + y * width].type 		= UNIT_UNO_S;
					game->levels[level_number]->cell[x + y * width].sprite_bg 	= CELL_SPR_EMPTY_TILE;
					game->levels[level_number]->cell[x + y * width].sprite_mg 	= CELL_SPR_UNIT_UNO_S;
					game->levels[level_number]->cell[x + y * width].sprite_fg 	= CELL_SPR_TRANSPARENT;
					break;
				}
				case 's':
				{
					game->levels[level_number]->cell[x + y * width].collision 	= CELL_HAS_UNIT;
					game->levels[level_number]->cell[x + y * width].type 		= UNIT_UNO_SW;
					game->levels[level_number]->cell[x + y * width].sprite_bg 	= CELL_SPR_EMPTY_TILE;
					game->levels[level_number]->cell[x + y * width].sprite_mg 	= CELL_SPR_UNIT_UNO_SW;
					game->levels[level_number]->cell[x + y * width].sprite_fg 	= CELL_SPR_TRANSPARENT;
					break;
				}
				case '<':
				{
					game->levels[level_number]->cell[x + y * width].collision 	= CELL_HAS_UNIT;
					game->levels[level_number]->cell[x + y * width].type 		= UNIT_UNO_W;
					game->levels[level_number]->cell[x + y * width].sprite_bg 	= CELL_SPR_EMPTY_TILE;
					game->levels[level_number]->cell[x + y * width].sprite_mg 	= CELL_SPR_UNIT_UNO_W;
					game->levels[level_number]->cell[x + y * width].sprite_fg 	= CELL_SPR_TRANSPARENT;
					break;
				}
				case 'w':
				{
					game->levels[level_number]->cell[x + y * width].collision 	= CELL_HAS_UNIT;
					game->levels[level_number]->cell[x + y * width].type 		= UNIT_UNO_NW;
					game->levels[level_number]->cell[x + y * width].sprite_bg 	= CELL_SPR_EMPTY_TILE;
					game->levels[level_number]->cell[x + y * width].sprite_mg 	= CELL_SPR_UNIT_UNO_NW;
					game->levels[level_number]->cell[x + y * width].sprite_fg 	= CELL_SPR_TRANSPARENT;
					break;
				}
				case '|':
				{
					game->levels[level_number]->cell[x + y * width].collision 	= CELL_HAS_UNIT;
					game->levels[level_number]->cell[x + y * width].type 		= UNIT_DUO_VERT;
					game->levels[level_number]->cell[x + y * width].sprite_bg 	= CELL_SPR_EMPTY_TILE;
					game->levels[level_number]->cell[x + y * width].sprite_mg 	= CELL_SPR_UNIT_DUO_VERT;
					game->levels[level_number]->cell[x + y * width].sprite_fg 	= CELL_SPR_TRANSPARENT;
					break;
				}
				case 'r':
				{
					game->levels[level_number]->cell[x + y * width].collision 	= CELL_HAS_UNIT;
					game->levels[level_number]->cell[x + y * width].type 		= UNIT_DUO_DIA_R;
					game->levels[level_number]->cell[x + y * width].sprite_bg 	= CELL_SPR_EMPTY_TILE;
					game->levels[level_number]->cell[x + y * width].sprite_mg 	= CELL_SPR_UNIT_DUO_DIA_R;
					game->levels[level_number]->cell[x + y * width].sprite_fg 	= CELL_SPR_TRANSPARENT;
					break;
				}
				case '-':
				{
					game->levels[level_number]->cell[x + y * width].collision 	= CELL_HAS_UNIT;
					game->levels[level_number]->cell[x + y * width].type 		= UNIT_DUO_HORI;
					game->levels[level_number]->cell[x + y * width].sprite_bg 	= CELL_SPR_EMPTY_TILE;
					game->levels[level_number]->cell[x + y * width].sprite_mg 	= CELL_SPR_UNIT_DUO_HORI;
					game->levels[level_number]->cell[x + y * width].sprite_fg 	= CELL_SPR_TRANSPARENT;
					break;
				}
				case 'l':
				{
					game->levels[level_number]->cell[x + y * width].collision 	= CELL_HAS_UNIT;
					game->levels[level_number]->cell[x + y * width].type 		= UNIT_DUO_DIA_L;
					game->levels[level_number]->cell[x + y * width].sprite_bg 	= CELL_SPR_EMPTY_TILE;
					game->levels[level_number]->cell[x + y * width].sprite_mg 	= CELL_SPR_UNIT_DUO_DIA_L;
					game->levels[level_number]->cell[x + y * width].sprite_fg 	= CELL_SPR_TRANSPARENT;
					break;
				}
				case '+':
				{
					game->levels[level_number]->cell[x + y * width].collision 	= CELL_HAS_UNIT;
					game->levels[level_number]->cell[x + y * width].type 		= UNIT_QUAD_ORT;
					game->levels[level_number]->cell[x + y * width].sprite_bg 	= CELL_SPR_EMPTY_TILE;
					game->levels[level_number]->cell[x + y * width].sprite_mg 	= CELL_SPR_UNIT_QUAD_ORT;
					game->levels[level_number]->cell[x + y * width].sprite_fg 	= CELL_SPR_TRANSPARENT;
					break;
				}
				case 'x':
				{
					game->levels[level_number]->cell[x + y * width].collision 	= CELL_HAS_UNIT;
					game->levels[level_number]->cell[x + y * width].type 		= UNIT_QUAD_DIA;
					game->levels[level_number]->cell[x + y * width].sprite_bg 	= CELL_SPR_EMPTY_TILE;
					game->levels[level_number]->cell[x + y * width].sprite_mg 	= CELL_SPR_UNIT_QUAD_DIA;
					game->levels[level_number]->cell[x + y * width].sprite_fg 	= CELL_SPR_TRANSPARENT;
					break;
				}
				case '*':
				{
					game->levels[level_number]->cell[x + y * width].collision 	= CELL_HAS_UNIT;
					game->levels[level_number]->cell[x + y * width].type 		= UNIT_QUEEN;
					game->levels[level_number]->cell[x + y * width].sprite_bg 	= CELL_SPR_EMPTY_TILE;
					game->levels[level_number]->cell[x + y * width].sprite_mg 	= CELL_SPR_UNIT_QUEEN;
					game->levels[level_number]->cell[x + y * width].sprite_fg 	= CELL_SPR_TRANSPARENT;
					break;
				}
			}
		}
	}

	ini_free(level_design);
}

/*closes current level, opens next and increments current_level_number*/
b8 NextLevel(Game* game, Menu* menus, Assets* assets)
{

	game->current_level_number++;
	if (game->current_level_number == game->num_levels)
	{
		//CloseActiveLevel(game);
		return 0;
	}
	else
	{
		CloseActiveLevel(game);
		OpenLevel(game, game->current_level_number, menus, assets);
		return 1;
	}
}

void RestartLevel(Game* game, Menu* menus, Assets* assets)
{
	CloseActiveLevel(game);
	OpenLevel(game, game->current_level_number, menus, assets);


}

void CloseActiveLevel(Game* game)
{
	if (game->level_active != NULL)
	{
		FreeGrid(game->level_active);
		game->level_active = NULL;
	}
	
}

void ClearUndoStack(Game* game)
{
	for (i32 i = 0; i < MAX_UNDOS; i++)
	{
		if (game->move_stack[i] != NULL)
		{
			free(game->move_stack[i]);
			game->move_stack[i] = NULL;
		}
	}
	game->last_move_idx = 0;
}

void OpenLevel(Game* game, u32 level_number, Menu* menus, Assets* assets)
{
	game->level_active = CreateGrid(game->levels[game->current_level_number]->width, game->levels[game->current_level_number]->height, game->levels[game->current_level_number]->origin);
	for (i32 y = 0; y < game->level_active->height; y++)
	{
		for (i32 x = 0; x < game->level_active->width; x++)
		{
			i32 i = x + y * game->level_active->width;
			game->level_active->cell[i].collision 	= game->levels[game->current_level_number]->cell[i].collision;
			game->level_active->cell[i].type 		= game->levels[game->current_level_number]->cell[i].type;
			game->level_active->cell[i].status 		= game->levels[game->current_level_number]->cell[i].status;
			game->level_active->cell[i].sprite_bg 	= game->levels[game->current_level_number]->cell[i].sprite_bg;
			game->level_active->cell[i].sprite_mg 	= game->levels[game->current_level_number]->cell[i].sprite_mg;
			game->level_active->cell[i].sprite_fg 	= game->levels[game->current_level_number]->cell[i].sprite_fg;
		}
	}
	game->board_invalid = 0;
	game->board_cleared = 0;
	ClearUndoStack(game);

	if (game->current_level_number > 0)
		ToggleMenu(&menus[MENU_NAV_PREV], ZENABLED);
	else
		ToggleMenu(&menus[MENU_NAV_PREV], ZDISABLED);
	if (game->levels_cleared > game->current_level_number)
		ToggleMenu(&menus[MENU_NAV_NEXT], ZENABLED);
	else
		ToggleMenu(&menus[MENU_NAV_NEXT], ZDISABLED);
	
	Mix_PlayChannel(SFX_OPEN_LEVEL, assets->sfx[SFX_OPEN_LEVEL], 0);
}

/*GAME SPECIFICS*/
void StartMove(Game* game, i2 cell_start, Assets* assets)
{
	game->move_active = 1;
	game->current_move.cell_start = cell_start;
	game->current_move.unit_type = game->level_active->cell[CelToIdx(cell_start, game->level_active)].type;
	u8 ghost_sprite = GET4IN8(game->level_active->cell[CelToIdx(cell_start, game->level_active)].sprite_mg, BITPOS_SPRITE_ROW) + 6;
	SET4IN8(ghost_sprite, &game->level_active->cell[CelToIdx(cell_start, game->level_active)].sprite_mg, BITPOS_SPRITE_ROW);
	Mix_PlayChannel(SFX_PICKUP, assets->sfx[SFX_PICKUP], 0);
}

void CancelMove(Game* game, Assets* assets)
{
	game->level_active->cell[CelToIdx(game->current_move.cell_start, game->level_active)].sprite_mg = UnitTypeToSprite(game->current_move.unit_type);
	game->move_active = 0;
	game->current_move.cell_start = make_i2(-1, -1);
	game->current_move.unit_type = UNIT_NONE;
	Mix_PlayChannel(SFX_DROP, assets->sfx[SFX_DROP], 0);
}





void EvaluateBoard(Game* game, Assets* assets)
{
	i32 cell_ok_counter = 0;
	i32 bad_counter = 0;

	for (i32 y = 0; y < game->level_active->height; y++)
	{
		for (i32 x = 0; x < game->level_active->width; x++)
		{
			i32 idx = x + y * game->level_active->width;
			u8 status = game->level_active->cell[idx].status;
			u8 collision = game->level_active->cell[idx].collision;

			if( (status == CELL_STATUS_IS_CONQUERED) || (collision == CELL_HAS_UNIT_MOVED) || (collision == CELL_WALL) )
			{
				//everything appears to be in order, continue
				cell_ok_counter++;
			}
			if ((status == CELL_STATUS_IS_DOUBLE_CONQUERED))
			{
				bad_counter++;
			}
		}
	}

	if (cell_ok_counter == (game->level_active->height * game->level_active->width))
	{
		game->board_cleared = 1;
		if (game->current_level_number == game->levels_cleared)
			game->levels_cleared++;
		Mix_PlayChannel(SFX_LEVEL_COMPLETE, assets->sfx[SFX_LEVEL_COMPLETE], 0);
	}
	else
	{
		game->board_cleared = 0;
	}

	if (bad_counter > 0)
	{
		game->board_invalid = 1;
	}
	else
	{
		game->board_invalid = 0;
	}
}





/*adds cell to conquered list in move, and increments the num_conquered val for the current move*/
void ConquerCell(Game* game, i2 cell)
{
	i32 idx = CelToIdx(cell, game->level_active);
	u8 collision 	= game->level_active->cell[idx].collision;
	u8 status 		= game->level_active->cell[idx].status;
	if ((collision == CELL_FLOOR) && (status == CELL_STATUS_FREE))
	{
		game->level_active->cell[idx].status = CELL_STATUS_IS_CONQUERED;
		game->level_active->cell[idx].sprite_bg = CELL_SPR_CONQUERED_TILE;
	}
	else
	{
		game->level_active->cell[idx].status = CELL_STATUS_IS_DOUBLE_CONQUERED;
		game->level_active->cell[idx].sprite_bg = CELL_SPR_DOUBLE_CONQUERED_TILE;
	}
	game->current_move.conquered_cells[game->current_move.num_conquered] = cell;
	game->current_move.num_conquered++;
}


/*---------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------*/
void PerformMove(Game* game, i2 cell_end, Assets* assets)
{
//reset conquered cells for move, TODO use memset?
	for (i32 i = 0; i < MAX_CONQUERED_CELLS; i++)
	{
		game->current_move.conquered_cells[i] = make_i2(-1, -1);
	}
	game->current_move.num_conquered = 0;
	game->current_move.cell_end = cell_end;
	i32 idx_0 = CelToIdx(game->current_move.cell_start, game->level_active);
	i32 idx_1 = CelToIdx(game->current_move.cell_end, game->level_active);

//move piece
	//clear old tile
	game->level_active->cell[idx_0].collision 	= CELL_FLOOR;
	game->level_active->cell[idx_0].type 		= UNIT_NONE;
	game->level_active->cell[idx_0].status 		= CELL_STATUS_FREE;
	game->level_active->cell[idx_0].sprite_bg 	= CELL_SPR_EMPTY_TILE;
	game->level_active->cell[idx_0].sprite_mg 	= CELL_SPR_UNIT_NONE;

//conquer new tile before moving there, so we don't accidentally think this piece was occupying that area before
	ConquerCell(game, game->current_move.cell_end);

	//update new tile
	game->level_active->cell[idx_1].collision 	= CELL_HAS_UNIT_MOVED;
	game->level_active->cell[idx_1].type 		= game->current_move.unit_type;
	game->level_active->cell[idx_1].sprite_mg 	= UnitTypeToSprite(game->current_move.unit_type);
	//game->level_active->cell[idx_1].status 		= CELL_STATUS_IS_CONQUERED;
	//game->level_active->cell[idx_1].sprite_bg 	= CELL_SPR_CONQUERED_TILE;



//perform piece specific conquering
/*vvvvvvvvvvvvvvvvvvvvvvvvvv  vvvvvvvvvvvvvvvvvvvvvvvvvv*/
	switch(game->current_move.unit_type)
	{
		case UNIT_NONE:
		{
			printf("~~~ NOTICE! Something wrong happened, and we're trying to perform a conquer with a NONE unit!! ~~~\n");
			break;
		}
    	case UNIT_BASE:
		{
			//only conquers the tile it lands on
			break;
		}
    	case UNIT_BOMB:
		{
			// game->level_active->cell[idx_0].status = CELL_STATUS_IS_CONQUERED;
			// game->level_active->cell[idx_0].sprite_bg = CELL_SPR_CONQUERED_TILE;
			// game->current_move.conquered_cells[1] = game->current_move.cell_start;
			ConquerCell(game, game->current_move.cell_start);
			break;
		}
    	case UNIT_UNO_N:
		{
			i2 cel_conquer = make_i2(game->current_move.cell_end.x, game->current_move.cell_end.y - 1);
			i32 conquer_idx = CelToIdx(cel_conquer, game->level_active);
			while( (game->level_active->cell[conquer_idx].collision != CELL_WALL) && (ValidateCel(cel_conquer, game->level_active) ) )
			{
				ConquerCell(game, cel_conquer);
				cel_conquer.y--;
				conquer_idx = CelToIdx(cel_conquer, game->level_active);
			}
			break;
		}
    	case UNIT_UNO_NE:
		{
			i2 cel_conquer = make_i2(game->current_move.cell_end.x + 1, game->current_move.cell_end.y - 1);
			i32 conquer_idx = CelToIdx(cel_conquer, game->level_active);
			while( (game->level_active->cell[conquer_idx].collision != CELL_WALL) && (ValidateCel(cel_conquer, game->level_active) ) )
			{
				ConquerCell(game, cel_conquer);
				cel_conquer.x++;
				cel_conquer.y--;
				conquer_idx = CelToIdx(cel_conquer, game->level_active);
			}
			break;
		}
    	case UNIT_UNO_E:
		{
			i2 cel_conquer = make_i2(game->current_move.cell_end.x + 1, game->current_move.cell_end.y);
			i32 conquer_idx = CelToIdx(cel_conquer, game->level_active);
			while( (game->level_active->cell[conquer_idx].collision != CELL_WALL) && (ValidateCel(cel_conquer, game->level_active) ) )
			{
				ConquerCell(game, cel_conquer);
				cel_conquer.x++;
				conquer_idx = CelToIdx(cel_conquer, game->level_active);
			}
			break;
		}
    	case UNIT_UNO_SE:
		{
			i2 cel_conquer = make_i2(game->current_move.cell_end.x + 1, game->current_move.cell_end.y + 1);
			i32 conquer_idx = CelToIdx(cel_conquer, game->level_active);
			while( (game->level_active->cell[conquer_idx].collision != CELL_WALL) && (ValidateCel(cel_conquer, game->level_active) ) )
			{
				ConquerCell(game, cel_conquer);
				cel_conquer.x++;
				cel_conquer.y++;
				conquer_idx = CelToIdx(cel_conquer, game->level_active);
			}
			break;
		}
    	case UNIT_UNO_S:
		{
			i2 cel_conquer = make_i2(game->current_move.cell_end.x, game->current_move.cell_end.y + 1);
			i32 conquer_idx = CelToIdx(cel_conquer, game->level_active);
			while( (game->level_active->cell[conquer_idx].collision != CELL_WALL) && (ValidateCel(cel_conquer, game->level_active) ) )
			{
				ConquerCell(game, cel_conquer);
				cel_conquer.y++;
				conquer_idx = CelToIdx(cel_conquer, game->level_active);
			}
			break;
		}
    	case UNIT_UNO_SW:
		{
			i2 cel_conquer = make_i2(game->current_move.cell_end.x - 1, game->current_move.cell_end.y + 1);
			i32 conquer_idx = CelToIdx(cel_conquer, game->level_active);
			while( (game->level_active->cell[conquer_idx].collision != CELL_WALL) && (ValidateCel(cel_conquer, game->level_active) ) )
			{
				ConquerCell(game, cel_conquer);
				cel_conquer.x--;
				cel_conquer.y++;
				conquer_idx = CelToIdx(cel_conquer, game->level_active);
			}
			break;
		}
    	case UNIT_UNO_W:
		{
			i2 cel_conquer = make_i2(game->current_move.cell_end.x - 1, game->current_move.cell_end.y);
			i32 conquer_idx = CelToIdx(cel_conquer, game->level_active);
			while( (game->level_active->cell[conquer_idx].collision != CELL_WALL) && (ValidateCel(cel_conquer, game->level_active) ) )
			{
				ConquerCell(game, cel_conquer);
				cel_conquer.x--;
				conquer_idx = CelToIdx(cel_conquer, game->level_active);
			}
			break;
		}
    	case UNIT_UNO_NW:
		{
			i2 cel_conquer = make_i2(game->current_move.cell_end.x - 1, game->current_move.cell_end.y - 1);
			i32 conquer_idx = CelToIdx(cel_conquer, game->level_active);
			while( (game->level_active->cell[conquer_idx].collision != CELL_WALL) && (ValidateCel(cel_conquer, game->level_active) ) )
			{
				ConquerCell(game, cel_conquer);
				cel_conquer.x--;
				cel_conquer.y--;
				conquer_idx = CelToIdx(cel_conquer, game->level_active);
			}
			break;
		}
    	case UNIT_DUO_VERT:
		{
			//north
			i2 cel_conquer = make_i2(game->current_move.cell_end.x, game->current_move.cell_end.y - 1);
			i32 conquer_idx = CelToIdx(cel_conquer, game->level_active);
			while( (game->level_active->cell[conquer_idx].collision != CELL_WALL) && (ValidateCel(cel_conquer, game->level_active) ) )
			{
				ConquerCell(game, cel_conquer);
				cel_conquer.y--;
				conquer_idx = CelToIdx(cel_conquer, game->level_active);
			}
			//south
			cel_conquer = make_i2(game->current_move.cell_end.x, game->current_move.cell_end.y + 1);
			conquer_idx = CelToIdx(cel_conquer, game->level_active);
			while( (game->level_active->cell[conquer_idx].collision != CELL_WALL) && (ValidateCel(cel_conquer, game->level_active) ) )
			{
				ConquerCell(game, cel_conquer);
				cel_conquer.y++;
				conquer_idx = CelToIdx(cel_conquer, game->level_active);
			}
			break;
		}
    	case UNIT_DUO_DIA_R:
		{
			//northeast
			i2 cel_conquer = make_i2(game->current_move.cell_end.x + 1, game->current_move.cell_end.y - 1);
			i32 conquer_idx = CelToIdx(cel_conquer, game->level_active);
			while( (game->level_active->cell[conquer_idx].collision != CELL_WALL) && (ValidateCel(cel_conquer, game->level_active) ) )
			{
				ConquerCell(game, cel_conquer);
				cel_conquer.x++;
				cel_conquer.y--;
				conquer_idx = CelToIdx(cel_conquer, game->level_active);
			}			
			//southwest
			cel_conquer = make_i2(game->current_move.cell_end.x - 1, game->current_move.cell_end.y + 1);
			conquer_idx = CelToIdx(cel_conquer, game->level_active);
			while( (game->level_active->cell[conquer_idx].collision != CELL_WALL) && (ValidateCel(cel_conquer, game->level_active) ) )
			{
				ConquerCell(game, cel_conquer);
				cel_conquer.x--;
				cel_conquer.y++;
				conquer_idx = CelToIdx(cel_conquer, game->level_active);
			}			
			break;
		}
    	case UNIT_DUO_HORI:
		{
			//east
			i2 cel_conquer = make_i2(game->current_move.cell_end.x + 1, game->current_move.cell_end.y);
			i32 conquer_idx = CelToIdx(cel_conquer, game->level_active);
			while( (game->level_active->cell[conquer_idx].collision != CELL_WALL) && (ValidateCel(cel_conquer, game->level_active) ) )
			{
				ConquerCell(game, cel_conquer);
				cel_conquer.x++;
				conquer_idx = CelToIdx(cel_conquer, game->level_active);
			}
			//west
			cel_conquer = make_i2(game->current_move.cell_end.x - 1, game->current_move.cell_end.y);
			conquer_idx = CelToIdx(cel_conquer, game->level_active);
			while( (game->level_active->cell[conquer_idx].collision != CELL_WALL) && (ValidateCel(cel_conquer, game->level_active) ) )
			{
				ConquerCell(game, cel_conquer);
				cel_conquer.x--;
				conquer_idx = CelToIdx(cel_conquer, game->level_active);
			}
			break;
		}
    	case UNIT_DUO_DIA_L:
		{
			//northwest
			i2 cel_conquer = make_i2(game->current_move.cell_end.x - 1, game->current_move.cell_end.y - 1);
			i32 conquer_idx = CelToIdx(cel_conquer, game->level_active);
			while( (game->level_active->cell[conquer_idx].collision != CELL_WALL) && (ValidateCel(cel_conquer, game->level_active) ) )
			{
				ConquerCell(game, cel_conquer);
				cel_conquer.x--;
				cel_conquer.y--;
				conquer_idx = CelToIdx(cel_conquer, game->level_active);
			}
			//southeast
			cel_conquer = make_i2(game->current_move.cell_end.x + 1, game->current_move.cell_end.y + 1);
			conquer_idx = CelToIdx(cel_conquer, game->level_active);
			while( (game->level_active->cell[conquer_idx].collision != CELL_WALL) && (ValidateCel(cel_conquer, game->level_active) ) )
			{
				ConquerCell(game, cel_conquer);
				cel_conquer.x++;
				cel_conquer.y++;
				conquer_idx = CelToIdx(cel_conquer, game->level_active);
			}
			break;
		}
    	case UNIT_QUAD_ORT:
		{
			//north
			i2 cel_conquer = make_i2(game->current_move.cell_end.x, game->current_move.cell_end.y - 1);
			i32 conquer_idx = CelToIdx(cel_conquer, game->level_active);
			while( (game->level_active->cell[conquer_idx].collision != CELL_WALL) && (ValidateCel(cel_conquer, game->level_active) ) )
			{
				ConquerCell(game, cel_conquer);
				cel_conquer.y--;
				conquer_idx = CelToIdx(cel_conquer, game->level_active);
			}
			//south
			cel_conquer = make_i2(game->current_move.cell_end.x, game->current_move.cell_end.y + 1);
			conquer_idx = CelToIdx(cel_conquer, game->level_active);
			while( (game->level_active->cell[conquer_idx].collision != CELL_WALL) && (ValidateCel(cel_conquer, game->level_active) ) )
			{
				ConquerCell(game, cel_conquer);
				cel_conquer.y++;
				conquer_idx = CelToIdx(cel_conquer, game->level_active);
			}			
			//east
			cel_conquer = make_i2(game->current_move.cell_end.x + 1, game->current_move.cell_end.y);
			conquer_idx = CelToIdx(cel_conquer, game->level_active);
			while( (game->level_active->cell[conquer_idx].collision != CELL_WALL) && (ValidateCel(cel_conquer, game->level_active) ) )
			{
				ConquerCell(game, cel_conquer);
				cel_conquer.x++;
				conquer_idx = CelToIdx(cel_conquer, game->level_active);
			}
			//west
			cel_conquer = make_i2(game->current_move.cell_end.x - 1, game->current_move.cell_end.y);
			conquer_idx = CelToIdx(cel_conquer, game->level_active);
			while( (game->level_active->cell[conquer_idx].collision != CELL_WALL) && (ValidateCel(cel_conquer, game->level_active) ) )
			{
				ConquerCell(game, cel_conquer);
				cel_conquer.x--;
				conquer_idx = CelToIdx(cel_conquer, game->level_active);
			}
			break;
		}
    	case UNIT_QUAD_DIA:
		{
			//northeast
			i2 cel_conquer = make_i2(game->current_move.cell_end.x + 1, game->current_move.cell_end.y - 1);
			i32 conquer_idx = CelToIdx(cel_conquer, game->level_active);
			while( (game->level_active->cell[conquer_idx].collision != CELL_WALL) && (ValidateCel(cel_conquer, game->level_active) ) )
			{
				ConquerCell(game, cel_conquer);
				cel_conquer.x++;
				cel_conquer.y--;
				conquer_idx = CelToIdx(cel_conquer, game->level_active);
			}			
			//southwest
			cel_conquer = make_i2(game->current_move.cell_end.x - 1, game->current_move.cell_end.y + 1);
			conquer_idx = CelToIdx(cel_conquer, game->level_active);
			while( (game->level_active->cell[conquer_idx].collision != CELL_WALL) && (ValidateCel(cel_conquer, game->level_active) ) )
			{
				ConquerCell(game, cel_conquer);
				cel_conquer.x--;
				cel_conquer.y++;
				conquer_idx = CelToIdx(cel_conquer, game->level_active);
			}				
			//northwest
			cel_conquer = make_i2(game->current_move.cell_end.x - 1, game->current_move.cell_end.y - 1);
			conquer_idx = CelToIdx(cel_conquer, game->level_active);
			while( (game->level_active->cell[conquer_idx].collision != CELL_WALL) && (ValidateCel(cel_conquer, game->level_active) ) )
			{
				ConquerCell(game, cel_conquer);
				cel_conquer.x--;
				cel_conquer.y--;
				conquer_idx = CelToIdx(cel_conquer, game->level_active);
			}
			//southeast
			cel_conquer = make_i2(game->current_move.cell_end.x + 1, game->current_move.cell_end.y + 1);
			conquer_idx = CelToIdx(cel_conquer, game->level_active);
			while( (game->level_active->cell[conquer_idx].collision != CELL_WALL) && (ValidateCel(cel_conquer, game->level_active) ) )
			{
				ConquerCell(game, cel_conquer);
				cel_conquer.x++;
				cel_conquer.y++;
				conquer_idx = CelToIdx(cel_conquer, game->level_active);
			}
			break;
		}
    	case UNIT_QUEEN:
		{
			//north
			i2 cel_conquer = make_i2(game->current_move.cell_end.x, game->current_move.cell_end.y - 1);
			i32 conquer_idx = CelToIdx(cel_conquer, game->level_active);
			while( (game->level_active->cell[conquer_idx].collision != CELL_WALL) && (ValidateCel(cel_conquer, game->level_active) ) )
			{
				ConquerCell(game, cel_conquer);
				cel_conquer.y--;
				conquer_idx = CelToIdx(cel_conquer, game->level_active);
			}
			//south
			cel_conquer = make_i2(game->current_move.cell_end.x, game->current_move.cell_end.y + 1);
			conquer_idx = CelToIdx(cel_conquer, game->level_active);
			while( (game->level_active->cell[conquer_idx].collision != CELL_WALL) && (ValidateCel(cel_conquer, game->level_active) ) )
			{
				ConquerCell(game, cel_conquer);
				cel_conquer.y++;
				conquer_idx = CelToIdx(cel_conquer, game->level_active);
			}			
			//east
			cel_conquer = make_i2(game->current_move.cell_end.x + 1, game->current_move.cell_end.y);
			conquer_idx = CelToIdx(cel_conquer, game->level_active);
			while( (game->level_active->cell[conquer_idx].collision != CELL_WALL) && (ValidateCel(cel_conquer, game->level_active) ) )
			{
				ConquerCell(game, cel_conquer);
				cel_conquer.x++;
				conquer_idx = CelToIdx(cel_conquer, game->level_active);
			}
			//west
			cel_conquer = make_i2(game->current_move.cell_end.x - 1, game->current_move.cell_end.y);
			conquer_idx = CelToIdx(cel_conquer, game->level_active);
			while( (game->level_active->cell[conquer_idx].collision != CELL_WALL) && (ValidateCel(cel_conquer, game->level_active) ) )
			{
				ConquerCell(game, cel_conquer);
				cel_conquer.x--;
				conquer_idx = CelToIdx(cel_conquer, game->level_active);
			}			
			//northeast
			cel_conquer = make_i2(game->current_move.cell_end.x + 1, game->current_move.cell_end.y - 1);
			conquer_idx = CelToIdx(cel_conquer, game->level_active);
			while( (game->level_active->cell[conquer_idx].collision != CELL_WALL) && (ValidateCel(cel_conquer, game->level_active) ) )
			{
				ConquerCell(game, cel_conquer);
				cel_conquer.x++;
				cel_conquer.y--;
				conquer_idx = CelToIdx(cel_conquer, game->level_active);
			}			
			//southwest
			cel_conquer = make_i2(game->current_move.cell_end.x - 1, game->current_move.cell_end.y + 1);
			conquer_idx = CelToIdx(cel_conquer, game->level_active);
			while( (game->level_active->cell[conquer_idx].collision != CELL_WALL) && (ValidateCel(cel_conquer, game->level_active) ) )
			{
				ConquerCell(game, cel_conquer);
				cel_conquer.x--;
				cel_conquer.y++;
				conquer_idx = CelToIdx(cel_conquer, game->level_active);
			}				
			//northwest
			cel_conquer = make_i2(game->current_move.cell_end.x - 1, game->current_move.cell_end.y - 1);
			conquer_idx = CelToIdx(cel_conquer, game->level_active);
			while( (game->level_active->cell[conquer_idx].collision != CELL_WALL) && (ValidateCel(cel_conquer, game->level_active) ) )
			{
				ConquerCell(game, cel_conquer);
				cel_conquer.x--;
				cel_conquer.y--;
				conquer_idx = CelToIdx(cel_conquer, game->level_active);
			}
			//southeast
			cel_conquer = make_i2(game->current_move.cell_end.x + 1, game->current_move.cell_end.y + 1);
			conquer_idx = CelToIdx(cel_conquer, game->level_active);
			while( (game->level_active->cell[conquer_idx].collision != CELL_WALL) && (ValidateCel(cel_conquer, game->level_active) ) )
			{
				ConquerCell(game, cel_conquer);
				cel_conquer.x++;
				cel_conquer.y++;
				conquer_idx = CelToIdx(cel_conquer, game->level_active);
			}
			break;
		}
	}	

/*^^^^^^^^^^^^^^^^^^^^^^^^^^  ^^^^^^^^^^^^^^^^^^^^^^^^^^*/

//update move stack
	game->move_stack[game->last_move_idx] = malloc(sizeof(Move));
	game->move_stack[game->last_move_idx]->cell_start 	= game->current_move.cell_start;
	game->move_stack[game->last_move_idx]->cell_end		= game->current_move.cell_end;
	game->move_stack[game->last_move_idx]->unit_type 	= game->current_move.unit_type;
	for (i32 i = 0; i < MAX_CONQUERED_CELLS; i++)
	{
		game->move_stack[game->last_move_idx]->conquered_cells[i] = game->current_move.conquered_cells[i];
	}
	game->last_move_idx++;

//clear active move
	game->move_active = 0;
	game->current_move.cell_start = make_i2(-1, -1);
	game->current_move.cell_end = make_i2(-1, -1);
	game->current_move.unit_type = UNIT_NONE;

//evaluate progress
	EvaluateBoard(game, assets);

	if (game->board_invalid)
		Mix_PlayChannel(SFX_BAD_MOVE, assets->sfx[SFX_BAD_MOVE], 0);
	else
		Mix_PlayChannel(SFX_GOOD_MOVE, assets->sfx[SFX_GOOD_MOVE], 0);
}


/*---------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------*/
void UndoMove(Game* game, Assets* assets)
{
	if (game->last_move_idx > 0)
	{
//get info from movestack
		i2 undo_move_start = game->move_stack[game->last_move_idx - 1]->cell_start;
		i2 undo_move_end = game->move_stack[game->last_move_idx - 1]->cell_end;
		eUnitTypes undo_move_unit = game->move_stack[game->last_move_idx - 1]->unit_type;
		i32 idx_0 = CelToIdx(undo_move_start, game->level_active);
		i32 idx_1 = CelToIdx(undo_move_end, game->level_active);

//move piece
	//clear the tile it was placed on
		game->level_active->cell[idx_1].collision 	= CELL_FLOOR;
		game->level_active->cell[idx_1].type 		= UNIT_NONE;
		//game->level_active->cell[idx_1].status 		= CELL_STATUS_FREE;
		game->level_active->cell[idx_1].sprite_bg 	= CELL_SPR_EMPTY_TILE;
		game->level_active->cell[idx_1].sprite_mg 	= CELL_SPR_UNIT_NONE;

	//and place it back to where it was
		game->level_active->cell[idx_0].collision 	= CELL_HAS_UNIT;
		game->level_active->cell[idx_0].type 		= undo_move_unit;
		//game->level_active->cell[idx_1].status 		= CELL_STATUS_FREE;
		game->level_active->cell[idx_0].sprite_bg 	= CELL_SPR_EMPTY_TILE;
		game->level_active->cell[idx_0].sprite_mg 	= UnitTypeToSprite(undo_move_unit);

//undo conquering
		for (i32 i = 0; i < MAX_CONQUERED_CELLS; i++)
		{
			if (ValidateCel(game->move_stack[game->last_move_idx - 1]->conquered_cells[i], game->level_active))
			{
				i32 undo_conquer_idx = CelToIdx(game->move_stack[game->last_move_idx - 1]->conquered_cells[i], game->level_active);
				//game->level_active->cell[undo_conquer_idx].status--;
				if (game->level_active->cell[undo_conquer_idx].status == CELL_STATUS_IS_CONQUERED)
				{
					game->level_active->cell[undo_conquer_idx].status = CELL_STATUS_FREE;
					game->level_active->cell[undo_conquer_idx].sprite_bg = CELL_SPR_EMPTY_TILE;
				}
				else if (game->level_active->cell[undo_conquer_idx].status == CELL_STATUS_IS_DOUBLE_CONQUERED)
				{
					if ((game->level_active->cell[undo_conquer_idx].collision == CELL_HAS_UNIT))
					{
						game->level_active->cell[undo_conquer_idx].status = CELL_STATUS_FREE;
						game->level_active->cell[undo_conquer_idx].sprite_bg = CELL_SPR_EMPTY_TILE;
					}
					else
					{
						game->level_active->cell[undo_conquer_idx].status = CELL_STATUS_IS_CONQUERED;
						game->level_active->cell[undo_conquer_idx].sprite_bg = CELL_SPR_CONQUERED_TILE;
					}
				}
			}
		}

//update move stack
		free(game->move_stack[game->last_move_idx - 1]);
		game->move_stack[game->last_move_idx - 1] = NULL;
		game->last_move_idx--;
		game->move_active = 0;
		game->current_move.cell_start = make_i2(-1, -1);
		game->current_move.cell_end = make_i2(-1, -1);
		game->current_move.unit_type = UNIT_NONE;

//refresh progress
		EvaluateBoard(game, assets);

		r32 r = RNG()*3;
        Mix_PlayChannel(SFX_UNDO_01 + (i32)r, assets->sfx[SFX_UNDO_01 + (i32)r], 0);
	}
}



u8 UnitTypeToSprite(eUnitTypes type)
{
	switch(type)
	{
		case UNIT_NONE:
			return CELL_SPR_UNIT_NONE;
    	case UNIT_BASE:
			return CELL_SPR_UNIT_BASE;
    	case UNIT_BOMB:
			return CELL_SPR_UNIT_BOMB;
    	case UNIT_UNO_N:
			return CELL_SPR_UNIT_UNO_N;
    	case UNIT_UNO_NE:
			return CELL_SPR_UNIT_UNO_NE;
    	case UNIT_UNO_E:
			return CELL_SPR_UNIT_UNO_E;
    	case UNIT_UNO_SE:
			return CELL_SPR_UNIT_UNO_SE;
    	case UNIT_UNO_S:
			return CELL_SPR_UNIT_UNO_S;
    	case UNIT_UNO_SW:
			return CELL_SPR_UNIT_UNO_SW;
    	case UNIT_UNO_W:
			return CELL_SPR_UNIT_UNO_W;
    	case UNIT_UNO_NW:
			return CELL_SPR_UNIT_UNO_NW;
    	case UNIT_DUO_VERT:
			return CELL_SPR_UNIT_DUO_VERT;
    	case UNIT_DUO_DIA_R:
			return CELL_SPR_UNIT_DUO_DIA_R;
    	case UNIT_DUO_HORI:
			return CELL_SPR_UNIT_DUO_HORI;
    	case UNIT_DUO_DIA_L:
			return CELL_SPR_UNIT_DUO_DIA_L;
    	case UNIT_QUAD_ORT:
			return CELL_SPR_UNIT_QUAD_ORT;
    	case UNIT_QUAD_DIA:
			return CELL_SPR_UNIT_QUAD_DIA;
    	case UNIT_QUEEN:
			return CELL_SPR_UNIT_QUEEN;
	}
}


void SaveProgress(Game* game)
{
	FILE* savefile = fopen(SAVEFILE_PATH, "wb");
	u8* save_buffer = malloc(sizeof(u32));
	SERIALIZEu32(game->levels_cleared, save_buffer, 0);
	fwrite(save_buffer, 1, sizeof(u32), savefile);
	free(save_buffer);
	fclose(savefile);
}

void LoadProgress(Game* game)
{
	FILE* savefile = fopen(SAVEFILE_PATH, "rb");
	u8* save_buffer = malloc(sizeof(u32));
	fread(save_buffer, 1, sizeof(u32), savefile);
	DESERIALIZEu32(save_buffer, 0, &game->levels_cleared);
	free(save_buffer);
	fclose(savefile);
}