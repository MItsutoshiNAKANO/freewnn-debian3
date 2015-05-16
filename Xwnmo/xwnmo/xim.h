/*
 * $Id: xim.h,v 1.2 2001/06/14 18:16:18 ura Exp $
 */

/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
 * Copyright 1991, 1992 by Massachusetts Institute of Technology
 *
 * Author: OMRON SOFTWARE Co., Ltd. <freewnn@rd.kyoto.omronsoft.co.jp>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Emacs; see the file COPYING.  If not, write to the
 * Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * Commentary:
 *
 * Change log:
 *
 * Last modified date: 8,Feb.1999
 *
 * Code:
 *
 */
/*      Version 4.0
 */
/*********

        xwnmo.h

*********/

#ifndef XJUTIL
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#ifdef X11R5
#include "Xi18nint.h"
#else
#include "Xlcint.h"             /* for XLCd */
#include <X11/Xlocale.h>        /* for LC_ALL */
#endif /* X11R5 */
#endif /* !XJUTIL */
#include "client.h"

#ifndef X11R5
typedef wchar_t wchar;
#endif /* !X11R5 */

#ifndef XJUTIL
#include "msg.h"

#if !defined(X11R5) && defined(BC_X11R5)
/*--------------------- begin of extracting Xi18nint.h --------------------*/
#ifdef XML
#define XNQueryLanguage         "queryLanguage"
#define XNUsingLanguage         "usingLanguage"
#define XNCurrentLanguage       "currentLanguage"
#define XNChangeLocaleCB        "changeLocaleCB"

#define IMQueryLanguage         1

#define ICUsingLanguage         28
#define ICCurrentLanguage       29
#define ICChangeLocaleCB        30
#define ICAllMask               0x73ffffff
#else /* XML */
#define ICAllMask               0x03ffffff
#endif /* XML */

#define StatusOffset            11
/*--------------------- end of extracting Xi18nint.h --------------------*/

/*------------------- begin of extracting Xlcint.h ----------------------*/
#define ICInputStyle            0
#define ICClientWindow          1
#define ICFocusWindow           2
#define ICFilterEvents          3
#define ICArea                  4
#define ICAreaNeeded            5
#define ICSpotLocation          6
#define ICColormap              7
#define ICStdColormap           8
#define ICForeground            9
#define ICBackground            10
#define ICBackgroundPixmap      11
#define ICFontSet               12
#define ICLineSpace             13
#define ICCursor                14
#define ICResourceClass         26
#define ICResourceName          27

#define IMQueryInputStyle       0

#define IMResourceWrite         1
#define IMResourceRead          2
#define IMResourceReadWrite     3
/*------------------- end of extracting Xlcint.h -----------------------*/
#endif /* !defined(X11R5) && defined(BC_X11R5) */

#define MaskNeeded      KeyPressMask

typedef struct _XIMLangDataBase
{
  char *lang;
  char *lc_name;
  int cswidth_id;
  Status connect_serv;
  FunctionTable *f_table;
  Bool read;
  char *uumrc_name;             /* uumrc file name              */
  char *rkfile_name;            /* romkan file name             */
  char *uumkey_name;            /* uumkey file name             */
  WnnEnv *normal_env;
  WnnEnv *reverse_env;
  char *host_name;
  char *rev_host_name;
  RomkanTable *rk_table;
  char *jishopath;
  char *hindopath;
  char *fuzokugopath;
  Bool h_off_def;
  Bool e_delete;
  Bool s_ascii_char_def;
  short m_chg;
  short m_bunsetsu;
  short m_history;
  int t_comment;
  Bool h_on_kuten;
#ifdef  USING_XJUTIL
  int xjutil_act;
  Window xjutil_id;
  int xjutil_pid;
  struct _XIMClientRec *xjutil_use;
#endif                          /* USING_XJUTIL */
#ifdef  USING_BUSHU
  int bushu_act;
  Window bushu_id;
  int bushu_pid;
  struct _XIMClientRec *bushu_use;
#endif                          /* USING_BUSHU */
  struct _XIMLangDataBase *next;
}
XIMLangDataBase;

