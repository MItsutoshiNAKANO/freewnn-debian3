/*
 *  $Id: de_header.h,v 1.24 2013/09/02 11:01:39 itisango Exp $
 */

/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright FreeWnn Project
 *                 1999, 2000, 2001, 2002, 2003, 2004, 2010, 2011, 2013
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

#ifndef JSERVER_DE_HEADER_H
#define JSERVER_DE_HEADER_H 1

#ifdef  GLOBAL_VALUE_DEFINE
#define GLOBAL
#define GLOBAL_VAL(v)  = (v)
#else
#define GLOBAL  extern
#define GLOBAL_VAL(v)
#endif /* GLOBAL_VALUE_DEFINE */

#define DAEMON

#if defined(CHINESE)
#define WNN_DAEMON_NAME "cserver"
#else
#  if defined(KOREAN)
#  define WNN_DAEMON_NAME "kserver"
#  else
#  define WNN_DAEMON_NAME "jserver"
#  endif        /* KOREAN */
#endif        /* CHINESE */

#include "jslib.h"
#include "commonhd.h"
#include "jdata.h"
#include "ddefine.h"
#include "wnn_os.h"

#include "fzk.h"
#include "etc.h"
#include "kaiseki.h"

#define FILENAME 128
#define SUCCESS 1
#define FAILURE NULL

#define CL_MAX 32
#define ST_MAX 10
#define MAX_ENV         256
#define MAX_FILES       600
#define MAX_DIC         1200

/* constants for one client */
/*
#define USER_NAME_LEN 20
#define HOST_NAME_LEN 20
*/
/* constants for one env. */
#define FILE_NAME_L     100

/* value of file.localf */
#define LOCAL   1
#define REMOTE  0

/* SERVER OPTIONS */
#define OPT_FORK	0x01
#define OPT_VERBOSE	0x02


/*
        Client
*/
struct cli
{
  char user_name[WNN_ENVNAME_LEN];
  char host_name[WNN_HOSTLEN];
  int env[WNN_MAX_ENV_OF_A_CLIENT];
};

typedef struct cli CLIENT;

/*
        Environment for Conversion
 */
/*      jisho Table for an Environment          */

/* 疑似文節の選択した候補 */

struct GIJI
{                               /* ADD */
  int eisuu;                    /* 半角/全角英数字 */
  int number;                   /* 半角/全角/漢数字 */
  int kigou;                    /* 半角/全角(JIS/ASCII)記号 */
};

struct cnv_env
{
  char env_name[WNN_ENVNAME_LEN];
  int ref_count;
  int sticky;                   /* クライアントがいなくなってもその環境を残す */
  int nbun;                     /* Ｎ(大)文節解析のＮ */
  int nshobun;                  /* 大文節中の小文節の最大数 */
  int hindoval;                 /* 幹語の頻度のパラメータ */
  int lenval;                   /* 小文節長のパラメータ */
  int jirival;                  /* 幹語長のパラメータ */
  int flagval;                  /* 今使ったよbitのパラメータ */
  int jishoval;                 /* 辞書のパラメータ */
  int sbn_val;                  /* 小文節の評価値のパラメータ */
  int dbn_len_val;              /* 大文節長のパラメータ */
  int sbn_cnt_val;              /* 小文節数のパラメータ */

  int suuji_val;                /* 疑似品詞 数字の頻度 */
  int kana_val;                 /* 疑似品詞 カナの頻度 */
  int eisuu_val;                /* 疑似品詞 英数の頻度 */
  int kigou_val;                /* 疑似品詞 記号の頻度 */
  int toji_kakko_val;           /* 疑似品詞 閉括弧の頻度 */
  int fuzokogo_val;             /* 疑似品詞 付属語の頻度 */
  int kaikakko_val;             /* 疑似品詞 開括弧の頻度 */

  struct GIJI giji;             /* ADD */

  int fzk_fid;

  int jishomax;
  int jisho[WNN_MAX_JISHO_OF_AN_ENV];
  int file[WNN_MAX_FILE_OF_AN_ENV];
};


/*
        structure of Dictionary
 */

struct wnn_dic
{
  int body;                     /* fid */
  int hindo;                    /* fid */
  int rw;
  int hindo_rw;
  int enablef;
  int nice;
  int rev;                      /* reverse dict */
};

/*
        structure of File
 */

struct wnn_file
{
  char name[FILE_NAME_L];
  struct wnn_file_uniq f_uniq;
  struct wnn_file_uniq f_uniq_org;
  int localf;
  int file_type;
  int ref_count;
  void *area;
  char passwd[WNN_PASSWD_LEN];
};


