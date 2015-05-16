/*
 *  $Id: jhlp.c $
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
static char *rcs_id = "$Id: jhlp.c $";
#endif /* lint */

#include <stdio.h>
#include <signal.h>
#include "jllib.h"
#include "commonhd.h"
#include "sdefine.h"
#include "sheader.h"
#include "wnn_config.h"

#include <sys/types.h>
#include <sys/ioctl.h>
#ifndef SYSVR2
#include <fcntl.h>
#endif /* !SYSVR2 */
#include <sys/errno.h>
#include <pwd.h>
#include <sys/time.h>
#ifdef UX386
#include <sys/kdef.h>
#endif

/*
struct passwd *getpwuid();
*/

#ifdef SYSVR2
#       include <setjmp.h>
/*      don't define bcopy!!!!          */
#endif /* SYSVR2 */
/* jlib.hの中でsetjmp.hがincludeされている!! */

jmp_buf kk_env;

#if (defined(__unix__) || defined(unix)) && !defined(USG)
#include <sys/param.h>
#endif
#ifdef SYSVR2
#       include <sys/param.h>
#endif /* SYSVR2 */

#ifdef HAVE_WAIT3
#       include <sys/wait.h>
#endif /* HAVE_WAIT3 */

#ifdef BSD42
#       include <sgtty.h>
#endif /* BSD42 */

#ifdef SYSVR2
#ifdef TERMINFO
#       include <curses.h>
#endif /* TERMINFO */
#       include <term.h>
#endif /* SYSVR2 */
#if defined(SVR4) || defined(hpux) || defined(sgi)
#include <sys/termio.h>
#endif
#ifdef linux
#include <termios.h>
#endif

#ifdef nec_ews_svr2
#include <sys/jtermio.h>
#endif /* nec_ews_svr2 */

#if defined(uniosu)
#       include <sys/pty.h>
#endif /* defined(uniosu) */

#ifdef SVR4
#include <sys/stropts.h>
#include <sys/euc.h>
#include <sys/eucioctl.h>
#endif /* SVR4 */

#define ERROR -1

#ifdef TIOCSSIZE
struct ttysize pty_rowcol;
#endif /* TIOCSSIZE */

int ttyfd;

char *tname;                    /* terminal name */
char *cmdnm = "csh";            /* char *cmdnm = "csh"; */

int child_id;
char *prog;

extern int errno;
extern char *optarg;
extern int optind;

extern char *ttyname (), *malloc (), *getenv ();

static void save_signals ();
static void restore_signals ();

static intfntype terminate_handler ();
static void do_end (), open_pty (), open_ttyp (), do_main (), exec_cmd (), parse_options (), setsize (), get_rubout (), usage (), change_size (), default_usage ();

/** メイン */
void
main (argc, argv)
     int argc;
     char **argv;
{

  char *name;
  char *p;
  char nlspath[64];
  FuncDataBase *f;
  char *server_env;
  int i;
  extern char *get_server_env ();

  prog = argv[0];
  flow_control = FLOW_CONTROL;
  code_trans = default_code_trans;

  strcpy (username, getpwuid (getuid ())->pw_name);
  if ((name = getenv (WNN_USERNAME_ENV)) != NULL)
    {
      strcpy (username, name);
    }
  for (i = 1; i < argc;)
    {
      if (!strcmp (argv[i++], "-L"))
        {
          if (i >= argc || argv[i][0] == '-')
            default_usage ();
          strcpy (lang_dir, argv[i++]);
          for (; i < argc; i++)
            {
              argv[i - 2] = argv[i];
            }
          argv[i - 2] = NULL;
          argc -= 2;
          break;
        }
    }

  if (*lang_dir == '\0')
    {
      if ((p = getenv ("LANG")) != NULL)
        {
          if (strlen (p) >= 4)
            {
              strncpy (lang_dir, p, 5);
              lang_dir[5] = '\0';
            }
          else
            {
              strcpy (lang_dir, p);
            }
        }
    }
  for (f = function_db; *lang_dir && f && f->lang; f++)
    {
      if (!strcmp (f->lang, lang_dir))
        {
          lang_db = f;
          break;
        }
    }
  if (lang_db == NULL)
    {
      if (*lang_dir)
        fprintf (stderr, "Lang \"%s\" is wrong, use default lang \"%s\".\n", lang_dir, WNN_DEFAULT_LANG);
      strcpy (lang_dir, WNN_DEFAULT_LANG);
      for (f = function_db; *lang_dir && f && f->lang; f++)
        {
          if (!strcmp (f->lang, lang_dir))
            {
              lang_db = f;
              break;
            }
        }
      if (lang_db == NULL)
        {
          fprintf (stderr, "Default lang \"%s\" is wrong.\n", lang_dir);
          exit (1);
        }
    }
  f_table = &(lang_db->f_table);
  code_trans = lang_db->code_trans;
  tty_c_flag = lang_db->tty_code;
  pty_c_flag = lang_db->pty_code;
  internal_code = lang_db->internal_code;
  file_code = lang_db->file_code;

  parse_options (argc, argv);

  strcpy (nlspath, LIBDIR);
  strcat (nlspath, "/%L/%N");
  cd = msg_open ("uum.msg", nlspath, lang_dir);

  if (*def_servername == '\0')
    {
      if (!(server_env = get_server_env (lang_dir)))
        {
          server_env = WNN_DEF_SERVER_ENV;
        }
      if (name = getenv (server_env))
        {
          strcpy (def_servername, name);
          strcpy (def_reverse_servername, name);
        }
    }

  if (!isatty (0))
    {
      fprintf (stderr, "Input must be a tty.\n");
      exit (1);
    }

  if ((p = getenv (WNN_COUNTDOWN_ENV)) == NULL)
    {
      setenv (WNN_COUNTDOWN_ENV, "0");
    }
  else if (atoi (p) <= 0)
    {
      puteustring (MSG_GET (4), stdout);
      /*
         puteustring("ｕｕｍからｕｕｍは起こせません。\n",stdout);
       */
      exit (126);
    }
  else
    {
      sprintf (p, "%d", atoi (p) - 1);
      setenv (WNN_COUNTDOWN_ENV, p);
    }

  if ((tname = getenv ("TERM")) == NULL)
    {
      fprintf (stderr, "Sorry. Please set your terminal type.\r\n");
      exit (1);
    }


#if defined(BSD42) && !defined(DGUX)
#if !(defined(BSD) && (BSD >= 199306))
  if (saveutmp () < 0)
    {
      puts ("Can't save utmp\n");
    }
#endif
#endif /* BSD42 */


  if (optind)
    {
      optind--;
      argc -= optind;
      argv += optind;
    }
  if (argc > 1)
    {
      cmdnm = *++argv;
    }
  else
    {
      if ((name = getenv ("SHELL")) != NULL)
        {
          cmdnm = name;
        }
      argv[0] = cmdnm;
      argv[1] = NULL;
    }

  save_signals ();

#ifdef TERMCAP
  if (getTermData () == -1)
    {
      fprintf (stderr, "Sorry. Something is wrong with termcap, maybe.\r\n");
      exit (21);
    }
#endif /* TERMCAP */
#ifdef TERMINFO
  if (openTermData () == -1)
    {
      fprintf (stderr, "Sorry. Something is wrong with terminfo, maybe.\r\n");
      exit (21);
    }
  maxlength = columns;
  crow = lines - conv_lines;
#endif /* TERMINFO */
#if defined(BSD43) || defined(DGUX)
  setsize ();
#endif /* BSD43 */

#ifdef TERMCAP
  if (set_TERMCAP () == -1)
    {
      fprintf (stderr, "Sorry. Something is wrong with termcap, maybe.\r\n");
      exit (21);
    }
#endif /* TERMCAP */

  open_pty ();
#ifndef linux
  open_ttyp ();
#endif
  exec_cmd (argv);

  /*
     close(0);
     close(1);
     close(2);
     if((ttyfd = open("/dev/tty", O_RDWR | O_NDELAY, 0)) != 0) {
     err("Can't open /dev/tty.");
     }
     dup(0);
     dup(0);
   */

  ttyfd = 0;
  get_rubout ();

  switch (init_uum ())
    {                           /* initialize of kana-kanji henkan */
    case -1:
      terminate_handler ();
      break;
    case -2:
      epilogue ();
      do_end ();
      break;
    }

  fcntl (ttyfd, F_SETFL, O_NDELAY);

  if (j_term_init () == ERROR)
    {
      err ("term initialize fault.");
    }

  if (!jl_isconnect (bun_data_))
    {
      if (!servername || *servername == 0)
        {
          printf ("%s\r\n", wnn_perror ());
        }
      else
        {
          printf ("jserver(at %s):%s\r\n", servername, wnn_perror ());
        }
      flush ();
    }

  puteustring (MSG_GET (1),
               /*
                  "\rｕｕｍ(かな漢字変換フロントエンドプロセッサ)\r\n",
                */
               stdout);
  initial_message_out ();       /* print message if exists. */

#if defined(uniosu)
  if (setjmp (kk_env))
    {
      disconnect_jserver ();
      ioctl_off ();
      connect_jserver (0);
    }
#endif /* defined(uniosu) */

  do_main ();
}

