/*
 * $Id: do_xjutil.c,v 1.2 2001/06/14 18:16:15 ura Exp $
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
#ifdef  USING_XJUTIL

#include <stdio.h>
#include <sys/wait.h>
#include <signal.h>
#include <X11/Xos.h>
#include "config.h"
#include "commonhd.h"
#include "sdefine.h"
#include "xim.h"
#include "sheader.h"
#include "proto.h"
#include "ext.h"

#define ROOT_WINDOW     (xim->root_pointer[xim->default_screen]->root_window)
static int seq_xjutil_fs_id = 0;

static XIMLangDataBase *
set_xjutil (prop_name)
     char *prop_name;
{
  register int i, nbytes;
  register unsigned char *p;
  register Xjutil_startRec *xj;
  XFontStruct **font_struct_list;
  char **font_name_list;
  int num;
  Atom id;
  register XIMLangRec *xl = cur_p->cur_xl;
  Xjutil_startRec xjutil_start;
  XIMLangDataBase *ld;
  WnnEnv *we;
  unsigned char tmp_buf[256];

  if (!(ld = xl->lang_db) || (ld->xjutil_pid))
    return (NULL);

  xj = &xjutil_start;
  if (uumkeyname && *uumkeyname)
    strcpy (xj->uumkey_name, uumkeyname);
  else
    xj->uumkey_name[0] = '\0';
  if (ld->rkfile_name && *ld->rkfile_name)
    strcpy (xj->rkfile_name, ld->rkfile_name);
  else
    xj->rkfile_name[0] = '\0';
  if (cvt_key_file && *cvt_key_file)
    strcpy (xj->cvtkey_name, cvt_key_file);
  else
    xj->cvtfun_name[0] = '\0';
  if (cvt_fun_file && *cvt_fun_file)
    strcpy (xj->cvtfun_name, cvt_fun_file);
  else
    xj->cvtfun_name[0] = '\0';
  if (cvt_meta_file && *cvt_meta_file)
    strcpy (xj->cvtmeta_name, cvt_meta_file);
  else
    xj->cvtmeta_name[0] = '\0';
  if (username && *username)
    strcpy (xj->user_name, username);
  else
    xj->user_name[0] = '\0';
  if (ld->lang && *ld->lang)
    strcpy (xj->lang, ld->lang);
  else
    xj->lang[0] = '\0';
  if (ld->lc_name && *ld->lc_name)
    strcpy (xj->lc_name, ld->lc_name);
  else
    xj->lc_name[0] = '\0';
  strcpy (xj->f_name, ld->f_table->name);
  if (ld->jishopath && *ld->jishopath)
    strcpy (xj->jishopath, ld->jishopath);
  else
    xj->jishopath[0] = '\0';
  if (ld->hindopath && *ld->hindopath)
    strcpy (xj->hindopath, ld->hindopath);
  else
    xj->hindopath[0] = '\0';
  if (ld->fuzokugopath && *ld->fuzokugopath)
    strcpy (xj->fuzokugopath, ld->fuzokugopath);
  else
    xj->fuzokugopath[0] = '\0';
  nbytes = 0;
  num = XFontsOfFontSet (xl->pe_fs, &font_struct_list, &font_name_list);
  for (i = 0; i < num; i++)
    {
      nbytes += (strlen (font_name_list[i]) + 1);
    }
  if (!(p = (unsigned char *) Malloc ((unsigned) (nbytes + 1))))
    {
      malloc_error ("allocation of work area for xjutil");
      return (NULL);
    }
  p[0] = '\0';
  for (i = 0; i < num; i++)
    {
      strcat ((char *) p, font_name_list[i]);
      strcat ((char *) p, ",");
    }
  p[nbytes - 1] = '\0';
  xj->fn_len = nbytes - 1;
  xj->rubout_code = rubout_code;
  xj->cswidth_id = ld->cswidth_id;

  if (xim->save_under)
    {
      xj->save_under = 1;
    }
  else
    {
      xj->save_under = 0;
    }

  for (i = 0, we = c_c->normal_env; we; we = we->next, i++);
  xj->max_env = i;
  for (i = 0, we = c_c->reverse_env; we; we = we->next, i++);
  xj->max_reverse_env = i;

  strcpy (prop_name, XJUTIL_START_PRO);
  strcat (prop_name, ld->lang);
  id = XInternAtom (dpy, prop_name, False);
  XChangeProperty (dpy, ROOT_WINDOW, id, XA_STRING, 8, PropModeReplace, (unsigned char *) xj, sizeof (Xjutil_startRec));

  for (i = 0; i < xim->screen_count; i++)
    {
      XChangeProperty (dpy, ROOT_WINDOW, id, XA_STRING, 8, PropModeAppend, (unsigned char *) &cur_p->pe.fg, sizeof (unsigned long));
      XChangeProperty (dpy, ROOT_WINDOW, id, XA_STRING, 8, PropModeAppend, (unsigned char *) &cur_p->pe.bg, sizeof (unsigned long));
      XChangeProperty (dpy, ROOT_WINDOW, id, XA_STRING, 8, PropModeAppend, (unsigned char *) &cur_p->root_pointer->bw, sizeof (unsigned int));
    }
  for (i = 0, we = c_c->normal_env; we; we = we->next, i++)
    {
      tmp_buf[0] = '\0';
      if (we->host_name)
        strcpy ((char *) tmp_buf, we->host_name);
      XChangeProperty (dpy, ROOT_WINDOW, id, XA_STRING, 8, PropModeAppend, tmp_buf, 256);
      tmp_buf[0] = '\0';
      if (we->envrc_name)
        strcpy ((char *) tmp_buf, we->envrc_name);
      XChangeProperty (dpy, ROOT_WINDOW, id, XA_STRING, 8, PropModeAppend, tmp_buf, 256);
      tmp_buf[0] = '\0';
      strcpy ((char *) tmp_buf, we->env_name_str);
      XChangeProperty (dpy, ROOT_WINDOW, id, XA_STRING, 8, PropModeAppend, tmp_buf, 32);
    }
  for (i = 0, we = c_c->reverse_env; we; we = we->next, i++)
    {
      tmp_buf[0] = '\0';
      if (we->host_name)
        strcpy ((char *) tmp_buf, we->host_name);
      XChangeProperty (dpy, ROOT_WINDOW, id, XA_STRING, 8, PropModeAppend, tmp_buf, 256);
      tmp_buf[0] = '\0';
      if (we->envrc_name)
        strcpy ((char *) tmp_buf, we->envrc_name);
      XChangeProperty (dpy, ROOT_WINDOW, id, XA_STRING, 8, PropModeAppend, tmp_buf, 256);
      tmp_buf[0] = '\0';
      strcpy ((char *) tmp_buf, we->env_name_str);
      XChangeProperty (dpy, ROOT_WINDOW, id, XA_STRING, 8, PropModeAppend, tmp_buf, 32);
    }
  XChangeProperty (dpy, ROOT_WINDOW, id, XA_STRING, 8, PropModeAppend, p, strlen ((char *) p));
  XSetSelectionOwner (dpy, id, xim->root_pointer[xim->default_screen]->ximclient->w, 0L);
  XFlush (dpy);
  Free (p);
  return (ld);
}

#ifndef NOTFORK_XJUTIL
static void
kill_old_xjutil ()
{
  char tmp_buf[256];
  XIMLangDataBase *ld;
  XEvent event;
  Window w;
  Atom ret;

  for (ld = language_db; ld != NULL; ld = ld->next)
    {
      strcpy (tmp_buf, XJUTIL_START_PRO);
      strcat (tmp_buf, ld->lang);
      strcat (tmp_buf, "_OK");
      if (ret = XInternAtom (dpy, tmp_buf, True))
        {
          if (w = XGetSelectionOwner (dpy, ret))
            {
              event.type = ClientMessage;
              event.xclient.format = 32;
              event.xclient.window = w;
              event.xclient.data.l[0] = DIC_KILL;
              XSendEvent (dpy, w, False, NoEventMask, &event);
              XFlush (dpy);
            }
        }
    }
}

static void
exec_xjutil (prop_name, counter)
     char *prop_name;
     int counter;
{
  char tmp[2];
  unsigned char *p, tmp_buf[256];

  tmp[0] = (char) counter;
  tmp[1] = '\0';
  if (xjutil_name == NULL)
    {
      strcpy ((char *) tmp_buf, XJUTILBINDIR);
      strcat ((char *) tmp_buf, "/");
      strcat ((char *) tmp_buf, XJUTIL_NAME);
      p = tmp_buf;
    }
  else
    {
      p = (unsigned char *) xjutil_name;
    }
  execlp ((char *) p, (char *) p, DisplayString (dpy), prop_name, tmp, 0);
}

static void
catch_sigchld ()
{
  XIMLangDataBase *ld;
#ifdef SYSVR2
  int status;
#else
  union wait status;
#endif
  int pid;

#ifdef SYSVR2
  pid = wait (&status);
#else
  pid = wait3 (&status, WNOHANG, 0);
#endif
  for (ld = language_db; ld != NULL; ld = ld->next)
    {
      if (ld->xjutil_pid == pid)
        {
          ld->xjutil_id = 0;
          ld->xjutil_pid = 0;
          return;
        }
    }
  return;
}
#endif /* !NOTFORK_XJUTIL */

