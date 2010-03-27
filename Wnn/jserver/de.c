/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright FreeWnn Project 1999, 2000, 2001, 2002, 2003
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
        Jserver         (Nihongo Daemon)
*/
static char rcs_id[] = "$Id: de.c,v 1.36 2004/06/18 16:32:41 hiroo Exp $";

#if defined(HAVE_CONFIG_H)
#  include <config.h>
#endif
#include "getopt.h"

#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <signal.h>
#if STDC_HEADERS
#  include <stdlib.h>
#  include <string.h>
#else
#  if HAVE_MALLOC_H
#    include <malloc.h>
#  endif
#  if HAVE_STRINGS_H
#    include <strings.h>
#  endif
#endif /* STDC_HEADERS */
#include <sys/ioctl.h>
#include <sys/stat.h>
#if TIME_WITH_SYS_TIME
#  include <sys/time.h>
#  include <time.h>
#else
#  if HAVE_SYS_TIME_H
#    include <sys/time.h>
#  else
#    include <time.h>
#  endif /* HAVE_SYS_TIME_H */
#endif /* TIME_WITH_SYS_TIME */
#if HAVE_UNISTD_H
#  include <sys/types.h>
#  include <unistd.h>
#endif
#ifdef HAVE_FCNTL_H
#  include <fcntl.h>
#endif
#if HAVE_SYS_PARAM_H
#  include <sys/param.h>
#endif

#include "getopt.h"

#include "commonhd.h"
#include "wnn_config.h"
#include "jd_sock.h"
#include "demcom.h"
#include "wnn_os.h"
#define GLOBAL_VALUE_DEFINE 1
#include "de_header.h"
#undef  GLOBAL_VALUE_DEFINE
#include "msg.h"

#ifdef SOLARIS
#ifdef SO_DONTLINGER
#undef SO_DONTLINGER
#endif
#endif /* SOLARIS */

#if defined(HAVE_SETSOCKOPT) && defined(SOL_SOCKET) && defined(SO_NONBLOCK)
#define USE_SETSOCKOPT 1
#else
#undef USE_SETSOCKOPT
#endif

#ifndef min
#define min(x,y)        ( (x)<(y) ? (x) : (y) )
#endif

#ifndef INET6
#  define OPTIONARGS  "Df:s:h:N:p:vu4"
#else
#  define OPTIONARGS  "Df:s:h:N:p:vu46"
#endif /* INET6 */

/*      Accept Socket   */
#ifdef INET6
#define MAX_ACCEPTS     3
#else
#define MAX_ACCEPTS     2
#endif
#define UNIX_ACPT       0
#define INET_ACPT       1
#ifdef INET6
#define INET6_ACPT      2
#endif

#define PROTO_ALL	0x1
#define PROTO_UN	0x2
#define PROTO_INET	0x4
#ifdef INET6
#define PROTO_INET6	0x8
#endif
static int listen_proto = PROTO_ALL;

jmp_buf client_dead;

static int port;
static int serverNO = 0;

struct cmblk
{
  int sd;                       /** ソケットのfd        **/
};
#define COMS_BLOCK      struct  cmblk

static COMS_BLOCK *cblk;
                /** クライアントごとのソケットを管理するテーブル **/

static COMS_BLOCK accept_blk[MAX_ACCEPTS];      /*accept sock blocks */


/*      Communication Buffers           */
static char snd_buf[R_BUF_SIZ];         /** 送信バッファ **/
static int sbp;                         /** 送信バッファポインタ **/

static int rbc;                         /** 受信バッファcounter **/
static char rcv_buf[S_BUF_SIZ];         /** 受信バッファ **/

#if defined(EAGAIN)
# if defined(EWOULDBLOCK)
# define ERRNO_CHECK(no)        ((no) == EAGAIN || (no) == EWOULDBLOCK)
# else /* defined(EWOULDBLOCK) */
# define ERRNO_CHECK(no)        ((no) == EAGAIN)
# endif /* defined(EWOULDBLOCK) */
#else /* defined(EAGAIN) */
# if defined(EWOULDBLOCK)
# define ERRNO_CHECK(no)        ((no) == EWOULDBLOCK)
# else /* defined(EWOULDBLOCK) */
# define ERRNO_CHECK(no)        (0)
# endif /* defined(EWOULDBLOCK) */
#endif /* defined(EAGAIN) */

/*      Client Table    */
int clientp;            /** cblkの有効なデータの最後を差している **/

int cur_clp;            /** 現在のクライアントの番号 **/

static fd_set *all_socks;	/** ビットパターン
				    which jserver to select を保持 **/
