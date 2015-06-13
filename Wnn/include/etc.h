/*
 * $Id: etc.h,v 1.2 2014/08/01 22:04:26 itisango Exp $
 */
/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright FreeWnn Project 1999, 2000, 2001, 2002, 2005, 2006, 2013
 *
 * Maintainer:  FreeWnn Project
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef FRWNN_ETC_H
#define FRWNN_ETC_H 1

#include "commonhd.h"
#include "jlib.h"
#include "jdata.h"
#include "rk_spclval.h"


#if __STDC__
#define FRWNN_PARAMS(paramlist)	paramlist
#else
#define FRWNN_PARAMS(paramlist)	()
#endif	/* __STDC__ */

/* gethinsi.c  */
extern int wnn_find_hinsi_by_name FRWNN_PARAMS((register char *c));


/* xutoj.c  */
extern int through FRWNN_PARAMS((char *, char *, int));
extern int flush_designate FRWNN_PARAMS((w_char *));
extern int extc_to_intc FRWNN_PARAMS((w_char *, unsigned char *, int));
extern int ibit8_to_ebit8 FRWNN_PARAMS((unsigned char *, w_char *, int));
extern unsigned int create_cswidth FRWNN_PARAMS((char *s));
extern void set_cswidth FRWNN_PARAMS((register unsigned int));
extern char *get_cswidth_name FRWNN_PARAMS((const char *));
extern int get_cswidth FRWNN_PARAMS((int));
extern int get_cswidth_by_char FRWNN_PARAMS((register unsigned char));
extern int get_cs_mask FRWNN_PARAMS((int));
extern int columnlen FRWNN_PARAMS((unsigned char *));
extern int columnlen_w FRWNN_PARAMS((w_char *));
extern int ieuc_to_eeuc FRWNN_PARAMS((unsigned char *, w_char *, int));
extern int eeuc_to_ieuc FRWNN_PARAMS((w_char *, unsigned char *, register int));
#ifdef nodef
extern void wnn_delete_ss2 FRWNN_PARAMS((register unsigned int *, register int n));
extern int wnn_byte_count FRWNN_PARAMS((register int));
#endif	/* nodef  */
extern void wnn_delete_w_ss2 FRWNN_PARAMS((register w_char *, register int));
#ifdef JAPANESE
#ifdef  JIS7
extern int iujis_to_jis FRWNN_PARAMS((unsigned char *, w_char *, int));
extern int eujis_to_jis FRWNN_PARAMS((unsigned char *, unsigned char *, int));
extern int sjis_to_jis FRWNN_PARAMS((unsigned char *, unsigned char *, int));
#endif	/* JIS7  */
extern int eujis_to_jis8 FRWNN_PARAMS((unsigned char *, unsigned char *, int));
extern int iujis_to_jis8 FRWNN_PARAMS((unsigned char *, w_char *, int));
extern int jis_to_eujis FRWNN_PARAMS((unsigned char *, unsigned char *, int));
extern int eujis_to_sjis FRWNN_PARAMS((unsigned char *, unsigned char *, int));
extern int iujis_to_sjis FRWNN_PARAMS((unsigned char *, w_char *, int));
extern int sjis_to_iujis FRWNN_PARAMS((w_char *, unsigned char *, int));
extern int sjis_to_eujis FRWNN_PARAMS((unsigned char *, unsigned char *, int));
extern int sjis_to_jis8 FRWNN_PARAMS((unsigned char *, unsigned char *, int));
extern int jis_to_iujis FRWNN_PARAMS((w_char *, unsigned char *, int));
extern int jis_to_sjis FRWNN_PARAMS((unsigned char *, unsigned char *, int));
extern int eujis_to_iujis FRWNN_PARAMS((w_char *, unsigned char *, int));
#endif	/* JAPANESE */
#ifdef  CHINESE
extern int ecns_to_icns FRWNN_PARAMS((w_char *, unsigned char *, int));
extern int icns_to_ecns FRWNN_PARAMS((unsigned char *, w_char *, int));
extern int icns_to_big5 FRWNN_PARAMS((unsigned char *, w_char *, int));
extern int ecns_to_big5 FRWNN_PARAMS((unsigned char *, unsigned char *, int));
extern int big5_to_icns FRWNN_PARAMS((w_char *, unsigned char *, int));
extern int big5_to_ecns FRWNN_PARAMS((unsigned char *, unsigned char *, int));
extern int iugb_to_eugb FRWNN_PARAMS((unsigned char *, w_char *, int));
extern int eugb_to_iugb FRWNN_PARAMS((w_char *, unsigned char *, int));
#endif	/* CHINESE  */
#ifdef  KOREAN
extern int iuksc_to_ksc FRWNN_PARAMS((unsigned char *, w_char *, int));
extern int euksc_to_ksc FRWNN_PARAMS((unsigned char *, unsigned char *, int));
extern int iuksc_to_euksc FRWNN_PARAMS((unsigned char *, w_char *, int));
extern int ksc_to_euksc FRWNN_PARAMS((unsigned char *, unsigned char *, int));
extern int ksc_to_iuksc FRWNN_PARAMS((w_char *, unsigned char *, int));
extern int euksc_to_iuksc FRWNN_PARAMS((w_char *, unsigned char *, int));
#endif	/* KOREAN */

