/*
 * $Id: do_xjpdrct.c,v 1.2 2001/06/14 18:16:15 ura Exp $
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

#if defined(XJPLIB) && defined(XJPLIB_DIRECT)

#include <stdio.h>
#include <X11/Xlib.h>
#ifdef  X11R4
#define NEED_EVENTS     1
#include <X11/Xlibos.h>
#else /* X11R4 */
#include "Xlibnet.h"

#ifndef X11R5
/* followings were included in Xlibnet.h, but none in X11R6 */
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <netdb.h>
/* end */
#endif /* !X11R5 */

#endif /* X11R4 */
#include <X11/Xos.h>
#if defined(TCPCONN) || defined(UNIXCONN)
#include <sys/socket.h>
#endif /* defined(TCPCONN) || defined(UNIXCONN) */
#ifdef  TCPCONN
#define XJP_PORT_IN    0x9494
#endif /* TCPCONN */

#ifdef  UNIXCONN
#include <sys/un.h>
#ifndef XJP_UNIX_PATH
#define XJP_UNIX_PATH   "/tmp/xwnmo.V2"
#endif /* XJP_UNIX_PATH */
#endif /* UNIXCONN */

#include "do_xjplib.h"

#define XJP_UNIX_ACPT   3
#define XJP_INET_ACPT   4

extern XIMCmblk accept_blk[];
extern XIMCmblk *cblk;
extern XIMCmblk *cur_cblk;

extern int *all_socks;
extern int *ready_socks;

extern int max_client;
extern int cur_sock;

#define BINTSIZE        (sizeof(int)*8)
#define sock_set(array,pos)     (array[pos/BINTSIZE] |= (1<<(pos%BINTSIZE)))
#define sock_clr(array,pos)     (array[pos/BINTSIZE] &= ~(1<<(pos%BINTSIZE)))
#define sock_tst(array,pos)     (array[pos/BINTSIZE] & (1<<(pos%BINTSIZE)))

extern int read (), write ();

extern XJpClientRec *xjp_clients;
extern XJpClientRec *xjp_cur_client;
extern XJpInputRec *xjp_inputs;
extern XJpInputRec *xjp_cur_input;

static int need_write = 0;

static int real_port = 0;

int
XJp_get_xjp_port ()
{
  return (real_port);
}

static void
xjp_init_net_area ()
{
#ifdef  UNIXCONN
  accept_blk[XJP_UNIX_ACPT].sd = -1;
#endif /* UNIXCONN */
#ifdef  TCPCONN
  accept_blk[XJP_INET_ACPT].sd = -1;
#endif /* TCPCONN */
}

#ifdef UNIXCONN
static int
xjp_init_net_un ()
{
  int so;
  struct sockaddr_un saddr_un;
  extern int unlink (), socket (), bind (), shutdown (), listen ();

  unlink (XJP_UNIX_PATH);
  saddr_un.sun_family = AF_UNIX;
  strcpy (saddr_un.sun_path, XJP_UNIX_PATH);

  if ((so = socket (AF_UNIX, SOCK_STREAM, 0)) < 0)
    {
      return (-1);
    }
  if (bind (so, (struct sockaddr *) &saddr_un, strlen (saddr_un.sun_path) + 2) < 0)
    {
      shutdown (so, 2);
      return (-1);
    }
  if (listen (so, 5) < 0)
    {
      shutdown (so, 2);
      return (-1);
    }
  accept_blk[XJP_UNIX_ACPT].sd = so;
  sock_set (all_socks, so);
  return (0);
}
#endif /* UNIXCONN */

#ifdef TCPCONN
static int
xjp_init_net_in ()
{
  int so;
  struct servent *sp;
  struct sockaddr_in saddr_in;
  unsigned short port;
#ifndef SOLARIS
  int on = 1;
#else /* SOLARIS */
  int on = 0;
#endif /* SOLARIS */
  int i, ok = 0;
  extern int setsockopt ();

  if ((sp = getservbyname ("xwnmo", "tcp")) == NULL)
    {
      real_port = port = XJP_PORT_IN;
    }
  else
    {
      real_port = port = ntohs (sp->s_port);
    }
  saddr_in.sin_family = AF_INET;
  saddr_in.sin_port = htons (port);
  saddr_in.sin_addr.s_addr = htonl (INADDR_ANY);

  for (i = 0; i < MAX_PORT_NUMBER; i++)
    {
      if ((so = socket (AF_INET, SOCK_STREAM, 0)) < 0)
        {
          return (-1);
        }
      setsockopt (so, SOL_SOCKET, SO_REUSEADDR, &on, sizeof (int));
      if (bind (so, (struct sockaddr *) &saddr_in, sizeof (struct sockaddr_in)) == 0)
        {
          ok = 1;
          break;
        }
      shutdown (so, 2);
      if (port == real_port)
        port += 0x10;
      else
        port += 1;
      saddr_in.sin_port = htons (port);
    }
  if (ok == 0)
    {
      return (-1);
    }
  real_port = port;
  if (listen (so, 5) < 0)
    {
      shutdown (so, 2);
      return (-1);
    }
  accept_blk[XJP_INET_ACPT].sd = so;
  sock_set (all_socks, so);
  return (0);
}
#endif /* TCPCONN */

