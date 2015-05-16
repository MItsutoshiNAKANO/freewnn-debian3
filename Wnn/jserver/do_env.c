/*
 *  $Id: do_env.c,v 1.9 2004/05/21 16:39:32 aono Exp $
 */

/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright FreeWnn Project 1999, 2000, 2002, 2003
 *
 * Maintainer:  FreeWnn Project   <freewnn@tomo.gr.jp>
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
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#if STDC_HEADERS
#  include <stdlib.h>
#  include <string.h>
#else
#  if HAVE_MALLOC_H
#    include <malloc.h>
#  endif
#  if HAVE_STRINGS_H
#    include <strings.h>
#  endif
#endif /* STDC_HEADERS */

#include "demcom.h"
#include "commonhd.h"
#include "de_header.h"

static int  add_env_to_client (int);
static int  conn1 (char*);
static void del_env_from_client (int);
static void disconnect (int);
static int  disconnect_last_sticky (void);
static int  find_env_by_name (char *);
static int  find_env_in_client (int);
static void new_env (int env_id, char *n);
static int  escape_strncpy(char *dst, const char *src, size_t len);

static int sticky_cnt = 0;
static int sticky_time = 0;
/* used only in initjserv.c and do_env.c */
extern struct wnn_param default_para;

/*
        client routines
 */

void
js_open (void)
{
  register int version;
/*
 register int i;
*/
  char tmp_buf[256];
  char *ptr;
  int valid;

  version = get4_cur ();
  /* host_name */
  gets_cur (tmp_buf, WNN_HOSTLEN);
  strcpy (c_c->host_name, tmp_buf);
  escape_strncpy(tmp_buf, c_c->host_name, sizeof(tmp_buf));
  /* string validation: We should check more (ex. reverse host lookup)? */
  valid = 1;
  ptr = c_c->host_name;
  for(; *ptr != '\0'; ptr++) {
    /* Rough check: We should comply with some RFC ... */
    if(! isalnum(*ptr) && strchr(".-_", *ptr) == NULL)
      {
	valid = 0;
	*ptr = '_';
      }
  }
  if(valid != 1)
    {
      log_err("JS_OPEN: Warning: Specified hostname (\"%s\") contains unwanted character. (Possible attack? but continue.)", tmp_buf);
    }
  /* user_name */
  gets_cur (tmp_buf, WNN_ENVNAME_LEN);
  strcpy (c_c->user_name, tmp_buf);
  escape_strncpy(tmp_buf, c_c->user_name, sizeof(tmp_buf));
  /* string validation: We should check more? */
  valid = 1;
  ptr = c_c->user_name;
  for(; *ptr != '\0'; ptr++) {
    if(! isalnum(*ptr))
      {
	valid = 0;
	*ptr = '_';
      }
  }
  if(valid != 1)
    {
      log_err("JS_OPEN: Warning: Specified username (\"%s\") contains unwanted character. (Possible attack? but continue.)", tmp_buf);
    }
  log_debug("Inet user=%s@%s\n", c_c->user_name, c_c->host_name);
  /* Moved to new_client, because del_client() will be called
     by longjmp before this initialization. By Kuwari
     for(i=0;i<WNN_MAX_ENV_OF_A_CLIENT;i++){
     (c_c->env)[i]= -1;
     }
   */
  if (version != JLIB_VERSION)
    {
      wnn_errorno = WNN_BAD_VERSION;
      error_ret ();
      return;
    }
  put4_cur (0);
  putc_purge ();
}

void
js_close (void)
{
  put4_cur (0);
  putc_purge ();
  del_client ();
}

/*
        env. control routines
 */

void
env_init (void)
{
  register int i;
  for (i = 0; i < MAX_ENV; i++)
    {
      env[i] = NULL;
    }
}

/*      connect */
void
js_connect (void)
{
  char n[256];
  register int x;
  gets_cur (n, 256);
  x = conn1 (n);
  if (x == -1)
    {
      error_ret ();
      return;
    }
  put4_cur (x);
  putc_purge ();
}