static fd_set *ready_socks;	/** データのきているソケットの
				    ビットパターンを保持 **/
static fd_set *dummy1_socks, *dummy2_socks;
static int no_of_ready_socks;
static int nofile;              /** No. of files **/
struct msg_cat *wnn_msg_cat;
struct msg_cat *js_msg_cat;

/* function prototypes */
static void daemon_main (void);
static void socket_disc_init (void);
static void sel_all (void);
static int  get_client (void);
static void new_client (void);
static void daemon_init (void);
static void daemon_fin_un (int);
static void daemon_fin_in (int);
static int  rcv_1_client (int);
static void snd_1_client (int, int);
static void socket_init_un (void);
static void socket_init_in (void);
static int  socket_accept_un (void);
static int  socket_accept_in (int);
static void xerror (char*);
static void get_options (int, char **);
static void usage (void);
static void print_version (void);
#ifdef DEBUG
static void dmp (char*, int);
#endif

static char cmd_name[16];

#if defined(HAVE_LIBWRAP)
int allow_severity;
int deny_severity;
#  include <syslog.h>
#  include <tcpd.h>
#endif /* HAVE_LIBWRAP */

/* No arguments are used. Only options. */
int
main (int argc, char *argv[])
{
  int tmpttyfd;
  char *cswidth_name;
  extern char *get_cswidth_name ();
  extern void set_cswidth ();

  char nlspath[64];

  strcpy (cmd_name, WNN_DAEMON_NAME);
  strcpy (lang_dir, LANG_NAME);
  strcpy (nlspath, LIBDIR);
  strcat (nlspath, "/%L/%N");
  js_msg_cat = msg_open (MESSAGE_FILE, nlspath, lang_dir);
  wnn_msg_cat = msg_open ("libwnn.msg", nlspath, lang_dir);
  if (wnn_msg_cat == NULL)
    {
      log_err ("cannot open message file libwnn.msg.");
    }
  if (cswidth_name = get_cswidth_name (LANG_NAME))
    set_cswidth (create_cswidth (cswidth_name));

  port = -1;
  /* option default */
  option_flag = (OPT_FORK & ~OPT_VERBOSE);

  setuid (geteuid ());
  get_options (argc, argv);
  print_version();
  log_debug("invoked as %s.", argv[0]);
  if (option_flag & OPT_FORK)
    {
      if (fork ())
	{
	  signal (SIGCHLD, _exit);
	  signal (SIGHUP, SIG_IGN);
	  signal (SIGINT, SIG_IGN);
	  signal (SIGQUIT, SIG_IGN);
#ifdef  SIGTSTP
	  signal (SIGTSTP, SIG_IGN);
#endif
	  signal (SIGTERM, _exit);
	  pause ();
	}
    }

#if defined(HAVE_LIBWRAP)
  allow_severity = LOG_INFO;
  deny_severity = LOG_WARNING;
  /*  hosts_access_verbose = 2; */
#endif /* HAVE_LIBWRAP */

  signal (SIGHUP, signal_hand);
  signal (SIGINT, signal_hand);
  signal (SIGQUIT, signal_hand);
  signal (SIGTERM, terminate_hand);
  if (option_flag & OPT_FORK)
    {
#ifdef  SIGTSTP
      signal (SIGTSTP, SIG_IGN);
#endif /* SIGTSTP */
    }
  read_default ();
  daemon_init ();

  env_init ();
  if (file_init () == NULL)
    {
      exit (1);
    }
  dic_init ();
  if (NULL == get_kaiseki_area (LENGTHCONV + 1))    /* 変換可能文字数 */
    {
      log_err ("get_kaiseki_area failed.");
      exit (1);
    }
  init_work_areas ();
  init_jmt ();

  read_default_files ();

  if (option_flag & OPT_FORK)
    {
      /* End of initialization, kill parent */
      kill (getppid (), SIGTERM);
      fclose (stdin);
      fclose (stdout);
      if (!(option_flag & OPT_VERBOSE))
	{
#if !(defined(BSD) && (BSD >= 199306))  /* !4.4BSD-Lite by Taoka */
	  fclose (stderr);
#else /* 4.4BSD-Lite */
	  int fd = open ("/dev/null", O_WRONLY);
	  if (fd < 0)
	    {
	      xerror ("Cannot open /dev/null");
	    }
	  dup2 (fd, 2);
	  close (fd);
#endif /* 4.4BSD-Lite */
	}

#ifdef SETPGRP_VOID
      setpgrp ();
#else /* !SETPGRP_VOID */
# if !defined(TIOCNOTTY) && defined(SVR4)
#  define TIOCNOTTY     _IO('t', 113)
# endif /* !defined(TIOCNOTTY) && defined(SVR4) */
#ifndef HITACHI
      if ((tmpttyfd = open ("/dev/tty", O_RDWR)) >= 0)
	{
	  ioctl (tmpttyfd, TIOCNOTTY, 0);
	  close (tmpttyfd);
	}
#endif /* HITACHI */
#endif /* SETPGRP_VOID */
    }

  daemon_main ();

  daemon_fin ();
  return (0);	/* it is not reached. only for avoiding compiler warning. */
}