void
xjutil_start ()
{
  char prop_name[32];
  static int counter = 1;
  XIMLangDataBase *ld;

  if (!(ld = set_xjutil (prop_name)))
    {
      print_out ("Could not exec xjutil");
      return;
    }
  ld->xjutil_act = counter++;
#ifndef NOTFORK_XJUTIL
  signal (SIGCHLD, catch_sigchld);
  if ((ld->xjutil_pid = fork ()) == 0)
    {
      close (ConnectionNumber (dpy));
      exec_xjutil (prop_name, ld->xjutil_act);
      print_out ("Could not execute xjutil");
      exit (1);
    }
#endif /* NOTFORK_XJUTIL */
}

void
kill_xjutil (ld)
     XIMLangDataBase *ld;
{
  extern int kill ();

  if (ld->xjutil_pid > 0)
    {
      kill (ld->xjutil_pid, SIGTERM);
      ld->xjutil_act = 0;
      ld->xjutil_id = 0;
      ld->xjutil_pid = 0;
    }
}

void
kill_all_xjutil ()
{
  XIMLangDataBase *ld;

  for (ld = language_db; ld; ld = ld->next)
    {
      if (ld->xjutil_pid > 0)
        {
          kill_xjutil (ld);
        }
    }
  return;
}

static void
send_xjutil_event (id)
     int id;
{
  XEvent event;

  event.type = ClientMessage;
  event.xclient.format = 32;
  event.xclient.window = xim->root_pointer[xim->default_screen]->ximclient->w;
  event.xclient.data.l[0] = id;
  XSendEvent (dpy, cur_p->cur_xl->lang_db->xjutil_id, False, NoEventMask, &event);
  XFlush (dpy);
  return;
}