/* hindo.c */
extern int asshuku FRWNN_PARAMS((int));
extern int motoni2 FRWNN_PARAMS((int));

/* gethinsi.c */
extern int wnn_loadhinsi FRWNN_PARAMS((unsigned char *fname));
#ifdef JSERVER
extern w_char *wnn_hinsi_name FRWNN_PARAMS((int));
extern int wnn_hinsi_number FRWNN_PARAMS((w_char *));
extern int wnn_hinsi_list FRWNN_PARAMS((w_char *, w_char **, struct wnn_hinsi_node *, int));
extern int wnn_has_hinsi FRWNN_PARAMS((struct wnn_hinsi_node *, int, w_char *));
#endif	/* JSERVER  */
extern int wnn_find_hinsi_by_name FRWNN_PARAMS((register char *));
extern char *wnn_get_hinsi_name FRWNN_PARAMS((int));
#ifdef JSERVER
extern int wnn_get_fukugou_component_body FRWNN_PARAMS((register int, register unsigned short **));
#endif	/* JSERVER  */
extern int wnn_get_fukugou_component FRWNN_PARAMS((register int, register unsigned short **));

/* dic_atojis.c */
extern void Get_kanji FRWNN_PARAMS((UCHAR *, w_char *, int, w_char *, w_char *, w_char *));
extern void Get_knj1 FRWNN_PARAMS((UCHAR *, w_char *, int, int, w_char *, w_char *, w_char *));
extern int substr FRWNN_PARAMS((char *, w_char *));
extern w_char *kanji_giji_str FRWNN_PARAMS((w_char *, int, w_char *, w_char *));
extern void kanji_esc_str FRWNN_PARAMS((w_char *, w_char *, int oyl));


/* bdic.c  */
#ifndef JS
extern int put_n_EU_str FRWNN_PARAMS((FILE* ofpter, w_char* c, int n));
extern int put_null FRWNN_PARAMS((FILE* ofpter, int n));
extern int put_nstring FRWNN_PARAMS((FILE* ofpter, char* c, int n));
extern int put_n_EU_str FRWNN_PARAMS((FILE* ofpter, w_char* c, int n));
extern int put_short FRWNN_PARAMS((FILE* ofpter, int i));
extern int put_int FRWNN_PARAMS((FILE* ofpter, int i));
extern int get_null FRWNN_PARAMS((FILE* ifpter, int n));
extern int get_nstring FRWNN_PARAMS((FILE* ifpter, int n, char* st));
extern int get_int FRWNN_PARAMS((int* ip, FILE* ifpter));
#ifdef BDIC_WRITE_CHECK
extern void check_backup FRWNN_PARAMS((char* n));
extern void delete_tmp_file FRWNN_PARAMS((char* n));
extern char* make_backup_file FRWNN_PARAMS((char* n));
extern char* make_tmp_file FRWNN_PARAMS((char* n, int copy, FILE** ret_fp));
extern void move_tmp_to_org FRWNN_PARAMS((char* tmp_name, char* org_name, int copy));
#endif /* BDIC_WRITE_CHECK */

