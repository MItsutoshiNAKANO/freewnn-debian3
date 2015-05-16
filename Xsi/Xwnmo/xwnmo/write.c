/*
 * $Id: write.c,v 1.9.2.1 1999/02/08 08:08:59 yamasita Exp $
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
#include "xim.h"
#include "sheader.h"
#include "proto.h"
#include "ext.h"

#if !defined(X11R5) && defined(BC_X11R5)
extern XIMCmblk *cur_cblk;
#define IS_XIMR5_CLIENT()       (cur_cblk->ximr5)
#endif /* !defined(X11R5) && defined(BC_X11R5) */

static int
send_returnend ()
{
  ximReturnReply reply;

  reply.type = 0;
  reply.length = 0;
  reply.keysym = 0;
  if (need_byteswap () == True)
    {
      byteswap_ReturnReply (&reply);
    }
  if (_WriteToClient (&reply, sz_ximReturnReply) == -1)
    return (-1);
  return (0);
}

static int
send_ct (ct, ct_len)
     register char *ct;
     register int ct_len;
{
  ximReturnReply reply;

  reply.type = XIM_STRING;
  reply.length = ct_len;
  reply.keysym = 0;
  if (need_byteswap () == True)
    {
      byteswap_ReturnReply (&reply);
    }
  if (_WriteToClient (&reply, sz_ximReturnReply) == -1)
    return (-1);
  if (ct_len > 0)
    {
      if (_WriteToClient (ct, ct_len) == -1)
        return (-1);
    }
  return (0);
}

static int
send_keysym (keysym, str)
     KeySym keysym;
     char str;
{
  ximReturnReply reply;
  char buf[2];
  int tmp_length;

  reply.type = XIM_KEYSYM;
  if (keysym < 0x20)
    {
      keysym |= 0xff00;
      reply.length = 0;
    }
  else
    {
      reply.length = 1;
      buf[0] = str & 0xff;
      buf[1] = '\0';
    }
  reply.keysym = keysym;
  tmp_length = reply.length;
  if (need_byteswap () == True)
    {
      byteswap_ReturnReply (&reply);
    }
  if (_WriteToClient (&reply, sz_ximReturnReply) == -1)
    return (-1);
  if (tmp_length > 0)
    {
      if (_WriteToClient (buf, tmp_length) == -1)
        return (-1);
    }
  return (0);
}

int
return_eventreply (state, mode, num)
     short state;
     short mode;
     short num;
{
  ximEventReply reply;

  reply.state = state;
  reply.detail = mode;
  reply.number = num;
  if (need_byteswap () == True)
    {
      byteswap_EventReply (&reply);
    }
  if (_WriteToClient (&reply, sz_ximEventReply) == -1)
    return (-1);
  return (0);
}

int
send_nofilter ()
{
#ifndef X11R5
#ifdef BC_X11R5
  if (IS_XIMR5_CLIENT ())
    {
      if (return_eventreply (0, XIM_NOFILTER, 0) == -1)
        return (-1);
    }
  else
#endif /* BC_X11R5 */
    return xim_send_nofilter ();
#else
  if (return_eventreply (0, XIM_NOFILTER, 0) == -1)
    return (-1);
#endif /* !X11R5 */
  return (0);
}

static int
send_ch_locale (locale)
     char *locale;
{
  return_eventreply (0, XIM_CH_LOCALE, strlen (locale));
  if (_WriteToClient (locale, strlen (locale)) == -1)
    return (-1);
  return (0);
}

int
send_end ()
{
  if (cur_p && cur_p->ch_lc_flg)
    {
      send_ch_locale (cur_p->cur_xl->cur_lc->lc_name);
      cur_p->ch_lc_flg = 0;
    }
  if (return_eventreply (0, 0, 0) == -1)
    return (-1);
  if (_Send_Flush () == -1)
    return (-1);
  return (0);
}

