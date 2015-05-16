/*
 *  $Id: cvt_read.c,v 1.8 2006/03/04 19:01:45 aonoto Exp $
 */

/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright FreeWnn Project 1999, 2000, 2002, 2003, 2006
 * Copyright Taketo Kabe 2003
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

/***********************************************************************
                        convert_read.c                   ＿＿＿
                                                88/06/19｜訂補｜
                                                         ￣￣￣
        ファンクションキーコンバート処理の、変換表リーダ。
        convert_key.c の部分品。指定した変換表及びtermcap・
        terminfoからの読みこみを行う関数を定義してある。
***********************************************************************/
/*  Version 4.0  */
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <ctype.h>
#if STDC_HEADERS
#  include <string.h>
#elif HAVE_STRINGS_H
#  include <strings.h>
#endif /* STDC_HEADERS */
#if HAVE_FCNTL_H
#  include <fcntl.h>
#endif
#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif

/* Solaris 2.x needs termio.h , so include both. */
#if HAVE_TERMIOS_H
#  include <termios.h>
#endif
#if HAVE_TERMIO_H
#  include <termio.h>
#endif

#if defined(HAVE_TERMIOS_H)
/* #  include <termios.h> */
#  define USE_TERMIOS 1
#elif defined(HAVE_TERMIO_H)
/* #  include <termio.h> */
#  define USE_TERMIO  1
#elif defined(HAVE_SYS_TERMIO_H)
#  include <sys/termio.h>
#  define USE_TERMIO  1
#elif defined(HAVE_SGTTY_H)
#  include <sgtty.h>
#  define USE_SGTTY
#else
#  error "No termio header."
#endif /* HAVE_TERMIOS_H */

#ifdef HAVE_TERMINFO
#  define TERMINFO 1
#else
#  define TERMCAP  1
#endif /* HAVE_TERMINFO */

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

#include "wnn_os.h"
#include "cvt_head.h"

#define ENTRY_LEN 10            /* コンバート表のエントリ名の最大長 */

/*
#define NOFILE_NOT_ABORT
*/
 /* これをdefineしておくと、変換表がopenできない時にエラー終了せず、
    単に変換関数が恒等関数になる。 */

#define is_digit(x) (isascii(x) && isdigit(x))
#define is_xdigit(x) (isascii(x) && isxdigit(x))
#define is_upper(x) (isascii(x) && isupper(x))
#define is_lower(x) (isascii(x) && islower(x))
#define is_space(x) (isascii(x) && isspace(x))

#define is_eofnl(x) ((x) == '\n'|| (x) == EOF)
#define is_eofsp(x) (is_space(x) || (x) == EOF)
#define is_octal(x) (is_digit(x) && (x) < '8')

#define ESCCHR  '\033'

 /*     convert_key 用変換表のファイル名のデフォルトをセット。
    変換表の名が明示的に指定されたら、それがこの変数の値になる。 */
static char *convert_filename;
static char default_cvt_fn[128];

static FILE *convert_hyo;


 /** エラーメッセージを表示 */
#define errdsp1(mes, conv_fnm)                                          \
        fprintf(stderr, "%s: %s.\r\n%s = %s\r\n\n",                     \
                "convert_key", mes, "convert-table filename", conv_fnm)

#define errdsp2(mes, termnm)                                            \
        fprintf(stderr, "%s: %s.\r\n%s = %s\r\n\n",                     \
                "convert_key", mes, "termname", termnm)

 /* Warningメッセージを表示 */
#define warn1(conv_fnm)                                                 \
        fprintf(stderr, "%s: %s.\r\n%s.\r\n%s = %s\r\n\n",              \
                "convert_key",                                          \
                "Warning! Convert-table file is not found",             \
                "Key conversion will not be performed",                 \
                "convert-table filename", conv_fnm)