void
js_env_sticky (void)
{
  register int eid;
  eid = get4_cur ();
  env[eid]->sticky = 1;
  put4_cur (0);
  putc_purge ();
}

void
js_env_un_sticky (void)
{
  register int eid;
  eid = get4_cur ();
  env[eid]->sticky = 0;
  put4_cur (0);
  putc_purge ();
}

/*
  Security issue (ja): n について制御文字などが入った際の措置について
  考慮する必要がある。今のところ n自体のチェックで拒否せず、ログの
  記録にとどめている。悪用を防止するため、ログに出力する制御文字は
  加工すること。
 */
static int
conn1 (char *n)
{
  register int eid;
  register struct cnv_env *ne;
  char n_escaped[256];

  if ((eid = find_env_by_name (n)) != -1)
    {                           /* exist */
      if (find_env_in_client (eid) != -1)
        return eid;             /* exist in client */
      if (add_env_to_client (eid) == -1)
        return -1;
      if (env[eid]->ref_count == 0 && env[eid]->sticky != 0 && sticky_cnt > 0)
        sticky_cnt--;
      env[eid]->ref_count++;
      return eid;
    }
  /* new */
  for (eid = 0; eid < MAX_ENV; eid++)
    {
      if (env[eid] == NULL)
        goto new;
    }
  if ((eid = disconnect_last_sticky ()) < 0)
    return -1;                  /* no more env */
new:
  if ((ne = (struct cnv_env *) malloc (sizeof (struct cnv_env))) == 0)
    return -1;
  env[eid] = ne;
  new_env (eid, n);
  if (add_env_to_client (eid) == -1)
    {
      free (ne);
      env[eid] = NULL;
      log_err("conn1: Cannot add new environment to client.");
      return -1;
    }
  escape_strncpy(n_escaped, n, sizeof(n_escaped));
  /* (少なくともログの問題が解決するまで)そのままnを使ってはならない */
  /* Quick check: Should we check n separately? */
  if(strcmp(n, n_escaped) != 0)
    {
      log_err("conn1: Warning: Specified env string (\"%s\") contains unwanted character. (Possible attack? But continue.)", n_escaped);
    }
  log_debug("new_env: Created , %s env_id=%d\n", n_escaped, eid);
  return eid;
}

void
js_env_exist (void)
{
  char n[256];

  gets_cur (n, 256);
  if (find_env_by_name (n) != -1)
    {                           /* exist */
      put4_cur (1);
    }
  else
    {
      put4_cur (0);
    }
  putc_purge ();
}

static int
add_env_to_client (int eid)
{
  register int j;
  for (j = 0; j < WNN_MAX_ENV_OF_A_CLIENT; j++)
    {
      if ((c_c->env)[j] == -1)
        {
          (c_c->env)[j] = eid;
          return j;
        }
    }
  return -1;                    /* no more env of Client */
}

static int
find_env_by_name (char *n)
{
  register int i;

  if (n[0] == 0)
    return -1;                  /* H.T. Null-string means new_env! */
  for (i = 0; i < MAX_ENV; i++)
    {
      if (env[i] == NULL)
        continue;
      if (strcmp (env[i]->env_name, n) == 0)
        {
          return i;
        }
    }
  return -1;
}

