/*
 * $Id: jis_in.c,v 1.2 2001/06/14 18:16:16 ura Exp $
 */

/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright 1991 by Massachusetts Institute of Technology
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
 */
/*
 *      programs for JIS NYUURYOKU
 */

#include <stdio.h>
#include "commonhd.h"
#include "sdefine.h"
#ifdef XJUTIL
#include "xjutil.h"
#include "sxheader.h"
#include "xext.h"
#else /* XJUTIL */
#include "xim.h"
#include "sheader.h"
#include "ext.h"
#endif /* XJUTIL */


#define FROM_JIS        0x21
#define TO_JIS          0x7E
#define RING(ret)       {ring_bell(); return(ret);}
#ifdef XJUTIL
#define MES1_NUM        113
#define MES2_NUM        114
#define MES3_NUM        8
#define SEL_RET         (xjutil->sel_ret)
#else /* XJUTIL */
#define MES1_NUM        17
#define MES2_NUM        18
#define MES3_NUM        19
#define SEL_RET         (xim->sel_ret)
#endif /* XJUTIL */

static int
hextodec (s)
     char s;
{
  register int c = (int) s;

  if (c >= '0' && c <= '9')
    {
      return (c - '0');
    }
  else if (c >= 'A' && c <= 'F')
    {
      return (c - 'A' + 10);
    }
  else if (c >= 'a' && c <= 'f')
    {
      return (c - 'a' + 10);
    }
  else
    return (-1);
}

static int
get_jis_ichiran (st, num, in, multi, cs, lang)
     char *st;
     int num;
     int in;
     int multi, cs;
     char *lang;
{
  static int from;
  static int i;
  static char *c;
  static int maxh;
  static w_char code[2];
  static char buf[1024];
  static char *buf_ptr[96];
  static char jis_title[32];
  static int jis_ichi_step = 0;

  if (jis_ichi_step == 0)
    {
      if (num == 2)
        {
          from = hextodec (st[0]) << (8 + 4) | hextodec (st[1]) << 8 | 0x21;
          from = (from & 0x7f7f) | get_cs_mask (cs);
          maxh = 94;
        }
      else
        {
          from = hextodec (st[0]) << (8 + 4) | hextodec (st[1]) << 8 | hextodec (st[2]) << 4;
          from = (from & 0x7f7f) | get_cs_mask (cs);
          maxh = 16;
          if ((from & 0xf0) == 0xf0)
            {
              maxh--;
            }
          else if ((from & 0xf0) == 0xa0)
            {
              maxh--;
              from++;
            }
        }

      code[1] = (w_char) 0;
      for (i = 0, c = buf; i < maxh; i++)
        {
          code[0] = (w_char) (from + i);
          buf_ptr[i] = c;
          c += sStrcpy (c, code) + 1;
        }
      if (multi > 1)
        {
          sprintf (jis_title, "%s %d.0x%s", msg_get (cd, MES1_NUM, default_message[MES1_NUM], lang), cs, st);
        }
      else
        {
          sprintf (jis_title, "%s 0x%s", msg_get (cd, MES1_NUM, default_message[MES1_NUM], lang), st);
        }
      jis_ichi_step++;
    }
  if ((i = xw_select_one_element (buf_ptr, maxh, 0, jis_title, JIS_IN, main_table[4], in)) == -1)
    {
      jis_ichi_step = 0;
      return (-1);
    }
  else if (i == BUFFER_IN_CONT)
    {
      return (BUFFER_IN_CONT);
    }
  jis_ichi_step = 0;
  return (from + i);
}

