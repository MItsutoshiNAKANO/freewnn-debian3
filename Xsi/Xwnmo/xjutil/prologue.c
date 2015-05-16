/*
 * $Id: prologue.c,v 1.7.2.1 1999/02/08 07:43:35 yamasita Exp $
 */

/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991, 1992
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
/* Standard Interface
 *    Initialize Routine
 */

#include <stdio.h>
#include "jslib.h"
#include "jd_sock.h"
#include "commonhd.h"
#include "config.h"
#include "sdefine.h"
#include "xjutil.h"
#include "sxheader.h"
#include "rk_spclval.h"
#include "rk_fundecl.h"
#include "xext.h"

extern char *getenv ();
extern FILE *fopen ();

int
open_romkan ()
{
  int rk_option_flg;
  extern int keyin ();

  if (rkfile_name == NULL)
    return (-1);
  cur_rk_table = romkan_table_init (NULL, rkfile_name, NULL, bytcntfn, NULL, 0);
  rk_option_flg = RK_REDRAW | RK_CHMOUT | (excellent_delete ? 0 : RK_SIMPLD);

  cur_rk = (Romkan *) romkan_init5 (rubout_code, rk_option_flg);
  if (cur_rk == NULL)
    {
      return (-1);
    }
  else
    {
      return (0);
    }
}

static int
allocate_areas ()
{
  char *area_start;
  char *area_pter;

  int k;
  int total_size;
  int len1 = maxchg * sizeof (w_char);
  int return_buf_len = len1 * 7;

  total_size = return_buf_len + len1 * 3 + maxbunsetsu * sizeof (int) * 3;

  if ((area_start = (char *) Malloc (total_size)) == NULL)
    {
      malloc_error ("allocation of Wnn's area");
      return (-1);
    }

  area_pter = area_start;
  return_buf = (w_char *) area_pter;
  area_pter += return_buf_len;
  input_buffer = (w_char *) area_pter;
  area_pter += len1;
  kill_buffer = (w_char *) area_pter;
  area_pter += len1;
  remember_buf = (w_char *) area_pter;
  *remember_buf = 0;
  area_pter += len1;
  bunsetsu = (int *) area_pter;
  area_pter += maxbunsetsu * sizeof (int);
  bunsetsuend = (int *) area_pter;
  area_pter += maxbunsetsu * sizeof (struct wnn_env *);
  touroku_bnst = (int *) area_pter;
  area_pter += maxbunsetsu * sizeof (int);
  for (k = 0; k < maxbunsetsu; k++)
    {
      bunsetsu[k] = 0;
      bunsetsuend[k] = 0;
      touroku_bnst[k] = 0;
    }

  return (0);
}

int
init_wnn ()
{
  if (init_key_table (xjutil->lang) == -1)
    {
      return (-1);
    }

  if (open_romkan () == -1)
    {
      return (-1);
    }

  if (allocate_areas () == -1)
    {
      return (-1);
    }
  return (0);
}

typedef struct _js_list_str
{
  WNN_JSERVER_ID *js;
  char *host_name;
  struct _js_list_str *next;
}
js_list_str;

static js_list_str *js_list = NULL;

static WNN_JSERVER_ID *
find_same_js (host_name)
     char *host_name;
{
  js_list_str *j;

  if (!host_name || !*host_name)
    {
      for (j = js_list; j; j = j->next)
        {
          if (js_list->host_name == NULL)
            return (js_list->js);
        }
      return (NULL);
    }
  else
    {
      for (j = js_list; j; j = j->next)
        {
          if (!strcmp (js_list->host_name, host_name))
            return (js_list->js);
        }
      return (NULL);
    }
}

static int
add_same_js (js, host_name)
     WNN_JSERVER_ID *js;
     char *host_name;
{
  js_list_str *j;

  if (!(j = (js_list_str *) Malloc (sizeof (js_list_str))))
    return (-1);
  j->js = js;
  j->host_name = host_name;
  j->next = js_list;
  js_list = j;
  return (0);
}

