/*
 * $Id: init_w.c,v 1.2 2001/06/14 18:16:12 ura Exp $
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
 *      '99/04/19       TAOKA Satoshi - 田岡 智志<taoka@infonets.hiroshima-u.ac.jp>
 *              index() の宣言をコメントアウト。
 *
 * Last modified date: 19,Apr.1999
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
#include "config.h"
#include "xext.h"
#include "rk_fundecl.h"

#define XOR(a,b)        ((a&(~b)) | ((~a)&b))

#define MaskOfButton (ButtonPressMask | ButtonReleaseMask | EnterWindowMask | \
                      LeaveWindowMask | ExposureMask)

Display *dpy = 0;
Xjutil *xjutil;

char xjutil_name[32] = { 0 };
char xjutil_name_ok[32] = { 0 };
char xjutil_env_name[32] = { 0 };
int counter = 0;
Cursor cursor1, cursor2, cursor3;

Window window_xim;
Atom atom_xim;
XSetWindowAttributes attributes;

Atom wm_id = 0;
Atom wm_id1 = 0;

Atom actual_type;
int actual_format;
unsigned long nitems, byte_after, leftover;

wchar_t dol_wchar_t;

char
env_state ()
{
  char *p;
  char ret = '\0';
#ifndef SYSVR2
/*    extern char *index(); */
#else
  extern char *strchr ();
#endif

  if ((p = romkan_dispmode ()) == NULL)
    return (ret);
#ifndef SYSVR2
  if ((p = (char *) index (p, ':')) == NULL)
    return (ret);
#else
  if ((p = (char *) strchr (p, ':')) == NULL)
    return (ret);
#endif
  return (*(p + 1));
}

int
set_cur_env (s)
     char s;
{
  register WnnEnv *p;
  register int i;

  for (p = normal_env; p; p = p->next)
    {
      for (i = 0; p->env_name_str[i]; i++)
        {
          if (s == p->env_name_str[i])
            {
              cur_normal_env = p;
              return (0);
            }
        }
    }
  return (0);
}

void
get_new_env (rev)
     int rev;
{
  WnnEnv *new, *p, *prev = NULL;

  new = (WnnEnv *) Malloc (sizeof (WnnEnv));
  new->host_name = NULL;
  new->env = NULL;
  new->sticky = 0;
  new->envrc_name = NULL;
  new->env_name_str[0] = '\0';
  new->next = NULL;
  if (rev)
    {
      cur_reverse_env = new;
      if (reverse_env == NULL)
        {
          reverse_env = new;
          return;
        }
      p = reverse_env;
    }
  else
    {
      cur_normal_env = new;
      if (normal_env == NULL)
        {
          normal_env = new;
          return;
        }
      p = normal_env;
    }
  for (; p; prev = p, p = p->next);
  if (prev)
    prev->next = new;
}

void
send_end_work ()
{
  XEvent event;

  event.type = ClientMessage;
  event.xclient.format = 32;
  event.xclient.window = xjutil->root_pointer[xjutil->default_screen]->jutil->w;
  event.xclient.data.l[0] = DIC_END;
  event.xclient.data.l[1] = counter;
  XSendEvent (dpy, window_xim, False, NoEventMask, &event);
  XFlush (dpy);
}

static void
send_ok ()
{
  XEvent event;
  Atom select_id;
  Window window = xjutil->root_pointer[xjutil->default_screen]->jutil->w;

  if (!(select_id = XInternAtom (dpy, xjutil_name_ok, True)))
    {
      select_id = XInternAtom (dpy, xjutil_name_ok, False);
    }
  XSetSelectionOwner (dpy, select_id, window, 0L);
  event.type = ClientMessage;
  event.xclient.format = 32;
  event.xclient.window = window;
  event.xclient.data.l[0] = DIC_START;
  event.xclient.data.l[1] = window;
  event.xclient.data.l[2] = counter;
  XSendEvent (dpy, window_xim, False, NoEventMask, &event);
  XFlush (dpy);
}

void
return_error ()
{
  XEvent event;
  Atom select_id;
  Window window = xjutil->root_pointer[xjutil->default_screen]->jutil->w;

  if (dpy == NULL)
    return;
  if (!(select_id = XInternAtom (dpy, xjutil_name_ok, True)))
    {
      select_id = XInternAtom (dpy, xjutil_name_ok, False);
    }
  XSetSelectionOwner (dpy, select_id, window, 0L);
  event.type = ClientMessage;
  event.xclient.format = 32;
  event.xclient.window = window;
  event.xclient.data.l[0] = DIC_START_ER;
  event.xclient.data.l[1] = window;
  event.xclient.data.l[2] = counter;
  XSendEvent (dpy, window_xim, False, NoEventMask, &event);
  XFlush (dpy);
}