static int
check_cur_xjutil ()
{
  register XIMLangRec *xl = cur_p->cur_xl;

  if (xl->lang_db->xjutil_id == 0 || xl->lang_db->xjutil_use)
    return (-1);
  xl->working_xjutil = 1;
  xl->lang_db->xjutil_use = cur_p;
  return (0);
}

void
xjutil_send_key_event (ev)
     XKeyEvent *ev;
{
  XSendEvent (dpy, cur_p->cur_xl->lang_db->xjutil_id, False, NoEventMask, (XEvent *) ev);
  XFlush (dpy);
  return;
}

void
set_xjutil_id (w, act, error)
     Window w;
     int act;
     char error;
{
  XIMLangDataBase *ld;

  for (ld = language_db; ld != NULL; ld = ld->next)
    {
      if (ld->xjutil_act == act)
        {
          if (error)
            {
              ld->xjutil_id = 0;
              ld->xjutil_pid = 0;
            }
          else
            {
              ld->xjutil_id = w;
              XSelectInput (dpy, w, StructureNotifyMask);
            }
          return;
        }
    }
  return;
}

void
reset_xjutil_fs_id (id, act)
     int id;
     int act;
{
  register int i;
  XIMLangDataBase *ld;
  XIMClientList xc = ximclient_list;
  XIMLangRec **xl;

  for (ld = language_db; ld; ld = ld->next)
    {
      if (ld->xjutil_act == act)
        break;
    }
  if (ld == NULL)
    return;
  for (; xc != NULL; xc = xc->next)
    {
      for (i = 0, xl = xc->xl; i < xc->lang_num; i++)
        {
          if ((xl[i]->lang_db == ld) && (xl[i]->xjutil_fs_id == id))
            {
              xl[i]->xjutil_fs_id = -1;
              return;
            }
        }
    }
  return;
}