void
new_env (int env_id, char *n)
{
  register int i;
  struct cnv_env *e = env[env_id];

  strcpy (e->env_name, n);
  e->ref_count = 1;

  for (i = 0; i < WNN_MAX_JISHO_OF_AN_ENV; i++)
    {
      (e->jisho)[i] = -1;
    }
  for (i = 0; i < WNN_MAX_FILE_OF_AN_ENV; i++)
    {
      (e->file)[i] = -1;
    }
  e->jishomax = 0;
  e->sticky = 0;

  e->fzk_fid = -1;              /* Not Loaded */
  e->nbun = default_para.n;
  e->nshobun = default_para.nsho;

  e->hindoval = default_para.p1;
  e->lenval = default_para.p2;
  e->jirival = default_para.p3;
  e->flagval = default_para.p4;
  e->jishoval = default_para.p5;
  e->sbn_val = default_para.p6;
  e->dbn_len_val = default_para.p7;
  e->sbn_cnt_val = default_para.p8;

  /* 疑似品詞の頻度のセット */
  e->suuji_val = default_para.p9;
#ifdef  CHINESE
  e->eisuu_val = default_para.p10;
  e->kigou_val = default_para.p11;
  e->kaikakko_val = default_para.p12;
  e->toji_kakko_val = default_para.p13;
  e->fuzokogo_val = default_para.p14;
  e->kana_val = default_para.p15;
#else
  e->kana_val = default_para.p10;
  e->eisuu_val = default_para.p11;
  e->kigou_val = default_para.p12;
  e->toji_kakko_val = default_para.p13;
  e->fuzokogo_val = default_para.p14;
  e->kaikakko_val = default_para.p15;
#endif /* CHINESE */

  /* 疑似文節のためのデフォルト候補のセット */
  e->giji.number = WNN_NUM_HAN;
  e->giji.eisuu = WNN_ALP_HAN;
  e->giji.kigou = WNN_KIG_HAN;
}

/*      disconnect all env of current client (socket)   */
void
disconnect_all_env_of_client (void)
{
  register int i, eid;
  for (i = 0; i < WNN_MAX_ENV_OF_A_CLIENT; i++)
    {
      eid = (c_c->env)[i];
      if (eid == -1)
        continue;
      disconnect (eid);
    }
}

/*      disconnect      */
void
js_disconnect (void)
{
  register int eid, j;
  eid = get4_cur ();

  if ((j = find_env_in_client (eid)) == -1)
    {
      put4_cur (-1);
      wnn_errorno = WNN_BAD_ENV;
      error_ret ();
      return;
    }
  del_env_from_client (j);
  disconnect (eid);
  put4_cur (0);
  putc_purge ();
}

static void
disconnect (int eid)
{
  if (env[eid]->ref_count && (--(env[eid]->ref_count)) != 0)
    return;
  if (env[eid]->sticky)
    {
      env[eid]->sticky = ++sticky_time;
      if (++sticky_cnt > max_sticky_env)
        disconnect_last_sticky ();
      return;
    }
  del_all_dic_in_env (eid);
  del_all_file_in_env (eid);
  free (env[eid]);
  env[eid] = NULL;
  error1 ("disconnect: Free eid=%d\n", eid);
}

static int
disconnect_last_sticky (void)
{
  register int i;

  register int min = 0x7fffffff;
  register int eid = -1;
  for (i = 0; i < MAX_ENV; i++)
    {
      if (env[i] == NULL)
        continue;
      if (env[i]->ref_count == 0)
        {
          if (min >= env[i]->sticky)
            {
              eid = i;
              min = env[i]->sticky;
            }
        }
    }
  if (eid >= 0)
    {
      sticky_cnt--;
      del_all_dic_in_env (eid);
      del_all_file_in_env (eid);
      free (env[eid]);
      env[eid] = NULL;
      error1 ("disconnect sticky Env: Free eid=%d\n", eid);
    }
  return (eid);
}

/* */
static void
del_env_from_client (int e)
{
  (c_c->env)[e] = -1;
}

static int
find_env_in_client (int eid)
{
  register int j;
  for (j = 0; j < WNN_MAX_ENV_OF_A_CLIENT; j++)
    {
      if ((c_c->env)[j] == eid)
        return j;
    }
  return -1;
}

