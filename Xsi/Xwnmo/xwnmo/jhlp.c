#ifndef lint
static char rcs_id[] = "$Id: jhlp.c,v 1.26.2.3 1999/04/19 06:10:57 nakanisi Exp $";
#endif /* lint */

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
#include <signal.h>
#include <pwd.h>
#include "commonhd.h"
#include "sdefine.h"
#include <X11/Xos.h>
#include "xim.h"
#include "sheader.h"
#include "config.h"
#include "ext.h"

#ifdef  BSD42
#include <sgtty.h>
#endif /* BSD42 */
#ifdef  SYSVR2
#include <termio.h>
#endif /* SYSVR2 */
extern char *optarg;
extern int optind;

extern char *getenv ();
extern void exit ();

static void save_signals ();

static struct _options
{
  char *opt;
  char *desc;
}
options[] =
{
  {
  "-D servername", "Wnn server to contact"}
  ,
  {
  "-lc langname", "language name"}
  ,
  {
  "-n username", "user name"}
  ,
  {
  "-k uumkeyname", "uumkey file"}
  ,
  {
  "-r rkfilename", "romkan file"}
  ,
  {
  "-X cvtximname", "convert keys file"}
  ,
  {
  "-F cvtfunname", "convert function file"}
  ,
  {
  "-M cvtmetaname", "convert meta file"}
  ,
  {
  "-fs fontnamelist", "font name list"}
  ,
  {
  "-geometry geom", "size (in pixels) and position"}
  ,
  {
  "-fg color", "foreground color"}
  ,
  {
  "-bg color", "background color"}
  ,
  {
  "-bd color", "border color"}
  ,
  {
  "-bw number", "border width in pixels"}
  ,
  {
  "-display displayname", "X server to contact"}
  ,
  {
  "-iconic", "start iconic"}
  ,
  {
  "#geom", "icon window geometry"}
  ,
  {
  "-help", "print out this message"}
  ,
  {
  "-h", "wake up in convertion off mode"}
  ,
  {
  "-H", "wake up in convertion on mode"}
  ,
#ifdef  USING_XJUTIL
  {
  "-ju", "xjutil (dictionary utility manager) name"}
  ,
#endif /* USING_XJUTIL */
  {
  "-RV", "Roo-mode window can switch map and unmap"}
  ,
  {
  NULL, NULL}
};

static int
message_open (lang)
     register char *lang;
{
  char nlspath[MAXPATHLEN];

  strcpy (nlspath, LIBDIR);
  strcat (nlspath, "/%L/%N");

#ifdef DEBUG
  cd = msg_open ("/usr/tmp/xim.msg", nlspath, lang);
#else
  cd = msg_open (XIMMSGFILE, nlspath, lang);
#endif
  if (cd->msg_bd == NULL)
    {
      print_out1 ("I can't open message_file \"%s\", use default message.", cd->nlspath);
    }
  return (0);
}

static void
usage (bad_option)
     char *bad_option;
{
  struct _options *opt;
  int col;

  fprintf (stderr, "%s:  bad command line option \"%s\"\r\n\n", prgname, bad_option);

  fprintf (stderr, "usage:  %s", prgname);
  col = 8 + strlen (prgname);
  for (opt = options; opt->opt; opt++)
    {
      int len = 3 + strlen (opt->opt);
      if (col + len > 79)
        {
          fprintf (stderr, "\r\n   ");
          col = 3;
        }
      fprintf (stderr, " [%s]", opt->opt);
      col += len;
    }

  fprintf (stderr, "\r\n\nType %s -help for a full description.\r\n\n", prgname);
  exit (1);
}

static void
help ()
{
  struct _options *opt;

  fprintf (stderr, "usage:\n        %s [-options ...]\n\n", prgname);
  fprintf (stderr, "where options include:\n");
  for (opt = options; opt->opt; opt++)
    {
      fprintf (stderr, "    %-28s %s\n", opt->opt, opt->desc);
    }
  fprintf (stderr, "\n");
  exit (0);
}

/*
 * Parse options
 */
static void
parse_options (argc, argv)
     int argc;
     char **argv;
{
  int i;

  for (i = 1; i < argc; i++)
    {
      if (!strcmp (argv[i], "-h"))
        {
          root_henkan_off_def = 1;
          defined_by_option |= OPT_WAKING_UP_MODE;
        }
      else if (!strcmp (argv[i], "-H"))
        {
          root_henkan_off_def = 0;
          defined_by_option |= OPT_WAKING_UP_MODE;
        }
      else if (!strcmp (argv[i], "-k"))
        {
          if ((++i >= argc) || (argv[i][0] == '-'))
            usage (argv[i]);
          root_uumkeyname = alloc_and_copy (argv[i]);
          if (root_uumkeyname == NULL || *root_uumkeyname == '\0')
            usage (argv[i]);
          defined_by_option |= OPT_WNNKEY;
        }
      else if (!strcmp (argv[i], "-r"))
        {
          if ((++i >= argc) || (argv[i][0] == '-'))
            usage (argv[i]);
          root_rkfilename = alloc_and_copy (argv[i]);
          if (root_rkfilename == NULL || *root_rkfilename == '\0')
            usage (argv[i]);
          defined_by_option |= OPT_RKFILE;
        }
      else if (!strcmp (argv[i], "-help"))
        {
          help ();
        }
      else
        {
          usage (argv[i]);
        }
    }
}


