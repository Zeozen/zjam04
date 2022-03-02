#ifndef GAME_H
#define GAME_H

#include "zmath.h"
#include "zgrid.h"

//UNIT TYPES, stores in zgrid->cell->type
typedef enum eUnitTypes
{
    UNIT_NONE,
    UNIT_BASE,
    UNIT_BOMB,
    UNIT_UNO_N,
    UNIT_UNO_NE,
    UNIT_UNO_E,
    UNIT_UNO_SE,
    UNIT_UNO_S,
    UNIT_UNO_SW,
    UNIT_UNO_W,
    UNIT_UNO_NW,
    UNIT_DUO_VERT,
    UNIT_DUO_DIA_R,
    UNIT_DUO_HORI,
    UNIT_DUO_DIA_L,
    UNIT_QUAD_ORT,
    UNIT_QUAD_DIA,
    UNIT_QUEEN,
} eUnitTypes;

typedef struct 
{
    i2 cell_start;
    i2 cell_end;
    eUnitTypes unit_type;
} Move;

#define NUMBER_OF_GAMESTATES 9
typedef enum
{
    GAMESTATE_INIT,
    GAMESTATE_MAIN,
    GAMESTATE_OPTS,
    GAMESTATE_PLAY,
    GAMESTATE_EVNT,
    GAMESTATE_LOSE,
    GAMESTATE_GOAL,
    GAMESTATE_EDIT,
    GAMESTATE_EXIT,
} Gamestate;

#define MAX_LEVELS 32
#define MAX_UNDOS 256
typedef struct Game
{
    u32 levels_cleared;
    u32 current_level_number;
    u32 move_active;
    u32 board_invalid;
    u32 board_cleared;
    Move current_move;
    Move* move_stack[MAX_UNDOS];
    u32 last_move_idx;
    zGrid* levels[MAX_LEVELS];
    zGrid* level_active;
} Game;

//CELL STATUS, stores in zgrid->cell->collision
typedef enum eCellStatus
{
    CELL_STATUS_WALL,
    CELL_STATUS_EMPTY,
    CELL_STATUS_HAS_UNIT,
    CELL_STATUS_HAS_UNIT_MOVED,
    CELL_STATUS_IS_CONQUERED,
    CELL_STATUS_IS_DOUBLE_CONQUERED,
} eCellStatus;

//CELL SPRITES
#define CELL_SPR_TRANSPARENT                  0b00000000
#define CELL_SPR_WALL_TILE                    0b00000001
#define CELL_SPR_EMPTY_TILE                   0b00000010
#define CELL_SPR_CONQUERED_TILE               0b00000011
#define CELL_SPR_DOUBLE_CONQUERED_TILE        0b00000100
#define CELL_SPR_CONQUER_PREVIEW_OVERLAY_TILE 0b00000101
#define CELL_SPR_SELECTION_OVERLAY_TILE       0b00000111
#define CELL_SPR_UNIT_NONE                    0b00000000
#define CELL_SPR_UNIT_BASE                    0b00010000
#define CELL_SPR_UNIT_BOMB                    0b00010001
#define CELL_SPR_UNIT_UNO_N                   0b00010010
#define CELL_SPR_UNIT_UNO_NE                  0b00010011
#define CELL_SPR_UNIT_UNO_E                   0b00010100
#define CELL_SPR_UNIT_UNO_SE                  0b00010101
#define CELL_SPR_UNIT_UNO_S                   0b00010110
#define CELL_SPR_UNIT_UNO_SW                  0b00010111
#define CELL_SPR_UNIT_UNO_W                   0b00100000
#define CELL_SPR_UNIT_UNO_NW                  0b00100001
#define CELL_SPR_UNIT_DUO_VERT                0b00100010
#define CELL_SPR_UNIT_DUO_DIA_R               0b00100011
#define CELL_SPR_UNIT_DUO_HORI                0b00100100
#define CELL_SPR_UNIT_DUO_DIA_L               0b00100101
#define CELL_SPR_UNIT_QUAD_ORT                0b00100110
#define CELL_SPR_UNIT_QUAD_DIA                0b00100111
#define CELL_SPR_UNIT_QUEEN                   0b00110000



char* GetGamestateName(Gamestate state);

Game* CreateGame();
void FreeGame(Game* game);
void RestartGame(Game* game);

void LoadLevel(Game* game, u32 level_number, const char* level_name);

void StartMove(Game* game, i2 cell_start);
void CancelMove(Game* game);
void PerformMove(Game* game, i2 cell_end);
void UndoMove(Game* game);
void RestartLevel(Game* game);
void CloseLevel(Game* game);
void OpenLevel(Game* game, u32 level_number);
u8 UnitTypeToSprite(eUnitTypes type);
void UpdatePositionAndPerformConquer(Game* game, i2 move_start, i2 move_end, eUnitTypes unit);
void UndoConquer(Game* game, i2 move_start, i2 move_end, eUnitTypes unit);
void EvaluateBoard(Game* game);


#endif // GAME_H