typedef struct _XIMNestLangRec *XIMNestLangList;

typedef struct _XIMNestLangRec
{
  char *lc_name;
  char *alias_name;
  XIMLangDataBase *lang_db;
  struct _XIMNestLangRec *next;
}
XIMNestLangRec;

typedef struct _XIMLcNameRec
{
  char *lc_name;
  XIMLangDataBase *lang_db;
  struct _XIMLcNameRec *next;
}
XIMLcNameRec;

typedef struct _XIMRootRec
{
  Window root_window;
  int screen;
  struct _XIMClientRec *ximclient;
  struct _Ichiran *ichi;
  struct _Inspect *inspect;
  unsigned int bw;
  unsigned long bc;
  char root_visible;
  char root_visible_flag;
}
XIMRootRec;

typedef struct _XIMCmblk
{
  int sd;
  int use;
  Bool byteOrder;
  int screen;
#if defined(XJPLIB) && defined(XJPLIB_DIRECT)
  int xjp;
#endif                          /* defined(XJPLIB) && defined(XJPLIB_DIRECT) */
#if !defined(X11R5) && defined(BC_X11R5)
  int ximr5;                    /* True indicates if client is from X11R5 */
#endif                          /* !defined(X11R5) && defined(BC_X11R5) */
}
XIMCmblk;

#endif /* !XJUTIL */

#ifdef XJUTIL
typedef struct _XIMRootRec
{
  Window root_window;
  int screen;
  struct _Ichiran *ichi;
  struct _JutilRec *jutil;
  struct _YesOrNo *yes_no;
  unsigned long fg;
  unsigned long bg;
  unsigned int bw;
  char *fontset_name;
  GC gc;
  GC reversegc;
  GC invertgc;
}
XIMRootRec;
#endif /* XJUTIL */

typedef struct _BoxRec
{
  Window window;
  int x, y;
  int width, height;
  int border_width;
  unsigned long fg, bg, bp;
  unsigned char *string;
  GC invertgc;
  int (*cb) ();
  int *cb_data;
  int (*redraw_cb) ();
  int *redraw_cb_data;
  int do_ret;
  int sel_ret;
  char reverse;
  char in;
  char map;
  char freeze;
  struct _BoxRec *next;
}
BoxRec;

typedef struct _YesOrNo
{
#ifndef XJUTIL
  XIMRootRec *root_pointer;
#endif                          /* !XJUTIL */
  Window w;
  BoxRec *title;
  BoxRec *button[2];
  char map;
  char exp;
  int x, y;
  int mode;
}
YesOrNo;

typedef struct _FromConvCode
{
  KeySym keysym;
  unsigned int state;
}
FromConvCode;

typedef struct _ConvCode
{
  int tokey;                    /* output keycode */
  FromConvCode fromkey;         /* input KeySym and state */
}
ConvCode;

#ifdef XJUTIL
typedef struct _Keytable
{
  int cnt;
  Window w;
  BoxRec *button[MAX_KEY_BUTTON];
  int max_button;
  XPoint save_p;
  char map;
  char exp;
}
Keytable;

#endif /* XJUTIL */

typedef struct _Ichiran
{
  Window w;
  Window w0;
  Window w1;
  BoxRec *title;
  BoxRec *subtitle;
  BoxRec *comment;
  BoxRec *button[MAX_BUTTON];
  int max_button;
  GC invertgc;
  Window nyuu_w;
  int kosuu;
  unsigned char *buf[200];
  unsigned char **save_buf;
  unsigned char *nyuu;
  char select_mode;
  char map;
  char exp;
  char lock;
  unsigned int height, width;
  int max_line;
  int start_line;
  int end_line;
  int page_height;
  int page;
  int cur_page;
  int kosuu_all;
  int max_len;
  int mode;
  int max_lines;
  int max_columns;
  int hilited_item;
  int hilited_ji_item;
  int init;
  int item_width;
  int item_height;
  unsigned short save_x, save_y;
#ifdef XJUTIL
  Keytable *keytable;
#endif                          /* XJUTIL */
}
Ichiran;

