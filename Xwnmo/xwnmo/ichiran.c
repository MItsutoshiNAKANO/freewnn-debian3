/*
 * $Id: ichiran.c,v 1.2 2001/06/14 18:16:15 ura Exp $
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
#include <stdio.h>
#include "commonhd.h"
#include "sdefine.h"
#ifdef XJUTIL
#include "xjutil.h"
#include "sxheader.h"
#include "xext.h"
#else /* XJUTIL */
#include "xim.h"
#include "sheader.h"
#include "ext.h"
#endif /* XJUTIL */

extern GC currentGC;

extern wchar_t cursor_wchar_t;

#ifdef XJUTIL
extern XPoint button;
extern XSizeHints hints;
extern Cursor cursor1;

#define ICHI            (cur_root->ichi)
#define FONTHEIGHT      FontHeight
#define FONTWIDTH       FontWidth
#define FONTASCENT      FontAscent
#define FONTDESCENT     FontDescent
#define CANCEL_NUM      2
#define MOUSE_NUM       111
#define NEXT_NUM        3
#define BACK_NUM        4
#define YES_NUM         5
#define NO_NUM          6
#define OK_NUM          7
#define FOREGROUND      (cur_root->fg)
#define BACKGROUND      (cur_root->bg)
#define SEL_RET         (xjutil->sel_ret)
#define ROOT_WINDOW     (cur_root->root_window)
#define SCREEN          (cur_root->screen)
#else /* XJUTIL */
#define ICHI            (xim->cur_j_c_root->ichi)
#define FONTHEIGHT      FontHeight(xl)
#define FONTWIDTH       FontWidth(xl)
#define FONTASCENT      FontAscent(xl)
#define FONTDESCENT     FontDescent(xl)
#define CANCEL_NUM      16
#define MOUSE_NUM       39
#define NEXT_NUM        41
#define BACK_NUM        42
#define YES_NUM         51
#define NO_NUM          52
#define OK_NUM          53
#define FOREGROUND      (cur_p->pe.fg)
#define BACKGROUND      (cur_p->pe.bg)
#define SEL_RET         (xim->sel_ret)
#define ROOT_WINDOW     (xim->cur_j_c_root->root_window)
#define SCREEN          (xim->cur_j_c_root->screen)
#endif /* XJUTIL */

static struct jikouho_ichi
{
  int x_s;
  int y_s;
  int x_e;
  int y_e;
  int line;
}
jikouho_ichi[MAXJIKOUHO + 1];

static int jikouho_line[MAXJIKOUHO + 1];

static int
Y_InItem (y)
     int y;
{
#ifndef XJUTIL
  register XIMLangRec *xl;

  if (xim->j_c == NULL)
    return (-1);
  xl = xim->j_c->cur_xl;
#endif /* !XJUTIL */

  if (y < 0 || y > ICHI->item_height)
    return (-1);
  return (y / (FONTHEIGHT + IN_BORDER));
}

static void
xw_fillrec ()
{
  register Ichiran *ichi;
#ifndef XJUTIL
  XIMLangRec *xl;

  if (xim->j_c == NULL)
    return;
  xl = xim->j_c->cur_xl;
#endif /* !XJUTIL */
  ichi = ICHI;

  if (ichi->hilited_item != -1)
    {
      XFillRectangle (dpy, ichi->w1, ichi->invertgc, 0, ichi->hilited_item * (FONTHEIGHT + IN_BORDER), ichi->item_width, (FONTHEIGHT + IN_BORDER));
      XFlush (dpy);
    }
  return;
}

static void
xw_ji_fillrec ()
{
  register Ichiran *ichi;

#ifndef XJUTIL
  if (!xim->j_c)
    return;
#endif /* !XJUTIL */
  ichi = ICHI;
  if (ichi->hilited_ji_item != -1)
    {
      XFillRectangle (dpy, ichi->w1, ichi->invertgc,
                      jikouho_ichi[ichi->hilited_ji_item].x_s,
                      jikouho_ichi[ichi->hilited_ji_item].y_s,
                      (jikouho_ichi[ichi->hilited_ji_item].x_e - jikouho_ichi[ichi->hilited_ji_item].x_s), (jikouho_ichi[ichi->hilited_ji_item].y_e - jikouho_ichi[ichi->hilited_ji_item].y_s));
      XFlush (dpy);
    }
  return;
}

#ifdef XJUTIL
void
decide_position (width, height, border, ret_x, ret_y)
#else /* XJUTIL */
static void
decide_position (root, xl, width, height, border, ret_x, ret_y)
     register XIMRootRec *root;
     register XIMLangRec *xl;
#endif /* XJUTIL */
     register int width, height, border;
     register int *ret_x, *ret_y;
{
#ifdef XJUTIL
  if (xjutil->sel_button)
    {
      *ret_x = button.x - width / 2;
      *ret_y = button.y - FontHeight;
      xjutil->sel_button = 0;
    }
  else
    {
      *ret_x = (DisplayWidth (dpy, SCREEN) - width) / 2;
      *ret_y = (DisplayHeight (dpy, SCREEN) - height) / 2;
    }
#else /* XJUTIL */
  Window r, parent, child, *children;
  XWindowAttributes to_attr;
  int root_x, root_y;
  unsigned int nc;

  if (xim->sel_button)
    {
      *ret_x = button.x - FONTWIDTH;;
      *ret_y = button.y - FONTHEIGHT;
      xim->sel_button = 0;
    }
  else
    {
      if (IsPreeditNothing (cur_x))
        {
          XGetWindowAttributes (dpy, root->ximclient->w, &to_attr);
          XQueryTree (dpy, root->ximclient->w, &r, &parent, &children, &nc);
          XTranslateCoordinates (dpy, parent, ROOT_WINDOW, to_attr.x, to_attr.y, &root_x, &root_y, &child);
          *ret_x = root_x;
          *ret_y = root_y - height;
        }
      else
        {
          XGetWindowAttributes (dpy, cur_x->w, &to_attr);
          XQueryTree (dpy, cur_x->w, &r, &parent, &children, &nc);
          XTranslateCoordinates (dpy, parent, ROOT_WINDOW, to_attr.x, to_attr.y, &root_x, &root_y, &child);
          *ret_x = root_x;
          *ret_y = root_y + to_attr.height;
        }
    }
#endif /* XJUTIL */
  if ((*ret_x + width + border) > DisplayWidth (dpy, SCREEN))
    *ret_x = DisplayWidth (dpy, SCREEN) - width - border;
  if (*ret_x < 0)
    *ret_x = 0;
  if ((*ret_y + height + border) > DisplayHeight (dpy, SCREEN))
    *ret_y = DisplayHeight (dpy, SCREEN) - height - border;
  if (*ret_y < 0)
    *ret_y = 0;
  return;
}

static int
xw_set_jikouho (info, kosuu, width)
     unsigned char **info;
     int kosuu;
     int width;
{
  register int cnt, cntt;
  register unsigned char *p;
  unsigned char tmp[256];
  register int columns = 0;
  register Ichiran *ichi;

#ifndef XJUTIL
  register XIMLangRec *xl;

  if (xim->j_c == NULL)
    return (-1);
  xl = xim->j_c->cur_xl;
#endif /* !XJUTIL */
  ichi = ICHI;

  jikouho_ichi[0].x_s = 0;
  jikouho_line[1] = 0;

  p = ichi->buf[0] = ichi->save_buf[0];
  *p = 0;
  for (cnt = 0, cntt = 0; cnt < kosuu; cnt++)
    {
      if (ichi->mode & JIKOUHO)
        sprintf ((char *) tmp, "  %s  ", info[cnt]);
      else if (ichi->mode & JIS_IN)
        sprintf ((char *) tmp, "%2x. %s ", cnt + ((kosuu < 16) ? 1 : 33), info[cnt]);
      else if (ichi->mode & KUTEN_IN)
        sprintf ((char *) tmp, "%2d. %s ", cnt + 1, info[cnt]);
      if ((columns += columnlen ((char *) tmp)) > width)
        {
          if (columns == columnlen ((char *) tmp))
            {
              width = columns;
            }
          else
            {
              jikouho_line[++cntt + 1] = cnt;
              jikouho_ichi[cnt--].x_s = 0;
              columns = 0;
              if (cntt >= ichi->max_lines)
                alloc_for_save_buf (ichi, ichi->max_lines + MAX_PAGE_LINES);
              p = ichi->buf[cntt] = ichi->save_buf[cntt];
              *p = 0;
              continue;
            }
        }
      jikouho_ichi[cnt + 1].x_s = columns * FONTWIDTH;
      jikouho_ichi[cnt].x_e = columns * FONTWIDTH - 1;
      jikouho_ichi[cnt].y_s = cntt * (FONTHEIGHT + IN_BORDER);
      jikouho_ichi[cnt].y_e = (cntt + 1) * (FONTHEIGHT + IN_BORDER) - 1;
      jikouho_ichi[cnt].line = cntt + 1;
      p = (unsigned char *) strcat ((char *) p, (char *) tmp);
    }
  jikouho_line[++cntt + 1] = kosuu;
  return (cntt);
}

