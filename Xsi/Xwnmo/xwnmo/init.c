/*
 * $Id: init.c,v 1.6.2.1 1999/02/08 08:08:47 yamasita Exp $
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

#include "sdefine.h"
#include "xim.h"
#include "sheader.h"
#include "ext.h"

static void
default_flag_set ()
{
  cursor_colum = 0;
  cursor_reverse = 0;
  cursor_underline = 0;
  cursor_invisible = 1;
  cursor_bold = 0;
  quote_flag = 0;
  send_ascii_char = 0;
  send_ascii_stack = 0;
  send_ascii_char_quote = 0;
  c_c->command_func_stack[0] = NULL;
  c_c->func_stack_count = -1;
  c_c->buffer_ignored = 0;
}

static int
copy_env (wc)
     register WnnClientRec *wc;
{
  WnnEnv *p;

  for (p = wc->normal_env; p; p = p->next)
    {
      if (get_new_env (c_c, NULL, 0) < 0 || (p->host_name && !(servername = alloc_and_copy (p->host_name))))
        goto ERROR_RET;
      envrcname = p->envrc_name;
      strcpy (env_name, p->env_name_str);
    }
  for (p = wc->reverse_env; p; p = p->next)
    {
      if (get_new_env (c_c, NULL, 1) < 0 || (p->host_name && !(reverse_servername = alloc_and_copy (p->host_name))))
        goto ERROR_RET;
      reverse_envrcname = p->envrc_name;
      strcpy (reverse_env_name, p->env_name_str);
    }

  send_ascii_char = send_ascii_char_def;

  c_c->m_table = wc->m_table;
  c_c->m_table->re_count++;
  return (0);
ERROR_RET:
  free_env (c_c->normal_env);
  free_env (c_c->reverse_env);
  return (-1);
}

static void
add_wnnclientlist (cl)
     register WnnClientRec *cl;
{
  cl->next = wnnclient_list;
  wnnclient_list = cl;
}

static void
remove_wnnclientlist (cl)
     register WnnClientRec *cl;
{
  register WnnClientList p, *prev;
  for (prev = &wnnclient_list; p = *prev; prev = &p->next)
    {
      if (p == cl)
        {
          *prev = p->next;
          break;
        }
    }
}

void
add_ximclientlist (cl)
     register XIMClientRec *cl;
{
  cl->next = ximclient_list;
  ximclient_list = cl;
}

void
remove_ximclientlist (cl)
     register XIMClientRec *cl;
{
  register XIMClientList p, *prev;
  for (prev = &ximclient_list; p = *prev; prev = &p->next)
    {
      if (p == cl)
        {
          *prev = p->next;
          break;
        }
    }
}

void
add_inputlist (xi)
     register XIMInputRec *xi;
{
  xi->next = input_list;
  input_list = xi;
}

void
remove_inputlist (xi)
     register XIMInputRec *xi;
{
  register XIMInputList p, *prev;
  for (prev = &input_list; p = *prev; prev = &p->next)
    {
      if (p == xi)
        {
          *prev = p->next;
          break;
        }
    }
}

static WnnClientRec *
find_same_lang (lang)
     char *lang;
{
  register XIMClientRec *p;
  register XIMLangRec *xl;
  register int i;
  for (p = ximclient_list; p != NULL; p = p->next)
    {
      for (i = 0; i < p->lang_num; i++)
        {
          xl = p->xl[i];
          if (xl->w_c && xl->w_c->m_table)
            {
              if (!strcmp (xl->lang_db->lang, lang) && xl->w_c != NULL)
                {
                  return ((WnnClientRec *) xl->w_c);
                }
            }
        }
    }
  return ((WnnClientRec *) NULL);
}

int
allocate_wnn (uname)
     char *uname;
{
  int len = 0;
  register char *p;
  extern void bzero ();

  if (uname && *uname)
    len = strlen (uname) + 1;
  if (!(p = Malloc (sizeof (WnnClientRec) + sizeof (ClientBuf) + len)))
    {
      malloc_error ("allocation of data for wnn");
      return (-1);
    }
  c_c = (WnnClientList) p;
  bzero ((char *) c_c, sizeof (WnnClientRec));
  p += sizeof (WnnClientRec);
  c_b = (ClientBuf *) p;
  bzero ((char *) c_b, sizeof (ClientBuf));
  if (uname && *uname)
    {
      p += sizeof (ClientBuf);
      c_c->user_name = (char *) p;
      strcpy (c_c->user_name, uname);
    }
  add_wnnclientlist (c_c);
  default_flag_set ();
  return (0);
}

void
free_wnn ()
{
  remove_wnnclientlist (c_c);
  Free ((char *) c_c);
}

int
initialize_wnn (xl, root)
     XIMLangRec *xl;
     int root;
{
  WnnClientRec *p;

  if ((p = find_same_lang (xl->lang_db->lang)) != NULL)
    {
      if (copy_env (p) < 0)
        return (-1);
    }
  else
    {
      if (uumrc_get_entries (xl->lang_db, 0, root) == -1)
        {
          return (-1);
        }
      if (init_key_table (xl->lang_db->lang) == -1)
        {
          del_client (c_c, 4);
          return (-1);
        }
    }
  henkan_off_flag = henkan_off_def;
  maxlength = maxchg * 2;
  if (allocate_areas () == -1)
    {
      del_client (c_c, 3);
      return (-1);
    }
  if (init_history () == -1)
    {
      del_client (c_c, 2);
      return (-1);
    }
  if (open_romkan (xl->lang_db) == -1)
    {
      del_client (c_c, 1);
      return (-1);
    }
  return (0);
}