int
return_cl_it ()
{
#ifdef  XJPLIB
  if (cur_x && cur_x->xjp)
    {
      XJp_return_cl_it ();
      return (0);
    }
#endif /* XJPLIB */
  if (cur_input != 0)
    {
      if (send_nofilter () == -1)
        return (-1);
    }
  return (0);
}

void
xw_write (w_buf, size)
     register w_char *w_buf;
     register int size;
{
  register XIMLangRec *xl;
  w_char w_c;
  w_char *start;
  register int cnt;
  int conv_len, send_len;
  KeySym keysym;

  if (!size)
    {
      return;
    }
#ifdef  XJPLIB
  if (cur_x && cur_x->xjp)
    {
      XJp_xw_write (w_buf, size);
      return;
    }
#endif /* XJPLIB */

  if (henkan_off_flag)
    {
      return_cl_it ();
      return;
    }
#ifdef BC_X11R5
  if (IS_XIMR5_CLIENT ())
#endif /* BC_X11R5 */
#if defined(X11R5) || defined(BC_X11R5)
    return_eventreply (0, XIM_RETURN, 0);
#endif /* defined(X11R5) || defined(BC_X11R5) */

  xl = cur_p->cur_xl;

  if (IsPreeditPosition (cur_x) && cur_p->cur_xl->del_x >= cur_p->cur_xl->max_pos)
    {
      JWMline_clear1 ();
    }

  conv_len = 0;
  start = w_buf;
  for (cnt = 0; cnt < size; cnt++, w_buf++)
    {
      if (!((w_c = *w_buf) & 0xff80))
        {
          if (conv_len)
            {
              while (1)
                {
                  send_len = w_char_to_ct (xl->xlc, start, ct_buf, conv_len, ct_buf_max);
                  if (send_len < -1)
                    {
                      break;
                    }
                  else if (send_len == -1)
                    {
                      if (realloc_ct_buf () < 0)
                        break;
                    }
                  else if (send_len > 0)
                    {
#ifndef X11R5
#ifdef BC_X11R5
                      if (IS_XIMR5_CLIENT ())
                        send_ct (ct_buf, send_len);
                      else
#endif /* BC_X11R5 */
                        xim_send_ct (ct_buf, send_len);
#else
                      send_ct (ct_buf, send_len);
#endif /* !X11R5 */
                      break;
                    }
                }
              conv_len = 0;
            }
          keysym = (int) w_c;
#ifndef X11R5
#ifdef BC_X11R5
          if (IS_XIMR5_CLIENT ())
            send_keysym (keysym, w_c);
          else
#endif /* BC_X11R5 */
            xim_send_keysym (keysym, w_c);
#else
          send_keysym (keysym, w_c);
#endif /* !X11R5 */
        }
      else
        {
          if (conv_len == 0)
            {
              start = w_buf;
            }
          conv_len++;
        }
    }
  if (conv_len)
    {
      while (1)
        {
          send_len = w_char_to_ct (xl->xlc, start, ct_buf, conv_len, ct_buf_max);
          if (send_len < -1)
            {
              break;
            }
          else if (send_len == -1)
            {
              if (realloc_ct_buf () < 0)
                break;
            }
          else if (send_len > 0)
            {
#ifndef X11R5
#ifdef BC_X11R5
              if (IS_XIMR5_CLIENT ())
                send_ct (ct_buf, send_len);
              else
#endif /* BC_X11R5 */
                xim_send_ct (ct_buf, send_len);
#else
              send_ct (ct_buf, send_len);
#endif /* !X11R5 */
              break;
            }
        }
      conv_len = 0;
    }
#ifdef BC_X11R5
  if (IS_XIMR5_CLIENT ())
#endif /* BC_X11R5 */
#if defined(X11R5) || defined(BC_X11R5)
    send_returnend ();
#endif /* defined(X11R5) || defined(BC_X11R5) */
  return;
}
