/*
 * $Id: client.c,v 1.15.2.3 1999/04/19 06:10:56 nakanisi Exp $
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
#include "config.h"
#include "sdefine.h"
#include "xim.h"
#include "sheader.h"
#include "ext.h"
#include "rk_spclval.h"
#include "rk_fundecl.h"

/*
 *      env. control routines
 */

char
env_state ()
{
  char *p;
  char ret = '\0';
/*    extern char *index();*/

  if ((p = romkan_dispmode ()) == NULL)
    return (ret);
  if ((p = (char *) index (p, ':')) == NULL)
    return (ret);
  return (*(p + 1));
}

int
set_cur_env (s)
     char s;
{
  register WnnEnv *p;
  register int i;

  for (p = c_c->normal_env; p; p = p->next)
    {
      for (i = 0; p->env_name_str[i]; i++)
        {
          if (s == p->env_name_str[i])
            {
              c_c->cur_normal_env = p;
              break;
            }
        }
    }
  for (p = c_c->reverse_env; p; p = p->next)
    {
      for (i = 0; p->env_name_str[i]; i++)
        {
          if (s == p->env_name_str[i])
            {
              c_c->cur_reverse_env = p;
              break;
            }
        }
    }
  return (0);
}

int
get_new_env (wc, ld, rev)
     WnnClientRec *wc;
     XIMLangDataBase *ld;
     int rev;
{
  register WnnEnv *p;

  if (wc == NULL && ld == NULL)
    return (-1);
  if (!(p = (WnnEnv *) Malloc (sizeof (WnnEnv))))
    {
      malloc_error ("allocation of work area");
      return (-1);
    }
  p->host_name = NULL;
  p->env = NULL;
  p->sticky = 0;
  p->envrc_name = NULL;
  p->env_name_str[0] = '\0';
  if (wc == NULL)
    {
      if (rev)
        {
          p->next = ld->reverse_env;
          ld->reverse_env = p;
        }
      else
        {
          p->next = ld->normal_env;
          ld->normal_env = p;
        }
    }
  else
    {
      if (rev)
        {
          p->next = wc->reverse_env;
          wc->reverse_env = wc->cur_reverse_env = p;
        }
      else
        {
          p->next = wc->normal_env;
          wc->normal_env = wc->cur_normal_env = p;
        }
    }
  return (0);
}

void
free_env (wnnenv)
     WnnEnv *wnnenv;
{
  WnnEnv *p, *next;

  for (p = wnnenv; p; p = next)
    {
      if (p->host_name)
        Free ((char *) p->host_name);
      next = p->next;
      Free ((char *) p);
    }
}

static int
set_server_name ()
{
  if ((def_servername == NULL || *def_servername == '\0') && root_def_servername != NULL && (*(root_def_servername) != '\0'))
    if (!(def_servername = alloc_and_copy (root_def_servername)))
      {
        malloc_error ("allocation of data for wnn");
        return (-1);
      }

  if ((def_reverse_servername == NULL || *def_reverse_servername == '\0') && (root_def_reverse_servername != NULL) && (*(root_def_reverse_servername) != '\0'))
    if (!(def_reverse_servername = alloc_and_copy (root_def_reverse_servername)))
      {
        if (def_servername)
          {
            Free (def_servername);
            def_servername = NULL;
          }
        malloc_error ("allocation of data for wnn");
        return (-1);
      }
  return (0);
}

static int
create_c_c (xc, xl, root)
     register XIMClientRec *xc;
     register XIMLangRec *xl;
     Bool root;
{
  xc->cur_xl = xl;
  cur_lang = xl->lang_db;
  if (allocate_wnn (xc->user_name) == -1)
    {
      return (-1);
    }
  xl->w_c = c_c;
  c_c->lang_db = xl->lang_db;
  if (root == True)
    {
      henkan_off_def = root_henkan_off_def;
      uumkeyname = root_uumkeyname;
      rkfilename = root_rkfilename;
    }
  c_c->f_table = xl->lang_db->f_table;
  if (set_server_name () < 0)
    {
      free_wnn ();
      return (-1);
    }
  if (initialize_wnn (xl, root) == -1)
    {
      return (-1);
    }
  if (connect_server (xl->lang_db) < 0)
    {
      del_client (c_c, 0);
      return (-1);
    }
  return (0);
}

