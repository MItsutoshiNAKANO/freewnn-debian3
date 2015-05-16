/*
 * $Id: change.c,v 1.15.2.1 1999/02/08 08:08:39 yamasita Exp $
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

XGCValues xgcv;

void
reset_preedit (xc)
     register XIMClientRec *xc;
{
  XIMClientRec *save_cur_p, *save_cur_x;
  WnnClientRec *save_c_c;

  if (!xc->cur_xl->w_c->h_flag && xc->cur_xl->max_pos)
    {
      save_c_c = c_c;
      save_cur_p = cur_p;
      save_cur_x = cur_x;
      cur_p = cur_x = xc;
      c_c = cur_p->cur_xl->w_c;
      cur_rk = c_c->rk;
      cur_rk_table = cur_rk->rk_table;
      if (IsPreeditPosition (xc))
        {
          invisual_window1 ();
          xc->cur_xl->visible = 0;
          visual_window1 ();
          check_scroll ();
#ifdef  CALLBACKS
        }
      else if (IsPreeditArea (xc) || IsPreeditCallbacks (xc))
        {
#else /* CALLBACKS */
        }
      else if (IsPreeditArea (xc))
        {
#endif /* CALLBACKS */
          invisual_window ();
          xc->cur_xl->visible = 0;
          visual_window ();
        }
      c_c = save_c_c;
      cur_p = save_cur_p;
      cur_x = save_cur_x;
      if (c_c)
        {
          cur_rk = c_c->rk;
          cur_rk_table = cur_rk->rk_table;
        }
    }
}

