// TeeVisuals Configuration Variables
#ifndef MACRO_CONFIG_INT
#error "The config macros must be defined"
#define MACRO_CONFIG_INT(Name, ScriptName, Def, Min, Max, Save, Desc) ;
#define MACRO_CONFIG_COL(Name, ScriptName, Def, Save, Desc) ;
#define MACRO_CONFIG_STR(Name, ScriptName, Len, Def, Save, Desc) ;
#endif

// Dynamic Island HUD
MACRO_CONFIG_INT(ClTvIsland, cl_tv_island, 1, 0, 1, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Enable TeeVisuals Dynamic Island HUD")
MACRO_CONFIG_INT(ClTvIslandScale, cl_tv_island_scale, 100, 50, 200, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Scale of Dynamic Island HUD in percent")
MACRO_CONFIG_INT(ClTvIslandX, cl_tv_island_x, 0, -1000, 1000, CFGFLAG_SAVE | CFGFLAG_CLIENT, "X offset for Dynamic Island HUD")
MACRO_CONFIG_INT(ClTvIslandY, cl_tv_island_y, 10, 0, 500, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Y offset for Dynamic Island HUD")
MACRO_CONFIG_INT(ClTvIslandPosY, cl_tv_island_pos_y, 10, 0, 500, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Legacy Y offset for Dynamic Island HUD")
MACRO_CONFIG_INT(ClTvIslandAlpha, cl_tv_island_alpha, 80, 0, 100, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Dynamic Island glassmorphism background opacity")
MACRO_CONFIG_INT(ClTvIslandBgAlpha, cl_tv_island_bg_alpha, 80, 0, 100, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Legacy background opacity")
MACRO_CONFIG_INT(ClTvIslandTimeMode, cl_tv_island_time_mode, 0, 0, 2, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Clock mode (0: system time, 1: session timer, 2: race timer)")
MACRO_CONFIG_INT(ClTvIslandTimerMode, cl_tv_island_timer_mode, 0, 0, 2, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Legacy clock mode")
MACRO_CONFIG_INT(ClTvIslandGlow, cl_tv_island_glow, 1, 0, 1, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Enable border glow on Dynamic Island HUD")

// Rainbow Mode
MACRO_CONFIG_INT(ClTvRainbow, cl_tv_rainbow, 0, 0, 1, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Master switch for HSV Rainbow mode")
MACRO_CONFIG_INT(ClTvRainbowBody, cl_tv_rainbow_body, 0, 0, 1, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Rainbow color for tee body")
MACRO_CONFIG_INT(ClTvRainbowFeet, cl_tv_rainbow_feet, 0, 0, 1, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Rainbow color for tee feet")
MACRO_CONFIG_INT(ClTvRainbowWeapon, cl_tv_rainbow_weapon, 0, 0, 1, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Rainbow color for active weapon")
MACRO_CONFIG_INT(ClTvRainbowHook, cl_tv_rainbow_hook, 0, 0, 1, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Rainbow color for hook line")
MACRO_CONFIG_INT(ClTvRainbowSpeed, cl_tv_rainbow_speed, 50, 1, 200, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Speed of rainbow color cycling")
MACRO_CONFIG_INT(ClTvRainbowSat, cl_tv_rainbow_sat, 100, 0, 100, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Saturation of rainbow effect")
MACRO_CONFIG_INT(ClTvRainbowVal, cl_tv_rainbow_val, 100, 0, 100, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Brightness/value of rainbow effect")

// Player Trails
MACRO_CONFIG_INT(ClTvTrails, cl_tv_trails, 1, 0, 3, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Player movement trails (0: off, 1: self only, 2: teammates, 3: all players)")
MACRO_CONFIG_INT(ClTvTrailLength, cl_tv_trail_length, 30, 5, 100, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Number of segments in player trail")
MACRO_CONFIG_INT(ClTvTrailsLength, cl_tv_trails_length, 30, 5, 100, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Trail length alias")
MACRO_CONFIG_INT(ClTvTrailWidth, cl_tv_trail_width, 8, 2, 32, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Width of player trail ribbon")
MACRO_CONFIG_INT(ClTvTrailsWidth, cl_tv_trails_width, 8, 2, 32, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Trail width alias")
MACRO_CONFIG_INT(ClTvTrailMode, cl_tv_trail_mode, 1, 0, 2, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Trail color mode (0: static color, 1: orange-yellow gradient, 2: rainbow)")
MACRO_CONFIG_COL(ClTvTrailColor, cl_tv_trail_color, 0xFF8C00, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Static trail color")

// Freeze & Off-Screen HUD
MACRO_CONFIG_INT(ClTvFreezeIndicator, cl_tv_freeze_indicator, 1, 0, 1, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Show freeze indicators and timers above frozen players")
MACRO_CONFIG_INT(ClTvFreezeSelf, cl_tv_freeze_self, 1, 0, 1, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Show local player frozen indicator")
MACRO_CONFIG_INT(ClTvOffscreen, cl_tv_offscreen, 1, 0, 1, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Master switch for off-screen player indicators")
MACRO_CONFIG_INT(ClTvOffscreenHud, cl_tv_offscreen_hud, 1, 0, 1, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Show off-screen player indicators alias")
MACRO_CONFIG_INT(ClTvOffscreenStyle, cl_tv_offscreen_style, 0, 0, 1, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Off-screen style (0: circular arrows, 1: mini-skins)")
MACRO_CONFIG_INT(ClTvOffscreenFilter, cl_tv_offscreen_filter, 0, 0, 4, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Off-screen filter (0: all, 1: teammates, 2: team list, 3: war list, 4: helper list)")
MACRO_CONFIG_INT(ClTvOffscreenMargin, cl_tv_offscreen_margin, 20, 5, 80, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Off-screen edge margin distance")
MACRO_CONFIG_INT(ClTvOffscreenNames, cl_tv_offscreen_names, 1, 0, 1, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Show player name on off-screen marker")
MACRO_CONFIG_INT(ClTvOffscreenDist, cl_tv_offscreen_dist, 1, 0, 1, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Show distance on off-screen marker")
MACRO_CONFIG_INT(ClTvOffscreenRange, cl_tv_offscreen_range, 3000, 500, 15000, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Max distance for off-screen radar tracking")
MACRO_CONFIG_INT(ClTvOffscreenTeammates, cl_tv_offscreen_teammates, 1, 0, 1, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Track off-screen teammates")
MACRO_CONFIG_INT(ClTvOffscreenWar, cl_tv_offscreen_war, 1, 0, 1, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Track off-screen war-listed players")

// Social / Player Lists
MACRO_CONFIG_INT(ClTvPlayerLists, cl_tv_player_lists, 1, 0, 1, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Enable TeeVisuals player lists (War, Team, Helper)")
MACRO_CONFIG_INT(ClTvListsWorld, cl_tv_lists_world, 1, 0, 1, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Highlight list players in world nameplates")
MACRO_CONFIG_INT(ClTvPlayerListsNameplates, cl_tv_player_lists_nameplates, 1, 0, 1, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Highlight list players on nameplates alias")
MACRO_CONFIG_INT(ClTvListsScoreboard, cl_tv_lists_scoreboard, 1, 0, 1, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Highlight list players in scoreboard")
MACRO_CONFIG_INT(ClTvPlayerListsScoreboard, cl_tv_player_lists_scoreboard, 1, 0, 1, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Highlight list players in scoreboard alias")
MACRO_CONFIG_INT(ClTvListsTag, cl_tv_lists_tag, 1, 0, 1, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Show category prefix tag [WAR]/[TEAM]/[HELPER]")
MACRO_CONFIG_COL(ClTvWarColor, cl_tv_war_color, 0xFF4D4D, CFGFLAG_SAVE | CFGFLAG_CLIENT, "War list highlight color")
MACRO_CONFIG_COL(ClTvTeamColor, cl_tv_team_color, 0x4DFF88, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Team list highlight color")
MACRO_CONFIG_COL(ClTvHelperColor, cl_tv_helper_color, 0x4DC3FF, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Helper list highlight color")

// Smooth Bindwheel
MACRO_CONFIG_INT(ClTvSmoothBindwheel, cl_tv_smooth_bindwheel, 1, 0, 1, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Enable Smooth Bindwheel radial menu")
MACRO_CONFIG_INT(ClTvBindwheelRadius, cl_tv_bindwheel_radius, 160, 60, 400, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Radius of Smooth Bindwheel")
MACRO_CONFIG_INT(ClTvBindwheelSpeed, cl_tv_bindwheel_speed, 100, 20, 300, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Animation speed of Smooth Bindwheel")
MACRO_CONFIG_INT(ClTvBindwheelSlots, cl_tv_bindwheel_slots, 8, 4, 12, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Number of slots in Bindwheel")

// UI Customization
MACRO_CONFIG_STR(ClTvMenuBg, cl_tv_menu_bg, 256, "", CFGFLAG_SAVE | CFGFLAG_CLIENT, "Custom main menu background image path")
MACRO_CONFIG_INT(ClTvMenuBgDim, cl_tv_menu_bg_dim, 30, 0, 100, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Darken overlay for custom menu background in percent")
MACRO_CONFIG_STR(ClTvCustomFont, cl_tv_custom_font, 256, "", CFGFLAG_SAVE | CFGFLAG_CLIENT, "Custom font file name in data/fonts/")

// Mobile UX
MACRO_CONFIG_INT(ClTvMobileTouchScale, cl_tv_mobile_touch_scale, 100, 50, 200, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Touch control button scale in percent")
MACRO_CONFIG_INT(ClTvMobileJoystickAlpha, cl_tv_mobile_joystick_alpha, 60, 10, 100, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Virtual joystick opacity in percent")
MACRO_CONFIG_INT(ClTvMobileSafeArea, cl_tv_mobile_safe_area, 1, 0, 1, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Force Safe Area insets on mobile displays")