/*
        external variables of daemon
*/
GLOBAL char SER_VERSION[] GLOBAL_VAL(PACKAGE_STRING);

GLOBAL CLIENT *client;
GLOBAL int max_client GLOBAL_VAL(CL_MAX);
GLOBAL CLIENT *c_c GLOBAL_VAL(NULL);	/* this means current client */
GLOBAL int cur_client;
GLOBAL int clientp;

/*      env,dic,file    */
GLOBAL struct cnv_env *env[MAX_ENV];
GLOBAL struct cnv_env *c_env;
/* GLOBAL struct cnv_env *s_env[]; */
GLOBAL int max_sticky_env GLOBAL_VAL(ST_MAX);

GLOBAL struct wnn_file *files;
GLOBAL struct wnn_dic dic_table[MAX_DIC];

GLOBAL int wnn_errorno GLOBAL_VAL(0);

/* option flags */
GLOBAL int noisy GLOBAL_VAL(0);
GLOBAL int option_flag GLOBAL_VAL(0);

GLOBAL char jserver_dir[MAXPATHLEN];
GLOBAL char jserverrcfile[MAXPATHLEN];
GLOBAL char *hinsi_file_name GLOBAL_VAL(NULL);
GLOBAL char lang_dir[MAXPATHLEN];

/* atojis.c */
extern w_char *get_giji_knj (int, int, int, w_char *);
extern void giji_hindoup (int);
/* b_index.c */
#ifdef CONVERT_by_STROKE
extern int create_b_index (struct JT *jt);
extern int b_index_add (struct JT *jt, w_char *yomi, int serial);
extern void b_index_delete (struct JT *jt, int serial);
#endif
/* bnsetu_kai.c */
extern int sbn_kai (int, int, int, int, struct SYO_BNSETSU **, int, struct SYO_BNSETSU *);
extern int j_max (struct ICHBNP *, int);
extern int getgiji (int, int, struct SYO_BNSETSU *, struct ICHBNP *, int, int);
extern int is_g_eisuu (w_char);
extern int getgiji_f (int,  struct SYO_BNSETSU *);
extern int check_bar_katakana (int, int);
extern int kan_ckvt (unsigned short, int);
/* daibn_kai.c */
extern int dbn_kai (int, register int, int,
#ifndef NO_FZK
		    w_char *,
#endif /* NO_FZK */
		    int, int, register int, struct BZD **);
extern int get_status (register int, int,
#ifndef NO_FZK
		       w_char *,
#endif /* NO_FZK */
		       register short *);

extern int zentan_able (int, register int
#ifndef NO_FZK
			, w_char *
#endif /* NO_FZK */
			);

#ifndef NO_FZK
extern w_char *rev_fzk (register w_char *, int);
#endif	/* NO_FZK  */
extern struct SYO_BNSETSU *que_reorder (register struct SYO_BNSETSU *, register struct SYO_BNSETSU *);
extern int sbjunjo (register struct SYO_BNSETSU *, register struct SYO_BNSETSU *);
extern int set_daibnsetu (struct BZD **, register struct BZD **, register struct SYO_BNSETSU *, int *, int);
extern int sum_hyouka (register struct SYO_BNSETSU *);
extern int ave_hyouka (register struct SYO_BNSETSU *);
extern int cmp_hyouka (register struct SYO_BNSETSU *, register struct SYO_BNSETSU *);

