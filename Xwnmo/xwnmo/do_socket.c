/*
 * $Id: do_socket.c,v 1.2 2001/06/14 18:16:14 ura Exp $
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

#include <stdio.h>
#include <errno.h>
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
#define XIM_PORT_IN    0x9495
#endif /* TCPCONN */

#ifdef  UNIXCONN
#include <sys/un.h>
#ifndef XIM_UNIX_PATH
#define XIM_UNIX_PATH   "/tmp/.X11-unix/XIM"
#endif /* XIM_UNIX_PATH */
#endif /* UNIXCONN */

#include "sdefine.h"
#include "commonhd.h"
#include "xim.h"
#include "proto.h"
#include "ext.h"

#if defined(XJPLIB) && defined(XJPLIB_DIRECT)
#define MAX_ACCEPT      5
#else
#define MAX_ACCEPT      3
#endif /* defined(XJPLIB) && defined(XJPLIB_DIRECT) */

#define UNIX_ACPT       0
#define INET_ACPT       1
#define SERVER_ACPT     2

char my_byteOrder;

XIMCmblk accept_blk[MAX_ACCEPT];
XIMCmblk *cblk;
XIMCmblk *cur_cblk = NULL;

int *all_socks;
int *ready_socks;
static int *dummy1_socks, *dummy2_socks;
static int sel_width;

static int nofile;
int max_client;
int cur_sock = -1;
static int rest_length = 0;

#ifndef SEND_BUF_SZ
#define SEND_BUF_SZ     8192
#endif
#ifndef RECV_BUF_SZ
#define RECV_BUF_SZ     8192
#endif

static char send_buf[SEND_BUF_SZ];
static int sp = 0;
static char recv_buf[RECV_BUF_SZ];
static int rp = 0;
static int rc = 0;

#define BINTSIZE        (sizeof(int)*8)
#define sock_set(array,pos)     (array[pos/BINTSIZE] |= (1<<(pos%BINTSIZE)))
#define sock_clr(array,pos)     (array[pos/BINTSIZE] &= ~(1<<(pos%BINTSIZE)))
#define sock_tst(array,pos)     (array[pos/BINTSIZE] & (1<<(pos%BINTSIZE)))

extern int read (), write ();

#define _Read(fd, data, size)   read((fd), (data), (size))
#define _Write(fd, data, size)  write((fd), (data), (size))

static void
_Error ()
{
#ifndef X11R5
  XimError (cur_sock);
#endif
  close_socket (cur_sock);
}

static int
_Writen (num)
     register int num;
{
  register int i, ret;

  if (cur_sock < 0)
    return (-1);
  for (i = 0; i < num;)
    {
      ret = _Write (cur_sock, &send_buf[i], num - i);
      if (ret <= 0)
        {
          _Error ();
          return (-1);
        }
      i += ret;
    }
  return (0);
}

int
_Send_Flush ()
{
  if (sp == 0)
    {
      return (0);
    }
  if (_Writen (sp) == -1)
    {
      sp = 0;
      return (-1);
    }
  sp = 0;
  return (0);
}

#define my_bcopy(src, dst, size) {register int i; \
        register unsigned char *ppp, *qqq;\
        for(i=size,ppp=(unsigned char*)src,qqq=(unsigned char*)dst;i>0;i--) \
        *qqq++=*ppp++; \
}

int
_WriteToClient (p, num)
     register char *p;
     register int num;
{
  if (num <= 0)
    return (0);
  for (;;)
    {
      if ((sp + num) > SEND_BUF_SZ)
        {
          my_bcopy (p, &send_buf[sp], SEND_BUF_SZ - sp);
          if (_Writen (SEND_BUF_SZ) == -1)
            {
              sp = 0;
              return (-1);
            }
          num -= (SEND_BUF_SZ - sp);
          p += (SEND_BUF_SZ - sp);
          sp = 0;
        }
      else
        {
          my_bcopy (p, &send_buf[sp], num);
          sp += num;
          return (0);
        }
    }
}

