/*
 * $Id: key_bind.c,v 1.2 2001/06/14 18:16:16 ura Exp $
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
#include <stdio.h>
#include <pwd.h>
#include "commonhd.h"
#include "config.h"
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

extern char *getenv ();
#ifdef  XJUTIL
static struct kansuu kansuu_hyo[] = {
  {"send_string", "send what is in the buffer to pty.", 0,
   NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"forward_char", "cursor wo hitotu maeni susumeru.", 1,
   NULL, forward_char, NULL, NULL, NULL,
   forward_char, forward_char, NULL, NULL, NULL,
   '\0'},
  {"backward_char", "cursor wo hitotu usironi susumeru.", 1,
   NULL, backward_c, NULL, NULL, NULL,
   backward_char, backward_char, NULL, NULL, NULL,
   '\0'},
  {"goto_top_of_line", "", 1,
   NULL, c_top, NULL, NULL, NULL,
   c_top, c_top, NULL, NULL, NULL,
   '\0'},
  {"goto_end_of_line", "", 1,
   NULL, c_end, c_end_nobi, NULL, NULL,
   c_end, c_end, NULL, NULL, NULL,
   '\0'},
  {"delete_char_at_cursor", "", 1,
   NULL, delete_c, NULL, NULL, NULL,
   t_delete_char, t_delete_char, NULL, NULL, NULL,
   '\0'},
  {"kaijo", "", 0,
   NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"henkan", "", 1,
   NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"tan_henkan", "", 1,
   NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"tan_henkan_dai", "", 1,
   NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"jikouho", "", 0,
   NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"zenkouho", "", 0,
   NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"jikouho_dai", "", 0,
   NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"zenkouho_dai", "", 0,
   NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"select_jikouho", "", 0,
   NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"select_jikouho_dai", "", 0,
   NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"kill", "", 1,
   NULL, kill_c, NULL, NULL, NULL,
   t_kill, t_kill, NULL, NULL, NULL,
   '\0'},
  {"yank", "", 1,
   NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"yank_e", "", 1,
   NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"bunsetu_nobasi", "", 0,
   NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"bunsetu_chijime", "", 0,
   NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"jisho_utility", "", 0,
   NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"jisho_utility_e", "", 0,
   NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"touroku", "", 1,
   NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"touroku_e", "", 1,
   NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"kakutei", "", 1,
   NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"sainyuuryoku", "", 1,
   NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"sainyuuryoku_e", "", 1,
   NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"kuten", "", 0,
   NULL, kuten, NULL, NULL, NULL,
   kuten, kuten, NULL, NULL, NULL,
   '\0'},
  {"kuten_e", "", 0,
   NULL, kuten, NULL, kuten, NULL,
   kuten, kuten, NULL, NULL, NULL,
   '\0'},
  {"jis", "", 0,
   NULL, jis, NULL, NULL, NULL,
   jis, jis, NULL, NULL, NULL,
   '\0'},
  {"jis_e", "", 0,
   NULL, jis, NULL, jis, NULL,
   jis, jis, NULL, NULL, NULL,
   '\0'},
  {"redraw_line", "", 1,
   NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"redraw_line_e", "", 1,
   NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"previous_history_e", "", 1,
   NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"previous_history", "", 1,
   NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"next_history_e", "", 1,
   NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"next_history", "", 1,
   NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"touroku_mark_set", "", 0,
   NULL, NULL, NULL, NULL, NULL,
   t_markset, NULL, NULL, NULL, NULL,
   '\0'},
  {"touroku_return", "", 0,
   NULL, NULL, NULL, NULL, NULL,
   NULL, t_ret, NULL, NULL, NULL,
   '\0'},
  {"quit", "", 0,
   NULL, NULL, NULL, NULL, quit_select,
   t_quit, t_quit, quit_select, quit_select, quit_select,
   '\0'},
  {"touroku_jump_forward", "", 0,
   NULL, NULL, NULL, NULL, NULL,
   NULL, t_jmp_forward, NULL, NULL, NULL,
   '\0'},
  {"touroku_jump_backward", "", 0,
   NULL, NULL, NULL, NULL, NULL,
   NULL, t_jmp_backward, NULL, NULL, NULL,
   '\0'},
  {"change_to_insert_mode", "", 0,
   NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"quote_e", "", 0,
   NULL, quote, NULL, quote, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"quote", "", 0,
   NULL, quote, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"send_ascii_char", "", 0,
   NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"toggle_send_ascii_char", "", 0,
   NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"not_send_ascii_char", "", 0,
   NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"pop_send_ascii_char", "", 0,
   NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"quote_send_ascii_char", "", 0,
   NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"select_select", "", 0,
   NULL, NULL, NULL, NULL, select_select,
   NULL, NULL, kworddel_op, select_select, select_select,
   '\0'},
  {"lineend_select", "", 0,
   NULL, NULL, NULL, NULL, lineend_select,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"linestart_select", "", 0,
   NULL, NULL, NULL, NULL, linestart_select,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"previous_select", "", 0,
   NULL, NULL, NULL, NULL, xw_previous_select,
   NULL, NULL, xw_previous_select, xw_previous_select,
   xw_previous_message,
   '\0'},
  {"next_select", "", 0,
   NULL, NULL, NULL, NULL, xw_next_select,
   NULL, NULL, xw_next_select, xw_next_select, xw_next_message,
   '\0'},
  {"backward_select", "", 0,
   NULL, NULL, NULL, NULL, backward_select,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"forward_select", "", 0,
   NULL, NULL, NULL, NULL, forward_select,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"reconnect_jserver", "", 0,
   reconnect_server, reconnect_server, reconnect_server, reconnect_server,
   reconnect_server,
   reconnect_server, reconnect_server, reconnect_server, reconnect_server,
   reconnect_server,
   '\0'},
  {"henkan_on", "", 0,
   NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"inspect", "", 0,
   NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"sakujo_kouho", "", 0,
   NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"nobi_henkan_dai", "", 0,
   NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"nobi_henkan", "", 0,
   NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"del_entry", "", 0,
   NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, kworddel, kdicdel, NULL,
   '\0'},
  {"use_entry", "", 0,
   NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, kworduse, kdicuse, NULL,
   '\0'},
  {"com_entry", "", 0,
   NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, kwordcom, kdiccom, NULL,
   '\0'},
  {"kana_henkan", "", 1,
   NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"hindo_set", "", 0,
   NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, kwordhindo, NULL, NULL,
   '\0'},
  {"ima_bit", "", 0,
   NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, kwordima, NULL, NULL,
   '\0'},
  {"change_locale", "", 0,
   NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"change_locale_ct", "", 0,
   NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'}
};
#else /* XJUTIL */
extern int lang_c ();
extern int lang_ct ();
static struct kansuu kansuu_hyo[] = {
  {"send_string", "send what is in the buffer to pty.", 0,
   send_string, send_string, send_string, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"forward_char", "cursor wo hitotu maeni susumeru.", 1,
   forward_bunsetsu, forward_char, henkan_forward, NULL, NULL,
   forward_char, forward_char, NULL, NULL, NULL,
   '\0'},
  {"backward_char", "cursor wo hitotu usironi susumeru.", 1,
   backward_bunsetsu, backward_c, henkan_backward, NULL, NULL,
   backward_char, backward_char, NULL, NULL, NULL,
   '\0'},
  {"goto_top_of_line", "", 1,
   top_bunsetsu, c_top, jmptijime, NULL, NULL,
   c_top, c_top, NULL, NULL, NULL,
   '\0'},
  {"goto_end_of_line", "", 1,
   end_bunsetsu, c_end, c_end_nobi, NULL, NULL,
   c_end, c_end, NULL, NULL, NULL,
   '\0'},
  {"delete_char_at_cursor", "", 1,
   NULL, delete_c, NULL, NULL, NULL,
   t_delete_char, t_delete_char, NULL, NULL, NULL,
   '\0'},
  {"kaijo", "", 0,
   kaijo, NULL, kaijo, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"henkan", "", 1,
   NULL, ren_henkan, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"tan_henkan", "", 1,
   NULL, tan_henkan, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"tan_henkan_dai", "", 1,
   NULL, tan_henkan_dai, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"jikouho", "", 0,
   jikouho_c, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"zenkouho", "", 0,
   zenkouho_c, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"jikouho_dai", "", 0,
   jikouho_dai_c, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"zenkouho_dai", "", 0,
   zenkouho_dai_c, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"select_jikouho", "", 0,
   select_jikouho, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"select_jikouho_dai", "", 0,
   select_jikouho_dai, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"kill", "", 1,
   NULL, kill_c, NULL, NULL, NULL,
   t_kill, t_kill, NULL, NULL, NULL,
   '\0'},
  {"yank", "", 1,
   NULL, yank_c, NULL, NULL, NULL,
   t_yank, t_yank, NULL, NULL, NULL,
   '\0'},
  {"yank_e", "", 1,
   NULL, yank_c, NULL, yank_c, NULL,
   t_yank, t_yank, NULL, NULL, NULL,
   '\0'},
  {"bunsetu_nobasi", "", 0,
   enlarge_bunsetsu, NULL, forward, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"bunsetu_chijime", "", 0,
   smallen_bunsetsu, NULL, tijime, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"jisho_utility", "", 0,
   jutil_c, jutil_c, jutil_c, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"jisho_utility_e", "", 0,
   jutil_c, jutil_c, jutil_c, jutil_c, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"touroku", "", 1,
   touroku_c, touroku_c, touroku_c, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"touroku_e", "", 1,
   touroku_c, touroku_c, touroku_c, touroku_c, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"kakutei", "", 1,
   kakutei, kakutei, kakutei, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"sainyuuryoku", "", 1,
   NULL, remember_me, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"sainyuuryoku_e", "", 1,
   NULL, remember_me, NULL, remember_me, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"kuten", "", 0,
   NULL, kuten, NULL, NULL, NULL,
   kuten, kuten, NULL, NULL, NULL,
   '\0'},
  {"kuten_e", "", 0,
   NULL, kuten, NULL, kuten, NULL,
   kuten, kuten, NULL, NULL, NULL,
   '\0'},
  {"jis", "", 0,
   NULL, jis, NULL, NULL, NULL,
   jis, jis, NULL, NULL, NULL,
   '\0'},
  {"jis_e", "", 0,
   NULL, jis, NULL, jis, NULL,
   jis, jis, NULL, NULL, NULL,
   '\0'},
  {"redraw_line", "", 1,
   reset_line, reset_line, reset_line, NULL, reset_line,
   reset_line, reset_line, NULL, NULL, NULL,
   '\0'},
  {"redraw_line_e", "", 1,
   reset_line, reset_line, reset_line, reset_line, reset_line,
   reset_line, reset_line, NULL, NULL, NULL,
   '\0'},
  {"previous_history_e", "", 1,
   NULL, previous_history, NULL, previous_history, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"previous_history", "", 1,
   NULL, previous_history, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"next_history_e", "", 1,
   NULL, next_history, NULL, next_history, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"next_history", "", 1,
   NULL, next_history, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
/*{ "touroku_mark_set",         "",                                     0,
  NULL,         NULL,           NULL,           NULL,           NULL,
  t_markset,    NULL,           NULL,           NULL,           NULL,
  '\0'}, */
  {"touroku_mark_set", "", 0,
   NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"touroku_return", "", 0,
   NULL, NULL, NULL, NULL, NULL,
   NULL, t_ret, NULL, NULL, NULL,
   '\0'},
  {"quit", "", 0,
   NULL, NULL, NULL, NULL, quit_select,
   t_quit, t_quit, NULL, NULL, quit_select,
   '\0'},
  {"touroku_jump_forward", "", 0,
   NULL, NULL, NULL, NULL, NULL,
   NULL, t_jmp_forward, NULL, NULL, NULL,
   '\0'},
  {"touroku_jump_backward", "", 0,
   NULL, NULL, NULL, NULL, NULL,
   NULL, t_jmp_backward, NULL, NULL, NULL,
   '\0'},
  {"change_to_insert_mode", "", 0,
   insert_it_as_yomi, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"quote_e", "", 0,
   NULL, quote, NULL, quote, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"quote", "", 0,
   NULL, quote, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"send_ascii_char", "", 0,
   send_ascii, send_ascii, send_ascii, send_ascii, send_ascii,
   send_ascii, send_ascii, send_ascii, send_ascii, send_ascii,
   '\0'},
  {"toggle_send_ascii_char", "", 0,
   toggle_send_ascii, toggle_send_ascii, toggle_send_ascii, toggle_send_ascii,
   toggle_send_ascii,
   toggle_send_ascii, toggle_send_ascii, toggle_send_ascii, toggle_send_ascii,
   toggle_send_ascii,
   '\0'},
  {"not_send_ascii_char", "", 0,
   not_send_ascii, not_send_ascii, not_send_ascii, not_send_ascii, not_send_ascii,
   not_send_ascii, not_send_ascii, not_send_ascii, not_send_ascii, not_send_ascii,
   '\0'},
  {"pop_send_ascii_char", "", 0,
   pop_send_ascii, pop_send_ascii, pop_send_ascii, pop_send_ascii, pop_send_ascii,
   pop_send_ascii, pop_send_ascii, pop_send_ascii, pop_send_ascii, pop_send_ascii,
   '\0'},
  {"quote_send_ascii_char", "", 0,
   NULL, NULL, NULL, quote_send_ascii_e, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"select_select", "", 0,
   NULL, NULL, NULL, NULL, select_select,
   NULL, NULL, NULL, NULL, select_select,
   '\0'},
  {"lineend_select", "", 0,
   NULL, NULL, NULL, NULL, lineend_select,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"linestart_select", "", 0,
   NULL, NULL, NULL, NULL, linestart_select,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
/*{ "previous_select",          "",                                     0,
  NULL,         NULL,           NULL,           NULL,           previous_select,
  NULL,         NULL,           NULL,           NULL,           previous_select,
  '\0'},*/
  {"previous_select", "", 0,
   NULL, NULL, NULL, NULL, previous_select,
   NULL, NULL, NULL, NULL, xw_back_inspect,
   '\0'},
/*{ "next_select",              "",                                     0,
  NULL,         NULL,           NULL,           NULL,           next_select,
  NULL,         NULL,           NULL,           NULL,           NULL,
  '\0'},*/
  {"next_select", "", 0,
   NULL, NULL, NULL, NULL, next_select,
   NULL, NULL, NULL, NULL, xw_next_inspect,
   '\0'},
  {"backward_select", "", 0,
   NULL, NULL, NULL, NULL, backward_select,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"forward_select", "", 0,
   NULL, NULL, NULL, NULL, forward_select,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"reconnect_jserver", "", 0,
   reconnect_server, reconnect_server, reconnect_server, reconnect_server,
   reconnect_server,
   reconnect_server, reconnect_server, reconnect_server, reconnect_server,
   reconnect_server,
   '\0'},
  {"henkan_on", "", 0,
   henkan_off, henkan_off, henkan_off, henkan_off, henkan_off,
   henkan_off, henkan_off, henkan_off, henkan_off, henkan_off,
   '\0'},
  {"inspect", "", 0,
   inspect_kouho, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"sakujo_kouho", "", 0,
   sakujo_kouho, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"nobi_henkan_dai", "", 0,
   NULL, NULL, nobi_henkan_dai, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"nobi_henkan", "", 0,
   NULL, NULL, nobi_henkan, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"del_entry", "", 0,
   NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, inspectdel,
   '\0'},
  {"use_entry", "", 0,
   NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, inspectuse,
   '\0'},
  {"com_entry", "", 0,
   NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"kana_henkan", "", 1,
   NULL, kankana_ren_henkan, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"hindo_set", "", 0,
   NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"ima_bit", "", 0,
   NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL,
   '\0'},
  {"change_locale", "", 0,
   lang_c, lang_c, lang_c, lang_c, lang_c,
   lang_c, lang_c, lang_c, lang_c, lang_c,
   '\0'},
  {"change_locale_ct", "", 0,
   lang_ct, lang_ct, lang_ct, lang_ct, lang_ct,
   lang_ct, lang_ct, lang_ct, lang_ct, lang_ct,
   '\0'}
};
#endif /* XJUTIL */

int kansuu_hyo_entry_suu = sizeof (kansuu_hyo) / sizeof (struct kansuu);

/** argument expansion. */
static int
expand_argument (st)
     char *st;
{
  int num;

  if (*st == '^')
    {
      num = (*(st + 1)) & 0x1f;
    }
  else if (*st == '0' || *st == '\\')
    {
      if (*(st + 1) == 'x' || *(st + 1) == 'X')
        {
          sscanf (st + 2, "%x", &num);
        }
      else
        {
          sscanf (st, "%o", &num);
        }
    }
  else if (!('0' <= *st && *st <= '9'))
    {
      return (-1);
    }
  else
    {
      num = atoi (st);
    }
  if (num >= TBL_SIZE)
    {
      print_out1 ("In uumkey file, you can only bind a function to code less than %d.", (char *) TBL_SIZE);
      return (-1);
    }
  return (num);
}

static void
remove_key_bind (p)
     register struct kansuu *p;
{
  register int tbl;
  register int com;
#ifdef XJUTIL
#define MTABLE main_table
#else
#define MTABLE (m_table->m_table)
  register MainTable *m_table;

  m_table = c_c->m_table;
#endif

  for (tbl = 0; tbl < TBL_CNT; tbl++)
    {
      if (p->func[tbl] != NULL)
        {
          for (com = 0; com < TBL_SIZE; com++)
            {
              if (MTABLE[tbl][com] == p->func[tbl])
                {
                  MTABLE[tbl][com] = (int (*)()) 0;
                  MTABLE[tbl][com] = 0;
                }
            }
        }
    }
}

static struct kansuu *
find_entry (com_name, tbl)
     char *com_name;
     struct kansuu **tbl;
{
  register int k;
  register struct kansuu **p;

  p = tbl + ((com_name[0] - 'a') * ONE_CHAR_TBL);
  for (k = 0; k < ONE_CHAR_TBL && *p; k++, p++)
    {
      if (strcmp (com_name, (*p)->kansuumei) == 0)
        {
          if ((*p)->key_binded)
            {
              return (*p);
            }
        }
    }
  return (NULL);
}

int
expand_expr (s, lang)
     char *s, *lang;
{
  char tmp[EXPAND_PATH_LENGTH];
  register char *p, *s1;
  int noerr, expandsuc;
  struct passwd *u;
  extern struct passwd *getpwnam ();

  if (*s != '~' && *s != '@')
    {
      strcpy (tmp, s);
      *s = '\0';
      noerr = 1;
    }
  else
    {
      if ((int) strlen (s) >= EXPAND_PATH_LENGTH)
        return (-1);

      s1 = s;
      if (NULL != (p = index (++s1, '/')))
        {
          strcpy (tmp, p);
          *p = '\0';
        }
      else
        *tmp = '\0';
      if (*s == '~')
        {
          if (*s1)
            {
              noerr = expandsuc = (NULL != (u = getpwnam (s1)) && (int) (strlen (p = u->pw_dir) + strlen (tmp)) < EXPAND_PATH_LENGTH);
            }
          else
            {
              noerr = expandsuc = (NULL != (p = getenv ("HOME")) && (int) (strlen (p) + strlen (tmp)) < EXPAND_PATH_LENGTH);
            }
        }
      else
        {                       /* then, *s must be '@' */
          if (!strcmp (s1, "HOME"))
            {
              noerr = expandsuc = (NULL != (p = getenv ("HOME")) && (int) (strlen (p) + strlen (tmp)) < EXPAND_PATH_LENGTH);
            }
          else if (!strcmp (s1, "LIBDIR"))
            {
              noerr = expandsuc = ((int) (strlen (p = LIBDIR) + strlen (tmp)) < EXPAND_PATH_LENGTH);
            }
          else if (!strcmp (s1, "LANG"))
            {
              noerr = expandsuc = ((int) (strlen (p = lang) + strlen (tmp)) < EXPAND_PATH_LENGTH);
            }
          else
            {                   /* @HOME, @LIBDIR igai ha kaenai */
              noerr = 1;
              expandsuc = 0;
            }
        }
      if (expandsuc)
        strcpy (s, p);
    }

  if (noerr)
    {
      int len = strlen ("@USR");
      int len1 = strlen ("@LANG");
      p = tmp;
      for (; *p; p++)
        {
          if (!strncmp (p, "@USR", len))
            {
              if ((int) (strlen (username) + strlen (p) + strlen (s) - len) < EXPAND_PATH_LENGTH)
                {
                  strcat (s, username);
                  p += len - 1;
                }
              else
                {
                  return (-1);
                }
            }
          else if (!strncmp (p, "@LANG", len1))
            {
              if ((int) (strlen (lang) + strlen (p) + strlen (s) - len) < EXPAND_PATH_LENGTH)
                {
                  strcat (s, lang);
                  p += len1 - 1;
                }
              else
                {
                  return (-1);
                }
            }
          else
            {
              strncat (s, p, 1);
            }
        }
    }
  return (noerr ? 0 : -1);
}

static int
key_binding (file_name, lang)
     char *file_name;
     char *lang;
{
  register int k, kk;
  FILE *fp, *fopen ();
  char func_name[64];
  int com_cnt;
  int com_num;
  char buf[256];
  char com_name[16][EXPAND_PATH_LENGTH];
  int lc = 0;
  int tbl;
  register struct kansuu **one_char_tbl, **p, *pp;

  if ((fp = fopen (file_name, "r")) == NULL)
    {
      print_out1 ("Key_binding file \"%s\" doesn't exist.", file_name);
      return (-1);
    }
  if (!(p = (struct kansuu **) Calloc (ONE_CHAR_TBL * ('z' - 'a' + 1), sizeof (struct kansuu *))))
    {
      malloc_error ("allocation of work area for key binding");
      fclose (fp);
      return (-1);
    }
  one_char_tbl = p;
  for (k = 0; k < kansuu_hyo_entry_suu; k++)
    {
      p = one_char_tbl + ((kansuu_hyo[k].kansuumei[0] - 'a') * ONE_CHAR_TBL);
      for (kk = 0; *p && kk < ONE_CHAR_TBL; kk++, p++);
      if (kk == ONE_CHAR_TBL)
        {
          print_out ("work area for key binding overflowed");
          Free (one_char_tbl);
          fclose (fp);
          return (-1);
        }
      *p = &kansuu_hyo[k];
    }

  for (tbl = 0; tbl < TBL_CNT; tbl++)
    {
      romkan_clear_tbl[tbl][rubout_code] = 0;
    }
  main_table[1][rubout_code] = rubout_c;
  main_table[5][rubout_code] = t_rubout;
  main_table[6][rubout_code] = t_rubout;

  lc = 0;
  while (fgets (buf, 256, fp))
    {
      lc += 1;
      if ((*buf == ';') || (*buf == ':') || (*buf == '\n'))
        continue;
      if ((com_cnt = sscanf (buf, "%s %s %s %s %s %s %s %s %s %s %s",
                             func_name, com_name[0], com_name[1], com_name[2], com_name[3], com_name[4], com_name[5], com_name[6], com_name[7], com_name[8], com_name[9])) < 2)
        {
          print_out2 ("In uumkey file \"%s\", Line %d is illegal.", file_name, (char *) lc);
          continue;             /* every command needs at least 1 argument. */
        }

      if (strcmp (func_name, "include") == 0)
        {
          if (0 != expand_expr (com_name[0], lang))
            {
              print_out3 ("In uumkey file \"%s\", %s %s.", file_name, com_name[0], msg_get (cd, 22, default_message[22], lang));
            }
          key_binding (com_name[0], lang);
        }
      else if (strcmp (func_name, "unset") == 0)
        {
          if ((pp = find_entry (com_name[0], one_char_tbl)) != NULL)
            {
              pp->key_binded = 0;
              remove_key_bind (pp);
            }
          else if (strcmp (com_name[0], "quote_keyin") == 0)
            {
#ifndef XJUTIL
              quote_code = -1;
#endif /* !XJUTIL */
            }
        }
      else if (strcmp (func_name, "quote_keyin") == 0)
        {
#ifndef XJUTIL
          if ((com_num = expand_argument (com_name[0])) < 0)
            {
              print_out3 ("In uumkey file \"%s\",%s is an inappropriate code for %s.", file_name, com_name[0], func_name);
              fflush (stderr);
            }
          else
            quote_code = com_num;
#endif /* !XJUTIL */
        }
      else
        {
          p = one_char_tbl + ((func_name[0] - 'a') * ONE_CHAR_TBL);
          for (k = 0; k < ONE_CHAR_TBL && *p; k++, p++)
            {
              if (strcmp (func_name, (*p)->kansuumei) == 0)
                {               /*found! */
                  if ((*p)->key_binded)
                    remove_key_bind (*p);
                  (*p)->key_binded = (char) 1;
                  for (kk = 0; kk < com_cnt - 1; kk++)
                    {
                      com_num = expand_argument (com_name[kk]);
                      if (com_num < 0)
                        {
                          print_out3 ("In uumkey file \"%s\",%s is an inappropriate code for %s.", file_name, com_name[kk], func_name);
                          fflush (stderr);
                        }
                      else
                        {
                          for (tbl = 0; tbl < TBL_CNT; tbl++)
                            {
                              if ((main_table[tbl][com_num] != NULL) && ((*p)->func[tbl] != NULL))
                                {
                                  print_out7 ("In uumkey file \"%s\", %s(0x%x)%s at line %d (%s)(table No. %d).",
                                              file_name, (char *) com_name[kk], (char *) com_num, (char *) msg_get (cd, 23, default_message[23], lang), (char *) lc, (*p)->kansuumei, (char *) tbl);
                                  main_table[tbl][com_num] = (*p)->func[tbl];
                                  romkan_clear_tbl[tbl][com_num] = (*p)->romkan_flag;
                                }
                              else if ((*p)->func[tbl] != NULL)
                                {
                                  main_table[tbl][com_num] = (*p)->func[tbl];
                                  romkan_clear_tbl[tbl][com_num] = (*p)->romkan_flag;
                                }
                            }
                        }
                    }
                  break;
                }               /* end of routine when command is founded. */
            }                   /* loop while command is founded */
          if (k == ONE_CHAR_TBL || !*p)
            {
              print_out2 ("In uumkey file \"%s\", unknown entry name %s.", file_name, func_name);
            }
        }
    }
  fclose (fp);
  Free (one_char_tbl);
  return (0);
}

int
init_key_table (lang)
     char *lang;
{
  FILE *fp = NULL, *fopen ();
  char file_name[PATHNAMELEN];
  int j, k;
  extern int fclose ();

#ifdef  XJUTIL
  if (uumkey_name && *uumkey_name)
    strcpy (file_name, uumkey_name);
  if ((fp = fopen (file_name, "r")) != NULL)
    {
      fclose (fp);
    }
  else
    {
      print_out2 ("%s %s", msg_get (cd, 72, default_message[72], lang), CPFILE);
      return (-1);
    }
#else /* XJUTIL */
  /* from uumrc */
  if (uumkeyname != NULL && uumkeyname[0] != '\0')
    {
      strcpy (file_name, uumkeyname);
      if (fp = fopen (file_name, "r"))
        fclose (fp);
    }
  if (!fp)
    {
      if (strcpy (file_name, LIBDIR), strcat (file_name, "/"), strcat (file_name, lang), strcat (file_name, CPFILE), (fp = fopen (file_name, "r")) != NULL)
        {
          /* default */
          if (!(uumkeyname = alloc_and_copy (file_name)))
            {
              return (-1);
            }
          fclose (fp);
        }
    }
  if (!fp)
    {
      print_out2 ("%s %s", msg_get (cd, 21, default_message[21], lang), CPFILE);
      return (-1);
    }

  if (!(c_c->m_table = (MainTable *) Malloc (sizeof (MainTable))))
    {
      malloc_error ("allocation of buffer for key binding");
      return (-1);
    }
  c_c->m_table->re_count = 1;
#endif /* XJUTIL */
  for (j = 0; j < TBL_CNT; j++)
    {
      for (k = 0; k < TBL_SIZE; k++)
        {
          main_table[j][k] = (int (*)()) 0;
        }
      if (j < 3)
        {
          main_table[j][NEWLINE] = (int (*)()) 0;
          main_table[j][CR] = (int (*)()) 0;
        }
    }
  if (key_binding (file_name, lang) == -1)
    return (-1);
  return (0);
}

/*
  Local Variables:
  eval: (setq kanji-flag t)
  eval: (setq kanji-fileio-code 0)
  eval: (mode-line-kanji-code-update)
  End:
*/