/* get rubout_code */
#define UNDEF_STTY 0xff

#ifdef BSD42
static void
get_rubout ()
{
  struct sgttyb savetmio;

  if ((ioctl (0, TIOCGETP, &savetmio) < 0) || (savetmio.sg_erase == UNDEF_STTY))
    {
      rubout_code = RUBOUT;     /* BackSpase */
    }
  else
    {
      rubout_code = savetmio.sg_erase;
    }
}
#endif /* BSD42 */

#ifdef SYSVR2
static void
get_rubout ()
{
  struct termio tmio;

  if ((ioctl (0, TCGETA, &tmio) < 0) || (tmio.c_cc[VERASE] == UNDEF_STTY))
    {
      rubout_code = RUBOUT;     /* BackSpase */
    }
  else
    {
      rubout_code = tmio.c_cc[VERASE];
    }
}
#endif /* SYSVR2 */

void
do_end ()
{
  xw_end ();
  epilogue ();
#ifdef  USING_XJUTIL
  kill_all_xjutil ();
#endif /* USING_XJUTIL */
  exit (0);
}

/** Handler of SIGTERM */
static void
terminate_handler ()
{
  do_end ();
}

/** Mail roop */

void
in_put (c)
     int c;
{
  register int ml;

  do
    {
      ml = kk (c);
      if (ml >= 0)
        {
          make_history (return_buf, ml);
          xw_write (return_buf, ml);
        }
    }
  while (if_unget_buf ());
}

static void
do_main ()
{
  int type;

  for (;;)
    {
      X_flush ();
      type = wait_for_socket ();
      switch (type)
        {
        case XEVENT_TYPE:
          XEventDispatch ();
          break;
#ifndef X11R5
        case REQUEST_TYPE_XIM:
          XimRequestDispatch ();
          break;
#endif /* !X11R5 */
#if defined(X11R5) || defined(BC_X11R5)
        case REQUEST_TYPE:
          RequestDispatch ();
          break;
#endif /* defined(X11R5) || defined(BC_X11R5) */
#if defined(XJPLIB) && defined(XJPLIB_DIRECT)
        case XJP_DIRECT_TYPE:
          XJp_Direct_Dispatch ();
#endif /* defined(XJPLIB) && defined(XJPLIB_DIRECT) */
        default:
          break;
        }
    }
}

/*
  signal settings
 */

static void
save_signals ()
{
  signal (SIGPIPE, SIG_IGN);
  signal (SIGHUP, SIG_IGN);
#ifndef NOTFORK
  signal (SIGINT, SIG_IGN);
#endif /* NOTFORK */
  signal (SIGQUIT, SIG_IGN);
  signal (SIGTERM, terminate_handler);
  signal (SIGCHLD, SIG_IGN);
}

static int
make_world_lang ()
{
  XIMLangDataBase *ld;
  register int len = 0;

  for (ld = language_db; ld; ld = ld->next)
    {
      len += strlen (ld->lc_name) + 1;
    }
  if (!(world_lang = Malloc (len)))
    return (-1);
  world_lang[0] = '\0';
  for (ld = language_db; ld; ld = ld->next)
    {
      strcat (world_lang, ld->lc_name);
      strcat (world_lang, ";");
    }
  if (*world_lang)
    {
      world_lang[strlen (world_lang) - 1] = '\0';
    }
  return (0);
}

static int
create_arg_string (argv, argc)
     char **argv;
     int argc;
{
  register int nbytes, i;
  register char *p;

  for (i = 0, nbytes = 0; i < argc; i++)
    {
      nbytes += (argv[i] ? strlen (argv[i]) : 0) + 1;
    }
  if (!(p = Malloc (nbytes)))
    {
      malloc_error ("allocation of work area");
      return (-1);
    }
  for (i = 0; i < argc; i++)
    {
      if (argv[i])
        {
          strcpy (p, argv[i]);
          p += strlen (argv[i]) + 1;
        }
    }
  arg_string = p;
  return (0);
}

