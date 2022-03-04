#ifndef ASSETS_H
#define ASSETS_H

//font
#define FONT_ID_ZSYS 0
#define FONT_PATH_ZSYS "./assets/font/font_zsys.png"

//texture aliases
#define T_TILE_ATLAS 0
#define T_UI_ATLAS 1
#define T_PLAYER_ATLAS 2
#define T_PLAYER_CURSOR 3
#define T_BG_ATLAS 4
//texture paths
#define T_TILE_ATLAS_PATH    "./assets/texture/game.png"
#define T_UI_ATLAS_PATH      "./assets/texture/gui.png"
#define T_PLAYER_CURSOR_PATH "./assets/cursor/cur_plr.png"
#define T_BG_ATLAS_PATH "./assets/texture/bg.png"

//cursors
#define CUR_POINT 0
#define CUR_CLICK 1
#define CUR_HAND 2
#define CUR_GRAB 3 
#define CUR_CROSS 4
//cursorpaths
#define CUR_PATH_POINT "./assets/cursor/cur_zsys_point.png"
#define CUR_PATH_CLICK "./assets/cursor/cur_zsys_click.png"
#define CUR_PATH_HAND "./assets/cursor/cur_zsys_hand.png"
#define CUR_PATH_GRAB "./assets/cursor/cur_zsys_grab.png"
#define CUR_PATH_CROSS "./assets/cursor/cur_zsys_cross.png"

#define MUS_BGM 0

//sound effect aliases
typedef enum
{
    SFX_HOVER,
    SFX_SELECT,
    SFX_TAP_01,
    SFX_TAP_02,
    SFX_TAP_03,
    SFX_UNDO_01,
    SFX_UNDO_02,
    SFX_UNDO_03,
    SFX_OPEN_LEVEL,
    SFX_LEVEL_COMPLETE,
    SFX_GOOD_MOVE,
    SFX_BAD_MOVE,
    SFX_PICKUP,
    SFX_DROP,
} SoundEffects;
//paths
#define SFX_PATH_HOVER "./assets/sound/hover.wav"
#define SFX_PATH_SELECT "./assets/sound/select.wav"
#define SFX_PATH_TAP_01 "./assets/sound/tap_01.wav"
#define SFX_PATH_TAP_02 "./assets/sound/tap_02.wav"
#define SFX_PATH_TAP_03 "./assets/sound/tap_03.wav"
#define SFX_PATH_UNDO_01 "./assets/sound/undo_01.wav"
#define SFX_PATH_UNDO_02 "./assets/sound/undo_02.wav"
#define SFX_PATH_UNDO_03 "./assets/sound/undo_03.wav"
#define SFX_PATH_OPEN_LEVEL "./assets/sound/open_level.wav"
#define SFX_PATH_LEVEL_COMPLETE "./assets/sound/complete_board.wav"
#define SFX_PATH_GOOD_MOVE "./assets/sound/move_good.wav"
#define SFX_PATH_BAD_MOVE "./assets/sound/move_bad.wav"
#define SFX_PATH_PICKUP "./assets/sound/pickup.wav"
#define SFX_PATH_DROP "./assets/sound/drop.wav"

//strings
#define STR_UNUSED_A
#define STR_UNUSED_B
#define STR_UNUSED_C
#define STR_LEVEL_IDX_0 4


#define GUI_DESIGN_PATH "./assets/design/gui.ini"
#define LEVEL_DESIGN_PATH "./assets/design/levels.ini"
#define SAVEFILE_PATH "./assets/data/save.bin"

#endif //ASSETS_H