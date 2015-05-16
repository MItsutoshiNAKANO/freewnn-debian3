/*
 * $Id: prologue.c,v 1.2 2001/06/14 18:16:17 ura Exp $
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
#include "commonhd.h"
#include "config.h"
#include "sdefine.h"
#include <X11/Xos.h>
#include "xim.h"
#include "sheader.h"
#include "ext.h"
#include "rk_spclval.h"
#include "rk_fundecl.h"

extern char *getenv ();
extern FILE *fopen ();

/** Initialize of romkan */
int
open_romkan (ld)
     XIMLangDataBase *ld;
{
  int rk_option_flg;

  rk_option_flg = RK_REDRAW | RK_CHMOUT | (excellent_delete ? 0 : RK_SIMPLD);

  if (ld->rk_table == NULL)
    {
      if ((ld->rk_table = romkan_table_init (NULL, ld->rkfile_name, NULL, bytcntfn, NULL, 0))
/*
                                              bytcntfn, letterpickfn, NULL, 0))
*/
          == NULL)
        {
          return (-1);
        }
    }
  cur_rk_table = ld->rk_table;
  c_c->rk = (Romkan *) romkan_init5 (rubout_code, rk_option_flg);
  if (c_c->rk == NULL)
    return (-1);
  else
    {
      c_c->rk->rk_table = cur_rk_table;
      return (0);
    }
}



/** Open the wnnenvrc_R file */
static int
get_envrc_name_reverse (str, lang)
     char *str, *lang;
{
  extern int access ();

  if (c_c->cur_reverse_env == NULL || reverse_envrcname == NULL || reverse_envrcname[0] == '\0')
    return (0);
  strcpy (str, reverse_envrcname);
  if (access (str, R_OK) == -1)
    {
      return (-1);
    }
  return (1);
}

/** Open the wnnenvrc file */
static int
get_envrc_name (str, lang)
     char *str, *lang;
{
  if (c_c->cur_normal_env == NULL || envrcname == NULL || envrcname[0] == '\0')
    {
      strcpy (str, LIBDIR);
      strcat (str, "/");
      strcat (str, lang);
      strcat (str, ENVRCFILE);
    }
  else
    {
      strcpy (str, envrcname);
    }
  return (1);
}

