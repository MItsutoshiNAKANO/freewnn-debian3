/*
 * $Id: sxheader.h,v 1.2 2001/06/14 18:16:13 ura Exp $
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
/**************************
 * header of standard i/o 
 **************************/

#include "jslib.h"

extern int max_env;
extern WnnEnv *normal_env;
extern WnnEnv *reverse_env;
extern WnnEnv *cur_normal_env;
extern WnnEnv *cur_reverse_env;
extern int save_env_id[32];
extern int save_env_reverse_id[32];
extern int env_is_reverse;

extern w_char *input_buffer;
extern w_char *return_buf;

extern int cur_bnst_;           /* current bunsetsu pointer */

extern int rubout_code;

extern char romkan_clear_tbl[TBL_CNT][TBL_SIZE];
extern int (*main_table[TBL_CNT][TBL_SIZE]) (); /* holding commands */
extern char jishopath[];
extern char hindopath[];
extern char fuzokugopath[];

extern ClientBuf *c_b;

extern int excellent_delete;

extern int *bunsetsu;
extern int *bunsetsuend;
extern int *touroku_bnst;
extern int touroku_bnst_cnt;
extern w_char *kill_buffer;
extern w_char *remember_buf;
extern int kill_buffer_offset;

extern int touroku_comment;

extern char *uumrc_name;
extern char *envrc_name;
extern char *uumkey_name;
extern char *cvt_key_file;
extern char *cvt_fun_file;
extern char *cvt_meta_file;
extern char *rkfile_name;

extern char *jserver_name;
extern char *display_name;
extern char *username;

extern int maxbunsetsu;
extern int max_ichiran_kosu;
extern int maxchg;
extern int maxlength;

extern WNN_DIC_INFO *dicinfo;
extern int dic_list_size;

extern char *prgname;

extern struct wnn_ret_buf rb;
extern struct wnn_ret_buf dicrb;
extern struct wnn_ret_buf wordrb;

extern Xjutil *xjutil;
extern XIMRootRec *cur_root;
extern JutilTextRec *cur_text;
extern Romkan *cur_rk;
extern RomkanTable *cur_rk_table;
extern BoxRec *box_list;

extern XjutilFSList font_set_list;
extern XjutilFSRec *cur_fs;


extern char *default_message[];
extern struct msg_cat *cd;
extern BoxRec *create_box ();

extern wchar *wc_buf;
extern unsigned char *ct_buf;
extern unsigned char *c_buf;
extern int wc_buf_max;
extern int ct_buf_max;
extern int c_buf_max;
#ifndef X_WCHAR
extern wchar_t *wt_buf;
extern int wt_buf_max;
#endif /* !X_WCHAR */

extern ConvCode cvt_key_tbl[];
extern int cvt_key_tbl_cnt;

extern FunctionTable *f_table;
extern FunctionTable function_db[];