int
_ReadFromClient (p, num)
     register char *p;
     register int num;
{
  register char *x = p;

  if (num <= 0)
    return (0);
  for (;;)
    {
      if (num > rc)
        {
          if (rc > 0)
            {
              my_bcopy (&recv_buf[rp], x, rc);
              x += rc;
              num -= rc;
              rc = 0;
            }
          rc = _Read (cur_sock, recv_buf, RECV_BUF_SZ);
          if (rc <= 0)
            {
              _Error ();
              rp = 0;
              rc = 0;
              return (-1);
            }
          rp = 0;
        }
      else
        {
          my_bcopy (&recv_buf[rp], x, num);
          rc -= num;
          rp += num;
          return (0);
        }
    }
}

static int
init_net_area ()
{
  register int sel_w;
  register int sel_bwidth;
  register char *p;

  nofile = OPEN_MAX;
  sel_w = (nofile - 1) / BINTSIZE + 1;
  sel_width = sel_w * sizeof (int);
  sel_bwidth = sel_width * 8;
  max_client = OPEN_MAX - MAX_ACCEPT;

  p = (char *) Malloc ((4 * sel_width) + (max_client * sizeof (XIMCmblk)));
  if (!p)
    {
      malloc_error ("allocation of dates for socket");
      return (-1);
    }
  all_socks = (int *) p;
  p += sel_width;
  ready_socks = (int *) p;
  p += sel_width;
  dummy1_socks = (int *) p;
  p += sel_width;
  dummy2_socks = (int *) p;
  p += sel_width;
  bzero (all_socks, sel_width);
  bzero (ready_socks, sel_width);

  cblk = (XIMCmblk *) p;
  bzero ((char *) cblk, max_client * sizeof (XIMCmblk));
  bzero ((char *) accept_blk, MAX_ACCEPT * sizeof (XIMCmblk));
#ifdef  UNIXCONN
  accept_blk[UNIX_ACPT].sd = -1;
#endif /* UNIXCONN */
#ifdef  TCPCONN
  accept_blk[INET_ACPT].sd = -1;
#endif /* TCPCONN */
  return (0);
}

#ifdef  UNIXCONN
static int
init_net_un ()
{
  int so;
  struct sockaddr_un saddr_un;
  extern int unlink (), socket (), bind (), shutdown (), listen ();

  unlink (XIM_UNIX_PATH);
  saddr_un.sun_family = AF_UNIX;
  strcpy (saddr_un.sun_path, XIM_UNIX_PATH);

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
  accept_blk[UNIX_ACPT].sd = so;
  sock_set (all_socks, so);
  return (0);
}
#endif /* UNIXCONN */

#ifdef  TCPCONN
static short
init_net_in ()
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

  if ((sp = getservbyname ("xim", "tcp")) == NULL)
    {
      port = XIM_PORT_IN;
    }
  else
    {
      port = ntohs (sp->s_port);
    }
  saddr_in.sin_family = AF_INET;
  saddr_in.sin_port = htons (port);
  saddr_in.sin_addr.s_addr = htonl (INADDR_ANY);

  for (i = 0; i < MAX_PORT_NUMBER; i++)
    {
      if ((so = socket (AF_INET, SOCK_STREAM, 0)) < 0)
        {
          return ((short) -1);
        }
      setsockopt (so, SOL_SOCKET, SO_REUSEADDR, &on, sizeof (int));
      if (bind (so, (struct sockaddr *) &saddr_in, sizeof (struct sockaddr_in)) == 0)
        {
          ok = 1;
          break;
        }
      shutdown (so, 2);
      saddr_in.sin_port = htons (++port);
    }
  if (ok == 0)
    {
      return ((short) -1);
    }
  if (listen (so, 5) < 0)
    {
      shutdown (so, 2);
      return ((short) -1);
    }
  accept_blk[INET_ACPT].sd = so;
  sock_set (all_socks, so);
  return ((short) port);
}
#endif /* TCPCONN */

short
init_net (fd, displayname)
     int fd;
     char *displayname;
{
  short port = -1;
  int indian = 1;

  if (*(char *) &indian)
    {
      my_byteOrder = 'l';
    }
  else
    {
      my_byteOrder = 'B';
    }
  if (init_net_area () < 0)
    return (port);
#ifdef  UNIXCONN
  if (!strncmp (displayname, "unix:", 5) || !strncmp (displayname, ":", 1))
    {
      if (init_net_un () == (short) -1)
        {
          print_out ("Could not open UNIX domain socket.");
        }
    }
#endif /* UNIXCONN */
#ifdef  TCPCONN
  if ((port = init_net_in ()) == (short) -1)
    {
      print_out ("Could not open INET domain socket.");
      return (port);
    }
  accept_blk[SERVER_ACPT].sd = fd;
  sock_set (all_socks, fd);
#endif /* TCPCONN */
#if defined(XJPLIB) && defined(XJPLIB_DIRECT)
  XJp_init_net ();
#endif /* defined(XJPLIB) && defined(XJPLIB_DIRECT) */
  return (port);
}