static void
move_ichi (ichi)
     register Ichiran *ichi;
{
  ichi->start_line = (ichi->cur_page - 1) * ichi->max_line;
  ichi->end_line = ichi->start_line + ichi->max_line - 1;
  XMoveWindow (dpy, ichi->w1, 0, -((ichi->cur_page - 1) * ichi->page_height));
}
static void
check_move_ichi (p)
     register int p;
{
  register int tmp;
  register Ichiran *ichi;

#ifndef XJUTIL
  if (!xim->j_c)
    return;
#endif /* !XJUTIL */
  ichi = ICHI;
  if ((tmp = p / ichi->max_line + (p % ichi->max_line ? 1 : 0)) != ichi->cur_page)
    {
      ichi->cur_page = tmp;
      move_ichi (ichi);
    }
}

void
JW3Mputc (s, w, li, flg, in)
     unsigned char *s;
     register Window w;
     register int li, flg, in;
{
#ifndef XJUTIL
  register XIMLangRec *xl = cur_p->cur_xl;
#endif /* !XJUTIL */
  int wc_len;
  XCharStruct cs;

#ifdef XJUTIL
  if (flg & REV_FLAG)
    {
      currentGC = cur_root->reversegc;
    }
  else
    {
      currentGC = cur_root->gc;
    }
#else /* XJUTIL */
  if (flg & REV_FLAG)
    {
      currentGC = cur_p->pe.reversegc;
    }
  else
    {
      currentGC = cur_p->pe.gc;
    }
#endif /* XJUTIL */
  cs.width = FONTWIDTH;
  cs.ascent = FONTASCENT;
  cs.descent = FONTDESCENT;

  while (1)
    {
#ifdef XJUTIL
      wc_len = char_to_wchar (xjutil->xlc, s, wc_buf, strlen ((char *) s),
#else /* XJUTOIL */
      wc_len = char_to_wchar (xl->xlc, s, wc_buf, strlen ((char *) s),
#endif /* XJUTOIL */
                              wc_buf_max);
      if (wc_len < -1)
        {
          return;
        }
      else if (wc_len == -1)
        {
          if (realloc_wc_buf () < 0)
            return;
        }
      else
        {
          break;
        }
    }
#ifdef XJUTIL
  JWOutput (w, cur_fs->fs, currentGC, 0, 0, flg,
#else /* XJUTOIL */
  JWOutput (w, xl->pe_fs, currentGC, 0, 0, flg,
#endif /* XJUTOIL */
            in, (li * (FONTHEIGHT + in) + in), &cs, wc_buf, wc_len);
}

static void
set_wm_size (window, width, height)
     Window window;
     unsigned int width, height;
{
  XSizeHints hints;

  hints.flags = (USSize | PMinSize | PMaxSize);
  hints.width = hints.min_width = hints.max_width = width;
  hints.height = hints.min_height = hints.max_height = height;
  XSetWMNormalHints (dpy, window, &hints);
}

void
set_wm_properties (window, x, y, width, height, gravity, name, class_name, input, state, flags, icon_x, icon_y)
     Window window;
     int x, y;
     unsigned int width, height;
     int gravity;
     char *name, *class_name;
     Bool input;
     int state;
     long flags;
     int icon_x, icon_y;
{
  XSizeHints hints;
  XClassHint class_hints;
  XWMHints wm_hints;
#ifndef X11R3
  XTextProperty winName, iconName;
#else /* X11R3 */
  char *winName, *iconName;
#endif /* X11R3 */


  if (name)
    {
#ifndef X11R3
      winName.encoding = XA_STRING;
      winName.format = 8;
      winName.value = (unsigned char *) class_name;
      winName.nitems = strlen ((char *) winName.value);
      iconName.encoding = XA_STRING;
      iconName.format = 8;
      iconName.value = (unsigned char *) class_name;
      iconName.nitems = strlen ((char *) winName.value);
#else /* X11R3 */
      winName = class_name;
      iconName = class_name;
#endif /* X11R3 */
      class_hints.res_name = name;
      class_hints.res_class = class_name;
      wm_hints.input = input;
      wm_hints.flags = flags;
      wm_hints.initial_state = state;
      wm_hints.icon_x = icon_x;
      wm_hints.icon_y = icon_y;
    }
  else
    {
      wm_hints.flags = StateHint;
      wm_hints.initial_state = NormalState;
    }

  hints.flags = (USSize | USPosition | PMinSize | PMaxSize | PResizeInc
#ifndef X11R3
                 | PBaseSize | PWinGravity
#endif /* X11R3 */
    );

  hints.x = x;
  hints.y = y;
  hints.width = hints.min_width = hints.max_width = width;
  hints.height = hints.min_height = hints.max_height = height;
  hints.width_inc = hints.height_inc = 0;
#ifndef X11R3
  hints.base_width = width;
  hints.base_height = height;
  hints.win_gravity = gravity;
  if (name)
    {
      XSetWMProperties (dpy, window, &winName, &iconName, NULL, 0, &hints, &wm_hints, &class_hints);
    }
  else
    {
      XSetWMNormalHints (dpy, window, &hints);
      XSetWMHints (dpy, window, &wm_hints);
    }
#else /* X11R3 */
  if (name)
    {
      XSetStandardProperties (dpy, window, winName, iconName, None, NULL, 0, &hints);
      XSetWMHints (dpy, window, &wm_hints);
      XSetClassHint (dpy, window, &class_hints);
    }
  else
    {
      XSetNormalHints (dpy, window, &hints);
      XSetWMHints (dpy, window, &wm_hints);
    }
#endif /* X11R3 */
}

static void
next_back_ichi (ichi, next)
     register Ichiran *ichi;
     register int next;
{
  register int start_end;

  move_ichi (ichi);
  if (next)
    {
      start_end = ichi->start_line;
    }
  else
    {
      start_end = ichi->end_line;
      if (start_end >= ichi->kosuu)
        start_end = ichi->kosuu - 1;
    }
  if (ichi->mode & SENTAKU)
    {
      ichi->hilited_item = start_end;
    }
  else
    {
      ichi->hilited_ji_item = jikouho_line[start_end + 1];
    }
  redraw_ichi_w ();
}

int
init_ichiran (info, kosuu, init, title, subtitle, exec_title, comment, nyuu_col, mode)
     unsigned char **info;
     int kosuu, init;
     unsigned char *title;
     unsigned char *subtitle;
     unsigned char *exec_title;
     unsigned char *comment;
     int nyuu_col, mode;
{
#ifndef XJUTIL
  register XIMLangRec *xl = cur_p->cur_xl;
  register XIMRootRec *root = cur_x->root_pointer;
#endif /* !XJUTIL */
  register Ichiran *ichi;
  int x_cood, y_cood, width, height;
  int save_x, save_y;
  int maxlen;
  int def_maxlen;
  int def_height;
  register int cnt;
  register int tmp = 0;
  register int len_t;
  Window r, child;
  int win_x, win_y;
  unsigned int keys_buttons;
  char *lang;

#ifdef XJUTIL
  xjutil->mode |= ICHIRAN_MODE;
  lang = xjutil->lang;
  ichi = cur_root->ichi;
  ichi->invertgc = cur_root->invertgc;
#else /* XJUTIL */
  if (xim->j_c)
    return (-1);
  xim->j_c = cur_p;
  xim->cur_j_c_root = root;
  set_cswidth (xl->lang_db->cswidth_id);
  lang = xl->lang_db->lang;
  ichi = root->ichi;
  ichi->invertgc = cur_p->pe.invertgc;
#endif /* XJUTIL */

  ichi->map = 0;
  ichi->exp = 0;

  ichi->mode = mode;
  ichi->kosuu = kosuu;
  if (init < -1 || init > kosuu)
    init = -1;
  ichi->init = init;
  ichi->title->string = title;
  ichi->subtitle->string = subtitle;
  ichi->button[EXEC_W]->string = exec_title;
  ichi->button[CANCEL_W]->string = (unsigned char *) msg_get (cd, CANCEL_NUM, default_message[CANCEL_NUM], lang);
#ifndef XJUTIL
  ichi->button[CANCEL_W]->cb_data = (int *) cur_x;
#endif /* !XJUTIL */
  ichi->button[MOUSE_W]->string = (unsigned char *) msg_get (cd, MOUSE_NUM, default_message[MOUSE_NUM], lang);

  def_height = FONTHEIGHT * 4;
  if (mode & (SENTAKU | JIKOUHO | JIS_IN | KUTEN_IN))
    {
      if (mode & (JIKOUHO | JIS_IN | KUTEN_IN))
        {
          ichi->button[NEXT_W]->string = (unsigned char *) msg_get (cd, NEXT_NUM, default_message[NEXT_NUM], lang);
          ichi->button[BACK_W]->string = (unsigned char *) msg_get (cd, BACK_NUM, default_message[BACK_NUM], lang);
#ifndef XJUTIL
          ichi->button[NEXT_W]->cb_data = ichi->button[BACK_W]->cb_data = (int *) cur_x;
#endif /* !XJUTIL */
          def_maxlen = columnlen (title)
            + columnlen (ichi->button[CANCEL_W]->string)
            + columnlen (ichi->button[MOUSE_W]->string) + columnlen (ichi->button[NEXT_W]->string) + columnlen (ichi->button[BACK_W]->string) + SPACING * 5 + 1;
          ichi->kosuu_all = xw_set_jikouho (info, kosuu, def_maxlen);
        }
      else if (mode & SENTAKU)
        {
          def_maxlen = columnlen (title) + columnlen (ichi->button[CANCEL_W]->string) + SPACING * 2 + 1;
          if (kosuu > ichi->max_lines)
            alloc_for_save_buf (ichi, kosuu);
          for (cnt = 0; cnt < kosuu; cnt++)
            {
              sprintf ((char *) ichi->save_buf[cnt], " %s ", info[cnt]);
              ichi->buf[cnt] = ichi->save_buf[cnt];
            }
          height = kosuu * (FONTHEIGHT + IN_BORDER) + 2 * FONTHEIGHT;
          ichi->kosuu_all = kosuu;
        }
      if (ichi->kosuu_all > MAX_PAGE_LINES)
        {
          ichi->page = ichi->kosuu_all / MAX_PAGE_LINES + (ichi->kosuu_all % MAX_PAGE_LINES ? 1 : 0);
          ichi->max_line = MAX_PAGE_LINES;
          height = ichi->max_line * (FONTHEIGHT + IN_BORDER) + 2 * FONTHEIGHT;
          ichi->page_height = ichi->max_line * (FONTHEIGHT + IN_BORDER);
          if (mode & SENTAKU)
            {
              def_maxlen += columnlen (ichi->button[NEXT_W]->string) + columnlen (ichi->button[BACK_W]->string) + SPACING * 3;
            }
        }
      else
        {
          ichi->page = 1;
          ichi->max_line = ichi->kosuu_all;
          height = ichi->kosuu_all * (FONTHEIGHT + IN_BORDER) + 2 * FONTHEIGHT;
          ichi->page_height = 0;
        }
      if (comment)
        {
          ichi->comment->string = comment;
          maxlen = columnlen (comment);
          height += FONTHEIGHT + IN_BORDER * 2;
        }
      else
        {
          ichi->comment->string = NULL;
          maxlen = columnlen (info[0]) + 2;
        }
      for (cnt = 1; cnt < ichi->kosuu_all; cnt++)
        {
          if ((tmp = columnlen (ichi->buf[cnt])) > maxlen)
            maxlen = tmp + 2;
        }
      if (maxlen < def_maxlen)
        maxlen = def_maxlen;
    }
  else if (mode & NYUURYOKU)
    {
      def_maxlen = columnlen (title) + columnlen (exec_title) + columnlen (ichi->button[CANCEL_W]->string) + SPACING * 3 + 1;
      if (def_maxlen < (columnlen (subtitle) + nyuu_col + 4))
        maxlen = columnlen (subtitle) + nyuu_col + 4;
      else
        maxlen = def_maxlen;
      height = def_height;
      ichi->kosuu_all = 0;
    }
  else
    {
      maxlen = def_maxlen;
      height = def_height;
    }

  ichi->width = width = ichi->item_width = maxlen * FONTWIDTH;
  ichi->height = height;
  ichi->item_height = ichi->kosuu_all * (FONTHEIGHT + IN_BORDER);
  ichi->max_len = maxlen;

  XQueryPointer (dpy, ROOT_WINDOW, &r, &child, &save_x, &save_y, &win_x, &win_y, &keys_buttons);
  ichi->save_x = (short) save_x;
  ichi->save_y = (short) save_y;
#ifdef XJUTIL
  decide_position (width, height, cur_root->bw, &x_cood, &y_cood);
#else /* XJUTIL */
  decide_position (root, xl, width, height, root->bw, &x_cood, &y_cood);
#endif /* XJUTIL */
#ifdef nodef
  XWarpPointer (dpy, None, root_window, 0, 0, 0, 0, x_cood + FONTWIDTH, y_cood + FONTHEIGHT * 2 + FONTHEIGHT / 2);
#endif

  XMoveResizeWindow (dpy, ichi->w, x_cood - 2, y_cood - 2, width, height);

  moveresize_box (ichi->title, FONTWIDTH, (FONTHEIGHT / 2 - IN_BORDER), (FONTWIDTH * (len_t = columnlen (title)) + IN_BORDER * 2), (FONTHEIGHT + IN_BORDER * 2));
  len_t += 1;
  moveresize_box (ichi->button[CANCEL_W],
                  (FONTWIDTH * (len_t += SPACING)), (FONTHEIGHT / 2 - IN_BORDER), (columnlen (ichi->button[CANCEL_W]->string) * FONTWIDTH + IN_BORDER * 2), (FONTHEIGHT + IN_BORDER * 2));
  len_t += columnlen (ichi->button[CANCEL_W]->string);

#ifndef XJUTIL
  XSetWindowColormap (dpy, ichi->w, cur_p->pe.colormap);
#endif /* !XJUTIL */
  XSetWindowBackground (dpy, ichi->w, BACKGROUND);
  XClearWindow (dpy, ichi->w);
  XSetWindowBorder (dpy, ichi->w, FOREGROUND);

  changecolor_box (ichi->title, FOREGROUND, BACKGROUND, FOREGROUND, BoxAll);
  changecolor_box (ichi->button[CANCEL_W], FOREGROUND, BACKGROUND, FOREGROUND, BoxAll);
  if (!(mode & NYUURYOKU))
    {
      if (comment)
        {
          y_cood = FONTHEIGHT * 3 + (IN_BORDER * 2);
          moveresize_box (ichi->comment, 0, FONTHEIGHT * 2, width, (FONTHEIGHT + IN_BORDER * 2));
          changecolor_box (ichi->comment, FOREGROUND, BACKGROUND, FOREGROUND, BoxAll);
          map_box (ichi->comment);
        }
      else
        {
          y_cood = FONTHEIGHT * 2;
        }
      XMoveResizeWindow (dpy, ichi->w0, -1, y_cood, FONTWIDTH * ichi->max_columns, ichi->max_line * (FONTHEIGHT + IN_BORDER));
      XMoveResizeWindow (dpy, ichi->w1, 0, 0, FONTWIDTH * ichi->max_columns, ichi->kosuu_all * (FONTHEIGHT + IN_BORDER));
#ifndef XJUTIL
      XSetWindowColormap (dpy, ichi->w0, cur_p->pe.colormap);
      XSetWindowColormap (dpy, ichi->w1, cur_p->pe.colormap);
#endif /* !XJUTIL */
      XSetWindowBackground (dpy, ichi->w0, BACKGROUND);
      XClearWindow (dpy, ichi->w0);
      XSetWindowBorder (dpy, ichi->w0, FOREGROUND);
      XSetWindowBackground (dpy, ichi->w1, BACKGROUND);
      XClearWindow (dpy, ichi->w1);
      XSetWindowBorder (dpy, ichi->w1, FOREGROUND);
    }
  if (mode & (JIKOUHO | JIS_IN | KUTEN_IN))
    {
      moveresize_box (ichi->button[MOUSE_W],
                      (FONTWIDTH * (len_t += SPACING)), (FONTHEIGHT / 2 - IN_BORDER), (columnlen (ichi->button[MOUSE_W]->string) * FONTWIDTH + IN_BORDER * 2), (FONTHEIGHT + IN_BORDER * 2));
      len_t += columnlen (ichi->button[MOUSE_W]->string);
      changecolor_box (ichi->button[MOUSE_W], FOREGROUND, BACKGROUND, FOREGROUND, BoxAll);
    }

  set_wm_properties (ichi->w, x_cood, y_cood, width, height, SouthWestGravity, NULL, NULL, 0, 0, 0, 0, 0);
  XRaiseWindow (dpy, ichi->w);
  XMapWindow (dpy, ichi->w);
  map_box (ichi->button[CANCEL_W]);
  XFlush (dpy);
  if (mode & NYUURYOKU)
    {
      moveresize_box (ichi->subtitle, FONTWIDTH, (FONTHEIGHT * 2 + FONTHEIGHT / 2 - IN_BORDER), (columnlen (subtitle) * FONTWIDTH + IN_BORDER * 2), (FONTHEIGHT + IN_BORDER * 2));
      changecolor_box (ichi->subtitle, FOREGROUND, BACKGROUND, FOREGROUND, BoxAll);
      map_box (ichi->subtitle);
      XMoveResizeWindow (dpy, ichi->nyuu_w, FONTWIDTH * (columnlen (subtitle) + 3), FONTHEIGHT * 2 + FONTHEIGHT / 2, nyuu_col * FONTWIDTH, FONTHEIGHT);
#ifndef XJUTIL
      XSetWindowColormap (dpy, ichi->nyuu_w, cur_p->pe.colormap);
#endif /* !XJUTIL */
      XSetWindowBackground (dpy, ichi->nyuu_w, BACKGROUND);
      XClearWindow (dpy, ichi->nyuu_w);
      XSetWindowBorder (dpy, ichi->nyuu_w, FOREGROUND);
      XMapWindow (dpy, ichi->nyuu_w);
      moveresize_box (ichi->button[EXEC_W], (FONTWIDTH * (len_t += SPACING)), (FONTHEIGHT / 2 - IN_BORDER), (columnlen (exec_title) * FONTWIDTH + IN_BORDER * 2), (FONTHEIGHT + IN_BORDER * 2));
      changecolor_box (ichi->button[EXEC_W], FOREGROUND, BACKGROUND, FOREGROUND, BoxAll);
      map_box (ichi->button[EXEC_W]);
      len_t += tmp;
    }
  if (ichi->kosuu_all)
    {
      XMapWindow (dpy, ichi->w0);
      XFlush (dpy);
      ichi->cur_page = 1;
      ichi->start_line = 0;
      ichi->end_line = ichi->kosuu_all - 1;
      if (mode & (JIKOUHO | JIS_IN | KUTEN_IN))
        {
          if (ichi->select_mode)
            {
              XSelectInput (dpy, ichi->w1, EnterWindowMask | LeaveWindowMask | PointerMotionMask | ButtonPressMask | ButtonReleaseMask | ExposureMask);
              XDefineCursor (dpy, ichi->w1, XCreateFontCursor (dpy, 52));
            }
          else
            {
              ichi->button[MOUSE_W]->string = (unsigned char *) msg_get (cd, MOUSE_NUM + 1, default_message[MOUSE_NUM + 1], lang);
              XSelectInput (dpy, ichi->w1, ExposureMask);
            }
          map_box (ichi->button[MOUSE_W]);
          ichi->hilited_ji_item = init;
          if (init != -1 && ichi->page > 1)
            {
              ichi->cur_page = ((jikouho_ichi[init].line - 1) / ichi->max_line) + 1;
              move_ichi (ichi);
            }
        }
      else if (mode & SENTAKU)
        {
          ichi->hilited_item = init;
          if (init != -1 && ichi->page > 1)
            {
              ichi->cur_page = (init / ichi->max_line) + 1;
              move_ichi (ichi);
            }
          XSelectInput (dpy, ichi->w1, EnterWindowMask | LeaveWindowMask | PointerMotionMask | ButtonPressMask | ButtonReleaseMask | ExposureMask);
          XDefineCursor (dpy, ichi->w1, XCreateFontCursor (dpy, 132));
        }
      if (ichi->page > 1)
        {
          moveresize_box (ichi->button[NEXT_W],
                          (FONTWIDTH * (len_t += SPACING)), (FONTHEIGHT / 2 - IN_BORDER), (columnlen (ichi->button[NEXT_W]->string) * FONTWIDTH + IN_BORDER * 2), (FONTHEIGHT + IN_BORDER * 2));
          len_t += columnlen (ichi->button[NEXT_W]->string);
          moveresize_box (ichi->button[BACK_W],
                          (FONTWIDTH * (len_t += SPACING)), (FONTHEIGHT / 2 - IN_BORDER), (columnlen (ichi->button[BACK_W]->string) * FONTWIDTH + IN_BORDER * 2), (FONTHEIGHT + IN_BORDER * 2));
          len_t += columnlen (ichi->button[BACK_W]->string);
          changecolor_box (ichi->button[NEXT_W], FOREGROUND, BACKGROUND, FOREGROUND, BoxAll);
          changecolor_box (ichi->button[BACK_W], FOREGROUND, BACKGROUND, FOREGROUND, BoxAll);
          map_box (ichi->button[NEXT_W]);
          map_box (ichi->button[BACK_W]);
        }
    }
  XFlush (dpy);
  return (0);
}

void
end_ichiran ()
{
  Ichiran *ichi;
  register int i;

#ifndef XJUTIL
  if (!xim->j_c)
    return;
#endif /* !XJUTIL */
  ichi = ICHI;
  if (ichi->kosuu)
    {
      ichi->init = -1;
      if (ichi->mode & SENTAKU && ichi->hilited_item != -1)
        xw_fillrec ();
      ichi->hilited_item = -1;
      if ((ichi->mode & (JIKOUHO | JIS_IN | KUTEN_IN)) && ichi->hilited_ji_item != -1)
        xw_ji_fillrec ();
      ichi->hilited_ji_item = -1;
      XUnmapWindow (dpy, ichi->w0);
    }
  XWithdrawWindow (dpy, ichi->w, DefaultScreen (dpy));
  XFlush (dpy);
  XUndefineCursor (dpy, ichi->w1);
  XSelectInput (dpy, ichi->w1, ExposureMask);
  unmap_box (ichi->subtitle);
  unmap_box (ichi->comment);
  if (ichi->mode & NYUURYOKU)
    XUnmapWindow (dpy, ichi->nyuu_w);
  for (i = 0; i < ichi->max_button; i++)
    {
      unmap_box (ichi->button[i]);
      ichi->button[i]->in = '\0';
    }
#ifdef nodef
  XWarpPointer (dpy, None, ROOT_WINDOW, 0, 0, 0, 0, (int) ichi->save_x, (int) ichi->save_y);
  XFlush (dpy);
#endif
  SEL_RET = -1;
  ichi->lock = 0;
  ichi->mode = 0;
  unfreeze_box (ichi->button[NEXT_W]);
  unfreeze_box (ichi->button[BACK_W]);
#ifdef XJUTIL
  xjutil->mode &= ~ICHIRAN_MODE;
#else /* XJUTIL */
  xim->j_c = NULL;
#endif /* XJUTIL */
}

#ifdef XJUTIL
void
lock_ichiran ()
{
  freeze_box (cur_root->ichi->button[NEXT_W]);
  freeze_box (cur_root->ichi->button[BACK_W]);
  cur_root->ichi->lock = 1;
}

void
unlock_ichiran ()
{
  cur_root->ichi->lock = 0;
  unfreeze_box (cur_root->ichi->button[NEXT_W]);
  unfreeze_box (cur_root->ichi->button[BACK_W]);
}
#endif /* XJUTIL */

#ifndef XJUTIL
void
check_map (w)
     Window w;
{
  register XIMClientRec *xc;
  register XIMInputRec *xi;
  register int i;
  int match = 0;

  if (xim->j_c)
    {
      if (w == xim->cur_j_c_root->ichi->w)
        {
          xim->cur_j_c_root->ichi->map = 1;
          return;
        }
      else if (w == xim->cur_j_c_root->inspect->w)
        {
          xim->cur_j_c_root->inspect->map = 1;
          return;
        }
    }
  if (cur_p && cur_p->yes_no && cur_p->yes_no->w == w)
    {
      cur_p->yes_no->map = 1;
      return;
    }
  for (xc = ximclient_list; xc != NULL; xc = xc->next)
    {
      if (IsPreeditArea (xc) || IsPreeditPosition (xc) || IsPreeditCallbacks (xc))
        {
          if (w == xc->yes_no->w)
            {
              xc->yes_no->map = 1;
              return;
            }
        }
      if (!xc->cur_xl)
        continue;
      for (i = 0; i < xim->screen_count; i++)
        {
          if (xc == xim->root_pointer[i]->ximclient)
            break;
        }
      if (i < xim->screen_count)
        continue;
      if (IsPreeditPosition (xc))
        {
          if (w == xc->cur_xl->wp[0] || w == xc->cur_xl->wp[1] || w == xc->cur_xl->wp[2] || w == xc->cur_xl->wn[0] || w == xc->cur_xl->wn[1] || w == xc->cur_xl->wn[2])
            {
              match = 1;
            }
        }
      else if (IsPreeditArea (xc))
        {
          if (w == xc->cur_xl->wp[0])
            match = 1;
        }
      if (IsStatusArea (xc))
        {
          if (w == xc->cur_xl->ws)
            match = 1;
        }
      if (match)
        {
          for (xi = input_list; xi != NULL; xi = xi->next)
            {
              if (xi->pclient && xi->pclient == xc)
                {
                  return;
                }
            }
#if defined(XJPLIB) && defined(XJPLIB_DIRECT)
          if (XJp_check_map (xc))
            return;
#endif /* defined(XJPLIB) && defined(XJPLIB_DIRECT) */
          XUnmapWindow (dpy, w);
        }
    }
}

void
lock_inspect ()
{
  register Inspect *ins;

  if (!xim->j_c)
    return;
  ins = xim->cur_j_c_root->inspect;
  ins->lock = 1;
  freeze_box (ins->button[DELETE_W]);
  freeze_box (ins->button[USE_W]);
  freeze_box (ins->button[NEXT_W]);
  freeze_box (ins->button[BACK_W]);
}

void
unlock_inspect ()
{
  register Inspect *ins;

  if (!xim->j_c)
    return;
  ins = xim->cur_j_c_root->inspect;
  ins->lock = 0;
  unfreeze_box (ins->button[DELETE_W]);
  unfreeze_box (ins->button[USE_W]);
  unfreeze_box (ins->button[NEXT_W]);
  unfreeze_box (ins->button[BACK_W]);
}

int
init_inspect (msg)
     unsigned char *msg;
{
  register XIMRootRec *root = cur_x->root_pointer;
  int width, height, x_cood, y_cood;
  int save_x, save_y;
  Window r, child;
  int win_x, win_y;
  unsigned int keys_buttons;
  Inspect *ins;
  register XIMLangRec *xl = cur_p->cur_xl;
  char *lang;
  int i, len_t = 0;

  if (xim->j_c != NULL)
    return (-1);
  xim->j_c = cur_p;
  xim->cur_j_c_root = root;
  ins = root->inspect;
  ins->map = 0;
  ins->exp = 0;

  set_cswidth (xl->lang_db->cswidth_id);
  lang = xl->lang_db->lang;
  ins->max_len = columnlen (msg);
  width = FONTWIDTH * ins->max_len;
  height = FONTHEIGHT * 3 + IN_BORDER * 2;

  XQueryPointer (dpy, ROOT_WINDOW, &r, &child, &save_x, &save_y, &win_x, &win_y, &keys_buttons);
  ins->save_x = (short) save_x;
  ins->save_y = (short) save_y;
  decide_position (root, xl, width, height, root->bw, &x_cood, &y_cood);
#ifdef nodef
  XWarpPointer (dpy, None, ROOT_WINDOW, 0, 0, 0, 0, x_cood + width / 2, y_cood + FONTHEIGHT);
#endif
  ins->msg = msg;
  ins->title->string = (unsigned char *) msg_get (cd, 34, default_message[34], lang);
  ins->button[CANCEL_W]->string = (unsigned char *) msg_get (cd, 16, default_message[16], lang);
  ins->button[DELETE_W]->string = (unsigned char *) msg_get (cd, 35, default_message[35], lang);
  ins->button[USE_W]->string = (unsigned char *) msg_get (cd, 36, default_message[36], lang);
  ins->button[NEXT_W]->string = (unsigned char *) msg_get (cd, 37, default_message[37], lang);
  ins->button[BACK_W]->string = (unsigned char *) msg_get (cd, 38, default_message[38], lang);
  ins->button[CANCEL_W]->cb_data = ins->button[DELETE_W]->cb_data = ins->button[USE_W]->cb_data = ins->button[NEXT_W]->cb_data = ins->button[BACK_W]->cb_data = (int *) cur_x;
  moveresize_box (ins->title, FONTWIDTH, (FONTHEIGHT / 2 - IN_BORDER), (FONTWIDTH * (len_t = columnlen (ins->title->string)) + IN_BORDER * 2), (FONTHEIGHT + IN_BORDER * 2));
  len_t += 1;
  for (i = 0; i < ins->max_button; i++)
    {
      moveresize_box (ins->button[i], (FONTWIDTH * (len_t += SPACING)), (FONTHEIGHT / 2 - IN_BORDER), (columnlen (ins->button[i]->string) * FONTWIDTH + IN_BORDER * 2), (FONTHEIGHT + IN_BORDER * 2));
      len_t += columnlen (ins->button[i]->string);
    }

  XMoveResizeWindow (dpy, ins->w, x_cood, y_cood, width, height);

  set_wm_properties (ins->w, x_cood, y_cood, width, height, CenterGravity, NULL, NULL, 0, 0, 0, 0, 0);

  height = FONTHEIGHT + IN_BORDER * 2;
  XMoveResizeWindow (dpy, ins->w1, -1, (FONTHEIGHT * 2), width, height);
  XRaiseWindow (dpy, ins->w);
  XSetWindowColormap (dpy, ins->w, cur_p->pe.colormap);
  XSetWindowBackground (dpy, ins->w, BACKGROUND);
  XSetWindowColormap (dpy, ins->w1, cur_p->pe.colormap);
  XSetWindowBackground (dpy, ins->w1, BACKGROUND);
  XClearWindow (dpy, ins->w);
  XClearWindow (dpy, ins->w1);
  XSetWindowBorder (dpy, ins->w, FOREGROUND);
  XSetWindowBorder (dpy, ins->w1, FOREGROUND);

  changecolor_box (ins->title, FOREGROUND, BACKGROUND, FOREGROUND, BoxAll);
  for (i = 0; i < ins->max_button; i++)
    {
      changecolor_box (ins->button[i], FOREGROUND, BACKGROUND, FOREGROUND, BoxAll);
    }
  XMapWindow (dpy, ins->w);
  XFlush (dpy);
  JW3Mputc (ins->msg, ins->w1, 0, 0, IN_BORDER);
  XFlush (dpy);
  return (0);
}

void
end_inspect ()
{
  Inspect *ins;
  int i;

  if (!xim->j_c)
    return;
  ins = xim->cur_j_c_root->inspect;
  XWithdrawWindow (dpy, ins->w, DefaultScreen (dpy));
#ifdef nodef
  XWarpPointer (dpy, None, ROOT_WINDOW, 0, 0, 0, 0, (int) ins->save_x, (int) ins->save_y);
#endif
  XFlush (dpy);
  for (i = 0; i < ins->max_button; i++)
    {
      ins->button[i]->in = '\0';
    }
  xim->sel_ret = -1;
  unlock_inspect ();
  xim->j_c = NULL;
}

static void
change_inspect (xl, s)
     register XIMLangRec *xl;
     register unsigned char *s;
{
  register Inspect *ins;
  int len;
  unsigned int width, height;

  if (!xim->j_c)
    return;
  ins = xim->cur_j_c_root->inspect;
  if ((len = columnlen (s)) > ins->max_len)
    {
      XResizeWindow (dpy, ins->w, (width = FONTWIDTH * len), (height = FONTHEIGHT * 3 + IN_BORDER * 2));
      XResizeWindow (dpy, ins->w1, FONTWIDTH * len, FONTHEIGHT + IN_BORDER * 2);
      ins->max_len = len;
      set_wm_size (ins->w, width, height);
    }
  ins->msg = s;
  XClearWindow (dpy, ins->w1);
  JW3Mputc (ins->msg, ins->w1, 0, 0, IN_BORDER);
  XFlush (dpy);
}

int
xw_next_inspect ()
{
  unsigned char *s;
  register XIMLangRec *xl;

  if (xim->j_c == NULL)
    return (-1);
  xl = xim->j_c->cur_xl;
  cur_cl_change3 (xim->j_c);

  set_cswidth (xl->lang_db->cswidth_id);
  s = next_inspect (s);
  change_inspect (xl, s);
  return (0);
}

int
xw_back_inspect ()
{
  unsigned char *s;
  register XIMLangRec *xl;

  if (xim->j_c == NULL)
    return (-1);
  xl = xim->j_c->cur_xl;
  cur_cl_change3 (xim->j_c);

  set_cswidth (xl->lang_db->cswidth_id);
  s = previous_inspect ();
  change_inspect (xl, s);
  return (0);
}
#endif /* !XJUTIL */

static int
serch_jikouho_table (x, y)
     int x, y;
{
  register int tmp, gap, kosuu;

#ifndef XJUTIL
  if (!xim->j_c)
    return (-1);
#endif /* !XJUTIL */
  kosuu = ICHI->kosuu;
  for (tmp = kosuu / 2, gap = kosuu / 4; (tmp >= 0 && tmp < kosuu); gap /= 2)
    {
      if (gap == 0)
        gap = 1;
      if (y > jikouho_ichi[tmp].y_e)
        {
          tmp += gap;
        }
      else if (y < jikouho_ichi[tmp].y_s)
        {
          tmp -= gap;
        }
      else
        {
          for (; tmp >= 0 && tmp < kosuu;)
            {
              if (x > jikouho_ichi[tmp].x_e)
                {
                  tmp++;
                }
              else if (x < jikouho_ichi[tmp].x_s)
                {
                  tmp--;
                }
              else if (y <= jikouho_ichi[tmp].y_e && y >= jikouho_ichi[tmp].y_s)
                {
                  return (tmp);
                }
              else
                {
                  return (-1);
                }
            }
          return (-1);
        }
    }
  return (-1);
}

void
xw_move_hilite (d)
     int d;
{
  static int y;
  register int item;
  register Ichiran *ichi;

#ifndef XJUTIL
  if (!xim->j_c)
    return;
#endif /* !XJUTIL */
  ichi = ICHI;
#ifdef XJUTIL
  if (ICHI->lock)
    return;
#endif /* XJUTIL */
  y = d;
  item = Y_InItem (y);
  if (item != -1 && item != ichi->hilited_item && item + (ichi->cur_page - 1) * ichi->max_line < ichi->kosuu)
    {
      if (ichi->hilited_item != -1)
        xw_fillrec ();
      if ((ichi->hilited_item = item) != -1)
        xw_fillrec ();
    }
}

void
xw_jikouho_move_hilite (d_x, d_y)
     int d_x, d_y;
{
  register int item;
  register Ichiran *ichi;

#ifndef XJUTIL
  if (!xim->j_c)
    return;
#endif /* !XJUTIL */
  ichi = ICHI;
  item = serch_jikouho_table (d_x, d_y);
  if (item != -1 && item != ichi->hilited_ji_item)
    {
      if (ichi->hilited_ji_item != -1)
        xw_ji_fillrec ();
      if ((ichi->hilited_ji_item = item) != -1)
        {
          xw_ji_fillrec ();
        }
    }
}

int
alloc_for_save_buf (ichi, line)
     register Ichiran *ichi;
     int line;
{
  register int i;
  register char *p;

  if (!(p = Malloc ((unsigned) (line * (ichi->max_columns * sizeof (char) + sizeof (char *))))))
    {
      malloc_error ("allocation of ichiran data");
      return (-1);
    }
  if (ichi->save_buf)
    Free (ichi->save_buf);
  ichi->save_buf = (unsigned char **) p;
  p += line * sizeof (char *);
  for (i = 0; i < line; i++)
    {
      ichi->save_buf[i] = (unsigned char *) p;
      p += ichi->max_columns * sizeof (char);
    }
  ichi->max_lines = line;
  return (0);
}

void
redraw_ichi_w ()
{
  register Ichiran *ichi;
  register int line = 0;

#ifndef XJUTIL
  if (!xim->j_c)
    return;
#endif /* !XJUTIL */
  ichi = ICHI;
  XClearWindow (dpy, ichi->w1);
  XFlush (dpy);
  for (line = ichi->start_line; line <= ichi->end_line && line < ichi->kosuu_all; line++)
    {
      JW3Mputc (ichi->buf[line], ichi->w1, line, 0, IN_BORDER);
    }
  if (ichi->mode & SENTAKU && ichi->hilited_item != -1)
    {
      xw_fillrec ();
    }
  if (ichi->mode & (JIKOUHO | JIS_IN | KUTEN_IN) && ichi->hilited_ji_item != -1)
    {
      xw_ji_fillrec ();
    }
  XFlush (dpy);
}

int
next_ichi ()
{
  register Ichiran *ichi;

#ifndef XJUTIL
  if (!xim->j_c)
    return (-1);
#endif /* !XJUTIL */
  ichi = ICHI;
  if (ichi->page < 2)
    return (-1);
  if (++ichi->cur_page > ichi->page)
    ichi->cur_page = 1;
  next_back_ichi (ichi, 1);
  return (0);
}

int
back_ichi ()
{
  register Ichiran *ichi;

#ifndef XJUTIL
  if (!xim->j_c)
    return (-1);
#endif /* !XJUTIL */
  ichi = ICHI;
  if (ichi->page < 2)
    return (-1);
  if (--ichi->cur_page < 1)
    ichi->cur_page = ichi->page;
  next_back_ichi (ichi, 0);
  return (0);
}

int
find_hilite ()
{
#ifndef XJUTIL
  if (!xim->j_c)
    return (-1);
#endif /* !XJUTIL */
  return (ICHI->hilited_item);
}

int
find_ji_hilite ()
{
#ifndef XJUTIL
  if (!xim->j_c)
    return (-1);
#endif /* !XJUTIL */
  return (ICHI->hilited_ji_item);
}

void
xw_forward_select ()
{
  register Ichiran *ichi;

#ifndef XJUTIL
  if (!xim->j_c)
    return;
#endif /* !XJUTIL */
  ichi = ICHI;
  if (ichi->mode & (JIKOUHO | JIS_IN | KUTEN_IN))
    {
      if (ichi->hilited_ji_item != -1)
        {
          xw_ji_fillrec ();
        }
      if (ichi->hilited_ji_item < ichi->kosuu - 1)
        {
          ichi->hilited_ji_item += 1;
        }
      else
        {
          ichi->hilited_ji_item = 0;
        }
      check_move_ichi (jikouho_ichi[ichi->hilited_ji_item].line);
      xw_ji_fillrec ();
    }
}

void
xw_backward_select ()
{
  register Ichiran *ichi;

#ifndef XJUTIL
  if (!xim->j_c)
    return;
#endif /* !XJUTIL */
  ichi = ICHI;
  if (ichi->mode & (JIKOUHO | JIS_IN | KUTEN_IN))
    {
      if (ichi->hilited_ji_item != -1)
        {
          xw_ji_fillrec ();
        }
      if (ichi->hilited_ji_item > 0)
        {
          ichi->hilited_ji_item -= 1;
        }
      else
        {
          ichi->hilited_ji_item = ichi->kosuu - 1;
        }
      check_move_ichi (jikouho_ichi[ichi->hilited_ji_item].line);
      xw_ji_fillrec ();
    }
}

int
xw_next_select ()
{
  register Ichiran *ichi;
  register int next, next_l, next_l2;
  register int cur_l;

#ifndef XJUTIL
  if (!xim->j_c)
    return (-1);
#endif /* !XJUTIL */
  ichi = ICHI;
  if (ichi->lock)
    return (0);
  cur_l = jikouho_ichi[ichi->hilited_ji_item].line;

  if (ichi->mode & (JIKOUHO | JIS_IN | KUTEN_IN))
    {
      if (ichi->max_line == 1)
        return (0);
      if (ichi->hilited_ji_item != -1)
        xw_ji_fillrec ();
      if (cur_l > ichi->kosuu_all - 1)
        {
          next_l = jikouho_line[1];
          next_l2 = jikouho_line[2];
        }
      else
        {
          next_l = jikouho_line[cur_l + 1];
          next_l2 = jikouho_line[cur_l + 2];
        }
      next = ichi->hilited_ji_item - jikouho_line[cur_l] + next_l;
      if (next >= next_l2)
        {
          ichi->hilited_ji_item = next_l2 - 1;
        }
      else
        {
          ichi->hilited_ji_item = next;
        }
      check_move_ichi (jikouho_ichi[ichi->hilited_ji_item].line);
      xw_ji_fillrec ();
    }
  else if (ichi->mode & SENTAKU)
    {
      if (ichi->max_line == 1)
        return (0);
      if (ichi->hilited_item != -1)
        xw_fillrec ();
      if ((ichi->hilited_item == ichi->end_line) || (ichi->hilited_item == ichi->kosuu - 1))
        {
          if (ichi->hilited_item == ichi->kosuu - 1)
            {
              ichi->hilited_item = 0;
            }
          else
            {
              ichi->hilited_item += 1;
            }
          if (ichi->page > 1)
            next_ichi ();
          else
            xw_fillrec ();
        }
      else
        {
          ichi->hilited_item += 1;
          xw_fillrec ();
        }
    }
  return (0);
}

int
xw_previous_select ()
{
  register Ichiran *ichi = ICHI;

  register int prev, prev_l, prev_l2;
  register int cur_l;

#ifndef XJUTIL
  if (!xim->j_c)
    return (-1);
#endif /* !XJUTIL */
  ichi = ICHI;
  if (ichi->lock)
    return (0);
  cur_l = jikouho_ichi[ichi->hilited_ji_item].line;

  if (ichi->mode & (JIKOUHO | JIS_IN | KUTEN_IN))
    {
      if (ichi->max_line == 1)
        return (0);
      if (ichi->hilited_ji_item != -1)
        xw_ji_fillrec ();
      if (cur_l == 1)
        {
          prev_l = jikouho_line[ichi->max_line];
          prev_l2 = jikouho_line[ichi->max_line + 1];
        }
      else
        {
          prev_l = jikouho_line[cur_l - 1];
          prev_l2 = jikouho_line[cur_l];
        }
      prev = ichi->hilited_ji_item - jikouho_line[cur_l] + prev_l;
      if (prev >= prev_l2)
        {
          ichi->hilited_ji_item = prev_l2 - 1;
        }
      else
        {
          ichi->hilited_ji_item = prev;
        }
      check_move_ichi (jikouho_ichi[ichi->hilited_ji_item].line);
      xw_ji_fillrec ();
    }
  else if (ichi->mode & SENTAKU)
    {
      if (ichi->max_line == 1)
        return (0);
      if (ichi->hilited_item != -1)
        xw_fillrec ();
      if ((ichi->hilited_item % ichi->max_line) == 0)
        {
          if (ichi->cur_page == 1)
            {
              ichi->hilited_item = ichi->kosuu - 1;
            }
          else
            {
              ichi->hilited_item -= 1;
            }
          if (ichi->page > 1)
            back_ichi ();
          else
            xw_fillrec ();
        }
      else
        {
          ichi->hilited_item -= 1;
          xw_fillrec ();
        }
    }
  return (0);
}

void
xw_linestart_select ()
{
  register Ichiran *ichi;

#ifndef XJUTIL
  if (!xim->j_c)
    return;
#endif /* !XJUTIL */
  ichi = ICHI;
  if (ichi->mode & (JIKOUHO | JIS_IN | KUTEN_IN))
    {
      if (ichi->hilited_ji_item != -1)
        xw_ji_fillrec ();
      ichi->hilited_ji_item = jikouho_line[jikouho_ichi[ichi->hilited_ji_item].line];
      check_move_ichi (jikouho_ichi[ichi->hilited_ji_item].line);
      xw_ji_fillrec ();
    }
}

void
xw_lineend_select ()
{
  register Ichiran *ichi;

#ifndef XJUTIL
  if (!xim->j_c)
    return;
#endif /* !XJUTIL */
  ichi = ICHI;
  if (ichi->mode & (JIKOUHO | JIS_IN | KUTEN_IN))
    {
      if (ichi->hilited_ji_item != -1)
        xw_ji_fillrec ();
      ichi->hilited_ji_item = jikouho_line[jikouho_ichi[ichi->hilited_ji_item].line + 1] - 1;
      check_move_ichi (jikouho_ichi[ichi->hilited_ji_item].line);
      xw_ji_fillrec ();
    }
}

int
init_yes_or_no (str, flg)
     unsigned char *str;
     int flg;
{
  int len, def_width, width, height, x_cood, y_cood;
  Window r, child;
  int win_x, win_y;
  unsigned int keys_buttons;
#ifdef XJUTIL
  register YesOrNo *yes_no = cur_root->yes_no;
#else /* XJUTIL */
  register YesOrNo *yes_no = cur_p->yes_no;
  register XIMLangRec *xl = cur_p->cur_xl;
#endif /* XJUTIL */
  char *lang;

#ifdef XJUTIL
  lang = xjutil->lang;
#else /* XJUTIL */
  xim->cur_j_c_root = cur_x->root_pointer;
  set_cswidth (xl->lang_db->cswidth_id);
  lang = xl->lang_db->lang;
#endif /* XJUTIL */
  yes_no->map = 0;
  yes_no->exp = 0;

  yes_no->mode = flg;
  yes_no->title->string = str;

  if (flg != MessageOnly)
    {
      if (flg == YesMessage)
        {
          yes_no->button[0]->string = (unsigned char *) msg_get (cd, OK_NUM, default_message[OK_NUM], lang);
        }
      else if (flg == YesNoMessage)
        {
          yes_no->button[0]->string = (unsigned char *) msg_get (cd, YES_NUM, default_message[YES_NUM], lang);
          yes_no->button[1]->string = (unsigned char *) msg_get (cd, NO_NUM, default_message[NO_NUM], lang);
        }
#ifndef XJUTIL
      yes_no->button[0]->cb_data = yes_no->button[1]->cb_data = (int *) cur_x;
#endif /* !XJUTIL */
      moveresize_box (yes_no->button[0], (FONTWIDTH * 2),
                      (FONTHEIGHT * 2 + FONTHEIGHT / 2 - IN_BORDER), ((len = columnlen (yes_no->button[0]->string)) * FONTWIDTH + IN_BORDER * 2), (FONTHEIGHT + IN_BORDER * 2));
      changecolor_box (yes_no->button[0], FOREGROUND, BACKGROUND, FOREGROUND, BoxAll);
      map_box (yes_no->button[0]);
      def_width = FONTWIDTH * (len + 8);
      if (flg == YesNoMessage)
        {
          moveresize_box (yes_no->button[1], (FONTWIDTH * (6 + len)),
                          (FONTHEIGHT * 2 + FONTHEIGHT / 2 - IN_BORDER), (FONTWIDTH * columnlen (yes_no->button[1]->string) + IN_BORDER * 2), (FONTHEIGHT + IN_BORDER * 2));
          changecolor_box (yes_no->button[1], FOREGROUND, BACKGROUND, FOREGROUND, BoxAll);
          map_box (yes_no->button[1]);
          len += columnlen (yes_no->button[1]->string);
          def_width = FONTWIDTH * (len + 8);
        }
      else
        {
          def_width = FONTWIDTH * (len + 4);
        }
      height = (FONTHEIGHT * 4);
    }
  else
    {
      def_width = 0;
      height = (FONTHEIGHT * 2);
    }
  if ((width = FONTWIDTH * ((len = columnlen (str)) + 2)) < def_width)
    width = def_width;
  moveresize_box (yes_no->title, FONTWIDTH, (FONTHEIGHT / 2 - IN_BORDER), (FONTWIDTH * len + IN_BORDER * 2), (FONTHEIGHT + IN_BORDER * 2));
  changecolor_box (yes_no->title, FOREGROUND, BACKGROUND, FOREGROUND, BoxAll);
  map_box (yes_no->title);

  if (flg != MessageOnly)
    {
      XQueryPointer (dpy, ROOT_WINDOW, &r, &child, &yes_no->x, &yes_no->y, &win_x, &win_y, &keys_buttons);
    }
  x_cood = (DisplayWidth (dpy, SCREEN) - width) / 2;
  y_cood = (DisplayHeight (dpy, SCREEN) - height) / 2;
  if ((x_cood + width) > DisplayWidth (dpy, SCREEN))
    x_cood = DisplayWidth (dpy, SCREEN) - width;
  if (x_cood < 0)
    x_cood = 0;
  if ((y_cood + height) > DisplayHeight (dpy, SCREEN))
    y_cood = DisplayHeight (dpy, SCREEN) - height;
  if (y_cood < 0)
    y_cood = 0;
  if (flg != MessageOnly)
    {
      XWarpPointer (dpy, None, ROOT_WINDOW, 0, 0, 0, 0, (x_cood + FONTWIDTH * 2), (y_cood + FONTHEIGHT * 3));
    }
  XMoveResizeWindow (dpy, yes_no->w, x_cood, y_cood, width, height);
#ifndef XJUTIL
  XSetWindowColormap (dpy, yes_no->w, cur_p->pe.colormap);
#endif /* !XJUTIL */
  XSetWindowBackground (dpy, yes_no->w, BACKGROUND);
  XClearWindow (dpy, yes_no->w);
  XSetWindowBorder (dpy, yes_no->w, FOREGROUND);

  set_wm_properties (yes_no->w, x_cood, y_cood, width, height, CenterGravity, NULL, NULL, 0, 0, 0, 0, 0);
  XRaiseWindow (dpy, yes_no->w);
  XMapWindow (dpy, yes_no->w);
  XFlush (dpy);
  return (0);
}

void
end_yes_or_no ()
{
#ifdef XJUTIL
  register YesOrNo *yes_no = cur_root->yes_no;
#else /* XJUTIL */
  register YesOrNo *yes_no = cur_p->yes_no;
#endif /* XJUTIL */

  unmap_box (yes_no->button[0]);
  unmap_box (yes_no->button[1]);
  XWithdrawWindow (dpy, yes_no->w, DefaultScreen (dpy));
  if (yes_no->mode != MessageOnly)
    {
#ifdef XJUTIL
      XWarpPointer (dpy, None, cur_root->root_window,
#else /* XJUTIL */
      XWarpPointer (dpy, None, yes_no->root_pointer->root_window,
#endif /* XJUTIL */
                    0, 0, 0, 0, yes_no->x, yes_no->y);
    }
  yes_no->mode = 0;
  XFlush (dpy);
  SEL_RET = -1;
  yes_no->button[0]->in = yes_no->button[1]->in = 0;
}

#ifndef XJUTIL
int
set_j_c ()
{
  if (xim->j_c)
    {
      cur_cl_change3 (xim->j_c);
    }
  return (0);
}

int
cur_cl_change3 (xc)
     XIMClientRec *xc;
{
  if (xc == 0)
    {
      return (0);
    }
  else
    {
      if (IsPreeditNothing (xc))
        {
          cur_p = xc->root_pointer->ximclient;
          cur_x = xc;
          c_c = cur_p->cur_xl->w_c;
        }
      else
        {
          cur_p = cur_x = xc;
          c_c = xc->cur_xl->w_c;
        }
    }
  cur_lang = cur_p->cur_xl->lang_db;
  cur_rk = c_c->rk;
  cur_rk_table = cur_rk->rk_table;
  cur_input = 0;
  return (0);
}

int
cur_cl_change4 (xc)
     XIMClientRec *xc;
{
  xc->yes_no->exp = 1;
  cur_cl_change3 (xc);
  return (0);
}
#endif /* !XJUTIL */

void
xw_select_button (event)
     XButtonEvent *event;
{
  static int y;
  register int item;
  register Ichiran *ichi;

#ifndef XJUTIL
  if (!xim->j_c)
    return;
#endif /* !XJUTIL */
  ichi = ICHI;
  y = event->y;
  if ((item = Y_InItem (y)) != ichi->hilited_item)
    {
      if (ichi->hilited_item != -1)
        xw_fillrec ();
      if ((ichi->hilited_item = item) != -1)
        xw_fillrec ();
    }
  XFlush (dpy);
  SEL_RET = item;
}

void
xw_select_jikouho_button (event)
     XButtonEvent *event;
{
  register int item;
  register Ichiran *ichi;

#ifndef XJUTIL
  if (!xim->j_c)
    return;
#endif /* !XJUTIL */
  ichi = ICHI;
  if ((item = serch_jikouho_table (event->x, event->y)) != ichi->hilited_ji_item)
    {
      if (ichi->hilited_ji_item != -1)
        xw_ji_fillrec ();
      if ((ichi->hilited_ji_item = item) != -1)
        xw_ji_fillrec ();
    }
  XFlush (dpy);
  SEL_RET = item;
}

void
xw_mouseleave ()
{
  register Ichiran *ichi;

#ifndef XJUTIL
  if (!xim->j_c)
    return;
#endif /* !XJUTIL */
  ichi = ICHI;
#ifdef XJUTIL
  if (ichi->lock)
    return;
#endif /* XJUTIL */
  if (ichi->kosuu == 1)
    {
      if (ichi->hilited_item != -1)
        {
          xw_fillrec ();
          ichi->hilited_item = -1;
        }
    }
}

int
xw_mouse_select ()
{
  register Ichiran *ichi;
  register char *lang;

#ifdef XJUTIL
  lang = xjutil->lang;
#else /* XJUTIL */
  if (!xim->j_c)
    return (-1);
  lang = cur_p->cur_xl->lang_db->lang;
#endif /* XJUTIL */
  ichi = ICHI;

  if (ichi->select_mode == '\0')
    {
      XSelectInput (dpy, ichi->w1, (EnterWindowMask | LeaveWindowMask | PointerMotionMask | ButtonPressMask | ButtonReleaseMask | ExposureMask));
      XDefineCursor (dpy, ichi->w1, XCreateFontCursor (dpy, 52));
      ichi->button[MOUSE_W]->string = (unsigned char *) msg_get (cd, MOUSE_NUM, default_message[MOUSE_NUM], lang);
      ichi->select_mode = 1;
    }
  else
    {
      XSelectInput (dpy, ichi->w1, ExposureMask);
      XUndefineCursor (dpy, ichi->w1);
      ichi->button[MOUSE_W]->string = (unsigned char *) msg_get (cd, MOUSE_NUM + 1, default_message[MOUSE_NUM + 1], lang);
      ichi->select_mode = '\0';
    }
  JW3Mputc (ichi->button[MOUSE_W]->string, ichi->button[MOUSE_W]->window, 0, REV_FLAG, IN_BORDER);
  XFlush (dpy);
  return (0);
}

void
draw_nyuu_w (buf, rev)
     unsigned char *buf;
     int rev;
{
  register Ichiran *ichi;

#ifndef XJUTIL
  if (!xim->j_c)
    return;
#endif /* !XJUTIL */
  ichi = ICHI;
  ichi->nyuu = (unsigned char *) buf;
  JW3Mputc (buf, ichi->nyuu_w, 0, (rev ? REV_FLAG : 0), 0);
}

void
clear_nyuu_w ()
{
#ifndef XJUTIL
  if (!xim->j_c)
    return;
#endif /* !XJUTIL */
  XClearWindow (dpy, ICHI->nyuu_w);
}

#ifndef XJUTIL
int
yes_or_no_lock (string)
     char *string;
{
  int c1;

  init_yes_or_no ((unsigned char *) string, YesNoMessage);
  for (;;)
    {
      c1 = get_yes_no_event ();
      if (!cur_p->yes_no->map)
        {
          xim->sel_ret = -1;
          continue;
        }
      if ((xim->sel_ret == 1) || (xim->sel_ret == -1 && (c1 == NEWLINE || c1 == CR || c1 == 'y' || c1 == 'Y')))
        {
          end_yes_or_no ();
          xim->sel_ret = -1;
          return (1);
        }
      else if ((xim->sel_ret == 0) || (xim->sel_ret == -1 && (c1 == 'n' || c1 == 'N')))
        {
          end_yes_or_no ();
          xim->sel_ret = -1;
          return (0);
        }
      else
        {
          xim->sel_ret = -1;
          continue;
        }
    }
}

int
yes_or_no (string, in)
     char *string;
     int in;
{
  int ret;
  static yes_no_step = 0;

  if (in == -99)
    {
      yes_no_step = 0;
      return (-1);
    }
  if (yes_no_step == 0)
    {
      init_yes_or_no ((unsigned char *) string, YesNoMessage);
      yes_no_step++;
      return (BUFFER_IN_CONT);
    }
  if (yes_no_step == 1)
    {
      if (!cur_p->yes_no->map)
        {
          xim->sel_ret = -1;
          return (BUFFER_IN_CONT);
        }
      if ((xim->sel_ret == 1) || (xim->sel_ret == -1 && (in == NEWLINE || in == CR || in == 'y' || in == 'Y')))
        {
          ret = 1;
        }
      else if ((xim->sel_ret == 0) || (xim->sel_ret == -1 && (in == 'n' || in == 'N')))
        {
          ret = 0;
        }
      else
        {
          xim->sel_ret = -1;
          return (BUFFER_IN_CONT);
        }
      end_yes_or_no ();
      xim->sel_ret = -1;
      yes_no_step = 0;
      return (ret);
    }
  return (BUFFER_IN_CONT);
}
#endif /* !XJUTIL */

#ifdef XJUTIL
void
change_ichi_buf (k, col, p)
     int k, col;
     char *p;
{
  register Ichiran *ichi = cur_root->ichi;

  bcopy (p, ichi->buf[k] + col, strlen (p));
  JW3Mputc (ichi->buf[k], ichi->w1, k, ((k == ichi->hilited_item) ? REV_FLAG : 0), IN_BORDER);
}

void
insert_space_in_ichi_buf (p, space_len)
     int p, space_len;
{
  register Ichiran *ichi = cur_root->ichi;
  register i, j, len, save_len, max_len = 0;
  register unsigned char *s;
  char buf[MAX_ICHI_COLUMNS];

  for (i = 0; i < ichi->kosuu_all; i++)
    {
      s = ichi->buf[i] + p;
      bcopy (s, buf, (save_len = strlen (s)));
      for (j = space_len; j > 0; *s++ = ' ', j--);
      bcopy (buf, s, save_len);
      s[save_len] = '\0';
      len = strlen (ichi->buf[i]);
      if (len > max_len)
        max_len = len;
    }
  ichi->max_len = max_len;
  ichi->width = ichi->item_width = max_len * FONTWIDTH;
  XResizeWindow (dpy, ichi->w, ichi->width, ichi->height);
  set_wm_size (ichi->w, ichi->width, ichi->height);
}
#endif /* XJUTIL */