int
isstart_xjutil (lang)
     char *lang;
{
  Atom ret;
  char tmp[64];

  strcpy (tmp, XJUTIL_START_PRO);
  strcat (tmp, lang);
  strcat (tmp, "_OK");
  if (ret = XInternAtom (dpy, tmp, True))
    {
      if (XGetSelectionOwner (dpy, ret))
        {
          return (1);
        }
    }
  return (0);
}

void
end_xjutil_work (act)
     int act;
{
  register XIMLangDataBase *ld;

  for (ld = language_db; ld; ld = ld->next)
    {
      if (ld->xjutil_act == act)
        {
          if (ld->xjutil_use)
            {
              ld->xjutil_use->cur_xl->working_xjutil = 0;
              ld->xjutil_use = NULL;
            }
          break;
        }
    }
}

int
xjutil_destroy (ev)
     register XDestroyWindowEvent *ev;
{
  Window window = ev->window;
  register XIMClientList xc;
  register XIMLangDataBase *ld;
  register int i;

  for (ld = language_db; ld; ld = ld->next)
    {
      if (ld->xjutil_id == window)
        break;
    }
  if (!ld)
    return (0);
  ld->xjutil_id = (Window) 0;
  ld->xjutil_pid = 0;
  ld->xjutil_use = NULL;
  XSelectInput (dpy, window, NoEventMask);
  for (xc = ximclient_list; xc; xc = xc->next)
    {
      for (i = 0; i < xc->lang_num; i++)
        {
          if (xc->xl[i]->lang_db == ld)
            {
              xc->xl[i]->xjutil_fs_id = -1;
              xc->xl[i]->working_xjutil = 0;
            }
        }
    }
  return (1);
}