/* de.c */
extern void del_client (void);
extern void daemon_fin (void);
extern char *gets_cur (char*, size_t);
extern w_char *getws_cur (w_char*, size_t);
extern int get2_cur (void);
extern int get4_cur (void);
extern int getc_cur (void);
extern void puts_cur (char*);
extern void puts_n_cur (char*, int);
extern void putws_cur (w_char*);
extern void putnws_cur (w_char*, int);
extern void put2_cur (int);
extern void put4_cur (int);
extern void putc_cur (int);
extern void putc_purge (void);
extern void js_who (void);
extern void js_kill (void);
/* dispatch.c */
extern void do_command ();
extern char *get_file_name (char*, size_t);
extern char *expand_file_name (char*, size_t);
extern void error_ret ();
/* do_dic_env.c */
extern void js_dic_add ();
extern int add_dic_to_env (int, int);
extern int chk_dic (int, int);
extern int chk_hindo (int, int);
extern int get_new_dic ();
extern int find_dic_in_env (int, int);
extern void js_dic_delete ();
extern void del_all_dic_in_env (int);
extern void js_dic_list_all ();
extern void js_dic_list ();
extern void js_dic_use ();
extern void js_fuzokugo_set ();
extern void js_fuzokugo_get ();
extern void used_dic_fuzoku_delete (int, int);
/* do_dic_no.c */
extern void dic_init ();
extern void js_dic_info ();
extern void put_dic_info (int);
extern void js_word_add ();
extern void js_word_delete ();
extern void js_word_search_by_env ();
extern void js_word_search ();
extern void js_word_comment_set ();
extern void js_word_info ();
extern void js_hinsi_name ();
extern void js_hinsi_number ();
extern void js_hinsi_list ();
extern void js_hinsi_dicts ();
extern void js_hinsi_table_set ();
/* do_env.c */
extern void js_open (void);
extern void js_close (void);
extern void env_init (void);
extern void js_connect (void);
extern void js_env_sticky (void);
extern void js_env_un_sticky (void);
extern void js_env_exist (void);
extern void disconnect_all_env_of_client (void);
extern void js_disconnect (void);
extern void js_env_list (void);
extern void js_param_set (void);
extern void js_param_get (void);
extern int envhandle (void);
/* do_filecom.c */
extern int file_init (void);
extern void js_mkdir (void);
extern void js_access (void);
extern void js_file_list_all (void);
extern void js_file_list (void);
extern void js_file_info (void);
extern void js_file_stat (void);
extern int file_stat (char *);
extern void js_file_discard (void);
extern void del_all_file_in_env (int);
extern void js_hindo_file_create (void);
extern void js_hindo_file_create_client (void);
extern void js_dic_file_create (void);
extern void js_file_comment_set (void);
extern void js_file_loaded_local (void);
extern void js_file_loaded (void);
extern int file_loaded (char *);
extern void js_file_send (void);
extern void js_file_read (void);
extern void js_file_write (void);
extern void js_file_receive (void);
extern int get_new_fid (void);
extern int find_fid_in_env (int, int);
extern void js_file_remove (void);
extern void js_file_password_set (void);
/* do_henkan.c */
extern void ret_dai (struct DSD_DBN *, int);
extern void ret_sho (register struct DSD_SBN *, register int cnt);
#ifdef CONVERT_by_STROKE
extern void ret_B (int, int, register int, int);
extern void ret_daiB (int, int, register int, int);
#endif
#ifdef DEBUG
extern void print_dlist (struct DSD_DBN *, int);
extern void print_dlist1 (struct DSD_SBN *, int);
#endif
/* do_henkan1.c */
extern void do_kanren (void);
extern void do_kantan_dai (void);
extern void do_kantan_sho (void);
extern void do_kanzen_dai (void);
extern void do_kanzen_sho (void);
extern void Get_knj (register struct jdata *, int, w_char *, w_char *, int);
extern void get_knj_com (register struct jdata *, int, w_char *, w_char *);
/* do_hinsi_s.c */
extern void js_hindo_set ();
/* error.c */
#if 0				/* not used for now */
extern void error_exit1 ();
extern void error_exit ();
#endif
extern void log_err (const char *, ...);
extern void log_debug (const char *, ...);
extern RETSIGTYPE signal_hand ();
extern RETSIGTYPE terminate_hand ();
/* error1() - For compatiblity */
#define error1 log_debug
extern void out ();
#ifdef DEBUG
#ifdef  putwchar
#undef  putwchar
#endif
extern void putwchar ();
extern void wsputs ();
#endif
/* fzk.c */
extern struct FT *fzk_read (FILE *fp);
extern struct FT *fzk_ld (FILE *fp);
extern void fzk_discard (struct FT *fzk_tbl);
extern int fzk_kai (w_char *, w_char *,	int, int, struct ICHBNP **);
extern int fzk_ckvt (int);
/* get_kaiarea.c */
extern int get_kaiseki_area (size_t);
extern void init_work_areas (void);
/* hinsi_list.c */
extern void make_hinsi_list (struct JT *);
extern int hinsi_table_set (struct JT *, w_char *);
/* initjserv.c */
extern int read_default (void);
extern int read_default_files (void);
/* jbiki.c */
extern int jishobiki (w_char *, struct jdata **);
extern int binary1 (register struct uind1 *, register w_char *, register int end, struct JT *);
extern int binary (register struct uind1 *, register w_char *, int, struct JT *);
extern int Strcmpud (struct uind1 *ui1p, w_char *yomi, struct JT *);
extern int Substrud (struct uind1 *, w_char *, int, struct JT *);
extern int Substrstud (w_char *, struct uind1 *, struct JT *);
extern int Substrudud (register struct uind1 *, register struct uind1 *, register struct JT *);
extern int word_search (int, w_char *, struct jdata **);
extern int rd_biki (struct JT *, w_char *, register int);
extern int rd_binary1 (register struct rind1 *, register w_char *, register int, struct rind2 *, UCHAR *, int);
extern int rd_binary (register struct rind1 *, register w_char *, register int end, struct rind2 *, UCHAR *, int which);
extern int rd_binary_same (register struct rind1 *, register w_char *, register int, struct rind2 *, UCHAR *kanji, int);
#ifdef CONVERT_by_STROKE
/* jbiki_b.c */
extern int is_bwnn_rev_dict ();
extern int jishobiki_b (int, int);
#endif
/* jikouho.c */
extern int jkt_sbn_one (int, int, struct JKT_SBN **, int, struct JKT_SBN *, struct ICHBNP *, register int);
#ifndef NO_KATA
extern struct JKT_SBN *get_kata_kouho (struct JKT_SBN **, int, int, int, int *, int *, int);
#endif
extern struct JKT_SBN *get_hira_kouho (struct JKT_SBN **, int, int);
extern int jkt_get_syo (int, int, int,
#ifndef NO_FZK
			w_char *,
#endif /* NO_FZK */
			int, int, struct DSD_SBN **);
