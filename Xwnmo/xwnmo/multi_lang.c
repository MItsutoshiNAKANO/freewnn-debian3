/*
 * $Id: multi_lang.c,v 1.2 2001/06/14 18:16:16 ura Exp $
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
#ifdef X11R5
#include "Xlocaleint.h"
#endif /* X11R5 */
#include "sheader.h"
#include "proto.h"
#include "ext.h"


XSetWindowAttributes attributes;
XGCValues xgcv;

char *
get_default_font_name (lang)
     register char *lang;
{
  return (DEFAULT_FONTSET_NAME);
}

int
load_font_set (xl, p, s)
     register XIMLangRec *xl;
     register char *p, *s;
{
  xl->pe_fs = xl->st_fs = (XFontSet) NULL;
  xl->pe_b_char = xl->st_b_char = NULL;
  if ((xl->pe_fs = create_font_set (p)) && (xl->pe_b_char = get_base_char (xl->pe_fs)) && (xl->st_fs = create_font_set (p)) && (xl->st_b_char = get_base_char (xl->st_fs)))
    return (0);
  if (xl->pe_fs)
    XFreeFontSet (dpy, xl->pe_fs);
  if (xl->pe_b_char)
    Free (xl->pe_b_char);
  if (xl->st_fs)
    XFreeFontSet (dpy, xl->st_fs);
  if (xl->st_b_char)
    Free (xl->st_b_char);
  xl->pe_fs = xl->st_fs = (XFontSet) NULL;
  xl->pe_b_char = xl->st_b_char = NULL;
  return (-1);
}

int
add_locale_to_xl (xl, lang)
     XIMLangRec *xl;
     XIMNestLangRec *lang;
{
  register XIMNestLangRec *lr;
  register int size;
  register char *alloc;

  size = sizeof (XIMNestLangRec) + strlen (lang->lc_name) + 1;
  if (lang->alias_name)
    size += strlen (lang->alias_name) + 1;

  if (!(alloc = Calloc (1, size)))
    {
      malloc_error ("allocation of work data");
      return (-1);
    }
  lr = (XIMNestLangRec *) alloc;
  alloc += sizeof (XIMNestLangRec);
  lr->lc_name = alloc;
  strcpy (lr->lc_name, lang->lc_name);
  if (lang->alias_name)
    {
      alloc += strlen (lang->lc_name) + 1;
      lr->alias_name = alloc;
      strcpy (lr->alias_name, lang->alias_name);
    }
  else
    {
      lr->alias_name = NULL;
    }
  lr->lang_db = NULL;
  lr->next = xl->lc_list;
  xl->lc_list = lr;
  return (0);
}

void
remove_locale_to_xl (xl, lang)
     XIMLangRec *xl;
     XIMNestLangRec *lang;
{
  register XIMNestLangRec *p, **prev;

  for (prev = &xl->lc_list; p = *prev; prev = &p->next)
    {
      if (lang == NULL || !strcmp (lang->lc_name, p->lc_name))
        {
          *prev = p->next;
          Free ((char *) p);
          if (lang)
            break;
        }
    }
}

