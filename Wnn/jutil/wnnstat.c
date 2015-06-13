/*
 *  $Id: wnnstat.c,v 1.12 2015/05/09 23:42:04 itisango Exp $
 */

/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright FreeWnn Project 1999, 2000, 2002, 2015
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

#ifndef lint
static char *rcs_id = "$Id: wnnstat.c,v 1.12 2015/05/09 23:42:04 itisango Exp $";
#endif /* lint */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#if STDC_HEADERS
#  include <stdlib.h>
#  include <string.h>
#else /* !STDC_HEADERS */
#  if HAVE_MALLOC_H
#    include <malloc.h>
#  endif
#  if HAVE_STRINGS_H
#    include <strings.h>
#  endif
#endif /* !STDC_HEADERS */
#if HAVE_UNISTD_H
#  include <unistd.h>
#endif
#include <stdarg.h>

#include "jslib.h"
#include "jllib.h"
#include "jd_sock.h"
#include "commonhd.h"
#include "wnn_config.h"
#include "wnn_os.h"
#include "msg.h"

#include "jlib_private.h"
#include "etc.h"

WNN_JSERVER_ID *js;
struct wnn_ret_buf rb = { 0, NULL };
#define WNNSTAT_ENV "This_is_wnnstat"

#define W_WHO 0
#define W_ENV 1
#define W_ALL 2
#define W_FILE 3
#define W_DIC 4
#define W_DIC_ALL 5
#define W_VERSION 6

extern int optind;
extern char *optarg;
extern char *getenv ();

#ifdef JAPANESE
extern int eujis_to_jis8 (), eujis_to_sjis ();
#endif
#ifdef CHINESE
extern int ecns_to_big5 ();
#endif
static void  err (), usage (), dic (), dic_all (), file_all ();
static void printall FRWNN_PARAMS((WNN_ENV_INFO *, int));
static void printpat FRWNN_PARAMS((WNN_ENV_INFO *, int));
static void wsttost FRWNN_PARAMS((char *, w_char *));

static void who FRWNN_PARAMS((char *lang));
static void  out FRWNN_PARAMS((const char *, ...));


#ifdef  CHINESE
#ifdef  TAIWANESE
int ocode = (TTY_TCODE + 6);
#else /* TAIWANESE */
int ocode = (TTY_CCODE + 4);
#endif /* TAIWANESE */
#else /* CHINESE */
#ifdef  KOREAN
int ocode = TTY_HCODE;
#else /* KOREAN */
int ocode = TTY_KCODE;
#endif /* KOREAN */
#endif /* CHINESE */

int com = W_WHO;

struct msg_cat *cd;