/*
  each options handling functions
 */

static int
do_h_opt ()
{
  henkan_off_flag = 1;
  defined_by_option |= OPT_WAKING_UP_MODE;
  return 0;
}

static int
do_H_opt ()
{
  henkan_off_flag = 0;
  defined_by_option |= OPT_WAKING_UP_MODE;
  return 0;
}

#ifdef  JAPANESE
int
do_u_opt ()
{
  pty_c_flag = J_EUJIS;
  return 0;
}

int
do_j_opt ()
{
  pty_c_flag = J_JIS;
  return 0;
}

int
do_s_opt ()
{
  pty_c_flag = J_SJIS;
  return 0;
}

int
do_U_opt ()
{
  tty_c_flag = J_EUJIS;
  return 0;
}

int
do_J_opt ()
{
  tty_c_flag = J_JIS;
  return 0;
}

int
do_S_opt ()
{
  tty_c_flag = J_SJIS;
  return 0;
}
#endif /* JAPANESE */

#ifdef  CHINESE
int
do_b_opt ()
{
  pty_c_flag = C_BIG5;
  return 0;
}

int
do_t_opt ()
{
  pty_c_flag = C_ECNS11643;
  return 0;
}

int
do_B_opt ()
{
  tty_c_flag = C_BIG5;
  return 0;
}

int
do_T_opt ()
{
  tty_c_flag = C_ECNS11643;
  return 0;
}
#endif /* CHINESE */

#ifdef KOREAN
int
do_u_opt ()
{
  pty_c_flag = K_EUKSC;
  return 0;
}

int
do_U_opt ()
{
  tty_c_flag = K_EUKSC;
  return 0;
}
#endif /* KOREAN */

static int
do_P_opt ()
{
  sleep (20);
  return 0;
}

static int
do_x_opt ()
{
  flow_control = 0;
  defined_by_option |= OPT_FLOW_CTRL;
  return 0;
}

static int
do_X_opt ()
{
  flow_control = 1;
  defined_by_option |= OPT_FLOW_CTRL;
  return 0;
}

static int
do_k_opt ()
{
  strcpy (uumkey_name_in_uumrc, optarg);
  if (*uumkey_name_in_uumrc == '\0')
    {
      return -1;
    }
  defined_by_option |= OPT_WNNKEY;
  return 0;
}

static int
do_c_opt ()
{
  strcpy (convkey_name_in_uumrc, optarg);
  if (*convkey_name_in_uumrc == '\0')
    {
      return -1;
    }
  defined_by_option |= OPT_CONVKEY;
  return 0;
}

static int
do_r_opt ()
{
  strcpy (rkfile_name_in_uumrc, optarg);
  if (*rkfile_name_in_uumrc == '\0')
    {
      return -1;
    }
  defined_by_option |= OPT_RKFILE;
  return 0;
}

static int
do_l_opt ()
{
  conv_lines = atoi (optarg);
  return 0;
}

static int
do_D_opt ()
{
  strcpy (def_servername, optarg);
  strcpy (def_reverse_servername, optarg);
  if (*def_servername == '\0')
    {
      return -1;
    }
  return 0;
}

static int
do_n_opt ()
{
  strcpy (username, optarg);
  if (*username == '\0')
    {
      return -1;
    }
  return 0;
}

static int
do_v_opt ()
{
  defined_by_option |= OPT_VERBOSE;
  return 0;
}

static int (*do_opt[]) () =
{
  do_h_opt,                     /* 'h' : waking_up_in_henkan_mode */
    do_H_opt,                   /* 'H' : waking_up_no_henkan_mode */
    do_P_opt,                   /* 'P' : sleep 20 seconds (for debug) */
    do_x_opt,                   /* 'x' : disable tty's flow control */
    do_X_opt,                   /* 'X' : enable tty's flow control */
    do_k_opt,                   /* 'k' : specify uumkey file */
    do_c_opt,                   /* 'c' : specify convert_key file */
    do_r_opt,                   /* 'r' : specify romkan mode file */
    do_l_opt,                   /* 'l' : specify # of lines used for henkan */
    do_D_opt,                   /* 'D' : specify hostname of jserver */
    do_n_opt,                   /* 'n' : specify username for jserver */
    do_v_opt,                   /* 'v' : verbose */
};

