/*
 * $Id: callback.c,v 1.10.2.1 1999/02/08 08:08:39 yamasita Exp $
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
#ifdef  CALLBACKS
#include <stdio.h>
#include <string.h>
#include "commonhd.h"
#include "sdefine.h"
#include "xim.h"
#include "sheader.h"
#include "proto.h"
#include "ext.h"

#if !defined(X11R5) && defined(BC_X11R5)
extern XIMCmblk *cur_cblk;
#define IS_XIMR5_CLIENT()       (cur_cblk->ximr5)
#endif /* !defined(X11R5) && defined(BC_X11R5) */

static void
send_statusdraw (str, len)
     wchar *str;
     int len;
{
  ximStatusDrawReply reply;
  int send_len;

  while (1)
    {
      send_len = wchar_to_ct (cur_p->cur_xl->xlc, str, ct_buf, len, ct_buf_max);
      if (send_len < -1)
        {
          return;
        }
      else if (send_len == -1)
        {
          if (realloc_ct_buf () < 0)
            return;
        }
      else
        {
          break;
        }
    }
  reply.type = XIM_ST_TEXT;
  reply.encoding_is_wchar = False;
  reply.length = send_len;
  reply.feedback = 0;
  if (need_byteswap () == True)
    {
      byteswap_StatusDrawReply (&reply);
    }
  if (_WriteToClient (&reply, sz_ximStatusDrawReply) == -1)
    return;
  if (send_len > 0)
    {
      if (_WriteToClient ((char *) ct_buf, send_len) == -1)
        return;
    }
  if (_Send_Flush () == -1)
    return;
  return;
}

static void
send_callbackpro (mode)
     int mode;
{
  ximNormalReply reply;

  reply.state = 0;
  reply.detail = mode;
  if (need_byteswap () == True)
    {
      byteswap_NormalReply (&reply);
    }
  if (_WriteToClient (&reply, sz_ximNormalReply) == -1)
    return;
  if (_Send_Flush () == -1)
    return;
  return;

}

void
CBStatusDraw (w_buf, len)
     wchar *w_buf;
     int len;
{
  if (len <= 0)
    return;
  if (cur_x->cb_st_start == 0)
    return;

#ifndef X11R5
#ifdef BC_X11R5
  if (IS_XIMR5_CLIENT ())
    {
      return_eventreply (0, XIM_CALLBACK);
      send_callbackpro (XIM_CB_ST_DRAW);
      send_statusdraw (w_buf, len);
      send_callbackpro (0);
    }
  else
#endif /* BC_X11R5 */
    xim_send_statusdraw (w_buf, len);
#else
  return_eventreply (0, XIM_CALLBACK);
  send_callbackpro (XIM_CB_ST_DRAW);
  send_statusdraw (w_buf, len);
  send_callbackpro (0);
#endif /* !X11R5 */

}

void
CBStatusStart ()
{
  if (cur_x->cb_st_start)
    return;

#ifndef X11R5
#ifdef BC_X11R5
  if (IS_XIMR5_CLIENT ())
    {
      return_eventreply (0, XIM_CALLBACK);
      send_callbackpro (XIM_CB_ST_START);
      send_callbackpro (0);
      if (_Send_Flush () == -1)
        return;
    }
  else
#endif /* BC_X11R5 */
    xim_send_statusstart ();
#else
  return_eventreply (0, XIM_CALLBACK);
  send_callbackpro (XIM_CB_ST_START);
  send_callbackpro (0);
  if (_Send_Flush () == -1)
    return;
#endif /* !X11R5 */

  cur_x->cb_st_start = 1;
}

void
CBStatusDone ()
{
  if (cur_x->cb_st_start == 0)
    return;

#ifndef X11R5
#ifdef BC_X11R5
  if (IS_XIMR5_CLIENT ())
    {
      return_eventreply (0, XIM_CALLBACK);
      send_callbackpro (XIM_CB_ST_DONE);
      send_callbackpro (0);
      if (_Send_Flush () == -1)
        return;
    }
  else
#endif /* BC_X11R5 */
    xim_send_statusdone ();
#else
  return_eventreply (0, XIM_CALLBACK);
  send_callbackpro (XIM_CB_ST_DONE);
  send_callbackpro (0);
  if (_Send_Flush () == -1)
    return;
#endif /* !X11R5 */
  cur_x->cb_st_start = 0;
}

