/*
 *  $Id: jllib.h,v 1.6 2008/10/26 10:27:53 aonoto Exp $
 */

/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright FreeWnn Project 1999, 2000, 2008
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

/*
 *       Nihongo Henkan  Library Header File
 */

#ifndef WNN_JLLIB_H
#define WNN_JLLIB_H

#include "jslib.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
   ユーザープログラムは、直接この構造体の内容をアクセスする必要は無いはず。
   ライブラリ関数を用いること。
*/

#define WNN_YOMI_SIZE 10

typedef struct wnn_jl_bun WNN_BUN;

struct wnn_jl_bun
{
  int jirilen;                  /* 候補文節の自立語 */
  int dic_no;
  int entry;
  int kangovect;                /* 接続ベクトルテーブルへのポインタ */
  int hinsi;                    /* 品詞 */
  int hindo:16;                 /* 頻度(仮想値) */
  int ref_cnt:4;                /* 文節リストに使われている */
  /* BUG FIX signed --> unsigned */
  unsigned int ima:1;           /* 今使ったよビット */
  unsigned int hindo_updated:1; /* この文節の頻度は更新されている */
  unsigned int nobi_top:1;      /* 伸ばし縮みを行ったかを示す。 */
  /* egg みたいに伸ばし縮みを繰り返し行う時に、途中に現れる候補の今
     使ったよ ビットを 落としてはならない。 */
  unsigned int dai_top:1;       /* 大文節の先頭 */
  unsigned int dai_end:1;       /* 大文節のsaigo *//* 次候補 nomi */
  unsigned int from_zenkouho:2; /*次候補から選択された文節かどうか */
  unsigned int bug:1;           /* BUG FIX とりあえず */
  /* BUG FIX signed --> unsigned */
  int hyoka;
  int daihyoka;
  short yomilen;
  short kanjilen;
  WNN_BUN *down;                /* 今使ったよビットを落とす対象の文節 */
  w_char yomi[WNN_YOMI_SIZE];   /* 読み、漢字が入る。入り切らない時には
                                   次につなげる */
  WNN_BUN *next;                /* 次のストラクトへのポインタ */
  WNN_BUN *free_next;           /* 次のストラクトへのポインタ */
};


struct wnn_buf
{
  struct wnn_env *env;          /* 環境 */
  int bun_suu;                  /* 文節数 */
  int zenkouho_suu;             /* 全候補の数 */
  WNN_BUN **bun;                /* 文節へのポインタ */
  WNN_BUN **down_bnst;          /* 今使ったよビットを落とす対象の文節 */

  WNN_BUN **zenkouho;           /* 全候補へのポインタ */
  int *zenkouho_dai;            /* daibunsetsu zenkouho */
  int zenkouho_dai_suu;         /* daibunsetsu zenkouho suu */

  short c_zenkouho;             /* 全候補中、注目している候補 */
  short zenkouho_daip;
  int zenkouho_bun;             /* 全候補を取っている文節 */
  int zenkouho_end_bun;         /* 全候補(dai)を取っている最後の文節 */
  int zenkouho_endvect;         /* 全候補を取った endvect *//* ADD KURI */

  WNN_BUN *free_heap;
  char *heap;                   /* linked list of alloced area */
  int msize_bun;
  int msize_zenkouho;
};


#define WNN_SHO 0
#define WNN_DAI 1

#define WNN_NO_USE    0
#define WNN_USE_MAE   1
#define WNN_USE_ATO   2
#define WNN_USE_ZENGO (WNN_USE_MAE | WNN_USE_ATO)
/* #define WNN_ZENGO_YUUSEN 4 */

#define WNN_UNIQ_KNJ 2
#define WNN_UNIQ 1
#define WNN_NO_UNIQ  0

#define WNN_NO_CREATE 0
#define WNN_CREATE (-1)

#define WNN_DIC_PRIO_DEFAULT 5

#define WNN_YOMI 0
#define WNN_KANJI 1

#define jl_next(buf) jl_set_jikouho((buf), (buf)->c_zenkouho + 1)

#define jl_previous(buf) jl_set_jikouho((buf), (buf)->c_zenkouho - 1)

#define jl_next_dai(buf) jl_set_jikouho_dai((buf), (buf)->c_zenkouho + 1)

