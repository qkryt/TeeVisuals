// TeeVisuals Configuration Variables
#ifndef MACRO_CONFIG_INT
#error "The config macros must be defined"
#define MACRO_CONFIG_INT(Name, ScriptName, Def, Min, Max, Save, Desc) ;
#define MACRO_CONFIG_COL(Name, ScriptName, Def, Save, Desc) ;
#define MACRO_CONFIG_STR(Name, ScriptName, Len, Def, Save, Desc) ;
#endif

// Dynamic Island HUD
MACRO_CONFIG_INT(TvIsland, cl_tv_island, 1, 0, 1, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Enable TeeVisuals Dynamic Island HUD")
MACRO_CONFIG_INT(TvIslandScale, cl_tv_island_scale, 100, 50, 200, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Scale of Dynamic Island HUD in percent")
MACRO_CONFIG_INT(TvIslandPosY, cl_tv_island_pos_y, 10, 0, 500, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Y offset for Dynamic Island HUD")
MACRO_CONFIG_INT(TvIslandBgAlpha, cl_tv_island_bg_alpha, 80, 0, 100, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Dynamic Island glassmorphism background opacity")
MACRO_CONFIG_INT(TvIslandTimerMode, cl_tv_island_timer_mode, 0, 0, 1, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Dynamic Island clock mode (0: system time, 1: session/race timer)")

// Rainbow Mode
MACRO_CONFIG_INT(TvRainbowBody, cl_tv_rainbow_body, 0, 0, 1, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Rainbow color for tee body")
MACRO_CONFIG_INT(TvRainbowFeet, cl_tv_rainbow_feet, 0, 0, 1, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Rainbow color for tee feet")
MACRO_CONFIG_INT(TvRainbowWeapon, cl_tv_rainbow_weapon, 0, 0, 1, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Rainbow color for active weapon")
MACRO_CONFIG_INT(TvRainbowHook, cl_tv_rainbow_hook, 0, 0, 1, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Rainbow color for hook line")
MACRO_CONFIG_INT(TvRainbowSpeed, cl_tv_rainbow_speed, 50, 1, 200, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Speed of rainbow color cycling")
MACRO_CONFIG_INT(TvRainbowSat, cl_tv_rainbow_sat, 100, 0, 100, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Saturation of rainbow effect")
MACRO_CONFIG_INT(TvRainbowVal, cl_tv_rainbow_val, 100, 0, 100, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Brightness/value of rainbow effect")

// Player Trails
MACRO_CONFIG_INT(TvTrails, cl_tv_trails, 1, 0, 2, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Player movement trails (0: off, 1: self only, 2: all players)")
MACRO_CONFIG_INT(TvTrailLength, cl_tv_trail_length, 30, 5, 100, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Number of segments in player trail")
MACRO_CONFIG_INT(TvTrailWidth, cl_tv_trail_width, 8, 2, 32, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Width of player trail ribbon")
MACRO_CONFIG_INT(TvTrailMode, cl_tv_trail_mode, 1, 0, 2, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Trail color mode (0: static color, 1: orange-yellow gradient, 2: rainbow)")
MACRO_CONFIG_COL(TvTrailColor, cl_tv_trail_color, 0xFF8C00, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Static trail color")

// Freeze & Off-Screen HUD
MACRO_CONFIG_INT(TvFreezeIndicator, cl_tv_freeze_indicator, 1, 0, 1, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Show freeze indicators and timers above frozen players")
MACRO_CONFIG_INT(TvOffscreenHud, cl_tv_offscreen_hud, 1, 0, 1, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Show off-screen player indicators")
MACRO_CONFIG_INT(TvOffscreenRange, cl_tv_offscreen_range, 3000, 500, 15000, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Max distance for off-screen radar tracking")
MACRO_CONFIG_INT(TvOffscreenTeammates, cl_tv_offscreen_teammates, 1, 0, 1, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Track off-screen teammates")
MACRO_CONFIG_INT(TvOffscreenWar, cl_tv_offscreen_war, 1, 0, 1, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Track off-screen war-listed players")

// Social / Player Lists
MACRO_CONFIG_INT(TvPlayerLists, cl_tv_player_lists, 1, 0, 1, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Enable TeeVisuals player lists (War, Team, Helper)")
MACRO_CONFIG_INT(TvPlayerListsScoreboard, cl_tv_player_lists_scoreboard, 1, 0, 1, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Highlight list players in scoreboard")
MACRO_CONFIG_INT(TvPlayerListsNameplates, cl_tv_player_lists_nameplates, 1, 0, 1, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Highlight list players on nameplates")

// Smooth Bindwheel
MACRO_CONFIG_INT(TvSmoothBindwheel, cl_tv_smooth_bindwheel, 1, 0, 1, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Enable Smooth Bindwheel radial menu")
MACRO_CONFIG_INT(TvBindwheelRadius, cl_tv_bindwheel_radius, 160, 60, 400, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Radius of Smooth Bindwheel")

// UI Customization
MACRO_CONFIG_STR(TvMenuBg, cl_tv_menu_bg, 256, "", CFGFLAG_SAVE | CFGFLAG_CLIENT, "Custom main menu background image path")
MACRO_CONFIG_INT(TvMenuBgDim, cl_tv_menu_bg_dim, 30, 0, 100, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Darken overlay for custom menu background in percent")
MACRO_CONFIG_STR(TvCustomFont, cl_tv_custom_font, 256, "", CFGFLAG_SAVE | CFGFLAG_CLIENT, "Custom font file name in data/fonts/")