/* Allocate of Wnn Area */
int
allocate_areas ()
{
  char *area_start;
  char *area_pter;

  int k;
  int total_size;
  int len1 = maxchg * sizeof (w_char);
  int return_buf_len = len1 * 7;

  total_size = return_buf_len + len1 * 3 + maxbunsetsu * sizeof (int) * 3 + maxbunsetsu * sizeof (struct wnn_env *);

  if ((area_start = Malloc (total_size)) == NULL)
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
  area_pter += maxbunsetsu * sizeof (int);
  bunsetsu_env = (struct wnn_env **) area_pter;
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

void
free_areas ()
{
  Free (return_buf);
}

static struct wnn_buf *
local_jl_open_lang ()
{
  struct wnn_buf *buf;
  register char **c, *p;

  if (!(p = Malloc (sizeof (struct wnn_buf) + sizeof (char *))))
    {
      malloc_error ("allocation of work area for Wnn");
      return (NULL);
    }
  buf = (struct wnn_buf *) p;
  p += sizeof (struct wnn_buf);
  buf->bun_suu = 0;
  buf->zenkouho_suu = 0;
  buf->zenkouho_daip = 0;
  buf->c_zenkouho = -1;
  buf->zenkouho_bun = -1;
  buf->zenkouho_end_bun = -1;
  buf->free_heap = NULL;
  c = (char **) p;
  *c = NULL;
  buf->heap = (char *) c;
  buf->zenkouho_dai_suu = 0;
  buf->msize_bun = 0;
  buf->msize_zenkouho = 0;
  buf->zenkouho_dai_suu = 0;
  buf->bun = NULL;
  buf->zenkouho_dai = NULL;
  buf->zenkouho = NULL;
  buf->down_bnst = NULL;
  buf->zenkouho_dai = NULL;
  buf->env = NULL;
  return (buf);
}

/** Connecting to jserver */
int
connect_server (db)
     XIMLangDataBase *db;
{
  char environment[PATHNAMELEN];
  char uumrc_name[PATHNAMELEN];
  register WnnEnv *p;
  WnnEnv *save_cur_normal_env = NULL, *save_cur_reverse_env = NULL;

  if ((db->lang == NULL) || (db->connect_serv == False))
    {
      if (c_c->normal_env == NULL)
        {
          if (get_new_env (c_c, NULL, 0) < 0)
            return (-1);
        }
      c_c->use_server = 0;
      c_c->cur_normal_env = c_c->normal_env;
      uumrc_name[0] = '\0';
      if (!(bun_data_ = local_jl_open_lang ()))
        return (-1);
      return (0);
    }
  else
    {
      c_c->use_server = 1;
    }

  if (c_c->cur_normal_env)
    save_cur_normal_env = c_c->cur_normal_env;
  for (p = c_c->normal_env; p; p = p->next)
    {
      c_c->cur_normal_env = p;
      get_envrc_name (uumrc_name, db->lang);    /* use username for env-name */
      environment[0] = '\0';
      strcpy (environment, username);
      strcat (environment, p->env_name_str);
      if (servername == NULL && def_servername && *def_servername)
        {
          servername = alloc_and_copy (def_servername);
        }
      if (bun_data_ == NULL)
        {
          bun_data_ = jl_open_lang (environment, servername, db->lang, uumrc_name, yes_or_no_lock, print_msg_wait, WNN_TIMEOUT);
          if (!bun_data_)
            {
              if (wnn_errorno == WNN_ALLOC_FAIL)
                {
                  malloc_error ("alloction of work area for Wnn");
                }
              return (-1);
            }
          if (bun_data_->env != NULL)
            {
              env_normal = bun_data_->env;
            }
        }
      else if (env_normal == 0 || jl_isconnect_e (env_normal) == 0)
        {
          env_normal = jl_connect_lang (environment, servername, db->lang, uumrc_name, yes_or_no_lock, print_msg_wait, WNN_TIMEOUT);
        }
      if (env_normal)
        {
          if (normal_sticky)
            jl_env_sticky_e (env_normal);
          else
            jl_env_un_sticky_e (env_normal);
        }
    }
  if (save_cur_normal_env)
    {
      c_c->cur_normal_env = save_cur_normal_env;
      if (env_normal)
        jl_env_set (bun_data_, env_normal);
    }
  if (c_c->normal_env == NULL)
    {
      if (get_new_env (c_c, NULL, 0) < 0)
        return (-1);
      c_c->use_server = 0;
      c_c->cur_normal_env = c_c->normal_env;
      uumrc_name[0] = '\0';
      if (!(bun_data_ = local_jl_open_lang ()))
        return (-1);
    }

  if (c_c->cur_reverse_env)
    save_cur_reverse_env = c_c->cur_reverse_env;
  for (p = c_c->reverse_env; p; p = p->next)
    {
      c_c->cur_reverse_env = p;
      if (get_envrc_name_reverse (uumrc_name, db->lang) > 0)
        {
          environment[0] = '\0';
          strcpy (environment, username);
          strcat (environment, p->env_name_str);
          strcat (environment, "R");
          if (reverse_servername == NULL && def_reverse_servername && *def_reverse_servername)
            {
              reverse_servername = alloc_and_copy (def_reverse_servername);
            }
          env_reverse = jl_connect_lang (environment, reverse_servername, db->lang, uumrc_name, yes_or_no_lock, print_msg_wait, WNN_TIMEOUT);
          if (env_reverse)
            {
              if (reverse_sticky)
                jl_env_sticky_e (env_reverse);
              else
                jl_env_un_sticky_e (env_reverse);
            }
        }
    }
  if (save_cur_reverse_env)
    c_c->cur_reverse_env = save_cur_reverse_env;
#ifdef  USING_XJUTIL
  if (!isstart_xjutil (db->lang))
    xjutil_start ();
#endif /* USING_XJUTIL */
  return (0);
}

int
init_xcvtkey ()
{
  char tmp_file[MAXPATHLEN];
  int ret;

  cvt_key_tbl_cnt = 0;
  /* Backward compatibility for cvt_meta and cvt_fun. */
  if (cvt_fun_file && *cvt_fun_file)
    {
      if ((ret = cvt_meta_and_fun_setup (cvt_fun_file, cvt_key_tbl, NULL)) == -1)
        {
          print_out1 ("I can't open cvt_fun file \"%s\".", cvt_fun_file);
          cvt_fun_file = NULL;
        }
      else
        {
          cvt_key_tbl_cnt = ret;
        }
    }
  if (cvt_meta_file && *cvt_meta_file)
    {
      if ((ret = cvt_meta_and_fun_setup (cvt_meta_file, cvt_key_tbl, "Meta")) == -1)
        {
          print_out1 ("I can't open cvt_meta file \"%s\".", cvt_meta_file);
          cvt_meta_file = NULL;
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
          print_out1 ("I can't open cvt_key file \"%s\", use default file.", cvt_key_file);
          cvt_key_file = NULL;
        }
    }
  if (cvt_key_file == NULL)
    {
      strcat (strcpy (tmp_file, LIBDIR), CVTKEYFILE);
      if (!(cvt_key_file = alloc_and_copy (tmp_file)))
        return (-1);
      if (cvt_key_setup (cvt_key_file) == -1)
        {
          print_out1 ("I can't open default cvt_key file \"%s\".\n I don't convert function keys.", cvt_key_file);
          Free (cvt_key_file);
          cvt_key_file = NULL;
        }
    }
  return (0);
}