static void
send_preeditdraw (caret, first, chg_len, str, len, flg)
     int caret, first, chg_len;
     wchar *str;
     int len;
     int flg;
{
  ximPreDrawReply reply;
  unsigned short fb = 0;
  int send_len;

  if (str)
    {
      while (1)
        {
          send_len = wchar_to_ct (cur_p->cur_xl->xlc, str, ct_buf, len, ct_buf_max);
          if (send_len < -1)
            {
              return;
            }
          else if (send_len == -1)
            {
              if (realloc_ct_buf () < 0)
                return;
            }
          else
            {
              break;
            }
        }
    }
  else
    {
      send_len = 0;
    }
  if (flg & REV_FLAG)
    fb |= XIMReverse;
  if (flg & UNDER_FLAG)
    fb |= XIMUnderline;
  if (flg & BOLD_FLAG)
    fb |= XIMHighlight;
  reply.caret = caret;
  reply.chg_first = first;
  reply.chg_length = chg_len;
  reply.length = send_len;
  reply.encoding_is_wchar = False;
  reply.feedback = fb;
  if (need_byteswap () == True)
    {
      byteswap_PreDrawReply (&reply);
    }
  if (_WriteToClient (&reply, sz_ximPreDrawReply) == -1)
    return;
  if (send_len > 0)
    {
      if (_WriteToClient ((char *) ct_buf, send_len) == -1)
        return;
    }
  if (_Send_Flush () == -1)
    return;
  return;
}

void
CBPreeditClear (x)
     int x;
{
  XIMLangRec *xl;
  int caret = 0, max_len = 0;
  int left;

  xl = cur_x->cur_xl;

  if (cur_x->cb_pe_start == 0)
    return;
  caret = XwcGetChars (xl->buf, x, &left);
  max_len = XwcGetChars (xl->buf, xl->max_pos, &left);
  if (max_len <= caret)
    return;
#ifndef X11R5
#ifdef BC_X11R5
  if (IS_XIMR5_CLIENT ())
    {
      return_eventreply (0, XIM_CALLBACK);
      send_callbackpro (XIM_CB_PRE_DRAW);
      send_preeditdraw (caret, caret, max_len - caret, (wchar *) NULL, 0, 0);
      send_callbackpro (0);
      if (_Send_Flush () == -1)
        return;
    }
  else
#endif /* BC_X11R5 */
    xim_send_preeditdraw (caret, caret, max_len - caret, (wchar *) NULL, 0, 0);
#else
  return_eventreply (0, XIM_CALLBACK);
  send_callbackpro (XIM_CB_PRE_DRAW);
  send_preeditdraw (caret, caret, max_len - caret, (wchar *) NULL, 0, 0);
  send_callbackpro (0);
  if (_Send_Flush () == -1)
    return;
#endif /* !X11R5 */
  return;
}

void
CBPreeditDraw (wc, wc_len)
     register wchar *wc;
     int wc_len;
{
  register int cnt = wc_len;
  register XIMLangRec *xl;
  register wchar *JW_buf, *p = wc;
  register unsigned char *JW_att, flg = 0;
  int caret, first, chg_len = 0, col = 0;
  int mb_len, left, save_len = 0;
  wchar tmp[80];

  if (cnt <= 0)
    return;
  xl = cur_x->cur_xl;

  visual_window ();
  if (xl->currentcol >= cur_x->maxcolumns)
    return;
  JW_buf = xl->buf + xl->currentcol;
  JW_att = xl->att + xl->currentcol;

  first = XwcGetChars (xl->buf, xl->currentcol, &left);

  if (xl->r_flag)
    flg |= REV_FLAG;
  if (xl->b_flag)
    flg |= BOLD_FLAG;
  if (xl->u_line_flag)
    flg |= UNDER_FLAG;

  if ((save_len = xl->max_pos - xl->currentcol) > 0)
    {
      bcopy (JW_buf, tmp, (save_len * sizeof (wchar)));
      tmp[save_len] = 0;
    }
  for (; cnt > 0; cnt--)
    {
      mb_len = put_pending_wchar_and_flg (*p, JW_buf, JW_att, flg);
      col += mb_len;
      JW_buf += mb_len;
      JW_att += mb_len;
    }
  xl->currentcol += col;
  if (save_len > 0)
    {
      chg_len = XwcGetChars (tmp, col, &left);
      if (left > 0)
        {
          CBPreeditClear (xl->currentcol + col);
        }
    }
  caret = XwcGetChars (xl->buf, xl->currentcol, &left);

#ifndef X11R5
#ifdef BC_X11R5
  if (IS_XIMR5_CLIENT ())
    {
      return_eventreply (0, XIM_CALLBACK);
      send_callbackpro (XIM_CB_PRE_DRAW);
      send_preeditdraw (caret, first, chg_len, wc, wc_len, flg);
      send_callbackpro (0);
      if (_Send_Flush () == -1)
        return;
    }
  else
#endif /* BC_X11R5 */
    xim_send_preeditdraw (caret, first, chg_len, wc, wc_len, flg);
#else
  return_eventreply (0, XIM_CALLBACK);
  send_callbackpro (XIM_CB_PRE_DRAW);
  send_preeditdraw (caret, first, chg_len, wc, wc_len, flg);
  send_callbackpro (0);
  if (_Send_Flush () == -1)
    return;
#endif /* !X11R5 */
}