#define warn2(nm, conv_fnm)                                             \
        fprintf(stderr, "%s: %s \"%s\" %s.\r\n%s.\r\n%s = %s\r\n\n",    \
                "convert_key",                                          \
                "Warning! Entry name", nm, "is duplicated",             \
                "The previous one was ignored",                         \
                "convert-table filename", conv_fnm)

#define warn3(nm, conv_fnm)                                             \
        fprintf(stderr, "%s: %s \"%s\" %s.\r\n%s = %s\r\n\n",           \
                "convert_key",                                          \
                "Warning! Illegal entry name", nm, "was ignored",       \
                "convert-table filename", conv_fnm)

#define warn4(entry, conv_fnm, code) {                                  \
        fprintf(stderr, "%s: %s \"%s\" %s,\r\n%s ",                     \
                "convert_key", "Warning! Entry name", entry,            \
                "isn't described in TERMCAP/INFO",                      \
                "so the corresponding key can't generate");             \
        printcode(stderr, code);                                        \
        fprintf(stderr, ".\r\n%s = %s\r\n\n",                           \
                "convert-table filename", conv_fnm);                    \
}

 /** コードcを目に見える形で印字 */
static void
printcode (f, c)
     FILE *f;
     int c;
{
  fprintf (f, (' ' <= c && c < '\177') ? "'%c'" : ((c & ~0xff) ? "0x%X" : "'\\%03o'"), c);
}

/*
getterm_fkeydata(pterp)
char    **pterp;
{廃止}
*/

 /**    8・10・16進コード用のキャラクタを実際のコードに直す。入力のチェックは
        しない（入力は英数字と仮定している）。*/
static int
charvalue (c)
     char c;
{
  if (isupper (c))
    return (c - 'A' + 10);
  if (islower (c))
    return (c - 'a' + 10);
  return (c - '0');
}


#ifdef TERMCAP
char *cvttbl_entry[] = {
  "k0", "k1", "k2", "k3", "k4", "k5", "k6", "k7", "k8", "k9",
#ifdef SUPPORT_TWODIGIT_FUNCTIONS
  "k;",
  "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "FA",
  "FB", "FC", "FD", "FE", "FF", "FG", "FH", "FI", "FJ", "FK",
  "FL", "FM", "FN", "FO", "FP", "FQ", "FR", "FS", "FT", "FU",
  "FV", "FW", "FX", "FY", "FZ", "Fa", "Fb", "Fc", "Fd", "Fe",
  "Ff", "Fg", "Fh", "Fi", "Fj", "Fk", "Fl", "Fm", "Fn", "Fo",
  "Fp", "Fq", "Fr",
#endif                          /* SUPPORT_TWODIGIT_FUNCTIONS */
  "ku", "kr", "kl", "kd", "kh",
  "up", "ri", "le", "do", "ho",
  "kb",
  "!del",                       /* 暫定 */
};
#endif /* TERMCAP */
#ifdef TERMINFO
char *cvttbl_entry[] = {
  "kf0", "kf1", "kf2", "kf3", "kf4", "kf5", "kf6", "kf7",
  "kf8", "kf9", "kf10",
#ifdef SUPPORT_TWODIGIT_FUNCTIONS
  "kf11", "kf12", "kf13", "kf14", "kf15", "kf16", "kf17",
  "kf18", "kf19", "kf20", "kf21", "kf22", "kf23", "kf24",
  "kf25", "kf26", "kf27", "kf28", "kf29", "kf30", "kf31",
#ifndef uniosu
  "kf32", "kf33", "kf34", "kf35", "kf36", "kf37", "kf38",
  "kf39", "kf40", "kf41", "kf42", "kf43", "kf44", "kf45",
  "kf46", "kf47", "kf48", "kf49", "kf50", "kf51", "kf52",
  "kf53", "kf54", "kf55", "kf56", "kf57", "kf58", "kf59",
  "kf60", "kf61", "kf62", "kf63",
#endif
#endif                          /* SUPPORT_TWODIGIT_FUNCTIONS */
  "kcuu1", "kcuf1", "kcub1", "kcud1", "khome",
  "kbs", "ktbc", "kclr", "kctab", "kdch1", "kdl1",
  "krmir", "kel", "ked", "kich1", "kil1",
  "kll", "knp", "kpp", "kind", "kri", "khts",
};
#endif /* TERMINFO */