static void
daemon_main (void)
{
  for (;;)
    {
      c_c = NULL;	/* Added for logging: server section */
      sel_all ();
      new_client ();
      for (;;)
        {
          if (get_client () == -1)
            break;
          c_c = &client[cur_clp];
          rbc = 0;
          sbp = 0;
/*      if(rcv_1_client(cur_clp) == 0){ del_client(); continue; }       */
          if (setjmp (client_dead))
            {
              del_client ();
              continue;
            }
          do_command (c_c);
        }
    }
}

/*
        allocs area for selecting socketts
*/
static void
socket_disc_init (void)
{
  if (WNN_NFD <= FD_SETSIZE)
    {
      nofile = WNN_NFD;
    }
  else
    {
      nofile = FD_SETSIZE;
    }
  all_socks = (fd_set *) malloc (sizeof (fd_set));
  FD_ZERO (all_socks);
  ready_socks = (fd_set *) malloc (sizeof (fd_set));
  dummy1_socks = (fd_set *) malloc (sizeof (fd_set));
  dummy2_socks = (fd_set *) malloc (sizeof (fd_set));
}

/**     全てのソケットについて待つ      **/
static void
sel_all (void)
{
  memcpy (ready_socks, all_socks, sizeof (fd_set));
  bzero (dummy1_socks, sizeof (fd_set));
  bzero (dummy2_socks, sizeof (fd_set));

top:
  errno = 0;
  if ((no_of_ready_socks = select (nofile, ready_socks, dummy1_socks, dummy2_socks, NULL)) == -1)
    {
      if (errno == EINTR)
        goto top;
      xerror ("select error");
    }
#ifdef DEBUG
  log_debug ("select OK, ready_socks[0]=%02X, n-r-s=%x\n", ready_socks[0], no_of_ready_socks);
#endif
}

/**     ready_socksから今やる一人を取り出して返す(cur_clpにもセットする)
        誰も居ない時は-1
        スケジューリングはラウンドロビン        **/
static int
get_client (void)
{
  int i;

  if (no_of_ready_socks == 0)
    return -1;                  /* no client waits service */

  for (i = cur_clp;;)
    {
      if (no_of_ready_socks == 0)
        return -1;
      i++;
      if (i >= clientp)
        i = 0;
      if (FD_ISSET (cblk[i].sd, ready_socks))
        {
	  FD_CLR (cblk[i].sd, ready_socks);
          no_of_ready_socks--;
          return cur_clp = i;
        }
    }
}

/**     新しいクライアントが居るか否かを調べる
        居た場合はcblkに登録する        **/