void
read_wm_id ()
{
  register XIMRootRec *root;
  register int i;

  if ((wm_id = XInternAtom (dpy, "WM_PROTOCOLS", True)))
    {
      wm_id1 = XInternAtom (dpy, "WM_DELETE_WINDOW", True);
      for (i = 0; i < xjutil->screen_count; i++)
        {
          root = xjutil->root_pointer[i];
          XChangeProperty (dpy, root->jutil->w, wm_id, XA_INTEGER, 32, PropModeReplace, (unsigned char *) &wm_id1, sizeof (Atom));
          XChangeProperty (dpy, root->ichi->w, wm_id, XA_INTEGER, 32, PropModeReplace, (unsigned char *) &wm_id1, sizeof (Atom));
          XChangeProperty (dpy, root->yes_no->w, wm_id, XA_INTEGER, 32, PropModeReplace, (unsigned char *) &wm_id1, sizeof (Atom));
        }
    }
  else
    {
      wm_id = wm_id1 = 0;
    }
}

static char *
get_property ()
{
  Xjutil_startRec *xjutil_start;
  unsigned char *data, *s_data;
  char *p, *s_p;
  char nlspath[64];
  int all_size = 0, uumkey_name_size, cvtkey_name_size, rkfile_name_size, cvtfun_name_size, cvtmeta_name_size, user_name_size, lang_size, lc_name_size, fn_size = 0;
  int i;
  unsigned char tmp_buf[256];
  FunctionTable *db;
  XIMRootRec *root;

  if ((atom_xim = XInternAtom (dpy, xjutil_name, True)) == (Atom) NULL)
    {
      return (NULL);
    }
  window_xim = XGetSelectionOwner (dpy, atom_xim);
  XSelectInput (dpy, window_xim, StructureNotifyMask);
  xjutil_start = (Xjutil_startRec *) Malloc (sizeof (Xjutil_startRec));
  XGetWindowProperty (dpy, RootWindow (dpy, xjutil->default_screen), atom_xim, 0L, 1000000L, False, XA_STRING, &actual_type, &actual_format, &nitems, &leftover, &data);

  bcopy (data, xjutil_start, sizeof (Xjutil_startRec));
  if (xjutil_start->fn_len > 0)
    {
      all_size = fn_size = xjutil_start->fn_len + 1;
    }
  all_size += uumkey_name_size = (strlen (xjutil_start->uumkey_name) + 1);
  all_size += rkfile_name_size = (strlen (xjutil_start->rkfile_name) + 1);
  all_size += cvtkey_name_size = (strlen (xjutil_start->cvtkey_name) + 1);
  all_size += cvtfun_name_size = (strlen (xjutil_start->cvtfun_name) + 1);
  all_size += cvtmeta_name_size = (strlen (xjutil_start->cvtmeta_name) + 1);
  all_size += user_name_size = (strlen (xjutil_start->user_name) + 1);
  all_size += lang_size = (strlen (xjutil_start->lang) + 1);
  all_size += lc_name_size = (strlen (xjutil_start->lc_name) + 1);
  if ((p = (char *) Malloc (all_size)) == NULL)
    {
      malloc_error ("allocation of initial area");
      Free (xjutil_start);
      XFree (data);
      return (NULL);
    }
  s_data = data;
  data += sizeof (Xjutil_startRec);
  for (i = 0; i < xjutil->screen_count; i++)
    {
      root = xjutil->root_pointer[i];
      bcopy (data, (char *) &root->fg, sizeof (unsigned long));
      data += sizeof (unsigned long);
      bcopy (data, (char *) &root->bg, sizeof (unsigned long));
      data += sizeof (unsigned long);
      bcopy (data, (char *) &root->bw, sizeof (unsigned int));
      data += sizeof (unsigned int);
    }
  for (i = 0; (int) i < (int) xjutil_start->max_env; i++)
    {
      get_new_env (0);
      tmp_buf[0] = '\0';
      strcpy ((char *) tmp_buf, (char *) data);
      cur_normal_env->host_name = alloc_and_copy (tmp_buf);
      data += 256;
      tmp_buf[0] = '\0';
      strcpy ((char *) tmp_buf, (char *) data);
      cur_normal_env->envrc_name = alloc_and_copy (tmp_buf);
      data += 256;
      tmp_buf[0] = '\0';
      strcpy ((char *) tmp_buf, (char *) data);
      strcpy ((char *) cur_normal_env->env_name_str, (char *) tmp_buf);
      data += 32;
    }
  for (i = 0; (int) i < (int) xjutil_start->max_reverse_env; i++)
    {
      get_new_env (1);
      tmp_buf[0] = '\0';
      strcpy ((char *) tmp_buf, (char *) data);
      cur_reverse_env->host_name = alloc_and_copy (tmp_buf);
      data += 256;
      tmp_buf[0] = '\0';
      strcpy ((char *) tmp_buf, (char *) data);
      cur_reverse_env->envrc_name = alloc_and_copy (tmp_buf);
      data += 256;
      tmp_buf[0] = '\0';
      strcpy ((char *) tmp_buf, (char *) data);
      strcpy ((char *) cur_reverse_env->env_name_str, (char *) tmp_buf);
      data += 32;
    }
  strncpy (p, (char *) data, fn_size);
  p[fn_size] = '\0';
  XFree (s_data);
  s_p = p;

  uumkey_name = p += fn_size;
  strcpy (p, xjutil_start->uumkey_name);
  rkfile_name = p += uumkey_name_size;
  strcpy (p, xjutil_start->rkfile_name);
  cvt_key_file = p += rkfile_name_size;
  strcpy (p, xjutil_start->cvtkey_name);
  cvt_fun_file = p += cvtkey_name_size;
  strcpy (p, xjutil_start->cvtfun_name);
  cvt_meta_file = p += cvtfun_name_size;
  strcpy (p, xjutil_start->cvtmeta_name);
  username = p += cvtmeta_name_size;
  strcpy (p, xjutil_start->user_name);
  xjutil->lang = p += user_name_size;
  strcpy (p, xjutil_start->lang);
  xjutil->lc_name = p += lang_size;
  strcpy (p, xjutil_start->lc_name);

  for (db = function_db; db->name != NULL; db++)
    {
      if (!strcmp (db->name, xjutil_start->f_name))
        {
          f_table = db;
          break;
        }
    }
  strcpy (jishopath, xjutil_start->jishopath);
  strcpy (hindopath, xjutil_start->hindopath);
  strcpy (fuzokugopath, xjutil_start->fuzokugopath);
  rubout_code = xjutil_start->rubout_code;
  xjutil->cswidth_id = xjutil_start->cswidth_id;
  xjutil->save_under = (char) xjutil_start->save_under;

  strcpy (nlspath, LIBDIR);
  strcat (nlspath, "/%L/%N");
  cd = msg_open ("xjutil.msg", nlspath, xjutil->lang);
  Free (xjutil_start);
  return (s_p);
}