extern int get_zen_giji (struct SYO_BNSETSU *, struct JKT_SBN **);

/* jikouho_d.c */
extern int jkt_get_dai (int, int, int,
#ifndef NO_FZK
			w_char *,
#endif /* NO_FZK */
			int, int, int, struct DSD_DBN **);
extern int get_jkt_status (int, int,
#ifndef NO_FZK
			   w_char *,
#endif /* NO_FZK */
			   short *);
extern struct DSD_DBN *get_dsd_dbn (register int);
extern struct DSD_SBN *get_dsd_sbn (register int);
extern int dbn_set (struct DSD_DBN **, register struct JKT_DBN *);
extern int cnt_sone (register struct JKT_SONE *);

/* jishoop.c */
extern int word_add  (int, int, w_char *, w_char *, int, int, w_char *);
extern int word_comment_set (int, int, int, w_char *);
extern int word_delete (int, int, int);
extern void kanjiadd (struct JT *, w_char *, w_char *,  w_char *);
extern int dic_in_envp (int, int);
extern int hindo_file_size_justify (struct wnn_file *, struct wnn_file *);
/* jishosub.c */
extern int inspect (int, int, w_char *, struct jdata *);
extern int get_yomi_from_serial (int, int, w_char *);
/* jmt0.c */
extern void init_jmt (void);
extern int jmt_set (int);
/* mknode0.c */
extern int init_bzd (void);
extern int init_ichbnp (void);
extern void clr_node (register struct BZD *);
extern void freebzd (register struct BZD *);
extern void freeibsp (struct ICHBNP *);
extern struct BZD *getbzdsp (void);
extern struct ICHBNP *getibsp (void);
extern struct fzkkouho *getfzkoh_body (register struct ICHBNP *, int);
extern struct fzkkouho *getfzkoh1_body (register struct ICHBNP *, register int);
extern int mknode (struct BZD *, int, int,
#ifndef NO_FZK
		   w_char *,
#endif /* NO_FZK */
		   int, int);
extern void freebzd (register struct BZD *);