#define jl_previous_dai(buf) jl_set_jikouho_dai((buf), (buf)->c_zenkouho - 1)

#define jl_get_kanji(buf, bun_no, bun_no2, area)\
                   wnn_get_area(buf, bun_no, bun_no2, area, WNN_KANJI)

#define jl_get_yomi(buf, bun_no, bun_no2, area)\
                   wnn_get_area(buf, bun_no, bun_no2, area, WNN_YOMI)

#define jl_bun_suu(buf) ((buf)->bun_suu)

#define jl_zenkouho_suu(buf) ((buf)->zenkouho_daip? (buf)->zenkouho_dai_suu:(buf)->zenkouho_suu)

#define jl_zenkouho_bun(buf) ((buf)->zenkouho_bun)

#define jl_c_zenkouho(buf) ((buf)->c_zenkouho)

#define jl_zenkouho_daip(buf) ((buf)->zenkouho_daip)

#define jl_dai_top(buf, k) ((buf)->bun[k]->dai_top)

#define jl_jiri_len(buf, k) ((buf)->bun[k]->jirilen)

#define jl_fuzoku_len(buf, k) \
                (jl_yomi_len((buf), k, k+1) - jl_jiri_len((buf), k))

#define jl_jiri_kanji_len(buf, k) \
                (jl_kanji_len((buf), k, k+1) - jl_fuzoku_len((buf), k))

/* Macros to create library functions from "_e" libraries */

#define jl_env(buf)     ((buf)->env)

#define jl_dic_add(buf,dic_name,hindo_name,rev, prio,rw, hrw, pwd_dic, pwd_hindo, error_handler, message_handler)\
    jl_dic_add_e(jl_env(buf),dic_name,hindo_name,rev,prio,rw, hrw, pwd_dic,\
                      pwd_hindo, error_handler, message_handler)

#define jl_fuzokugo_set(buf, fname)  jl_fuzokugo_set_e(jl_env(buf), fname)

#define jl_fuzokugo_get(buf, fname)  jl_fuzokugo_get_e(jl_env(buf), fname)


#define jl_dic_save(buf,dic_no) jl_dic_save_e(jl_env(buf),dic_no)

#define jl_dic_save_all(buf)    jl_dic_save_all_e(jl_env(buf))

#define jl_dic_list(buf, dip)   jl_dic_list_e(jl_env(buf), dip)

#define jl_word_search(buf,dic_no, yomi, jdp) \
    jl_word_search_e(jl_env(buf),dic_no, yomi, jdp)

#define jl_word_search_by_env(buf, yomi, jdp) \
    jl_word_search_by_env_e(jl_env(buf), yomi, jdp)

#define jl_word_use(buf, dic_no, entry) \
    jl_word_use_e(jl_env(buf), dic_no, entry)


#define jl_param_set(buf, param) jl_param_set_e(jl_env(buf), param)

#define jl_param_get(buf, param) jl_param_get_e(jl_env(buf), param)

#define jl_dic_delete(buf, dic_no) jl_dic_delete_e(jl_env(buf), dic_no)

#define jl_dic_use(buf, dic_no, flag) jl_dic_use_e(jl_env(buf), dic_no, flag)

#define jl_word_add(buf, dic, yomi, kanji, com, hinsi, hindo) \
    jl_word_add_e(jl_env(buf), dic, yomi, kanji, com, hinsi, hindo)

#define jl_word_delete(buf, dic, entry)\
    jl_word_delete_e(jl_env(buf), dic, entry)

#define jl_word_info(buf, dic_no, entry)\
    jl_word_info_e(jl_env(buf), dic_no, entry)

#define jl_inspect(buf, bun_no)\
    jl_word_info(buf, (buf)->bun[bun_no]->dic_no,(buf)->bun[bun_no]->entry)

#define jl_hinsi_number(buf, name) jl_hinsi_number_e(jl_env(buf), name)

#define jl_hinsi_name(buf, no) jl_hinsi_name_e(jl_env(buf), no)

#define jl_word_comment_set(buf, dic, entry, comm) \
    jl_word_comment_set_e((buf)->env, dic, entry, comm)

#define jl_dic_comment_set(buf, dic, comm) \
    jl_dic_comment_set_e((buf)->env, dic, comm)

