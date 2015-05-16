/*
 *  $Id: prologue.c $
 */

/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright FreeWnn Project 1999, 2000
 *
 * Maintainer:  FreeWnn Project   <freewnn@tomo.gr.jp>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* Standard Interface
 *    Initialize Routine
 */

#include <stdio.h>
#include <ctype.h>
#include <sys/errno.h>
#include <pwd.h>
#include "jllib.h"
#include "commonhd.h"
#include "wnn_config.h"
#include "buffer.h"
#include "rk_spclval.h"
#include "sdefine.h"
#include "sheader.h"
#include "wnn_os.h"
#ifdef UX386
#include <unistd.h>
#endif

#undef putchar
extern int putchar ();
extern struct passwd *getpwnam ();

extern char *getenv ();
extern FILE *fopen ();




/** romkan のイニシャライズ */
int
open_romkan ()
{
  char name[PATHNAMELEN];
  int rk_option_flg;

  romkan_set_lang (lang_dir);
  if (*rkfile_name_in_uumrc != '\0')
    {
      strcpy (name, rkfile_name_in_uumrc);
    }
  else
    {
      strcpy (name, LIBDIR);
      strcat (name, RKFILE);
    }
  rk_option_flg = RK_REDRAW | RK_CHMOUT | (excellent_delete ? 0 : RK_SIMPLD) | (verbose_option ? RK_VERBOS : 0);

  return (romkan_init5 (name, rubout_code, NULL, NULL, rk_option_flg) == 0 ? 0 : -1);
  /* romaji-kana henkan junbi */
}


/** 漢字かな変換用 envrc ファイルのオープン */
int
get_envrc_name_reverse (str)
     char *str;
{
  if (cur_reverse_env == NULL || *reverse_envrcname == 0)
    return (0);
  strcpy (str, reverse_envrcname);
  if (access (str, R_OK) == -1)
    {
      return (-1);
    }
  return (1);
}


/** envrc ファイルのオープン */
int
get_envrc_name (str)
     char *str;
{
  if (cur_normal_env == NULL || envrcname[0] == '\0')
    {
      strcpy (str, LIBDIR);
      strcat (str, "/");
      strcat (str, lang_dir);
      strcat (str, ENVRCFILE);
    }
  else
    {
      strcpy (str, envrcname);
    }
  return (1);
}

