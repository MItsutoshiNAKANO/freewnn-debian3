/*
 * $Id: readximrc.c,v 1.8.2.1 1999/02/08 08:08:54 yamasita Exp $
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
#include "config.h"
#include "ext.h"

/** Set parameter by reading ximrc file */
static int
read_xim_rcfile (filenm, expand_filenm)
     char *filenm;
     int (*expand_filenm) ();
{
  char buf[BUFSIZ];
  char entrynm[64];
  char s[2][MAXPATHLEN + 1];
  int n;
  int error;
  FILE *fp;
  XIMLangDataBase *ld;
  extern int fclose ();

  if ((fp = fopen (filenm, "r")) == NULL)
    {
      return (-1);
    }

  while (fgets (buf, BUFSIZ, fp))
    {
      if (comment_char (*buf) || (n = sscanf (buf, "%s %s %s", entrynm, s[0], s[1])) < 1)
        {
          continue;
        }
      error = 0;
      switch (*(entrynm + 3))
        {
        case 'l':
          if (!strcmp (entrynm, "preloadrkfile"))
            {
              if (n > 1)
                {
                  register ReadRkfileRec *rr, *p, *prev;
                  if (!(rr = (ReadRkfileList) Malloc (sizeof (ReadRkfileRec))))
                    goto CLOSE_RET;
                  if (read_rkfile_list == NULL)
                    {
                      read_rkfile_list = rr;
                    }
                  else
                    {
                      for (p = read_rkfile_list, prev = NULL; p != NULL; prev = p, p = p->next);
                      prev->next = rr;
                    }
                  if (!(rr->name = alloc_and_copy (s[0])))
                    goto CLOSE_RET;
                  rr->next = NULL;
                }
              else
                error = 1;
            }
          else
            error = 2;
          break;
        case 'u':
          if (!strcmp (entrynm, "setuumrc"))
            {
              if (n > 2)
                {
                  for (ld = language_db; ld; ld = ld->next)
                    {
                      if (!strcmp (ld->lang, s[0]))
                        break;
                    }
                  if (!ld)
                    {
                      print_out2 ("In ximrc \"%s\", I don't support the lang \"%s\".", filenm, s[0]);
                    }
                  if ((*expand_filenm) (s[1]) != 0)
                    {
                      print_out1 ("In ximrc \"%s\", I could not expand %s.", s[1]);
                    }
                  if (!(ld->uumrc_name = alloc_and_copy (s[1])))
                    goto CLOSE_RET;
                  if (rkfile_defined_by_option && cur_lang == ld)
                    {
                      if (!(ld->rkfile_name = alloc_and_copy (root_rkfilename)))
                        goto CLOSE_RET;
                    }
                }
              else
                error = 1;
            }
          else
            error = 2;
          break;
        case 'b':
          if (!strcmp (entrynm, "setbackspacechar"))
            {
              if (n > 1)
                {
                  rubout_code = s[0][0];
                }
              else
                error = 1;
            }
          else
            error = 2;
          break;
        default:
          error = 2;
          break;
        }
      if (error == 1)
        {
          print_out2 ("In ximrc file \"%s\", I found a unknown entry name \"%s\".", filenm, entrynm);
        }
      else if (error == 2)
        {
          print_out2 ("In ximrc file \"%s\", I found a Illegal line %d.", filenm, entrynm);
        }
    }
  fclose (fp);
  return (0);
CLOSE_RET:
  fclose (fp);
  return (-2);
}

int
read_ximrc ()
{
  register char *n;
  char tmp_xim_rc_file[MAXPATHLEN + 1];
  register int ret;
  extern char *getenv ();

  if (ximrc_file)
    {
      n = ximrc_file;
    }
  else
    {
      n = getenv (XIM_RCENV);
    }
  if (n && *n)
    {
      strcpy (tmp_xim_rc_file, n);
      if ((*expand_expr) (tmp_xim_rc_file) != 0)
        {
          print_out1 ("I could not expand %s.", tmp_xim_rc_file);
        }
      if ((ret = read_xim_rcfile (tmp_xim_rc_file, expand_expr)) == 0)
        {
          goto OK_RET;
        }
      else if (ret == -2)
        return (-1);
      print_out1 ("Can't open a specified ximrc \"%s\".", tmp_xim_rc_file);
      ximrc_file = NULL;
    }
  if ((n = getenv ("HOME")) && *n)
    {
      strcat (strcpy (tmp_xim_rc_file, n), USR_XIMRC);
      if ((ret = read_xim_rcfile (tmp_xim_rc_file, expand_expr)) == 0)
        {
          goto OK_RET;
        }
      else if (ret == -2)
        return (-1);
      ximrc_file = NULL;
    }
  strcat (strcpy (tmp_xim_rc_file, LIBDIR), XIMRCFILE);
  if (read_xim_rcfile (tmp_xim_rc_file, expand_expr) >= 0)
    {
      goto OK_RET;
    }
  print_out1 ("Can't open a default ximrc file \"%s\".", tmp_xim_rc_file);
  ximrc_file = NULL;
  return (-1);
OK_RET:
  if (!(ximrc_file = alloc_and_copy (tmp_xim_rc_file)))
    return (-1);
  return (0);
}

static void
default_set_of_ld (ld)
     register XIMLangDataBase *ld;
{
  ld->read = 0;
  ld->uumkey_name = NULL;
  ld->host_name = NULL;
  ld->rev_host_name = NULL;
  ld->rk_table = NULL;
  ld->jishopath = NULL;
  ld->hindopath = NULL;
  ld->fuzokugopath = NULL;
  ld->h_off_def = 0;
  ld->e_delete = 1;
  ld->s_ascii_char_def = 0;
  ld->m_chg = 0;
  ld->m_bunsetsu = 0;
  ld->m_history = 0;
  ld->t_comment = 0;
  ld->h_on_kuten = 0;
#ifdef  USING_XJUTIL
  ld->xjutil_act = 0;
  ld->xjutil_id = (Window) 0;
  ld->xjutil_pid = 0;
  ld->xjutil_use = NULL;
#endif /* USING_XJUTIL */
}

