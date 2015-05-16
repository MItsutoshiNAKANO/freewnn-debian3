/*
 *  $Id: basic_op.c,v 1.5 2006/02/10 18:29:38 aonoto Exp $
 */

/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright FreeWnn Project 1999, 2000, 2002, 2006
 *
 * Maintainer:  FreeWnn Project
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
bufferの使い方とかbufferの内容がどのように画面に表示されるかとか
の詳しい説明は、buffer.hか又は、screen.cを参照して下さい。
このファイルは、baffer を制御するための基本関数で、
ユーザーインターフェース、登録時、その他の入力に共通するような関数を
定義しています。
*/
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
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
#include "jllib.h"
#include "commonhd.h"
#include "sdefine.h"
#include "sheader.h"
#include "rk_spclval.h"
#include "buffer.h"

/** bufferを使って、仮名文字列を入力します。
buflenはバッファーの長さ。modeは反転、下線の状態
bufferの中に初期文字列を入れて置いて下さい。
msgは、仮名入力行の先頭に表すメッセージ文(w_charであることに注意)
横にスクロールする時には、このメッセージも一緒にスクロールします
これは、bufferを使う高レベルのかな入力ルーチンです。
更に高度な指定をしたい人は、buffer_inを用いて下さい。
*/

extern int uum_cursor_invisible;

int
redraw_nisemono ()
{
  call_t_redraw_move (c_b->t_c_p, c_b->t_c_p, c_b->maxlen, 2, 1);
  return (0);
}


int
kana_in_w_char_msg (msg, mode, buffer, buflen)
     w_char *msg;
     int mode;
     w_char *buffer;
     int buflen;
{
  struct buf c_b1, *c_btmp;
  int msg_len = Strlen (msg);
  c_btmp = c_b;
  c_b = &c_b1;
  if ((c_b->buffer = (w_char *) malloc ((buflen + msg_len) * 2)) == NULL)
    {
      print_msg_getc (MSG_GET (2));
      return (-1);
    }
  Strcpy (c_b->buffer, msg);
  c_b->t_m_start = Strlen (c_b->buffer);
  Strcpy (c_b->buffer + msg_len, buffer);
  c_b->buf_msg = "";

  c_b->hanten = mode;
  c_b->start_col = 0;
  c_b->maxlen = Strlen (c_b->buffer);
  c_b->t_c_p = c_b->maxlen;
  c_b->buflen = buflen + msg_len;
  set_screen_vars_default ();
  c_b->redraw_fun = redraw_nisemono;
  c_b->key_table = main_table[6];
  c_b->rk_clear_tbl = romkan_clear_tbl[6];
  c_b->key_in_fun = NULL;
  c_b->ctrl_code_fun = (int (*)()) NULL;
  kill_buffer_offset = 0;

  init_screen ();
  if (buffer_in () == -1)
    {
      free (c_b->buffer);
      c_b = c_btmp;
      return (-1);
    }
  free (c_b->buffer);
  Strncpy (buffer, c_b->buffer + msg_len, c_b->maxlen - msg_len);
  buffer[c_b->maxlen - msg_len] = 0;
  c_b = c_btmp;
  return (0);
}

/** bufferを使って、仮名文字列を入力します。
kana_in_w_char_msgとの違いは、
msgが、charであることと、横にスクロールする時には、このメッセージを除いて
スクロールすること。
*/
int
kana_in (msg, mode, buffer, buflen)
     char *msg;
     int mode;
     w_char *buffer;
     int buflen;
{
  struct buf c_b1, *c_btmp;

  c_btmp = c_b;
  c_b = &c_b1;

  c_b->buffer = buffer;
  c_b->buf_msg = msg;

  c_b->hanten = mode;
  c_b->start_col = strlen (msg);
  c_b->maxlen = Strlen (buffer);
  c_b->t_c_p = c_b->maxlen;
  c_b->buflen = buflen;
  c_b->t_m_start = -2;
  set_screen_vars_default ();
  c_b->key_table = main_table[6];
  c_b->rk_clear_tbl = romkan_clear_tbl[6];
  c_b->key_in_fun = NULL;
  c_b->redraw_fun = redraw_nisemono;
  c_b->ctrl_code_fun = (int (*)()) NULL;
  kill_buffer_offset = 0;

  init_screen ();
  if (buffer_in () == -1)
    {
      c_b = c_btmp;
      return (-1);
    }
  c_b->buffer[c_b->maxlen] = 0;
  c_b = c_btmp;
  return (0);
}