void
XJp_init_net ()
{
  xjp_init_net_area ();
#ifdef  UNIXCONN
  if (xjp_init_net_un () < 0)
    {
      print_out ("Could not open UNIX domain socket for XJp protocol.");
    }
#endif /* UNIXCONN */
#ifdef  TCPCONN
  if (xjp_init_net_in () < 0)
    {
      print_out ("Could not open INET domain socket for XJp protocol.");
    }
#endif /* TCPCONN */
}

void
XJp_close_net ()
{
  int trueFlag = 1;
#ifdef  UNIXCONN
  struct sockaddr_un addr_un;
#endif /* UNIXCONN */
#ifdef  TCPCONN
  struct sockaddr_in addr_in;
#endif /* TCPCONN */
#if defined(UNIXCONN) || defined(TCPCONN)
  int addrlen;
  extern int accept (), close ();
#endif

#ifdef  UNIXCONN
  if (accept_blk[XJP_UNIX_ACPT].sd != -1)
    {
#ifndef SOLARIS
#if defined(FIONBIO)
      ioctl (accept_blk[XJP_UNIX_ACPT].sd, FIONBIO, &trueFlag);
#endif
#else /* !SOLARIS */
      fcntl (accept_blk[XJP_UNIX_ACPT].sd, F_SETFL, F_UNLCK);
#endif /* !SOLARIS */
      for (;;)
        {
          addrlen = sizeof (addr_un);
          if (accept (accept_blk[XJP_UNIX_ACPT].sd, (struct sockaddr *) &addr_un, &addrlen) < 0)
            break;
        }
      shutdown (accept_blk[XJP_UNIX_ACPT].sd, 2);
      close (accept_blk[XJP_UNIX_ACPT].sd);
    }
#endif /* UNIXCONN */

#ifdef  TCPCONN
  if (accept_blk[XJP_INET_ACPT].sd != -1)
    {
#ifndef SOLARIS
#if defined(FIONBIO)
      ioctl (accept_blk[XJP_INET_ACPT].sd, FIONBIO, &trueFlag);
#endif
#else /* !SOLARIS */
      fcntl (accept_blk[XJP_INET_ACPT].sd, F_SETFL, F_UNLCK);
#endif /* !SOLARIS */
      for (;;)
        {
          addrlen = sizeof (addr_in);
          if (accept (accept_blk[XJP_INET_ACPT].sd, (struct sockaddr *) &addr_in, &addrlen) < 0)
            break;
        }
      shutdown (accept_blk[XJP_INET_ACPT].sd, 2);
      close (accept_blk[XJP_INET_ACPT].sd);
    }
#endif /* TCPCONN */
}

int
XJp_return_sock (state, detail, open, keysym, string)
     short state, detail;
     int open;
     KeySym keysym;
     char *string;
{
  unsigned short tmp_short;
  unsigned long tmp_long;

  tmp_short = (unsigned short) htons (state);
  if (_WriteToClient (&tmp_short, sizeof (short)) == -1)
    return (-1);
  tmp_short = (unsigned short) htons (detail);
  if (_WriteToClient (&tmp_short, sizeof (short)) == -1)
    return (-1);

  if (open == 0 && state == (short) 0)
    {
      tmp_long = (unsigned long) htonl (keysym);
      if (_WriteToClient (&tmp_long, sizeof (KeySym)) == -1)
        return (-1);
      if (detail > (short) 0)
        {
          if (_WriteToClient (string, (int) detail) == -1)
            return (-1);
        }
      need_write = 0;
    }
  if (_Send_Flush () == -1)
    return (-1);
  return (0);
}