static void
clear_text (t)
     JutilTextRec *t;
{
  t->vst = 0;
  t->note[0] = t->note[1] = 0;
  t->currentcol = 0;
  t->max_pos = 0;
  t->cursor_flag = 0;
  t->mark_flag = 0;
  t->u_line_flag = t->r_flag = t->b_flag = 0;
}

int
create_text (text, root)
     register JutilTextRec *text;
     XIMRootRec *root;
{
  int border_width;
  register int i;

  if ((text->buf = (wchar *) Malloc (sizeof (wchar) * (maxchg * 2 + 1))) == NULL)
    {
      malloc_error ("allocation of client's area");
      return (-1);
    }
  if ((text->att = (unsigned char *) Malloc (sizeof (unsigned char) * (maxchg * 2 + 1))) == NULL)
    {
      malloc_error ("allocation of client's area");
      return (-1);
    }
  for (i = (maxchg * 2); i >= 0; i--)
    {
      *(text->buf + i) = 0;
      *(text->att + i) = '\0';
    }
  clear_text (text);

  border_width = 0;
  text->maxcolumns = maxchg * 2;

  text->wn[0] = XCreateSimpleWindow (dpy, text->wp, 0, 0, FontWidth, FontHeight, border_width, root->fg, root->bg);

  text->wn[1] = XCreateSimpleWindow (dpy, text->wp, text->width - FontWidth, 0, FontWidth, FontHeight, border_width, root->fg, root->bg);

  text->wn[2] = XCreateSimpleWindow (dpy, text->wp, text->width - FontWidth, 0, FontWidth, FontHeight, border_width, root->fg, root->bg);

  text->w = XCreateSimpleWindow (dpy, text->wp, 0, 0, FontWidth * (text->maxcolumns + 1), FontHeight, border_width, root->fg, root->bg);

  text->vst = 0;
  text->note[0] = 0;
  text->note[1] = 0;

  XSelectInput (dpy, text->w, ExposureMask);
  XSelectInput (dpy, text->wn[0], ExposureMask);
  XSelectInput (dpy, text->wn[1], ExposureMask);
  return (0);
}