/** かな入力を行い、画面制御するための基本ルーチン
グローバル変数c_bの指す所(struct buf)に、種種な初期設定をしてから
これを呼んで下さい。
文字列は、c_b->bufferに入って終了します。
c_b->maxlenに文字列の長さが入ります。
*/
int
buffer_in ()
{
  int c;
  int ret;
  int romkan;
  int ignored = 0;
  int not_redrawtmp = not_redraw;
  int tmp_send = 0;
  int in;
  unsigned int *output;

  extern int kakutei ();
  extern int send_string ();
  extern int return_it ();

  not_redraw = 0;
  while (1)
    {

      if (uum_cursor_invisible && !cursor_invisible_fun)
        {
          throw_col (maxlength - 1);
          flush ();
        }
      if (if_unget_buf ())
        {
          output = get_unget_buf ();
        }
      else
        {
          in = keyin ();
          output = romkan_henkan (in);
        }

      conv_ltr_to_ieuc (output);
      if (input_func)
        (*input_func) (output, output);

      for (; *output != EOLTTR; output++)
        {
          c = ((romkan = *output) & 0x0000ffff);
          if (uum_cursor_invisible && !cursor_invisible_fun)
            {
              t_throw ();
            }

          if (isSPCL (romkan))
            {
              if (romkan == REDRAW)
                {
                  (*c_b->redraw_fun) ();
                  continue;
                }
              else if (romkan == CHMSIG)
                {               /* mode changed */
                  if (redraw_when_chmsig_func)
                    (*redraw_when_chmsig_func) ();
                  disp_mode ();
                  continue;
                }
              else if (romkan == NISEBP)
                {               /* error */
                  ring_bell ();
                  continue;
                }
              else
                continue;       /* if (romkan == EOLTTR) */
            }
          if (ignored)
            {
              if (isNISE (romkan))
                {
                  if (c == rubout_code)
                    {
                      --ignored;
                    }
                  else
                    {
                      ++ignored;
                    }
                }
              else
                {
                  ignored = 0;
                }
              continue;
            }
          /* 偽物のコントロールコードは画面表示しないので後で来るrubout
             のコードも無視する必要がある。ignoredは無視すべき
             ruboutの個数を保持する。 */
          if (isNISE (romkan) && (ESCAPE_CHAR (c) || NOT_NORMAL_CHAR (c)) && c != rubout_code)
            {
              ++ignored;
              continue;
            }

          if (c < TBL_SIZE && c_b->key_table[c])
            {
              if (c_b->rk_clear_tbl[c])
                romkan_clear ();
              ret = (*c_b->key_table[c]) (c, romkan);
/* Because 1byte-kana is 0xa0-0xff
    }else if(c >= 0x80 && c < 0xff){
*/
            }
          else if (c >= 0x80 && c <= 0x9f)
            {
              if (tmp_send)
                {
                  ret = return_it (c, romkan);
                  tmp_send = 0;
                }
              else
                {
                  ret = 0;
                  ring_bell ();
                }
            }
          else if (!(ESCAPE_CHAR (c)) || c == 0x09)
            {                   /* TAB is not ESCAPE char */
              if (c_b->maxlen < c_b->buflen)
                {
                  if (tmp_send)
                    {
                      ret = return_it (c);
                      tmp_send = 0;
                    }
                  else if (c_b->key_in_fun)
                    {
                      ret = (*c_b->key_in_fun) (c, romkan);
                    }
                  else
                    {
                      ret = insert_char (c);
                      call_t_redraw_move (c_b->t_c_p + 1, c_b->t_c_p, c_b->maxlen, 0, 1);
                      if (henkan_on_kuten && c_b->t_c_p == c_b->maxlen)
                        henkan_if_maru (c);
                    }
                }
              else
                {
                  romkan_clear ();
                  ret = 0;
                }

            }
          else
            {
              if (c_b->ctrl_code_fun)
                {
                  ret = (*c_b->ctrl_code_fun) (c, romkan);
                }
              else
                {
                  ret = 0;
                  ring_bell ();
                }
            }
          if (ret == 1)
            {
              not_redraw = not_redrawtmp;
              return (0);
            }
          else if (ret == -1)
            {
              not_redraw = not_redrawtmp;
              return (-1);
            }
        }
    }
}

int
t_rubout (c, romkan)
     int c, romkan;
{

  if (c_b->t_c_p != 0)
    {
      backward ();
      t_delete_char ();
    }
  return (0);
}


void
delete_char1 ()
{
  if (c_b->t_c_p < c_b->maxlen)
    {
      Strncpy (c_b->buffer + c_b->t_c_p, c_b->buffer + c_b->t_c_p + 1, c_b->maxlen - c_b->t_c_p - 1);
      c_b->maxlen -= 1;
    }
}

int
t_delete_char ()
{
  delete_char1 ();
  call_t_redraw_move (c_b->t_c_p, c_b->t_c_p, c_b->maxlen, 2, 1);
  return (0);
}

extern int in_kuten ();
extern int in_jis ();

int
kuten ()
{
  input_a_char_from_function (in_kuten);
  return (0);
}

int
jis ()
{
  input_a_char_from_function (in_jis);
  return (0);
}

int
input_a_char_from_function (fun)
     int (*fun) ();
{
  int c;
  int ret = 0;

  if ((c = (*fun) ()) != -1)
    {
      if (c_b->key_in_fun)
        {
          ret = (*c_b->key_in_fun) (c, c);
        }
      else
        {
          ret = insert_char (c);
          t_redraw_move (c_b->t_c_p + 1, c_b->t_c_p, c_b->maxlen, 0);
        }
    }
  redraw_line ();
  return (ret);
}