void
close_net ()
{
  int i;
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
  if (accept_blk[UNIX_ACPT].sd != -1)
    {
#ifndef SOLARIS
#if defined(FIONBIO)
      ioctl (accept_blk[UNIX_ACPT].sd, FIONBIO, &trueFlag);
#endif
#else /* !SOLARIS */
      fcntl (accept_blk[UNIX_ACPT].sd, F_SETFL, F_UNLCK);
#endif /* !SOLARIS */
      for (;;)
        {
          addrlen = sizeof (addr_un);
          if (accept (accept_blk[UNIX_ACPT].sd, (struct sockaddr *) &addr_un, &addrlen) < 0)
            break;
        }
      shutdown (accept_blk[UNIX_ACPT].sd, 2);
      close (accept_blk[UNIX_ACPT].sd);
    }
#endif /* UNIXCONN */

#ifdef  TCPCONN
  if (accept_blk[INET_ACPT].sd != -1)
    {
#ifndef SOLARIS
#if defined(FIONBIO)
      ioctl (accept_blk[INET_ACPT].sd, FIONBIO, &trueFlag);
#endif
#else /* !SOLARIS */
      fcntl (accept_blk[INET_ACPT].sd, F_SETFL, F_UNLCK);
#endif /* !SOLARIS */
      for (;;)
        {
          addrlen = sizeof (addr_in);
          if (accept (accept_blk[INET_ACPT].sd, (struct sockaddr *) &addr_in, &addrlen) < 0)
            break;
        }
      shutdown (accept_blk[INET_ACPT].sd, 2);
      close (accept_blk[INET_ACPT].sd);
    }
#endif /* TCPCONN */
#if defined(XJPLIB) && defined(XJPLIB_DIRECT)
  XJp_close_net ();
#endif /* defined(XJPLIB) && defined(XJPLIB_DIRECT) */

  for (i = 0; i < max_client; i++)
    {
      if (cblk[i].use)
        {
          shutdown (cblk[i].sd, 2);
          close (cblk[i].sd);
        }
    }
}

#if defined(UNIXCONN) || defined(TCPCONN)
static int
get_screen_number (p)
     char *p;
{
  register char *s;

  if (!(s = index (p, ':')))
    return (-1);
  if (!(s = index (s, '.')))
    return (-1);
  if (!*(++s))
    return (-1);
  return (atoi (s));
}

#if defined(X11R5) || defined(BC_X11R5)
static void
new_cl_sock (b)
     XIMCmblk *b;
{
  char displaybuf[256];
  ximConnClient client;
  ximNormalReply reply;
  int len;

  if (_ReadFromClient (&client, sz_ximConnClient) == -1)
    return;
  if (my_byteOrder != client.byteOrder)
    {
      b->byteOrder = True;
    }
  else
    {
      b->byteOrder = False;
    }
  if (need_byteswap () == True)
    {
      client.length = byteswap_s (client.length);
    }
  if (_ReadFromClient (displaybuf, client.length) == -1)
    return;
  displaybuf[client.length] = '\0';
  if ((len = strlen (displaybuf)) > 0)
    {
      b->screen = get_screen_number (displaybuf);
    }
  else
    {
      b->screen = -1;
    }
  reply.state = 0;
  if (need_byteswap () == True)
    {
      byteswap_NormalReply (&reply);
    }
  if (_WriteToClient (&reply, sz_ximNormalReply) == -1)
    return;
  if (_Send_Flush () == -1)
    return;
}
#endif /* defined(X11R5) || defined(BC_X11R5) */

#ifndef X11R5
static void
xim_new_cl_sock (b)
     XIMCmblk *b;
{
#ifdef BC_X11R5
  extern int ximr5_client;
  b->ximr5 = ximr5_client;
  if (b->ximr5)
    {
      new_cl_sock (b);
      return;
    }
  ximr5_client = 1;
#endif /* BC_X11R5 */
  /* nothing to do, all are done in XimPreConnect() */
}
#endif /* !X11R5 */