int
create_jutil (root)
     XIMRootRec *root;
{
  JutilRec *jutil = root->jutil;
  unsigned int width, height;
  int i;
  extern int jutil_mode_set ();

  width = FontWidth;
  height = FontHeight;

  jutil->w = XCreateSimpleWindow (dpy, root->root_window, 0, 0, FontWidth, FontHeight, root->bw, root->fg, root->bg);
  XSetTransientForHint (dpy, jutil->w, root->root_window);
  if (xjutil->save_under)
    {
      attributes.save_under = True;
      XChangeWindowAttributes (dpy, jutil->w, CWSaveUnder, &attributes);
    }
  XSelectInput (dpy, jutil->w, KeyPressMask | StructureNotifyMask);
  XDefineCursor (dpy, jutil->w, cursor3);

  jutil->title = create_box (jutil->w, 0, 0, FontWidth, FontHeight, 1, root->fg, root->bg, root->fg, NULL, (char) 0);
  jutil->rk_mode = create_box (jutil->w, 0, 0, FontWidth, FontHeight, 1, root->fg, root->bg, root->fg, NULL, (char) 0);
  map_box (jutil->title);
  map_box (jutil->rk_mode);

  jutil->max_button = MAX_JU_BUTTON;
  for (i = 0; i < MAX_JU_BUTTON; i++)
    {
      jutil->button[i] = create_box (jutil->w, 0, 0, FontWidth, FontHeight, 2, root->fg, root->bg, root->fg, cursor2, (char) 1);
      map_box (jutil->button[i]);
    }
  jutil->button[CANCEL_W]->string = (unsigned char *) msg_get (cd, 2, default_message[2], xjutil->lang);
  jutil->button[CANCEL_W]->sel_ret = -2;
  jutil->button[EXEC_W]->sel_ret = 0;
  jutil->button[CANCEL_W]->do_ret = jutil->button[EXEC_W]->do_ret = True;

  for (i = 0; i < JCLIENTS; i++)
    {
      if ((jutil->mes_text[i] = (JutilTextRec *) Malloc (sizeof (JutilTextRec))) == NULL)
        {
          malloc_error ("allocation of client's area");
          return (-1);
        }
      jutil->mes_button[i] = create_box (jutil->w, 0, 0, FontWidth, FontHeight, 2, root->fg, root->bg, root->bg, cursor2, (char) 0);
      jutil->mes_mode_return[i] = i;
      jutil->mes_button[i]->sel_ret = 1;
      jutil->mes_button[i]->do_ret = True;
      jutil->mes_button[i]->cb = jutil_mode_set;
      jutil->mes_button[i]->cb_data = (int *) i;

      jutil->mes_text[i]->x = 0;
      jutil->mes_text[i]->y = 0;
      jutil->mes_text[i]->width = FontWidth * TEXT_WIDTH;
      jutil->mes_text[i]->height = FontHeight;
      jutil->mes_text[i]->max_columns = TEXT_WIDTH;
      jutil->mes_text[i]->bp = 0;
      jutil->mes_text[i]->wp = XCreateSimpleWindow (dpy, jutil->w, 0, 0, jutil->mes_text[i]->width, FontHeight, 0, root->fg, root->bg);
      if (create_text (jutil->mes_text[i], root))
        {
          return (-1);
        }
      XFlush (dpy);
    }
  set_wm_properties (jutil->w, 0, 0, width, height, CenterGravity, "jutil", "Jutil", True, 0, InputHint, 0, 0);
  return (0);
}

