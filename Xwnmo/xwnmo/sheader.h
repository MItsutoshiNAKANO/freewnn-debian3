/*
 * $Id: sheader.h,v 1.2 2001/06/14 18:16:17 ura Exp $
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

#ifndef _HEADER_H_
#define _HEADER_H_

extern int rubout_code;
extern int kk_on;
extern int quote_code;

extern char *ximrc_file;
extern char *cvt_key_file;
extern char *cvt_fun_file;
extern char *cvt_meta_file;
extern int defined_by_option;

extern WNN_DIC_INFO *dicinfo;
extern int dic_list_size;

extern char *prgname;

extern char *xim_title;

extern char *display_name;
#ifdef  USING_XJUTIL
extern char *xjutil_name;
#endif /* USING_XJUTIL */

extern char *def_lang;
extern char *def_locale;
extern char *world_locale;

extern XIMLangDataBase *cur_lang;

extern WnnClientRec *c_c;
extern XIMClientRec *cur_p;
extern XIMClientRec *cur_x;
extern XIMInputRec *cur_input;
extern XInputManager *xim;
extern XIMInputList input_list;
extern WnnClientList wnnclient_list;
extern XIMClientList ximclient_list;
extern ReadRkfileList read_rkfile_list;
extern BoxRec *box_list;

extern Romkan *cur_rk;
extern RomkanTable *cur_rk_table;

extern char read_rk_file[MAX_LANGS][62];

extern struct msg_cat *cd;

extern char *root_def_servername;
extern char *root_def_reverse_servername;
extern char *root_username;
extern Bool root_henkan_off_def;
extern char *root_uumkeyname;
extern char *root_rkfilename;

extern char *world_lang;
extern char *arg_string;

extern char *default_message[];

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

extern XIMLangDataBase *language_db;
extern XIMLcNameRec *lc_name_list;
extern FunctionTable function_db[];
#endif /* _HEADER_H_ */
