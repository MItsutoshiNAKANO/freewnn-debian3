/*
 *  $Id: wnnkill.c $
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

#ifndef lint
static char *rcs_id = "$Id: wnnkill.c $";
#endif /* lint */

#include <stdio.h>
#include "jllib.h"
#include "jslib.h"
#include "jd_sock.h"
#include "commonhd.h"
#include "wnn_config.h"
#include "wnn_os.h"

static void out ();

WNN_JSERVER_ID *js;
struct wnn_ret_buf rb = { 0, NULL };

#ifdef JAPANESE
int ocode = TTY_KCODE;
#endif
#ifdef  CHINESE
#ifdef  TAIWANESE
int ocode = (TTY_TCODE + 6);
#else /* TAIWANESE */
int ocode = (TTY_CCODE + 4);
#endif /* TAIWANESE */
#endif /* CHINESE */
#ifdef KOREAN
int ocode = TTY_HCODE;
#endif

int
main (argc, argv)
     int argc;
     char **argv;
{
  extern char *getenv ();
  int c;
  char *serv;
  int x;
  static char lang[64] = { 0 };
  extern int optind;
  extern char *optarg;
/*
    char *p;
*/
  char *server_env = NULL;
  char *prog = argv[0];
  extern char *_wnn_get_machine_of_serv_defs (), *get_server_env ();

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
#  define OPTSTRING "USJL:"
#endif
#ifdef  CHINESE
#  define OPTSTRING "USJBCL:"
#endif /* CHINESE */
#ifdef KOREAN
#  define OPTSTRING "UL:"
#endif

  while ((c = getopt (argc, argv, OPTSTRING)) != EOF)
    {
      switch (c)
        {
        case 'U':
#ifdef JAPANESE
          ocode = J_EUJIS;
#endif
#ifdef CHINESE
          ocode = C_EUGB;
#endif
#ifdef KOREAN
          ocode = K_EUKSC;
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
          ocode = (C_BIG5 + 6);
          break;
        case 'C':
          ocode = (C_ECNS11643 + 6);
          break;
#endif /* CHINESE */
        case 'L':
          strcpy (lang, optarg);
          break;
        default:
          break;
        }
    }
  if (optind)
    {
      optind--;
      argc -= optind;
      argv += optind;
    }

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
        out (serv);
      out ("%s\n", wnn_perror_lang (lang));
/*      fprintf(stderr, "Can't connect to jserver.\n"); */
      exit (255);
    }
  if ((x = js_kill (js)) > 0)
    {
      if (x == 1)
        {
          out ("%d User Exists.\n", x);
        }
      else
        {
          out ("%d Users Exist.\n", x);
        }
      out ("%s Not Killed.\n", server_env);
      exit (1);
    }
  else if (x == 0)
    {
      out ("%s Terminated\n", server_env);
      exit (0);
    }
  else
    {
      out ("%s Terminated\n", server_env);
      exit (2);
    }
  exit (0);
}

#ifdef JAPANESE
extern int eujis_to_jis8 (), eujis_to_sjis ();
#endif
#ifdef CHINESE
extern int ecns_to_big5 ();
#endif

static void
out (a1, a2, a3, a4, a5, a6, a7, a8)
     char *a1, *a2, *a3, *a4, *a5, *a6, *a7, *a8;
{
  int len;
  char buf[1024];
  char jbuf[1024];
  sprintf (buf, a1, a2, a3, a4, a5, a6, a7, a8);

  len = strlen (buf);
  switch (ocode)
    {
#ifdef JAPANESE
    case J_EUJIS:
#endif
#ifdef  CHINESE
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
    }
  fprintf (stderr, "%s", jbuf);
}