/*      env_list        */
void
js_env_list (void)
{
  register int i, c, j;
  for (c = i = 0; i < MAX_ENV; i++)
    {
      if (env[i] == NULL)
        continue;
      c++;
    }
  put4_cur (c);
  for (c = i = 0; i < MAX_ENV; i++)
    {
      if (env[i] == NULL)
        continue;
      put4_cur (i);
      puts_cur (env[i]->env_name);
      put4_cur (env[i]->ref_count);
      put4_cur (env[i]->fzk_fid);
      put4_cur (env[i]->jishomax);
      for (j = 0; j < WNN_MAX_JISHO_OF_AN_ENV; j++)
        {
          put4_cur ((env[i]->jisho)[j]);
        }
      for (j = 0; j < WNN_MAX_FILE_OF_AN_ENV; j++)
        {
          put4_cur ((env[i]->file)[j]);
        }
    }
  putc_purge ();
}

/*
        conv. param routines
 */

void
js_param_set (void)
{
  register int eid;
  register int ret = 0;
  struct cnv_env s_env;
  if ((eid = envhandle ()) == -1)
    {
      int dummy, i;
      ret = -1;
      for (i = 0; i < 17; i++)
        dummy = get4_cur ();
    }
  else
    {
      bcopy (env[eid], &s_env, sizeof (struct cnv_env));

      env[eid]->nbun = get4_cur ();     /* Ｎ(大)文節解析のＮ */
      env[eid]->nshobun = get4_cur ();  /* 大文節中の小文節の最大数 */

      env[eid]->hindoval = get4_cur (); /* 幹語の頻度のパラメータ */
      env[eid]->lenval = get4_cur ();   /* 小文節長のパラメータ */
      env[eid]->jirival = get4_cur ();  /* 幹語長のパラメータ */
      env[eid]->flagval = get4_cur ();  /* 今使ったよbitのパラメータ */
      env[eid]->jishoval = get4_cur (); /* 辞書のパラメータ */

      env[eid]->sbn_val = get4_cur ();  /* 小文節の評価値のパラメータ */
      env[eid]->dbn_len_val = get4_cur ();      /* 大文節長のパラメータ */
      env[eid]->sbn_cnt_val = get4_cur ();      /* 小文節数のパラメータ */

      env[eid]->suuji_val = get4_cur ();        /* 疑似品詞 数字の頻度 */
#ifdef  CHINESE
      env[eid]->eisuu_val = get4_cur ();        /* 疑似品詞 英数の頻度 */
      env[eid]->kigou_val = get4_cur ();        /* 疑似品詞 記号の頻度 */
      env[eid]->kaikakko_val = get4_cur ();     /* 疑似品詞 開括弧の頻度 */
      env[eid]->toji_kakko_val = get4_cur ();   /* 疑似品詞 閉括弧の頻度 */
      env[eid]->fuzokogo_val = get4_cur ();     /* No of koho for BWNN  */
      env[eid]->kana_val = get4_cur (); /* Not used  */
#else
      env[eid]->kana_val = get4_cur (); /* 疑似品詞 カナの頻度 */
      env[eid]->eisuu_val = get4_cur ();        /* 疑似品詞 英数の頻度 */
      env[eid]->kigou_val = get4_cur ();        /* 疑似品詞 記号の頻度 */
      env[eid]->toji_kakko_val = get4_cur ();   /* 疑似品詞 閉括弧の頻度 */
      env[eid]->fuzokogo_val = get4_cur ();     /* 疑似品詞 付属語の頻度 */
      env[eid]->kaikakko_val = get4_cur ();     /* 疑似品詞 開括弧の頻度 */
#endif /* CHINESE */
      if (env[eid]->nbun < 1 || env[eid]->nshobun < 1)
        {
          ret = -1;
          bcopy (&s_env, env[eid], sizeof (struct cnv_env));
        }
    }

  if (ret == -1)
    {
      error_ret ();
      return;
    }
  put4_cur (0);
  putc_purge ();
#ifdef DEBUG
  error1 ("param_set:eid=%d\n", eid);
#endif
}