static void
parse_options (argc, argv)
     int argc;
     char **argv;
{
  register int c;
  register char *default_getoptstr = GETOPTSTR;
  register char *default_ostr = OPTIONS;
  char ostr[64];
  register char *p;
#ifdef  SYSVR2
#define index   strchr
#endif /* SYSVR2 */
  extern char *index ();

  strcpy (ostr, default_getoptstr);
  strcat (ostr, lang_db->getoptstr);
  while ((c = getopt (argc, argv, ostr)) != EOF)
    {
      if (!(p = index (default_ostr, c)) || (*do_opt[p - default_ostr]) () < 0)
        {
          if (!(p = index (lang_db->ostr, c)) || (*lang_db->do_opt[p - lang_db->ostr]) () < 0)
            {
              strcpy (ostr, default_ostr);
              strcat (ostr, lang_db->ostr);
              usage (ostr);
            }
        }
    }
#ifdef  SYSVR2
#undef index
#endif /* SYSVR2 */
}

/** tty に対する ioctl のセット */

#ifdef BSD42
struct sgttyb savetmio;
/*
  struct sgttyb ttyb =
  { B9600, B9600, 0x7f, 0x15, EVENP | ODDP | ECHO | CRMOD };
  int local_mode = LCRTBS | LCRTERA | LCRTKIL | LCTLECH | LPENDIN | LDECCTQ;
*/

/* added later */
struct tchars tcharsv;
struct ltchars ltcharsv;
struct sgttyb ttyb;
int local_mode;

static void
get_rubout ()
{
#ifdef nodef
  if (savetmio.sg_erase == UNDEF_STTY)
    {
      rubout_code = RUBOUT;
    }
  else
    {
#endif
      rubout_code = savetmio.sg_erase;
#ifdef nodef
    }
#endif
}

int
j_term_init ()
{
  struct sgttyb buf;

  buf = savetmio;
  buf.sg_flags |= RAW;
  buf.sg_flags &= ~ECHO;
  ioctl (ttyfd, TIOCSETP, &buf);

  return 0;
}
#endif /* BSD42 */

#ifdef SYSVR2
#define UNDEF_STTY 0xff

struct termio savetmio;
struct termio terms = {
  IGNBRK | ICRNL | IXON,        /* c_iflag */
  ONLCR | OPOST,                /* c_oflag */
  B9600 | CS8 | CSTOPB | CREAD | CLOCAL,        /* c_cflag */
  ISIG | ICANON | ECHO | ECHOE | ECHOK, /* c_lflag */
  0,                            /* c_line */
  {0x3, 0x1c, 0x8, 0x15, 0x4, 0, 0, 0}  /* c_cc */
};

#if defined(uniosu)
struct jtermio savejtmio;
struct jtermio savejtmiottyp;
struct auxtermio saveauxterm;
struct auxtermio auxterm = {
  0,                            /* -tostop */
  {0x1a, 0, 0, 0, 0, 0, 0, 0}   /* c_cc2 */
};
#endif /* defined(uniosu) */

static void
get_rubout ()
{
  if (savetmio.c_cc[VERASE] == UNDEF_STTY)
    {
      rubout_code = RUBOUT;
    }
  else
    {
      rubout_code = savetmio.c_cc[VERASE];
    }
}

int
j_term_init ()
{
  struct termio buf1;
#if defined(uniosu)
  struct jtermio buf2;
#endif /* defined(uniosu) */

  buf1 = savetmio;
#ifdef linux
  buf1.c_lflag &= ~(ECHONL | ECHOK | ECHOE | ECHO | XCASE | ICANON | ISIG);
  buf1.c_iflag = 0;
  buf1.c_oflag &= ~OPOST;
  buf1.c_cflag |= CS8;
  buf1.c_cc[VMIN] = 1;          /* cf. ICANON */
  buf1.c_cc[VTIME] = 0;
#else /* linux */
  buf1.c_iflag &= ~(ISTRIP | INLCR | IGNCR | ICRNL | IUCLC | IXON);
  if (flow_control)
    {
      buf1.c_iflag |= IXON;
    }
  buf1.c_lflag &= ~(ECHONL | ECHOK | ECHOE | ECHO | XCASE | ICANON | ISIG);
  buf1.c_oflag = OPOST;
  buf1.c_cc[VINTR] = CDEL;
  buf1.c_cc[VQUIT] = CDEL;
  buf1.c_cc[VERASE] = CDEL;
  buf1.c_cc[VKILL] = CDEL;
  buf1.c_cc[VEOF] = 1;          /* cf. ICANON */
  buf1.c_cc[VEOL] = 0;
#endif /* linux */
  if (ioctl (ttyfd, TCSETA, &buf1) == -1)
    {
      fprintf (stderr, "error in ioctl TCSETA.\n");
      exit (1);
    }

#if defined(uniosu)
  if (ioctl (ttyfd, JTERMGET, &buf2) < 0)
    {
      fprintf (stderr, "error in ioctl JTERMGET in j_term_init.\n");
      exit (1);
    }
  savejtmio = buf2;

  buf2.j_flg = CONVTOEXT | WNN_EXIST;
  buf2.j_level = jterm;
  switch (jcode_set)
    {
    case 0:
      buf2.j_ecode = JIS;
      break;
    case 1:
      buf2.j_ecode = SJIS;
      break;
    case 2:
      buf2.j_ecode = UJIS;
      break;
    default:
      fprintf (stderr, "uum: kanji code set not supported in terminfo\n");
      exit (1);
    }
  if (jis_kanji_in)
    {
      strcpy (buf2.j_jst, jis_kanji_in);
      buf2.j_jstl = strlen (jis_kanji_in);
    }
  if (jis_kanji_out)
    {
      strcpy (buf2.j_jend, jis_kanji_out);
      buf2.j_jendl = strlen (jis_kanji_out);
    }
  if (jgaiji_start_address)
    {
      *(short *) buf2.j_gcsa = jgaiji_start_address;
    }
  if (jgaiji_disp)
    {
      strcpy (buf2.j_gdsp, jgaiji_disp);
      buf2.j_gdspl = strlen (jgaiji_disp);
    }

  if (ioctl (ttyfd, JTERMSET, &buf2) < 0)
    {
      fprintf (stderr, "error in ioctl JTERMSET.\n");
      exit (1);
    }
#endif /* defined(uniosu) */

  return 0;
}
#endif /* SYSVR2 */