#define ENTRY_CNT (sizeof(cvttbl_entry) / sizeof(*cvttbl_entry))
#define CVTTBLSIZ ENTRY_CNT

char *keydef_key[ENTRY_CNT];    /* terminfo/termcapの各データへのポインタ */
char tdataarea[AREASIZE];       /* データ収納のための専用エリア */

struct CONVCODE tbl[CVTTBLSIZ];
int cnv_tbl_cnt;                /* convert table count */
 /* これらは元は convert_key.c中のstatic変数だった */

 /** コンバート表のエントリ部分をとる。許される最大長を超えたら0を返す */
static int
get_entrynm (buf)
     char *buf;
{
  int i, c;

  for (i = 0; i <= ENTRY_LEN; i++)
    {
      c = getc (convert_hyo);
      if (is_eofsp (c))
        {
          ungetc (c, convert_hyo);
          *buf = '\0';
          return (1);
        }
      else
        *buf++ = c;
    }
  return (0);
}

 /** 空白（改行文字を除く）をとばして次の文字を返す */
static int
blank_ign_getc (f)
     FILE *f;
{
  int c;

  while (c = getc (f), is_space (c) && c != '\n');
  return (c);
}

 /** 行末（又はEOF）まで飛ばす。その間に非空白文字があったら非0を返す。*/
static int
linepass (f)
     FILE *f;
{
  int c, flg = 0;

  while (c = getc (f), !is_eofnl (c))
    flg = (flg || !is_space (c));
  return (flg);
}

 /** コントロールコード形式のコード表記一文字分をとる。エラーがあれば返値≠0*/
static int
get_ctrl (cptr)
     int *cptr;
{
  int c;

  if (!(' ' <= (c = getc (convert_hyo)) && c < '\177'))
    return (1);
  *cptr = (c == '?' ? '\177' : c & 0x1f);
  return (0);
}

 /** バックスラッシュ形式のコード表記一文字分をとる。エラーがあれば非0を返す*/
static int
get_bcksla (cptr)
     int *cptr;
{
  int c, code = 0, digflg = 0;

  switch (c = getc (convert_hyo))
    {
    case 'n':
      *cptr = '\n';
      return (0);
    case 't':
      *cptr = '\t';
      return (0);
    case 'b':
      *cptr = '\b';
      return (0);
    case 'r':
      *cptr = '\r';
      return (0);
    case 'f':
      *cptr = '\f';
      return (0);
    case 'e':
    case 'E':
      *cptr = ESCCHR;
      return (0);
    case 'o':
      while (c = getc (convert_hyo), is_octal (c))
        {
          code <<= 3;
          code += charvalue (c);
          digflg = 1;
        }
      ungetc (c, convert_hyo);
      *cptr = code;
      return (digflg == 0);
    case 'd':
      while (c = getc (convert_hyo), is_digit (c))
        {
          code *= 10;
          code += charvalue (c);
          digflg = 1;
        }
      ungetc (c, convert_hyo);
      *cptr = code;
      return (digflg == 0);
    case 'x':
      while (c = getc (convert_hyo), is_xdigit (c))
        {
          code <<= 4;
          code += charvalue (c);
          digflg = 1;
        }
      ungetc (c, convert_hyo);
      *cptr = code;
      return (digflg == 0);
    default:
      if (is_octal (c))
        {
          code = charvalue (c);
          while (c = getc (convert_hyo), is_octal (c))
            {
              code <<= 3;
              code += charvalue (c);
            }
          ungetc (c, convert_hyo);
          *cptr = code;
          return (0);
        }
      else
        {
          *cptr = c;
          return (0);
        }
    }
}

 /**    コード表記をとって、cptrの指し先にそのコードを入れる。
        文法エラー発見時は非0を返す。*/