int
create_ichi (root)
     XIMRootRec *root;
{
  Ichiran *ichi = root->ichi;
  Keytable *kt;
  unsigned int width, height;
  int i;
  extern int next_ichi (), back_ichi ();

  if ((ichi->keytable = (Keytable *) Malloc (sizeof (Keytable))) == NULL)
    {
      malloc_error ("allocation of the initial area");
      return (-1);
    }
  kt = ichi->keytable;

  width = 1;
  height = 1;

  ichi->save_buf = NULL;
  ichi->max_columns = MAX_ICHI_COLUMNS;
  if (alloc_for_save_buf (ichi, MAX_ICHI_LINES) < 0)
    return (-1);
  ichi->max_button = MAX_BUTTON;
  ichi->select_mode = '\0';

  ichi->w = XCreateSimpleWindow (dpy, root->root_window, 0, 0, width, height, root->bw, root->fg, root->bg);
  ichi->w0 = XCreateSimpleWindow (dpy, ichi->w, 0, FontWidth * 4, width, height, 1, root->fg, root->bg);
  ichi->w1 = XCreateSimpleWindow (dpy, ichi->w0, 0, FontWidth * 4, width, height, 0, root->fg, root->bg);
  ichi->nyuu_w = XCreateSimpleWindow (dpy, ichi->w, 0, 0, FontWidth * 5, FontHeight, 1, root->fg, root->bg);

  XSetTransientForHint (dpy, ichi->w, root->root_window);
  if (xjutil->save_under)
    {
      attributes.save_under = True;
      XChangeWindowAttributes (dpy, ichi->w, CWSaveUnder, &attributes);
    }
  XSelectInput (dpy, ichi->w, KeyPressMask | StructureNotifyMask);
  XSelectInput (dpy, ichi->w1, ExposureMask);
  XSelectInput (dpy, ichi->nyuu_w, ExposureMask);

  ichi->comment = create_box (ichi->w, 0, FontHeight * 4, FontWidth, FontHeight, 1, root->fg, root->bg, root->fg, NULL, (char) 0);
  ichi->title = create_box (ichi->w, 0, 0, FontWidth, FontHeight, 1, root->fg, root->bg, root->fg, NULL, (char) 0);
  ichi->subtitle = create_box (ichi->w, 0, 0, FontWidth, FontHeight, 0, root->fg, root->bg, root->fg, NULL, (char) 0);
  for (i = 0; i < MAX_BUTTON; i++)
    {
      ichi->button[i] = create_box (ichi->w, 0, 0, FontWidth, FontHeight, 2, root->fg, root->bg, root->fg, cursor2, (char) 1);

    }
  ichi->button[CANCEL_W]->string = (unsigned char *) msg_get (cd, 2, default_message[2], xjutil->lang);
  ichi->button[NEXT_W]->string = (unsigned char *) msg_get (cd, 3, default_message[3], xjutil->lang);
  ichi->button[BACK_W]->string = (unsigned char *) msg_get (cd, 4, default_message[4], xjutil->lang);

  ichi->button[CANCEL_W]->do_ret = ichi->button[EXEC_W]->do_ret = True;
  ichi->button[CANCEL_W]->sel_ret = -2;
  ichi->button[EXEC_W]->sel_ret = 1;
  ichi->button[MOUSE_W]->cb = xw_mouse_select;
  ichi->button[NEXT_W]->cb = next_ichi;
  ichi->button[BACK_W]->cb = back_ichi;

  map_box (ichi->button[CANCEL_W]);
  map_box (ichi->title);

  kt->w = XCreateSimpleWindow (dpy, root->root_window, 0, 0, FontWidth, FontHeight, root->bw, root->fg, root->bg);
  XSetTransientForHint (dpy, kt->w, root->root_window);
  if (xjutil->save_under)
    {
      attributes.save_under = True;
      XChangeWindowAttributes (dpy, kt->w, CWSaveUnder, &attributes);
    }
  XSelectInput (dpy, kt->w, KeyPressMask | StructureNotifyMask);
  XDefineCursor (dpy, kt->w, cursor3);

  kt->max_button = MAX_KEY_BUTTON;
  for (i = 0; i < MAX_KEY_BUTTON; i++)
    {
      kt->button[i] = create_box (kt->w, 0, 0, FontWidth, FontHeight, 2, root->fg, root->bg, root->fg, cursor2, (char) 1);
      kt->button[i]->sel_ret = i;
      kt->button[i]->do_ret = True;
    }

  XDefineCursor (dpy, ichi->w, cursor3);
  XMapWindow (dpy, ichi->w1);

  set_wm_properties (ichi->w, 0, 0, width, height, CenterGravity, "ichiran", "Ichiran", True, 0, InputHint, 0, 0);
  return (0);
}