extern int create_file_header FRWNN_PARAMS((FILE* ofpter, int file_type, char* file_passwd));
extern int output_file_header FRWNN_PARAMS((FILE* ofpter, struct wnn_file_head* hp));
extern int input_file_header FRWNN_PARAMS((FILE* ifpter,  struct wnn_file_head* hp));
extern int output_file_uniq FRWNN_PARAMS((struct wnn_file_uniq* funiq, FILE* ofpter));
extern int input_file_uniq FRWNN_PARAMS((struct wnn_file_uniq* funiq, FILE* ifpter));
extern int check_inode FRWNN_PARAMS((FILE* f, struct wnn_file_head* fh));
extern int change_file_uniq FRWNN_PARAMS((struct wnn_file_head* fh, char* n));
extern int change_file_uniq1 FRWNN_PARAMS((
	FILE* ofpter, int file_type, char* file_passwd,
	struct wnn_file_uniq* file_uniq));

#ifdef JSERVER
int f_uniq_cmp FRWNN_PARAMS((struct wnn_file_uniq *a, struct wnn_file_uniq* b));
#endif /* JSERVER */

#if !defined(JSERVER)
void vputs FRWNN_PARAMS((char* c, FILE* fp));
int  vputws FRWNN_PARAMS((w_char* w, FILE* fp));
int  put_yomi_str FRWNN_PARAMS((w_char* yomi, FILE* ofpter));
#endif /*  !JSERVER */

void Get_knj2 FRWNN_PARAMS((UCHAR* kptr, int kanji2, w_char* kouho, w_char* yomi, w_char* comment));
void Get_kanji_str_r FRWNN_PARAMS((UCHAR* kptr, w_char** tmpk, w_char** tmpy, w_char** tmpc));
UCHAR kanjiaddr FRWNN_PARAMS((UCHAR* d0, w_char* kanji, w_char* yomi, w_char* comment));
extern int create_null_dic FRWNN_PARAMS((
	char* fn, w_char* comm, char* passwd, char* hpasswd, int which));
extern int create_hindo_file FRWNN_PARAMS((
	struct wnn_file_uniq* funiq,
	char* fn, w_char* comm, char* passwd, int serial));

/* XXX: input_* are not used in JSlib */
extern int input_header_jt FRWNN_PARAMS((FILE* ifpter, struct JT* jt1));
extern int output_header_jt FRWNN_PARAMS((FILE* ofpter, struct JT* jt1));
extern int input_header_hjt FRWNN_PARAMS((FILE* ifpter, struct HJT* hjt1));
extern int output_header_hjt FRWNN_PARAMS((FILE* ofpter, struct HJT* hjt1));

#if !defined(JSERVER)
/* Only used in JUTIL */
void Print_entry FRWNN_PARAMS((
	w_char* yomi, w_char* kstr, w_char* cstr,
	int hindo, int ima, int hinsi, int serial,
	FILE* ofpter, int esc_exp));
#endif /* !JSERVER  */

#endif	/* ndef JS  */
extern void udytoS FRWNN_PARAMS((w_char* yomi, int tnum, char* hostart, struct uind1 * tary));
extern void get_kanji_str FRWNN_PARAMS((UCHAR*, w_char*, w_char*, w_char*));

/* revdic.c */
extern int little_endian ();
extern int revdic FRWNN_PARAMS((struct JT *, int));

/* yincoding.c  */
extern int cwnn_pzy_yincod FRWNN_PARAMS((letter *, letter *, int));
#ifdef CHINESE
extern void cwnn_zy_str_analysis FRWNN_PARAMS((register char *, register char *, w_char *, w_char *));
extern void cwnn_py_str_analysis FRWNN_PARAMS((register char *, register char *, register w_char *, register w_char *));
#endif	/* CHINESE */
#ifdef CONVERT_with_SiSheng
extern int cwnn_yincod_pzy_str FRWNN_PARAMS((register w_char *, register w_char *, int, int));
#endif	/* CONVERT_with_SiSheng  */

#endif	/* FRWNN_ETC_H */