static int
create_lang_env (xc, num, lang, pre_font, st_font, detail)
     register XIMClientRec *xc;
     int num;
     XIMNestLangList lang;
     char *pre_font, *st_font;
     short *detail;
{
  register XIMLangRec *xl;
  register XIMRootRec *root = xc->root_pointer;
#ifdef X11R5
  XLocale xlc;
#endif /* X11R5 */

  if (!setlocale (LC_ALL, lang->lang_db->lc_name))
    {
      *detail = BadLanguage;
      return (0);
    }

#ifdef  CALLBACKS
  if (IsPreeditCallbacks (xc))
    {
      if (pre_font == NULL || *pre_font == '\0')
        {
          if ((pre_font = get_default_font_name ()) == NULL)
            {
              *detail = BadFontSet;
              return (0);
            }
        }
    }
  if (IsStatusCallbacks (xc))
    {
      if (st_font == NULL || *st_font == '\0')
        {
          if ((st_font = get_default_font_name ()) == NULL)
            {
              *detail = BadFontSet;
              return (0);
            }
        }
    }
#endif /* CALLBACKS */
  if ((xl = xc->xl[num] = (XIMLangRec *) Calloc (1, sizeof (XIMLangRec))) == NULL)
    {
      *detail = AllocError;
      return (-1);
    }
#ifdef X11R5
  if (!(xlc = _XFallBackConvert ()) || !(xl->xlc = _XlcDupLocale (xlc)))
    {
#else
  if (!(xl->xlc = _XlcCurrentLC ()))
    {
#endif /* X11R5 */
      print_out ("Could not create locale environment.");
      Free (xl);
      *detail = AllocError;
      return (-1);
    }
  xl->lc_list = NULL;
  if (add_locale_to_xl (xl, lang) < 0)
    {
      Free (xl);
      *detail = AllocError;
      return (-1);
    }
  xl->lang_db = lang->lang_db;
  xl->w_c = NULL;
#ifdef  USING_XJUTIL
  xl->xjutil_fs_id = -1;
  xl->working_xjutil = 0;
#endif /* USING_XJUTIL */
  if (IsPreeditArea (xc) || IsPreeditPosition (xc) || IsPreeditCallbacks (xc))
    {
      /*
       * Loading multipule fonts
       */
      if (load_font_set (xl, pre_font, st_font) == -1)
        {
          *detail = BadFontSet;
          goto ERROR_OCCUR1;
          return (0);
        }
      /*
       * Creating GC
       */
      if (num == 0)
        {
          if (!(xc->yes_no = (YesOrNo *) Malloc (sizeof (YesOrNo))))
            {
              malloc_error ("allocation of client data");
              goto ERROR_OCCUR2;
            }
          if (create_yes_no (xc) < 0)
            goto ERROR_OCCUR3;
          xgcv.foreground = xc->pe.fg;
          xgcv.background = xc->pe.bg;
          if (!(xc->pe.gc = XCreateGC (dpy, root->root_window, (GCForeground | GCBackground), &xgcv)))
            goto ERROR_OCCUR3;
          xgcv.foreground = xc->pe.bg;
          xgcv.background = xc->pe.fg;
          if (!(xc->pe.reversegc = XCreateGC (dpy, root->root_window, (GCForeground | GCBackground), &xgcv)))
            goto ERROR_OCCUR4;
          xgcv.foreground = xc->pe.bg;
          xgcv.function = GXinvert;
          xgcv.plane_mask = XOR (xc->pe.fg, xc->pe.bg);
          if (!(xc->pe.invertgc = XCreateGC (dpy, root->root_window, (GCForeground | GCFunction | GCPlaneMask), &xgcv)))
            goto ERROR_OCCUR4;
          xgcv.foreground = xc->st.fg;
          xgcv.background = xc->st.bg;
          if (!(xc->st.gc = XCreateGC (dpy, root->root_window, (GCForeground | GCBackground), &xgcv)))
            goto ERROR_OCCUR4;
          xgcv.foreground = xc->st.bg;
          xgcv.background = xc->st.fg;
          if (!(xc->st.reversegc = XCreateGC (dpy, root->root_window, (GCForeground | GCBackground), &xgcv)))
            goto ERROR_OCCUR4;
          if (IsPreeditArea (xc))
            {
              xc->max_columns = ((int) PreeditWidth (xc) / (int) FontWidth (xl));
              if (xc->max_columns <= 0)
                xc->max_columns = 1;
              xc->columns = xc->max_columns;
            }
          else if (IsPreeditPosition (xc))
            {
              xc->max_columns = ((int) xc->client_area.width / (int) FontWidth (xl));
              if (xc->max_columns <= 0)
                xc->max_columns = 1;
              xc->columns = xc->max_columns;
            }
        }
    }
  if (new_client (xc, xl, False) != -1)
    return (1);
  if (num == 0)
    {
    ERROR_OCCUR4:
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
      remove_box (xc->yes_no->title);
      remove_box (xc->yes_no->button[0]);
      remove_box (xc->yes_no->button[1]);
    ERROR_OCCUR3:
      Free ((char *) xc->yes_no);
    }
ERROR_OCCUR2:
  if (xl->pe_fs)
    XFreeFontSet (dpy, xl->pe_fs);
  if (xl->pe_b_char)
    Free ((char *) xl->pe_b_char);
  if (xl->st_fs)
    XFreeFontSet (dpy, xl->st_fs);
  if (xl->st_b_char)
    Free ((char *) xl->st_b_char);
ERROR_OCCUR1:
  remove_locale_to_xl (xl, lang);
#ifdef X11R5
  XFree ((char *) xl->xlc);
#endif /* X11R5 */
  Free ((char *) xl);
  return (0);
}

int
add_lang_env (xc, lang, new)
     register XIMClientRec *xc;
     XIMNestLangRec *lang;
     int *new;
{
  register int i;
  register char *p;
  Status match = False;
  int ret;
  short detail;

  *new = 0;
  for (i = 0; i < xc->lang_num; i++)
    {
      if (lang->lang_db == xc->xl[i]->lang_db)
        {
          match = True;
          break;
        }
    }
  if (match == True)
    {
      if (add_locale_to_xl (xc->xl[i], lang) < 0)
        return (-1);
      return (i);
    }
  if (!(p = Malloc (sizeof (XIMLangRec *) * (xc->lang_num + 1))))
    {
      malloc_error ("allocation of client data");
      return (-2);
    }
  if (xc->lang_num > 0)
    {
      bcopy ((char *) xc->xl, (char *) p, sizeof (XIMLangRec *) * xc->lang_num);
      Free ((char *) xc->xl);
    }
  xc->xl = (XIMLangRec **) p;
  if ((ret = create_lang_env (xc, xc->lang_num, lang, xc->pe.fontset, xc->st.fontset, &detail)) <= 0)
    {
      return (ret - 1);
    }
  *new = 1;
  return (xc->lang_num);
}

void
default_xc_set (xc)
     register XIMClientRec *xc;
{
  register XIMRootRec *root = xc->root_pointer;
  XWindowAttributes attr;

  if (xc->w)
    {
      XGetWindowAttributes (dpy, xc->w, &attr);
    }
  else
    {
      XGetWindowAttributes (dpy, root->root_window, &attr);
    }
  xc->filter_events = MaskNeeded;
  xc->client_area.x = attr.x;
  xc->client_area.y = attr.y;
  xc->client_area.width = attr.width;
  xc->client_area.height = attr.height;
  xc->pe.colormap = attr.colormap;
  xc->pe.std_colormap = 0;
  xc->pe.fg = root->ximclient->pe.fg;
  xc->pe.bg = root->ximclient->pe.bg;
  xc->pe.bg_pixmap = NULL;
  xc->pe.line_space = 0;
  xc->pe.cursor = NULL;
  xc->pe.area_needed.width = xc->pe.area_needed.height = 0;
  xc->st.colormap = attr.colormap;
  xc->st.std_colormap = 0;
  xc->st.fg = root->ximclient->st.fg;
  xc->st.bg = root->ximclient->st.bg;
  xc->st.bg_pixmap = NULL;
  xc->st.line_space = 0;
  xc->st.cursor = NULL;
  xc->st.area_needed.width = xc->st.area_needed.height = 0;
#ifdef  CALLBACKS
  xc->cb_pe_start = xc->cb_st_start = 0;
#endif /* CALLBACKS */
  xc->have_ch_lc = 0;
  xc->ch_lc_flg = 0;
}

XIMClientRec *
create_client (ic_req, pre_req, st_req, lc_list, init_lc, have_world, cur_is_world, pre_font, st_font, c_data, detail)
     ximICValuesReq *ic_req;
     ximICAttributesReq *pre_req, *st_req;
     XIMNestLangList lc_list, init_lc;
     int have_world, cur_is_world;
     char *pre_font, *st_font;
     char *c_data;
     short *detail;
{
  register XIMClientRec *xc;
  register i, num, num_xl = 0, using_l_len = 0;
  register WnnClientRec *save_c_c;
  register XIMClientRec *save_cur_p, *save_cur_x;
  register XIMNestLangList p, xp;
  register XIMLangDataBase *cur_lang_sv;
  register XIMRootRec *root;
  XIMLangRec *cur_xl = NULL;
  XIMNestLangRec *cur_lc = NULL;
  int ret, error = 0;
  int count;
  int screen;
  Status match = False, same = False;
#ifdef  X11R3
  XStandardColormap map_info;
#else /* X11R3 */
  XStandardColormap *map_info;
#endif /* !X11R3 */
  XWindowAttributes attr;

  if ((xc = (XIMClientRec *) Calloc (1, sizeof (XIMClientRec))) == NULL)
    {
      *detail = AllocError;
      return (NULL);
    }
  if ((screen = get_client_screen ()) == -1)
    {
      screen = DefaultScreen (dpy);
    }
  for (xc->root_pointer = NULL, i = 0; i < xim->screen_count; i++)
    {
      if (screen == xim->root_pointer[i]->screen)
        {
          xc->root_pointer = root = xim->root_pointer[i];
          break;
        }
    }
  if (!xc->root_pointer)
    xc->root_pointer = root = xim->root_pointer[xim->default_screen];
  xc->cur_xl = NULL;
  xc->next = NULL;
  xc->using_language = NULL;
  if (!xc->user_name || !*xc->user_name)
    {
      if (root->ximclient->user_name)
        {
          xc->user_name = alloc_and_copy (root->ximclient->user_name);
        }
    }
  else
    {
      xc->user_name = alloc_and_copy (c_data);
    }
  if (!xc->user_name)
    {
      Free (xc);
      *detail = AllocError;
      return (NULL);
    }
  if (!(ic_req->mask & (1 << ICClientWindow)))
    xc->w = RootWindow (dpy, root->screen);
  else
    xc->w = ic_req->c_window;
  xc->mask = ic_req->mask;
  xc->fd = get_cur_sock ();
  xc->focus_window = xc->w;
  xc->have_world = have_world;
  xc->world_on = cur_is_world;
#ifdef  CALLBACKS
  xc->max_keycode = ic_req->max_keycode;
#endif /* CALLBACKS */
  default_xc_set (xc);

  if (xc->w)
    XSelectInput (dpy, xc->w, StructureNotifyMask);

  update_ic (xc, ic_req, pre_req, st_req, pre_font, st_font, &error);
  if (error)
    goto ERROR_RET2;

  if (xc->focus_window != xc->w)
    {
      XSelectInput (dpy, xc->focus_window, StructureNotifyMask);
      XGetWindowAttributes (dpy, xc->focus_window, &attr);
      xc->focus_area.x = attr.x;
      xc->focus_area.y = attr.y;
      xc->focus_area.width = attr.width;
      xc->focus_area.height = attr.height;
    }
  else
    {
      xc->focus_area.x = 0;
      xc->focus_area.y = 0;
      xc->focus_area.x = xc->focus_area.y = 0;
      xc->focus_area.width = xc->focus_area.height = 0;
    }

  if (xc->pe.std_colormap)
    {
#ifdef  X11R3
      if (XGetStandardColormap (dpy, root->root_window, &map_info, xc->pe.std_colormap))
        {
          xc->pe.colormap = map_info.colormap;
#else /* X11R3 */
      if (XGetRGBColormaps (dpy, root->root_window, &map_info, &count, xc->pe.std_colormap))
        {
          xc->pe.colormap = map_info->colormap;
          XFree (map_info);
#endif /* X11R3 */
        }
    }
  if (xc->st.std_colormap)
    {
#ifdef  X11R3
      if (XGetStandardColormap (dpy, root->root_window, &map_info, xc->st.std_colormap))
        {
          xc->st.colormap = map_info.colormap;
#else /* X11R3 */
      if (XGetRGBColormaps (dpy, root->root_window, &map_info, &count, xc->st.std_colormap))
        {
          xc->pe.colormap = map_info->colormap;
          XFree (map_info);
#endif /* X11R3 */
        }
    }

  if (!(IsPreeditNothing (xc)))
    {
      if (ClientWidth (xc) <= 0 || ClientHeight (xc) <= 0)
        {
          goto ERROR_RET2;
        }
      if (pre_font == NULL)
        {
          if (IsPreeditArea (xc) || IsPreeditPosition (xc))
            {
              *detail = BadFontSet;
              goto ERROR_RET2;
            }
        }
      if (st_font == NULL)
        {
          if (IsStatusArea (xc))
            {
              st_font = pre_font;
            }
        }
      for (num = 0, p = lc_list; p != NULL; p = p->next)
        {
          num++;
          using_l_len += strlen (p->lc_name) + 1;
        }
      if ((xc->using_language = (char *) Malloc (sizeof (char) * using_l_len)) == NULL)
        {
          malloc_error ("allocation of client data");
          *detail = AllocError;
          goto ERROR_RET2;
        }
      xc->using_language[0] = '\0';
      if ((xc->xl = (XIMLangRec **) Malloc (sizeof (XIMLangRec *) * num)) == NULL)
        {
          malloc_error ("allocation of client data");
          *detail = AllocError;
          goto ERROR_RET2;
        }
      /*
       * Create lang env.
       */
      save_c_c = c_c;
      save_cur_p = cur_p;
      save_cur_x = cur_x;
      cur_lang_sv = cur_lang;
      cur_p = cur_x = xc;
      for (i = 0, p = lc_list; p != NULL; p = p->next)
        {
          match = False;
          same = False;
          for (i = 0; i < num_xl; i++)
            {
              if (p->lang_db == xc->xl[i]->lang_db)
                {
                  match = True;
                  break;
                }
            }
          if (match == True)
            {
              for (xp = xc->xl[i]->lc_list; xp != NULL; xp = xp->next)
                {
                  if (!strcmp (p->lc_name, xp->lc_name))
                    {
                      same = True;
                      break;
                    }
                }
              if (same == True)
                continue;
              if (add_locale_to_xl (xc->xl[i], p) < 0)
                {
                  for (i = 0; i < num_xl; i++)
                    {
                      destroy_xl (xc, xc->xl[i]);
                    }
                  Free ((char *) xc->xl);
                  goto ERROR_RET1;
                }
              if (init_lc && !strcmp (p->lc_name, init_lc->lc_name))
                {
                  cur_xl = xc->xl[i];
                  cur_lc = cur_xl->lc_list;
                }
            }
          else
            {
              if ((ret = create_lang_env (xc, num_xl, p, pre_font, st_font, detail)) == -1)
                {
                  for (i = 0; i < num_xl; i++)
                    {
                      destroy_xl (xc, xc->xl[i]);
                    }
                  Free ((char *) xc->xl);
                  goto ERROR_RET1;
                }
              else if (ret == 0)
                {
                  continue;
                }
              if (init_lc && !strcmp (p->lc_name, init_lc->lc_name))
                {
                  cur_xl = xc->xl[i];
                  cur_lc = cur_xl->lc_list;
                }
              num_xl++;
            }
          strcat (xc->using_language, p->lc_name);
          strcat (xc->using_language, ";");
        }
      c_c = save_c_c;
      cur_p = save_cur_p;
      cur_x = save_cur_x;
      cur_lang = cur_lang_sv;
      if (c_c)
        {
          cur_rk = c_c->rk;
          cur_rk_table = cur_rk->rk_table;
        }
      if (num_xl == 0)
        {
          Free ((char *) xc->xl);
          *detail = BadLanguage;
          goto ERROR_RET2;
        }
      xc->using_language[strlen (xc->using_language) - 1] = '\0';
      if (cur_xl == NULL)
        {
          cur_xl = xc->xl[0];
          cur_lc = cur_xl->lc_list;
        }
      xc->lang_num = num_xl;
    }
  else
    {
      xc->cur_xl = NULL;
      xc->lang_num = 0;
    }
  add_ximclientlist (xc);
  if (IsPreeditNothing (xc) || xc->lang_num == 0)
    {
      xc->yes_no = NULL;
      return (xc);
    }
  save_c_c = c_c;
  save_cur_p = cur_p;
  save_cur_x = cur_x;
  cur_lang_sv = cur_lang;
  if (IsPreeditPosition (xc))
    {
      if (!(xc->mask & (1 << ICArea)))
        {
          xc->pe.area.x = xc->focus_area.x;
          xc->pe.area.y = xc->focus_area.y;
          xc->pe.area.width = xc->focus_area.width;
          xc->pe.area.height = xc->focus_area.height;
        }
      update_spotlocation (xc, ic_req, pre_req);
    }
  cur_p = cur_x = xc;
  cur_p->cur_xl = cur_xl;
  cur_xl->cur_lc = cur_lc;
  cur_lang = cur_p->cur_xl->lang_db;
  c_c = cur_p->cur_xl->w_c;
  cur_rk = c_c->rk;
  cur_rk_table = cur_rk->rk_table;
  if (IsStatusArea (xc)
#ifdef  CALLBACKS
      || IsStatusCallbacks (xc)
#endif /* CALLBACKS */
    )
    {
#ifdef  nodef
      visual_status ();
#endif /* nodef */
      if (henkan_off_flag == 0)
        {
          disp_mode ();
        }
      else
        {
          display_henkan_off_mode ();
        }
    }

  XSelectInput (dpy, xc->w, StructureNotifyMask);
  if (c_c->use_server && !jl_isconnect (bun_data_))
    {
      if (servername && *servername)
        {
          print_msg_getc (" I can not connect server(at %s)", servername, NULL, NULL);
        }
      else
        {
          print_msg_getc (" I can not connect server", NULL, NULL, NULL);
        }
    }
  c_c = save_c_c;
  cur_p = save_cur_p;
  cur_x = save_cur_x;
  cur_lang = cur_lang_sv;
  if (c_c)
    {
      cur_rk = c_c->rk;
      cur_rk_table = cur_rk->rk_table;
    }
  return (xc);

ERROR_RET1:
  c_c = save_c_c;
  cur_p = save_cur_p;
  cur_x = save_cur_x;
  cur_lang = cur_lang_sv;
  if (c_c)
    {
      cur_rk = c_c->rk;
      cur_rk_table = cur_rk->rk_table;
    }
ERROR_RET2:
  if (xc->using_language)
    Free ((char *) xc->using_language);
  Free ((char *) xc->user_name);
  if (xc->pe.fontset != NULL && *(xc->pe.fontset) != '\0')
    {
      Free (xc->pe.fontset);
    }
  if (xc->st.fontset != NULL && *(xc->st.fontset) != '\0')
    {
      Free (xc->st.fontset);
    }
  Free ((char *) xc);
  return (NULL);
}

static char *lang_title = "LANGUAGE";
int
lang_set (in, p_xl, p_lang)
     int in;
     XIMLangRec **p_xl;
     XIMNestLangRec **p_lang;
{
  static int init;
  static int c, cnt;
  int i;
  static char *buf[MAX_LANGS];
  static WnnClientRec *c_c_sv = 0;
  static lang_step = 0;
  XIMNestLangRec *p;

/*
    if (henkan_off_flag == 1) {
        return(-1);
    }
*/
  if (c_c_sv != 0 && c_c != c_c_sv)
    {
      ring_bell ();
      return (-1);
    }
  if (lang_step == 0)
    {
      if ((cur_p->lang_num == 0) || (cur_p->have_ch_lc == 0))
        return (-1);
      c_c_sv = c_c;
      cnt = 0;
      for (i = 0; i < cur_p->lang_num; i++)
        {
          for (p = cur_p->xl[i]->lc_list; p; p = p->next)
            {
              if (p->alias_name)
                buf[cnt] = p->alias_name;
              else
                buf[cnt] = p->lc_name;
              if (cur_p->cur_xl == cur_p->xl[i] && cur_p->xl[i]->cur_lc == p)
                init = cnt;
              cnt++;
            }
        }
      if (cur_p->have_world)
        {
          buf[cnt] = world_locale;
          if (cur_p->world_on)
            {
              init = cnt;
            }
          cnt++;
        }
      lang_step++;
    }
  if (lang_step == 1)
    {
      c = xw_select_one_element (buf, cnt, init, lang_title, SENTAKU, main_table[4], in);
      if (c == BUFFER_IN_CONT)
        {
          return (BUFFER_IN_CONT);
        }
      if (c == -1 || c == -3)
        {
          c_c_sv = 0;
          lang_step = 0;
          return (-1);
        }
      lang_step++;
    }
  if (lang_step == 2)
    {
      c_c_sv = 0;
      lang_step = 0;
      if (cur_p->have_world)
        {
          if (!strcmp (cur_p->xl[0]->lc_list->lc_name, buf[c]))
            {
              *p_xl = cur_p->xl[0];
              *p_lang = cur_p->xl[0]->lc_list;
              cur_p->ch_lc_flg = 1;
              cur_p->world_on = 1;
              return (c);
            }
          else
            {
              cur_p->world_on = 0;
            }
        }
      for (i = 0; i < cur_p->lang_num; i++)
        {
          for (p = cur_p->xl[i]->lc_list; p; p = p->next)
            {
              if ((p->alias_name && !strcmp (p->alias_name, buf[c])) || !strcmp (p->lc_name, buf[c]))
                {
                  *p_xl = cur_p->xl[i];
                  *p_lang = p;
                  cur_p->ch_lc_flg = 1;
                  return (c);
                }
            }
        }
      return (-1);
    }
  c_c_sv = 0;
  lang_step = 0;
  return (-1);
}

static char *lang_title_ct = "LANGUAGE_CT";
int
lang_set_ct (in, p_xl, p_lang)
     int in;
     XIMLangRec **p_xl;
     XIMNestLangRec **p_lang;
{
  static int init;
  static int c;
  int i;
  static char *buf[MAX_LANGS];
  static WnnClientRec *c_c_sv = 0;
  static lang_step = 0;

  if (c_c_sv != 0 && c_c != c_c_sv)
    {
      ring_bell ();
      return (-1);
    }
  if (lang_step == 0)
    {
      if ((cur_p->lang_num == 0) || (cur_p->world_on == 0))
        return (-1);
      c_c_sv = c_c;
      for (i = 0; i < cur_p->lang_num; i++)
        {
          buf[i] = cur_p->xl[i]->lang_db->lang;
          if (cur_p->cur_xl == cur_p->xl[i])
            init = i;
        }
      lang_step++;
    }
  if (lang_step == 1)
    {
      c = xw_select_one_element (buf, cur_p->lang_num, init, lang_title_ct, SENTAKU, main_table[4], in);
      if (c == BUFFER_IN_CONT)
        {
          return (BUFFER_IN_CONT);
        }
      if (c == -1 || c == -3)
        {
          c_c_sv = 0;
          lang_step = 0;
          return (-1);
        }
      lang_step++;
    }
  if (lang_step == 2)
    {
      c_c_sv = 0;
      lang_step = 0;
      for (i = 0; i < cur_p->lang_num; i++)
        {
          if (!strcmp (cur_p->xl[i]->lang_db->lang, buf[c]))
            {
              *p_xl = cur_p->xl[i];
              *p_lang = cur_p->xl[i]->lc_list;
              return (c);
            }
        }
      return (-1);
    }
  c_c_sv = 0;
  lang_step = 0;
  return (-1);
}

void
change_lang (xl, p)
     XIMLangRec *xl;
     XIMNestLangRec *p;
{
  if (IsPreeditNothing (cur_p))
    return;
  if (cur_p->cur_xl == xl)
    {
      if (cur_p->cur_xl->cur_lc == p)
        return;
      cur_p->cur_xl->cur_lc = p;
    }
  else
    {
      invisual_window1 ();
      invisual_status ();
      cur_p->cur_xl = xl;
      cur_p->cur_xl->cur_lc = p;
      c_c = cur_p->cur_xl->w_c;
      cur_lang = cur_p->cur_xl->lang_db;
      cur_rk = c_c->rk;
      cur_rk_table = cur_rk->rk_table;
      visual_status ();
      if (henkan_off_flag == 0)
        {
          disp_mode ();
        }
      else
        {
          display_henkan_off_mode ();
        }
      if (!ifempty ())
        {
          visual_window ();
        }
      cur_input = NULL;
    }
}
