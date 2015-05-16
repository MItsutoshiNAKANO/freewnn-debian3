/*
 * $Id: printf.c,v 1.8.2.1 1999/02/08 08:08:53 yamasita Exp $
 */

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
 *
 * Last modified date: 8,Feb.1999
 *
 * Code:
 *
 */
/*      Version 4.0
        xwnmo
 */
#include <stdio.h>
#include "commonhd.h"
#include "sdefine.h"
#ifdef  XJUTIL
#include "xjutil.h"
#include "sxheader.h"
#include "xext.h"
#else /* XJUTIL */
#include "xim.h"
#include "sheader.h"
#include "ext.h"
#endif /* XJUTIL */
#include "jllib.h"

#ifdef  XJUTIL
extern int cursor_colum;
static int t_byte_first = 0;
w_char work_wc;
#else /* XJUTIL */
#define t_byte_first    (c_c->two_byte_first)
#define work_wc         (c_c->wc)
#endif /* XJUTIL */

#define W_BUFLEN 32
static w_char w_buffer[W_BUFLEN];
static int w_maxbuf = 0;
static char buf[256];
#ifdef XJUTIL
static int tmp_buf[32];
#endif /* XJUTIL */

int
char_q_len (x)
     w_char x;
{
  return ((*char_q_len_func) (x));
}

void
put_char (x)
     unsigned char x;
{
  if ((x & 0x80) != 0)
    {
      if (t_byte_first == 1)
        {
          work_wc += x;
          JWMflushw_buf (&work_wc, 1);
          t_byte_first = 0;
        }
      else
        {
          t_byte_first = 1;
          work_wc = x * 0x100;
        }
    }
  else
    {
      work_wc = x;
      t_byte_first = 0;
      JWMflushw_buf (&work_wc, 1);
    }
}

void
flushw_buf ()
{
  JWMflushw_buf (w_buffer, w_maxbuf);
  w_maxbuf = 0;
}

int
w_putchar (w)
     w_char w;
{
  w_char wch = w;
  w_char tmp_wch[10];
  int len, i, c_len = 0;
  int ret_col = 0;

  delete_w_ss2 (&wch, 1);
  if (ESCAPE_CHAR (wch))
    {
      ret_col = char_q_len (wch);
      w_buffer[w_maxbuf++] = (w_char) ('^');
      if (wch == 0x7f)
        w_buffer[w_maxbuf++] = (w_char) ('?');
      else
        w_buffer[w_maxbuf++] = (w_char) (wch + 'A' - 1);
    }
  else
    {
      if (print_out_func)
        {
          len = (*print_out_func) (tmp_wch, &wch, 1);
          delete_w_ss2 (tmp_wch, len);
          for (i = 0; i < len; i++)
            {
              w_buffer[w_maxbuf++] = tmp_wch[i];
              c_len = char_q_len (tmp_wch[i]);
              ret_col += c_len;
            }
        }
      else
        {
          ret_col = char_q_len (wch);
          w_buffer[w_maxbuf++] = wch;
        }
    }
  cursor_colum += ret_col;
  if (w_maxbuf >= W_BUFLEN - 2)
    {
      flushw_buf ();
    }
  return (ret_col);
}

void
put_char1 (c)
     int c;
{
  put_char ((unsigned char) c);
  cursor_colum += 1;
}

void
putchar_norm (c)
     char c;
{
  push_hrus ();
  put_char1 (c);
  pop_hrus ();
}

void
print_msg_getc (format, arg1, arg2, arg3)
     register char *format;
     char *arg1, *arg2, *arg3;
{
  int ret;

  sprintf (buf, format, arg1, arg2, arg3);
  init_yes_or_no ((unsigned char *) buf, YesMessage);
  for (;;)
    {
#ifdef  XJUTIL
      ret = xw_read (tmp_buf);
      if (ret != -1 && cur_root->yes_no->map)
        break;
#else /* XJUTIL */
      ret = get_yes_no_event ();
      if (ret && cur_p->yes_no->map)
        break;
      xim->sel_ret = -1;
#endif /* XJUTIL */
    }
  end_yes_or_no ();
  return;
}

#ifdef  XJUTIL
#define CUR_LANG        xjutil->lang
#else
#define CUR_LANG        cur_lang->lang
#endif
extern char *wnn_perror_lang ();
void
errorkeyin ()
{
  print_msg_getc ("%s", wnn_perror_lang (CUR_LANG), NULL, NULL);
}

#undef CUR_LANG

void
print_out7 (format, x1, x2, x3, x4, x5, x6, x7)
     char *format;
     char *x1, *x2, *x3, *x4, *x5, *x6, *x7;
{
  extern char *prgname;
  extern int fflush ();

  fprintf (stderr, "%s : ", prgname);
  fprintf (stderr, format, x1, x2, x3, x4, x5, x6, x7);
  fprintf (stderr, "\r\n");
  fflush (stderr);
}

void
print_out3 (format, x1, x2, x3)
     char *format;
     char *x1, *x2, *x3;
{
  extern char *prgname;
  extern int fflush ();

  fprintf (stderr, "%s : ", prgname);
  fprintf (stderr, format, x1, x2, x3);
  fprintf (stderr, "\r\n");
  fflush (stderr);
}

void
print_out2 (format, x1, x2)
     char *format;
     char *x1, *x2;
{
  extern char *prgname;
  extern int fflush ();

  fprintf (stderr, "%s : ", prgname);
  fprintf (stderr, format, x1, x2);
  fprintf (stderr, "\r\n");
  fflush (stderr);
}

void
print_out1 (format, x1)
     char *format;
     char *x1;
{
  extern char *prgname;
  extern int fflush ();

  fprintf (stderr, "%s : ", prgname);
  fprintf (stderr, format, x1);
  fprintf (stderr, "\r\n");
  fflush (stderr);
}

void
print_out (format)
     char *format;
{
  extern char *prgname;
  extern int fflush ();

  fprintf (stderr, "%s : ", prgname);
  fprintf (stderr, format);
  fprintf (stderr, "\r\n");
  fflush (stderr);
}

void
malloc_error (x)
     char *x;
{
  print_out1 ("Malloc failed in %s.", x);
}

void
print_msg_wait (format)
     register char *format;
{
  init_yes_or_no ((unsigned char *) format, MessageOnly);
  for (;;)
    {
#ifdef  XJUTIL
      xw_read (tmp_buf);
      if (cur_root->yes_no->map && cur_root->yes_no->exp)
        break;
#else /* XJUTIL */
      get_yes_no_event ();
      if (cur_p->yes_no->map && cur_p->yes_no->exp)
        break;
      xim->sel_ret = -1;
#endif /* XJUTIL */
    }
  XFlush (dpy);
  sleep (2);
  end_yes_or_no ();
}
