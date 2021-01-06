/* See LICENSE file for copyright and license details. */

#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx  = 0;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;     /* 0 means no systray */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "Firacode:size=10" };
static const char dmenufont[]       = "Firacode:size=10";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#005577";
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel]  = { col_gray4, col_cyan,  col_cyan  },
	[SchemeHid]  = { col_cyan,  col_gray1, col_cyan  },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

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

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },
#define HOLDKEY 0 // replace 0 with the keysym to activate holdbar

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = {
    "dmenu_run", "-m",      dmenumon, "-fn",    dmenufont, "-nb",     col_gray1,
    "-nf",       col_gray3, "-sb",    col_cyan, "-sf",     col_gray4, NULL};
static const char *termcmd[]    = { "st", NULL };
static const char *network[]    = { "st","nmtui", NULL };
static const char *browser[]    = { "firefox", NULL };
static const char *browserout[] = { "firefox-developer-edition", NULL };
static const char *music[]      = { "netease-cloud-music", NULL };
static const char *file[]       = { "rang", NULL };
static const char *le[]         = { "rang", NULL };
static const char *volumn[]     = { "pavucontrol-qt", NULL };
static const char *screenshot[] = { "flameshot", "gui", NULL };

static const char *vmute[]      = { "amixer", "set", "Master", "toggle", NULL};
static const char *vumnup[]     = { "amixer", "set", "Master", "10%+", NULL};
static const char *vdown[]      = { "amixer", "set", "Master", "10%-", NULL};
static const char *bldown[]     = { "xbacklight", "-dec", "10", NULL};
static const char *blup[]       = { "xbacklight", "-inc", "10", NULL};

static const char *lock[]       = { "blurlock", NULL};

/* commands spawned when clicking statusbar, the mouse button pressed is exported as BUTTON */
static char *statuscmds[] = { "notify-send Mouse$BUTTON" };
static char *statuscmd[] = { "/bin/sh", "-c", NULL, NULL };

#define M | MODKEY
#define S | ShiftMask
#define C | ControlMask

static Key keys[] = {
	/* modifier   key                      function        argument */
	{ 0 M,        XK_d,                    spawn,          {.v = dmenucmd } },
	{ 0 M,        XK_Return,               spawn,          {.v = termcmd } },
	{ 0 M,        XK_u,                    spawn,          {.v = browser} },
	{ 0 M S,      XK_u,                    spawn,          {.v = browserout} },
	{ 0 M,        XK_i,                    spawn,          {.v = music} },
	{ 0 M,        XK_e,                    spawn,          {.v = file} },
	{ 0 M,        XK_n,                    spawn,          {.v = network} },
	{ 0 M,        XK_v,                    spawn,          {.v = volumn} },
    { 0 M S,      XK_s,                    spawn,          {.v = screenshot} },

    { 0,          XF86XK_AudioLowerVolume, spawn,          {.v = vdown} },
    { 0,          XF86XK_AudioRaiseVolume, spawn,          {.v = vumnup} },
    { 0,          XF86XK_AudioMute,        spawn,          {.v = vmute} },
    { 0,          XF86XK_MonBrightnessDown,spawn,          {.v = bldown} },
    { 0,          XF86XK_MonBrightnessUp,  spawn,          {.v = blup} },

    { 0 M,          XK_BackSpace,            spawn,          {.v = blup} },

	{ 0 M,        XK_j,                    focusstack,     {.i = +1 } },
	{ 0 M,        XK_k,                    focusstack,     {.i = -1 } },
	{ 0 M S,      XK_j,                    incnmaster,     {.i = +1 } },
	{ 0 M S,      XK_k,                    incnmaster,     {.i = -1 } },

	{ 0 M,        XK_h,                    setmfact,       {.f = -0.05} },
	{ 0 M,        XK_l,                    setmfact,       {.f = +0.05} },
	{ 0 M S,      XK_Return,               zoom,           {0} },
	{ 0 M,        XK_Tab,                  view,           {0} },
	{ 0 M S,      XK_q,                    killclient,     {0} },
	{ 0 M,        XK_z,                    setlayout,      {.v = &layouts[0]} },
	{ 0 M,        XK_x,                    setlayout,      {.v = &layouts[1]} },
	{ 0 M,        XK_c,                    setlayout,      {.v = &layouts[2]} },
	{ 0 M,        XK_space,                setlayout,      {0} },
	{ 0 M S,      XK_space,                togglefloating, {0} },
	{ 0 M,        XK_0,                    view,           {.ui = ~0u } },
	{ 0 M S,      XK_0,                    tag,            {.ui = ~0u } },

	{ 0 M,        XK_comma,                viewnext,       {.i = -1 } },
	{ 0 M,        XK_period,               viewnext,       {.i = +1 } },
	{ 0 M S,      XK_comma,                viewnext,       {.i = -3 } },
	{ 0 M S,      XK_period,               viewnext,       {.i = +3 } },


	{ 0 M S,      XK_e,                    quit,           {0} },
	{ 0 M S C,    XK_e,                    quit,           {1} }, 
	{ 0 M ,       XK_f,                    togglebar,      {1} }, 
	//{ 0,          MODKEY,                 holdbar,        {0} },

	TAGKEYS(      XK_1,                    0)
	TAGKEYS(      XK_2,                    1)
	TAGKEYS(      XK_3,                    2)
	TAGKEYS(      XK_4,                    3)
	TAGKEYS(      XK_5,                    4)
	TAGKEYS(      XK_6,                    5)
	TAGKEYS(      XK_7,                    6)
	TAGKEYS(      XK_8,                    7)
	TAGKEYS(      XK_9,                    8)
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
    /* click                event mask      button          function argument
     */
    {ClkLtSymbol,   0   ,   Button1,        setlayout,      {0}},
    {ClkLtSymbol,   0   ,   Button3,        setlayout,      {.v = &layouts[2]}},
	{ClkWinTitle,   0   ,   Button1,        togglewin,      {0} },
    {ClkWinTitle,   0   ,   Button2,        zoom,           {0}},
//  {ClkStatusText, 0   ,   Button2,        spawn,          {.v = termcmd}},
	{ClkStatusText, 0   ,   Button1,        spawn,          {.v = statuscmd } },
	{ClkStatusText, 0   ,   Button2,        spawn,          {.v = statuscmd } },
	{ClkStatusText, 0   ,   Button3,        spawn,          {.v = statuscmd } },
    {ClkClientWin,  0  M,   Button1,        movemouse,      {0}},
    {ClkClientWin,  0 S M,  Button1,        resizemouse,    {0}},
    {ClkClientWin,  0  M,   Button2,        togglefloating, {0}},
    {ClkClientWin,  0  M,   Button3,        resizemouse,    {0}},
    {ClkClientWin,  0  M,   Button4,        focusstack,     {.i = -1}},
    {ClkClientWin,  0  M,   Button5,        focusstack,     {.i = +1}},
    {ClkTagBar,     0   ,   Button1,        view,           {0}},
    {ClkTagBar,     0   ,   Button3,        toggleview,     {0}},
    {ClkTagBar,     0  M,   Button1,        tag,            {0}},
    {ClkTagBar,     0  M,   Button3,        toggletag,      {0}},
};