int
connect_server ()
{
  register WNN_JSERVER_ID *js = NULL;
  char *machine_n;
  WnnEnv *p;
  int i;
  char environment[PATHNAMELEN];
  extern char *_wnn_get_machine_of_serv_defs ();

  for (i = 0, p = normal_env; p; p = p->next, i++)
    {
      cur_normal_env = p;
      js = NULL;
      if (!p->host_name || !*p->host_name)
        {
          if (machine_n = _wnn_get_machine_of_serv_defs (xjutil->lang))
            {
              p->host_name = alloc_and_copy (machine_n);
              if (js = find_same_js (p->host_name))
                goto NEXT_STEP1;
              if ((js = js_open_lang (p->host_name, xjutil->lang, WNN_TIMEOUT)) == NULL)
                {
                  p->host_name = NULL;
                }
            }
          if (!p->host_name || !*p->host_name)
            {
              p->host_name = alloc_and_copy ("unix");
            }
        }
      if (js = find_same_js (p->host_name))
        goto NEXT_STEP1;
      if (js == NULL)
        {
          if ((js = js_open_lang (p->host_name, xjutil->lang, WNN_TIMEOUT)) == NULL)
            {
              return (-1);
            }
        }
      add_same_js (js, p->host_name);
    NEXT_STEP1:
      environment[0] = '\0';
      strcpy (environment, username);
      strcat (environment, p->env_name_str);
      if ((p->env = js_connect_lang (js, environment, xjutil->lang)) == NULL)
        {
          return (-1);
        }
      save_env_id[i] = p->env->env_id;
    }
  for (i = 0, p = reverse_env; p; p = p->next, i++)
    {
      cur_reverse_env = p;
      js = NULL;
      if (!p->host_name || !*p->host_name)
        {
          if (machine_n = _wnn_get_machine_of_serv_defs (xjutil->lang))
            {
              p->host_name = alloc_and_copy (machine_n);
              if (js = find_same_js (p->host_name))
                goto NEXT_STEP2;
              if ((js = js_open_lang (p->host_name, xjutil->lang, WNN_TIMEOUT)) == NULL)
                {
                  p->host_name = NULL;
                }
            }
          if (!p->host_name || !*p->host_name)
            {
              p->host_name = alloc_and_copy ("unix");
            }
        }
      if (js = find_same_js (p->host_name))
        goto NEXT_STEP2;
      if (js == NULL)
        {
          if ((js = js_open_lang (p->host_name, xjutil->lang, WNN_TIMEOUT)) == NULL)
            {
              return (-1);
            }
        }
      add_same_js (js, p->host_name);
    NEXT_STEP2:
      environment[0] = '\0';
      strcpy (environment, username);
      strcat (environment, p->env_name_str);
      if ((p->env = js_connect_lang (js, environment, xjutil->lang)) == NULL)
        {
          return (-1);
        }
      save_env_id[i] = p->env->env_id;
    }
  return (0);
}

int
init_xcvtkey ()
{
  int ret;

  cvt_key_tbl_cnt = 0;
  /* Backward compatibility for cvt_meta and cvt_fun. */
  if (cvt_fun_file && *cvt_fun_file)
    {
      if ((ret = cvt_meta_and_fun_setup (cvt_fun_file, cvt_key_tbl)) == -1)
        {
          fprintf (stderr, "File \"%s\" can't open.", cvt_fun_file);
        }
      else
        {
          cvt_key_tbl_cnt = ret;
        }
    }
  if (cvt_meta_file && *cvt_meta_file)
    {
      if ((ret = cvt_meta_and_fun_setup (cvt_meta_file, cvt_key_tbl)) == -1)
        {
          fprintf (stderr, "File \"%s\" can't open.", cvt_meta_file);
        }
      else
        {
          cvt_key_tbl_cnt = ret;
        }
    }
  if (cvt_key_file && *cvt_key_file)
    {
      if (cvt_key_setup (cvt_key_file) == -1)
        {
          fprintf (stderr, "File \"%s\" can't open.", cvt_key_file);
        }
    }
  return (0);
}