int
main (argc, argv)
     int argc;
     char **argv;
{
  char *serv;
  int c, i, j;
  WNN_ENV_INFO *w;
  static char lang[64] = { 0 };
  char nlspath[64];
/*
    char *p;
*/
  char *server_env = NULL;
  char *prog = argv[0];
  extern char *get_server_env ();

/*
    if ((p = getenv("LANG")) != NULL) {
        strcpy(lang, p);
        lang[5] = '\0';
    } else {
        lang[0] = '\0';
    }

    if (*lang == '\0')
*/
  strcpy (lang, WNN_DEFAULT_LANG);


#ifdef JAPANESE
#  define OPTSTRING "weEdDfFUSJVL:"
#endif
#ifdef  CHINESE
#  define OPTSTRING "weEdDfFUBCVL:"
#endif
# ifdef KOREAN
#  define OPTSTRING "weEdDfFUKVL:"
#endif


  while ((c = getopt (argc, argv, OPTSTRING)) != EOF)
    {
      switch (c)
        {
        case 'w':
          com = W_WHO;
          break;
        case 'E':
          com = W_ALL;
          break;
        case 'e':
          com = W_ENV;
          break;
        case 'd':
          com = W_DIC;
          break;
        case 'D':
          com = W_DIC_ALL;
          break;
        case 'f':
        case 'F':
          com = W_FILE;
          break;
        case 'V':
          com = W_VERSION;
          break;
        case 'U':
#ifdef JAPANESE
          ocode = J_EUJIS;
#endif
#ifdef CHINESE
          ocode = C_EUGB;
#endif
#ifdef KOREAN
          ocode = K_EUKSC;
          break;
        case 'K':
          ocode = K_KSC;
#endif
          break;
#ifdef JAPANESE
        case 'J':
          ocode = J_JIS;
          break;
        case 'S':
          ocode = J_SJIS;
          break;
#endif
#ifdef  CHINESE
        case 'B':
          ocode = C_BIG5 + 6;
          break;
        case 'C':
          ocode = C_ECNS11643 + 6;
          break;
#endif /* CHINESE */
        case 'L':
          strcpy (lang, optarg);
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
  strcpy (nlspath, LIBDIR);
  strcat (nlspath, "/%L/%N");
  cd = msg_open ("wnnstat.msg", nlspath, lang);

  rb.buf = (char *) malloc ((unsigned) (rb.size = 0));

  if ((server_env = get_server_env (lang)) == NULL)
    {
      server_env = WNN_DEF_SERVER_ENV;
    }
  if (argc > 1)
    {
      serv = argv[1];
    }
  else if (!(serv = getenv (server_env)))
    {
      serv = "";
    }

  if (!*serv)
    {
      if (serv = _wnn_get_machine_of_serv_defs (lang))
        {
          if ((js = js_open_lang (serv, lang, WNN_TIMEOUT)) == NULL)
            {
              serv = "";
            }
        }
    }
  if (js == NULL && (js = js_open_lang (serv, lang, WNN_TIMEOUT)) == NULL)
    {
      out ("%s:", prog);
      if (serv && *serv)
        out ("%s", serv);
      out ("%s", wnn_perror_lang (lang));
      putchar ('\n');
/*      out(" jserver と接続出来ません。\n"); */
      exit (-1);
    }
  switch (com)
    {
    case W_WHO:
      who (lang);
      break;
    case W_ALL:
      if ((c = js_env_list (js, &rb)) == -1)
        err ();
      w = (WNN_ENV_INFO *) rb.buf;
      printall (w, c);
      break;
    case W_ENV:
      if ((c = js_env_list (js, &rb)) == -1)
        err ();
      w = (WNN_ENV_INFO *) rb.buf;
      printpat (w, c);
      break;
    case W_DIC:
      dic ();
      break;
    case W_DIC_ALL:
      dic_all ();
      break;
    case W_FILE:
      file_all ();
      break;
    case W_VERSION:
      js_version (js, &i, &j);
      printf ("Jserver Version %x, Jlib Version %x\n", i, j);
    }
  js_close (js);
  exit (0);
}

static void
printall (w, c)
     WNN_ENV_INFO *w;
     int c;
{
  int i, j;
  /*
     out("Env No.\t環境名\t参照数\t付属語\t辞書数(辞書No.)\t\tファイル\n");
   */
  out (msg_get (cd, 1, "Env No.\tEnv Name\tRefs\tGrammer\tDicts(Dict No.)\t\tFile\n", NULL));
  for (i = 0; i < c; i++, w++)
    {
      out ("%d\t%s\t%d\t%d\t%d(", w->env_id, w->env_name, w->ref_count, w->fzk_fid, w->jishomax);
      for (j = 0; j < w->jishomax; j++)
        {
          if (w->jisho[j] != -1)
            out ("%d ", w->jisho[j]);
        }
      out (")\t");
      for (j = 0; j < WNN_MAX_FILE_OF_AN_ENV; j++)
        {
          if (w->file[j] != -1)
            out ("%d ", w->file[j]);
        }
      putchar ('\n');
    }
}

static void
printpat (w, c)
     WNN_ENV_INFO *w;
     int c;
{
  int i;
  /*
     out("Env No.\t環境名\t参照数\n");
   */
  out (msg_get (cd, 2, "Env No.\tEnv Name\tRefs\n", NULL));
  for (i = 0; i < c; i++, w++)
    {
      out ("%d\t%s\t%d\n", w->env_id, w->env_name, w->ref_count);
    }
}

static void
err ()
{
  out (wnn_perror ());
  putchar ('\n');
  exit (-1);
}

static void
usage ()
{
#ifdef JAPANESE
  fprintf (stderr, "wnnstat [-weEdDfFUSJV] [-L lang_name] [server_name]\n");
#endif /* JAPANESE */
#ifdef  CHINESE
  fprintf (stderr, "cwnnstat [-weEdDfFUBCV] [-L lang_name] [server_name]\n");
#endif /* CHINESE */
#ifdef KOREAN
  fprintf (stderr, "kwnnstat [-weEdDfFUKV] [-L lang_name] [server_name]\n");
#endif /* KOREAN */
  exit (-1);
}

static void
who (lang)
     char *lang;
{
  WNN_JWHO *w;
  int c, k, j;
  struct wnn_env *env;

  if ((env = js_connect_lang (js, WNNSTAT_ENV, lang)) == NULL)
    err ();

  if ((c = js_who (js, &rb)) == -1)
    err ();
  w = (WNN_JWHO *) rb.buf;
  /*
     out("ユーザ名:ホスト名\t(ソケットNo.)\t環境番号\n");
   */
  out (msg_get (cd, 3, "User name:Host name\t(socket)\tEnv No.\n", NULL));
  for (k = 0; k < c; k++, w++)
    {
      if (w->env[0] == env->env_id)
        continue;
      out ("%s:%s\t\t(%d)\t\t", w->user_name, w->host_name, w->sd);
      for (j = 0; j < WNN_MAX_ENV_OF_A_CLIENT; j++)
        {
          if (w->env[j] != -1)
            {
              out ("%d ", w->env[j]);
            }
        }
      out ("\n");
    }
  exit (c - 1);
}

static void
dic ()
{
  char ch[256];
  WNN_DIC_INFO *w;
  int c, k;
  if ((c = js_dic_list_all (js, &rb)) == -1)
    err ();
  w = (WNN_DIC_INFO *) rb.buf;
  /*
     out("No.\t種類\tニックネーム\tファイル\t\n");
   */
  out (msg_get (cd, 4, "No.\tDict type\tName\tFile\n", NULL));
  for (k = 0; k < c; k++, w++)
    {
      wsttost (ch, w->comment);
      out ("%d:%s\t%s\t%d:%s", w->dic_no,
           w->type & 0xff00 ? ((w->type & 0xff00) == (BWNN_REV_DICT & 0xff00) ? bwnn_dic_types[0x00ff & w->type] : cwnn_dic_types[0x00ff & w->type]) : wnn_dic_types[w->type], ch, w->body, w->fname);
      if (w->hindo != -1)
        {
          out ("(%d)\n", w->hindo);
        }
      else
        {
          out ("\n");
        }
    }
}

static void
dic_all ()
{
  char ch[256];
  WNN_DIC_INFO *w;
  int c, k;
  if ((c = js_dic_list_all (js, &rb)) == -1)
    err ();
  w = (WNN_DIC_INFO *) rb.buf;
  /*
     out("No.種類\t正/逆\t語数\t更新(頻)使用 プライオリティ\t[ニックネーム]\tファイル[(頻度)]\t[パスワード(頻度)]\n");
   */
  out (msg_get (cd, 5, "No.Dict type\tN/R\tWords\tR/W(F)Use Priorty\t[Name]\tFile[(Freq)]\t[Passwd(Freq)]\n", NULL));
  for (k = 0; k < c; k++, w++)
    {
      wsttost (ch, w->comment);
      out ("%d:%s\t%s\t%d\t%s(%s)\t%s   %d\t",
           w->dic_no, w->type & 0xff00 ? ((w->type & 0xff00) == (BWNN_REV_DICT & 0xff00) ? bwnn_dic_types[0x00ff & w->type] : cwnn_dic_types[0x00ff & w->type]) : wnn_dic_types[w->type],
           /*
              w->rev ? "逆変換":"正変換",
              w->gosuu, 
              (w->rw)? "不":"可",
              (w->hindo_rw)? "不":"可",
              (w->enablef)? "中":"停止",
            */
           w->rev ? msg_get (cd, 6, "Rev", NULL) :
           msg_get (cd, 7, "Norm", NULL),
           w->gosuu,
           (w->rw) ? msg_get (cd, 8, "Y", NULL) : msg_get (cd, 9, "N", NULL),
           (w->hindo_rw) ? msg_get (cd, 8, "Y", NULL) : msg_get (cd, 9, "N", NULL), (w->enablef) ? msg_get (cd, 10, "Y", NULL) : msg_get (cd, 11, "N", NULL), w->nice);
      if (*ch)
        out ("%s\t", ch);
      out ("%d:%s\t", w->body, w->fname);
      if (w->hindo != -1)
        {
          out ("(%d:%s)\t", w->hindo, w->hfname);
        }
      if (w->passwd[0])
        out ("%s", w->passwd);
      if (w->hpasswd[0])
        out ("(%s)", w->hpasswd);
      out ("\n");
    }
}


static void
file_all ()
{
  char *d;
  WNN_FILE_INFO_STRUCT *w;
  int c, k;
  if ((c = js_file_list_all (js, &rb)) == -1)
    err ();
  w = (WNN_FILE_INFO_STRUCT *) rb.buf;
  /*
     out("Fid\t種類\t場所\t参照数\t\tファイル名\n");
   */
  out (msg_get (cd, 12, "Fid\tType\tL/R\tRefs\t\tFile\n", NULL));
  for (k = 0; k < c; k++, w++)
    {
      switch (w->type)
        {
        case WNN_FT_DICT_FILE:
          /*
             d = "辞書";
           */
          d = msg_get (cd, 13, "Dict", NULL);
          break;
        case WNN_FT_HINDO_FILE:
          /*
             d = "頻度";
           */
          d = msg_get (cd, 14, "Freq", NULL);
          break;
        case WNN_FT_FUZOKUGO_FILE:
          /*
             d = "付属語";
           */
          d = msg_get (cd, 15, "Gram", NULL);
          break;
        }
      out ("%d\t%s\t%s\t%d\t%s\n", w->fid, d, w->localf ? "LOCAL" : "REMOTE", w->ref_count, w->name);
    }
}


static void
wsttost (c, w)
     char *c;
     w_char *w;
{
  while (*w)
    {
      if (*w & 0xff00)
        *c++ = (*w & 0xff00) >> 8;
      *c++ = *w++ & 0x00ff;
    }
  *c = 0;
}

static void
out (const char * format, ...)
{
  int len;
  char buf[1024];
  char jbuf[1024];
  va_list ap;

  va_start (ap, format);
#ifdef HAVE_SNPRINTF	/* Should also have vsnprintf */
  vsnprintf (buf, sizeof buf, format, ap);
#else
  vsprintf (buf, format, ap);	/* Dangerous */
#endif /* HAVE_SNPRINTF */
  va_end (ap);

  len = strlen (buf);
  switch (ocode)
    {
#ifdef JAPANESE
    case J_EUJIS:
#endif
#ifdef  CHINESE
    case C_EUGB:
    case (C_EUGB + 4):
    case (C_ECNS11643 + 6):
#endif /* CHINESE */
#ifdef KOREAN
    case K_EUKSC:
#endif
      strncpy (jbuf, buf, len + 1);
      break;
#ifdef JAPANESE
    case J_JIS:
      eujis_to_jis8 (jbuf, buf, len + 1);
      break;
    case J_SJIS:
      eujis_to_sjis (jbuf, buf, len + 1);
      break;
#endif
#ifdef  CHINESE
    case (C_BIG5 + 6):
      ecns_to_big5 (jbuf, buf, len + 1);
      break;
#endif /* CHINESE */
#ifdef  KOREAN
    case K_KSC:
      euksc_to_ksc (jbuf, buf, len + 1);
      break;
#endif /* KOREAN */
    }
  printf ("%s", jbuf);
}
