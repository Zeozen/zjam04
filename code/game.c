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


	RestartGame(new_game);
    return new_game;
}

void FreeGame(Game* game)
{
    if (game != NULL)
        free(game);
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
			switch(lvl_string_raw[x])
			{
				case '.':
				{
					game->levels[level_number]->cell[x + y * width].collision 	= CELL_STATUS_WALL;
					game->levels[level_number]->cell[x + y * width].type 		= UNIT_NONE;
					game->levels[level_number]->cell[x + y * width].sprite_bg 	= CELL_SPR_WALL_TILE;
					game->levels[level_number]->cell[x + y * width].sprite_mg 	= CELL_SPR_UNIT_NONE;
					game->levels[level_number]->cell[x + y * width].sprite_fg 	= CELL_SPR_TRANSPARENT;
					break;
				}
				case '0':
				{
					game->levels[level_number]->cell[x + y * width].collision 	= CELL_STATUS_EMPTY;
					game->levels[level_number]->cell[x + y * width].type 		= UNIT_NONE;
					game->levels[level_number]->cell[x + y * width].sprite_bg 	= CELL_SPR_EMPTY_TILE;
					game->levels[level_number]->cell[x + y * width].sprite_mg 	= CELL_SPR_UNIT_NONE;
					game->levels[level_number]->cell[x + y * width].sprite_fg 	= CELL_SPR_TRANSPARENT;
					break;
				}
				case 'o':
				{
					game->levels[level_number]->cell[x + y * width].collision 	= CELL_STATUS_HAS_UNIT;
					game->levels[level_number]->cell[x + y * width].type 		= UNIT_BASE;
					game->levels[level_number]->cell[x + y * width].sprite_bg 	= CELL_SPR_EMPTY_TILE;
					game->levels[level_number]->cell[x + y * width].sprite_mg 	= CELL_SPR_UNIT_BASE;
					game->levels[level_number]->cell[x + y * width].sprite_fg 	= CELL_SPR_TRANSPARENT;
					break;
				}
				case '@':
				{
					game->levels[level_number]->cell[x + y * width].collision 	= CELL_STATUS_HAS_UNIT;
					game->levels[level_number]->cell[x + y * width].type 		= UNIT_BOMB;
					game->levels[level_number]->cell[x + y * width].sprite_bg 	= CELL_SPR_EMPTY_TILE;
					game->levels[level_number]->cell[x + y * width].sprite_mg 	= CELL_SPR_UNIT_BOMB;
					game->levels[level_number]->cell[x + y * width].sprite_fg 	= CELL_SPR_TRANSPARENT;
					break;
				}
				case '^':
				{
					game->levels[level_number]->cell[x + y * width].collision 	= CELL_STATUS_HAS_UNIT;
					game->levels[level_number]->cell[x + y * width].type 		= UNIT_UNO_N;
					game->levels[level_number]->cell[x + y * width].sprite_bg 	= CELL_SPR_EMPTY_TILE;
					game->levels[level_number]->cell[x + y * width].sprite_mg 	= CELL_SPR_UNIT_UNO_N;
					game->levels[level_number]->cell[x + y * width].sprite_fg 	= CELL_SPR_TRANSPARENT;
					break;
				}
				case 'n':
				{
					game->levels[level_number]->cell[x + y * width].collision 	= CELL_STATUS_HAS_UNIT;
					game->levels[level_number]->cell[x + y * width].type 		= UNIT_UNO_NE;
					game->levels[level_number]->cell[x + y * width].sprite_bg 	= CELL_SPR_EMPTY_TILE;
					game->levels[level_number]->cell[x + y * width].sprite_mg 	= CELL_SPR_UNIT_UNO_NE;
					game->levels[level_number]->cell[x + y * width].sprite_fg 	= CELL_SPR_TRANSPARENT;
					break;
				}
				case '>':
				{
					game->levels[level_number]->cell[x + y * width].collision 	= CELL_STATUS_HAS_UNIT;
					game->levels[level_number]->cell[x + y * width].type 		= UNIT_UNO_E;
					game->levels[level_number]->cell[x + y * width].sprite_bg 	= CELL_SPR_EMPTY_TILE;
					game->levels[level_number]->cell[x + y * width].sprite_mg 	= CELL_SPR_UNIT_UNO_E;
					game->levels[level_number]->cell[x + y * width].sprite_fg 	= CELL_SPR_TRANSPARENT;
					break;
				}
				case 'e':
				{
					game->levels[level_number]->cell[x + y * width].collision 	= CELL_STATUS_HAS_UNIT;
					game->levels[level_number]->cell[x + y * width].type 		= UNIT_UNO_SE;
					game->levels[level_number]->cell[x + y * width].sprite_bg 	= CELL_SPR_EMPTY_TILE;
					game->levels[level_number]->cell[x + y * width].sprite_mg 	= CELL_SPR_UNIT_UNO_SE;
					game->levels[level_number]->cell[x + y * width].sprite_fg 	= CELL_SPR_TRANSPARENT;
					break;
				}
				case 'v':
				{
					game->levels[level_number]->cell[x + y * width].collision 	= CELL_STATUS_HAS_UNIT;
					game->levels[level_number]->cell[x + y * width].type 		= UNIT_UNO_S;
					game->levels[level_number]->cell[x + y * width].sprite_bg 	= CELL_SPR_EMPTY_TILE;
					game->levels[level_number]->cell[x + y * width].sprite_mg 	= CELL_SPR_UNIT_UNO_S;
					game->levels[level_number]->cell[x + y * width].sprite_fg 	= CELL_SPR_TRANSPARENT;
					break;
				}
				case 's':
				{
					game->levels[level_number]->cell[x + y * width].collision 	= CELL_STATUS_HAS_UNIT;
					game->levels[level_number]->cell[x + y * width].type 		= UNIT_UNO_SW;
					game->levels[level_number]->cell[x + y * width].sprite_bg 	= CELL_SPR_EMPTY_TILE;
					game->levels[level_number]->cell[x + y * width].sprite_mg 	= CELL_SPR_UNIT_UNO_SW;
					game->levels[level_number]->cell[x + y * width].sprite_fg 	= CELL_SPR_TRANSPARENT;
					break;
				}
				case '<':
				{
					game->levels[level_number]->cell[x + y * width].collision 	= CELL_STATUS_HAS_UNIT;
					game->levels[level_number]->cell[x + y * width].type 		= UNIT_UNO_W;
					game->levels[level_number]->cell[x + y * width].sprite_bg 	= CELL_SPR_EMPTY_TILE;
					game->levels[level_number]->cell[x + y * width].sprite_mg 	= CELL_SPR_UNIT_UNO_W;
					game->levels[level_number]->cell[x + y * width].sprite_fg 	= CELL_SPR_TRANSPARENT;
					break;
				}
				case 'w':
				{
					game->levels[level_number]->cell[x + y * width].collision 	= CELL_STATUS_HAS_UNIT;
					game->levels[level_number]->cell[x + y * width].type 		= UNIT_UNO_NW;
					game->levels[level_number]->cell[x + y * width].sprite_bg 	= CELL_SPR_EMPTY_TILE;
					game->levels[level_number]->cell[x + y * width].sprite_mg 	= CELL_SPR_UNIT_UNO_NW;
					game->levels[level_number]->cell[x + y * width].sprite_fg 	= CELL_SPR_TRANSPARENT;
					break;
				}
				case '|':
				{
					game->levels[level_number]->cell[x + y * width].collision 	= CELL_STATUS_HAS_UNIT;
					game->levels[level_number]->cell[x + y * width].type 		= UNIT_DUO_VERT;
					game->levels[level_number]->cell[x + y * width].sprite_bg 	= CELL_SPR_EMPTY_TILE;
					game->levels[level_number]->cell[x + y * width].sprite_mg 	= CELL_SPR_UNIT_DUO_VERT;
					game->levels[level_number]->cell[x + y * width].sprite_fg 	= CELL_SPR_TRANSPARENT;
					break;
				}
				case 'r':
				{
					game->levels[level_number]->cell[x + y * width].collision 	= CELL_STATUS_HAS_UNIT;
					game->levels[level_number]->cell[x + y * width].type 		= UNIT_DUO_DIA_R;
					game->levels[level_number]->cell[x + y * width].sprite_bg 	= CELL_SPR_EMPTY_TILE;
					game->levels[level_number]->cell[x + y * width].sprite_mg 	= CELL_SPR_UNIT_DUO_DIA_R;
					game->levels[level_number]->cell[x + y * width].sprite_fg 	= CELL_SPR_TRANSPARENT;
					break;
				}
				case '-':
				{
					game->levels[level_number]->cell[x + y * width].collision 	= CELL_STATUS_HAS_UNIT;
					game->levels[level_number]->cell[x + y * width].type 		= UNIT_DUO_HORI;
					game->levels[level_number]->cell[x + y * width].sprite_bg 	= CELL_SPR_EMPTY_TILE;
					game->levels[level_number]->cell[x + y * width].sprite_mg 	= CELL_SPR_UNIT_DUO_HORI;
					game->levels[level_number]->cell[x + y * width].sprite_fg 	= CELL_SPR_TRANSPARENT;
					break;
				}
				case 'l':
				{
					game->levels[level_number]->cell[x + y * width].collision 	= CELL_STATUS_HAS_UNIT;
					game->levels[level_number]->cell[x + y * width].type 		= UNIT_DUO_DIA_L;
					game->levels[level_number]->cell[x + y * width].sprite_bg 	= CELL_SPR_EMPTY_TILE;
					game->levels[level_number]->cell[x + y * width].sprite_mg 	= CELL_SPR_UNIT_DUO_DIA_L;
					game->levels[level_number]->cell[x + y * width].sprite_fg 	= CELL_SPR_TRANSPARENT;
					break;
				}
				case '+':
				{
					game->levels[level_number]->cell[x + y * width].collision 	= CELL_STATUS_HAS_UNIT;
					game->levels[level_number]->cell[x + y * width].type 		= UNIT_QUAD_ORT;
					game->levels[level_number]->cell[x + y * width].sprite_bg 	= CELL_SPR_EMPTY_TILE;
					game->levels[level_number]->cell[x + y * width].sprite_mg 	= CELL_SPR_UNIT_QUAD_ORT;
					game->levels[level_number]->cell[x + y * width].sprite_fg 	= CELL_SPR_TRANSPARENT;
					break;
				}
				case 'x':
				{
					game->levels[level_number]->cell[x + y * width].collision 	= CELL_STATUS_HAS_UNIT;
					game->levels[level_number]->cell[x + y * width].type 		= UNIT_QUAD_DIA;
					game->levels[level_number]->cell[x + y * width].sprite_bg 	= CELL_SPR_EMPTY_TILE;
					game->levels[level_number]->cell[x + y * width].sprite_mg 	= CELL_SPR_UNIT_QUAD_DIA;
					game->levels[level_number]->cell[x + y * width].sprite_fg 	= CELL_SPR_TRANSPARENT;
					break;
				}
				case '*':
				{
					game->levels[level_number]->cell[x + y * width].collision 	= CELL_STATUS_HAS_UNIT;
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


/*GAME SPECIFICS*/
void StartMove(Game* game, i2 cell_start)
{
	game->move_active = 1;
	game->current_move.cell_start = cell_start;
	game->current_move.unit_type = game->level_active->cell[CelToIdx(cell_start, game->level_active)].type;
}

void CancelMove(Game* game)
{
	game->move_active = 0;
	game->current_move.cell_start = make_i2(-1, -1);
	game->current_move.unit_type = UNIT_NONE;
}

void PerformMove(Game* game, i2 cell_end)
{
	game->current_move.cell_end = cell_end;
	game->move_stack[game->last_move_idx] = malloc(sizeof(Move));
	game->move_stack[game->last_move_idx]->cell_start 	= game->current_move.cell_start;
	game->move_stack[game->last_move_idx]->cell_end		= game->current_move.cell_end;
	game->move_stack[game->last_move_idx]->unit_type 	= game->current_move.unit_type;
	game->last_move_idx++;

	i32 idx_0 = CelToIdx(game->current_move.cell_start, game->level_active);
	i32 idx_1 = CelToIdx(game->current_move.cell_end, game->level_active);

	game->level_active->cell[idx_0].collision = CELL_STATUS_EMPTY;
	game->level_active->cell[idx_0].type = UNIT_NONE;
	game->level_active->cell[idx_0].sprite_mg = CELL_SPR_UNIT_NONE;

	UpdatePositionAndPerformConquer(game, game->current_move.cell_start, game->current_move.cell_end, game->current_move.unit_type);

	game->move_active = 0;
	game->current_move.cell_start = make_i2(-1, -1);
	game->current_move.cell_end = make_i2(-1, -1);
	game->current_move.unit_type = UNIT_NONE;

	EvaluateBoard(game);
}

void UndoMove(Game* game)
{
	if (game->last_move_idx > 0)
	{
		i32 idx_0 = CelToIdx(game->move_stack[game->last_move_idx - 1]->cell_start, game->level_active);
		i32 idx_1 = CelToIdx(game->move_stack[game->last_move_idx - 1]->cell_end, game->level_active);

		game->level_active->cell[idx_0].collision = CELL_STATUS_HAS_UNIT;
		game->level_active->cell[idx_0].type = game->move_stack[game->last_move_idx - 1]->unit_type;
		game->level_active->cell[idx_0].sprite_mg = UnitTypeToSprite(game->move_stack[game->last_move_idx - 1]->unit_type);
		game->level_active->cell[idx_0].sprite_bg = CELL_SPR_EMPTY_TILE;

		game->level_active->cell[idx_1].collision = CELL_STATUS_EMPTY;
		game->level_active->cell[idx_1].type = UNIT_NONE;
		game->level_active->cell[idx_1].sprite_bg = CELL_SPR_EMPTY_TILE;
		game->level_active->cell[idx_1].sprite_mg = CELL_SPR_UNIT_NONE;

		UndoConquer(game, game->move_stack[game->last_move_idx - 1]->cell_start, game->move_stack[game->last_move_idx - 1]->cell_end, game->move_stack[game->last_move_idx - 1]->unit_type);

		EvaluateBoard(game);

		free(game->move_stack[game->last_move_idx - 1]);
		game->last_move_idx--;
		game->move_active = 0;
		game->current_move.cell_start = make_i2(-1, -1);
		game->current_move.cell_end = make_i2(-1, -1);
		game->current_move.unit_type = UNIT_NONE;
	}
}



void EvaluateBoard(Game* game)
{
	i32 cell_ok_counter = 0;
	i32 bad_counter = 0;

	for (i32 y = 0; y < game->level_active->height; y++)
	{
		for (i32 x = 0; x < game->level_active->width; x++)
		{
			i32 idx = x + y * game->level_active->width;
			u8 status = game->level_active->cell[idx].collision;

			if( (status == CELL_STATUS_IS_CONQUERED) || (status == CELL_STATUS_HAS_UNIT_MOVED) || (status == CELL_STATUS_WALL) )
			{
				//everything appears to be in order, continue
				cell_ok_counter++;
			}
			if (status == CELL_STATUS_IS_DOUBLE_CONQUERED)
			{
				bad_counter++;
			}
		}
	}

	if (cell_ok_counter == (game->level_active->height * game->level_active->width))
	{
		game->board_cleared = 1;
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

void RestartLevel(Game* game)
{
	CloseLevel(game);
	OpenLevel(game, game->current_level_number);
	for (i32 i = 0; i < game->last_move_idx; i++)
	{
		if (game->move_stack[i] != NULL)
		{
			free(game->move_stack[i]);
			game->move_stack[i] = NULL;
		}
	}
	game->last_move_idx = 0;
	game->board_invalid = 0;
}

void CloseLevel(Game* game)
{
	if (game->level_active != NULL)
	{
		FreeGrid(game->level_active);
		game->level_active = NULL;
	}
}

void OpenLevel(Game* game, u32 level_number)
{
	game->level_active = CreateGrid(game->levels[game->current_level_number]->width, game->levels[game->current_level_number]->height, game->levels[game->current_level_number]->origin);
	for (i32 y = 0; y < game->level_active->height; y++)
	{
		for (i32 x = 0; x < game->level_active->width; x++)
		{
			game->level_active->cell[x + y * game->level_active->width].collision = game->levels[game->current_level_number]->cell[x + y * game->level_active->width].collision;
			game->level_active->cell[x + y * game->level_active->width].type = game->levels[game->current_level_number]->cell[x + y * game->level_active->width].type;
			game->level_active->cell[x + y * game->level_active->width].id = game->levels[game->current_level_number]->cell[x + y * game->level_active->width].id;
			game->level_active->cell[x + y * game->level_active->width].sprite_bg = game->levels[game->current_level_number]->cell[x + y * game->level_active->width].sprite_bg;
			game->level_active->cell[x + y * game->level_active->width].sprite_mg = game->levels[game->current_level_number]->cell[x + y * game->level_active->width].sprite_mg;
			game->level_active->cell[x + y * game->level_active->width].sprite_fg = game->levels[game->current_level_number]->cell[x + y * game->level_active->width].sprite_fg;
		}
	}
	game->board_invalid = 0;
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
		return CELL_SPR_UNIT_DUO_DIA_R;
    	case UNIT_QUAD_ORT:
		return CELL_SPR_UNIT_QUAD_ORT;
    	case UNIT_QUAD_DIA:
		return CELL_SPR_UNIT_QUAD_DIA;
    	case UNIT_QUEEN:
		return CELL_SPR_UNIT_QUEEN;
	}
}

void UpdatePositionAndPerformConquer(Game* game, i2 move_start, i2 move_end, eUnitTypes unit)
{
	i32 idx_0 = CelToIdx(move_start, game->level_active);
	i32 idx_1 = CelToIdx(move_end, game->level_active);
	
	game->level_active->cell[idx_1].collision = CELL_STATUS_HAS_UNIT_MOVED;
	game->level_active->cell[idx_1].type = unit;
	game->level_active->cell[idx_1].sprite_bg = CELL_SPR_CONQUERED_TILE;
	game->level_active->cell[idx_1].sprite_mg = UnitTypeToSprite(unit);
	switch(unit)
	{
		case UNIT_NONE:
		{
			printf("~~~ NOTICE! Something wrong happened, and we're trying to perform a conquer with a NONE unit!! ~~~\n");
			break;
		}
    	case UNIT_BASE:
		{
			//no conquering, but we're counting where it lands as being "conquered" through "has_unit_moved"
			break;
		}
    	case UNIT_BOMB:
		{
			game->level_active->cell[idx_0].collision = CELL_STATUS_IS_CONQUERED;
			game->level_active->cell[idx_0].sprite_bg = CELL_SPR_CONQUERED_TILE;
			break;
		}
    	case UNIT_UNO_N:
		{
			i32 i = idx_1 - game->level_active->width;
			while(game->level_active->cell[i].collision != CELL_STATUS_WALL)
			break;
		}
    	case UNIT_UNO_NE:
		{
			break;
		}
    	case UNIT_UNO_E:
		{
			break;
		}
    	case UNIT_UNO_SE:
		{
			break;
		}
    	case UNIT_UNO_S:
		{
			break;
		}
    	case UNIT_UNO_SW:
		{
			break;
		}
    	case UNIT_UNO_W:
		{
			break;
		}
    	case UNIT_UNO_NW:
		{
			break;
		}
    	case UNIT_DUO_VERT:
		{
			break;
		}
    	case UNIT_DUO_DIA_R:
		{
			break;
		}
    	case UNIT_DUO_HORI:
		{
			break;
		}
    	case UNIT_DUO_DIA_L:
		{
			break;
		}
    	case UNIT_QUAD_ORT:
		{
			break;
		}
    	case UNIT_QUAD_DIA:
		{
			break;
		}
    	case UNIT_QUEEN:
		{
			break;
		}
	}	
}

void UndoConquer(Game* game, i2 move_start, i2 move_end, eUnitTypes unit)
{
	i32 idx_0 = CelToIdx(move_start, game->level_active);
	i32 idx_1 = CelToIdx(move_end, game->level_active);
	switch(unit)
	{
		case UNIT_NONE:
		{
			printf("~~~ NOTICE! Something wrong happened, and we're trying to UNDO a conquer with a NONE unit!! ~~~\n");
			break;
		}
    	case UNIT_BASE:
		{
			//no conquering, so no undo of board except for movement (already performed in undo move) necessary
			break;
		}
    	case UNIT_BOMB:
		{
			//should un-conquer the cell it started on, in this case, it is handled when setting the cell status to HAS_UNIT in UndoMove() which overwrites cell status CONQUERED, but we should update the sprite too
			game->level_active->cell[idx_0].sprite_bg = CELL_SPR_EMPTY_TILE;
			break;
		}
    	case UNIT_UNO_N:
		{
			break;
		}
    	case UNIT_UNO_NE:
		{
			break;
		}
    	case UNIT_UNO_E:
		{
			break;
		}
    	case UNIT_UNO_SE:
		{
			break;
		}
    	case UNIT_UNO_S:
		{
			break;
		}
    	case UNIT_UNO_SW:
		{
			break;
		}
    	case UNIT_UNO_W:
		{
			break;
		}
    	case UNIT_UNO_NW:
		{
			break;
		}
    	case UNIT_DUO_VERT:
		{
			break;
		}
    	case UNIT_DUO_DIA_R:
		{
			break;
		}
    	case UNIT_DUO_HORI:
		{
			break;
		}
    	case UNIT_DUO_DIA_L:
		{
			break;
		}
    	case UNIT_QUAD_ORT:
		{
			break;
		}
    	case UNIT_QUAD_DIA:
		{
			break;
		}
    	case UNIT_QUEEN:
		{
			break;
		}
	}	
}