#endif /* defined(UNIXCONN) || defined(TCPCONN) */

static int
new_client_acpt (fd)
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
  if (accept_blk[UNIX_ACPT].sd != -1)
    {
      if (fd == accept_blk[UNIX_ACPT].sd)
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
#if defined(XJPLIB) && defined(XJPLIB_DIRECT)
                  cur_cblk->xjp = 0;
#endif /* defined(XJPLIB) && defined(XJPLIB_DIRECT) */
                  cur_sock = cur_cblk->sd;
                  sock_set (all_socks, cur_sock);
#ifdef X11R5
                  new_cl_sock (cur_cblk);
#else
                  xim_new_cl_sock (cur_cblk);
#endif /* X11R5 */
                  return (0);
                }
            }
        }
    }
#endif /* UNIXCONN */
#ifdef  TCPCONN
  if (fd == accept_blk[INET_ACPT].sd)
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
#if defined(XJPLIB) && defined(XJPLIB_DIRECT)
              cur_cblk->xjp = 0;
#endif /* defined(XJPLIB) && defined(XJPLIB_DIRECT) */
              cur_sock = cur_cblk->sd;
              sock_set (all_socks, cur_sock);
#ifdef X11R5
              new_cl_sock (cur_cblk);
#else
              xim_new_cl_sock (cur_cblk);
#endif /* X11R5 */
              break;
            }
        }
    }
#endif /* TCPCONN */
  return (0);
}

int
wait_for_socket ()
{
  register int i;
  int n, ret;
  extern int errno;
  extern int select ();

  /*
   * If recv_buf has some data, return with as cur_sock get data
   */
  if (cur_sock != -1 && rc > 0)
    {
#if defined(XJPLIB) && defined(XJPLIB_DIRECT)
      if (cur_cblk && cur_cblk->xjp)
        return (XJP_DIRECT_TYPE);
#endif /* defined(XJPLIB) && defined(XJPLIB_DIRECT) */
#ifdef X11R5
      return (REQUEST_TYPE);
#else
#ifdef BC_X11R5
      if (cur_cblk && cur_cblk->ximr5)
        return REQUEST_TYPE;
      else
#endif /* BC_X11R5 */
        return REQUEST_TYPE_XIM;
#endif /* X11R5 */
    }
  if (!*ready_socks)
    {
      my_bcopy (all_socks, ready_socks, sel_width);
      bzero (dummy1_socks, sel_width);
      bzero (dummy2_socks, sel_width);

      if ((n = select (nofile, ready_socks, dummy1_socks, dummy2_socks, (struct timeval *) NULL)) == -1)
        {
          if (errno != EINTR)
            do_end ();
          else
            {
              bzero (ready_socks, sel_width);
              return (0);
            }
        }
    }
#ifdef  UNIXCONN
  if (accept_blk[UNIX_ACPT].sd != -1)
    {
      if (sock_tst (ready_socks, accept_blk[UNIX_ACPT].sd))
        {
          sock_clr (ready_socks, accept_blk[UNIX_ACPT].sd);
          cur_cblk = NULL;
          cur_sock = -1;
          new_client_acpt (accept_blk[UNIX_ACPT].sd);
          return (0);
        }
    }
#endif /* UNIXCONN */
#ifdef  TCPCONN
  if (accept_blk[INET_ACPT].sd != -1)
    {
      if (sock_tst (ready_socks, accept_blk[INET_ACPT].sd))
        {
          sock_clr (ready_socks, accept_blk[INET_ACPT].sd);
          cur_cblk = NULL;
          cur_sock = -1;
          new_client_acpt (accept_blk[INET_ACPT].sd);
          return (0);
        }
    }
#endif /* TCPCONN */
  if (sock_tst (ready_socks, accept_blk[SERVER_ACPT].sd))
    {
      sock_clr (ready_socks, accept_blk[SERVER_ACPT].sd);
      cur_cblk = NULL;
      cur_sock = -1;
      return (XEVENT_TYPE);
    }
  else
    {
      if (cur_sock != -1 && sock_tst (ready_socks, cur_sock))
        {
          sock_clr (ready_socks, cur_sock);
#if defined(XJPLIB) && defined(XJPLIB_DIRECT)
          if (cur_cblk && cur_cblk->xjp)
            return (XJP_DIRECT_TYPE);
#endif /* defined(XJPLIB) && defined(XJPLIB_DIRECT) */
#ifdef X11R5
          return (REQUEST_TYPE);
#else
#ifdef BC_X11R5
          if (cur_cblk && cur_cblk->ximr5)
            return REQUEST_TYPE;
          else
#endif /* BC_X11R5 */
            return REQUEST_TYPE_XIM;
#endif /* X11R5 */
        }
      cur_cblk = NULL;
      cur_sock = -1;
      for (i = 0; i < max_client; i++)
        {
          if (cblk[i].use && sock_tst (ready_socks, cblk[i].sd))
            {
              sock_clr (ready_socks, cblk[i].sd);
              cur_cblk = &cblk[i];
              cur_sock = cur_cblk->sd;
#if defined(XJPLIB) && defined(XJPLIB_DIRECT)
              if (cur_cblk && cur_cblk->xjp)
                return (XJP_DIRECT_TYPE);
#endif /* defined(XJPLIB) && defined(XJPLIB_DIRECT) */
#ifdef X11R5
              return (REQUEST_TYPE);
#else
#ifdef BC_X11R5
              if (cur_cblk && cur_cblk->ximr5)
                return REQUEST_TYPE;
              else
#endif /* BC_X11R5 */
                return REQUEST_TYPE_XIM;
#endif /* X11R5 */
            }
        }
#if defined(XJPLIB) && defined(XJPLIB_DIRECT)
      if (ret = XJp_wait_for_socket ())
        return (0);
#endif /* defined(XJPLIB) && defined(XJPLIB_DIRECT) */
      return (0);
    }
}