static Atom
set_uum_env ()
{
  Atom id;
  Xjutil_envRec xjutil_env;
  register int i, nbytes = 0;
  register unsigned char *p = NULL;
  XFontStruct **font_struct_list;
  char **font_name_list;
  int num;
  WnnEnv *we;
  char prop_name[32];

  strcpy (prop_name, XJUTIL_ENV_PRO);
  strcat (prop_name, cur_p->cur_xl->lang_db->lang);
  if ((id = XInternAtom (dpy, prop_name, True)) == 0)
    {
      id = XInternAtom (dpy, prop_name, False);
    }
  xjutil_env.screen = cur_p->root_pointer->screen;
  if (cur_p->cur_xl->xjutil_fs_id == -1)
    {
      nbytes = 0;
      num = XFontsOfFontSet (cur_p->cur_xl->pe_fs, &font_struct_list, &font_name_list);
      for (i = 0; i < num; i++)
        {
          nbytes += (strlen (font_name_list[i]) + 1);
        }
      if (!(p = (unsigned char *) Malloc ((unsigned) (nbytes + 1))))
        {
          malloc_error ("Allocation of work area for xjutil");
          return ((Atom) 0);
        }
      p[0] = '\0';
      for (i = 0; i < num; i++)
        {
          strcat ((char *) p, font_name_list[i]);
          strcat ((char *) p, ",");
        }
      p[nbytes - 1] = '\0';
      xjutil_env.fs_id = cur_p->cur_xl->xjutil_fs_id = seq_xjutil_fs_id++;
      xjutil_env.fn_len = nbytes - 1;
    }
  else
    {
      xjutil_env.fs_id = cur_p->cur_xl->xjutil_fs_id;
      xjutil_env.fn_len = 0;
    }
  xjutil_env.fore_ground = cur_p->pe.fg;
  xjutil_env.back_ground = cur_p->pe.bg;
  xjutil_env.cur_env_id = -1;
  xjutil_env.cur_env_reverse_id = -1;
  if (c_c->cur_normal_env && c_c->cur_normal_env->env)
    xjutil_env.cur_env_id = c_c->cur_normal_env->env->env_id;
  if (c_c->cur_reverse_env && c_c->cur_reverse_env->env)
    xjutil_env.cur_env_reverse_id = c_c->cur_reverse_env->env->env_id;
  for (i = 0, we = c_c->normal_env; we; we = we->next, i++)
    {
      if (we->env)
        xjutil_env.env_id[i] = we->env->env_id;
      else
        xjutil_env.env_id[i] = -1;
    }
  for (i = 0, we = c_c->reverse_env; we; we = we->next, i++)
    {
      if (we->env)
        xjutil_env.env_reverse_id[i] = we->env->env_id;
      else
        xjutil_env.env_reverse_id[i] = -1;
    }
  if (c_c->cur_reverse_env && jl_env_get (bun_data_) == c_c->cur_reverse_env->env)
    xjutil_env.env_is_reverse = 1;
  else
    xjutil_env.env_is_reverse = 0;
  strcpy (xjutil_env.lc_name, cur_lang->lc_name);
  xjutil_env.cswidth_id = cur_lang->cswidth_id;
  XChangeProperty (dpy, ROOT_WINDOW, id, XA_STRING, 8, PropModeReplace, (unsigned char *) &xjutil_env, sizeof (Xjutil_envRec));
  if (nbytes > 0)
    {
      XChangeProperty (dpy, ROOT_WINDOW, id, XA_STRING, 8, PropModeAppend, p, strlen ((char *) p));
      Free (p);
    }
  XFlush (dpy);
  return (id);
}

static void
set_uum_touroku (start)
     int start;
{
  Atom id;

  if ((id = set_uum_env ()) == 0)
    return;
  XChangeProperty (dpy, ROOT_WINDOW, id, XA_STRING, 8, PropModeAppend, (unsigned char *) &start, sizeof (int));
  XChangeProperty (dpy, ROOT_WINDOW, id, XA_STRING, 8, PropModeAppend, (unsigned char *) c_b->buffer, Strlen (c_b->buffer) * 2);
  XFlush (dpy);
}

void
xw_jishoadd ()
{
  if (check_cur_xjutil ())
    return;
  if (set_uum_env () == 0)
    return;
  send_xjutil_event (DIC_ADD);
}

void
xw_kensaku ()
{
  if (check_cur_xjutil ())
    return;
  if (set_uum_env () == 0)
    return;
  send_xjutil_event (DIC_KENSAKU);
}

void
xw_select_one_dict9 ()
{
  if (check_cur_xjutil ())
    return;
  if (set_uum_env () == 0)
    return;
  send_xjutil_event (DIC_ICHIRAN);
}

void
xw_paramchg ()
{
  if (check_cur_xjutil ())
    return;
  if (set_uum_env () == 0)
    return;
  send_xjutil_event (DIC_PARAM);
}

void
xw_dicinfoout ()
{
  if (check_cur_xjutil ())
    return;
  if (set_uum_env () == 0)
    return;
  send_xjutil_event (DIC_INFO);
}

void
xw_fuzoku_set ()
{
  if (check_cur_xjutil ())
    return;
  if (set_uum_env () == 0)
    return;
  send_xjutil_event (DIC_FZK);
}

void
xw_touroku (s)
     int s;
{
  if (check_cur_xjutil ())
    return;
  set_uum_touroku (s);
  send_xjutil_event (DIC_TOUROKU);
}

void
call_kill_old_xjutil ()
{
#ifndef NOTFORK_XJUTIL
  kill_old_xjutil ();
#endif /* !NOTFORK_XJUTIL */
}

#endif /* USING_XJUTIL */