int
in_jis (in)                     /*  returns code for a moji  */
     int in;
{
  static char buffer[7];
  static int c_p = 0;
#ifndef XJUTIL
  static WnnClientRec *c_c_sv = 0;
#endif /* !XJUTIL */
  static int jis_step = 0;
  static char *lang;
  static int cs = 0, valid_cs[3], multi, os = 0, dot = 0;
  int i, k;
  register char t_c, *start_p;
  unsigned char c;
  register int code;

#ifndef XJUTIL
  if (in == -99)
    {
      end_ichiran ();
      code = -1;
      goto LAST;
    }
  if (c_c_sv != 0 && c_c != c_c_sv)
    {
      RING (-1);
    }
#endif /* !XJUTIL */
  if (jis_step == 0)
    {
#ifdef XJUTIL
      set_cswidth (xjutil->cswidth_id);
      lang = xjutil->lang;
#else /* XJUTIL */
      set_cswidth (cur_lang->cswidth_id);
      lang = cur_lang->lang;
      c_c_sv = c_c;
#endif /* !XJUTIL */
      cs = 0;
      for (multi = 0, i = 0; i < 3; i++)
        {
          multi += valid_cs[i] = (get_cswidth (i) == 2) ? 1 : 0;
          if (valid_cs[i])
            cs = i;
        }
      if (multi <= 0)
        {
          RING (-1);
        }
      if (multi > 1)
        {
          os = 2;
        }
      else
        {
          os = 0;
        }
      for (k = 0; k < os + 5; buffer[k++] = ' ');
      buffer[os + 5] = '\0';
      dot = 0;
      c_p = 0;
      jis_step++;
    }
start:
  if (jis_step == 1)
    {
      if (init_ichiran ((unsigned char **) NULL, 0, -1,
                        (unsigned char *) msg_get (cd, MES1_NUM, default_message[MES1_NUM], lang),
                        (unsigned char *) msg_get (cd, MES2_NUM, default_message[MES2_NUM], lang),
                        (unsigned char *) msg_get (cd, MES3_NUM, default_message[MES3_NUM], lang), NULL, os + 5, NYUURYOKU) == -1)
        {
#ifndef XJUTIL
          c_c_sv = 0;
#endif /* !XJUTIL */
          RING (-1);
        }
      draw_nyuu_w ((unsigned char *) buffer, 1);
      jis_step++;
      return (BUFFER_IN_CONT);
    }
  if (jis_step == 2)
    {
      c = (unsigned char) (in & 0xff);
#ifdef XJUTIL
      if (!cur_root->ichi->map)
        return (BUFFER_IN_CONT);
#else /* XJUTIL */
      if (!xim->cur_j_c_root->ichi->map)
        return (BUFFER_IN_CONT);
#endif /* XJUTIL */
      if (SEL_RET == -2)
        {
          end_ichiran ();
          SEL_RET = -1;
          code = -1;
          goto LAST;
        }
      if ((c == ESC) || (t_quit == main_table[5][c]))
        {
          end_ichiran ();
          code = -1;
          goto LAST;
#ifndef XJUTIL
        }
      else if (henkan_off == main_table[5][c])
        {
          RING (BUFFER_IN_CONT);
#endif /* !XJUTIL */
        }
      else if (c_p < os + 4 && ((c <= '9' && c >= '0') || (c <= 'F' && c >= 'A') || (c <= 'f' && c >= 'a') || (os > 0 && c_p == 1 && c == '.')))
        {
          if (c_p > 0)
            t_c = buffer[c_p - 1];
          if (os > 0 && c_p == 0)
            {
              buffer[c_p++] = c;
              goto RET;
            }
          else if (os > 0 && c_p == 1)
            {
              if (c == '.')
                {
                  if (t_c < '0' || t_c > '2' || valid_cs[t_c - '0'] == 0)
                    {
                      RING (BUFFER_IN_CONT);
                    }
                  buffer[c_p++] = c;
                  dot = 1;
                  goto RET;
                }
              else if (t_c == '0' || t_c == '1')
                {
                  RING (BUFFER_IN_CONT);
                }
              else if (!((t_c == '2' || t_c == 'A' || t_c == 'a') && c == '0' || (t_c == '7' || t_c == 'F' || t_c == 'f') && (c == 'F' || c == 'f')))
                {
                  buffer[c_p++] = c;
                  goto RET;
                }
              RING (BUFFER_IN_CONT);
            }
          else if ((c_p == os || c_p == os + 2) ? (c <= 'F' && c >= 'A') ||     /* For EUC */
                   (c <= 'f' && c >= 'a') ||    /* For EUC */
                   (c <= '7' && c >= '2') : !((t_c == '2' || t_c == 'A' || t_c == 'a') && c == '0' || (t_c == '7' || t_c == 'F' || t_c == 'f') && (c == 'F' || c == 'f')))
            {
              buffer[c_p++] = c;
              goto RET;
            }
          else
            {
              RING (BUFFER_IN_CONT);
            }
        }
      else if (c == rubout_code && c_p)
        {
          buffer[--c_p] = ' ';
          if (c_p <= 1)
            dot = 0;
          goto RET;
        }
      else if (c == rubout_code && c_p == 0)
        {
          end_ichiran ();
          code = -1;
          goto LAST;
        }
      else if (c == NEWLINE || c == CR)
        {
          start_p = &buffer[(dot) ? os : 0];
          if ((os > 0) ? (c_p < os || (dot && c_p < os + 2)) : (c_p < os + 2))
            {
              goto RET;
            }
          else if (c_p == os + 4 || (os > 0 && !dot && c_p == 4))
            {
              if (os > 0)
                {
                  if (dot)
                    {
                      cs = hextodec (buffer[0]);
                    }
                  else
                    {
                      cs = 0;
                    }
                }
              code = 0;
              for (k = 0; k < 4; k++)
                {
                  code = code * 16 + hextodec (start_p[k]);
                }
              code &= 0x7f7f;
              code |= get_cs_mask (cs);
              end_ichiran ();
              goto LAST;
            }
          end_ichiran ();
          jis_step++;
          goto NEXT;
        }
      else
        {
          RING (BUFFER_IN_CONT);
        }
    RET:
      draw_nyuu_w ((unsigned char *) buffer, 1);
      return (BUFFER_IN_CONT);
    }
NEXT:
  if (jis_step == 3)
    {
      if (os > 0)
        {
          if (dot)
            {
              cs = hextodec (buffer[0]);
            }
          else
            {
              cs = 0;
            }
        }
      if ((code = get_jis_ichiran (start_p, c_p - (dot ? os : 0), in, multi, cs, lang)) == -1)
        {
          jis_step = 0;
          goto start;
        }
      else if (code == BUFFER_IN_CONT)
        {
          return (BUFFER_IN_CONT);
        }
    }
LAST:
#ifndef XJUTIL
  c_c_sv = 0;
#endif /* !XJUTIL */
  jis_step = 0;
  return (code);
}

#undef RING

/*
  Local Variables:
  eval: (setq kanji-flag t)
  eval: (setq kanji-fileio-code 0)
  eval: (mode-line-kanji-code-update)
  End:
*/