/** signal SIGCHLD を受けた後の処理をする。*/
/* *INDENT-OFF* */
intfntype
chld_handler ()
/* *INDENT-ON* */
{
#ifdef HAVE_WAIT3
#ifdef linux
  int status;
#else
  union wait status;
#endif
  int pid;

/*
 * Remove warning.
 * Patched by Hidekazu Kuroki(hidekazu@cs.titech.ac.jp)         1996/8/20
 */
/*  if ((pid = wait3(&status, WNOHANG | WUNTRACED, NULL)) == child_id) { */
  if ((pid = wait3 ((int *) &status, WNOHANG | WUNTRACED, NULL)) == child_id)
    {
      if (WIFSTOPPED (status))
        {
#ifdef SIGCONT
          kill (pid, SIGCONT);
#ifndef SYSVR2
          pid = getpgrp (pid);
          killpg (pid, SIGCONT);
#else
#if defined(uniosu)
          pid = ngetpgrp (pid);
          kill (pid, SIGCONT);
#endif
#endif
#endif
        }
      else
        {
          signal (SIGCHLD, SIG_IGN);
          printf (MSG_GET (3));
          /*
             printf("\r\nｕｕｍを終わります。\r\n");
           */
          epilogue ();
          do_end ();
        }
    }
#else /* ! HAVE_WAIT3 */
  if (wait (0) == child_id)
    {
      signal (SIGCHLD, SIG_IGN);
      printf (MSG_GET (3));
      /*
         printf("\r\nｕｕｍを終わります。\r\n");
       */
      epilogue ();
      do_end ();
    }
#endif /* HAVE_WAIT3 */

  re_signal (SIGCHLD, chld_handler);
  SIGNAL_RETURN;
}

/** signal SIGTERM を受けた時の処理をする。*/
static intfntype
terminate_handler ()
{
  signal (SIGCHLD, SIG_IGN);
  epilogue_no_close ();
  do_end ();
  SIGNAL_RETURN;
}

#ifdef  SIGWINCH
/* *INDENT-OFF* */
intfntype
resize_handler ()
/* *INDENT-ON* */
{
  re_signal (SIGWINCH, resize_handler);
  change_size ();
  SIGNAL_RETURN;
}
#endif /* SIGWINCH */

/** メインループ */

int sel_ptn;
int pty_bit;
int tty_bit;
int ptyfd;

static void
do_main ()
{
  unsigned char *buf;
  int ml;

  if ((buf = (unsigned char *) malloc (maxchg * 4)) == NULL)
    {
      printf (MSG_GET (2));
      printf (MSG_GET (3));
      /*
         printf("malloc に失敗しました。ｕｕｍを終わります。\r\n");
       */
      epilogue ();
      do_end ();
    }

  pty_bit = 1 << ptyfd;
  tty_bit = 1 << ttyfd;
  sel_ptn = pty_bit | tty_bit;

  if (henkan_off_flag == 0)
    {
      disp_mode ();
    }

  for (;;)
    {

      ml = kk ();

/*
      if (!((ml < 2) && ((return_buf[0] <= ' ') || (return_buf[0] == 0x7f)))) {
*/
      make_history (return_buf, ml);
/*
      }
*/
      ml = (*code_trans[(internal_code << 2) | pty_c_flag]) (buf, return_buf, sizeof (w_char) * ml);
      if (ml > 0)
        write (ptyfd, buf, ml);
    }
}

unsigned char keyin0 ();

int
keyin2 ()
{
  int total, ret;
  unsigned char in;

  in = keyin0 ();
  if (in == 0xff)
    return (-1);
  total = (int) (in & 0xff);
  if (henkan_off_flag == 0 || pty_c_flag != tty_c_flag)
    {
      ret = get_cswidth_by_char (in);
      for (; ret > 1; ret--)
        {
          total = ((total & 0xff) << 8) + (int) (keyin0 () & 0xff);
        }
    }
  return (total);
}

/** convert_key nomi okonau key-in function */
int
conv_keyin (inkey)
     char *inkey;
{
  return keyin1 (keyin2, inkey);
}

/** キー入力関数 1 */
int
keyin ()
{
  char inkey[16];
  return (conv_keyin (inkey));
}

/** キー入力関数 2 */
unsigned char
keyin0 ()
{
  static unsigned char buf[BUFSIZ];
  static unsigned char outbuf[BUFSIZ];
  static unsigned char *bufend = outbuf;
  static unsigned char *bufstart = outbuf;
  int n;
  int rfds;
  int i, j, mask;
  unsigned char *p;
  extern int henkan_off_flag;
  struct timeval time_out;      /* If your OS's select was implemented as 
                                   a pointer for int, you must modify the
                                   time_out variable to integer           */
  int sel_ret;

  if (bufstart < bufend)
    {
      return (*bufstart++);
    }
  for (;;)
    {
      if ((n = read (ttyfd, buf, BUFSIZ)) > 0)
        {
          if (henkan_off_flag == 1)
            {
              if (tty_c_flag == pty_c_flag)
                {
                  i = through (outbuf, buf, n);
                }
              else
                {
                  i = (*code_trans[(tty_c_flag << 2) | file_code]) (outbuf, buf, n);
                }
            }
          else
            {
              i = (*code_trans[(tty_c_flag << 2) | file_code]) (outbuf, buf, n);
            }
          if (i <= 0)
            continue;
          bufstart = outbuf;
          bufend = outbuf + i;
          return (*bufstart++);
        }

      time_out.tv_sec = 0;
      time_out.tv_usec = 200 * 1000;    /* 200 msec 間待つのだゾ! */
      for (rfds = sel_ptn;
#ifdef linux
           (sel_ret = select (20, &rfds, 0, 0, NULL)) < 0 && errno == EINTR;
#else
           (sel_ret = select (20, &rfds, 0, 0, &time_out)) < 0 && errno == EINTR;
#endif
           rfds = sel_ptn)
        ;
      if (sel_ret == 0)
        {
          if ((tty_c_flag == J_JIS) && ((i = flush_designate ((w_char *) outbuf)) > 0))
            {
              /* 溜まっているＥＳＣを吐き出す */
              bufstart = outbuf;
              bufend = outbuf + i;
              return (*bufstart++);
            }
          return (0xff);
/*
            continue;
*/
        }

      if (rfds & pty_bit)
        {
          if ((n = read (ptyfd, buf, BUFSIZ)) <= 0)
            {
              epilogue ();
              do_end ();
            }
#if defined(uniosu)
          if (*buf == PIOCPKT_IOCTL)
            {
              arrange_ioctl (1);
            }
          else if (*buf == 0)
#endif /* defined(uniosu) */
            {                   /* sequence of data */
#if defined(uniosu)
              i = (*code_trans[(pty_c_flag << 2) | tty_c_flag]) (outbuf, buf + 1, n - 1);
#else /* defined(uniosu) */
              i = (*code_trans[(pty_c_flag << 2) | tty_c_flag]) (outbuf, buf, n);
#endif /* defined(uniosu) */
              if (i <= 0)
                continue;
              p = outbuf;
              push_cursor ();
              kk_restore_cursor ();
              while ((j = write (ttyfd, p, i)) < i)
                {
                  if (j >= 0)
                    {
                      p += j;
                      i -= j;
                    }
                  mask = 1 << ttyfd;
                  select (32, 0, &mask, 0, 0);
                }
              pop_cursor ();
            }
        }
      if (rfds & tty_bit)
        {
          if ((n = read (ttyfd, buf, BUFSIZ)) > 0)
            {
              if (henkan_off_flag == 1)
                {
                  if (tty_c_flag == pty_c_flag)
                    {
                      i = through (outbuf, buf, n);
                    }
                  else
                    {
                      i = (*code_trans[(tty_c_flag << 2) | file_code]) (outbuf, buf, n);
                    }
                }
              else
                {
                  i = (*code_trans[(tty_c_flag << 2) | file_code]) (outbuf, buf, n);
                }
              if (i <= 0)
                continue;
              bufstart = outbuf;
              bufend = outbuf + i;
              return (*bufstart++);
#ifdef nodef
            }
          else
            {                   /* Consider it as EOF */
              epilogue ();
              do_end ();
#endif /* It seems that select does not return EOF when  Non-brock
          What should I do? */
            }
        }
    }
}