int
t_kill ()
{
  kill_buffer_offset = min (c_b->maxlen - c_b->t_c_p, maxchg);
  Strncpy (kill_buffer, c_b->buffer + c_b->t_c_p, kill_buffer_offset);
  c_b->maxlen = c_b->t_c_p;
  t_redraw_move (c_b->t_c_p, c_b->t_c_p, c_b->maxlen, 1);
  return (0);
}


int
t_yank ()
{
  if (kill_buffer_offset <= c_b->buflen - c_b->t_c_p)
    {
      Strncpy (c_b->buffer + c_b->t_c_p + kill_buffer_offset, c_b->buffer + c_b->t_c_p, c_b->maxlen - c_b->t_c_p);
      Strncpy (c_b->buffer + c_b->t_c_p, kill_buffer, kill_buffer_offset);
      c_b->maxlen += kill_buffer_offset;
      call_t_redraw_move (c_b->t_c_p + kill_buffer_offset, c_b->t_c_p, c_b->maxlen, 0, 1);
    }
  return (0);
}

int
t_ret ()
{
  return (1);
}

int
t_quit ()
{
  return (-1);
}


int
c_top ()
{
  (*c_top_func) ();
  return (0);
}

int
c_end ()
{
  (*c_end_func) ();
  return (0);
}

int
c_end_nobi ()
{
  (*c_end_nobi_func) ();
  return (0);
}

int
t_jmp_backward ()
{
  int k;

  for (k = 1; k < touroku_bnst_cnt; k++)
    {
      if (touroku_bnst[k] >= c_b->t_c_p)
        {
          if (touroku_bnst[k - 1] >= c_b->t_m_start)
            {
              call_t_redraw_move_1 (touroku_bnst[k - 1], touroku_bnst[k - 1], c_b->maxlen, 0, 1, 0, 3);
            }
          else
            {
              c_top ();
            }
          return (0);
        }
    }
  backward ();
  return (0);
}

int
t_jmp_forward ()
{
  int k;

  for (k = 1; k < touroku_bnst_cnt; k++)
    {
      if (touroku_bnst[k] > c_b->t_c_p)
        {
          call_t_redraw_move_1 (touroku_bnst[k], c_b->t_c_p, c_b->maxlen, 0, 1, 0, 3);
          return (0);
        }
    }
  forward_char ();
  return (0);
}


int
forward_char ()
{
  if (c_b->t_c_p < c_b->maxlen)
    {
      call_t_redraw_move_2 (c_b->t_c_p + 1, c_b->t_c_p, c_b->t_m_start, c_b->t_c_p + 2, c_b->t_c_p + 1, 0, 1);
    }
  return (0);
}

int
forward ()
{
  if (c_b->t_c_p < c_b->maxlen)
    {
      t_move (c_b->t_c_p + 1);
    }
  return (0);
}

int
backward_char ()
{
  if (c_b->t_c_p >= 1 && c_b->t_c_p > c_b->t_m_start)
    {
      call_t_redraw_move_2 (c_b->t_c_p - 1, c_b->t_c_p - 1, c_b->t_m_start, c_b->t_c_p + 1, c_b->t_c_p, 0, 1);
    }
  return (0);
}

int
backward ()
{
  if (c_b->t_c_p >= 1 && c_b->t_c_p > c_b->t_m_start)
    {
      t_move (c_b->t_c_p - 1);
    }
  return (0);
}

int
insert_char (c)
     w_char c;
{
  if (c_b->maxlen < c_b->buflen)
    {
      Strncpy (c_b->buffer + c_b->t_c_p + 1, c_b->buffer + c_b->t_c_p, c_b->maxlen - c_b->t_c_p);
      c_b->maxlen += 1;
      c_b->buffer[c_b->t_c_p] = c;
    }
  return (0);
}

#ifdef  nodef
int
insert_string (string, len)
     w_char *string;
     int len;
{
  if (c_b->maxlen + len <= c_b->buflen)
    {
      Strncpy (c_b->buffer + c_b->t_c_p + len, c_b->buffer + c_b->t_c_p, c_b->maxlen - c_b->t_c_p);
      Strncpy (c_b->buffer + c_b->t_c_p, string, len);
      c_b->maxlen += len;
    }
}
#endif

/** vlenとduplicateを適当と思われる状態に設定します。*/
void
set_screen_vars_default ()
{
  c_b->vlen = maxlength * conv_lines - c_b->start_col - 3 - disp_mode_length;
  c_b->vst = 0;
  c_b->duplicate = max ((c_b->vlen + 3) >> 2, 2);
}

int
quote ()
{
  int c;
  c = keyin ();
  if (c_b->key_table[c] == henkan_off)
    {
      (*c_b->key_table[c]) ();
      return (0);
    }
  if (empty_modep () && c_b->key_in_fun)
    return ((*c_b->key_in_fun) (c, c));
  insert_char (c);
  t_redraw_move (c_b->t_c_p + 1, c_b->t_c_p, c_b->maxlen, 0);
  return (0);
}