void
js_param_get (void)
{
  register int eid;
  if ((eid = envhandle ()) == -1)
    {
      error_ret ();
      return;
    }
  put4_cur (0);
  put4_cur (env[eid]->nbun);    /* Ｎ(大)文節解析のＮ */
  put4_cur (env[eid]->nshobun); /* 大文節中の小文節の最大数 */
  put4_cur (env[eid]->hindoval);        /* 幹語の頻度のパラメータ */
  put4_cur (env[eid]->lenval);  /* 小文節長のパラメータ */
  put4_cur (env[eid]->jirival); /* 幹語長のパラメータ */
  put4_cur (env[eid]->flagval); /* 今使ったよビットのパラメータ */
  put4_cur (env[eid]->jishoval);        /* 辞書のパラメータ */

  put4_cur (env[eid]->sbn_val); /* 小文節の評価値のパラメータ */
  put4_cur (env[eid]->dbn_len_val);     /* 大文節長のパラメータ */
  put4_cur (env[eid]->sbn_cnt_val);     /* 小文節数のパラメータ */

  put4_cur (env[eid]->suuji_val);       /* 疑似品詞 数字の頻度 */
#ifdef  CHINESE
  put4_cur (env[eid]->eisuu_val);       /* 疑似品詞 英数の頻度 */
  put4_cur (env[eid]->kigou_val);       /* 疑似品詞 記号の頻度 */
  put4_cur (env[eid]->kaikakko_val);    /* 疑似品詞 開括弧の頻度 */
  put4_cur (env[eid]->toji_kakko_val);  /* 疑似品詞 閉括弧の頻度 */
  put4_cur (env[eid]->fuzokogo_val);    /*  No of koho for BWNN  */
  put4_cur (env[eid]->kana_val);        /*  Not used  */
#else
  put4_cur (env[eid]->kana_val);        /* 疑似品詞 カナの頻度 */
  put4_cur (env[eid]->eisuu_val);       /* 疑似品詞 英数の頻度 */
  put4_cur (env[eid]->kigou_val);       /* 疑似品詞 記号の頻度 */
  put4_cur (env[eid]->toji_kakko_val);  /* 疑似品詞 閉括弧の頻度 */
  put4_cur (env[eid]->fuzokogo_val);    /* 疑似品詞 付属語の頻度 */
  put4_cur (env[eid]->kaikakko_val);    /* 疑似品詞 開括弧の頻度 */
#endif /* CHINESE */

  putc_purge ();
#ifdef DEBUG
  error1 ("param_get:eid=%d\n", eid);
#endif
}

int
envhandle (void)
{
#ifdef nodef
  register int eid;
  eid = get4_cur ();
  if (find_env_in_client (eid) == -1)
    {
      wnn_errorno = WNN_BAD_ENV;
      return (-1);
    }
  c_env = env[eid];
  return (eid);
#endif
  return (get4_cur ());
}

/*
    strncpy with escaping undesirable (ex. control) character.
    But destination string should be null-terminated in this function.
    If it's neat, you should extern this function (and maybe
    move to other file).
    Return value: Not 0 if all char in src is not filled in dst.
    FIXME: It may not be 8bit safe.
 */
static int
escape_strncpy(char *dst, const char *src, size_t len)
{
  char *dstptr = dst;		/* ウォーニングをなくすためsrcはそのまま使用 */
  char *dstend = dst + (len - 1);	/* Preserve last char (for '\0') */
  int overrun = 0;

  if(len == 0 || dst == NULL || src == NULL) return 0;
  while(*src != '\0' && dstptr < dstend) {
    if(*src == '\\') {
      /* Escape backslash as '\\' */
      if(dstptr + 2 > dstend) {
	/* Buffer exceeded: Give up */
	overrun = 1;
	break;
      } else {
	*dstptr++ = '\\';
	*dstptr++ = '\\';
      }
    } else if(isgraph(*src)) {
      *dstptr++ = *src++;
    } else {
      /* Escape this char: like "\xFF" */
      if (dstptr + 4 > dstend) {
	/* Buffer exceeded: Give up */
	overrun = 1;
	break;
      } else {
	sprintf(dstptr, "\\x%02X", *src++); /* Take care of buffer length */
	dstptr += 4;
      }
    }
  }
  if(*src != '\0') {
    overrun = 1;
  }
  *dstptr = '\0';

  return overrun;
}