int
create_yes_no (root)
     XIMRootRec *root;
{
  YesOrNo *yes_no = root->yes_no;
  unsigned int width, height;

  width = 1;
  height = 1;

  yes_no->w = XCreateSimpleWindow (dpy, root->root_window, 0, 0, width, height, root->bw, root->fg, root->bg);
  yes_no->title = create_box (yes_no->w, 0, 0, width, height, 0, root->fg, root->bg, root->fg, NULL, (char) 0);
  yes_no->button[0] = create_box (yes_no->w, 0, 0, width, height, 2, root->fg, root->bg, root->fg, cursor2, (char) 1);
  yes_no->button[1] = create_box (yes_no->w, 0, 0, width, height, 2, root->fg, root->bg, root->fg, cursor2, (char) 1);
  yes_no->button[0]->sel_ret = 1;
  yes_no->button[1]->sel_ret = 0;
  yes_no->button[0]->do_ret = yes_no->button[1]->do_ret = True;

  XSetTransientForHint (dpy, yes_no->w, root->root_window);
  if (xjutil->save_under)
    {
      attributes.save_under = True;
      XChangeWindowAttributes (dpy, yes_no->w, CWSaveUnder, &attributes);
    }
  XDefineCursor (dpy, yes_no->w, cursor3);
  XSelectInput (dpy, yes_no->w, (StructureNotifyMask | KeyPressMask));
  map_box (yes_no->title);
  XFlush (dpy);
  set_wm_properties (yes_no->w, 0, 0, width, height, CenterGravity, "yesNo", "YesNo", True, 0, InputHint, 0, 0);
  return (0);
}

void
xw_end ()
{
  if (dpy)
    {
      XCloseDisplay (dpy);
    }
}

static int
xerror (d, ev)
     Display *d;
     register XErrorEvent *ev;
{
  return (0);
}

static int
xioerror (d)
     Display *d;
{
  perror ("xjutil");
  disconnect_server ();
  XCloseDisplay (dpy);
  exit (2);
  return (0);
}


XjutilFSRec *
add_fontset_list (fs, act)
     XFontSet fs;
     int act;
{
  register XjutilFSRec *rec;
  XCharStruct *cs;
  extern XCharStruct *get_base_char ();

  if ((rec = (XjutilFSRec *) Malloc (sizeof (XjutilFSRec))) == NULL)
    {
      return (NULL);
    }
  if ((cs = get_base_char (fs)) == NULL)
    {
      return (NULL);
    }
  rec->fs = fs;
  rec->act = act;
  rec->b_char = cs;
  rec->next = font_set_list;
  font_set_list = rec;
  return (rec);
}

XjutilFSRec *
get_fontset_list (act)
     int act;
{
  register XjutilFSRec *p;

  for (p = font_set_list; p; p = p->next)
    {
      if (p->act == act)
        return (p);
    }
  return (NULL);
}

static int
create_one_root (root, screen)
     register XIMRootRec *root;
     int screen;
{
  register Ichiran *ichi;
  register YesOrNo *yes_no;
  register JutilRec *jutil;
  char *p;
  int size_of_jutil;
  int size_of_ichiran;
  int size_of_yes_no;
  int all_size;
  XGCValues xgcv;

  size_of_jutil = sizeof (JutilRec);
  size_of_ichiran = sizeof (Ichiran);
  size_of_yes_no = sizeof (YesOrNo);
  all_size = size_of_jutil + size_of_ichiran + size_of_yes_no;
  if ((p = (char *) Malloc (all_size)) == NULL)
    {
      malloc_error ("allocation of initial area");
      return (-1);
    }
  jutil = (JutilRec *) p;
  p += size_of_jutil;
  ichi = (Ichiran *) p;
  p += size_of_ichiran;
  yes_no = (YesOrNo *) p;

  root->root_window = RootWindow (dpy, screen);
  root->screen = screen;
  root->jutil = jutil;
  root->ichi = ichi;
  root->yes_no = yes_no;

  xgcv.foreground = root->fg;
  xgcv.background = root->bg;
  root->gc = XCreateGC (dpy, root->root_window, (GCForeground | GCBackground), &xgcv);
  xgcv.foreground = root->bg;
  xgcv.background = root->fg;
  root->reversegc = XCreateGC (dpy, root->root_window, (GCForeground | GCBackground), &xgcv);
  xgcv.foreground = root->fg;
  xgcv.function = GXinvert;
  xgcv.plane_mask = XOR (root->fg, root->bg);
  root->invertgc = XCreateGC (dpy, root->root_window, (GCForeground | GCFunction | GCPlaneMask), &xgcv);

  if (create_ichi (root) == -1)
    return (-1);
  if (create_jutil (root) == -1)
    return (-1);
  if (create_yes_no (root) == -1)
    return (-1);
  return (0);
}

