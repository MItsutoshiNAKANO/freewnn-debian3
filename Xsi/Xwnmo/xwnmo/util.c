/*
 * $Id: util.c,v 1.25.2.1 1999/02/08 08:08:58 yamasita Exp $
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

static int c_data_len = 0, use_langs_len = 0, cur_l_len = 0;
static int pre_font_len = 0, st_font_len = 0;
static char *c_data = NULL, *use_langs = NULL, *cur_l = NULL;
static char *pre_font = NULL, *st_font = NULL;

void
GetIM ()
{
  ximGetIMReq req;
  ximGetIMReply reply;
  unsigned long mask;
  int i;
  XIMStyle tmp;
  int tmp_num_styles, tmp_nbytes;

  if (EACH_REQUEST (&req, sz_ximGetIMReq) == -1)
    return;
  if (need_byteswap () == True)
    {
      mask = byteswap_l (req.mask);
    }
  else
    {
      mask = req.mask;
    }
  reply.state = 0;
  if (mask & (1 << IMQueryInputStyle))
    {
      reply.num_styles = MAX_SUPPORT_STYLE;
    }
  else
    {
      reply.num_styles = 0;
    }
  if (mask & (1 << IMQueryLanguage))
    {
      reply.nbytes = strlen (xim->supported_language);
    }
  else
    {
      reply.nbytes = 0;
    }
  tmp_num_styles = reply.num_styles;
  tmp_nbytes = reply.nbytes;
  if (need_byteswap () == True)
    {
      byteswap_GetIMReply (&reply);
    }
  if (_WriteToClient (&reply, sz_ximGetIMReply) == -1)
    return;
  if (tmp_num_styles > 0)
    {
      for (i = 0; i < MAX_SUPPORT_STYLE; i++)
        {
          if (need_byteswap () == True)
            {
              tmp = byteswap_l (xim->supported_style[i]);
            }
          else
            {
              tmp = xim->supported_style[i];
            }
          if (_WriteToClient (&tmp, sizeof (XIMStyle)) == -1)
            {
              return;
            }
        }
    }
  if (tmp_nbytes > 0)
    {
      if (_WriteToClient (xim->supported_language, tmp_nbytes) == -1)
        {
          return;
        }
    }
  if (_Send_Flush () == -1)
    return;
  return;
}

/* *INDENT-OFF* */
Status
have_world (lc_list)
    XIMNestLangList lc_list;
/* *INDENT-ON* */

{
  XIMNestLangList p = lc_list;

  for (; p != NULL; p = p->next)
    {
      if (!strcmp (p->lc_name, world_locale))
        return (True);
    }
  return (False);
}