static void
new_client (void)               /* NewClient */
{
  int sd;
  int full, i;
  FILE *f[3];
  char gomi[1024];
#ifdef  HAVE_LIBWRAP
  int is_internet_socket;
  struct request_info tcpd_request;
#endif /* HAVE_LIBWRAP */
#ifdef  AF_UNIX
  if ((serverNO == 0) &&
      (FD_ISSET (accept_blk[UNIX_ACPT].sd, ready_socks)))
    {
      FD_CLR (accept_blk[UNIX_ACPT].sd, ready_socks);
      no_of_ready_socks--;
      sd = socket_accept_un ();
#ifdef  HAVE_LIBWRAP
      is_internet_socket = 0;
#endif
    }
  else
#endif
#ifdef INET6
  if (FD_ISSET (accept_blk[INET6_ACPT].sd, ready_socks))
    {
      FD_CLR (accept_blk[INET6_ACPT].sd, ready_socks);
      no_of_ready_socks--;
      sd = socket_accept_in (accept_blk[INET6_ACPT].sd);
#ifdef  HAVE_LIBWRAP
      is_internet_socket = 1;
#endif
    }
  else
#endif
  if (FD_ISSET (accept_blk[INET_ACPT].sd, ready_socks))
    {
      FD_CLR (accept_blk[INET_ACPT].sd, ready_socks);
      no_of_ready_socks--;
      sd = socket_accept_in (accept_blk[INET_ACPT].sd);
#ifdef  HAVE_LIBWRAP
      is_internet_socket = 1;
#endif
    }
  else
    {
      return;
    }
  log_debug ("new client: sd = %d", sd);
  /* reserve 2 fd */
  for (full = i = 0; i < 2; i++)
    {
      if (NULL == (f[i] = fopen ("/dev/null", "r")))
        {
          full = 1;
        }
    }
  for (i = 0; i < 2; i++)
    {
      if (NULL != f[i])
        fclose (f[i]);
    }

  if (full || sd >= nofile || clientp >= max_client)
    {
      log_err ("no more client.");
#ifdef HAVE_RECV
      recv (sd, gomi, 1024, 0);
#else
      read (sd, gomi, 1024);
#endif
      shutdown (sd, 2);
#ifdef HAVE_CLOSESOCKET
      closesocket (sd);
#else
      close (sd);
#endif
      return;
    }

#ifdef HAVE_LIBWRAP
  if (is_internet_socket) {
    request_init (&tcpd_request,RQ_DAEMON, WNN_DAEMON_NAME,
		  RQ_FILE, sd, NULL);
    fromhost (&tcpd_request);
    if (!hosts_access (&tcpd_request))
      {
	log_err ("reject client."); /* should be log_info? */
				    /* should we log IP address / hostname? */
#ifdef HAVE_RECV
	recv (sd, gomi, 1024, 0);
#else
	read (sd, gomi, 1024);
#endif
	shutdown (sd, 2);
#ifdef HAVE_CLOSESOCKET
	closesocket (sd);
#else
	close (sd);
#endif
	return;
      }
  }
#endif /*  HAVE_LIBWRAP */

  cblk[clientp].sd = sd;
  FD_SET (sd, all_socks);
  for (i = 0; i < WNN_MAX_ENV_OF_A_CLIENT; i++)
    {
      (client[clientp].env)[i] = -1;
    }
  clientp++;
}

/**     クライアントをcblkから削除する  **/
/* delete Client (please call from JS_CLOSE) */
void
del_client (void)
{
  disconnect_all_env_of_client ();
  FD_CLR (cblk[cur_clp].sd, all_socks);
#ifdef HAVE_CLOSESOCKET
  closesocket (cblk[cur_clp].sd);
#else
  close (cblk[cur_clp].sd);
#endif
  /* logging here because c_c (used in log_debug) will be broken after
     following section */
  log_debug("Delete Client: cur_clp = %d\n", cur_clp);
  cblk[cur_clp] = cblk[clientp - 1];
  client[cur_clp] = client[clientp - 1];
  /* Clear host/user name with zero - needed for logging */
  client[clientp - 1].user_name[0] = '\0';	/* Should we use bzero()? */
  client[clientp - 1].host_name[0] = '\0';
  clientp--;
}


/**     サーバをイニシャライズする      **/
static void
daemon_init (void)               /* initialize Daemon */
{
  /*
  signal (SIGHUP, SIG_IGN);
  signal (SIGINT, SIG_IGN);
  signal (SIGQUIT, SIG_IGN);
  */


  if ((cblk = (COMS_BLOCK *) malloc (max_client * sizeof (COMS_BLOCK))) == NULL)
    {
      xerror ("daemon_init: ");
    }
  if ((client = (CLIENT *) malloc (max_client * sizeof (CLIENT))) == NULL)
    {
      xerror ("daemon_init: ");
    }
  SDRAND (time (NULL));
  clientp = 0;                  /* V3.0 */
  cur_clp = 0;                  /* V3.0 */
  socket_disc_init ();
#ifdef INET6
  if (listen_proto&(PROTO_ALL|PROTO_INET|PROTO_INET6))
      socket_init_in ();
#else
  if (listen_proto&(PROTO_ALL|PROTO_INET))
      socket_init_in ();
#endif
#ifdef  AF_UNIX
  if (listen_proto&(PROTO_ALL|PROTO_UN))
      socket_init_un ();
#endif /* AF_UNIX */
}

/**     サーバを終わる  **/
#ifdef  AF_UNIX
static void
daemon_fin_un (int sock_d_un)
{
  int trueFlag = 1;
  struct sockaddr_un addr_un;
  socklen_t addrlen;

  if (serverNO == 0)
    {
#ifndef SOLARIS
#if defined(FIONBIO)
      ioctl (sock_d_un, FIONBIO, &trueFlag);
#endif
#else /* !SOLARIS */
      fcntl (sock_d_un, F_SETFL, F_UNLCK);
#endif /* !SOLARIS */
      for (;;)
        {
          addrlen = sizeof (addr_un);
          if (accept (sock_d_un, (struct sockaddr *) &addr_un, &addrlen) < 0)
            break;
          /* EWOULDBLOCK EXPECTED, but we don't check */
        }
      shutdown (sock_d_un, 2);
      close (sock_d_un);
    }
}
#endif /* AF_UNIX */