static int
xjp_new_cl_sock (fd)
     int fd;
{
  register XJpClientRec *xjp;
  register XIMClientRec *xc;
  short detail;
  char buf[32];
  ximICValuesReq ic_req;
  ximICAttributesReq pre_req, st_req;
  XCharStruct cs;

  if ((_ReadFromClient (buf, 2) == -1) || (_ReadFromClient (buf, (int) buf[1]) == -1))
    {
      return (-1);
    }
  if ((xjp = (XJpClientRec *) Malloc (sizeof (XJpClientRec))) == NULL)
    {
      XJp_return_sock ((short) -1, (short) 7, 1, (KeySym) 0, NULL);
      return (-1);
    }
  xjp->dispmode = XJP_ROOT;
  xjp->w = 0;
  XJp_xjp_to_xim (xjp, &ic_req, &pre_req, &st_req, &cs);
  if ((xc = create_client (&ic_req, &pre_req, &st_req, NULL, NULL, 0, 0, NULL, NULL, NULL, &detail)) == NULL)
    {
      Free ((char *) xjp);
      XJp_return_sock ((short) -1, (short) 7, 1, (KeySym) 0, NULL);
      return (-1);
    }
  if (XJp_return_sock ((short) 0, (short) 0, 1, (KeySym) 0, NULL) < 0)
    {
      Free ((char *) xjp);
      return (-1);
    }
  xc->xjp = 1;
  xjp->xim_client = xc;
  xjp->direct_fd = fd;
  xjp->next = xjp_clients;
  xjp->ref_count = 0;
  xjp_clients = xjp;
  return (0);
}

static int
xjp_new_client_acpt (fd)
     int fd;
{
#ifdef  UNIXCONN
  struct sockaddr_un addr_un;
#endif /* UNIXCONN */
#ifdef  TCPCONN
  struct sockaddr_in addr_in;
#endif /* TCPCONN */
#if defined(UNIXCONN) || defined(TCPCONN)
  register int i;
  int addrlen;
#endif

#ifdef  UNIXCONN
  if (accept_blk[XJP_UNIX_ACPT].sd != -1)
    {
      if (fd == accept_blk[XJP_UNIX_ACPT].sd)
        {
          for (i = 0; i < max_client; i++)
            {
              if (cblk[i].use == 0)
                {
                  addrlen = sizeof (addr_un);
                  if ((cblk[i].sd = accept (fd, (struct sockaddr *) &addr_un, &addrlen)) < 0)
                    {
                      return (-1);
                    }
                  cur_cblk = &cblk[i];
                  cur_cblk->use = 1;
                  cur_cblk->xjp = 1;
                  cur_cblk->byteOrder = False;
                  cur_sock = cur_cblk->sd;
                  if (xjp_new_cl_sock (cur_sock) == 0)
                    {
                      sock_set (all_socks, cur_sock);
                    }
                  return (0);
                }
            }
        }
    }
#endif /* UNIXCONN */
#ifdef  TCPCONN
  if (fd == accept_blk[XJP_INET_ACPT].sd)
    {
      for (i = 0; i < max_client; i++)
        {
          if (cblk[i].use == 0)
            {
              addrlen = sizeof (addr_in);
              if ((cblk[i].sd = accept (fd, (struct sockaddr *) &addr_in, &addrlen)) < 0)
                {
                  return (-1);
                }
              cur_cblk = &cblk[i];
              cur_cblk->use = 1;
              cur_cblk->xjp = 1;
              cur_cblk->byteOrder = False;
              cur_sock = cur_cblk->sd;
              if (xjp_new_cl_sock (cur_sock) == 0)
                {
                  sock_set (all_socks, cur_sock);
                }
              break;
            }
        }
    }
#endif /* TCPCONN */
  return (0);
}

int
XJp_wait_for_socket ()
{
#ifdef  UNIXCONN
  if (accept_blk[XJP_UNIX_ACPT].sd != -1)
    {
      if (sock_tst (ready_socks, accept_blk[XJP_UNIX_ACPT].sd))
        {
          sock_clr (ready_socks, accept_blk[XJP_UNIX_ACPT].sd);
          cur_cblk = NULL;
          cur_sock = -1;
          xjp_new_client_acpt (accept_blk[XJP_UNIX_ACPT].sd);
          return (XJP_DIRECT_TYPE);
        }
    }
#endif /* UNIXCONN */
#ifdef  TCPCONN
  if (accept_blk[XJP_INET_ACPT].sd != -1)
    {
      if (sock_tst (ready_socks, accept_blk[XJP_INET_ACPT].sd))
        {
          sock_clr (ready_socks, accept_blk[XJP_INET_ACPT].sd);
          cur_cblk = NULL;
          cur_sock = -1;
          xjp_new_client_acpt (accept_blk[XJP_INET_ACPT].sd);
          return (XJP_DIRECT_TYPE);
        }
    }
#endif /* TCPCONN */
  return (0);
}

