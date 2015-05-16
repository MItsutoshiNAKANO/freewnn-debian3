/*
 * $Id: xjutil.h,v 1.9.2.1 1999/02/08 07:43:38 yamasita Exp $
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

        dicserver.h

*********/

#include <X11/Xlib.h>
#include <X11/IntrinsicP.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#ifdef X11R5
#include "Xi18nint.h"
#else
#include "Xlcint.h"
#include <X11/Xlocale.h>
#endif /* X11R5 */
#include "msg.h"
#include "xim.h"
/*
#include "rk_header.h"
#include "rk_multi.h"
*/

typedef struct _XjutilFSRec
{
  XFontSet fs;
  XCharStruct *b_char;
  int act;
  struct _XjutilFSRec *next;
}
XjutilFSRec;

typedef XjutilFSRec *XjutilFSList;

typedef struct _JutilTextRec
{
  Window wp;
  Window w;
  Window wn[3];
  long x;
  long y;
  long width;
  long height;
  long bp;
  short max_columns;
  short vst;
  wchar *buf;
  unsigned char *att;
  short max_pos;
  short currentcol;
  short maxcolumns;
  short note[2];
  unsigned char cursor_flag;
  unsigned char mark_flag;
  unsigned char u_line_flag;
  unsigned char r_flag;
  unsigned char b_flag;
}
JutilTextRec;

#define JCLIENTS        4

typedef struct _JutilRec
{
  Window w;
  BoxRec *rk_mode;
  BoxRec *title;
  BoxRec *button[MAX_JU_BUTTON];
  int max_button;
  BoxRec *mes_button[JCLIENTS];
  int mes_mode_return[JCLIENTS];
  int max_mes;
  JutilTextRec *mes_text[JCLIENTS];
  int mode;
  int mes_mode;
  XPoint save_p;
}
JutilRec;

typedef struct _Xjutil
{
  char *lang;
  char *lc_name;
  unsigned int cswidth_id;
#ifdef X11R5
  XLocale xlc;
#else
  XLCd xlc;
#endif                          /* X11R5 */
  int mode;                     /* ICHIRAN or JUTIL */
  int screen_count;
  XIMRootRec **root_pointer;
  int default_screen;
  short sel_ret;
  char sel_button;
  unsigned char work;
  char save_under;
}
Xjutil;

extern Display *dpy;
extern Atom select_id;

#define FontWidth               (cur_fs->b_char->width)
#define FontAscent              (cur_fs->b_char->ascent)
#define FontDescent             (cur_fs->b_char->descent)
#define FontHeight              (FontAscent + FontDescent)

#define print_out_func          (f_table->print_out_function)
#define input_func              (f_table->input_function)
#define call_t_redraw_move_func (f_table->call_t_redraw_move_function)
#define call_t_redraw_move_1_func (f_table->call_t_redraw_move_1_function)
#define call_t_redraw_move_2_func (f_table->call_t_redraw_move_2_function)
#define call_t_print_l_func     (f_table->call_t_print_l_function)
#define redraw_when_chmsig_func (f_table->redraw_when_chmsig_function)
#define char_len_func           (f_table->char_len_function)
#define char_q_len_func         (f_table->char_q_len_function)
#define t_redraw_move_func      (f_table->t_redraw_move_function)
#define t_print_l_func          (f_table->t_print_l_function)
#define c_top_func              (f_table->c_top_function)
#define c_end_func              (f_table->c_end_function)
#define c_end_nobi_func         (f_table->c_end_nobi_function)
#define hani_settei_func        (f_table->hani_settei_function)

#define cur_env                 (env_is_reverse? cur_reverse_env->env: cur_normal_env->env)