#define jl_hinsi_list(buf, dic_no, name, area) \
    jl_hinsi_list_e(jl_env(buf), dic_no, name, area)

#define jl_hinsi_dicts(buf, no, area) \
    jl_hinsi_dicts_e(jl_env(buf), no, area)

#ifdef nodef                    /*当面の間、頻度ファイルのコメントはユーザに見せない。 */
#define jl_hindo_comment_set(buf, dic, comm) \
        jl_hindo_comment_set_e((buf)->env, dic, comm)
#endif

#define jl_isconnect(buf)       jl_isconnect_e((buf)->env)

#define jl_env_sticky(buf)      js_env_sticky((buf)->env)
#define jl_env_sticky_e(env)    js_env_sticky(env)
#define jl_env_un_sticky(buf)   js_env_un_sticky((buf)->env)
#define jl_env_un_sticky_e(env) js_env_un_sticky(env)

/* for select server by useing $LANG */
#define jl_open(env_n, server_n, wnnrc_n, error_handler, message_handler, timeout) \
    jl_open_lang(env_n, server_n, NULL, wnnrc_n, error_handler, message_handler, timeout)

#define jl_connect(env_n, server_n, wnnrc_n, error_handler, message_handler, timeout) \
    jl_connect_lang(env_n, server_n, NULL, wnnrc_n, error_handler, message_handler, timeout)
#define jl_get_lang(buf)        js_get_lang((buf)->env)
#define jl_set_lang(buf, lang)  js_set_lang((buf)->env, lang)

/* extern function defines */
/* Note: FRWNN_PARAMS is defined in jslib.h . */

extern struct wnn_buf *jl_open_lang    FRWNN_PARAMS((char *env_n, char *server_n, char *lang, char *wnnrc_n, int (*error_handler) (), int (*message_handler) (), int timeout));
extern struct wnn_env *jl_connect_lang FRWNN_PARAMS((register char *env_n, register char *server_n, register char *lang, register char *wnnrc_n,  int (*error_handler) (), int (*message_handler) (), int timeout));
extern struct wnn_env *jl_env_get      FRWNN_PARAMS((register struct wnn_buf *buf));
extern struct wnn_jdata *jl_word_info_e FRWNN_PARAMS((register struct wnn_env *env, int dic_no, int entry));
extern w_char *jl_hinsi_name_e FRWNN_PARAMS((register struct wnn_env *env, register int no));

