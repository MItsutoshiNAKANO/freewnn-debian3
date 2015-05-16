/*
 * $Id: kuten.c,v 1.7.2.1 1999/02/08 08:08:51 yamasita Exp $
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
 */
/*
**   programs for KUTEN NYUURYOKU
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

#define MAX_TEN_LENGTH  94

#define RING(ret)       {ring_bell(); return(ret);}
#ifdef XJUTIL
#define MES0_NUM        117
#define MES1_NUM        115
#define MES2_NUM        116
#define MES3_NUM        8
#define SEL_RET         (xjutil->sel_ret)
#else /* XJUTIL */
#define MES0_NUM        26
#define MES1_NUM        24
#define MES2_NUM        25
#define MES3_NUM        19
#define SEL_RET         (xim->sel_ret)
#endif /* XJUTIL */

static int
get_ichiran (ku, in, multi, cs, lang)
     int ku;
     int in;
     int multi, cs;
     char *lang;
{
  static char buf[1024];
  static char *buf_ptr[MAX_TEN_LENGTH];
  static int i;
  static char *c;
  static char kuten_title[32];
  static int kuten_ichi_step = 0;
  static w_char code[2];

  if (kuten_ichi_step == 0)
    {
      code[1] = (w_char) 0;
      for (i = 0, c = buf; i < MAX_TEN_LENGTH; i++)
        {
          code[0] = (((ku + 32) << 8) | ((i + 1) + 32)) & 0x7f7f;
          code[0] |= get_cs_mask (cs);
          buf_ptr[i] = c;
          c += sStrcpy (c, code) + 1;
        }
      if (multi > 1)
        {
          sprintf (kuten_title, "%s %d.%2d %s", msg_get (cd, MES1_NUM, default_message[MES1_NUM], lang), cs, ku, msg_get (cd, MES0_NUM, default_message[MES0_NUM], lang));
        }
      else
        {
          sprintf (kuten_title, "%s %2d %s", msg_get (cd, MES1_NUM, default_message[MES1_NUM], lang), ku, msg_get (cd, MES0_NUM, default_message[MES0_NUM], lang));
        }
      kuten_ichi_step++;
    }
  if ((i = xw_select_one_element (buf_ptr, MAX_TEN_LENGTH, 0, kuten_title, KUTEN_IN, main_table[4], in)) == -1)
    {
      kuten_ichi_step = 0;
      return (-1);
    }
  else if (i == BUFFER_IN_CONT)
    {
      return (BUFFER_IN_CONT);
    }
  kuten_ichi_step = 0;
  code[0] = ((((ku + 32) << 8) | ((i + 1) + 32)) & 0x7f7f) | get_cs_mask (cs);
  return ((int) code[0]);
}