int
create_xjutil ()
{
  extern int xerror (), xioerror ();
  extern char *getenv ();
  register int i;
  int screen_count;
  int ret;
  char *p;
  int size_of_xjutil;
  int size_of_root_pointer;
  int size_of_root;
  int all_size;
#ifdef X11R5
  XLocale xlc;
#else
  XLCd xlc;
#endif /* X11R5 */
  char **miss, *def;
  int miss_cnt;
  XFontSet fs;
  extern XjutilFSRec *add_fontset_list ();

#ifdef  X_WCHAR
  dol_wchar_t = 0x20202024;
#else /* X_WCHAR */
  mbtowc (&dol_wchar_t, "$", 1);
#endif /* X_WCHAR */
  if ((dpy = XOpenDisplay (display_name)) == NULL)
    {
      print_out1 ("I could not open Display : %s.", display_name);
      return (-1);
    }
  screen_count = ScreenCount (dpy);
  XSetErrorHandler (xerror);
  XSetIOErrorHandler (xioerror);

  if ((atom_xim = XInternAtom (dpy, xjutil_name, True)) == (Atom) NULL)
    {
      print_out ("I could not find InputManager.");
      return (-1);
    }
  if ((window_xim = XGetSelectionOwner (dpy, atom_xim)) == 0)
    {
      print_out ("I could not find InputManager.");
      return (-1);
    }
  XSelectInput (dpy, window_xim, StructureNotifyMask);
  strcpy (xjutil_name_ok, xjutil_name);
  strcat (xjutil_name_ok, "_OK");
  if (ret = XInternAtom (dpy, xjutil_name_ok, True))
    {
      if (XGetSelectionOwner (dpy, ret))
        {
          print_out ("I am already running.");
          return (-1);
        }
    }
  size_of_xjutil = sizeof (Xjutil);
  size_of_root_pointer = sizeof (XIMRootRec *) * screen_count;
  size_of_root = sizeof (XIMRootRec);
  all_size = size_of_xjutil + size_of_root_pointer + size_of_root * screen_count;
  if ((p = (char *) Malloc (all_size)) == NULL)
    {
      malloc_error ("allocation of initial area");
      return (-1);
    }
  xjutil = (Xjutil *) p;
  p += size_of_xjutil;
  xjutil->root_pointer = (XIMRootRec **) p;
  p += size_of_root_pointer;
  for (i = 0; i < screen_count; i++)
    {
      xjutil->root_pointer[i] = (XIMRootRec *) p;
      p += size_of_root;
    }
  xjutil->screen_count = screen_count;
  xjutil->default_screen = DefaultScreen (dpy);
  xjutil->sel_ret = -1;
  xjutil->sel_button = 0;

  cursor1 = XCreateFontCursor (dpy, 52);
  cursor2 = XCreateFontCursor (dpy, 60);
  cursor3 = XCreateFontCursor (dpy, 24);

  p = get_property ();

  strcpy (xjutil_env_name, XJUTIL_ENV_PRO);
  strcat (xjutil_env_name, xjutil->lang);

  set_cswidth (xjutil->cswidth_id);
  setlocale (LC_ALL, xjutil->lc_name);
#ifdef X11R5
  xlc = _XFallBackConvert ();
  xjutil->xlc = _XlcDupLocale (xlc);
#else
  xlc = _XlcCurrentLC ();
  xjutil->xlc = xlc;
#endif /* X11R5 */

  if (((fs = XCreateFontSet (dpy, p, &miss, &miss_cnt, &def)) == NULL))
    {
      return (-1);
    }
  cur_fs = add_fontset_list (fs, 0);

  for (i = 0; i < screen_count; i++)
    {
      if (create_one_root (xjutil->root_pointer[i], i) == -1)
        return (-1);
    }
  send_ok ();
  read_wm_id ();
  XFlush (dpy);
  if (f_table == NULL)
    return (-1);
  return (0);
}
