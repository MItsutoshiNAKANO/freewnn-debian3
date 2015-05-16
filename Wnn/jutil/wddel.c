/*
 *  $Id: wddel.c,v 1.8 2002/05/12 22:51:17 hiroo Exp $
 */

/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright FreeWnn Project 1999, 2000, 2002
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

/*
 * UJIS ファイルを読みとって、単語登録を行なう。
 */

#ifndef lint
static char *rcs_id = "$Id: wddel.c,v 1.8 2002/05/12 22:51:17 hiroo Exp $";
#endif /* lint */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#if STDC_HEADERS
#  include <stdlib.h>
#else
#  if HAVE_MALLOC_H
#    include <malloc.h>
#  endif
#endif /* STDC_HEADERS */
#if HAVE_UNISTD_H
#  include <unistd.h>
#endif

#include "commonhd.h"
#include "wnn_config.h"
#include "jllib.h"
#include "jslib.h"
#include "wnn_string.h"
#include "wnn_os.h"

#define WORD_DELETE 1
#define COMMENT_SET 2
#define HINDO_SET 3

#define LINE_SIZE 256

char *def_server;

int dic_no = -1;
char *server_n;
char *env_n = "wddel";
char *fname = NULL;
int fid;
int client = 0;                 /* server site dict */
int what = WORD_DELETE;

WNN_JSERVER_ID *js;
struct wnn_env *env, *rev_env;
static struct wnn_ret_buf rb = { 0, NULL };

static void
usage ()
{
  fprintf (stderr, "wddel [-D server_name] [-n env_name][-d dic_no][-L][-E][-H][-C] [file_name]  < text dict\n");
  fprintf (stderr, "file_name or -d dic_no must be specified\n");
  fprintf (stderr, "default env_name = wddel\n");
  fprintf (stderr, "default server_name = %s\n", def_server);
  fprintf (stderr, "L is to specify that the file at the client site.\n");
  fprintf (stderr, "-E is word_delete, -C is word_comment_set, -H is hindo_set.\n");
  exit (1);
}

static void
err ()
{
  printf ((char *) wnn_perror ());
  printf ("\n bye.\n");
  exit (1);
}

int
main (argc, argv)
     int argc;
     char **argv;
{
  extern char *getenv ();
  extern int optind;
  extern char *optarg;
  struct wnn_dic_info *info;
  int c;
  int k;
  int num;
  int sno;
  char s[LINE_SIZE];
  char *cswidth_name;
  extern char *get_cswidth_name ();
  extern void set_cswidth ();

  if (getenv (WNN_DEF_SERVER_ENV))
    {
      def_server = getenv (WNN_DEF_SERVER_ENV);
    }
  else
    {
      def_server = "";
    }
  server_n = def_server;

  if (cswidth_name = get_cswidth_name (WNN_DEFAULT_LANG))
    set_cswidth (create_cswidth (cswidth_name));
  while ((c = getopt (argc, argv, "D:n:d:CHEL")) != EOF)
    {
      switch (c)
        {
        case 'D':
          server_n = optarg;
          break;
        case 'n':
          env_n = optarg;
          break;
        case 'd':
          dic_no = atoi (optarg);
          break;
        case 'C':
          what = COMMENT_SET;
          break;
        case 'H':
          what = HINDO_SET;
          break;
        case 'E':
          what = WORD_DELETE;
          break;
        case 'L':
          client = 1;
          break;
        default:
          usage ();
          break;
        }
    }
  if (optind)
    {
      optind--;
      argc -= optind;
      argv += optind;
    }
  if (argc > 1)
    {
      fname = argv[1];
    }
  if (fname && dic_no != -1)
    usage ();
  if (!fname && dic_no == -1)
    usage ();


  rb.buf = (char *) malloc ((unsigned) (rb.size = 0));

  if ((js = js_open (server_n, WNN_TIMEOUT)) == NULL)
    err ();
  if ((env = js_connect (js, env_n)) == NULL)
    err ();
  if (fname)
    {
      if (client)
        {
          if ((fid = js_file_send (env, fname)) == -1)
            err ();
        }
      else
        {
          if ((fid = js_file_read (env, fname)) == -1)
            err ();
        }
      if ((dic_no = js_dic_add (env, fid, -1, 0, 0, WNN_DIC_RW, WNN_DIC_RW, "", "")) == -1)
        err ();
    }

  if ((num = js_dic_list (env, &rb)) == -1)
    err ();

  info = (struct wnn_dic_info *) (rb.buf);
  for (k = 0; k < num; k++)
    {
      if (info[k].dic_no == dic_no)
        break;
    }
  if (k == num)
    {
      /*
         fprintf(stderr, "指定された番号の辞書は、環境に存在しません。\n");
       */
      fprintf (stderr, "The specified dictionary isn't exist in current environment\n");
      exit (1);
    }
  if (info[k].type != WNN_UD_DICT && info[k].type != WNN_REV_DICT)
    {
      /*
         fprintf(stderr, "指定された番号の辞書は、登録可能ではありません。\n");
       */
      fprintf (stderr, "The specified dictionary isn't registable\n");
      exit (1);
    }
  while (fgets (s, sizeof (s), stdin))
    {
      char com[LENGTHYOMI];
      char Com[LENGTHYOMI];
      int ima, hindo;
      if (s[0] == '\\')
        continue;
      switch (what)
        {
        case WORD_DELETE:
          if (sscanf (s, "%d", &sno) <= 0)
            {
              fprintf (stderr, "Bad line \"%s\"", s);
              continue;
            }
          if (js_word_delete (env, dic_no, sno) == -1)
            {
              fprintf (stderr, "serial-no = %d\n", sno);
              err ();
            }
          break;
        case COMMENT_SET:
          if (sscanf (s, "%d %s", &sno, com) <= 1)
            {
              fprintf (stderr, "Bad line \"%s\"", s);
              continue;
            }
          wnn_Sstrcpy (Com, com);
          if (js_word_comment_set (env, dic_no, sno, Com) == -1)
            {
              fprintf (stderr, "serial-no = %d\n", sno);
              err ();
            }
          break;
        case HINDO_SET:
          if (sscanf (s, "%d %n %n", &sno, &ima, &hindo) <= 2)
            {
              fprintf (stderr, "Bad line \"%s\"", s);
              continue;
            }
          if (js_hindo_set (env, dic_no, sno, ima, hindo) == -1)
            {
              fprintf (stderr, "serial-no = %d\n", sno);
              err ();
            }
          break;
        }
    }
  if (fname)
    {
      if (client)
        {
          if (js_file_receive (env, fid, "") == -1)
            err ();
        }
      else
        {
          if (js_file_write (env, fid, "") == -1)
            err ();
        }
      fprintf (stderr, "Wrote the file back.\n");
    }
  exit (0);
}