int
in_kuten (in)                   /*  returns code for a moji  */
     int in;
{
  static char buffer[7];
  static int code;
  static int c_p = 0;
  static int ret1, ret2;
  static int mode = -1, mode1 = -1;
#ifndef XJUTIL
  static WnnClientRec *c_c_sv = 0;
#endif /* !XJUTIL */
  static int kuten_step = 0;
  static char *lang;
  static int cs = 0, valid_cs[3], multi, os = 0;
  register int i, k;
  register char t_c, *start_p;
  unsigned char c;

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
  if (kuten_step == 0)
    {
#ifdef XJUTIL
      set_cswidth (xjutil->cswidth_id);
      lang = xjutil->lang;
#else /* XJUTIL */
      set_cswidth (cur_lang->cswidth_id);
      lang = cur_lang->lang;
      c_c_sv = c_c;
#endif /* !XJUTIL */
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
      c_p = 0;
      mode = mode1 = -1;
      kuten_step++;
    }

start:
  if (kuten_step == 1)
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
      kuten_step++;
      return (BUFFER_IN_CONT);
    }
  if (kuten_step == 2)
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
      if ((c <= '9' && c >= '0') || c == '.')
        {
          if (os > 0)
            {
              if (mode >= 0)
                {
                  if (mode1 >= 0)
                    {
                      if ((c_p - mode1) <= 2 && c != '.')
                        {
                          buffer[c_p++] = (char) c;
                          goto RET;
                        }
                    }
                  else
                    {
                      t_c = buffer[0];
                      if ((c_p - mode) <= 4 && c != '.')
                        {
                          buffer[c_p++] = c;
                          goto RET;
                        }
                      else if (mode == 1 && t_c >= '0' && t_c <= '2' && valid_cs[t_c - '0'] && (c_p - mode) <= 3 && (c_p - mode) >= 2 && c == '.')
                        {
                          mode1 = c_p;
                          buffer[c_p++] = c;
                          goto RET;
                        }
                    }
                  RING (BUFFER_IN_CONT);
                }
              else
                {
                  if (c == '.')
                    {
                      if (c_p > 0 && c_p <= 2)
                        {
                          mode = c_p;
                          buffer[c_p++] = c;
                          goto RET;
                        }
                    }
                  else
                    {
                      if (c_p < 4)
                        {
                          buffer[c_p++] = c;
                          goto RET;
                        }
                    }
                  RING (BUFFER_IN_CONT);
                }
            }
          else
            {
              if (c == '.')
                {
                  if ((mode == -1) && 0 < c_p && c_p <= 2)
                    {
                      mode = c_p;
                      buffer[c_p++] = c;
                      goto RET;
                    }
                }
              else
                {
                  if ((mode >= 0 ? (c_p - mode) <= 2 : c_p < 4))
                    {
                      buffer[c_p++] = c;
                      goto RET;
                    }
                }
              RING (BUFFER_IN_CONT);
            }
        }
      else if (c == rubout_code && c_p)
        {
          if (os > 0)
            {
              if (mode1 >= 0 && buffer[c_p - 1] == '.')
                {
                  mode1 = -1;
                }
              else if (mode >= 0 && buffer[c_p - 1] == '.')
                {
                  mode = -1;
                }
            }
          else
            {
              if (buffer[c_p - 1] == '.')
                mode = -1;
            }
          buffer[--c_p] = ' ';
        }
      else if (c == rubout_code && c_p == 0)
        {
          end_ichiran ();
          code = -1;
          goto LAST;
        }
      else if ((c == NEWLINE) || (c == CR))
        {
          if (os > 0)
            {
              if ((mode1 >= 0 && (buffer[c_p - 1] != '.')) || (mode1 == -1 && mode >= 0 && (c_p - mode) >= 2) || (mode == -1 && c_p >= 3))
                {
                  if (mode1 >= 0 || (mode == 1 && (c_p - mode) >= 4))
                    {
                      cs = buffer[0] - '0';
                      if (valid_cs[cs] == 0)
                        RING (BUFFER_IN_CONT);
                      start_p = &buffer[os];
                    }
                  else
                    {
                      cs = 0;
                      start_p = buffer;
                    }
                  if ((!(sscanf (start_p, "%d.%d", &ret1, &ret2) == 2) &&
                       !(((mode == -1) ? (c_p == 4) : (c_p == os + 4)) &&
                         sscanf (start_p, "%2d%2d", &ret1, &ret2) == 2) &&
                       !(((mode == -1) ? (c_p == 3) : (c_p == os + 3)) && sscanf (start_p, "%1d%2d", &ret1, &ret2) == 2)) || ret1 <= 0 || ret1 > 94 || ret2 <= 0 || ret2 > 94)
                    {
                      RING (BUFFER_IN_CONT);
                    }
                  code = (((ret1 + 32) << 8) | (ret2 + 32)) & 0x7f7f;
                  code |= get_cs_mask (cs);
                  end_ichiran ();
                  goto LAST;
                }
              else
                {
                  if (mode1 >= 0 && (buffer[c_p - 1] == '.'))
                    {
                      if (sscanf (&buffer[os], "%d.", &ret1) != 1 || (ret1 <= 0 || ret1 > 94))
                        {
                          RING (BUFFER_IN_CONT);
                        }
                    }
                  else if (mode == -1 && (c_p == 1 || c_p == 2))
                    {
                      if (sscanf (&buffer[0], "%d", &ret1) != 1 || (ret1 <= 0 || ret1 > 94))
                        {
                          RING (BUFFER_IN_CONT);
                        }
                    }
                  else
                    {
                      RING (BUFFER_IN_CONT);
                    }
                  end_ichiran ();
                  kuten_step++;
                  goto NEXT;
                }
              RING (BUFFER_IN_CONT);
            }
          else
            {
              if (c_p >= 3 && (buffer[c_p - 1] != '.'))
                {
                  start_p = buffer;
                  if ((!(sscanf (start_p, "%d.%d", &ret1, &ret2) == 2) &&
                       !((c_p == 4) &&
                         sscanf (start_p, "%2d%2d", &ret1, &ret2) == 2) && !((c_p == 3) && sscanf (start_p, "%1d%2d", &ret1, &ret2) == 2)) || ret1 <= 0 || ret1 > 94 || ret2 <= 0 || ret2 > 94)
                    {
                      RING (BUFFER_IN_CONT);
                    }
                  code = (((ret1 + 32) << 8) | (ret2 + 32)) & 0x7f7f;
                  code |= get_cs_mask (cs);
                  end_ichiran ();
                  goto LAST;
                }
              else if (c_p <= 2 && mode == -1)
                {
                  if (sscanf (buffer, "%d", &ret1) != 1 || (ret1 <= 0 || ret1 > 94))
                    {
                      RING (BUFFER_IN_CONT);
                    }
                  end_ichiran ();
                  kuten_step++;
                  goto NEXT;
                }
              RING (BUFFER_IN_CONT);
            }
        }
    RET:
      draw_nyuu_w ((unsigned char *) buffer, 1);
      return (BUFFER_IN_CONT);
    }
NEXT:
  if (kuten_step == 3)
    {
      if (os > 0)
        {
          if (mode1 >= 0)
            {
              cs = buffer[0] - '0';
            }
          else
            {
              cs = 0;
            }
        }
      if ((code = get_ichiran (ret1, in, multi, cs, lang)) == -1)
        {
          kuten_step = 0;
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
  kuten_step = 0;
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