static int
get_code (cptr)
     int *cptr;
{
  int c;

  switch (c = getc (convert_hyo))
    {
    case '\\':
      return (get_bcksla (cptr));
    case '^':
      return (get_ctrl (cptr));
    default:
      *cptr = c;
      return (0);
    }
}

#ifdef TERMCAP
 /**    tgetstrと同等の関数。但し、エントリ名「!del」（DELキーの変換をするため
        暫定的に追加してあるエントリ）に対しては、定文字列"\177"を返す。*/
static char *
my_tgetstr (name, ptr)
     char *name, **ptr;
{
  extern char *tgetstr ();
  static char *del = "\177";

  if (strcmp (name, "!del") == 0)
    return (del);
  return (tgetstr (name, ptr));
}
#endif

#ifdef TERMINFO
 /**    pp1の指しているエリアにp2の内容を入れた後、pp1のポインタを進めておく*/
static char *
stradd (pp1, p2)
     char **pp1, *p2;
{
  if (p2 != NULL)
    {
      strcpy (*pp1, p2);
      (*pp1) += strlen (p2 = *pp1) + 1;
    }
  return (p2);
}
#endif

 /** termcap/infoから読んだキーのデータを専用エリアにコピー。*/
static void
convert_getstrs (flg)
     int flg;                   /* verboseモードで起こすかのフラグ（今の所それだけだが、将来
                                   拡張があるかもしれないので、1か0で与えて下さい。このファイル
                                   中の、名前がconvert_で始まる他の関数でも同じです） */
{
  char *tdataptr;
  int i;

  tdataptr = tdataarea;

#ifdef TERMCAP
  for (i = 0; i < ENTRY_CNT; i++)
    keydef_key[i] = my_tgetstr (cvttbl_entry[i], &tdataptr);
  /* keydef_key[i] の値は、NULL又はtdataareaの範囲内とは限らない。
     "\177"などの特殊データを指すこともある。 */

  if (flg)
    fprintf (stderr, "convert_key: finished getting TERMCAP.\r\n");
#endif

#ifdef TERMINFO
  i = 0;
  keydef_key[i++] = stradd (&tdataptr, key_f0);
  keydef_key[i++] = stradd (&tdataptr, key_f1);
  keydef_key[i++] = stradd (&tdataptr, key_f2);
  keydef_key[i++] = stradd (&tdataptr, key_f3);
  keydef_key[i++] = stradd (&tdataptr, key_f4);
  keydef_key[i++] = stradd (&tdataptr, key_f5);
  keydef_key[i++] = stradd (&tdataptr, key_f6);
  keydef_key[i++] = stradd (&tdataptr, key_f7);
  keydef_key[i++] = stradd (&tdataptr, key_f8);
  keydef_key[i++] = stradd (&tdataptr, key_f9);
  keydef_key[i++] = stradd (&tdataptr, key_f10);
#ifdef SUPPORT_TWODIGIT_FUNCTIONS
  keydef_key[i++] = stradd (&tdataptr, key_f11);
  keydef_key[i++] = stradd (&tdataptr, key_f12);
  keydef_key[i++] = stradd (&tdataptr, key_f13);
  keydef_key[i++] = stradd (&tdataptr, key_f14);
  keydef_key[i++] = stradd (&tdataptr, key_f15);
  keydef_key[i++] = stradd (&tdataptr, key_f16);
  keydef_key[i++] = stradd (&tdataptr, key_f17);
  keydef_key[i++] = stradd (&tdataptr, key_f18);
  keydef_key[i++] = stradd (&tdataptr, key_f19);
  keydef_key[i++] = stradd (&tdataptr, key_f20);
  keydef_key[i++] = stradd (&tdataptr, key_f21);
  keydef_key[i++] = stradd (&tdataptr, key_f22);
  keydef_key[i++] = stradd (&tdataptr, key_f23);
  keydef_key[i++] = stradd (&tdataptr, key_f24);
  keydef_key[i++] = stradd (&tdataptr, key_f25);
  keydef_key[i++] = stradd (&tdataptr, key_f26);
  keydef_key[i++] = stradd (&tdataptr, key_f27);
  keydef_key[i++] = stradd (&tdataptr, key_f28);
  keydef_key[i++] = stradd (&tdataptr, key_f29);
  keydef_key[i++] = stradd (&tdataptr, key_f30);
  keydef_key[i++] = stradd (&tdataptr, key_f31);
# ifndef uniosu
  keydef_key[i++] = stradd (&tdataptr, key_f32);
  keydef_key[i++] = stradd (&tdataptr, key_f33);
  keydef_key[i++] = stradd (&tdataptr, key_f34);
  keydef_key[i++] = stradd (&tdataptr, key_f35);
  keydef_key[i++] = stradd (&tdataptr, key_f36);
  keydef_key[i++] = stradd (&tdataptr, key_f37);
  keydef_key[i++] = stradd (&tdataptr, key_f38);
  keydef_key[i++] = stradd (&tdataptr, key_f39);
  keydef_key[i++] = stradd (&tdataptr, key_f40);
  keydef_key[i++] = stradd (&tdataptr, key_f41);
  keydef_key[i++] = stradd (&tdataptr, key_f42);
  keydef_key[i++] = stradd (&tdataptr, key_f43);
  keydef_key[i++] = stradd (&tdataptr, key_f44);
  keydef_key[i++] = stradd (&tdataptr, key_f45);
  keydef_key[i++] = stradd (&tdataptr, key_f46);
  keydef_key[i++] = stradd (&tdataptr, key_f47);
  keydef_key[i++] = stradd (&tdataptr, key_f48);
  keydef_key[i++] = stradd (&tdataptr, key_f49);
  keydef_key[i++] = stradd (&tdataptr, key_f50);
  keydef_key[i++] = stradd (&tdataptr, key_f51);
  keydef_key[i++] = stradd (&tdataptr, key_f52);
  keydef_key[i++] = stradd (&tdataptr, key_f53);
  keydef_key[i++] = stradd (&tdataptr, key_f54);
  keydef_key[i++] = stradd (&tdataptr, key_f55);
  keydef_key[i++] = stradd (&tdataptr, key_f56);
  keydef_key[i++] = stradd (&tdataptr, key_f57);
  keydef_key[i++] = stradd (&tdataptr, key_f58);
  keydef_key[i++] = stradd (&tdataptr, key_f59);
  keydef_key[i++] = stradd (&tdataptr, key_f60);
  keydef_key[i++] = stradd (&tdataptr, key_f61);
  keydef_key[i++] = stradd (&tdataptr, key_f62);
  keydef_key[i++] = stradd (&tdataptr, key_f63);
# endif /* !uniosu */
#endif /* SUPPORT_TWODIGIT_FUNCTIONS */
/* 以下順に
        "kcuu1", "kcuf1", "kcub1", "kcud1", "khome",
        "kbs", "ktbc", "kclr", "kctab", "kdch1", "kdl1",
        "krmir", "kel", "ked", "kich1", "kil1",
        "kll", "knp", "kpp", "kind", "kri", "khts",
*/
  keydef_key[i++] = stradd (&tdataptr, key_up);
  keydef_key[i++] = stradd (&tdataptr, key_right);
  keydef_key[i++] = stradd (&tdataptr, key_left);
  keydef_key[i++] = stradd (&tdataptr, key_down);
  keydef_key[i++] = stradd (&tdataptr, key_home);
  keydef_key[i++] = stradd (&tdataptr, key_backspace);
  keydef_key[i++] = stradd (&tdataptr, key_catab);
  keydef_key[i++] = stradd (&tdataptr, key_clear);
  keydef_key[i++] = stradd (&tdataptr, key_ctab);
  keydef_key[i++] = stradd (&tdataptr, key_dc);
  keydef_key[i++] = stradd (&tdataptr, key_dl);
  keydef_key[i++] = stradd (&tdataptr, key_eic);
  keydef_key[i++] = stradd (&tdataptr, key_eol);
  keydef_key[i++] = stradd (&tdataptr, key_eos);
  keydef_key[i++] = stradd (&tdataptr, key_ic);
  keydef_key[i++] = stradd (&tdataptr, key_il);
  keydef_key[i++] = stradd (&tdataptr, key_ll);
  keydef_key[i++] = stradd (&tdataptr, key_npage);
  keydef_key[i++] = stradd (&tdataptr, key_ppage);
  keydef_key[i++] = stradd (&tdataptr, key_sf);
  keydef_key[i++] = stradd (&tdataptr, key_sr);
  keydef_key[i++] = stradd (&tdataptr, key_stab);

  if (flg)
    fprintf (stderr, "convert_key: finished getting TERMINFO.\r\n");
#endif
}

 /**    TERMINFO の場合は setupterm()、TERMCAPの場合は tgetent()の処理を行った
        後、キーが発生するシーケンスのデータを専用のエリアにコピーする。
        termcap/infoのデータベースがオープンできなかったら非0を返す。*/