#if defined(uniosu)
/** pty から ioctl がかかった時の処理 */
int
arrange_ioctl (jflg)
     int jflg;                  /* jtermio の j_flg の変換フラグがオフの時 0 オンの時 1 */
{
  struct jtermio jbuf1;
  struct termio frombuf;
  struct termio tobuf;
  int i;

  ioctl (ptyfd, TCGETA, &frombuf);
  ioctl (ttyfd, TCGETA, &tobuf);

  if ((i = (frombuf.c_iflag & IXON)) != (tobuf.c_iflag & IXON))
    {
      if (i == 0)
        {
          tobuf.c_iflag &= ~IXON;
        }
      else
        {
          tobuf.c_iflag |= IXON;
        }
    }
  if ((i = (frombuf.c_iflag & IXOFF)) != (tobuf.c_iflag & IXOFF))
    {
      if (i == 0)
        {
          tobuf.c_iflag &= ~IXOFF;
        }
      else
        {
          tobuf.c_iflag |= IXOFF;
        }
    }
  if ((i = (frombuf.c_oflag & OPOST)) != (tobuf.c_oflag & OPOST))
    {
      if (i == 0)
        {
          tobuf.c_oflag &= ~OPOST;
        }
      else
        {
          tobuf.c_oflag |= OPOST;
        }
    }
  tobuf.c_cflag = (tobuf.c_cflag & ~CBAUD) | (frombuf.c_cflag & CBAUD);

  ioctl (ttyfd, TCSETA, &tobuf);        /* set again */

  ioctl (ptyfd, JTERMGET, &jbuf1);      /* about Japanease */

  if ((jflg) && ((jbuf1.j_flg & KANJIINPUT) == 0))
    {
      jbuf1.j_flg &= ~(KANJIINPUT | CONVTOINT); /* kanji henkan flg off */
      ioctl (ttyfd, JTERMSET, &jbuf1);
      kk_restore_cursor ();
      reset_cursor ();
      longjmp (kk_env, 1);
    }
  if ((!jflg) && ((jbuf1.j_flg & KANJIINPUT) != 0))
    {
      jbuf1.j_flg &= ~(KANJIINPUT | CONVTOINT); /* kanji henkan flg off */
      ioctl (ttyfd, JTERMSET, &jbuf1);
      return (1);
    }
  jbuf1.j_flg &= ~(KANJIINPUT | CONVTOINT);     /* kanji henkan flg off */
  ioctl (ttyfd, JTERMSET, &jbuf1);
  return (0);
}
#endif /* defined(uniosu) */

/** 子プロセスを起こす。*/

int ttypfd;

