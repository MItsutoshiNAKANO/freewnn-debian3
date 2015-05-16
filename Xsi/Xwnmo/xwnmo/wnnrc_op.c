/*
 * $Id: wnnrc_op.c,v 1.10.2.1 1999/02/08 08:08:59 yamasita Exp $
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
/* uumrc operations */

#include "stdio.h"
#include <pwd.h>
#include "commonhd.h"
#include "config.h"
#include "sdefine.h"
#include "xim.h"
#include "sheader.h"
#include "ext.h"
#include "rk_spclval.h"
#include "rk_fundecl.h"

extern char *getenv ();
extern FILE *fopen ();

static char open_uumrc_filenm[1024];

static void
err_expand (f, s)
     char *f, *s;
{
  print_out2 ("In uumrc file \"%s\", I could not expand %s.", f, s);
}

static int
change_ascii_to_int (st, dp)
     char *st;
     int *dp;
{
  register int total, flag;

  total = 0;
  flag = 0;
  while (*st != '\0')
    {
      if (isdigit (*st))
        {
          total = total * 10 + (*st - '0');
        }
      else if (*st == '+')
        {
          if (flag != 0)
            {
              return (-1);
            }
          flag = 1;
        }
      else if (*st == '-')
        {
          if (flag != 0)
            {
              return (-1);
            }
          flag = -1;
        }
      else
        {
          return (-1);
        }
      st++;
    }
  if (flag == 0)
    {
      flag = 1;
    }
  *dp = total * flag;
  return (1);
}

/*
 * set parameter
 */