static void
daemon_fin_in (int sock_d_in)
{
  int trueFlag = 1;
  struct sockaddr_in addr_in;
  socklen_t addrlen;
#ifdef USE_SETSOCKOPT
  int on = ~0;
#endif

#ifndef SOLARIS
#ifdef USE_SETSOCKOPT
  setsockopt (sock_d_in, SOL_SOCKET, SO_NONBLOCK, &on, sizeof (int));
#else
#if defined(FIONBIO)
  ioctl (sock_d_in, FIONBIO, &trueFlag);
#endif
#endif /* USE_SETSOCKOPT */
#else /* !SOLARIS */
  fcntl (sock_d_in, F_SETFL, F_UNLCK);
#endif /* !SOLARIS */
  for (;;)
    {
      addrlen = sizeof (addr_in);
      if (accept (sock_d_in, (struct sockaddr *) &addr_in, &addrlen) < 0)
        break;
      /* EWOULDBLOCK EXPECTED, but we don't check */
    }
  shutdown (sock_d_in, 2);
#ifdef HAVE_CLOSESOCKET
  closesocket (sock_d_in);
#else
  close (sock_d_in);
#endif
}

void
daemon_fin (void)
{
  int fd;
#ifdef  AF_UNIX
  int sock_d_un = accept_blk[UNIX_ACPT].sd;
#endif /* AF_UNIX */
  int sock_d_in = accept_blk[INET_ACPT].sd;
#ifdef INET6
  int sock_d_in6 = accept_blk[INET6_ACPT].sd;
#endif

  /*
     accept all pending connection from new clients,
     avoiding kernel hangup.
   */
#ifdef  AF_UNIX
  daemon_fin_un (sock_d_un);
#endif
  daemon_fin_in (sock_d_in);
#ifdef INET6
  daemon_fin_in (sock_d_in6);
#endif

  for (fd = nofile - 1; fd >= 0; fd--)
    {
      if ((fd != sock_d_in) &&
#ifdef INET6
          (fd != sock_d_in6) &&
#endif
#ifdef AF_UNIX
          (fd != sock_d_un) &&
#endif /* AF_UNIX */
          FD_ISSET (fd, all_socks))
        {
          shutdown (fd, 2);
#ifdef HAVE_CLOSESOCKET
          closesocket (fd);
#else
          close (fd);
#endif
        }
    }
}

/*------*/

/**     **/
char *
gets_cur (char *buffer, size_t buffer_size)
{
  char *b;

  if (!buffer || !buffer_size)
    return NULL;

  b = buffer;

  while (--buffer_size && (*b = getc_cur ()) != '\0')
    b++;

  if (!buffer_size)
    {
      *b = '\0';
      while (getc_cur () != '\0')
        ;
    }

  return buffer;
}

/**     **/
w_char *
getws_cur (w_char *buffer, size_t buffer_size)
{
  w_char *b;

  if (!buffer || !buffer_size)
    return NULL;

  b = buffer;

  while (--buffer_size && (*b = get2_cur ()) != 0)
    b++;

  if (!buffer_size)
    {
      *b = 0;
      while (getc_cur () != 0)
        ;
    }

  return buffer;
}

/**     カレント・クライアントから2バイト取る   **/
int
get2_cur (void)
{
  int x;
  x = getc_cur ();
  return (x << 8) | getc_cur ();
}

/**     カレント・クライアントから4バイト取る   **/
int
get4_cur (void)
{
  int x1, x2, x3;
  x1 = getc_cur ();
  x2 = getc_cur ();
  x3 = getc_cur ();
  return (x1 << (8 * 3)) | (x2 << (8 * 2)) | (x3 << (8 * 1)) | getc_cur ();
}

/**     カレント・クライアントから1バイト取る   **/
int
getc_cur (void)
{
  static int rbp;
  if (rbc <= 0)
    {
      rbc = rcv_1_client (cur_clp);
      rbp = 0;
    }
  rbc--;
  return rcv_buf[rbp++] & 0xFF;
}