int
read_ximconf ()
{
  char buf[BUFSIZ];
  char filenm[MAXPATHLEN + 1];
  char s[6][128];
  register int n, i, lc_cnt = 0, all_size;
  FILE *fp;
  register char *p, *ptr, *ret;
  FunctionTable *f;
  XIMLangDataBase *ld;
  XIMLcNameRec *lnl, *lnl_p;

  strcpy (filenm, LIBDIR);
  strcat (filenm, XIM_CONFIG_FILE);
  if ((fp = fopen (filenm, "r")) == NULL)
    {
      print_out1 ("Can't open xim.conf \"%s\".", filenm);
      return (-1);
    }

  while (1)
    {
      ptr = buf;
      while (1)
        {
          if (!(ret = fgets (ptr, BUFSIZ, fp)))
            {
              break;
            }
          if (*ptr == '\n')
            continue;
          if ((i = strlen (ptr)) > 1 && ptr[i - 2] == '\\')
            {
              ptr += i - 2;
              continue;
            }
          break;
        }
      if (!ret)
        break;
      if (comment_char (*buf) || (n = sscanf (buf, "%s %s %s %s %s %s", s[0], s[1], s[2], s[3], s[4], s[5])) < 1)
        {
          continue;
        }
      if (!strcmp (s[0], "setdefaultlocale"))
        {
          if (n < 2)
            goto ARG_ERR;
          if (!(def_locale = alloc_and_copy (s[1])))
            goto CLOSE_RET;

        }
      else if (!strcmp (s[0], "setworldlocale"))
        {
          if (n < 2)
            goto ARG_ERR;
          if (!(world_locale = alloc_and_copy (s[1])))
            goto CLOSE_RET;
        }
      else
        {
          if (n < 6)
            goto ARG_ERR;
          all_size = sizeof (XIMLangDataBase) + strlen (s[0]) + strlen (s[1]) + strlen (s[2]) + 3;
          if (!(p = (char *) Malloc (all_size)))
            {
              malloc_error ("allocation of the initial area");
              goto CLOSE_RET;
            }
          bzero (p, all_size);
          ld = (XIMLangDataBase *) p;
          p += sizeof (XIMLangDataBase);
          strcpy (p, s[0]);
          ptr = p;
          lc_cnt = 0;
          do
            {
              if (lc_cnt)
                {
                  *ptr++ = '\0';
                }
              lc_cnt++;
            }
          while (*ptr && (ptr = index (ptr, '|')));
          if (!(lnl_p = (XIMLcNameRec *) Malloc (sizeof (XIMLcNameRec) * lc_cnt)))
            {
              malloc_error ("allocation of the initial area");
              goto CLOSE_RET;
            }
          ptr = p;
          for (i = 0; i < lc_cnt; i++)
            {
              for (lnl = lc_name_list; lnl; lnl = lnl->next)
                {
                  if (!strcmp (lnl->lc_name, ptr))
                    {
                      print_out2 ("In xim.conf file \"%s\", locale \"%s\" is redeclared.", filenm, ptr);
                      goto CLOSE_RET;
                    }
                }
              lnl_p[i].lc_name = ptr;
              ptr += strlen (ptr) + 1;
              lnl_p[i].lang_db = ld;
              if (i != 0)
                {
                  lnl_p[i].next = &lnl_p[i - 1];
                }
            }
          lnl_p[0].next = lc_name_list;
          lc_name_list = &lnl_p[lc_cnt - 1];

          p += strlen (s[0]) + 1;
          ld->lang = (char *) p;
          strcpy (ld->lang, s[1]);

          p += strlen (s[1]) + 1;
          ld->lc_name = (char *) p;
          strcpy (ld->lc_name, s[2]);

          if ((ld->cswidth_id = create_cswidth (s[3])) == 0)
            {
              print_out2 ("In xim.conf file \"%s\", \"%s\" is a bad string for CSWIDTH.", filenm, s[3]);
              goto CLOSE_RET;
            }
          if (!strcmp (s[4], "True") || !strcmp (s[4], "TRUE"))
            {
              ld->connect_serv = True;
            }
          else
            {
              ld->connect_serv = False;
            }
          ld->f_table = (FunctionTable *) NULL;
          for (f = function_db, i = 0; f[i].name; i++)
            {
              if (!strcmp (s[5], f[i].name))
                {
                  ld->f_table = &(f[i]);
                  break;
                }
            }
          if (!ld->f_table)
            {
              print_out2 ("In xim.conf file \"%s\", \"%s\" is a bad string for functions.", filenm, s[5]);
              goto CLOSE_RET;
            }
          if (language_db)
            {
              ld->next = language_db;
            }
          else
            {
              ld->next = NULL;
            }
          language_db = ld;
          default_set_of_ld (ld);
        }
    }
  fclose (fp);
  if (!def_locale)
    if (!(def_locale = alloc_and_copy (DEFAULT_LANG)))
      return (-1);
  if (!world_locale)
    if (!(world_locale = alloc_and_copy (DEFAULT_WORLD_LOCALE)))
      return (-1);
  return (0);
ARG_ERR:
  print_out2 ("In xim.conf file \"%s\", too few arguments at line \"%s\".", filenm, buf);
CLOSE_RET:
  fclose (fp);
  return (-1);
}