static int
uumrc_set_entry (data, ld, rk_pre_load, root)
     char *data;
     XIMLangDataBase *ld;
     int rk_pre_load, root;
{
  int num, d1;
  char code[256];
  char s[7][EXPAND_PATH_LENGTH];
  char tmp[1024];
  FILE *fp;
  char *s0or1;
  register char **dst1, **dst2, *src1, *src2;
  register int copy = 0;

  /*
   * get one list
   */
  if (comment_char (*data) || (num = sscanf (data, "%s %s %s %s %s %s %s %s", code, s[0], s[1], s[2], s[3], s[4], s[5], s[6])) <= 0)
    return (0);

  if (strcmp (code, "include") == 0)
    {
      if (expand_expr (s[0], ld->lang) != 0)
        {
          err_expand (open_uumrc_filenm, s[0]);
        }
      if ((fp = fopen (s[0], "r")) != NULL)
        {
          while (fgets (tmp, 1024, fp) != NULL)
            {
              if (uumrc_set_entry (tmp, ld, rk_pre_load, root) == -1)
                {
                  fclose (fp);
                  return (-1);
                }
            }
          fclose (fp);
        }
      else
        {
          print_out2 ("Could not open uumrc \"%s\" included in \"%s\".", s[0], open_uumrc_filenm);
        }
    }
  else if (strcmp (code, "setrkfile") == 0)
    {
      if (!(rkfile_defined_by_option && (rk_pre_load || root) && ld == cur_lang) && (num > 1))
        {
          if (expand_expr (s[0], ld->lang) != 0)
            {
              err_expand (open_uumrc_filenm, s[0]);
            }
          copy++;
          dst1 = &ld->rkfile_name;
          src1 = s[0];
        }
    }
  else if (rk_pre_load == 0)
    {
      if (strcmp (code, "setuumkey") == 0)
        {
          if (num > 1)
            {
              if (ld->uumkey_name)
                Free (ld->uumkey_name);
              ld->uumkey_name = NULL;
              if (expand_expr (s[0], ld->lang) != 0)
                {
                  err_expand (open_uumrc_filenm, s[0]);
                }
              copy++;
              dst1 = &ld->uumkey_name;
              src1 = s[0];
            }
        }
      else if (strcmp (code, "setconvenv") == 0)
        {
          if (num > 1)
            {
              if (get_new_env (NULL, ld, 0) < 0)
                goto ERROR_RET;
              if (num > 2)
                {
                  if (!(strcmp (s[num - 2], "sticky")))
                    {
                      ld->normal_env->sticky = 1;
                      num--;
                    }
                }
              if (num == 2)
                s0or1 = s[0];
              else
                {
                  s0or1 = s[1];
                  if (def_servername && *def_servername)
                    src2 = def_servername;
                  else
                    src2 = s[0];
                  copy++;
                  dst2 = &ld->normal_env->host_name;
                }
              if (expand_expr (s0or1, ld->lang) != 0)
                {
                  err_expand (open_uumrc_filenm, s0or1);
                }
              copy++;
              dst1 = &ld->normal_env->envrc_name;
              src1 = s0or1;
            }
        }
      else if (strcmp (code, "setkankanaenv") == 0)
        {
          if (num > 1)
            {
              if (get_new_env (NULL, ld, 1) < 0)
                goto ERROR_RET;
              if (num > 2)
                {
                  if (!(strcmp (s[num - 2], "sticky")))
                    {
                      ld->reverse_env->sticky = 1;
                      num--;
                    }
                }
              if (num == 2)
                s0or1 = s[0];
              else
                {
                  s0or1 = s[1];
                  if (def_reverse_servername && *def_reverse_servername)
                    src2 = def_reverse_servername;
                  else
                    src2 = s[0];
                  copy++;
                  dst2 = &ld->reverse_env->host_name;
                }
              if (expand_expr (s0or1, ld->lang) != 0)
                {
                  err_expand (open_uumrc_filenm, s0or1);
                }
              copy++;
              dst1 = &ld->reverse_env->envrc_name;
              src1 = s0or1;
            }
        }
      else if (strcmp (code, "setenv") == 0)
        {
          if (num > 2)
            {
              if (get_new_env (NULL, ld, 0) < 0)
                goto ERROR_RET;
              if (!(strcmp (s[num - 3], "sticky")))
                {
                  ld->normal_env->sticky = 1;
                  num--;
                }
              if (num == 3)
                s0or1 = s[1];
              else
                {
                  s0or1 = s[2];
                  if (def_servername && *def_servername)
                    src2 = def_servername;
                  else
                    src2 = s[1];
                  copy++;
                  dst2 = &ld->normal_env->host_name;
                }
              if (expand_expr (s0or1, ld->lang) != 0)
                {
                  err_expand (open_uumrc_filenm, s0or1);
                }
              copy++;
              dst1 = &ld->normal_env->envrc_name;
              src1 = s0or1;
              strcpy (ld->normal_env->env_name_str, s[0]);
            }
        }
      else if (strcmp (code, "setenv_R") == 0)
        {
          if (num > 2)
            {
              if (get_new_env (NULL, ld, 1) < 0)
                goto ERROR_RET;
              if (!(strcmp (s[num - 3], "sticky")))
                {
                  ld->reverse_env->sticky = 1;
                  num--;
                }
              if (num == 3)
                s0or1 = s[1];
              else
                {
                  s0or1 = s[2];
                  if (def_reverse_servername && *def_reverse_servername)
                    src2 = def_reverse_servername;
                  else
                    src2 = s[1];
                  copy++;
                  dst2 = &ld->reverse_env->host_name;
                }
              if (expand_expr (s0or1, ld->lang) != 0)
                {
                  err_expand (open_uumrc_filenm, s0or1);
                }
              copy++;
              dst1 = &ld->reverse_env->envrc_name;
              src1 = s0or1;
              strcpy (ld->reverse_env->env_name_str, s[0]);
            }
        }
      else if (strcmp (code, "waking_up_in_henkan_mode") == 0)
        {
          if (!(defined_by_option & OPT_WAKING_UP_MODE))
            {
              henkan_off_def = 0;
            }
        }
      else if (strcmp (code, "waking_up_no_henkan_mode") == 0)
        {
          if (!(defined_by_option & OPT_WAKING_UP_MODE))
            {
              henkan_off_def = 1;
            }
        }
      else if (strcmp (code, "simple_delete") == 0)
        {
          excellent_delete = 0;
        }
      else if (strcmp (code, "excellent_delete") == 0)
        {
          excellent_delete = 1;
        }
      else if (strcmp (code, "send_ascii_char") == 0)
        {
          send_ascii_char_def = 1;
        }
      else if (strcmp (code, "not_send_ascii_char") == 0)
        {
          send_ascii_char_def = 0;
        }
      else if (strcmp (code, "setmaxchg") == 0)
        {
          if (change_ascii_to_int (s[0], &d1) != -1)
            {
              maxchg = (d1 <= 0) ? maxchg : d1;
            }
        }
      else if (strcmp (code, "setmaxbunsetsu") == 0)
        {
          if (num >= 2)
            {
              if (change_ascii_to_int (s[0], &d1) != -1)
                {
                  maxbunsetsu = (d1 <= 0) ? maxbunsetsu : d1;
                }
            }
        }
      else if (strcmp (code, "setmaxhistory") == 0)
        {
          if (num >= 2)
            {
              if (change_ascii_to_int (s[0], &d1) != -1)
                {
                  max_history = (d1 <= 0) ? max_history : d1 + 1;
                }
            }
        }
      else if (strcmp (code, "setjishopath") == 0)
        {
          if (num > 1)
            {
              if (expand_expr (s[0], ld->lang) != 0)
                {
                  err_expand (open_uumrc_filenm, s[0]);
                }
              copy++;
              dst1 = &ld->jishopath;
              src1 = s[0];
            }
        }
      else if (strcmp (code, "sethindopath") == 0)
        {
          if (num > 1)
            {
              if (expand_expr (s[0], ld->lang) != 0)
                {
                  err_expand (open_uumrc_filenm, s[0]);
                }
              copy++;
              dst1 = &ld->hindopath;
              src1 = s[0];
            }
        }
      else if (strcmp (code, "setfuzokugopath") == 0)
        {
          if (num > 1)
            {
              if (expand_expr (s[0], ld->lang) != 0)
                {
                  err_expand (open_uumrc_filenm, s[0]);
                }
              copy++;
              dst1 = &ld->fuzokugopath;
              src1 = s[0];
            }
        }
      else if (strcmp (code, "touroku_comment") == 0)
        {
          touroku_comment = 1;
        }
      else if (strcmp (code, "touroku_no_comment") == 0)
        {
          touroku_comment = 0;
        }
      else if (strcmp (code, "henkan_on_kuten") == 0)
        {
          henkan_on_kuten = 1;
        }
      else if (strcmp (code, "henkan_off_kuten") == 0)
        {
          henkan_on_kuten = 0;
          /*
           * unnessesary in xim
           } else if(strcmp(code , "setkanaromenv") == 0) {
           } else if(strcmp(code , "flow_control_on") == 0) {
           } else if(strcmp(code , "flow_control_off") == 0) {
           } else if(strcmp(code , "convkey_always_on") == 0) {
           } else if(strcmp(code , "convkey_not_always_on") == 0) {
           } else if (strcmp(code, "setmaxichirankosu") == 0) {
           } else if(strcmp(code, "remove_cs") == 0) {
           } else if(strcmp(code, "not_remove_cs") == 0) {
           */
        }
      else
        {
        }
    }
  if (copy)
    {
      if (copy > 1)
        {
          if (!(*dst2 = alloc_and_copy (src2)))
            return (-1);
        }
      if (!(*dst1 = alloc_and_copy (src1)))
        return (-1);
    }
  return (0);
ERROR_RET:
  free_env (ld->normal_env);
  free_env (ld->reverse_env);
#define check_and_free(s) if (s) {Free(s); s = NULL;}
  check_and_free (ld->uumkey_name);
  check_and_free (ld->jishopath);
  check_and_free (ld->hindopath);
  check_and_free (ld->fuzokugopath);
#undef check_and_free
  return (-1);
}