/* uum で必要なエリアをアロケートする。*/
int
allocate_areas ()
{
  char *malloc ();
  char *area_start;
  char *area_pter;

  int k;
  int total_size;
  int len1 = maxchg * sizeof (w_char);
  int return_buf_len = len1 * 7;

  total_size = return_buf_len + len1 * 3 + maxbunsetsu * sizeof (int) * 3 + maxbunsetsu * sizeof (struct wnn_env *);

  if ((area_start = malloc (total_size)) == NULL)
    {
      /*
         fprintf(stderr,"初期化でmalloc不能。\n");
       */
      fprintf (stderr, MSG_GET (2));
      return (-1);
    }

  area_pter = area_start;
  return_buf = (w_char *) area_pter;    /* mojiretsu -> return_buf */
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
  area_pter += maxbunsetsu * sizeof (int);
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


/* henkan initialize */
/** uum のイニシャライズ */
int
init_uum ()
{
  char *p;

  uumrc_get_entries ();

  if (*(p = convkey_name_in_uumrc) == '\0')
    p = NULL;
  if (convert_key_setup (p, (verbose_option != 0)) == -1)
    return (-1);

  if (init_key_table () == -1)
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

  if (init_history () == -1)
    {
      return (-1);
    }

  connect_jserver (0);

  throw_cur_raw (0, crow + conv_lines);
  if (keypad_fun)
    set_keypad_on ();
  scroll_up ();
  set_scroll_region (0, crow - 1);
  throw_cur_raw (0, crow - 1);
  flush ();

  return (0);
}

static void
puts1 (s)
     char *s;
{
  printf ("%s\n", s);
  flush ();
}

static void
puts2 (s)
     char *s;
{
  throw_c (0);
  clr_line ();
  printf ("%s", s);
  flush ();
}

static int
yes_or_no_init (s)
     char *s;
{
  for (;;)
    {
      char x[256];
      printf (s);
      flush ();
      if (fgets (x, 256, stdin) == NULL)
        {
          return (0);
        }
      if (*x == 'n' || *x == 'N')
        return (0);
      if (*x == 'y' || *x == 'Y')
        return (1);
    }
}

/** Connecting to jserver */
int
connect_jserver (first)
     int first;
{
  char uumrc_name[PATHNAMELEN];
  char environment[PATHNAMELEN];
  WnnEnv *p;
  WnnEnv *save_cur_normal_env = NULL, *save_cur_reverse_env = NULL;
  int (*yes_no_func) ();
  void (*puts_func) ();
  extern char *malloc ();

  if (first == 0)
    {
      bun_data_ = NULL;
      yes_no_func = yes_or_no_init;
      puts_func = puts1;
    }
  else
    {
      yes_no_func = yes_or_no;
      puts_func = puts2;
    }
  get_envrc_name (uumrc_name);  /* use username for env-name */

  if (cur_normal_env)
    save_cur_normal_env = cur_normal_env;
  for (p = normal_env; p; p = p->next)
    {
      cur_normal_env = p;
      get_envrc_name (uumrc_name);
      strcpy (environment, username);
      strcat (environment, p->env_name_str);
      if (servername == NULL && def_servername && *def_servername)
        {
          servername = malloc (strlen (def_servername) + 1);
          strcpy (servername, def_servername);
        }
      if (bun_data_ == NULL)
        {
          bun_data_ = jl_open_lang (environment, servername, lang_dir, uumrc_name, yes_no_func, puts_func, WNN_TIMEOUT);
          if (bun_data_ != NULL && bun_data_->env != NULL)
            {
              env_normal = bun_data_->env;
            }
        }
      else if (env_normal == 0 || jl_isconnect_e (env_normal) == 0)
        {
          env_normal = jl_connect_lang (environment, servername, lang_dir, uumrc_name, yes_no_func, puts_func, WNN_TIMEOUT);
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
      cur_normal_env = save_cur_normal_env;
      if (env_normal)
        jl_env_set (bun_data_, env_normal);
    }
  if (normal_env == NULL)
    {
      get_new_env (0);
      cur_normal_env = normal_env;
      uumrc_name[0] = '\0';
      bun_data_ = jl_open_lang (username, servername, lang_dir, uumrc_name, yes_no_func, puts_func, WNN_TIMEOUT);
    }

  if (cur_reverse_env)
    save_cur_reverse_env = cur_reverse_env;
  for (p = reverse_env; p; p = p->next)
    {
      cur_reverse_env = p;
      if (get_envrc_name_reverse (uumrc_name) > 0)
        {
          strcpy (environment, username);
          strcat (environment, p->env_name_str);
          strcat (environment, "R");
          if (reverse_servername == NULL && def_reverse_servername && *def_reverse_servername)
            {
              reverse_servername = malloc (strlen (def_reverse_servername) + 1);
              strcpy (reverse_servername, def_reverse_servername);
            }
          env_reverse = jl_connect_lang (environment, reverse_servername, lang_dir, uumrc_name, yes_no_func, puts_func, WNN_TIMEOUT);
          if (env_reverse)
            {
              if (reverse_sticky)
                jl_env_sticky_e (env_reverse);
              if (!reverse_sticky)
                jl_env_un_sticky_e (env_reverse);
            }
        }
    }
  if (save_cur_reverse_env)
    cur_reverse_env = save_cur_reverse_env;
  return (0);
}


/** 立ち上がり時にメッセージファイルが存在すればそれを表示する。*/
int
initial_message_out ()
{
  FILE *fp;
  char data[1024];
  char fname[PATHNAMELEN];

  strcpy (fname, LIBDIR);
  strcat (fname, MESSAGEFILE);

  if ((fp = fopen (fname, "r")) != NULL)
    {
      while (fgets (data, 1024, fp) != NULL)
        {
          fprintf (stderr, "\r%s", data);
        }
      fclose (fp);
      fprintf (stderr, "\r\n");
    }
  return (1);
}
