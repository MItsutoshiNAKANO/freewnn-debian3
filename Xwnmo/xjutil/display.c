/*
 * $Id: display.c,v 1.2 2001/06/14 18:16:12 ura Exp $
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
#include "xjutil.h"
#include "sxheader.h"
#include "xext.h"

extern wchar_t dol_wchar_t;

wchar cursor_wchar = 0x20202020;
GC currentGC;

static void
JWwindow_move (text)
     register JutilTextRec *text;
{

  if ((check_mb (text->buf, text->vst)) == 2)
    text->vst++;
  XMoveWindow (dpy, text->w, -(text->vst * FontWidth), 0);
  XFlush (dpy);
}

static void
JWM_note (which)
     int which;
{
  if (!cur_text->note[which])
    {
      XMapWindow (dpy, cur_text->wn[which]);
      XRaiseWindow (dpy, cur_text->wn[which]);
      XwcDrawImageString (dpy, cur_text->wn[which], cur_fs->fs, cur_root->gc, 0, FontAscent, &dol_wchar_t, 1);
      XFlush (dpy);
    }
}

static void
invisual_note (which)
     int which;
{
  if (cur_text->note[which])
    {
      XUnmapWindow (dpy, cur_text->wn[which]);
      XFlush (dpy);
    }
}

static void
check_move (text, mb)
     register JutilTextRec *text;
     int mb;
{
  if ((text->currentcol - text->vst + mb) >= text->max_columns)
    {
      text->vst = text->currentcol - text->max_columns + mb + 1;
      JWwindow_move (text);
    }
  else if (text->currentcol < text->vst)
    {
      text->vst = text->currentcol;
      JWwindow_move (text);
    }
  if (text->vst)
    {
      JWM_note (0);
      text->note[0] = 1;
    }
  else
    {
      invisual_note (0);
      text->note[0] = 0;
    }
  if (text->max_pos > (text->vst + text->max_columns))
    {
      JWM_note (1);
      text->note[1] = 1;
      if ((check_mb (text->buf, text->vst + text->max_columns - 1)) == 1)
        XMapWindow (dpy, text->wn[2]);
      else
        XUnmapWindow (dpy, text->wn[2]);
    }
  else
    {
      invisual_note (1);
      text->note[1] = 0;
      XUnmapWindow (dpy, text->wn[2]);
    }
}

static void
visual_window ()
{
  XMapWindow (dpy, cur_text->wp);
  XMapWindow (dpy, cur_text->w);
  XFlush (dpy);
}

void
JWmark_cursor (on)
     int on;
{
  register JutilTextRec *text;
  wchar *JW_buf;
  unsigned char *JW_att;
  int mb_len;
  char flg;
  XCharStruct cs;

  text = cur_text;
  JW_buf = text->buf + text->currentcol;
  JW_att = text->att + text->currentcol;
  visual_window ();
  cs.width = FontWidth;
  cs.ascent = FontAscent;
  cs.descent = FontDescent;

  if (text->cursor_flag != 1 || text->mark_flag == on)
    return;
  text->mark_flag = on;

  if ((*JW_att & REV_FLAG) != 0)
    return;

  if (*JW_buf)
    {
      if (*JW_buf == PENDING_WCHAR)
        {
          JW_buf++;
          JW_att++;
        }
      mb_len = XwcGetColumn (*JW_buf);
      flg = *JW_att;
    }
  else
    {
      JW_buf = &cursor_wchar;
      JW_att = 0;
      mb_len = 1;
      flg = 0;
    }
  if (on)
    {
      if (mb_len > 1)
        {
          check_move (text, 1);
        }
      else
        {
          check_move (text, 0);
        }
      currentGC = cur_root->reversegc;
      flg |= REV_FLAG;
    }
  else
    {
      currentGC = cur_root->gc;
    }
  JWOutput (text->w, cur_fs->fs, currentGC, text->currentcol, mb_len, flg, 0, 0, &cs, JW_buf, 1);
  XFlush (dpy);
}

void
JW1Mputwc (wc, wc_len)
     register wchar *wc;
     int wc_len;
{
  register int cnt = wc_len;
  register JutilTextRec *text;
  register wchar *JW_buf, *p = wc;
  unsigned char *JW_att;
  int mb_len;
  short col = 0;
  register int flg = 0;
  XCharStruct cs;

  if (wc_len <= 0)
    return;
  text = cur_text;
  visual_window ();
  if (text->currentcol >= text->maxcolumns)
    return;
  JW_buf = text->buf + text->currentcol;
  JW_att = text->att + text->currentcol;
  if (text->r_flag)
    flg |= REV_FLAG;
  if (text->b_flag)
    flg |= BOLD_FLAG;
  if (text->u_line_flag)
    flg |= UNDER_FLAG;

  cs.width = FontWidth;
  cs.ascent = FontAscent;
  cs.descent = FontDescent;

  for (; cnt > 0; cnt--, JW_buf++, JW_att++)
    {
      mb_len = XwcGetColumn (*p);
      if (mb_len > 1)
        {
          *(JW_buf++) = PENDING_WCHAR;
          *(JW_att++) = flg;
        }
      *JW_buf = *p++;
      *JW_att = flg;
      col += mb_len;
      if (flg & REV_FLAG)
        {
          if (mb_len)
            {
              check_move (text, 1);
            }
          else
            {
              check_move (text, 0);
            }
        }
    }
  JWOutput (text->w, cur_fs->fs, ((flg & REV_FLAG) ? cur_root->reversegc : cur_root->gc), text->currentcol, col, flg, 0, 0, &cs, wc, wc_len);
  text->currentcol += col;
}

void
JWMflushw_buf (w_buf, len)
     w_char *w_buf;
     int len;
{
  int wc_len;

  if (len <= 0)
    return;
  while (1)
    {
      wc_len = w_char_to_wchar (xjutil->xlc, w_buf, wc_buf, len, wc_buf_max);
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
  JW1Mputwc (wc_buf, wc_len);
  if (cur_text->currentcol > cur_text->max_pos)
    {
      cur_text->max_pos = cur_text->currentcol;
    }
  XFlush (dpy);
}

void
jw_disp_mode (mode)
     unsigned char *mode;
{
  if (xjutil->mode & JUTIL_MODE)
    {
      cur_root->jutil->rk_mode->string = mode;
      redraw_box (cur_root->jutil->rk_mode);
    }
  return;
}

void
JWMline_clear (x)
     register int x;
{
  register JutilTextRec *text;
  register int cnt;
  register wchar *JW_buf;
  unsigned char *JW_att;

  text = cur_text;
  JW_buf = text->buf + x;
  JW_att = text->att + x;
  XClearArea (dpy, text->w, x * FontWidth, 0, (text->max_pos - x) * FontWidth + 1, 0, False);
  if (x <= (text->vst + text->max_columns))
    {
      invisual_note (1);
      text->note[1] = 0;
      XUnmapWindow (dpy, text->wn[2]);
    }
  for (cnt = x; cnt < text->max_pos; cnt++)
    {
      *JW_buf++ = 0;
      *JW_att++ = 0;
    }
  text->currentcol = x;
  text->max_pos = x;
  XFlush (dpy);
}

void
JWcursor_visible ()
{
  cur_text->cursor_flag = 1;
  JWmark_cursor (1);
}

void
JWcursor_invisible ()
{
  JWmark_cursor (0);
  cur_text->cursor_flag = 0;
}

void
JWcursor_move (x)
     int x;
{
  visual_window ();
  JWmark_cursor (0);
  cur_text->currentcol = x;
  if (x >= 0)
    JWmark_cursor (1);
}

void
redraw_text (tx, x, width)
     register JutilTextRec *tx;
     int x, width;
{
  register wchar *JW_buf, *old_JW_buf, *wc;
  unsigned char *JW_att;
  unsigned char old_JW_att;
  int currentcol_tmp = 0;
  int start, end, startcol;
  int wc_len, mb_len;
  int col;
  XCharStruct cs;

  currentcol_tmp = tx->currentcol;
  start = x / FontWidth;
  end = start + width / FontWidth + ((width % FontWidth) ? 1 : 0);
  if (end > tx->max_pos)
    end = tx->max_pos;
  if (check_mb (tx->buf, start) == 2)
    start--;
  if (check_mb (tx->buf, (end - 1)) == 1)
    end++;

  old_JW_buf = JW_buf = tx->buf + start;
  JW_att = tx->att + start;
  old_JW_att = *JW_att;
  while (wc_buf_max < (end - start))
    {
      if (realloc_wc_buf () < 0)
        return;
    }
  wc = wc_buf;
  skip_pending_wchar (wc, JW_buf);
  wc_len = 0;
  col = 0;
  startcol = start;
  cs.width = FontWidth;
  cs.ascent = FontAscent;
  cs.descent = FontDescent;

  for (tx->currentcol = start; tx->currentcol < end;)
    {
      JW_buf = tx->buf + tx->currentcol;
      JW_att = tx->att + tx->currentcol;
      if (*JW_buf == PENDING_WCHAR)
        {
          JW_buf++;
          JW_att++;
        }
      mb_len = XwcGetColumn (*JW_buf);
      if ((*JW_att != old_JW_att) && (wc_len > 0))
        {
          if (old_JW_att & REV_FLAG)
            {
              currentGC = cur_root->reversegc;
            }
          else
            {
              currentGC = cur_root->gc;
            }
          JWOutput (tx->w, cur_fs->fs, currentGC, startcol, col, old_JW_att, 0, 0, &cs, wc, wc_len);
          old_JW_buf = JW_buf;
          old_JW_att = *JW_att;
          wc += wc_len;
          wc_len = 0;
          col = 0;
          startcol = tx->currentcol;
        }
      if (*JW_att & REV_FLAG)
        {
          if (mb_len)
            {
              check_move (tx, 1);
            }
          else
            {
              check_move (tx, 0);
            }
        }
      wc_len++;
      col += mb_len;
      tx->currentcol += mb_len;
    }
  if (wc_len > 0)
    {
      if (old_JW_att & REV_FLAG)
        {
          currentGC = cur_root->reversegc;
        }
      else
        {
          currentGC = cur_root->gc;
        }
      JWOutput (tx->w, cur_fs->fs, currentGC, startcol, col, old_JW_att, 0, 0, &cs, wc, wc_len);
    }
  tx->currentcol = currentcol_tmp;
  if (tx->cursor_flag == 1 && tx->mark_flag == 1)
    {
      tx->mark_flag = 0;
      JWmark_cursor (1);
    }
}

void
redraw_note (tx, which)
     register JutilTextRec *tx;
     int which;
{
  XwcDrawImageString (dpy, tx->wn[which], cur_fs->fs, cur_root->gc, 0, FontAscent, &dol_wchar_t, 1);
  XFlush (dpy);
}

void
check_scroll ()
{
  int mb;

  mb = check_mb (cur_text->buf, cur_text->currentcol);
  if ((cur_text->currentcol < cur_text->vst) || ((cur_text->currentcol - cur_text->vst - mb) >= cur_text->max_columns))
    {
      if (mb == 1)
        check_move (cur_text, 1);
      else
        check_move (cur_text, 0);
    }
}
