/*
 * $Id: jutil.h,v 1.2 2014/08/01 22:04:26 itisango Exp $
 */
/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright FreeWnn Project 1999, 2000, 2002, 2010, 2013
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


#ifndef FRWNN_JUTIL_H
#define FRWNN_JUTIL_H 1

#include <stdio.h>

#if __STDC__
#define FRWNN_PARAMS(paramlist)	paramlist
#else
#define FRWNN_PARAMS(paramlist)	()
#endif	/* __STDC__ */

/* -- macro  -- */
/* ujisf.c  */
#define WNN_HINSI_LEN 4096

/* -- extern  --  */
/* wdreg.c  */
extern struct JT jt;
/* ujisf.c  */
extern w_char file_comment[WNN_COMMENT_LEN];
extern w_char hinsi_list[WNN_HINSI_LEN];
extern struct je **jeary;
extern int wnnerror;
extern int lc;
#ifdef CHINESE
extern int pzy_flag;
#endif	/*  CHINESE  */

/* atod.c  */

/* -- function --  */
/* ujisf.c  */
extern int init_heap FRWNN_PARAMS((int, int, int, int, FILE *));
extern void exit1 ();
extern int get_line FRWNN_PARAMS((register char *));
extern void unget_line FRWNN_PARAMS((char *));
extern char *get_string FRWNN_PARAMS((register char *, char *));
extern void bad_line FRWNN_PARAMS((char *));
extern void error_no_heap FRWNN_PARAMS((void));
extern int w_stradd FRWNN_PARAMS((register w_char *, register w_char **));

#ifdef CHINESE
extern void ujis_header FRWNN_PARAMS((int *));
#else
extern void ujis_header FRWNN_PARAMS((void));
#endif	/* CHINESE  */

extern void read_ujis FRWNN_PARAMS((int, int, int));
extern void reverse_yomi FRWNN_PARAMS((void));
extern void print_je FRWNN_PARAMS((register struct je *, register FILE *, int, int));
#ifdef nodef
extern kprint FRWNN_PARAMS((register FILE *, register w_char *));
#endif	/* nodef  */
extern void output_ujis FRWNN_PARAMS((register FILE *, int, int));
extern int init_heap FRWNN_PARAMS((int, int, int, int, FILE *));
extern void init_jeary FRWNN_PARAMS((void));
extern void exit1 FRWNN_PARAMS((void));
extern int sort_func_je FRWNN_PARAMS((char *, char *));
extern int sort_func_je_kanji FRWNN_PARAMS((char *, char *));
extern int sort_func FRWNN_PARAMS((register char *, register char *, int));
extern void sort FRWNN_PARAMS((void));
extern void sort_if_not_sorted FRWNN_PARAMS((void));
extern void sort_kanji FRWNN_PARAMS((void));
extern void uniq_je FRWNN_PARAMS((int (*) ()));
#ifdef nodef
extern int make_kanji_str FRWNN_PARAMS((register UCHAR *, register UCHAR *));
#endif	/* nodef  */
extern int Sorted FRWNN_PARAMS((register char *, register int, int, int (*) ()));
extern int is_katakana FRWNN_PARAMS((register char *, register char *));

/* dic_head.c */
/* extern int output_header FRWNN_PARAMS((FILE *, struct JT *, struct wnn_file_head *));  */
extern int input_header FRWNN_PARAMS((FILE *, struct JT *, struct wnn_file_head *));
extern int input_hindo_header FRWNN_PARAMS((FILE *, struct HJT *, struct wnn_file_head *));

/* atod.c  */
extern void upd_kanjicount FRWNN_PARAMS((int));
extern void  output_dic_data FRWNN_PARAMS((void));


#endif	/*  FRWNN_JUTIL_H  */