extern void jl_close FRWNN_PARAMS((register struct wnn_buf *buf));
extern int jl_dic_add_e FRWNN_PARAMS((register struct wnn_env *env, char *dic_name, char *hindo_name, int rev, int prio, int rw, int hrw, char *pwd_dic, char *pwd_hindo, int (*error_handler) (), int (*message_handler) () ));
extern int jl_dic_comment_set_e FRWNN_PARAMS((register struct wnn_env *env, int dic_no, w_char *comment));
extern int jl_dic_delete_e FRWNN_PARAMS((register struct wnn_env *env, register int dic_no));
extern int jl_dic_list_e   FRWNN_PARAMS((struct wnn_env *env, WNN_DIC_INFO **dicinfo));
extern int jl_dic_save_all_e FRWNN_PARAMS((struct wnn_env *env));
extern int jl_dic_save_e   FRWNN_PARAMS((register struct wnn_env *env, int dic_no));
extern int jl_dic_use_e    FRWNN_PARAMS((struct wnn_env *env, int dic_no, int flag));
extern void jl_disconnect FRWNN_PARAMS((register struct wnn_env *env));
extern void jl_disconnect_if_server_dead FRWNN_PARAMS((register struct wnn_env *env));
extern void jl_env_set FRWNN_PARAMS((register struct wnn_buf *buf, register struct wnn_env *env));
extern int jl_fuzokugo_get_e FRWNN_PARAMS((register struct wnn_env *env, char *fname));
extern int jl_fuzokugo_set_e FRWNN_PARAMS((struct wnn_env *env, char *fname));
extern void jl_get_zenkouho_kanji FRWNN_PARAMS((register struct wnn_buf *buf, int zen_num, w_char *area));
extern int jl_hinsi_dicts_e  FRWNN_PARAMS((register struct wnn_env *env, int no, int **area));
extern int jl_hinsi_list_e   FRWNN_PARAMS((register struct wnn_env *env, int dic_no, w_char *name, w_char ***area));
extern int jl_hinsi_number_e FRWNN_PARAMS((register struct wnn_env *env, w_char *name));
extern int jl_isconnect_e FRWNN_PARAMS((register struct wnn_env *env));
extern int jl_kanji_len FRWNN_PARAMS((struct wnn_buf *buf, register int bun_no, register int bun_no2));
extern int jl_kill FRWNN_PARAMS((struct wnn_buf *buf, register int bun_no, register int bun_no2));
extern int jl_nobi_conv    FRWNN_PARAMS((register struct wnn_buf *buf, register int bun_no, int ichbn_len, register int bun_no2, int use_maep, int ich_shop));
extern int jl_nobi_conv_e2 FRWNN_PARAMS((register struct wnn_buf *buf, struct wnn_env *env, register int bun_no, int ichbn_len, register int bun_no2, int use_maep, int ich_shop));
extern int jl_param_get_e FRWNN_PARAMS((struct wnn_env *env, struct wnn_param *para));
extern int jl_param_set_e FRWNN_PARAMS((register struct wnn_env *env, struct wnn_param *para));
extern int jl_ren_conv FRWNN_PARAMS((register struct wnn_buf *buf, register w_char *yomi, int bun_no, int bun_no2, int use_maep));
extern int jl_set_env_wnnrc  FRWNN_PARAMS((register struct wnn_env *env, char *wnnrc_n, int (*error_handler) (), int (*message_handler) () ));
extern int jl_set_env_wnnrc1 FRWNN_PARAMS((register struct wnn_env *env, char *wnnrc_n, int (*error_handler) (), int (*message_handler) (), int level));
extern int jl_set_jikouho     FRWNN_PARAMS((register struct wnn_buf *buf, register int offset));
extern int jl_set_jikouho_dai FRWNN_PARAMS((register struct wnn_buf *buf, int offset));
extern int jl_tan_conv FRWNN_PARAMS((register struct wnn_buf *buf, w_char *yomi, register int bun_no, register int bun_no2, int use_maep, int ich_shop));
extern int jl_update_hindo FRWNN_PARAMS((register struct wnn_buf *buf, int bun_no, int bun_no2));
extern int jl_word_add_e         FRWNN_PARAMS((struct wnn_env *env, int dic_no, w_char *yomi, w_char *kanji, w_char *comment, int hinsi, int init_hindo));
extern int jl_word_comment_set_e FRWNN_PARAMS((register struct wnn_env *env, int dic_no, int entry, w_char *comment));
extern int jl_word_delete_e      FRWNN_PARAMS((struct wnn_env *env, int dic_no, int entry));
extern int jl_word_search_by_env_e FRWNN_PARAMS((register struct wnn_env *env, w_char *yomi, struct wnn_jdata **jdp));
extern int jl_word_search_e      FRWNN_PARAMS((register struct wnn_env *env, int dic_no, w_char *yomi, struct wnn_jdata **jdp));
extern int jl_word_use_e         FRWNN_PARAMS((register struct wnn_env *env, int dic_no, int entry));
extern int jl_yomi_len FRWNN_PARAMS((struct wnn_buf *buf, register int bun_no, register int bun_no2));
extern int jl_zenkouho     FRWNN_PARAMS((register struct wnn_buf *buf, int bun_no, int use_maep, int uniq_level));
extern int jl_zenkouho_dai FRWNN_PARAMS((register struct wnn_buf *buf, int bun_no, int bun_no2, int use_maep, int uniq_level));
extern int wnn_get_area FRWNN_PARAMS((struct wnn_buf *buf, register int bun_no, register int bun_no2, w_char *area, int kanjip));

extern char *wnn_perror      FRWNN_PARAMS((void));
extern char *wnn_perror_lang FRWNN_PARAMS((char *lang));

#ifdef  CONVERT_by_STROKE
extern void jl_get_zenkouho_yomi FRWNN_PARAMS((register struct wnn_buf *buf, int zen_num, w_char *area));
#endif

#ifdef __cplusplus
}
#endif

#endif  /* WNN_JLLIB_H */
