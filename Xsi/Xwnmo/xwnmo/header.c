/*
 * $Id: header.c,v 1.12.2.1 1999/02/08 08:08:45 yamasita Exp $
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
/*
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991, 1992
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that all of the following conditions are satisfied:
 *
 * 1) The above copyright notices appear in all copies
 * 2) Both those copyright notices and this permission notice appear
 *    in supporting documentation
 * 3) The name of "Wnn" isn't changed unless substantial modifications
 *    are made, or
 * 3') Following words followed by the above copyright notices appear
 *    in all supporting documentation of software based on "Wnn":
 *
 *   "This software is based on the original version of Wnn developed by
 *    Kyoto University Research Institute for Mathematical Sciences (KURIMS),
 *    OMRON Corporation and ASTEC Inc."
 *
 * 4) The names KURIMS, OMRON and ASTEC not be used in advertising or
 *    publicity pertaining to distribution of the software without
 *    specific, written prior permission
 *
 * KURIMS, OMRON and ASTEC make no representations about the suitability
 * of this software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 *
 * Wnn consortium is one of distributors of the official Wnn source code
 * release.  Wnn consortium also makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * KURIMS, OMRON, ASTEC AND WNN CONSORTIUM DISCLAIM ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL KURIMS, OMRON, ASTEC OR
 * WNN CONSORTIUM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTUOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */
/*      Version 4.0
 */
#include <stdio.h>
#include "sdefine.h"
#include "xim.h"
#include "sheader.h"
#include "ext.h"

int rubout_code = 127;
int kk_on = 0;
int quote_code = -1;

char *ximrc_file = NULL;
char *cvt_key_file = NULL;
char *cvt_fun_file = NULL;
char *cvt_meta_file = NULL;
int defined_by_option = 0;

WNN_DIC_INFO *dicinfo;
int dic_list_size;

char *prgname;                  /* program name (argv[0])       */

char *xim_title = NULL;         /* title string when henkan-off */

char *display_name = NULL;      /* display string               */
#ifdef  USING_XJUTIL
char *xjutil_name = NULL;       /* xjutil string                */
#endif /* USING_XJUTIL */

/*
 * Default env
 */
char *def_lang;
char *def_locale = NULL;
char *world_locale = NULL;

XIMLangDataBase *cur_lang = NULL;

WnnClientRec *c_c = NULL;
XIMClientRec *cur_p = NULL;
XIMClientRec *cur_x = NULL;
XIMInputRec *cur_input = NULL;
XInputManager *xim = NULL;
XIMInputList input_list = NULL;
WnnClientList wnnclient_list = NULL;
XIMClientList ximclient_list = NULL;
ReadRkfileList read_rkfile_list = NULL;
BoxRec *box_list = NULL;

Romkan *cur_rk = NULL;
RomkanTable *cur_rk_table = NULL;

char read_rk_file[MAX_LANGS][62];

struct msg_cat *cd;

char *root_def_servername = NULL;
char *root_def_reverse_servername = NULL;
char *root_username = NULL;
Bool root_henkan_off_def = 1;
char *root_uumkeyname = NULL;
char *root_rkfilename = NULL;

char *world_lang = NULL;
char *arg_string = NULL;

wchar *wc_buf = NULL;
unsigned char *ct_buf = NULL;
unsigned char *c_buf = NULL;
int wc_buf_max = 0;
int ct_buf_max = 0;
int c_buf_max = 0;
#ifndef X_WCHAR
wchar_t *wt_buf = NULL;
int wt_buf_max = 0;
#endif /* !X_WCHAR */

ConvCode cvt_key_tbl[MAX_CVT_ENTRY_CNT];
int cvt_key_tbl_cnt = 0;        /* convert table count */

XIMLangDataBase *language_db = NULL;
XIMLcNameRec *lc_name_list = NULL;

FunctionTable function_db[] = {
  {"Normal",
   NULL, NULL, call_t_redraw_move_normal,
   call_t_redraw_move_1_normal, call_t_redraw_move_2_normal,
   call_t_print_l_normal, NULL, char_len_normal, char_q_len_normal,
   t_redraw_move_normal, t_print_l_normal, c_top_normal, c_end_normal,
   c_end_normal, NULL, errorkeyin, call_jl_yomi_len},
#ifdef  CHINESE
  {"Yincoding",
   print_out_yincod, input_yincod, call_t_redraw_move_yincod,
   call_t_redraw_move_1_yincod, call_t_redraw_move_2_yincod,
   call_t_print_l_yincod, redraw_when_chmsig_yincod, char_len_yincod,
   char_q_len_yincod, t_redraw_move_yincod,
   t_print_l_yincod, c_top_yincod, c_end_yincod, c_end_nobi_yincod,
   NULL, errorkeyin_q, not_call_jl_yomi_len},
#endif /* CHINESE */
  {NULL,
   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL, NULL}
};
