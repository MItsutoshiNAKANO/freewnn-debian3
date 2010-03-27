/*
 * $Id: byte_swap.c,v 1.2 2001/06/14 18:16:14 ura Exp $
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

#include "commonhd.h"
#include "sdefine.h"
#include "xim.h"
#include "sheader.h"
#include "proto.h"
#include "ext.h"

unsigned long
byteswap_l (l)
     unsigned long l;
{
  return (((l & 0xff) << 24) | ((l & 0xff00) << 8) | ((l & 0xff0000) >> 8) | ((l >> 24) & 0xff));
}

unsigned short
byteswap_s (s)
     unsigned short s;
{
  return (((s & 0xff) << 8) | ((s >> 8) & 0xff));
}

void
byteswap_xevent (ev)
     register XKeyEvent *ev;
{
  ev->type = byteswap_l (ev->type);
  ev->serial = byteswap_l (ev->serial);
  ev->window = byteswap_l (ev->window);
  ev->root = byteswap_l (ev->root);
  ev->subwindow = byteswap_l (ev->subwindow);
  ev->time = byteswap_l (ev->time);
  ev->x_root = byteswap_l (ev->x_root);
  ev->y_root = byteswap_l (ev->y_root);
  ev->x = byteswap_l (ev->x);
  ev->y = byteswap_l (ev->y);
  ev->state = byteswap_l (ev->state);
  ev->keycode = byteswap_l (ev->keycode);
}

void
byteswap_GetIMReply (p)
     register ximGetIMReply *p;
{
  p->state = byteswap_s (p->state);
  p->num_styles = byteswap_s (p->num_styles);
  p->nbytes = byteswap_s (p->nbytes);
}

void
byteswap_CreateICReply (p)
     register ximCreateICReply *p;
{
  p->state = byteswap_s (p->state);
  p->detail = byteswap_s (p->detail);
  p->xic = byteswap_l (p->xic);
}

void
byteswap_NormalReply (p)
     register ximNormalReply *p;
{
  p->state = byteswap_s (p->state);
  p->detail = byteswap_s (p->detail);
}

void
byteswap_GetICReply (p)
     register ximGetICReply *p;
{
  p->state = byteswap_s (p->state);
  p->detail = byteswap_s (p->detail);
}

void
byteswap_ICValuesReq (p)
     register ximICValuesReq *p;
{
  p->mask = byteswap_l (p->mask);
  p->input_style = byteswap_l (p->input_style);
  p->c_window = byteswap_l (p->c_window);
  p->focus_window = byteswap_l (p->focus_window);
  p->filter_events = byteswap_l (p->filter_events);
  p->max_keycode = byteswap_l (p->max_keycode);
  p->nbytes = byteswap_s (p->nbytes);
  p->nbytes2 = byteswap_s (p->nbytes2);
}

void
byteswap_ICAttributesReq (p)
     register ximICAttributesReq *p;
{
  p->area_x = byteswap_s (p->area_x);
  p->area_y = byteswap_s (p->area_y);
  p->area_width = byteswap_s (p->area_width);
  p->area_height = byteswap_s (p->area_height);
  p->areaneeded_width = byteswap_s (p->areaneeded_width);
  p->areaneeded_height = byteswap_s (p->areaneeded_height);
  p->spot_x = byteswap_s (p->spot_x);
  p->spot_y = byteswap_s (p->spot_y);
  p->colormap = byteswap_l (p->colormap);
  p->std_colormap = byteswap_l (p->std_colormap);
  p->foreground = byteswap_l (p->foreground);
  p->background = byteswap_l (p->background);
  p->pixmap = byteswap_l (p->pixmap);
  p->line_space = byteswap_s (p->line_space);
  p->cursor = byteswap_l (p->cursor);
  p->nfonts = byteswap_s (p->nfonts);
  p->nbytes = byteswap_s (p->nbytes);
}

void
byteswap_EventReply (p)
     register ximEventReply *p;
{
  p->state = byteswap_s (p->state);
  p->detail = byteswap_s (p->detail);
  p->number = byteswap_s (p->number);
}

void
byteswap_ReturnReply (p)
     register ximReturnReply *p;
{
  p->type = byteswap_s (p->type);
  p->length = byteswap_s (p->length);
  p->keysym = byteswap_l (p->keysym);
}

#ifdef  CALLBACKS
void
byteswap_StatusDrawReply (p)
     register ximStatusDrawReply *p;
{
  p->type = byteswap_s (p->type);
  p->encoding_is_wchar = byteswap_s (p->encoding_is_wchar);
  p->length = byteswap_s (p->length);
  p->feedback = byteswap_s (p->feedback);
  p->bitmap = byteswap_l (p->bitmap);
}

void
byteswap_PreDrawReply (p)
     register ximPreDrawReply *p;
{
  p->caret = byteswap_s (p->caret);
  p->chg_first = byteswap_s (p->chg_first);
  p->chg_length = byteswap_s (p->chg_length);
  p->encoding_is_wchar = byteswap_s (p->encoding_is_wchar);
  p->length = byteswap_s (p->length);
  p->feedback = byteswap_l (p->feedback);
}

void
byteswap_PreCaretReply (p)
     register ximPreCaretReply *p;
{
  p->position = byteswap_s (p->position);
  p->direction = byteswap_s (p->direction);
  p->style = byteswap_s (p->style);
}
#endif /* CALLBACKS */
