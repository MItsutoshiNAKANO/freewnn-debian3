/*
 *  $Id: sdefine.h,v 1.5 2013/09/02 11:01:40 itisango Exp $
 */

/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright FreeWnn Project 1999, 2000, 2006
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

/*************************
 * define of standard i/o
 *************************/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "commonhd.h"

/* ncurses' term.h defines them to either 0 or 1! */
#ifndef HAVE_TERMIOS_H
#  define DONT_HAVE_TERMIOS_H
#endif
#ifndef HAVE_TERMIO_H
#  define DONT_HAVE_TERMIO_H
#endif
#ifndef HAVE_SYS_TERMIO_H
#  define DONT_HAVE_SYS_TERMIO_H
#endif
#ifndef HAVE_SGTTY_H
#  define DONT_HAVE_SGTTY_H
#endif

#if defined(HAVE_TERMINFO)
#  if defined(HAVE_CURSES_H)
#    include <curses.h>
#  elif defined(HAVE_NCURSES_H)
#    include <ncurses.h>
#  else
#    error "no terminfo header"
#  endif /* HAVE_CURSES_H */
#  ifdef HAVE_TERM_H
#    include <term.h>
#  endif
#else /* HAVE_TERMINFO */
#  if defined(HAVE_TERMCAP_H)
#    include <termcap.h>
#  endif /* HAVE_TERMCAP_H */
#endif /* HAVE_TERMINFO */

#ifdef DONT_HAVE_TERMIOS_H
#  undef HAVE_TERMIOS_H
#  undef DONT_HAVE_TERMIOS_H
#endif
#ifdef DONT_HAVE_TERMIO_H
#  undef HAVE_TERMIO_H
#  undef DONT_HAVE_TERMIO_H
#endif
#ifdef DONT_HAVE_SYS_TERMIO_H
#  undef HAVE_SYS_TERMIO_H
#  undef DONT_HAVE_SYS_TERMIO_H
#endif
#ifdef DONT_HAVE_SGTTY_H
#  undef HAVE_SGTTY_H
#  undef DONT_HAVE_SGTTY_H
#endif

#ifdef HAVE_KILLPG
# define KILLPG(pgrp, sig) killpg(pgrp, sig)
#else
# define KILLPG(pgrp, sig) kill(-(pgrp), sig)
#endif /* HAVE_KILLPG */

#if defined(HAVE_GETPGID)	/* SVR4 and most modern systems */
#  define GETPGID(pid) getpgid(pid)
#elif defined(HAVE_GETPGRP) && !defined(GETPGRP_VOID) /* 4.3BSD */
#  define GETPGID(pid) getpgrp(pid)
#elif defined(uniosu)
#  define GETPGID(pid) ngetpgrp(pid)
#else
/* no way to get process group id */
#endif /* GETPGID */

#if defined(HAVE_GETPGRP)
#  if defined(GETPGRP_VOID)
#    define GETMYPGRP() getpgrp()	/* SYSV, POSIX */
#  else
#    define GETMYPGRP() getpgrp(0)
#  endif /* !GETPGRP_VOID */
#elif defined(GETPGID)
#  define GETMYPGRP() GETPGID(getpid())
#else
/* probably some build error occured */
#  error "don't know how to get my process group id"
#endif /* GETMYPGRP */

#define MAXCHG 80               /* 解析可能文字数 */
        /*漢字バッファ(次候補、単語検索など)の大きさ */
#define MAX_ICHIRAN_KOSU 36     /* 一覧表示字の表示個数 */
#define MAXWORDS        MAXJIKOUHO      /* 単語検索での検索語数 */
#define NBUN    2               /* n文節解析 */
#define JISHO_PRIO_DEFAULT 5    /*辞書プライオリティのデフォルト値 */

/* 評価関数の係数 */
#define HINDOVAL 3
#define LENGTHVAL 100
#define JIRITUGOVAL 1
#define FLAGVAL 200             /*今使ったよビットの係数 */
#define JISHOPRIOVAL 5

#define TBL_CNT 10              /* key_table no kazu */
/* Two tables are add. one is for selecting zenkouho's, and the other is jisho-ichiran. */
/* One more table is add, which is used in inspect */
/* Deleted table[7] because those are for Wnn3 */
#define TBL_SIZE 256

#define ESCAPE_CHAR(c)  (((c) < ' ') || ((c) == 127))
#define NORMAL_CHAR(c)  (((c) >= ' ') && ((c) < 127))
#define NOT_NORMAL_CHAR(c)      (((c) >= 128) && ((c) < 160))
#define ONEBYTE_CHAR(c)         (!((c) & ~0xff))        /* added by Nide */
/*
#define NISEMONO(c)             ((c) & 0x80000000)
#define HONMONO(c)              (!NISEMONO(c))
#define KANJI_CHAR(c)           ((c) >= 256 )
*/
#define KANJI_CHAR(c)           (((c) >= 256 ) && ((c) & 0xff00) != 0x8e00)