/* mknode1.c */
extern int init_sbn ();
extern int get_area (register int, register int, struct free_list **);
extern void free_area (register struct free_list *);
extern void freesbn (register struct SYO_BNSETSU *);
extern void clr_sbn_node (register struct SYO_BNSETSU *);
extern struct SYO_BNSETSU *getsbnsp ();
/* mknode2.c */
extern int init_jktdbn (void);
extern int init_jktsbn (void);
extern int init_jktsone (void);
extern void freejktdbn (register struct JKT_DBN *);
extern void freejktsbn (struct JKT_SBN *);
extern struct JKT_DBN *getjktdbn (void);
extern struct JKT_SBN *getjktsbn (void);
extern struct JKT_SONE *getjktsone (void);
/* rd_jishoop.c */
extern int rd_word_add1 (struct JT *, w_char *, int, w_char *, w_char *);
extern int rd_word_delete1 (struct JT *, struct HJT *, int);
extern int inspect_rd (int, int, w_char *, struct jdata *);
/* readfile.c */
extern int read_file (struct wnn_file *);
extern int ud_realloc_hontai (struct JT *);
extern int ud_realloc_kanji (struct JT *);
extern int ud_realloc_serial (struct JT *);
extern int ud_realloc_table (struct JT *);
extern int rd_realloc_ri1 (struct JT *, int);
extern int hindo_file_realloc (struct HJT *);
#ifdef CONVERT_by_STROKE
extern int rd_realloc_bind (struct JT *);
#endif
extern int must_write_file (struct wnn_file *, struct wnn_file_uniq *);
extern void clear_dirty_bit (struct wnn_file *);
extern int rcv_file (struct wnn_file *, int);
extern int write_file (struct wnn_file *, char *);
extern int discardfile (struct wnn_file *);
extern int create_hindo_file1 (struct wnn_file *, char *, w_char *, char *);
extern int match_dic_and_hindo_p (struct wnn_file *, struct wnn_file *);
extern int file_comment_set (struct wnn_file *, w_char *);
extern int file_password_set (struct wnn_file *, int, char *, char *);
/* renbn_kai.c */
#ifndef NO_FZK         
extern int renbn_kai (int, int, int, w_char*, int, int, int, int, int, struct DSD_DBN **);
#else /* NO_FZK */
extern int renbn_kai (int, int, int, int, int, int, int, int, struct DSD_DBN **);
#endif /* NO_FZK */    
#ifndef NO_FZK
extern int tan_dai (int, int, int, w_char *, int, int, int, struct DSD_DBN **);
#else /* NO_FZK */
extern int tan_dai (int, int, int, int, int, int, struct DSD_DBN **);
#endif /* NO_FZK */             
#ifndef NO_FZK   
extern int tan_syo (int, int, int, w_char*, int, int, struct DSD_SBN **);
#else /* NO_FZK */
extern int tan_syo (int, int, int, int, int, struct DSD_SBN **);
#endif /* NO_FZK */
/* sisheng.c */
extern int get_sisheng (w_char *, char *, w_char *);
extern w_char *biki_sisheng (w_char *, char *, w_char *);
extern unsigned int diff_sisheng (int, int);
/* snd_rcv.c */
extern int fopen_read_cur (char *);
extern int fopen_write_cur (char *);
extern int fread_cur (char *, register int, register int);
extern int xgetc_cur ();
extern void xungetc_cur (int);
extern void fwrite_cur (unsigned char *, int, int);
extern void xputc_cur (unsigned char);
extern void fclose_cur ();
/* w_string.c */
extern void Sreverse (register w_char *, register w_char *);
extern int Sstrcpy (register w_char *, register unsigned char *);
extern int Strcmp (register w_char *, register w_char *);
extern int Substr (register w_char *, register w_char *);
extern int Strncmp (register w_char *w1, register w_char *, register int n);
extern w_char *Strcpy (register w_char *, register w_char *);
extern w_char *Strncpy (register w_char *w1, register w_char *, register int n);
extern int Strlen (register w_char *w);

/* bdic.c */
extern void check_backup (char*);
extern void delete_tmp_file (char*);
extern char *make_backup_file (char*);
extern char *make_tmp_file (char*, int, FILE**);
extern void move_tmp_to_org (char*, char*, int);
extern int output_file_header (FILE*, struct wnn_file_head*);
extern int input_file_header  (FILE*, struct wnn_file_head*);
extern int output_file_uniq (struct wnn_file_uniq*, FILE*);
extern int check_inode (FILE*, struct wnn_file_head*);
extern int change_file_uniq (struct wnn_file_head*, char*);
extern int f_uniq_cmp (struct wnn_file_uniq*, struct wnn_file_uniq*);
extern void Get_knj2 (unsigned char*, int, w_char*, w_char*, w_char*);
extern void get_kanji_str (unsigned char*, w_char*, w_char*, w_char*);
extern void Get_kanji_str_r (unsigned char*, w_char**, w_char**, w_char**);
extern unsigned char kanjiaddr (unsigned char*, w_char*, w_char*, w_char*);
extern int create_null_dic (char*, w_char*, char*, char*, int);
extern int create_hindo_file (struct wnn_file_uniq*, char*, w_char*, char*, int);
extern int input_header_jt  (FILE*, struct JT*);
extern int output_header_jt (FILE*, struct JT*);
extern int input_header_hjt  (FILE*, struct HJT*);
extern int output_header_hjt (FILE*, struct HJT*);


/* pwd.c */
extern void new_pwd  (char*, char*);
extern int check_pwd (char*, char*);

#endif /* JSERVER_DE_HEADER_H */