int
new_client (xc, xl, root)
     XIMClientRec *xc;
     XIMLangRec *xl;
     Bool root;
{
  if (IsPreeditArea (xc) || IsPreeditPosition (xc)
#ifdef  CALLBACKS
      || IsPreeditCallbacks (xc)
#endif /* CALLBACKS */
    )
    {
      if (create_c_c (xc, xl, root) == -1)
        {
          return (-1);
        }
      if (create_preedit (xc, xl, 0) < 0)
        {
          free_wnn ();
          return (-1);
        }
      clear_c_b ();
    }
  else if (IsPreeditNothing (xc))
    {
      if (create_preedit (xc, NULL, 0) < 0)
        {
          return (-1);
        }
      xc->yes_no = NULL;
    }
  if (IsStatusArea (xc)
#ifdef  CALLBACKS
      || IsStatusCallbacks (xc)
#endif /* CALLBACKS */
    )
    {
      xc->cur_xl = xl;
      if (create_status (xc, xl, 0) < 0)
        {
          free_wnn ();
          return (-1);
        }
      if (xc->cur_xl->w_c->h_flag)
        {
          push_func (xc->cur_xl->w_c, henkan_off);
        }
    }
  else if (IsStatusNothing (xc))
    {
      if (create_status (xc, NULL, 0) < 0)
        {
          return (-1);
        }
    }
  return (0);
}

static int
find_same_env (name, p, rev)
     char *name;
     WnnEnv *p;
     int rev;
{
  register WnnEnv *pp;
  register WnnClientList cc;

  for (cc = wnnclient_list; cc; cc = cc->next)
    {
      pp = rev ? cc->reverse_env : cc->normal_env;
      if (pp != p && !strcmp (name, cc->user_name))
        {
          for (; pp; pp = pp->next)
            {
              if (!p->host_name && !pp->host_name)
                {
                  if (!*p->env_name_str && !*pp->env_name_str)
                    {
                      return (1);
                    }
                  else if (*p->env_name_str && *pp->env_name_str && !strcmp (p->env_name_str, pp->env_name_str))
                    {
                      return (1);
                    }
                }
              else if (p->host_name && pp->host_name && !strcmp (p->host_name, pp->host_name))
                {
                  if (!*p->env_name_str && !*pp->env_name_str)
                    {
                      return (1);
                    }
                  else if (*p->env_name_str && *pp->env_name_str && !strcmp (p->env_name_str, pp->env_name_str))
                    {
                      return (1);
                    }
                }
            }
        }
    }
  return (0);
}

void
del_client (wc, level)
     register WnnClientRec *wc;
     int level;
{
  register WnnClientRec *c_c_sv;
  register WnnEnv *p;

  c_c_sv = c_c;
  c_c = wc;
  switch (level)
    {
    case 4:
      goto LEVEL4;
    case 3:
      goto LEVEL3;
    case 2:
      goto LEVEL2;
    case 1:
      goto LEVEL1;
    default:
      break;
    }
  if (c_c->rk)
    rk_close (c_c->rk);
LEVEL1:
  destroy_history ();
LEVEL2:
  free_areas ();
LEVEL3:
  if (c_c->m_table->re_count == 1)
    {
      Free ((char *) c_c->m_table);
    }
  else
    {
      c_c->m_table->re_count--;
    }
LEVEL4:
  if (c_c->use_server)
    {
      for (p = c_c->normal_env; p; p = p->next)
        {
          if (jl_isconnect_e (p->env))
            {
              if (!find_same_env (username, p, 0))
                {
                  jl_dic_save_all_e (p->env);
                }
              jl_disconnect (p->env);
            }
        }
      for (p = c_c->reverse_env; p; p = p->next)
        {
          if (jl_isconnect_e (p->env))
            {
              if (!find_same_env (username, p, 1))
                {
                  jl_dic_save_all_e (p->env);
                }
              jl_disconnect (p->env);
            }
        }
    }
  free_env (c_c->normal_env);
  free_env (c_c->reverse_env);
  if (bun_data_)
    {
      if (c_c->use_server)
        {
          jl_env_set (bun_data_, NULL);
          jl_close (bun_data_);
        }
      else
        {
          Free (bun_data_);
        }
    }

  free_wnn ();
  c_c = c_c_sv;
}

void
epilogue ()
{
  register WnnClientRec *wc;
  register WnnEnv *p;

  for (wc = wnnclient_list; wc != NULL; wc = wc->next)
    {
      c_c = wc;
      for (p = c_c->reverse_env; p; p = p->next)
        {
          c_c->cur_reverse_env = p;
          disconnect_server (1);
        }
      for (p = c_c->normal_env; p; p = p->next)
        {
          c_c->cur_normal_env = p;
          disconnect_server (0);
        }
    }
}