void
CBPreeditStart ()
{
  if (cur_x->cb_pe_start)
    return;
  cur_x->cur_xl->visible = 1;

#ifndef X11R5
#ifdef BC_X11R5
  if (IS_XIMR5_CLIENT ())
    {
      return_eventreply (0, XIM_CALLBACK);
      send_callbackpro (XIM_CB_PRE_START);
      send_callbackpro (0);
      if (_Send_Flush () == -1)
        return;
    }
  else
#endif /* BC_X11R5 */
    xim_send_preeditstart ();
#else
  return_eventreply (0, XIM_CALLBACK);
  send_callbackpro (XIM_CB_PRE_START);
  send_callbackpro (0);
  if (_Send_Flush () == -1)
    return;
#endif /* !X11R5 */
  cur_x->cb_pe_start = 1;
}

void
CBPreeditDone ()
{
  if (cur_x->cb_pe_start == 0)
    return;
  cur_x->cur_xl->visible = 0;
#ifndef X11R5
#ifdef BC_X11R5
  if (IS_XIMR5_CLIENT ())
    {
      return_eventreply (0, XIM_CALLBACK);
      send_callbackpro (XIM_CB_PRE_DONE);
      send_callbackpro (0);
      if (_Send_Flush () == -1)
        return;
    }
  else
#endif /* BC_X11R5 */
    xim_send_preeditdone ();
#else
  return_eventreply (0, XIM_CALLBACK);
  send_callbackpro (XIM_CB_PRE_DONE);
  send_callbackpro (0);
  if (_Send_Flush () == -1)
    return;
#endif /* !X11R5 */
  cur_x->cb_pe_start = 0;
}

void
CBPreeditRedraw (xl)
     register XIMLangRec *xl;
{
  register wchar *JW_buf, *wc;
  register unsigned char *JW_att, old_JW_att;
  register int currentcol = 0;
  int caret = 0, first = 0;
  int wc_len = 0, mb_len = 0;
  int left;

  if (xl->currentcol >= cur_x->maxcolumns || xl->max_pos <= 0)
    return;
  visual_window ();
  CBPreeditClear (0);

  return_eventreply (0, XIM_CALLBACK);
  send_callbackpro (XIM_CB_PRE_DRAW);

  xl = cur_p->cur_xl;

  JW_buf = xl->buf;
  JW_att = xl->att;
  old_JW_att = *JW_att;
  wc = wc_buf;
  skip_pending_wchar (wc, JW_buf);
  wc_len = 0;
  caret = 0;
  first = 0;
  currentcol = 0;
  for (currentcol = 0; currentcol < xl->max_pos;)
    {
      JW_buf = xl->buf + currentcol;
      JW_att = xl->att + currentcol;
      if ((mb_len = get_columns_wchar (JW_buf)) > 0)
        {
          JW_buf += (mb_len - 1);
          JW_att += (mb_len - 1);
        }
      if ((*JW_att != old_JW_att) && (wc_len > 0))
        {
          caret = XwcGetChars (xl->buf, currentcol, &left);
          send_preeditdraw (caret, first, 0, wc, wc_len, old_JW_att);
          old_JW_att = *JW_att;
          wc += wc_len;
          wc_len = 0;
          first = caret;
        }
      wc_len++;
      currentcol += mb_len;
    }

  caret = XwcGetChars (xl->buf, currentcol, &left);
#ifndef X11R5
#ifdef BC_X11R5
  if (IS_XIMR5_CLIENT ())
    {
      send_preeditdraw (caret, first, 0, wc, wc_len, old_JW_att);
      send_callbackpro (0);
      if (_Send_Flush () == -1)
        return;
    }
  else
#endif /* BC_X11R5 */
    xim_send_preeditdraw (caret, first, 0, wc, wc_len, old_JW_att);
#else
  send_preeditdraw (caret, first, 0, wc, wc_len, old_JW_att);
  send_callbackpro (0);
  if (_Send_Flush () == -1)
    return;
#endif /* !X11R5 */
  return;
}