/**     クライアントから1パケット取る   **/
static int
rcv_1_client (int clp)		/* clp=クライアント番号 */
{
  int cc = 0;
  while (cc <= 0)
    {
      errno = 0;
#ifdef HAVE_RECV
      cc = recv (cblk[clp].sd, rcv_buf, S_BUF_SIZ, 0);
#else
      cc = read (cblk[clp].sd, rcv_buf, S_BUF_SIZ);
#endif
      if (cc <= 0)
        {
          if (ERRNO_CHECK (errno))
            {
              continue;
            }
          else if (cc == 0)
            {                   /* client dead */
              longjmp (client_dead, 666);
            }
          else
            {                   /* cc == -1 */
              if (errno != EINTR)
                longjmp (client_dead, 666);
              continue;
            }
        }
    }
#ifdef DEBUG
  log_debug ("rcv: clp = %d, sd = %d, cc = %d", clp, cblk[clp].sd, cc);
  dmp (rcv_buf, cc);
#endif
  return cc;
}

/**     クライアントへ1パケット送る     **/
static void
snd_1_client (int clp,	/* clp: クライアント番号 */
	      int n	/* n : number of bytes to send */ )
{
  int cc, x;
#ifdef  DEBUG
  log_debug ("snd: clp = %d, sd = %d", clp, cblk[clp].sd);
  dmp (snd_buf, n);
#endif
  for (cc = 0; cc < n;)
    {
      errno = 0;
#ifdef HAVE_SEND
      x = send (cblk[clp].sd, &snd_buf[cc], n - cc, 0);
#else
      x = write (cblk[clp].sd, &snd_buf[cc], n - cc);
#endif
      if (x < 0)
        {
          if (ERRNO_CHECK (errno) || errno == EINTR)
            {
              errno = 0;
              continue;
            }
          else
            {                   /* client dead */
              longjmp (client_dead, 666);
            }
        }
      cc += x;
    }
}

/**     **/
void
puts_cur (char *p)
{
  int c;
  while (c = *p++)
    putc_cur (c);
  putc_cur (0);
}

/**     **/
void
puts_n_cur (char *p, int n)
{
  int c;
  while ((c = *p++) && --n >= 0)
    putc_cur (c);
  putc_cur (0);
}

/**     **/
void
putws_cur (w_char *p)
{
  int c;
  while (c = *p++)
    put2_cur (c);
  put2_cur (0);
}

/**     **/
void
putnws_cur (w_char *p, int n)
{
  int c;
  for (; n > 0; n--)
    {
      if ((c = *p++) == 0)
        break;
      put2_cur (c);
    }
  put2_cur (0);
}

/**     カレント・クライアントへ2バイト送る     **/
void
put2_cur (int c)
{
  putc_cur (c >> (8 * 1));
  putc_cur (c);
}

/**     カレント・クライアントへ4バイト送る     **/
void
put4_cur (int c)
{
  putc_cur (c >> (8 * 3));
  putc_cur (c >> (8 * 2));
  putc_cur (c >> (8 * 1));
  putc_cur (c);
}

/**     カレント・クライアントへ1バイト送る     **/
void
putc_cur (int c)
{
  snd_buf[sbp++] = c;
  if (sbp >= R_BUF_SIZ)
    {
      snd_1_client (cur_clp, R_BUF_SIZ);
      sbp = 0;
    }
}

/**     カレント・クライアントの送信バッファをフラッシュする    **/
void
putc_purge (void)
{
  if (sbp != 0)
    {
      snd_1_client (cur_clp, sbp);
      sbp = 0;
    }
}

/*-----*/

/**     ソケットのイニシャライズ        **/
#ifdef  AF_UNIX
static void
socket_init_un (void)
{
  struct sockaddr_un saddr_un;
  int sock_d_un;
  if (serverNO == 0)
    {
      saddr_un.sun_family = AF_UNIX;
      unlink (sockname);
      strcpy (saddr_un.sun_path, sockname);
      if ((sock_d_un = socket (AF_UNIX, SOCK_STREAM, 0)) == ERROR)
        {
          xerror ("could not create unix domain socket");
        }
      if (bind (sock_d_un, (struct sockaddr *) &saddr_un, strlen (saddr_un.sun_path) + 2) == ERROR)
        {
          shutdown (sock_d_un, 2);
          xerror ("could not bind unix domain socket");
        }
      if (listen (sock_d_un, 5) == ERROR)
        {
          shutdown (sock_d_un, 2);
          xerror ("could not listen unix domain socket");
        }
      chmod (sockname, 0777);
      signal (SIGPIPE, SIG_IGN);
#ifdef DEBUG
      log_debug ("sock_d_un = %d", sock_d_un);
#endif
      accept_blk[UNIX_ACPT].sd = sock_d_un;
      FD_SET (sock_d_un, all_socks);
    }
}
#endif /* AF_UNIX */