void
main (argc, argv)
     int argc;
     char **argv;
{
  char *name;
  char lc_name_buf[64], *lc_name = NULL;
  register XIMLangDataBase *db = NULL;
  register XIMLcNameRec *lnl;
  extern struct passwd *getpwuid ();
  char *server_env;
#ifdef X11R5
  XLocale xlc;
#else
  XLCd xlc;
#endif /* X11R5 */
  extern char *get_server_env ();
/*    extern char *index(); */

  prgname = argv[0];
  if (create_arg_string (argv, argc) == -1)
    exit (1);

  /*
   * get application resources
   */
  argc = get_application_resources (argc, argv);
  /*
   * parse option
   */
  parse_options (argc, argv);

  /*
   * Get user name and server name
   */
  if (root_username == NULL)
    {
      if ((name = getenv (WNN_USERNAME_ENV)) != NULL)
        {
          if (!(root_username = alloc_and_copy (name)))
            exit (1);
        }
      else
        {
          if (!(root_username = alloc_and_copy (getpwuid (getuid ())->pw_name)))
            exit (1);
        }
    }

  /*
   * read xim.conf file
   */
  if (read_ximconf () == -1)
    exit (1);

  if (def_lang)
    {
      for (db = language_db; db; db = db->next)
        {
          if (!strcmp (db->lang, def_lang))
            {
              lc_name = setlocale (LC_ALL, db->lc_name);
              cur_lang = db;
              break;
            }
        }
      if (db == NULL || db->lang == NULL)
        {
          print_out1 ("Sorry, langName \"%s\" is not my supported language.", def_lang);
          print_out ("I refer to the system locale.");
        }
      else if (lc_name == NULL)
        {
          print_out2 ("Sorry, can not set locale \"%s\" related to langName \"%s\".", db->lc_name, def_lang);
          print_out ("I refer to the system locale.");
        }
    }
  if (lc_name == NULL)
    {
      lc_name = setlocale (LC_ALL, "");
      if (lc_name == NULL || !strcmp (lc_name, "C"))
        {
          if (lc_name)
            {
              print_out ("The current locale is C.");
            }
          else
            {
              print_out ("The current locale is wrong.");
            }
          print_out1 ("I try to set locale to defualt \"%s\".", def_locale);
          lc_name = setlocale (LC_ALL, def_locale);
          if (lc_name == NULL)
            {
              print_out ("Could not set locale by setlocale().");
              print_out ("I will abort.");
              exit (1);
            }
          if (!strcmp (lc_name, "C"))
            {
              print_out ("Sorry, can not run under the C locale environment.");
              print_out ("I will abort.");
              exit (1);
            }
        }
#ifdef X11R5
      if (!(xlc = _XFallBackConvert ()))
        {
          print_out ("Could not create locale environment.");
          print_out ("I will abort.");
          exit (1);
        }
      if (strcmp (lc_name, xlc->xlc_db->lc_name))
        {
          lc_name = xlc->xlc_db->lc_name;
#else
      if (!(xlc = _XlcCurrentLC ()))
        {
          print_out ("Could not create locale environment.");
          print_out ("I will abort.");
          exit (1);
        }
      if (strcmp (lc_name, xlc->core->name))
        {
          lc_name = xlc->core->name;
#endif /* X11R5 */
        }
      if (!strcmp (lc_name, world_locale))
        {
          lnl = lc_name_list;
        }
      else
        {
          for (lnl = lc_name_list; lnl; lnl = lnl->next)
            {
              if (!strcmp (lnl->lc_name, lc_name))
                break;
            }
        }
      if (!lnl)
        {
          print_out1 ("Sorry, locale \"%s\" is not my supported locale.", lc_name);
          print_out ("I will abort.");
          exit (1);
        }
      cur_lang = lnl->lang_db;
    }
  strcpy (lc_name_buf, lc_name);

  if (root_def_servername == NULL)
    {
      if (!(server_env = get_server_env (cur_lang->lang)))
        {
          server_env = WNN_DEF_SERVER_ENV;
        }
      if (name = getenv (server_env))
        {
          if (!(root_def_servername = root_def_reverse_servername = alloc_and_copy (name)))
            exit (1);
        }
      else
        {
          root_def_servername = root_def_reverse_servername = NULL;
        }
    }

  save_signals ();

  get_rubout ();

#ifndef NOTFORK
  if (fork ())
    {
      signal (SIGTERM, exit);
      signal (SIGCHLD, exit);
      pause ();
    }
#endif /* NOTFORK */

  close (fileno (stdin));
  close (fileno (stdout));
  if (make_world_lang () < 0 || /* make world_lang string */
      read_ximrc () < 0 ||      /* read ximrc file */
      message_open (cur_lang->lang) < 0 ||      /* open message mechanism */
      read_default_rk () < 0 || init_xcvtkey () < 0 || alloc_all_buf () < 0 || create_xim (lc_name_buf) < 0)    /* create X window environment */
    exit (1);

  if (c_c->use_server && !jl_isconnect (bun_data_))
    {
      if (servername && *servername)
        {
          print_msg_getc (" I can not connect server(at %s)", servername, NULL, NULL);
        }
      else
        {
          print_msg_getc (" I can not connect server", NULL, NULL, NULL);
        }
    }

#ifndef NOTFORK
  /* End of initialization, kill parent */
  kill (getppid (), SIGTERM);
#endif /* !NOTFORK */

  clear_c_b ();

  cur_rk = c_c->rk;
  cur_rk_table = cur_rk->rk_table;
  visual_status ();
  if (henkan_off_flag == 0)
    {
      disp_mode ();
/*
    } else {
        henkan_off();
*/
    }
  do_main ();
}