#ifndef XJUTIL
typedef struct _XIMAttributes
{
  XRectangle area;
  XRectangle area_needed;
  Colormap colormap;
  Atom std_colormap;
  unsigned long fg;
  unsigned long bg;
  Pixmap bg_pixmap;
  char *fontset;
  int line_space;
  Cursor cursor;
  GC gc;
  GC reversegc;
  GC invertgc;
}
XIMAttributes;

typedef struct _XIMLangRec
{
  XIMNestLangList lc_list;      /* List of locale */
  XIMNestLangRec *cur_lc;
  XIMLangDataBase *lang_db;
  XFontSet pe_fs;
  XFontSet st_fs;
  XCharStruct *pe_b_char;
  XCharStruct *st_b_char;
#ifdef X11R5
  XLocale xlc;
#else
  XLCd xlc;
#endif                          /* X11R5 */
  Window wp[3];                 /*  Parent windows */
  Window w[3];                  /*  child windows */
  Window wn[3];                 /*   */
  Window ws;                    /*  Status window */
  short linefeed[3];            /*  */
  short vst;                    /*  */
  unsigned char max_l1;
  unsigned char max_l2;
  unsigned char max_l3;
  wchar *buf;                   /* Buffer of Preedit */
  wchar *buf0;                  /* Buffer of Status */
  unsigned char *att;           /* Attributes of buf */
  unsigned char *att0;          /* Attributes of buf0 */
  short currentcol;             /* Current cursor position of Preedit */
  short currentcol0;            /* Current cursor position of Status */
  short max_pos;                /* Max cursor position of Preedit */
  short max_pos0;               /* Max cursor position of Status */
  short note[2];                /*  */
  short max_cur;
  short del_x;
  unsigned char cursor_flag;
  unsigned char mark_flag;
  unsigned char u_line_flag;
  unsigned char r_flag;
  unsigned char b_flag;
  unsigned char visible;
  unsigned char visible_line;
  unsigned char m_cur_flag;
  WnnClientRec *w_c;
#ifdef  USING_XJUTIL
  int xjutil_fs_id;
  int working_xjutil;
#endif                          /* USING_XJUTIL */
}
XIMLangRec;

typedef struct _XIMClientRec *XIMClientList;

typedef struct _XIMClientRec
{
  XIMRootRec *root_pointer;
  char *user_name;
  char *using_language;
  int lang_num;
#ifndef X11R5
  Bool sync_needed;             /* if True, need sync */
  XEvent cur_event;             /* back to client if nofilter */
  int im_id;                    /* IM id */
#endif                          /* X11R5 */
  int number;                   /* IC id */
  XIMLangRec **xl;
  XIMLangRec *cur_xl;
  XIMStyle input_style;         /* Input style */
  Window w;                     /* Client window */
  unsigned long mask;           /*  mask value of each element */
  XRectangle client_area;       /* Area of client window */
  Window focus_window;
  XRectangle focus_area;        /* Area of focus window */
  XPoint point;
  unsigned long filter_events;  /* Event mask that IM need */
  XIMAttributes pe;
  XIMAttributes st;
  int fd;                       /* File descripter */
  short columns;                /*  */
  short max_columns;            /*  Max columns */
  short c0;                     /*  column number of first line */
  short c1;                     /*  column number of first line */
  short c2;                     /*  column number of third line */
  short maxcolumns;
  YesOrNo *yes_no;
#ifdef  CALLBACKS
  unsigned int max_keycode;     /* Max keycode */
  unsigned char cb_redraw_needed;
  int cb_pe_start;
  int cb_st_start;
#endif                          /* CALLBACKS */
  int have_ch_lc;
  int ch_lc_flg;
  int have_world;
  int world_on;
#ifdef  XJPLIB
  int xjp;
#endif                          /* XJPLIB */
  struct _XIMClientRec *next;
}
XIMClientRec;


typedef struct _Inspect
{
  Window w;
  Window w1;
  BoxRec *title;
  BoxRec *button[MAX_BUTTON];
  int max_button;
  unsigned char *msg;
  char map;
  char exp;
  char lock;
  int max_len;
  unsigned short save_x, save_y;
}
Inspect;