static void
send_preeditcaret (flg, pos)
     int flg;
     int pos;
{
  ximPreCaretReply reply;

  if (flg)
    {
      reply.style = XIMIsPrimary;
    }
  else
    {
      reply.style = XIMIsInvisible;
    }
  reply.position = pos;
  reply.direction = XIM_CB_DONT_CHANGE;
  if (need_byteswap () == True)
    {
      byteswap_PreCaretReply (&reply);
    }
  if (_WriteToClient (&reply, sz_ximPreCaretReply) == -1)
    return;
  if (_Send_Flush () == -1)
    return;
  return;
}

void
CBCursorMove (x)
     int x;
{
  XIMLangRec *xl;
  int pos = 0;
  int left;

  xl = cur_x->cur_xl;

  if (cur_x->cb_pe_start == 0)
    return;
#ifndef X11R5
  pos = XwcGetChars (xl->buf, x, &left);
#ifdef BC_X11R5
  if (IS_XIMR5_CLIENT ())
    {
      return_eventreply (0, XIM_CALLBACK);
      send_callbackpro (XIM_CB_PRE_CARET);
      send_preeditcaret (xl->mark_flag, pos);
      send_callbackpro (0);
      if (_Send_Flush () == -1)
        return;
    }
  else
#endif /* BC_X11R5 */
    xim_send_preeditcaret (xl->mark_flag, pos);
#else
  return_eventreply (0, XIM_CALLBACK);
  send_callbackpro (XIM_CB_PRE_CARET);
  pos = XwcGetChars (xl->buf, x, &left);
  send_preeditcaret (xl->mark_flag, pos);
  send_callbackpro (0);
  if (_Send_Flush () == -1)
    return;
  return;
#endif /* !X11R5 */
}

void
CBCursorMark (flg)
     int flg;
{
  XIMLangRec *xl;
  int pos = 0;
  int left;

  xl = cur_x->cur_xl;

  if (cur_x->cb_pe_start == 0)
    return;
  if (xl->cursor_flag == 1 && xl->mark_flag != flg)
    {
      xl->mark_flag = flg;
#ifndef X11R5
      pos = XwcGetChars (xl->buf, xl->currentcol, &left);
#ifdef BC_X11R5
      if (IS_XIMR5_CLIENT ())
        {
          return_eventreply (0, XIM_CALLBACK);
          send_callbackpro (XIM_CB_PRE_CARET);
          send_preeditcaret (flg, pos);
          send_callbackpro (0);
          if (_Send_Flush () == -1)
            return;
        }
      else
#endif /* BC_X11R5 */
        xim_send_preeditcaret (flg, pos);
#else
      return_eventreply (0, XIM_CALLBACK);
      send_callbackpro (XIM_CB_PRE_CARET);
      pos = XwcGetChars (xl->buf, xl->currentcol, &left);
      send_preeditcaret (flg, pos);
      send_callbackpro (0);
      if (_Send_Flush () == -1)
        return;
      return;
#endif /* !X11R5 */
    }
}

void
SendCBRedraw ()
{
  XEvent ev;

  ev.type = KeyPress;
  ev.xkey.window = cur_x->focus_window;
  ev.xkey.root = cur_x->root_pointer->root_window;
  ev.xkey.state = 0;
  ev.xkey.keycode = cur_x->max_keycode + 2;
  XSendEvent (dpy, ev.xkey.window, False, NoEventMask, &ev);
  XFlush (dpy);
}
#endif /* CALLBACKS */
