/*
 * $Id: keyin.c,v 1.2 2001/06/14 18:16:16 ura Exp $
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
#include "sdefine.h"
#include "xim.h"
#include "sheader.h"
#include "proto.h"
#include "ext.h"

static int
read_xevent (ev)
     XEvent *ev;
{
  ximEventReq req;

  if (EACH_REQUEST (&req, sz_ximEventReq) == -1)
    return (-1);
  if (_ReadFromClient (ev, sizeof (XEvent)) == -1)
    return (-1);
  if (need_byteswap () == True)
    {
      byteswap_xevent (ev);
    }
  xim->sel_button = '\0';
  return (0);
}

static int
check_cur_input (w)
     Window w;
{
  if (cur_input == NULL)
    {
#ifdef  XJPLIB
      if (XJp_check_cur_input (w) == 0)
        return (0);
#endif /* XJPLIB */
    }
  else
    {
      if (w == cur_input->w)
        return (0);
    }
  if (cur_p && w == cur_p->yes_no->w)
    return (0);
  return (1);
}

static int
cur_cl_set (set_window)
     Window set_window;
{
  register XIMClientRec *xc = NULL;
  register XIMInputRec *xi;

  for (xi = input_list; xi != NULL; xi = xi->next)
    {
      if (xi->w == set_window)
        {
          xc = xi->pclient;
          break;
        }
    }
#ifdef  XJPLIB
  if (xc)
    {
      XJp_cur_cl_set (0);
    }
  else
    {
      xc = XJp_cur_cl_set (set_window);
    }
#endif /* XJPLIB */
  if (xc)
    {
      cur_x = xc;
      if (IsPreeditNothing (cur_x))
        {
          cur_p = cur_x->root_pointer->ximclient;
          cur_lang = cur_p->cur_xl->lang_db;
          c_c = cur_p->cur_xl->w_c;
        }
      else
        {
          cur_p = cur_x;
          cur_lang = cur_p->cur_xl->lang_db;
          c_c = cur_x->cur_xl->w_c;
        }
      cur_rk = c_c->rk;
      cur_rk_table = cur_rk->rk_table;
      cur_input = xi;
      set_cswidth (cur_lang->cswidth_id);
      return (0);
    }
  for (xc = ximclient_list; xc; xc = xc->next)
    {
      if (!IsPreeditNothing (xc) && set_window == xc->yes_no->w)
        {
          cur_p = xc;
          cur_lang = cur_p->cur_xl->lang_db;
          c_c = cur_p->cur_xl->w_c;
          cur_rk = c_c->rk;
          cur_rk_table = cur_rk->rk_table;
          set_cswidth (cur_lang->cswidth_id);
          return (0);
        }
    }
  if (xim->j_c && (set_window == xim->cur_j_c_root->ichi->w || set_window == xim->cur_j_c_root->inspect->w))
    {
      cur_p = xim->j_c;
      cur_lang = cur_p->cur_xl->lang_db;
      c_c = cur_p->cur_xl->w_c;
      cur_rk = c_c->rk;
      cur_rk_table = cur_rk->rk_table;
      set_cswidth (cur_lang->cswidth_id);
      return (0);
    }
  return (-1);
}

static XComposeStatus compose_status = { NULL, 0 };