typedef struct _XInputManager
{
  int screen_count;
  XIMRootRec **root_pointer;
  int default_screen;
  XIMRootRec *cur_j_c_root;
  XIMClientRec *j_c;
  int client_count;
  short sel_ret;
  XIMStyle supported_style[MAX_SUPPORT_STYLE];
  char *supported_language;
  char sel_button;
  char exit_menu;
  char exit_menu_flg;
  char save_under;
}
XInputManager;

typedef struct _XIMInputRec *XIMInputList;

typedef struct _XIMInputRec
{
  Window w;                     /*  Focus window */
  XIMClientRec *pclient;        /* Client window structure */
  struct _XIMInputRec *next;
}
XIMInputRec;

typedef struct _ReadRkfileRec *ReadRkfileList;

typedef struct _ReadRkfileRec
{
  char *name;
  struct _ReadRkfileRec *next;
}
ReadRkfileRec;

extern Display *dpy;
extern Atom select_id;
extern XPoint button;

#define ClientWidth(xc)                 (xc->client_area.width)
#define ClientHeight(xc)                (xc->client_area.height)
#define ClientX(xc)                     (xc->client_area.x)
#define ClientY(xc)                     (xc->client_area.y)

#define FocusX(xc)              (xc->focus_window ? \
                                 xc->focus_area.x : ClientX(xc))
#define FocusY(xc)              (xc->focus_window ? \
                                 xc->focus_area.y : ClientY(xc))
#define FocusWidth(xc)          (xc->focus_window ? \
                                 xc->focus_area.width : ClientWidth(xc))
#define FocusHeight(xc)         (xc->focus_window ? \
                                 xc->focus_area.height : ClientHeight(xc))

#define PreeditWidth(xc)        (xc->pe.area.width)
#define PreeditHeight(xc)       (xc->pe.area.height)
#define PreeditX(xc)            (xc->pe.area.x)
#define PreeditY(xc)            (xc->pe.area.y)

#define StatusWidth(xc)         (xc->st.area.width)
#define StatusHeight(xc)        (xc->st.area.height)
#define StatusX(xc)             (xc->st.area.x)
#define StatusY(xc)             (xc->st.area.y)

#define PreeditSpotX(xc)        (xc->point.x)
#define PreeditSpotY(xc)        (xc->point.y)

#define FontWidth(xl)           (xl->pe_b_char->width)
#define FontAscent(xl)          (xl->pe_b_char->ascent)
#define FontDescent(xl)         (xl->pe_b_char->descent)
#define FontHeight(xl)          (FontAscent(xl) + FontDescent(xl))
#define StatusFontWidth(xl)     (xl->st_b_char->width)
#define StatusFontAscent(xl)    (xl->st_b_char->ascent)
#define StatusFontDescent(xl)   (xl->st_b_char->descent)
#define StatusFontHeight(xl)    (StatusFontAscent(xl) + StatusFontDescent(xl))

#define c012(xc)        (xc->c0 + xc->c1 + xc->c2)
#define c01(xc)         (xc->c0 + xc->c1)

#endif /* !XJUTIL */

#if defined(USING_XJUTIL) || defined(XJUTIL)
typedef struct _Xjutil_startRec
{
  char uumkey_name[256];
  char rkfile_name[256];
  char cvtkey_name[256];
  char cvtfun_name[256];
  char cvtmeta_name[256];
  char user_name[32];
  char lang[32];
  char lc_name[32];
  char f_name[32];
  char jishopath[256];
  char hindopath[256];
  char fuzokugopath[256];
  unsigned short fn_len;
  int rubout_code;
  unsigned short max_env;
  unsigned short max_reverse_env;
  int cswidth_id;
  int save_under;
}
Xjutil_startRec;

typedef struct _Xjutil_envRec
{
  int screen;
  char lc_name[32];
  int cswidth_id;
  int fs_id;
  unsigned short fn_len;
  unsigned long fore_ground;
  unsigned long back_ground;
  int cur_env_id;
  int cur_env_reverse_id;
  int env_is_reverse;
  int env_id[16];
  int env_reverse_id[16];
}
Xjutil_envRec;
#endif /* defined(USING_XJUTIL) || defined(XJUTIL) */