#define zenkaku(x)(KANJI_CHAR(x) || ESCAPE_CHAR(x))
#define hankaku(x) (!zenkaku(x))
#define ZENKAKU_HYOUJI(x)     zenkaku(x)
        /* CHANGE AFTERWARD TO BE ABLE TO TREAT hakaku katakana. */

#ifndef min
#define max(a , b) (((a) > (b))?(a) : (b))
#define min(a , b) (((a) <= (b))?(a) : (b))
#endif

#define numeric(x)      (((x >= S_NUM)&&(x <= E_NUM))? True : False)
#define ISKUTENCODE(x)  ((x) == 0xa1a3)

struct jisho_
{                               /* 辞書管理用structure */
  char name[1024];
  char hname[1024];             /* 頻度ファイル名 */
  int dict_no;                  /* server が返してくる辞書ナンバー */
  int prio;
  int rdonly;
};

struct kansuu
{                               /* kansuu_hyo no entry */
  char *kansuumei;
  char *comment;
  int romkan_flag;              /* Clear Romkan or Not.  */
  int (*func[TBL_CNT]) ();
};

#ifndef w_char
#define w_char  unsigned short
#endif

/* #define printf        PRINTF  */
/* #define fprintf       FPRINTF  */
#define remove        REMOVE

#define throw_c(col)  throw_col((col) + disp_mode_length)

#define MAX_HISTORY 10


#define flush() fflush(stdout)
#define print_msg(X) {push_cursor();throw_c(0); clr_line();printf(X);flush();pop_cursor();}
#define print_msg_getc(X) {push_cursor();throw_c(0); clr_line();printf(X);flush();keyin();pop_cursor();}


#define UNDER_LINE_MODE (0x02 | 0x08 | 0x20)

#define OPT_CONVKEY             0x01
#define OPT_RKFILE              0x02
#define OPT_WNNKEY              0x04
#define OPT_FLOW_CTRL           0x08
#define OPT_WAKING_UP_MODE      0x10
#define OPT_VERBOSE             0x20

#define convkey_defined_by_option       (defined_by_option & OPT_CONVKEY)
#define rkfile_defined_by_option        (defined_by_option & OPT_RKFILE)
#define uumkey_defined_by_option        (defined_by_option & OPT_WNNKEY)
#define verbose_option                  (defined_by_option & OPT_VERBOSE)

/*
#define char_len(X)((hankaku(X))? 1 : 2)
*/

/*
  GETOPT string & ALL OPTIONS string for configuration.
  see each config.h for detail.
  NOTE: WHEN YOU MODIFY THESE, YOU ALSO MODIFY do_opt[] ARRAY AND
  ALL config.h!!!!
 */

#define GETOPTSTR   "hHPxXk:c:r:l:D:n:v"
#define ALL_OPTIONS "hHujsUJSPxXkcrlDnvbtBT"
#define OPTIONS     "hHPxXkcrlDnv"

/* for message file */
#include "msg.h"
#define MSG_GET(no)     msg_get(cd, no, NULL, NULL)

#define CWNN_PINYIN             0
#define CWNN_ZHUYIN             1

#define print_out_func          (f_table->print_out_function)
#define input_func              (f_table->input_function)
#define call_t_redraw_move_func (f_table->call_t_redraw_move_function)
#define call_t_redraw_move_1_func (f_table->call_t_redraw_move_1_function)
#define call_t_redraw_move_2_func (f_table->call_t_redraw_move_2_function)
#define call_t_print_l_func     (f_table->call_t_print_l_function)
#define redraw_when_chmsig_func (f_table->redraw_when_chmsig_function)
#define char_len_func           (f_table->char_len_function)
#define char_q_len_func         (f_table->char_q_len_function)
#define t_redraw_move_func      (f_table->t_redraw_move_function)
#define t_print_l_func          (f_table->t_print_l_function)
#define c_top_func              (f_table->c_top_function)
#define c_end_func              (f_table->c_end_function)
#define c_end_nobi_func         (f_table->c_end_nobi_function)
#define call_redraw_line_func   (f_table->call_redraw_line_function)
#define hani_settei_func        (f_table->hani_settei_function)
#define errorkeyin_func         (f_table->errorkeyin_function)
#define call_jl_yomi_len_func   (f_table->call_jl_yomi_len_function)

#define env_normal               (cur_normal_env->env)
#define env_reverse              (cur_reverse_env->env)

#define envrcname               (cur_normal_env->envrc_name)
#define reverse_envrcname       (cur_reverse_env->envrc_name)

#define env_name_s               (cur_normal_env->env_name_str)
#define reverse_env_name_s       (cur_reverse_env->env_name_str)

#define servername              (cur_normal_env->host_name)
#define reverse_servername      (cur_reverse_env->host_name)

#define normal_sticky           (cur_normal_env->sticky)
#define reverse_sticky          (cur_reverse_env->sticky)