int
key_input (buff, ev)
     register int *buff;
     register XEvent *ev;
{
  unsigned char strbuf[512];
  KeySym keysym;
  int nbytes;
  int ck;
  register int i;

  if (check_cur_input (ev->xkey.window))
    {
      if (cur_cl_set (ev->xkey.window) != 0)
        {
          return (-1);
        }
    }
  if (dpy != ev->xkey.display)
    ev->xkey.display = dpy;
#ifdef  CALLBACKS
  if (IsPreeditCallbacks (cur_x) && cur_x->cb_redraw_needed)
    {
      *buff++ = 0;
      return (1);
    }
#endif /* CALLBACKS */
#ifdef  USING_XJUTIL
  if (cur_p->cur_xl->working_xjutil)
    {
      xjutil_send_key_event (ev);
      return (-1);
    }
#endif /* USING_XJUTIL */
  nbytes = XLookupString ((XKeyEvent *) ev, strbuf, 512, &keysym, &compose_status);
#if defined(XJPLIB) && defined(XJPLIB_DIRECT)
  XJp_save_sockbuf (nbytes, strbuf, keysym);
#endif /* defined(XJPLIB) && defined(XJPLIB_DIRECT) */
  if ((ck = cvt_key_fun (keysym, ev->xkey.state)) >= 0)
    {
#ifdef  XJPLIB
      XJp_check_save_event (ev, 0);
#endif /* XJPLIB */
      *buff++ = ck;
      return (1);
    }
  if ((keysym & 0xff00) == 0xff00)
    {                           /* Function Key */
      if (nbytes > 0)
        {
#ifdef  XJPLIB
          XJp_check_save_event (ev, 1);
#endif /* XJPLIB */
          for (i = 0; i < nbytes; i++)
            {
              *buff++ = (int) strbuf[i];
            }
          return (nbytes);
        }
      else
        {
#ifdef  XJPLIB
          XJp_check_send_cl_key (ev);
#endif /* XJPLIB */
          return (0);
        }
    }
  if (ev->xkey.state & ControlMask)
    {                           /* Control Key */
      if (*strbuf >= 0x20)
        {
#ifdef  XJPLIB
          XJp_check_send_cl_key (ev);
#endif /* XJPLIB */
          return (-1);
        }
      else if (*strbuf == 0x0)
        {
#ifdef  XJPLIB
          XJp_check_save_event (ev, 0);
#endif /* XJPLIB */
          *buff++ = (int) *strbuf;
          return (1);
        }
      else if (*strbuf <= 0x1f)
        {
#ifdef  XJPLIB
          XJp_check_save_event (ev, 0);
#endif /* XJPLIB */
          *buff++ = (int) *strbuf;
          return (1);
        }
    }
  if ((keysym >= 0x4a0) && (keysym <= 0x4df))
    {                           /* Kana Key */
#ifdef  XJPLIB
      XJp_check_save_event (ev, 0);
#endif /* XJPLIB */
      *buff++ = (int) ((SS2 << 8) | (keysym & 0xff));
      return (1);
    }
  else if ((keysym < 0x100) && (keysym > 0))
    {
#ifdef  XJPLIB
      XJp_check_save_event (ev, 1);
#endif /* XJPLIB */
      if (nbytes <= 0)
        {
          nbytes = -1;
        }
      for (i = 0; i < nbytes; i++)
        {
          *buff++ = (int) strbuf[i];
        }
      return (nbytes);
    }
  return (-1);
}

int
ifempty ()
{
  if (cur_p->cur_xl->max_pos == 0)
    {
      return (1);
    }
  else
    {
      return (0);
    }
}

int
RequestDispatch ()
{
  int buff[32], in;
  XEvent event;
  int ret = 0;
  register int i, n_bytes;

  switch (read_requestheader ())
    {
    case XIM_Event:
      if (read_xevent (&event) == -1)
        {
          break;
        }
      ret = key_input (buff, &event);
      if (ret == 0)
        {
          send_nofilter ();
        }
      else if (ret > 0)
        {
          for (i = 0; i < ret;)
            {
              n_bytes = get_cswidth_by_char (in = buff[i++]);
              for (; n_bytes > 1 && i < ret; n_bytes--, i++)
                {
                  in = (in << 8) + buff[i];
                }
              in_put (in);
            }
        }
      send_end ();
      break;
    case XIM_GetIM:
      GetIM ();
      break;
    case XIM_CreateIC:
      CreateIC ();
      break;
    case XIM_ChangeIC:
      ChangeIC ();
      break;
    case XIM_GetIC:
      GetIC ();
      break;
    case XIM_DestroyIC:
      DestroyIC ();
      break;
    case XIM_SetICFocus:
      SetICFocus ();
      break;
    case XIM_UnsetICFocus:
      UnsetICFocus ();
      break;
    case XIM_ResetIC:
      ResetIC ();
      break;
#ifdef  SPOT
    case XIM_ChangeSpot:
      ChangeSpot ();
      break;
#endif /* SPOT */
    }
  return (0);
}