void
XJp_destroy_for_sock (fd)
     int fd;
{
  XJpClientRec *p, **prev_p;
  XJpInputRec *i, **prev_i;

  for (prev_p = &xjp_clients; p = *prev_p; prev_p = &p->next)
    {
      if (p->direct_fd != -1 && p->direct_fd == fd)
        {
          if (p == xjp_cur_client)
            {
              xjp_cur_client = NULL;
              xjp_cur_input = NULL;
            }
          destroy_client (p->xim_client);
          for (prev_i = &xjp_inputs; i = *prev_i;)
            {
              if (i->pclient == p)
                {
                  *prev_i = i->next;
                  Free ((char *) i);
                }
              else
                {
                  prev_i = &i->next;
                }
            }
          *prev_p = p->next;
          Free ((char *) p);
          return;
        }
    }
}

static void
xjp_buffer_to_xevent (buf, ev)
     char *buf;
     XEvent *ev;
{
  ev->xkey.display = dpy;
  ev->xkey.type = (int) buf[0];
  ev->xkey.state = (unsigned int) buf[1];
  ev->xkey.serial = (unsigned long) ntohs (*(short *) (buf + 2));
  ev->xkey.time = (Time) ntohl (*(Time *) (buf + 4));
  ev->xkey.root = (Window) ntohl (*(Window *) (buf + 8));
  ev->xkey.window = (Window) ntohl (*(Window *) (buf + 12));
  ev->xkey.subwindow = (Window) ntohl (*(Window *) (buf + 16));
  ev->xkey.x_root = (int) ntohs (*(short *) (buf + 20));
  ev->xkey.y_root = (int) ntohs (*(short *) (buf + 22));
  ev->xkey.x = (int) ntohs (*(short *) (buf + 24));
  ev->xkey.y = (int) ntohs (*(short *) (buf + 26));
  ev->xkey.keycode = (unsigned int) ntohs (*(short *) (buf + 28));
  ev->xkey.same_screen = (Bool) buf[30];
}

static int
xjp_get_event (ev)
     XEvent *ev;
{
  char readbuf[64];
  register XJpClientRec *p;
  register XJpInputRec *i;

  if (_ReadFromClient (readbuf, 32) == -1)
    {
      return (-1);
    }
  xjp_buffer_to_xevent (readbuf, ev);
  for (p = xjp_clients; p; p = p->next)
    {
      if (p->direct_fd != -1 && p->direct_fd == cur_sock)
        {
          if (p->w == (Window) 0)
            {
              p->w = ev->xkey.window;
            }
          for (i = xjp_inputs; i; i = i->next)
            {
              if (i->pclient == p && i->w == ev->xkey.window)
                return (0);
            }
          if ((i = (XJpInputRec *) Malloc (sizeof (XJpInputRec))) == NULL)
            {
              malloc_error ("allocation of Focus data struct(XJp)");
              return (-1);
            }
          i->w = ev->xkey.window;
          i->pclient = p;
          i->save_event = 0;
          i->next = xjp_inputs;
          xjp_inputs = i;
          return (0);
        }
    }
  return (-1);
}

void
XJp_Direct_Dispatch ()
{
  XEvent ev;
  int buff[32], in;
  int ret, i, n_bytes;

  if (xjp_get_event (&ev) == -1)
    return;
  ret = key_input (buff, &ev);
  for (i = 0; i < ret;)
    {
      n_bytes = get_cswidth_by_char (in = buff[i++]);
      for (; n_bytes > 1 && i < ret; n_bytes--, i++)
        {
          in = (in << 8) + buff[i];
        }
      in_put (in);
    }
  if (need_write)
    {
      XJp_return_sock ((short) 0, (short) 0, 0, (KeySym) 0, NULL);
    }
  return;
}

static KeySym save_sock_keysym = (KeySym) 0;
static unsigned char save_sock_str[32];
static unsigned int save_sock_str_len = 0;

void
XJp_save_sockbuf (len, str, keysym)
     int len;
     unsigned char *str;
     KeySym keysym;
{
  if (xjp_cur_client && xjp_cur_client->direct_fd != -1)
    {
      need_write = 1;
      save_sock_keysym = keysym;
      save_sock_str_len = len;
      if (len > 0)
        {
          bcopy (str, save_sock_str, len);
        }
    }
}

void
XJp_direct_send_cl_key ()
{
  XJp_return_sock ((short) 0, (short) save_sock_str_len, 0, save_sock_keysym, save_sock_str);
}

#endif /* defined(XJPLIB) && defined(XJPLIB_DIRECT) */