void
close_socket (fd)
     int fd;
{
  int i;
  for (i = 0; i < max_client; i++)
    {
      if (fd == cblk[i].sd && cblk[i].use == 1)
        {
          cblk[i].use = 0;
          shutdown (cblk[i].sd, 2);
          sock_clr (all_socks, cblk[i].sd);
          close (cblk[i].sd);
#if defined(XJPLIB) && defined(XJPLIB_DIRECT)
          if (cblk[i].xjp)
            XJp_destroy_for_sock (cblk[i].sd);
          else
#endif /* defined(XJPLIB) && defined(XJPLIB_DIRECT) */
            destroy_for_sock (cblk[i].sd);
          if (cur_sock == fd)
            cur_sock = -1;
          return;
        }
    }
}

int
get_cur_sock ()
{
  return (cur_sock);
}

int
get_rest_len ()
{
  return (rest_length);
}

int
read_requestheader ()
{
  ximRequestHeader request_header;

  if (_ReadFromClient (&request_header, sz_ximRequestHeader) == -1)
    {
      return (-1);
    }
  else
    {
      if (need_byteswap () == True)
        request_header.length = byteswap_s (request_header.length);
      rest_length = request_header.length - sz_ximRequestHeader;
      return (request_header.reqType);
    }
}

int
read_strings (p, p_len, r_len)
     char **p;
     int *p_len, r_len;
{
  register int len;
  register char *ptr;
  char tmp;

  if (r_len > 0)
    {
      if (*p_len <= r_len)
        {
          len = r_len + 1;
          if (!(ptr = Malloc (len)))
            {
              malloc_error ("allocation of client data");
              for (; r_len > 0; r_len--)
                {               /* read data in dummy */
                  if (_ReadFromClient (&tmp, 1) == 1)
                    return (-1);
                }
              return (-2);
            }
          if (*p)
            Free (*p);
          *p = ptr;
          *p_len = len;
        }
      if (_ReadFromClient (*p, r_len) == 1)
        return (-1);
      p[0][r_len] = '\0';
    }
  else
    {
      if (*p_len > 0)
        **p = '\0';
    }
  return (0);
}

/* *INDENT-OFF* */
Bool
need_byteswap ()
/* *INDENT-ON* */

{
  if (!cur_cblk)
    return (False);
  return (cur_cblk->byteOrder);
}

int
get_client_screen ()
{
  if (!cur_cblk)
    return (-1);
  return (cur_cblk->screen);
}
