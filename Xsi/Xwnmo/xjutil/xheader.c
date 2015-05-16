/*
 * $Id: xheader.c,v 1.11.2.1 1999/02/08 07:43:36 yamasita Exp $
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
#include "config.h"
#include "commonhd.h"
#include "sdefine.h"
#include "xjutil.h"
#include "sxheader.h"
#include "xext.h"

int max_env = 0;
WnnEnv *normal_env = NULL;
WnnEnv *reverse_env = NULL;
WnnEnv *cur_normal_env = NULL;
WnnEnv *cur_reverse_env = NULL;

int save_env_id[32];
int save_env_reverse_id[32];
int env_is_reverse = 0;

w_char *input_buffer;
w_char *return_buf;             /* return you mojiretsu buffer */

int cur_bnst_ = 0;              /* current bunsetsu pointer */

int rubout_code = 127;

int (*main_table[TBL_CNT][TBL_SIZE]) ();
char romkan_clear_tbl[TBL_CNT][TBL_SIZE];
char jishopath[256];
char hindopath[256];
char fuzokugopath[256];

ClientBuf *c_b;

int excellent_delete = 1;

int *bunsetsu;
int *bunsetsuend;
int *touroku_bnst;
int touroku_bnst_cnt;
w_char *kill_buffer;
w_char *remember_buf;
int kill_buffer_offset = 0;

int touroku_comment = 0;

char *uumrc_name = NULL;
char *envrc_name = NULL;
char *uumkey_name = NULL;
char *cvt_key_file = NULL;
char *cvt_fun_file = NULL;
char *cvt_meta_file = NULL;
char *rkfile_name = NULL;

char *jserver_name = NULL;
char *display_name = NULL;
char *username = NULL;

int maxbunsetsu;
int max_ichiran_kosu;
int maxchg;
int maxlength;

WNN_DIC_INFO *dicinfo;
int dic_list_size;

char *prgname;                  /* argv[0] */

struct wnn_ret_buf rb = { 0, NULL };
struct wnn_ret_buf dicrb = { 0, NULL };
struct wnn_ret_buf wordrb = { 0, NULL };

Xjutil *xjutil = NULL;
XIMRootRec *cur_root = NULL;
JutilTextRec *cur_text = NULL;

Romkan *cur_rk = NULL;
RomkanTable *cur_rk_table = NULL;

BoxRec *box_list = NULL;

XjutilFSList font_set_list = NULL;
XjutilFSRec *cur_fs = NULL;

struct msg_cat *cd;

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

FunctionTable *f_table = NULL;

FunctionTable function_db[] = {
  {"Normal",
   NULL, NULL, call_t_redraw_move_normal,
   call_t_redraw_move_1_normal, call_t_redraw_move_2_normal,
   call_t_print_l_normal, NULL, char_len_normal, char_q_len_normal,
   t_redraw_move_normal, t_print_l_normal, c_top_normal, c_end_normal,
   c_end_normal, hani_settei_normal, NULL, NULL},
#ifdef  CHINESE
  {"Yincoding",
   print_out_yincod, input_yincod, call_t_redraw_move_yincod,
   call_t_redraw_move_1_yincod, call_t_redraw_move_2_yincod,
   call_t_print_l_yincod, redraw_when_chmsig_yincod, char_len_yincod,
   char_q_len_yincod, t_redraw_move_yincod,
   t_print_l_yincod, c_top_yincod, c_end_yincod, c_end_nobi_yincod,
   hani_settei_yincod, NULL, NULL},
#endif /* CHINESE */
  {NULL,
   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}
};
