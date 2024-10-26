/* See LICENSE file for copyright and license details. */

/* appearance */
static const unsigned int borderpx  = 1;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int gappih    = 20;       /* horiz inner gap between windows */
static const unsigned int gappiv    = 10;       /* vert inner gap between windows */
static const unsigned int gappoh    = 10;       /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    = 30;       /* vert outer gap between windows and screen edge */
static       int smartgaps          = 0;        /* 1 means no outer gap when there is only one window */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "monospace:size=10" };
static const char dmenufont[]       = "monospace:size=10";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#005577";
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel]  = { col_gray4, col_cyan,  col_cyan  },
};

/* tagging */
/* static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" }; */
/* static const char *tags[] = { "➊", "➋", "➌", "➍", "➎", "➏", "➐", "➑", "➒" }; */
/* static const char *tags[] = { "☹", "♨", "♺", "♿", "⚒", "⚓", "⚕", "⚗", "i⚛ }; */
/* static const char *tags[] = { "", "Finder", "File", "Edit", "View", "Settings", "Go", "Window", "Help"}; */
static const char *tags[] = { "", "", "", "", "", "", "󰊻", "", "" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",     NULL,       NULL,       0,            1,           -1 },
	{ "Firefox",  NULL,       NULL,       1 << 8,       0,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

#define FORCE_VSPLIT 1  /* nrowgrid layout: force two clients to always split vertically */

/* include */
#include <X11/XF86keysym.h>
#include "vanitygaps.c"
#include "movestack.c"

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "   Tile     ", tile },                                                                                     // first entry is default
	{ "   Float    ", NULL },                                                                                     // no layout function means floating behavior
	{ "   Monocle  ", monocle },
	{ "   Spiral   ", spiral },
	{ "   Dwindle  ", dwindle },
	{ "   Deck     ", deck },
	{ "   BStack   ", bstack },
	{ "   BStackH  ", bstackhoriz },
	{ "   Grid     ", grid },
	{ "   nRow Grid", nrowgrid },
	{ "   HorizGrid", horizgrid },
	{ "   Gapless G", gaplessgrid },
	{ "| |CMaster  ", centeredmaster },
	{ "| |CFMaster ", centeredfloatingmaster },
	{ NULL,            NULL },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0";                                                                                  // component of dmenucmd, manipulated in spawn()
static const char *dmenucmd[]           = { "dmenu_run", "-m", dmenumon, "-i", "-c", "-l", "15", "-bw", "2", "-p", "Яцп ТЋїѕ Ѕћїт: 󰜎 ", NULL };
static const char *dmenusystem[]        = { "dwm-system", NULL };
static const char *dmenumedia[]         = { "dwm-media", NULL };
static const char *termcmd[]            = { "st", NULL };
static const char *tmuxcmd[]            = { "st", "-e", "tmux-start.sh", "Login", NULL };
static const char *volumeup[]           = { "dwm-volumectrl", "up", NULL };
static const char *volumedown[]         = { "dwm-volumectrl", "down", NULL };
static const char *volumemute[]         = { "dwm-volumectrl", "mute", NULL };
static const char *brightnessup[]       = { "dwm-brightness", "up", NULL };
static const char *brightnessdown[]     = { "dwm-brightness", "down", NULL };
static const char *xmenu[]              = { "xmenu.sh", NULL };
static const char *scratchpad1[]        = { "1", "st", "-t", "ScratchPad1", "-g", "200x40", "-e", "tmux-start.sh", "ScratchPad1", NULL}; 
static const char *scratchpad2[]        = { "2", "xterm", "-class", "XTermScratchPad", "-title", "ScratchPad2", "-e", "tmux-start.sh", "ScratchPad2", NULL}; 
static const char *NoteTaking[]         = { "3", "xterm", "-class", "XTermNoteTaking", "-title", "NoteTaking", "-e", "dwm-notetaking", NULL}; 
static const char *layoutmenu_cmd       = "dwm-layoutmenu";

static const Key keys[] = {
	/* modifier                     key                         function        argument */
	{ MODKEY|ShiftMask,             XK_q,                       quit,           {0} },
	{ MODKEY,                       XK_q,                       killclient,     {0} },
	{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,             XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY|ShiftMask,             XK_h,      setcfact,       {.f = +0.25} },
	{ MODKEY|ShiftMask,             XK_l,      setcfact,       {.f = -0.25} },
	{ MODKEY|ShiftMask,             XK_o,      setcfact,       {.f =  0.00} },
	{ MODKEY,                       XK_Return, zoom,           {0} },
	{ MODKEY|ShiftMask,              XK_u,      incrgaps,       {.i = +1 } },
	{ MODKEY|ShiftMask|ShiftMask,    XK_u,      incrgaps,       {.i = -1 } },
	{ MODKEY|ShiftMask,              XK_i,      incrigaps,      {.i = +1 } },
	{ MODKEY|ShiftMask|ShiftMask,    XK_i,      incrigaps,      {.i = -1 } },
	{ MODKEY|ShiftMask,              XK_o,      incrogaps,      {.i = +1 } },
	{ MODKEY|ShiftMask|ShiftMask,    XK_o,      incrogaps,      {.i = -1 } },
	{ MODKEY|ShiftMask,              XK_6,      incrihgaps,     {.i = +1 } },
	{ MODKEY|ShiftMask|ShiftMask,    XK_6,      incrihgaps,     {.i = -1 } },
	{ MODKEY|ShiftMask,              XK_7,      incrivgaps,     {.i = +1 } },
	{ MODKEY|ShiftMask|ShiftMask,    XK_7,      incrivgaps,     {.i = -1 } },
	{ MODKEY|ShiftMask,              XK_8,      incrohgaps,     {.i = +1 } },
	{ MODKEY|ShiftMask|ShiftMask,    XK_8,      incrohgaps,     {.i = -1 } },
	{ MODKEY|ShiftMask,              XK_9,      incrovgaps,     {.i = +1 } },
	{ MODKEY|ShiftMask|ShiftMask,    XK_9,      incrovgaps,     {.i = -1 } },
	{ MODKEY|ShiftMask,              XK_0,      togglegaps,     {0} },
	{ MODKEY|ShiftMask|ShiftMask,    XK_0,      defaultgaps,    {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	{ 0,                            XF86XK_AudioLowerVolume,    spawn,          {.v = volumedown } },
	{ 0,                            XF86XK_AudioRaiseVolume,    spawn,          {.v = volumeup } },
	{ 0,                            XF86XK_AudioMute,           spawn,          {.v = volumemute } },
	{ 0,                            XF86XK_MonBrightnessUp,     spawn,          {.v = brightnessup } },
	{ 0,                            XF86XK_MonBrightnessDown,   spawn,          {.v = brightnessdown } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizeorfacts,  {0} },
	{ ClkClientWin,       MODKEY|ShiftMask, Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