#ifdef TERMCAP
int
convert_getterm (termname, flg)
     char *termname;
     int flg;
{
  extern int tgetent ();
  char tcaparea[AREASIZE];

  if (flg)
    fprintf (stderr, "convert_key: using TERMCAP entry %s...\r\n", termname);
  if (tgetent (tcaparea, termname) <= 0)
    {
      errdsp2 ("Can't get termcap entry", termname);
      return (1);
    }
  convert_getstrs (flg);
  return (0);
}
#endif
#ifdef TERMINFO
int
convert_getterm (termname, flg)
     char *termname;
     int flg;
{
  int fd, rsl;

  if (flg)
    fprintf (stderr, "convert_key: using TERMINFO entry %s...\r\n", termname);
  fd = open ("/dev/null", O_WRONLY, 0);
  setupterm (termname, fd, &rsl);
  close (fd);
  if (rsl != 1)
    {
      errdsp2 ("Can't get terminfo entry", termname);
      /* このときresetterm()は必要か？ */
      return (1);
    }
  convert_getstrs (flg);
  resetterm ();
  return (0);
}
#endif

 /**    termcap/infoから取ったキーのデータが既に専用エリアに収まっていると
        して、それを使ってconvert_key処理の初期化をする。返値は正常終了時0、
        表読み込み時にWarningが発生した時1、Errorを検出したとき-1。*/