static void
exec_cmd (argv)
     char **argv;
{
  int i;
#ifdef BSD42
  int pid;
#endif
#ifdef linux
  struct winsize win;
  extern Term_RowWidth, crow;
#endif
#if defined(SYSVR2) && !defined(linux)
  setpgrp ();
  close (open (ttyname (ttypfd), O_WRONLY, 0));
#endif /* SYSVR2 */

  if (!(child_id = fork ()))
    {
#ifdef BSD42
#ifdef TIOCNOTTY
      /* set notty */
      {
        int fd;
        if ((fd = open ("/dev/tty", O_WRONLY)) >= 0)
          {
            (void) ioctl (fd, TIOCNOTTY, 0);
            close (fd);
          }
      }
#endif /* TIOCNOTTY */
#ifdef TIOCSCTTY
      setsid ();
      ioctl (ttypfd, TIOCSCTTY, 0);
#endif /* TIOCSCTTY */
      /* set tty process group */
      pid = getpid ();
      ioctl (ttypfd, TIOCSPGRP, &pid);
      setpgrp (0, 0);
      close (open (ttyname (ttypfd), O_WRONLY, 0));
      setpgrp (0, pid);
#endif /* BSD42 */

#if !(defined(BSD) && (BSD >= 199306))
      if (setutmp (ttypfd) == ERROR)
        {
          puts ("Can't set utmp.");
        }
#endif

#ifdef linux
      setsid ();
      open_ttyp ();
      close (ptyfd);
      ioctl (ttyfd, TIOCGWINSZ, &win);
      ioctl (ttypfd, TCSETA, &savetmio);
      dup2 (ttypfd, 0);
      dup2 (ttypfd, 1);
      dup2 (ttypfd, 2);
#else /* linux */
      setgid (getgid ());
      setuid (getuid ());
      close (0);
      close (1);
      close (2);
      if (dup (ttypfd) != 0 || dup (ttypfd) != 1 || dup (ttypfd) != 2)
        {
          err ("redirection fault.");
        }
#endif /* linux */
#ifdef UX386
      for (i = NOFILES - 1; i > 2; i--)
        {
#else
      for (i = getdtablesize () - 1; i > 2; i--)
        {
#endif
          close (i);
        }

      restore_signals ();

#ifdef SIGTSTP
      signal (SIGTSTP, SIG_IGN);
#endif
#ifdef SIGTTIN
      signal (SIGTTIN, SIG_IGN);
#endif
#ifdef SIGTTOU
      signal (SIGTTOU, SIG_IGN);
#endif

/*      if (setutmp(0) == ERROR) {
            err("Can't set utmp.");
        } 
*/
#ifdef linux
      crow = win.ws_row = Term_RowWidth = win.ws_row - conv_lines;
      ioctl (ttyfd, TIOCSWINSZ, &win);
      setgid (getgid ());
      setuid (getuid ());
#endif
      execvp (cmdnm, argv);
      err ("exec fault.");
    }
}

/** 環境変数のセット */
void
setenv (var, value)
     char *var;
     char *value;
{
  extern char **environ;
  char *malloc ();
  char **newenv;
  register int i, j;

  j = strlen (var);
  for (i = 0; environ[i] != NULL; i++)
    {
      if (strncmp (var, environ[i], j) == 0 && environ[i][j] == '=')
        {
          break;
        }
    }
  if (environ[i] == NULL)
    {
      if ((newenv = (char **) malloc ((sizeof (char *)) * (i + 2))) == NULL)
        {
          puts ("Can't allocate environ.");
          exit (1);
        }
      for (j = 0; j < i + 1; j++)
        {
          newenv[j] = environ[j];
        }
      newenv[i + 1] = NULL;
      environ = newenv;
    }
  if ((environ[i] = malloc (strlen (var) + strlen (value) + 2)) == NULL)
    {
      puts ("Can't allocate environ area.");
      exit (1);
    }
  strcpy (environ[i], var);
  strcat (environ[i], "=");
  strcat (environ[i], value);
}

#ifdef SVR4
static int
euc_set (eucioc, ttyfd)
     eucioc_t *eucioc;
     int ttyfd;
{
  struct strioctl sb;

  sb.ic_cmd = EUC_WSET;
  sb.ic_timout = 0;
  sb.ic_len = sizeof (struct eucioc);
  sb.ic_dp = (char *) eucioc;
  if (ioctl (ttyfd, I_STR, &sb) < 0)
    {
      return (1);
    }
  return (0);
}

static int
set_euc_term (ttyfd)
     int ttyfd;
{
  eucioc_t eucioc;

  /* for Japanese EUC */
  eucioc.eucw[0] = 1;
  eucioc.eucw[1] = 2;
  eucioc.eucw[2] = 2;
  eucioc.eucw[3] = 3;
  eucioc.scrw[0] = 1;
  eucioc.scrw[1] = 2;
  eucioc.scrw[2] = 1;
  eucioc.scrw[3] = 2;
  if (euc_set (&eucioc, ttyfd) != 0)
    {
      fprintf (stderr, "eucwidth set failed\n");
      return (1);
    }
  return (0);
}

#endif /* SVR4 */

#ifdef nec_ews_svr2
static void
set_jterm (ttyfd, ttypfd)
     int ttyfd, ttypfd;
{
  struct jtermio buf;

  if (ioctl (ttyfd, TCJGETA, &buf) == -1)
    {
      fprintf (stderr, "error in ioctl TCJGETA.\n");
      exit (1);
    }
  buf.c_iflag = 0;
  buf.c_oflag = 0;
  if (ioctl (ttypfd, TCJSETA, &buf) < 0)
    {
      fprintf (stderr, "error in ioctl TCSETA.\n");
      exit (1);
    }
}
#endif /* nec_ews_svr2 */

/** ttyp のオープン */

int ptyno;
char *ptynm = "/dev/pty";
#ifdef sgi
extern char *_getpty (int *, int, mode_t, int);
char *ttypnm = "/dev/ttyqxxx";
#else
char *ttypnm = "/dev/tty";
#endif /* sgi */

#if defined(BSD43) || defined(DGUX)     /* FOR PASS8 */
static int local_mode_sv;
#endif

#ifndef sgi
static void ptyname ();
#endif

static void
open_ttyp ()
{
  char nmbuf[20];
#ifdef BSD42
  int word;
#endif
#if defined(SYSVR2) && !defined(linux)
  struct termio buf1;
#endif
#if defined(uniosu)
  struct jtermio buf2;
#endif

#ifdef sgi
  if ((ttypfd = open (ttypnm, O_RDWR)) == ERROR)
    {
#else
  ptyname (nmbuf, ttypnm, ptyno);
  if ((ttypfd = open (nmbuf, O_RDWR, 0)) == ERROR)
    {
#endif
      err ("Can't open ttyp.");
    }
#ifndef linux
  chown (nmbuf, getuid (), getgid ());
  chmod (nmbuf, 0622);
#endif /* !linux */
#ifdef BSD42
  ioctl (ttyfd, TIOCGETC, &tcharsv);
  ioctl (ttyfd, TIOCGLTC, &ltcharsv);
  ioctl (ttyfd, TIOCGETP, &ttyb);
  savetmio = ttyb;
#if defined(BSD43) || defined(DGUX)
  ioctl (ttyfd, TIOCLGET, &local_mode_sv);
  local_mode = local_mode_sv | LPASS8;  /* set PASS8 */
  ioctl (ttyfd, TIOCLSET, &local_mode);
#else /* defined(BSD43) || defined(DGUX) */
  ioctl (ttyfd, TIOCLGET, &local_mode);
#endif /* defined(BSD43) || defined(DGUX) */

  ioctl (ttypfd, TIOCSETC, &tcharsv);
  ioctl (ttypfd, TIOCSLTC, &ltcharsv);
  ioctl (ttypfd, TIOCSETP, &ttyb);
  ioctl (ttypfd, TIOCLSET, &local_mode);
#ifdef notdef
  pid = getpid ();
  ioctl (ttypfd, TIOCSPGRP, &pid);
#endif
  if (pty_c_flag == J_JIS)
    {
      word = LCTLECH;
      ioctl (ttypfd, TIOCLBIC, &word);
    }
#endif /* BSD42 */
#if defined(SYSVR2) && !defined(linux)
  if (ioctl (ttyfd, TCGETA, &buf1) == -1)
    {
      buf1 = terms;
    }
  savetmio = buf1;
#ifdef DGUX                     /* copied from JLS5.4.2 */
  buf1.c_iflag &= ~ISTRIP;
#endif /* DGUX */
#ifdef nec_ews_svr2
  buf1.c_line = JAPANLD;
#endif
  if (ioctl (ttypfd, TCSETA, &buf1) < 0)
    {
      fprintf (stderr, "error in ioctl TCSETA.\n");
      exit (1);
    }
#if defined(uniosu)
  if (ioctl (ttyfd, JTERMGET, &buf2) < 0)
    {
      fprintf (stderr, "uum: error in ioctl JTERMGET in open_ttyp.\n");
      exit (1);
    }
  savejtmiottyp = buf2;
  buf2.j_flg = CONVTOEXT | KANJIINPUT;  /* kanji input & output ok */
  buf2.j_level = jterm;
  switch (jcode_set)
    {
    case 0:
      buf2.j_ecode = JIS;
      break;
    case 1:
      buf2.j_ecode = SJIS;
      break;
    case 2:
      buf2.j_ecode = UJIS;
      break;
    default:
      fprintf (stderr, "uum: kanji code set not supported in terminfo.\n");
      exit (1);
    }

  if (jis_kanji_in)
    {
      strcpy (buf2.j_jst, jis_kanji_in);
      buf2.j_jstl = strlen (jis_kanji_in);
    }
  if (jis_kanji_out)
    {
      strcpy (buf2.j_jend, jis_kanji_out);
      buf2.j_jendl = strlen (jis_kanji_out);
    }
  if (jgaiji_start_address)
    {
      *(short *) buf2.j_gcsa = jgaiji_start_address;
    }
  if (jgaiji_disp)
    {
      strcpy (buf2.j_gdsp, jgaiji_disp);
      buf2.j_gdspl = strlen (jgaiji_disp);
    }

  if (ioctl (ttypfd, JTERMSET, &buf2) < 0)
    {
      fprintf (stderr, "error in ioctl JTERMSET.\n");
      exit (1);
    }

  if (ioctl (ttypfd, TIOCSETAUX, &auxterm) < 0)
    {
      fprintf (stderr, "error in ioctl TIOCSETAUX.\n");
      exit (1);
    }

/*
    pid = getpid();
*/
#endif /* defined(uniosu) */

#if defined(nec_ews_svr2)
  set_jterm (ttyfd, ttypfd);
#endif

/*
    setpgrp();
    close(open(ttyname(ttypfd), O_WRONLY, 0));
*/
#endif /* defined(SYSVR2) && !defined(linux) */
#ifdef TIOCSSIZE
  pty_rowcol.ts_lines = crow;   /* instead of lines */
  pty_rowcol.ts_cols = maxlength;       /* instead of columns */
  ioctl (ttypfd, TIOCSSIZE, &pty_rowcol);
#endif /* TIOCSSIZE */
}

/** pty のオープン */
#ifdef sgi
static void
open_pty ()
{
  char nmbuf[20];
  char *tty_name_buff;
  tty_name_buff = _getpty (&ptyfd, O_RDWR | O_NDELAY, 0600, 0);
  if (tty_name_buff == 0)
    err ("Can't get pty.");
  strcpy (ttypnm, tty_name_buff);
  return;

}
#else
static void
open_pty ()
{
  char nmbuf[20];
#ifdef linux
  struct termio buf1;
#endif

  for (ptyno = 0; ptyno < 32; ptyno++)
    {
      ptyname (nmbuf, ptynm, ptyno);
      if ((ptyfd = open (nmbuf, O_RDWR, 0)) != ERROR)
        {
#if defined(uniosu)
          if (ioctl (ptyfd, PIOCPKT, 1) < 0)
            {                   /* packet mode on */
              fprintf (stderr, "error in ioctl PIOCPKT.\n");
              exit (1);
            }
#endif
#ifdef linux
          if (ioctl (0, TCGETA, &buf1) == -1)
            {
              buf1 = terms;
            }
          savetmio = buf1;
#endif
          return;
        }
    }
  err ("Can't get pty.");
}
#endif

/** エラーだよ。さようなら。 */
void
err (s)
     char *s;
{
  puts (s);
  fclose (stdout);
  fclose (stderr);
  fclose (stdin);
  do_end ();
}

/** 立つ鳥後を濁さず 終わりの処理 */
static void
do_end ()
{
  char nmbuf[20];

  static int do_end_flg = 0;
  if (do_end_flg == 1)
    return;
  do_end_flg = 1;

  signal (SIGCHLD, SIG_DFL);
  fcntl (ttyfd, F_SETFL, 0);

#ifdef BSD42
  ioctl (ttyfd, TIOCSETP, &savetmio);
#endif /* BSD42 */
#if defined(BSD43) || defined(DGUX)
  ioctl (ttyfd, TIOCLSET, &local_mode_sv);
#endif /* BSD43 */

#ifdef SYSVR2
  if (ioctl (ttyfd, TCSETA, &savetmio) < 0)
    {
      fprintf (stderr, "error in ioctl TCSETA.\n");
      exit (1);
    }
#endif /* SYSVR2 */

#if defined(uniosu)
  if (ioctl (ttyfd, JTERMSET, &savejtmio) < 0)
    {
      fprintf (stderr, "error in ioctl JTERMSET.\n");
      exit (1);
    }
#endif /* defined(uniosu) */

#ifndef sgi
  ptyname (nmbuf, ptynm, ptyno);
  if (chown (nmbuf, 0, 0) == ERROR)
    {
      perror (prog);
    }
  if (chmod (nmbuf, 0666) == ERROR)
    {
      perror (prog);
    }

  ptyname (nmbuf, ttypnm, ptyno);
  if (chown (nmbuf, 0, 0) == ERROR)
    {
      perror (prog);
    }
  if (chmod (nmbuf, 0666) == ERROR)
    {
      perror (prog);
    }

#if !(defined(BSD) && (BSD >= 199306))
  if (resetutmp (ttypfd) == ERROR)
    {
      printf ("Can't reset utmp.");
    }
#endif
#ifdef TIOCSSIZE
  pty_rowcol.ts_lines = 0;
  pty_rowcol.ts_cols = 0;
  ioctl (ttypfd, TIOCSSIZE, &pty_rowcol);
#endif /* TIOCSSIZE */
#endif /* !sgi */
  close (ttyfd);
  close (ptyfd);

  chdir ("/tmp");               /* to avoid making too many mon.out files */


#ifdef BSD42
  killpg (child_id, SIGHUP);
/*    killpg(getpgrp(),SIGHUP);  */
#endif
#ifdef SYSVR2
/*    kill(0, SIGHUP); */
  kill (-child_id, SIGHUP);
#endif /* SYSVR2 */

  exit (0);
}

#if defined(uniosu)
/** 仮名漢字変換を ioctl でオフした時の keyin に代わる関数 */
int
ioctl_off ()
{
  static unsigned char buf[BUFSIZ];
  int n;
  int rfds;

  kk_restore_cursor ();
  clr_line_all ();
  display_henkan_off_mode ();

  for (;;)
    {
      if ((n = read (ttyfd, buf, BUFSIZ)) > 0)
        {
          write (ptyfd, buf, n);
        }
      rfds = sel_ptn;
      select (20, &rfds, 0, 0, NULL);
      if (rfds & pty_bit)
        {
          if ((n = read (ptyfd, buf, BUFSIZ)) <= 0)
            {
              epilogue ();
              do_end ();
            }
          if (*buf == 0)
            {                   /* sequence of data */
              write (ttyfd, buf + 1, n - 1);
            }
          else if (*buf == PIOCPKT_IOCTL)
            {
              if (arrange_ioctl (0) > 0)
                {
                  return;
                }
            }
        }
      if (rfds & tty_bit)
        {
          if ((n = read (ttyfd, buf, BUFSIZ)) > 0)
            {
              write (ptyfd, buf, n);
            }
        }
    }
}
#endif /* defined(uniosu) */


#ifndef sgi
static void
ptyname (b, pty, no)
     char *b, *pty;
     int no;
{
/*
 * Change pseudo-devices.
 * Because FreeBSD's master pseudo-devices are pty[p-sP-S][0-9a-v].
 * Patched by Hidekazu Kuroki(hidekazu@cs.titech.ac.jp)         1996/8/20
 */
#if (defined(BSD) && (BSD >= 199306))   /* 4.4BSD-Lite by Taoka */
  sprintf (b, "%s%1c%1c", pty, "pqrsPQRS"[(no >> 5)], ((no & 0x1f > 9) ? 'a' : '0') + (no & 0x1f));
#else /* ! 4.4BSD-Lite */
  sprintf (b, "%s%1c%1x", pty, 'p' + (no >> 4), no & 0x0f);
#endif /* ! 4.4BSD-Lite */
}
#endif /* !sgi */

/* Move to etc/xutoj.c
int
through(x, y, z)
char *x, *y;
int z;
{
    bcopy(y, x, z);
    return z;
}
*/

static void
default_usage ()
{
  fprintf (stderr, "%s: Bad -L option\n", prog);
  exit (0);
}

static void
usage (optstr)
     char *optstr;
{
  printf ("usage: prog %s by lang \"%s\"\n", optstr, lang_dir);
  exit (0);
}

/*
  save/restore signal settings
 */

intfnptr sigpipe, sighup, sigint, sigquit, sigterm, sigtstp, sigttin, sigttou, sigchld;
#ifdef SIGWINCH
intfnptr sigwinch;
#endif /* SIGWINCH */

static void
save_signals ()
{
  sigpipe = signal (SIGPIPE, SIG_IGN);
/*     sighup  = signal(SIGHUP,  SIG_IGN); /88/12/27*/
#ifdef linux                    /* XXX */
  sighup = signal (SIGHUP, SIG_IGN);
#endif
  sighup = signal (SIGHUP, terminate_handler);
  sigint = signal (SIGINT, SIG_IGN);
  sigquit = signal (SIGQUIT, SIG_IGN);
  sigterm = signal (SIGTERM, terminate_handler);
  sigchld = signal (SIGCHLD, chld_handler);
#ifdef SIGWINCH
  sigwinch = signal (SIGWINCH, resize_handler);
#endif /* SIGWINCH */
#ifdef SIGTSTP
  sigtstp = signal (SIGTSTP, SIG_IGN);
  sigttin = signal (SIGTTIN, SIG_IGN);
  sigttou = signal (SIGTTOU, SIG_IGN);
#endif /* SIGTSTP */
}

static void
restore_signals ()
{
  signal (SIGPIPE, sigpipe);
  signal (SIGHUP, sighup);
  signal (SIGINT, sigint);
  signal (SIGQUIT, sigquit);
  signal (SIGTERM, sigterm);
  signal (SIGCHLD, sigchld);
#ifdef SIGWINCH
  signal (SIGWINCH, sigwinch);
#endif /* SIGWINCH */
#ifdef SIGTSTP
  signal (SIGTSTP, sigtstp);
  signal (SIGTTIN, sigttin);
  signal (SIGTTOU, sigttou);
#endif /* SIGTSTP */
}

#if defined(BSD43) || defined(DGUX)
static void
setsize ()
{
  register int i;
  struct winsize win;
  extern Term_LineWidth, Term_RowWidth, maxlength, crow;

  if (ioctl (ttyfd, TIOCGWINSZ, &win) < 0)
    {
      /* Default set at getTermData() */
      return;
    }
  else
    {
      if ((i = win.ws_row) != 0)
        {
          crow = Term_RowWidth = i - conv_lines;
        }
      if ((i = win.ws_col) != 0)
        {
          maxlength = Term_LineWidth = i;
        }
    }
}
#endif /* BSD43 */

#ifdef  SIGWINCH
static void
change_size ()
{
  register int i;
  struct winsize win;
  extern Term_LineWidth, Term_RowWidth, maxlength, crow;

  if (ioctl (ttyfd, TIOCGWINSZ, &win) < 0)
    {
      /* Default set at getTermData() */
      return;
    }
  else
    {
      throw_cur_raw (0, crow);
      clr_line ();

      if ((i = win.ws_row) != 0)
        {
          crow = Term_RowWidth = i - conv_lines;
#ifdef linux
          win.ws_row = crow;
#endif
        }
      if ((i = win.ws_col) != 0)
        {
          maxlength = Term_LineWidth = i;
        }
#ifdef linux
      ioctl (ttypfd, TIOCSWINSZ, &win);
#else /* linux */
#ifdef  TIOCSSIZE
      pty_rowcol.ts_lines = crow;       /* instead of lines */
      pty_rowcol.ts_cols = maxlength;   /* instead of columns */
      ioctl (ttypfd, TIOCSSIZE, &pty_rowcol);
#endif /* TIOCSSIZE */
#ifdef  sun                     /* When your machine needs SIGWINCH, add your machine */
      {
        int grp;
        ioctl (ptyfd, TIOCGPGRP, &grp);
#ifdef SVR4
        kill (-grp, SIGWINCH);
#else
        killpg (grp, SIGWINCH);
#endif
      }
#endif /* sun */
#endif /* linux */

      set_scroll_region (0, crow - 1);
      if (henkan_off_flag)
        {
          kk_restore_cursor ();
          throw_cur_raw (0, 0);
          kk_save_cursor ();
          display_henkan_off_mode ();
          set_screen_vars_default ();
          t_print_l ();
          kk_restore_cursor ();
        }
      else
        {
          kk_restore_cursor ();
          throw_cur_raw (0, 0);
          kk_save_cursor ();
          disp_mode ();
          set_screen_vars_default ();
          t_print_l ();
        }
    }
}
#endif /* SIGWINCH */