void
CreateIC ()
{
  ximICValuesReq ic_req;
  ximICAttributesReq pre_req, st_req;
  ximCreateICReply reply;
#ifdef  CALLBACKS
  ximEventReply reply1;
#endif /* CALLBACKS */
  int c_len;
  XIMNestLangList p;
  XIMNestLangList lc_list = NULL, cur_lc = NULL;
  XIMClientRec *xc = NULL;
  int error = 0;
  int i, ret;
  short detail;
  int have_world = 0, cur_is_world = 0;

  c_len = get_rest_len ();
  if ((ret = read_strings (&c_data, &c_data_len, c_len)) < 0)
    {
      if (ret == -1)
        return;
      error++;
    }

  if (_ReadFromClient (&ic_req, sz_ximICValuesReq) == -1)
    return;
  if (need_byteswap () == True)
    {
      byteswap_ICValuesReq (&ic_req);
    }
  if ((ret = read_strings (&use_langs, &use_langs_len, ic_req.nbytes)) < 0)
    {
      if (ret == -1)
        return;
      error++;
    }
  if ((ret = read_strings (&cur_l, &cur_l_len, ic_req.nbytes2)) < 0)
    {
      if (ret == -1)
        return;
      error++;
    }

  if (_ReadFromClient (&pre_req, sz_ximICAttributesReq) == -1)
    return;
  if (need_byteswap () == True)
    {
      byteswap_ICAttributesReq (&pre_req);
    }
  if ((ret = read_strings (&pre_font, &pre_font_len, pre_req.nbytes)) < 0)
    {
      if (ret == -1)
        return;
      error++;
    }

  if (_ReadFromClient (&st_req, sz_ximICAttributesReq) == -1)
    return;
  if (need_byteswap () == True)
    {
      byteswap_ICAttributesReq (&st_req);
    }
  if ((ret = read_strings (&st_font, &st_font_len, st_req.nbytes)) < 0)
    {
      if (ret == -1)
        return;
      error++;
    }
  if (error > 0)
    {
      detail = AllocError;
      goto ERROR_RET;
    }

  /*
   * Check resources.
   */
  error = -1;
  if ((ic_req.mask & (1 << ICInputStyle)))
    {
      for (i = 0; i < MAX_SUPPORT_STYLE; i++)
        {
          if (xim->supported_style[i] == ic_req.input_style)
            {
              error = 0;
              break;
            }
        }
    }
  if (error != 0)
    {
      detail = BadStyle;
    }
  else
    {
      if (ic_req.input_style & (XIMPreeditArea | XIMPreeditPosition))
        {
          if (!(ic_req.mask & (1 << ICFontSet)) || !pre_font || !*pre_font)
            {
              error = -1;
              detail = BadFontSet;
            }
        }
      if (ic_req.input_style & XIMStatusArea)
        {
          if (!(ic_req.mask & (1 << (ICFontSet + StatusOffset))) || !st_font || !*st_font)
            {
              error = -1;
              detail = BadFontSet;
            }
        }
      if (ic_req.input_style & XIMPreeditPosition)
        {
          if (!(ic_req.mask & (1 << ICSpotLocation)))
            {
              error = -1;
              detail = BadSpotLocation;
            }
        }
    }
  if (error == -1)
    goto ERROR_RET;

  if (!(ic_req.input_style & XIMPreeditNothing))
    {
      if ((have_world = get_langlist (use_langs, &lc_list)) < 0)
        {
          error = -1;
          detail = AllocError;
          goto ERROR_RET;
        }
      if (!lc_list)
        {
          error = -1;
          detail = BadLanguage;
          goto ERROR_RET;
        }
      if (cur_l && *cur_l)
        {
          for (p = lc_list; p != NULL; p = p->next)
            {
              if (!strcmp (p->lc_name, cur_l) || (p->alias_name && !strcmp (p->alias_name, cur_l)))
                break;
            }
          if (p == NULL)
            {
              if ((i = get_langlist (cur_l, &cur_lc)) < 0)
                {
                  error = -1;
                  detail = AllocError;
                  goto ERROR_RET;
                }
              else if (i > 0)
                {
                  cur_is_world++;
                  free_langlist (cur_lc);
                  cur_lc = lc_list;
                }
              else
                {
                  if (cur_lc)
                    {
                      cur_lc->next = lc_list;
                    }
                  else
                    {
                      cur_lc = lc_list;
                    }
                }
            }
          else
            {
              cur_lc = p;
            }
        }
      else
        {
          if (have_world == 2)
            cur_is_world++;
          cur_lc = lc_list;
        }
    }
  xc = create_client (&ic_req, &pre_req, &st_req, lc_list, cur_lc, have_world, cur_is_world, pre_font, st_font, c_data, &detail);
  free_langlist (lc_list);

ERROR_RET:
  if (error != 0 || xc == NULL)
    {
      reply.state = -1;
      reply.detail = detail;
      reply.xic = 0;
    }
  else
    {
      reply.state = 0;
      reply.xic = xim->client_count++;
      xc->number = reply.xic;
    }
#ifdef  CALLBACKS
  reply1.state = 0;
  reply1.detail = XIM_IC;
  if (need_byteswap () == True)
    {
      byteswap_EventReply (&reply1);
      byteswap_CreateICReply (&reply);
    }
  if ((_WriteToClient (&reply1, sz_ximEventReply) == -1) || (_WriteToClient (&reply, sz_ximCreateICReply) == -1) ||
#else /* CALLBACKS */
  if (need_byteswap () == True)
    {
      byteswap_CreateICReply (&reply);
    }
  if ((_WriteToClient (&reply, sz_ximCreateICReply) == -1) ||
#endif /* CALLBACKS */
      (_Send_Flush () == -1))
    {
      return;
    }
  if (xc && IsPreeditNothing (xc))
    {
      check_root_mapping (xc->root_pointer);
    }
  return;
}

void
GetIC ()
{
  ximGetICReq req;
  ximGetICReply reply;
  ximICValuesReq reply1;
  ximICAttributesReq reply2;
  register XIMClientRec *xc;
  register XIMRootRec *root;
  unsigned long mask = 0L;
  XIMAttributes *pe, *st;
  int tmp_nbytes, tmp_nbytes2;

  if (EACH_REQUEST (&req, sz_ximGetICReq) == -1)
    return;
  if (need_byteswap () == True)
    {
      req.mask = byteswap_l (req.mask);
      req.xic = byteswap_l (req.xic);
    }
  mask = req.mask;

  for (xc = ximclient_list; xc != NULL; xc = xc->next)
    {
      if (xc->number == req.xic)
        {
          break;
        }
    }
  if (xc == NULL)
    {
      reply.state = -1;
      reply.detail = BadIC;
      if (need_byteswap () == True)
        {
          byteswap_GetICReply (&reply);
        }
      if (_WriteToClient (&reply, sz_ximGetICReply) == -1)
        return;
      return;
    }
  else
    {
      reply.state = 0;
    }
  if (need_byteswap () == True)
    {
      byteswap_GetICReply (&reply);
    }
  if (_WriteToClient (&reply, sz_ximGetICReply) == -1)
    return;
#ifdef  CALLBACKS
/*
    if (IsPreeditNothing(xc)) {
        xc = xc->root_pointer->ximclient;
    }
*/
#endif /* CALLBACKS */

  root = xc->root_pointer;
  reply1.mask = mask;

  if (mask & (1 << ICInputStyle))
    {
      reply1.input_style = xc->input_style;
    }
  if (mask & (1 << ICClientWindow))
    {
      reply1.c_window = xc->w;
    }
  if (mask & (1 << ICFocusWindow))
    {
      reply1.focus_window = xc->focus_window;
    }
  if (mask & (1 << ICFilterEvents))
    {
      reply1.filter_events = xc->filter_events;
    }
  if (mask & (1 << ICUsingLanguage))
    {
      if (IsPreeditNothing (xc))
        {
          reply1.nbytes = strlen (root->ximclient->using_language);
        }
      else if (xc->using_language)
        {
          reply1.nbytes = strlen (xc->using_language);
        }
      else
        {
          reply1.nbytes = 0;
        }
    }
  else
    {
      reply1.nbytes = 0;
    }
  if (mask & (1 << ICCurrentLanguage))
    {
      if (IsPreeditNothing (xc))
        {
          if (root->ximclient->world_on)
            {
              reply1.nbytes2 = strlen (world_locale);
            }
          else
            {
              reply1.nbytes2 = strlen (root->ximclient->cur_xl->cur_lc->lc_name);
            }
        }
      else if (xc->world_on)
        {
          reply1.nbytes2 = strlen (world_locale);
        }
      else if (xc->cur_xl->cur_lc->lc_name)
        {
          reply1.nbytes2 = strlen (xc->cur_xl->cur_lc->lc_name);
        }
      else
        {
          reply1.nbytes2 = 0;
        }
/*
    if ((mask & (1 << ICCurrentLanguage)) && !IsPreeditNothing(xc)
        && xc->cur_xl->cur_lc->lc_name) {
        reply1.nbytes2 = strlen(xc->cur_xl->cur_lc->lc_name);
*/
    }
  else
    {
      reply1.nbytes2 = 0;
    }
  tmp_nbytes = reply1.nbytes;
  tmp_nbytes2 = reply1.nbytes2;
  if (need_byteswap () == True)
    {
      byteswap_ICValuesReq (&reply1);
    }
  if (_WriteToClient (&reply1, sz_ximICValuesReq) == -1)
    return;
  if (tmp_nbytes > 0)
    {
      if (IsPreeditNothing (xc))
        {
          if (_WriteToClient (root->ximclient->using_language, tmp_nbytes) == -1)
            return;
        }
      else
        {
          if (_WriteToClient (xc->using_language, tmp_nbytes) == -1)
            return;
        }
    }
  if (tmp_nbytes2 > 0)
    {
      if (IsPreeditNothing (xc))
        {
          if (root->ximclient->world_on)
            {
              if (_WriteToClient (world_locale, tmp_nbytes2) == -1)
                return;
            }
          else
            {
              if (_WriteToClient (root->ximclient->cur_xl->cur_lc->lc_name, tmp_nbytes2) == -1)
                return;
            }
        }
      else
        {
          if (xc->world_on)
            {
              if (_WriteToClient (world_locale, tmp_nbytes2) == -1)
                return;
            }
          else
            {
              if (_WriteToClient (xc->cur_xl->cur_lc->lc_name, tmp_nbytes2) == -1)
                return;
            }
        }
    }

  pe = &(xc->pe);
  if (mask & (1 << ICArea))
    {
      reply2.area_x = pe->area.x;
      reply2.area_y = pe->area.y;
      reply2.area_width = pe->area.width;
      reply2.area_height = pe->area.height;
    }
  if (mask & (1 << ICAreaNeeded))
    {
      reply2.areaneeded_width = pe->area_needed.width;
      reply2.areaneeded_height = pe->area_needed.height;
    }
  if (IsPreeditPosition (xc))
    {
      if (mask & (1 << ICSpotLocation))
        {
          reply2.spot_x = xc->point.x;
          reply2.spot_y = xc->point.y + FontAscent (xc->cur_xl);
        }
    }
  if (mask & (1 << ICColormap))
    {
      reply2.colormap = pe->colormap;
    }
  if (mask & (1 << ICStdColormap))
    {
      reply2.std_colormap = pe->std_colormap;
    }
  if (mask & (1 << ICForeground))
    {
      reply2.foreground = pe->fg;
    }
  if (mask & (1 << ICBackground))
    {
      reply2.background = pe->bg;
    }
  if (mask & (1 << ICBackgroundPixmap))
    {
      reply2.pixmap = pe->bg_pixmap;
    }
  if (mask & (1 << ICLineSpace))
    {
      reply2.line_space = pe->line_space;
    }
  if (mask & (1 << ICCursor))
    {
      reply2.cursor = pe->cursor;
    }
  reply2.nfonts = 0;
  reply2.nbytes = 0;
  if (need_byteswap () == True)
    {
      byteswap_ICAttributesReq (&reply2);
    }
  if (_WriteToClient (&reply2, sz_ximICAttributesReq) == -1)
    return;

  st = &(xc->st);
  if (mask & (1 << (ICArea + StatusOffset)))
    {
      reply2.area_x = st->area.x;
      reply2.area_y = st->area.y;
      reply2.area_width = st->area.width;
      reply2.area_height = st->area.height;
    }
  if (mask & (1 << (ICAreaNeeded + StatusOffset)))
    {
      reply2.areaneeded_width = st->area_needed.width;
      reply2.areaneeded_height = st->area_needed.height;
    }
  if (mask & (1 << (ICColormap + StatusOffset)))
    {
      reply2.colormap = st->colormap;
    }
  if (mask & (1 << (ICStdColormap + StatusOffset)))
    {
      reply2.std_colormap = st->std_colormap;
    }
  if (mask & (1 << (ICForeground + StatusOffset)))
    {
      reply2.foreground = st->fg;
    }
  if (mask & (1 << (ICBackground + StatusOffset)))
    {
      reply2.background = st->bg;
    }
  if (mask & (1 << (ICBackgroundPixmap + StatusOffset)))
    {
      reply2.pixmap = st->bg_pixmap;
    }
  if (mask & (1 << (ICLineSpace + StatusOffset)))
    {
      reply2.line_space = st->line_space;
    }
  if (mask & (1 << (ICCursor + StatusOffset)))
    {
      reply2.cursor = st->cursor;
    }
  reply2.nfonts = 0;
  reply2.nbytes = 0;
  if (need_byteswap () == True)
    {
      byteswap_ICAttributesReq (&reply2);
    }
  if (_WriteToClient (&reply2, sz_ximICAttributesReq) == -1)
    return;
  if (_Send_Flush () == -1)
    return;
}

void
SetICFocus ()
{
  ximICFocusReq req;
  ximEventReply reply;
  register XIMClientRec *xc;
  register XIMInputRec *xi;
  XIMClientRec *save_cur_p, *save_cur_x;
  WnnClientRec *save_c_c;

  if (EACH_REQUEST (&req, sz_ximICFocusReq) == -1)
    return;
  if (need_byteswap () == True)
    {
      req.xic = byteswap_l (req.xic);
    }

  for (xc = ximclient_list; xc != NULL; xc = xc->next)
    {
      if (xc->number == req.xic)
        break;
    }
  if (xc == NULL)
    {
      reply.state = -1;
      reply.detail = BadIC;
    }
  else
    {
      for (xi = input_list; xi != NULL; xi = xi->next)
        {
          if (xi->w == xc->focus_window)
            break;
        }
      if (xi == NULL)
        {
          if (!(xi = (XIMInputRec *) Malloc (sizeof (XIMInputRec))))
            {
              malloc_error ("allocation of Focus data struct");
              reply.state = -1;
              reply.detail = AllocError;
              goto ERROR_RET;
            }
          xi->w = xc->focus_window;
          xi->pclient = xc;
          xi->next = NULL;
          add_inputlist (xi);
        }
      save_cur_p = cur_p;
      save_cur_x = cur_x;
      save_c_c = c_c;
      cur_x = xc;
      if (IsPreeditNothing (xc))
        {
          cur_p = xc->root_pointer->ximclient;
        }
      else
        {
          cur_p = cur_x;
        }
      c_c = cur_p->cur_xl->w_c;
      cur_rk = c_c->rk;
      cur_rk_table = cur_rk->rk_table;
#ifdef  CALLBACKS
      if (IsPreeditPosition (xc) || IsPreeditArea (xc) || IsStatusCallbacks (xc))
        {
#else /* CALLBACKS */
      if (IsPreeditPosition (xc) || IsPreeditArea (xc))
        {
#endif /* CALLBACKS */
          reset_preedit (xc);
        }
#ifdef  CALLBACKS
      if (IsStatusArea (xc) || IsStatusCallbacks (xc))
        {
#else /* CALLBACKS */
      if (IsStatusArea (xc))
        {
#endif /* CALLBACKS */
          visual_status ();
        }
      cur_p = save_cur_p;
      cur_x = save_cur_x;
      c_c = save_c_c;
      if (c_c)
        {
          cur_rk = c_c->rk;
          cur_rk_table = cur_rk->rk_table;
        }
      reply.state = 0;
      reply.detail = 0;
    }
ERROR_RET:
  if (need_byteswap () == True)
    {
      byteswap_EventReply (&reply);
    }
  if ((_WriteToClient (&reply, sz_ximEventReply) == -1) || (_Send_Flush () == -1))
    return;
  return;
}

void
UnsetICFocus ()
{
  ximICFocusReq req;
  ximEventReply reply;
  register XIMClientRec *xc;
  register XIMInputRec *xi;
  XIMClientRec *save_cur_p, *save_cur_x;
  WnnClientRec *save_c_c;

  if (EACH_REQUEST (&req, sz_ximICFocusReq) == -1)
    return;
  if (need_byteswap () == True)
    {
      req.xic = byteswap_l (req.xic);
    }

  for (xc = ximclient_list; xc != NULL; xc = xc->next)
    {
      if (xc->number == req.xic)
        break;
    }
  if (xc == NULL)
    {
      reply.state = -1;
      reply.detail = BadIC;
    }
  else
    {
      for (xi = input_list; xi != NULL; xi = xi->next)
        {
          if (xi->w == xc->focus_window)
            break;
        }
      if (xi == NULL)
        {
          reply.state = -1;
          reply.detail = BadFocusWindow;
        }
      else
        {
          remove_inputlist (xi);
          if (cur_input == xi)
            cur_input = 0;
          Free ((char *) xi);
          save_cur_p = cur_p;
          save_cur_x = cur_x;
          save_c_c = c_c;
          cur_x = xc;
          if (IsPreeditNothing (xc))
            {
              cur_p = xc->root_pointer->ximclient;
            }
          else
            {
              cur_p = cur_x;
            }
          c_c = cur_p->cur_xl->w_c;
          cur_rk = c_c->rk;
          cur_rk_table = cur_rk->rk_table;
#ifdef  CALLBACKS
          if (IsPreeditPosition (xc) || IsPreeditArea (xc) || IsPreeditCallbacks (xc))
            {
#else /* CALLBACKS */
          if (IsPreeditPosition (xc) || IsPreeditArea (xc))
            {
#endif /* CALLBACKS */
              invisual_window ();
            }
#ifdef  CALLBACKS
          if (IsStatusArea (xc) || IsStatusCallbacks (xc))
            {
#else /* CALLBACKS */
          if (IsStatusArea (xc))
            {
#endif /* CALLBACKS */
              invisual_status ();
            }
          cur_p = save_cur_p;
          cur_x = save_cur_x;
          c_c = save_c_c;
          if (c_c)
            {
              cur_rk = c_c->rk;
              cur_rk_table = cur_rk->rk_table;
            }
          reply.state = 0;
          reply.detail = 0;
        }
    }
  if (need_byteswap () == True)
    {
      byteswap_EventReply (&reply);
    }
  if ((_WriteToClient (&reply, sz_ximEventReply) == -1) || (_Send_Flush () == -1))
    return;
  return;
}

void
ChangeIC ()
{
  register XIMClientRec *xc;
  XIMNestLangRec *p = NULL;
  short detail;
  ximChangeICReq req;
  ximICValuesReq ic_req;
  ximICAttributesReq pre_req, st_req;
  ximEventReply reply;
  int error = 0, ret, cur_is_world = 0;

  if (EACH_REQUEST (&req, sz_ximChangeICReq) == -1)
    return;
  if (need_byteswap () == True)
    {
      req.xic = byteswap_l (req.xic);
    }

  if (_ReadFromClient (&ic_req, sz_ximICValuesReq) == -1)
    return;
  if (need_byteswap () == True)
    {
      byteswap_ICValuesReq (&ic_req);
    }
  if ((ret = read_strings (&use_langs, &use_langs_len, ic_req.nbytes)) < 0)
    {
      if (ret == -1)
        return;
      error++;
    }
  if ((ret = read_strings (&cur_l, &cur_l_len, ic_req.nbytes2)) < 0)
    {
      if (ret == -1)
        return;
      error++;
    }

  if (_ReadFromClient (&pre_req, sz_ximICAttributesReq) == -1)
    return;
  if (need_byteswap () == True)
    {
      byteswap_ICAttributesReq (&pre_req);
    }
  if ((ret = read_strings (&pre_font, &pre_font_len, pre_req.nbytes)) < 0)
    {
      if (ret == -1)
        return;
      error++;
    }

  if (_ReadFromClient (&st_req, sz_ximICAttributesReq) == -1)
    return;
  if (need_byteswap () == True)
    {
      byteswap_ICAttributesReq (&st_req);
    }
  if ((ret = read_strings (&st_font, &st_font_len, st_req.nbytes)) < 0)
    {
      if (ret == -1)
        return;
      error++;
    }
  if (error > 0)
    {
      reply.state = -1;
      reply.detail = AllocError;
      goto ERROR_RET;
    }

  for (xc = ximclient_list; xc != NULL; xc = xc->next)
    {
      if (xc->number == req.xic)
        break;
    }
  if (xc == NULL)
    {
      reply.state = -1;
      reply.detail = BadIC;
    }
  else
    {
      if (cur_l && *cur_l)
        {
          if ((cur_is_world = get_langlist (cur_l, &p)) < 0)
            {
              reply.state = -1;
              reply.detail = AllocError;
              goto ERROR_RET;
            }
        }
      if (change_client (xc, &ic_req, &pre_req, &st_req, p, cur_is_world, pre_font, st_font, &detail) == -1)
        {
          reply.state = -1;
          reply.detail = detail;
        }
      else
        {
          reply.state = 0;
          reply.detail = 0;
        }
      if (p)
        free_langlist (p);
    }
ERROR_RET:
  if (need_byteswap () == True)
    {
      byteswap_EventReply (&reply);
    }
  if ((_WriteToClient (&reply, sz_ximEventReply) == -1) || (_Send_Flush () == -1))
    return;
  return;
}

void
DestroyIC ()
{
  ximDestroyICReq req;
  ximEventReply reply;
  register XIMClientRec *xc;

  if (EACH_REQUEST (&req, sz_ximDestroyICReq) == -1)
    return;
  if (need_byteswap () == True)
    {
      req.xic = byteswap_l (req.xic);
    }

  for (xc = ximclient_list; xc != NULL; xc = xc->next)
    {
      if (xc->number == req.xic)
        break;
    }
  if (xc == NULL)
    {
      reply.state = -1;
      reply.detail = BadIC;
    }
  else
    {
      reply.state = 0;
      reply.detail = 0;
    }
  if (need_byteswap () == True)
    {
      byteswap_EventReply (&reply);
    }
  if ((_WriteToClient (&reply, sz_ximEventReply) == -1) || (_Send_Flush () == -1))
    return;
  if (xc)
    {
      (void) destroy_client (xc);
    }
  return;
}

void
destroy_for_sock (fd)
     int fd;
{
  register XIMClientRec *xc;

  for (xc = ximclient_list; xc != NULL; xc = xc->next)
    {
      if (xc->fd == fd)
        {
          (void) destroy_client (xc);
/*
            break;
*/
        }
    }
}

void
destroy_xl (xc, xl)
     register XIMClientRec *xc;
     register XIMLangRec *xl;
{
  remove_locale_to_xl (xl, NULL);
#ifdef X11R5
  if (xl->xlc)
    XFree ((char *) xl->xlc);
#endif /* X11R5 */
  if (!(IsPreeditNothing (xc)))
    {
      if (IsPreeditArea (xc))
        {
          XDestroyWindow (dpy, xl->wp[0]);
        }
      else if (IsPreeditPosition (xc))
        {
          XDestroyWindow (dpy, xl->wp[0]);
          XDestroyWindow (dpy, xl->wp[1]);
          XDestroyWindow (dpy, xl->wp[2]);
          XDestroyWindow (dpy, xl->wn[0]);
          XDestroyWindow (dpy, xl->wn[1]);
          XDestroyWindow (dpy, xl->wn[2]);
        }
      if (xl->pe_fs)
        XFreeFontSet (dpy, xl->pe_fs);
      if (xl->pe_b_char)
        Free ((char *) xl->pe_b_char);
      del_client (xl->w_c, 0);
      Free ((char *) xl->buf);
    }
  if (!(IsStatusNothing (xc)))
    {
      if (IsStatusArea (xc))
        {
          XDestroyWindow (dpy, xl->ws);
        }
      if (xl->st_fs)
        XFreeFontSet (dpy, xl->st_fs);
      if (xl->st_b_char)
        Free ((char *) xl->st_b_char);
      Free ((char *) xl->buf0);
    }
  Free ((char *) xl);
}

void
destroy_client (xc)
     register XIMClientRec *xc;
{
  register int i;
  register XIMInputRec *xi;

  if (cur_x && cur_x == xc)
    {
      if (
#ifdef  CALLBACKS
           IsPreeditCallbacks (xc) ||
#endif /* CALLBACKS */
           IsPreeditPosition (xc) || IsPreeditArea (xc))
        {
          invisual_window ();
        }
      if (!IsPreeditNothing (xc) && c_c->func_stack_count >= 0 && c_c->command_func_stack[c_c->func_stack_count] != 0)
        {
          (*c_c->command_func_stack[c_c->func_stack_count]) (-99);
        }
      cur_input = NULL;
      cur_x = NULL;
      cur_p = NULL;
      c_c = NULL;
    }
  for (xi = input_list; xi != NULL; xi = xi->next)
    {
      if (xi->pclient == xc)
        {
          remove_inputlist (xi);
          if (cur_input == xi)
            cur_input = NULL;
          Free ((char *) xi);
        }
    }
  for (i = 0; i < xc->lang_num; i++)
    {
      destroy_xl (xc, xc->xl[i]);
    }
  remove_ximclientlist (xc);
  if (xc->yes_no)
    {
      remove_box (xc->yes_no->title);
      remove_box (xc->yes_no->button[0]);
      remove_box (xc->yes_no->button[1]);
      Free ((char *) xc->yes_no);
    }
  if (xc->using_language)
    Free ((char *) xc->using_language);
  if (xc->user_name)
    Free ((char *) xc->user_name);
  if (xc->pe.fontset)
    Free ((char *) xc->pe.fontset);
  if (xc->st.fontset)
    Free ((char *) xc->st.fontset);
  if (xc->pe.gc)
    XFreeGC (dpy, xc->pe.gc);
  if (xc->pe.invertgc)
    XFreeGC (dpy, xc->pe.invertgc);
  if (xc->pe.reversegc)
    XFreeGC (dpy, xc->pe.reversegc);
  if (xc->st.gc)
    XFreeGC (dpy, xc->st.gc);
  if (xc->st.reversegc)
    XFreeGC (dpy, xc->st.reversegc);
  if (xc->xl)
    Free ((char *) xc->xl);
  if (IsPreeditNothing (xc))
    {
      check_root_mapping (xc->root_pointer);
    }
  Free ((char *) xc);
  return;
}

void
ResetIC ()
{
  ximResetICReq req;
  ximEventReply reply;
  register XIMClientRec *xc;
  WnnClientRec *save_c_c;
  XIMClientRec *save_cur_p, *save_cur_x;
  int ret;

  if (EACH_REQUEST (&req, sz_ximResetICReq) == -1)
    return;
  if (need_byteswap () == True)
    {
      req.xic = byteswap_l (req.xic);
    }

  for (xc = ximclient_list; xc != NULL; xc = xc->next)
    {
      if (xc->number == req.xic)
        break;
    }
  if (xc == NULL)
    {
      reply.state = -1;
      reply.detail = BadIC;
    }
  else
    {
      save_c_c = c_c;
      save_cur_p = cur_p;
      save_cur_x = cur_x;
      cur_x = xc;
      if (IsPreeditNothing (xc))
        {
          cur_p = xc->root_pointer->ximclient;
        }
      else
        {
          cur_p = cur_x;
        }
      c_c = cur_p->cur_xl->w_c;
      ret = reset_c_b ();
      JWMline_clear (0);
      c_c = save_c_c;
      cur_p = save_cur_p;
      cur_x = save_cur_x;
      if (ret == -1)
        {
          reply.state = -1;
          reply.detail = BadIC;
        }
      else
        {
          reply.state = 0;
          reply.detail = 0;
        }
    }
  if (need_byteswap () == True)
    {
      byteswap_EventReply (&reply);
    }
  if ((_WriteToClient (&reply, sz_ximEventReply) == -1) || (_Send_Flush () == -1))
    return;
  return;
}

void
free_langlist (p)
     register XIMNestLangList p;
{
  register XIMNestLangList n;
  for (; p != NULL; p = n)
    {
      n = p->next;
      Free ((char *) p);
    }
}

int
get_langlist (lang, nl)
     char *lang;
     XIMNestLangList *nl;
{
  register char *d, *save_d, *p, *alloc, *next;
  register int alias_name_len, lc_name_len, have_world = 0;
  register XIMNestLangList pp = NULL, prev = NULL, ls;
  XIMNestLangList w_ls;
  register XIMLcNameRec *lnl;

  *nl = NULL;
  if (lang == NULL || *lang == '\0')
    return (0);
  if (!(save_d = d = alloc_and_copy (lang)))
    return (-1);
  for (;;)
    {
      if (p = index ((p = d), ';'))
        {
          *p = '\0';
          next = ++p;
        }
      else
        {
          next = NULL;
        }
      if (p = index ((p = d), ','))
        {
          *(p++) = '\0';
          alias_name_len = strlen (p);
        }
      else
        {
          alias_name_len = 0;
        }
      lc_name_len = strlen (d);

      if (!strcmp (world_locale, d))
        {
          have_world++;
          d = next;
          if (!d)
            break;
          continue;
        }
      for (lnl = lc_name_list; lnl; lnl = lnl->next)
        {
          if (!strcmp (lnl->lc_name, d))
            break;
        }
      if (!lnl)
        {
          print_out1 ("I don't support such locale \"%s\".", d);
          d = next;
          if (!d)
            break;
          continue;
        }
      if (!(alloc = Calloc (1, (sizeof (XIMNestLangRec) + lc_name_len + alias_name_len + 2))))
        {
          malloc_error ("allocation langlist struct");
          return (-1);
        }
      ls = (XIMNestLangList) alloc;
      alloc += sizeof (XIMNestLangRec);
      ls->lc_name = alloc;
      strcpy (ls->lc_name, d);
      alloc += lc_name_len + 1;
      if (alias_name_len > 0)
        {
          ls->alias_name = alloc;
          strcpy (ls->alias_name, p);
        }
      ls->lang_db = lnl->lang_db;
      if (prev == NULL)
        {
          pp = ls;
        }
      else
        {
          prev->next = ls;
        }
      prev = ls;
      ls->next = NULL;
      d = next;
      if (!d)
        break;
    }
  Free (save_d);
  if (have_world)
    {
      if (get_langlist (world_lang, &w_ls) < 0)
        return (-1);
      if (w_ls)
        {
          if (pp)
            {
              ls->next = w_ls;
            }
          else
            {
              pp = w_ls;
              have_world++;
            }
        }
      else
        {
          have_world = 0;
        }
    }
  *nl = pp;
  return (have_world);
}

#ifdef SPOT
void
ChangeSpot ()
{
  register XIMClientRec *xc;
  ximChangeSpotReq req;
  ximEventReply reply;

  if (EACH_REQUEST (&req, sz_ximChangeSpotReq) == -1)
    return;
  if (need_byteswap () == True)
    {
      req.xic = byteswap_l (req.xic);
      req.spot_x = byteswap_s (req.spot_x);
      req.spot_y = byteswap_s (req.spot_y);
    }

  for (xc = ximclient_list; xc != NULL; xc = xc->next)
    {
      if (xc->number == req.xic)
        break;
    }
  if (xc == NULL)
    {
      reply.state = -1;
      reply.detail = BadIC;
    }
  else
    {
      if (change_spotlocation (xc, req.spot_x, req.spot_y) == -1)
        {
          reply.state = -1;
          reply.detail = 0;
        }
      else
        {
          reply.state = 0;
          reply.detail = 0;
        }
    }
  if (need_byteswap () == True)
    {
      byteswap_EventReply (&reply);
    }
  if ((_WriteToClient (&reply, sz_ximEventReply) == -1) || (_Send_Flush () == -1))
    return;
  return;
}
#endif /* SPOT */