int
convert_key_setup (filename, flg)
     char *filename;
     int flg;
{
  register int i;
  int c, d, cnt = 0, warn_occur = 0, entry_found;
  char name[ENTRY_LEN + 1];

  int keydef_code[ENTRY_CNT];   /* convert code */

  if (CHANGE_MAX < div_up (ENTRY_CNT, BITSIZ))
    {
      fprintf (stderr, "%s%s%d%s", "Sorry, please set CHANGE_MAX(in file ", "conv/cvt_head.h) larger than ", div_up (ENTRY_CNT, BITSIZ) - 1, ",\r\nand recompile.\r\n");
      return (-1);
    }

  for (i = 0; i < ENTRY_CNT; i++)
    keydef_code[i] = -1;
  /* codeが -1 のままなら、convert_key表にエントリがないことを示す。 */

  if (NULL != filename)
    convert_filename = filename;
  else
    {
      convert_filename = default_cvt_fn;
#ifdef  WNNDEFAULT
      strcpy (convert_filename, LIBDIR);        /* /usr/local/lib/wnn */
      strcat (convert_filename, CONVERT_FILENAME);      /* /cvt_key_tbl */
#else
      strcpy (convert_filename, CONVERT_FILENAME);
#endif /* WNNDEFAULT */
    }
  if (flg)
    fprintf (stderr, "convert_key: using convert_key table %s...\r\n", convert_filename);
  if (NULL == (convert_hyo = fopen (convert_filename, "r")))
    {
#ifdef NOFILE_NOT_ABORT
      warn1 (convert_filename);
      cnv_tbl_cnt = 0;
      return (1);
#else
      errdsp1 ("Convert-table file is not found", convert_filename);
      return (-1);
#endif
    }

  /* 表からの読みこみ */
  while (EOF != (d = getc (convert_hyo)))
    {
      if (d == '#' || d == ';')
        {
          /* '#'又は';'で始まる行は注釈として読み飛ばす。 */
          linepass (convert_hyo);
          continue;
        }
      ungetc (d, convert_hyo);

      d = blank_ign_getc (convert_hyo);
      if (is_eofnl (d))
        continue;               /* 空行 */
      else
        ungetc (d, convert_hyo);
      if (!get_entrynm (name))
        {
          errdsp1 ("Entry name too long", convert_filename);
          fclose (convert_hyo);
          return (-1);
        }                       /* エントリ名を読み込んだ。 */

      d = blank_ign_getc (convert_hyo);
      if (is_eofnl (d))
        {
          errdsp1 ("Convert code missing", convert_filename);
          fclose (convert_hyo);
          return (-1);
        }
      else
        ungetc (d, convert_hyo);
      if (get_code (&c) != 0 || linepass (convert_hyo) != 0)
        {
          errdsp1 ("Convert code illegal", convert_filename);
          fclose (convert_hyo);
          return (-1);
        }                       /* 変換コードを読み込んだ。 */
#ifdef DEBUG
      printf ("%s %d\n", name, c);
#endif
      for (entry_found = i = 0; i < ENTRY_CNT; i++)
        {
          if (!strcmp (cvttbl_entry[i], name))
            {
              if (keydef_code[i] != -1)
                {
                  warn2 (name, convert_filename);
                  warn_occur = 1;
                }
              keydef_code[i] = c;
              entry_found = 1;
              break;
            }
        }
      if (!entry_found)
        {
          warn3 (name, convert_filename);
          warn_occur = 1;
        }
    }

  /* 読みこんだデータをテーブルにセット */
  for (i = 0; i < ENTRY_CNT; i++)
    {
      if (keydef_key[i] != NULL)
        {
          if (keydef_code[i] != -1 && ((int) strlen (keydef_key[i]) > 1 || *keydef_key[i] != keydef_code[i]))
            {
              tbl[cnt].fromkey = keydef_key[i];
              tbl[cnt++].tokey = keydef_code[i];
            }
        }
      else if (keydef_code[i] != -1)
        {
          warn4 (cvttbl_entry[i], convert_filename, keydef_code[i]);
          warn_occur = 1;
        }
    }

  cnv_tbl_cnt = cnt;
  fclose (convert_hyo);
  if (flg)
    fprintf (stderr, "convert_key: finished setting up.\r\n");
  return (warn_occur);
}

 /**    convert_key処理初期化ルーチン。TERMINFO の場合は setupterm()、TERMCAPの
        場合は tgetent()をそれぞれコールした後に呼ぶ。返値はconvert_key_setup
        参照。*/
int
convert_key_init (filename, flg)
     char *filename;
     int flg;
{
  convert_getstrs (flg);
  return (convert_key_setup (filename, flg));
}

 /**    TERMINFO の場合は setupterm()、TERMCAPの場合は tgetent()まで含めて実行
        convert_key処理初期化ルーチン。返す値はconvert_key_setup参照。但し、
        termcap/infoの読み込みに失敗した場合は、-2が返る。*/
int
convert_getterm_init (termname, filename, flg)
     char *termname, *filename;
     int flg;
{
  if (convert_getterm (termname, flg) != 0)
    return (-2);
  return (convert_key_setup (filename, flg));
}