static int
copy_wnn_env (src, dst)
     register WnnEnv *src, *dst;
{
  if (src->host_name && !(dst->host_name = alloc_and_copy (src->host_name)))
    return (-1);
  dst->envrc_name = src->envrc_name;
  strcpy (dst->env_name_str, src->env_name_str);
  dst->sticky = src->sticky;
  return (0);
}

static int
copy_lang_db (ld)
     register XIMLangDataBase *ld;
{
  register WnnEnv *p, *pp;

  for (p = ld->normal_env; p; p = p->next)
    {
      if (get_new_env (c_c, NULL, 0) < 0)
        goto ERROR_RET;
      pp = c_c->cur_normal_env;
      if (copy_wnn_env (p, pp) < 0)
        goto ERROR_RET;
    }
  for (p = ld->reverse_env; p; p = p->next)
    {
      if (get_new_env (c_c, NULL, 1) < 0)
        goto ERROR_RET;
      pp = c_c->cur_reverse_env;
      if (copy_wnn_env (p, pp) < 0)
        goto ERROR_RET;
    }
  send_ascii_char = send_ascii_char_def;
  return (0);
ERROR_RET:
  free_env (c_c->normal_env);
  free_env (c_c->reverse_env);
  return (-1);
}

/*
 * read uumrc file 
 */