void
reset_status (xc)
     register XIMClientRec *xc;
{
  XIMClientRec *save_cur_p, *save_cur_x;
  WnnClientRec *save_c_c;

  save_c_c = c_c;
  save_cur_p = cur_p;
  save_cur_x = cur_x;
  cur_p = cur_x = xc;
  c_c = cur_p->cur_xl->w_c;
  cur_rk = c_c->rk;
  cur_rk_table = cur_rk->rk_table;
#ifdef  CALLBACKS
  if (IsStatusArea (xc) || IsStatusCallbacks (xc))
    {
#else /* CALLBACKS */
  if (IsStatusArea (xc))
    {
#endif /* CALLBACKS */
      if (!henkan_off_flag)
        {
          disp_mode ();
        }
      else
        {
          display_henkan_off_mode ();
        }
    }
  c_c = save_c_c;
  cur_p = save_cur_p;
  cur_x = save_cur_x;
  if (c_c)
    {
      cur_rk = c_c->rk;
      cur_rk_table = cur_rk->rk_table;
    }
}

unsigned long
update_ic (xc, ic, pre, st, pre_font, st_font, ret)
     register XIMClientRec *xc;
     register ximICValuesReq *ic;
     register ximICAttributesReq *pre, *st;
     char *pre_font, *st_font;
     int *ret;
{
  unsigned long mask;
  unsigned long dirty = 0L;

  *ret = 0;
  mask = ic->mask;

  if (mask & (1 << ICInputStyle))
    {
      xc->input_style = ic->input_style;
    }

  if (mask & (1 << ICClientWindow))
    {
      if (xc->w == xc->root_pointer->root_window)
        {
          xc->w = ic->c_window;
          dirty |= ChClientWindow;
        }
    }
  if (mask & (1 << ICFocusWindow))
    {
      if (xc->focus_window != ic->focus_window)
        {
          xc->focus_window = ic->focus_window;
          dirty |= ChFocusWindow;
        }
    }
  if (mask & (1 << ICArea))
    {
      if ((xc->pe.area.x != pre->area_x) || (xc->pe.area.y != pre->area_y))
        {
          xc->pe.area.x = pre->area_x;
          xc->pe.area.y = pre->area_y;
          dirty |= ChPreeditLocation;
        }
      if ((xc->pe.area.width != pre->area_width) || (xc->pe.area.height != pre->area_height))
        {
          xc->pe.area.width = pre->area_width;
          xc->pe.area.height = pre->area_height;
          dirty |= ChPreeditSize;
        }
    }
  if (mask & (1 << ICAreaNeeded))
    {
      if (xc->pe.area_needed.width < pre->areaneeded_width)
        {
          xc->pe.area_needed.width = pre->areaneeded_width;
        }
      if (xc->pe.area_needed.height < pre->areaneeded_height)
        {
          xc->pe.area_needed.height = pre->areaneeded_height;
        }
    }
  if (mask & (1 << ICColormap))
    {
      if (xc->pe.colormap != pre->colormap)
        {
          xc->pe.colormap = pre->colormap;
          dirty |= ChPreColormap;
        }
    }
  if (mask & (1 << ICForeground))
    {
      if (xc->pe.fg != pre->foreground)
        {
          xc->pe.fg = pre->foreground;
          dirty |= ChPreForeground;
        }
    }
  if (mask & (1 << ICBackground))
    {
      if (xc->pe.bg != pre->background)
        {
          xc->pe.bg = pre->background;
          dirty |= ChPreBackground;
        }
    }
  if (mask & (1 << ICBackgroundPixmap))
    {
      if (xc->pe.bg_pixmap != pre->pixmap)
        {
          xc->pe.bg_pixmap = pre->pixmap;
          dirty |= ChPreBackgroundPixmap;
        }
    }
  if (pre_font && (mask & (1 << ICFontSet)))
    {
      if (xc->pe.fontset != NULL && *(xc->pe.fontset) != '\0')
        {
          Free (xc->pe.fontset);
        }
      if (!(xc->pe.fontset = alloc_and_copy (pre_font)))
        {
          *ret = 1;
          return (0);
        }
      dirty |= ChPreFontSet;
    }
  if (mask & (1 << ICLineSpace))
    {
      if (xc->pe.line_space != pre->line_space)
        {
          xc->pe.line_space = pre->line_space;
          dirty |= ChPreLineSpace;
        }
    }
  if (mask & (1 << ICCursor))
    {
      if (xc->pe.cursor != pre->cursor)
        {
          xc->pe.cursor = pre->cursor;
          dirty |= ChPreCursor;
        }
    }
  if (mask & (1 << (ICArea + StatusOffset)))
    {
      if ((xc->st.area.x != st->area_x) || (xc->st.area.y != st->area_y))
        {
          xc->st.area.x = st->area_x;
          xc->st.area.y = st->area_y;
          dirty |= ChStatusLocation;
        }
      if ((xc->st.area.width != st->area_width) || (xc->st.area.height != st->area_height))
        {
          xc->st.area.width = st->area_width;
          xc->st.area.height = st->area_height;
          dirty |= ChStatusSize;
        }
    }
  if (mask & (1 << (ICAreaNeeded + StatusOffset)))
    {
/* No op area_needed of Status because the area of it is fixed.
        if (xc->st.area_needed.width < st->areaneeded_width) {
            xc->st.area_needed.width = st->areaneeded_width;
        }
        if (xc->st.area_needed.height < st->areaneeded_height) {
            xc->st.area_needed.height = st->areaneeded_height;
        }
*/
    }
  if (mask & (1 << (ICColormap + StatusOffset)))
    {
      if (xc->st.colormap != st->colormap)
        {
          xc->st.colormap = st->colormap;
          dirty |= ChStColormap;
        }
    }
  if (mask & (1 << (ICForeground + StatusOffset)))
    {
      if (xc->st.fg != st->foreground)
        {
          xc->st.fg = st->foreground;
          dirty |= ChStForeground;
        }
    }
  if (mask & (1 << (ICBackground + StatusOffset)))
    {
      if (xc->st.bg != st->background)
        {
          xc->st.bg = st->background;
          dirty |= ChStBackground;
        }
    }
  if (mask & (1 << (ICBackgroundPixmap + StatusOffset)))
    {
      if (xc->st.bg_pixmap != st->pixmap)
        {
          xc->st.bg_pixmap = st->pixmap;
          dirty |= ChStBackgroundPixmap;
        }
    }
  if (st_font && (mask & (1 << (ICFontSet + StatusOffset))))
    {
      if (xc->st.fontset != NULL && *(xc->st.fontset) != '\0')
        {
          Free (xc->st.fontset);
        }
      if (!(xc->st.fontset = alloc_and_copy (st_font)))
        {
          *ret = 1;
          return (0);
        }
      dirty |= ChStFontSet;
    }
  if (mask & (1 << (ICLineSpace + StatusOffset)))
    {
      if (xc->st.line_space != st->line_space)
        {
          xc->st.line_space = st->line_space;
          dirty |= ChStLineSpace;
        }
    }
  if (mask & (1 << (ICCursor + StatusOffset)))
    {
      if (xc->st.cursor != st->cursor)
        {
          xc->st.cursor = st->cursor;
          dirty |= ChStCursor;
        }
    }
  if (mask & (1 << (ICChangeLocaleCB)))
    {
      xc->have_ch_lc = 1;
    }
  return (dirty);
}


/*
 * update spotlocation.
 */
unsigned long
update_spotlocation (xc, ic, pre)
     register XIMClientRec *xc;
     register ximICValuesReq *ic;
     register ximICAttributesReq *pre;
{
  unsigned long dirty = 0L;

  if (ic->mask & (1 << ICSpotLocation))
    {
      if ((xc->point.x != pre->spot_x) || (xc->point.y != (pre->spot_y - FontAscent (xc->cur_xl))))
        {
          xc->point.x = pre->spot_x;
          xc->point.y = (pre->spot_y - FontAscent (xc->cur_xl));
          dirty = ChPreeditLocation;
        }
    }
  return (dirty);
}

int
change_current_language (xc, cur_l, cur_is_world)
     register XIMClientRec *xc;
     register XIMNestLangRec *cur_l;
     int cur_is_world;
{
  register XIMLangRec *xl;
  register XIMNestLangRec *p, *pp;
  register int i, ret;
  int new = 0;

  xc->world_on = 0;
  for (pp = cur_l; pp != NULL; pp = pp->next)
    {
      for (i = 0; i < xc->lang_num; i++)
        {
          for (p = xc->xl[i]->lc_list; p; p = p->next)
            {
              if (!strcmp (pp->lc_name, p->lc_name))
                {
                  if (cur_is_world > 0)
                    break;
                  if (xc->cur_xl == xc->xl[i] && xc->xl[i]->cur_lc == pp)
                    {
                      return (0);
                    }
                  cur_p = cur_x = xc;
                  change_lang (xc->xl[i], p);
                  return (0);
                }
            }
          if (p)
            break;
        }
      if (i < xc->lang_num)
        continue;
      if ((ret = add_lang_env (xc, pp, &new)) == -2)
        {
          return (-1);
        }
      else if (ret == -1)
        {
          if (!cur_is_world)
            return (-1);
        }
      else
        {
          xl = xc->xl[ret];
          if (new != 0)
            {
              xc->lang_num++;
            }
          if (!cur_is_world)
            {
              change_lang (xl, pp);
              return (0);
            }
        }
    }
  if (cur_is_world)
    {
      cur_p = cur_x = xc;
      xc->world_on = 1;
      xc->have_world = 1;
      change_lang (xc->xl[0], xc->xl[0]->lc_list);
      return (0);
    }
  return (-1);
}

static void
change_client_window (xc, old)
     register XIMClientRec *xc;
     Window old;
{
  register int i;
  register Window w = xc->w;

  if (old)
    XSelectInput (dpy, old, NoEventMask);
  XSelectInput (dpy, w, StructureNotifyMask);

  if (IsPreeditPosition (xc) && xc->focus_window == 0)
    {
      for (i = 0; i < xc->lang_num; i++)
        {
          XReparentWindow (dpy, xc->xl[i]->wp[0], w, 0, 0);
          XReparentWindow (dpy, xc->xl[i]->wp[1], w, 0, 0);
          XReparentWindow (dpy, xc->xl[i]->wp[2], w, 0, 0);
          XReparentWindow (dpy, xc->xl[i]->wn[0], w, 0, 0);
          XReparentWindow (dpy, xc->xl[i]->wn[1], w, 0, 0);
          XReparentWindow (dpy, xc->xl[i]->wn[2], w, 0, 0);
        }
    }
  else if (IsPreeditArea (xc))
    {
      for (i = 0; i < xc->lang_num; i++)
        {
          XReparentWindow (dpy, xc->xl[i]->wp[0], w, 0, 0);
        }
    }
  if (IsStatusArea (xc))
    {
      for (i = 0; i < xc->lang_num; i++)
        {
          XReparentWindow (dpy, xc->xl[i]->ws, w, 0, 0);
        }
    }
}

static int
change_focus_window (xc, old)
     register XIMClientRec *xc;
     Window old;
{
  register XIMInputRec *xi;
  register int i;
  register Window w = xc->focus_window;
  XWindowAttributes attr;

  /*
   * Get the attributes of focus_window.
   */
  if (XGetWindowAttributes (dpy, w, &attr) == 0)
    {
      return (BadFocusWindow);
    }
  if (old)
    XSelectInput (dpy, old, NoEventMask);
  XSelectInput (dpy, w, StructureNotifyMask);
  xc->focus_area.x = attr.x;
  xc->focus_area.y = attr.y;
  xc->focus_area.width = attr.width;
  xc->focus_area.height = attr.height;

  for (xi = input_list; xi != NULL; xi = xi->next)
    {
      if (xc == xi->pclient)
        {
          xi->w = w;
        }
    }
  if (IsPreeditPosition (xc))
    {
      if (!(xc->mask & (1 << ICArea)))
        {
          xc->pe.area.x = xc->focus_area.x;
          xc->pe.area.y = xc->focus_area.y;
          xc->pe.area.width = xc->focus_area.width;
          xc->pe.area.height = xc->focus_area.height;
        }
      for (i = 0; i < xc->lang_num; i++)
        {
          XReparentWindow (dpy, xc->xl[i]->wp[0], w, 0, 0);
          XReparentWindow (dpy, xc->xl[i]->wp[1], w, 0, 0);
          XReparentWindow (dpy, xc->xl[i]->wp[2], w, 0, 0);
          XReparentWindow (dpy, xc->xl[i]->wn[0], w, 0, 0);
          XReparentWindow (dpy, xc->xl[i]->wn[1], w, 0, 0);
          XReparentWindow (dpy, xc->xl[i]->wn[2], w, 0, 0);
        }
    }
  return (0);
}

void
change_client_area (xc, x, y, width, height)
     register XIMClientRec *xc;
     int x, y, width, height;
{
  if (IsPreeditPosition (xc))
    {
      xc->client_area.x = x;
      xc->client_area.y = y;
      xc->client_area.width = width;
      xc->client_area.height = height;
      reset_preedit (xc);
    }
}

void
change_focus_area (xc, x, y, width, height)
     register XIMClientRec *xc;
     int x, y, width, height;
{
  if (IsPreeditPosition (xc))
    {
      if (!(xc->mask & (1 << ICArea)))
        {
          xc->pe.area.x = xc->focus_area.x;
          xc->pe.area.y = xc->focus_area.y;
          xc->pe.area.width = xc->focus_area.width;
          xc->pe.area.height = xc->focus_area.height;
        }
      xc->focus_area.x = x;
      xc->focus_area.y = y;
      xc->focus_area.width = width;
      xc->focus_area.height = height;
      reset_preedit (xc);
    }
}

static void
change_pre_fontset (xc)
     register XIMClientRec *xc;
{
  register int i, j;
  register XIMLangRec *xl;
  register XFontSet save_fs;
  register XCharStruct *save_b_char;
  int width, height;

  if (IsPreeditPosition (xc) || IsPreeditArea (xc))
    {
      for (i = 0; i < xc->lang_num; i++)
        {
          xl = xc->xl[i];
          setlocale (LC_ALL, xl->lang_db->lc_name);
          save_fs = xl->pe_fs;
          save_b_char = xl->pe_b_char;
          if ((xl->pe_fs = create_font_set (xc->pe.fontset)) && (xl->pe_b_char = get_base_char (xl->pe_fs)))
            {
              if (save_fs)
                XFreeFontSet (dpy, save_fs);
              if (xl->pe_b_char)
                Free (xl->pe_b_char);
              xl->pe_b_char = get_base_char (xl->pe_fs);
              height = FontHeight (xl);
              if (IsPreeditPosition (xc))
                {
                  width = FontWidth (xl) * (xc->maxcolumns + 1);
                  for (j = 0; j < 3; j++)
                    {
                      XResizeWindow (dpy, xl->w[j], width, height);
                      XResizeWindow (dpy, xl->wn[j], FontWidth (xl), height);
                    }
                }
              else
                {
                  XResizeWindow (dpy, xl->w[0], FontWidth (xl) * (xc->maxcolumns + 1), height);
                  XResizeWindow (dpy, xl->wn[0], FontWidth (xl), height);
                  XResizeWindow (dpy, xl->wn[1], FontWidth (xl), height);
                }
            }
          else
            {
              if (xl->pe_fs)
                XFreeFontSet (dpy, xl->pe_fs);
              xl->pe_fs = save_fs;
              xl->pe_b_char = save_b_char;
            }
          if (xc->pe.area_needed.width < FontWidth (xl) * 2)
            xc->pe.area_needed.width = FontWidth (xl) * 2;
          if (xc->pe.area_needed.height < FontHeight (xl))
            xc->pe.area_needed.height = FontHeight (xl);
        }
    }
}


static void
change_pre_location (xc)
     register XIMClientRec *xc;
{
  register int i;

  if (IsPreeditPosition (xc))
    {
      /*
       * Need not calling X functions, because the change_client() calles
       * invisual_window() and visual_window().
       */
    }
  else if (IsPreeditArea (xc))
    {
      for (i = 0; i < xc->lang_num; i++)
        {
          XMoveWindow (dpy, xc->xl[i]->wp[0], PreeditX (xc), PreeditY (xc));
        }
    }
}

static void
change_pre_size (xc)
     register XIMClientRec *xc;
{
  register int i;
  register XIMLangRec *xl;

  if (IsPreeditPosition (xc))
    {
      /*
       * Need not calling X functions, because the change_client() calles
       * invisual_window() and visual_window().
       */
    }
  else if (IsPreeditArea (xc))
    {
      for (i = 0; i < xc->lang_num; i++)
        {
          xl = xc->xl[i];
          xc->max_columns = (int) PreeditWidth (xc) / (int) FontWidth (xl);
          if (xc->max_columns <= 0)
            xc->max_columns = 1;
          XResizeWindow (dpy, xl->wp[0], PreeditWidth (xc), PreeditHeight (xc));
          XMoveWindow (dpy, xl->wn[1], PreeditWidth (xc) - FontWidth (xl), 0);
        }
    }
}

static void
change_pre_colormap (xc)
     register XIMClientRec *xc;
{
  register int i;
  register XIMLangRec *xl;
  register Colormap c = xc->pe.colormap;

  if (IsPreeditPosition (xc) || IsPreeditArea (xc))
    {
      for (i = 0; i < xc->lang_num; i++)
        {
          xl = xc->xl[i];
          XSetWindowColormap (dpy, xl->wp[0], c);
          XSetWindowColormap (dpy, xl->w[0], c);
          XSetWindowColormap (dpy, xl->wn[0], c);
          XSetWindowColormap (dpy, xl->wn[1], c);
          XSetWindowColormap (dpy, xl->wn[2], c);
          if (IsPreeditPosition (xc))
            {
              XSetWindowColormap (dpy, xl->wp[1], c);
              XSetWindowColormap (dpy, xl->w[1], c);
              XSetWindowColormap (dpy, xl->wp[2], c);
              XSetWindowColormap (dpy, xl->w[2], c);
            }
        }
    }
}

static void
change_pre_stdcolormap (xc)
     register XIMClientRec *xc;
{
  XStandardColormap cmap;

  XGetStandardColormap (dpy, xc->root_pointer->root_window, &cmap, xc->pe.std_colormap);
  xc->pe.colormap = cmap.colormap;
  change_pre_colormap (xc);
}

static void
change_pre_fg (xc)
     register XIMClientRec *xc;
{
  if (IsPreeditPosition (xc) || IsPreeditArea (xc))
    {
      xgcv.foreground = xc->pe.fg;
      if (xc->pe.gc)
        {
          XChangeGC (dpy, xc->pe.gc, GCForeground, &xgcv);
        }
      xgcv.background = xc->pe.fg;
      if (xc->pe.reversegc)
        {
          XChangeGC (dpy, xc->pe.reversegc, GCBackground, &xgcv);
        }
      xgcv.plane_mask = XOR (xc->pe.fg, xc->pe.bg);
      if (xc->pe.invertgc)
        {
          XChangeGC (dpy, xc->pe.invertgc, (GCForeground | GCPlaneMask), &xgcv);
        }
    }
}

static void
change_pre_bg (xc)
     register XIMClientRec *xc;
{
  register int i;
  register XIMLangRec *xl;
  register unsigned long bg = xc->pe.bg;

  if (IsPreeditPosition (xc) || IsPreeditArea (xc))
    {
      xgcv.background = xc->pe.bg;
      if (xc->pe.gc)
        {
          XChangeGC (dpy, xc->pe.gc, GCBackground, &xgcv);
        }
      xgcv.foreground = xc->pe.bg;
      if (xc->pe.reversegc)
        {
          XChangeGC (dpy, xc->pe.reversegc, GCForeground, &xgcv);
        }
      xgcv.plane_mask = XOR (xc->pe.fg, xc->pe.bg);
      if (xc->pe.invertgc)
        {
          XChangeGC (dpy, xc->pe.invertgc, GCPlaneMask, &xgcv);
        }
      for (i = 0; i < xc->lang_num; i++)
        {
          xl = xc->xl[i];
          XSetWindowBackground (dpy, xl->wp[0], bg);
          XSetWindowBackground (dpy, xl->w[0], bg);
          XSetWindowBackground (dpy, xl->wn[0], bg);
          XSetWindowBackground (dpy, xl->wn[1], bg);
          XSetWindowBackground (dpy, xl->wn[2], bg);
          if (IsPreeditPosition (xc))
            {
              XSetWindowBackground (dpy, xl->wp[1], bg);
              XSetWindowBackground (dpy, xl->w[1], bg);
              XSetWindowBackground (dpy, xl->wp[2], bg);
              XSetWindowBackground (dpy, xl->w[2], bg);
            }
        }
    }
}

static void
change_pre_bgpixmap (xc)
     register XIMClientRec *xc;
{
  register int i;
  register XIMLangRec *xl;
  register Pixmap bg_p = xc->pe.bg_pixmap;

  if (IsPreeditPosition (xc) || IsPreeditArea (xc))
    {
      for (i = 0; i < xc->lang_num; i++)
        {
          xl = xc->xl[i];
          XSetWindowBackgroundPixmap (dpy, xl->wp[0], bg_p);
          XSetWindowBackgroundPixmap (dpy, xl->w[0], bg_p);
          XSetWindowBackgroundPixmap (dpy, xl->wn[0], bg_p);
          XSetWindowBackgroundPixmap (dpy, xl->wn[1], bg_p);
          XSetWindowBackgroundPixmap (dpy, xl->wn[2], bg_p);
          if (IsPreeditPosition (xc))
            {
              XSetWindowBackgroundPixmap (dpy, xl->wp[1], bg_p);
              XSetWindowBackgroundPixmap (dpy, xl->w[1], bg_p);
              XSetWindowBackgroundPixmap (dpy, xl->wp[2], bg_p);
              XSetWindowBackgroundPixmap (dpy, xl->w[2], bg_p);
            }
        }
    }
}

static void
change_pre_linespace (xc)
     register XIMClientRec *xc;
{
  if (IsPreeditPosition (xc))
    {
    }
}

static void
change_pre_cursor (xc)
     register XIMClientRec *xc;
{
  /*
   * Not support
   */
}

static void
change_pre_attr (xc, mask)
     register XIMClientRec *xc;
     unsigned long mask;
{
  if (mask & ChPreFontSet)
    change_pre_fontset (xc);
  if (mask & ChPreeditLocation)
    change_pre_location (xc);
  if (mask & ChPreeditSize)
    change_pre_size (xc);
  if (mask & ChPreColormap)
    change_pre_colormap (xc);
  if (mask & ChPreStdColormap)
    change_pre_stdcolormap (xc);
  if (mask & ChPreForeground)
    change_pre_fg (xc);
  if (mask & ChPreBackground)
    change_pre_bg (xc);
  if (mask & ChPreBackgroundPixmap)
    change_pre_bgpixmap (xc);
  if (mask & ChPreLineSpace)
    change_pre_linespace (xc);
  if (mask & ChPreCursor)
    change_pre_cursor (xc);
}

static void
change_st_fontset (xc)
     register XIMClientRec *xc;
{
  register int i;
  register XIMLangRec *xl;
  register XFontSet save_fs;
  register XCharStruct *save_b_char;

  if (IsStatusArea (xc))
    {
      for (i = 0; i < xc->lang_num; i++)
        {
          xl = xc->xl[i];
          setlocale (LC_ALL, xl->lang_db->lc_name);
          save_fs = xl->st_fs;
          save_b_char = xl->st_b_char;
          if ((xl->st_fs = create_font_set (xc->st.fontset)) && (xl->st_b_char = get_base_char (xl->st_fs)))
            {
              if (save_fs)
                XFreeFontSet (dpy, save_fs);
              if (xl->st_b_char)
                Free (xl->st_b_char);
              xl->st_b_char = get_base_char (xl->st_fs);
              XResizeWindow (dpy, xl->ws, StatusWidth (xc), StatusHeight (xc));
              XClearWindow (dpy, xl->ws);
            }
          else
            {
              if (xl->st_fs)
                XFreeFontSet (dpy, xl->st_fs);
              xl->st_fs = save_fs;
              xl->st_b_char = save_b_char;
            }
          if (xc->st.area_needed.width < StatusFontWidth (xl) * MHL0);
          xc->st.area_needed.width = StatusFontWidth (xl) * MHL0;
          if (xc->st.area_needed.height < StatusFontHeight (xl))
            xc->st.area_needed.height = StatusFontHeight (xl);
        }
    }
}

static void
change_st_location (xc)
     register XIMClientRec *xc;
{
  register int i;

  if (IsStatusArea (xc))
    {
      for (i = 0; i < xc->lang_num; i++)
        {
          XMoveWindow (dpy, xc->xl[i]->ws, StatusX (xc), StatusY (xc));
        }
    }
}

static void
change_st_size (xc)
     register XIMClientRec *xc;
{
  register int i;

  if (IsStatusArea (xc))
    {
      for (i = 0; i < xc->lang_num; i++)
        {
          XResizeWindow (dpy, xc->xl[i]->ws, StatusWidth (xc), StatusHeight (xc));
        }
    }
}

static void
change_st_colormap (xc)
     register XIMClientRec *xc;
{
  register int i;

  if (IsStatusArea (xc))
    {
      for (i = 0; i < xc->lang_num; i++)
        {
          XSetWindowColormap (dpy, xc->xl[i]->ws, xc->st.colormap);
        }
    }
}

static void
change_st_stdcolormap (xc)
     register XIMClientRec *xc;
{
  XStandardColormap cmap;

  XGetStandardColormap (dpy, xc->root_pointer->root_window, &cmap, xc->st.std_colormap);
  xc->st.colormap = cmap.colormap;
  change_st_colormap (xc);
}

static void
change_st_fg (xc)
     register XIMClientRec *xc;
{
  if (IsStatusArea (xc))
    {
      xgcv.foreground = xc->st.fg;
      if (xc->st.gc)
        {
          XChangeGC (dpy, xc->st.gc, GCForeground, &xgcv);
        }
      xgcv.background = xc->st.fg;
      if (xc->st.reversegc)
        {
          XChangeGC (dpy, xc->st.reversegc, GCBackground, &xgcv);
        }
    }
}

static void
change_st_bg (xc)
     register XIMClientRec *xc;
{
  register int i;
  if (IsStatusArea (xc))
    {
      xgcv.background = xc->st.bg;
      if (xc->st.gc)
        {
          XChangeGC (dpy, xc->st.gc, GCBackground, &xgcv);
        }
      xgcv.foreground = xc->st.bg;
      if (xc->st.reversegc)
        {
          XChangeGC (dpy, xc->st.reversegc, GCForeground, &xgcv);
        }
      for (i = 0; i < xc->lang_num; i++)
        {
          XSetWindowBackground (dpy, xc->xl[i]->ws, xc->st.bg);
          XClearWindow (dpy, xc->xl[i]->ws);
        }
      XFlush (dpy);
    }
}

static void
change_st_bgpixmap (xc)
     register XIMClientRec *xc;
{
  register int i;

  if (IsStatusArea (xc))
    {
      for (i = 0; i < xc->lang_num; i++)
        {
          XSetWindowBackgroundPixmap (dpy, xc->xl[i]->ws, xc->st.bg_pixmap);
        }
    }
}

static void
change_st_linespace (xc)
     register XIMClientRec *xc;
{
  /*
   * Not support
   */
}

static void
change_st_cursor (xc)
     register XIMClientRec *xc;
{
  /*
   * Not support
   */
}

static void
change_st_attr (xc, mask)
     register XIMClientRec *xc;
     unsigned long mask;
{
  if (mask & ChStFontSet)
    change_st_fontset (xc);
  if (mask & ChStatusLocation)
    change_st_location (xc);
  if (mask & ChStatusSize)
    change_st_size (xc);
  if (mask & ChStColormap)
    change_st_colormap (xc);
  if (mask & ChStStdColormap)
    change_st_stdcolormap (xc);
  if (mask & ChStForeground)
    change_st_fg (xc);
  if (mask & ChStBackground)
    change_st_bg (xc);
  if (mask & ChStBackgroundPixmap)
    change_st_bgpixmap (xc);
  if (mask & ChStLineSpace)
    change_st_linespace (xc);
  if (mask & ChStCursor)
    change_st_cursor (xc);
}

/*
 * Update the client's attributes.
 */
int
change_client (xc, ic, pre, st, cur_l, cur_is_world, pre_font, st_font, detail)
     register XIMClientRec *xc;
     register ximICValuesReq *ic;
     register ximICAttributesReq *pre, *st;
     XIMNestLangRec *cur_l;
     int cur_is_world;
     char *pre_font, *st_font;
     short *detail;             /* If error is occured, set. */
{
  unsigned long dirty = 0L;
  WnnClientRec *save_c_c;
  XIMClientRec *save_cur_p, *save_cur_x;
  XIMLangDataBase *cur_lang_sv;
  int ret, error = 0;
  Window old_client_window, old_focus_window;

  xc->mask |= ic->mask;
  old_client_window = xc->w;
  old_focus_window = xc->focus_window;
  dirty = update_ic (xc, ic, pre, st, pre_font, st_font, &error);
  if (error)
    {
      *detail = (short) error;
      xc->focus_window = old_focus_window;
      return (-1);
    }

  if (dirty & ChClientWindow)
    change_client_window (xc, old_client_window);
  if (dirty & ChFocusWindow)
    {
      if ((ret = change_focus_window (xc, old_focus_window)) > 0)
        {
          *detail = (short) ret;
          xc->focus_window = old_focus_window;
          return (-1);
        }
    }
  if (IsPreeditArea (xc) || IsPreeditPosition (xc))
    {
      if (dirty & ChPreeditAttributes)
        change_pre_attr (xc, dirty);
      if (dirty & ChStatusAttributes)
        change_st_attr (xc, dirty);
      if (IsPreeditPosition (xc))
        {
          dirty |= update_spotlocation (xc, ic, pre);
        }

      if ((dirty & ChClientWindow) || (dirty & ChPreeditAttributes) || (IsPreeditPosition (xc) && (dirty & ChFocusWindow)))
        {
          reset_preedit (xc);
        }
      if ((dirty & ChClientWindow) || (dirty & ChStatusAttributes))
        {
          reset_status (xc);
        }
    }
  if (xc->mask & (1 << ICCurrentLanguage) && cur_l && (IsPreeditPosition (xc) || IsPreeditArea (xc)))
    {
      save_c_c = c_c;
      save_cur_p = cur_p;
      save_cur_x = cur_x;
      cur_lang_sv = cur_lang;
      ret = change_current_language (xc, cur_l, cur_is_world);
      c_c = save_c_c;
      cur_p = save_cur_p;
      cur_x = save_cur_x;
      cur_lang = cur_lang_sv;
      if (c_c)
        {
          cur_rk = c_c->rk;
          cur_rk_table = cur_rk->rk_table;
        }
      if (ret != 0)
        {
          *detail = BadSomething;
          return (-1);
        }
    }
  return (0);
}

#ifdef  SPOT
int
change_spotlocation (xc, x, y)
     register XIMClientRec *xc;
     register short x, y;
{
  if (!IsPreeditPosition (xc))
    return (-1);
  if ((xc->point.x == x) && (xc->point.y == (y - FontAscent (xc->cur_xl))))
    return (0);
  xc->point.x = x;
  xc->point.y = (y - FontAscent (xc->cur_xl));
  reset_preedit (xc);
  return (0);
}
#endif /* SPOT */