/*      Inet V3.0 */
static void
socket_init_in (void)
{
#ifndef SOLARIS
  int on = 1;
#else /* SOLARIS */
  int on = 0;
#endif /* SOLARIS */
  struct servent *sp;
#if !defined(SO_DONTLINGER) && defined(SO_LINGER)
  struct linger linger;
#endif
#ifdef INET6
  struct addrinfo hints, *res, *res0;
  int error;
  char sport[6];
#else
  struct sockaddr_in saddr_in;
#endif
  int sock_d_in;

  if (port < 0)
    {
      if ((sp = getservbyname (SERVERNAME, "tcp")) == NULL)
        {
          port = WNN_PORT_IN;
        }
      else
        {
          port = ntohs (sp->s_port);
        }
    }

  port += serverNO;

#if DEBUG
  log_debug ("port=%x", port);
#endif
#ifdef INET6
  memset(&hints, 0, sizeof(hints));
  if (listen_proto&PROTO_INET && !(listen_proto&PROTO_INET6))
      hints.ai_family = PF_INET;
  else if (listen_proto&PROTO_INET6 && !(listen_proto&PROTO_INET))
      hints.ai_family = PF_INET6;
  else
      hints.ai_family = PF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  sprintf(sport, "%d", port);
  error = getaddrinfo(NULL, sport, &hints, &res0);
  if (error)
    {
      xerror (gai_strerror(error));
    }
  for (res = res0; res; res = res->ai_next) {
    if (res->ai_family == AF_INET || res->ai_family == AF_INET6){
      if ((sock_d_in = socket (res->ai_family, res->ai_socktype, res->ai_protocol)) == ERROR)
#else
  saddr_in.sin_family = AF_INET;
  saddr_in.sin_port = htons (port);
  saddr_in.sin_addr.s_addr = htonl (INADDR_ANY);
  if ((sock_d_in = socket (AF_INET, SOCK_STREAM, 0)) == ERROR)
#endif
    {
#ifdef INET6
      if (res->ai_family == AF_INET6)
        xerror ("could not create inet6 socket");
      else if (res->ai_family == AF_INET)
#endif
      xerror ("could not create inet socket");
    }
  setsockopt (sock_d_in, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof (int));
#ifdef SO_DONTLINGER
  setsockopt (sock_d_in, SOL_SOCKET, SO_DONTLINGER, (char *) 0, 0);
#else
# ifdef SO_LINGER
  linger.l_onoff = 0;
  linger.l_linger = 0;
  setsockopt (sock_d_in, SOL_SOCKET, SO_LINGER, (char *) &linger, sizeof linger);
# endif /* SO_LINGER */
#endif /* SO_DONTLINGER */

#ifdef INET6
  if (bind (sock_d_in, res->ai_addr, res->ai_addrlen) == ERROR)
#else
  if (bind (sock_d_in, (struct sockaddr *) &saddr_in, sizeof (saddr_in)) == ERROR)
#endif
    {
      shutdown (sock_d_in, 2);
#ifdef INET6
      if (res->ai_family == AF_INET6)
        xerror ("can't bind inet6 socket");
      else if (res->ai_family == AF_INET)
#endif
      xerror ("can't bind inet socket");
    }
  if (listen (sock_d_in, 5) == ERROR)
    {
      shutdown (sock_d_in, 2);
#ifdef INET6
      if (res->ai_family == AF_INET6)
        xerror ("can't listen inet6 socket");
      else if (res->ai_family == AF_INET)
#endif
      xerror ("can't listen inet socket");
    }
#if DEBUG
  log_debug ("sock_d_in = %d", sock_d_in);
#endif
  FD_SET (sock_d_in, all_socks);
#ifdef INET6
      if (res->ai_family == AF_INET)
	accept_blk[INET_ACPT].sd = sock_d_in;
      else if (res->ai_family == AF_INET6)
	accept_blk[INET6_ACPT].sd = sock_d_in;
    }
  }
  freeaddrinfo(res0);
#else
  accept_blk[INET_ACPT].sd = sock_d_in;
#endif
}


/**     accept new client socket        **/
#ifdef  AF_UNIX
static int
socket_accept_un (void)
{
  struct sockaddr_un addr;
  socklen_t addrlen;

  addrlen = sizeof (addr);
  return accept (accept_blk[UNIX_ACPT].sd, (struct sockaddr *) &addr, &addrlen);
}
#endif /* AF_UNIX */

static int
socket_accept_in (int fd)
{
  struct sockaddr_in addr;
  socklen_t addrlen;

  addrlen = sizeof (addr);
  return accept (fd, (struct sockaddr *) &addr, &addrlen);
}

static void
xerror (char *s)
{
  log_err ("%s (%s).", s, strerror(errno));
  exit (1);
}