int
uumrc_get_entries (ld, rk_pre_load, root)
     XIMLangDataBase *ld;
     int rk_pre_load, root;
{
  FILE *fp = NULL;
  char data[1024];
  int err = 0;

  if (ld->read)
    {
      if (copy_lang_db (ld) < 0)
        return (-1);
      return (0);
    }
  /*
   * default setting
   */
  ld->m_chg = MAXCHG;
  ld->m_bunsetsu = MAXBUNSETSU;
  ld->m_history = MAX_HISTORY + 1;

  if (ld->uumrc_name && *ld->uumrc_name)
    {
      strcpy (open_uumrc_filenm, ld->uumrc_name);
      if ((fp = fopen (open_uumrc_filenm, "r")) == NULL)
        {
          print_out2 ("I could not open specified uumrc file \"%s\" for lang \"%s\".", open_uumrc_filenm, ld->lang);
          err = 1;
        }
    }
  if (fp == NULL)
    {                           /* Open default uumrc */
      if (strcpy (open_uumrc_filenm, LIBDIR), strcat (open_uumrc_filenm, "/"), strcat (open_uumrc_filenm, ld->lang), strcat (open_uumrc_filenm, RCFILE), (fp = fopen (open_uumrc_filenm, "r")) != NULL)
        {
          if (err)
            {
              Free (ld->uumrc_name);
            }
          if (rkfile_defined_by_option && (rk_pre_load || root) && ld == cur_lang)
            {
              if (!(ld->rkfile_name = alloc_and_copy (root_rkfilename)))
                return (-1);
            }
          if (!(ld->uumrc_name = alloc_and_copy (open_uumrc_filenm)))
            return (-1);
        }
      else
        {
          print_out1 ("I could not open a default uumrc file \"%s\".", open_uumrc_filenm);
          return (-1);
        }
    }

  while (fgets (data, 1024, fp) != NULL)
    {
      if (uumrc_set_entry (data, ld, rk_pre_load, root) == -1)
        {
          fclose (fp);
          return (-1);
        }
    }
  fclose (fp);
  if (!rk_pre_load)
    {
      ld->read = 1;
      if (copy_lang_db (ld) < 0)
        return (-1);
    }
  set_cswidth (ld->cswidth_id);
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
  return (0);
}

int
read_default_rk ()
{
  register ReadRkfileRec *rr;
  register XIMLangDataBase *ld;

  for (rr = read_rkfile_list; rr != NULL; rr = rr->next)
    {
      for (ld = language_db; ld; ld = ld->next)
        {
          if (!strcmp (ld->lang, rr->name))
            break;
        }
      if (!ld)
        {
          print_out1 ("In ximrc, I don't support such language \"%s\".", rr->name);
          return (-1);
        }
      if (uumrc_get_entries (ld, 1, 0) == -1)
        {
          return (-1);
        }
    }
  return (0);
}