#if DEBUG
static void
dmp (char *p, int n)
{
  int i, j;

  for (i = 0; i < n; i += 16)
    {
      for (j = 0; j < 16; j++)
        {
          fprintf (stderr, "%02x ", p[i + j] & 0xFF);
        }
      fprintf (stderr, "n=%d\n", n);
    }
}
#endif

static void
get_options (int argc, char **argv)
{
  int c;
  int digit_optind = 0;

  strcpy (jserverrcfile, LIBDIR);       /* usr/local/lib/wnn */
  strcat (jserverrcfile, SERVER_INIT_FILE);     /* ja_JP/jserverrc */

  while (1)
    {
      int this_option_optind = optind ? optind : 1;
      int option_index = 0;
      static struct option long_options[] =
      {
	{"baseport",	1, NULL, 'p'},
	{"inet",	0, NULL, '4'},
	{"inet6",	0, NULL, '6'},
	{"jserverrc",	1, NULL, 'f'},
	{"version",	0, NULL, 'v'},
	{0, 0, 0, 0}
      };

      c = getopt_long (argc, argv, OPTIONARGS,
		       long_options, &option_index);
      if (c == -1)
        break;

      switch (c)
	{
	case 'D': /* do not detach, not a daemon */
	  option_flag &= ~OPT_FORK;
	  break;

        case 'f': /* --jserverrc FILENAME */
          strcpy (jserverrcfile, optarg);
          break;

        case 's':
	  /* should nuke noisy someday */
          noisy = 1; option_flag |= OPT_VERBOSE;
          if (strcmp ("-", optarg) != 0)
            {
	      /** maybe FILE wnnerr = stderr; or wnnerr = open(optarg...) is better? or freopen is normal method? */
	      /** take a look at daemon(3) */
              if (freopen (optarg, "a", stderr) == NULL)
                {
		  /** fprintf to stderr? */
                  printf ("Error in opening scriptfile %s.\n", optarg);
                  exit (1);
                }
            }
          log_debug ("script started");
          break;

        case 'h':
	  /* var hinsi_file_name polluted */
          hinsi_file_name = optarg; 
          break;

        case 'N':
          serverNO = atoi (optarg);
	  /* error handling needed */
          break;

        case 'p':
          port = atoi (optarg);
	  /* error handling needed */
          break;

        case 'v':
          print_version();
          usage();
	  break;

        case 'u':
	  listen_proto &= ~PROTO_ALL;
	  listen_proto |= PROTO_UN;
	  break;

        case '4':
	  listen_proto &= ~PROTO_ALL;
	  listen_proto |= PROTO_INET;
	  break;

#ifdef INET6
        case '6':
	  listen_proto &= ~PROTO_ALL;
	  listen_proto |= PROTO_INET6;
          break;
#endif /* INET6 */

        default:
          print_version();
	  usage();
	  break;
	}
    }
}


/*
*/
void
js_who (void)
{
  int i, j;

  put4_cur (clientp);
  for (i = 0; i < clientp; i++)
    {
      put4_cur (cblk[i].sd);
      puts_cur (client[i].user_name);
      puts_cur (client[i].host_name);
      for (j = 0; j < WNN_MAX_ENV_OF_A_CLIENT; j++)
        {
          put4_cur ((client[i].env)[j]);
        }

    }
  putc_purge ();
}

void
js_kill (void)
{
  if (clientp == 1)
    {
      put4_cur (0);
      putc_purge ();
      terminate_hand ();
    }
  else
    {
      put4_cur (clientp - 1);
      putc_purge ();
    }
}

void
usage (void)
{
  fprintf(stderr, 
#ifdef INET6
	  "usage: %s [-Du46][-f <init_file> -s <log_file(\"-\" for stderr)> -h <pos_file> -N <serverNO> -p <port_base>]\n",
#else
	  "usage: %s [-Du4][-f <init_file> -s <log_file(\"-\" for stderr)> -h <pos_file> -N <serverNO> -p <port_base>]\n",
#endif
	  cmd_name);
  fprintf(stderr,
	  "       %s -v\n",
	  cmd_name);
  exit (1);
}

void
print_version (void)
{
#if  defined(CHINESE)
  printf ("%s (%s) Chinese Multi Client Server\n", cmd_name, SER_VERSION);
#elif  defined(KOREAN)
  printf ("%s (%s) Korean Multi Client Server\n", cmd_name, SER_VERSION);
#else
  printf ("%s (%s) Nihongo Multi Client Server\n", cmd_name, SER_VERSION);
#endif /* CHINESE */
}
