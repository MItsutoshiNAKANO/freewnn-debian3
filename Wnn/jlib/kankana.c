/*
 *  $Id: kankana.c,v 1.6 2005/04/10 15:26:37 aonoto Exp $
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

#ifndef lint
static char *rcs_id = "$Id: kankana.c,v 1.6 2005/04/10 15:26:37 aonoto Exp $";
#endif /* lint */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#if STDC_HEADERS
#  include <stdlib.h>
#else
#  if HAVE_MALLOC_H
#    include <malloc.h>
#  endif
#endif /* STDC_HEADERS */
#include "jslib.h"

#define FZK_FILE "pubdic/full.fzk"
#define KIHON_DIC "pubdic/kihon.dic"
#define SETTO_DIC "pubdic/setsuji.dic"

void err (void);
void henkan (void);
void p_set (struct wnn_env *env1);
void print_kanji (struct wnn_dai_bunsetsu *dlist, int cnt);
void putws (unsigned short *s);
void strtows (w_char *u, unsigned char *e);

WNN_JSERVER_ID *js;
struct wnn_env *env, *rev_env;
static struct wnn_ret_buf rb = { 0, NULL };

int dcnt, scnt;

char yomi[1024];
char kanji[5000];

int kihon_file, setto_file, rev_file, fzk_file, rev_setto_file;
int kihon_dic_no, setto_dic_no, rev_dic_no, rev_setto_dic_no;

int
main (int argc, char **argv)
{
  char *mname = "";
  rb.buf = (char *) malloc ((unsigned) (rb.size = 0));
  if (argc > 1)
    mname = argv[1];

  if ((js = js_open (mname, 0)) == NULL)
    err ();

  if ((env = js_connect (js, "kana")) == NULL)
    err ();
  if ((fzk_file = js_file_read (env, FZK_FILE)) == -1)
    err ();
  if ((kihon_file = js_file_read (env, KIHON_DIC)) == -1)
    err ();
  if ((setto_file = js_file_read (env, SETTO_DIC)) == -1)
    err ();

  if (js_fuzokugo_set (env, fzk_file) == -1)
    err ();
  if ((kihon_dic_no = js_dic_add (env, kihon_file, -1, WNN_DIC_ADD_NOR, 1, WNN_DIC_RDONLY, WNN_DIC_RDONLY, NULL, NULL)) == -1)
    err ();
  if ((setto_dic_no = js_dic_add (env, setto_file, -1, WNN_DIC_ADD_NOR, 1, WNN_DIC_RDONLY, WNN_DIC_RDONLY, NULL, NULL)) == -1)
    err ();
  p_set (env);

  if ((rev_env = js_connect (js, "kanji")) == NULL)
    err ();

  if ((fzk_file = js_file_read (rev_env, FZK_FILE)) == -1)
    err ();
  if ((rev_file = js_file_read (rev_env, KIHON_DIC)) == -1)
    err ();
  if ((rev_setto_file = js_file_read (rev_env, SETTO_DIC)) == -1)
    err ();
  if (js_fuzokugo_set (rev_env, fzk_file) == -1)
    err ();

  if ((rev_dic_no = js_dic_add (rev_env, kihon_file, -1, WNN_DIC_ADD_REV, 1, WNN_DIC_RDONLY, WNN_DIC_RDONLY, NULL, NULL)) == -1)
    err ();
  if ((rev_setto_dic_no = js_dic_add (rev_env, setto_file, -1, WNN_DIC_ADD_REV, 1, WNN_DIC_RDONLY, WNN_DIC_RDONLY, NULL, NULL)) == -1)
    err ();
  p_set (rev_env);

#ifdef DEBUG
  printf ("Now discard file push any key\n");
  getchar ();
  js_file_discard (rev_env, rev_file);
  printf ("Now discard file\n");
#endif

  henkan ();
  js_close (js);
}

void
henkan (void)
{
  w_char u[1024];
  struct wnn_env *c_env = env;
  struct wnn_dai_bunsetsu *dp;

  for (;;)
    {
      if (c_env == env)
        {
          printf ("yomi> ");
          fflush (stdout);
        }
      else
        {
          printf ("kanji> ");
          fflush (stdout);
        }
      if (gets (yomi) == NULL)
        return;
      if (yomi[0] == '!')
        return;
      if (yomi[0] == '@')
        {
          c_env = (c_env == env) ? rev_env : env;
          continue;
        }
      strtows (u, yomi);
      dcnt = js_kanren (c_env, u, WNN_BUN_SENTOU, NULL, WNN_VECT_KANREN, WNN_VECT_NO, WNN_VECT_BUNSETSU, &rb);
      dp = (struct wnn_dai_bunsetsu *) rb.buf;
      print_kanji (dp, dcnt);
    }
}

void
print_kanji (struct wnn_dai_bunsetsu *dlist, int cnt)
{
  int i;
  struct wnn_sho_bunsetsu *sbn;

  if (dlist == 0)
    return;
  putchar ('\n');
  for (; cnt > 0; dlist++, cnt--)
    {
      sbn = dlist->sbn;
      for (i = dlist->sbncnt; i > 0; i--)
        {
          putws (sbn->kanji);
          printf ("-");
          putws (sbn->fuzoku);
          printf (" ");
          sbn++;
        }
      printf ("|");
    }
  putchar ('\n');
  fflush (stdout);
}

void
p_set (struct wnn_env *env1)
{
  struct wnn_param pa;
  pa.n = 2;                     /* n_bun */
  pa.nsho = 10;                 /* nshobun */
  pa.p1 = 2;                    /* hindoval */
  pa.p2 = 40;                   /* lenval */
  pa.p3 = 0;                    /* jirival */
  pa.p4 = 100;                  /* flagval */
  pa.p5 = 5;                    /* jishoval */
  pa.p6 = 1;                    /* sbn_val */
  pa.p7 = 15;                   /* dbn_len_val */
  pa.p8 = -20;                  /* sbn_cnt_val */
  pa.p9 = 0;                    /* kan_len_val */

  js_param_set (env1, &pa);
}

void
putwchar (unsigned short x)
{
  putchar (x >> 8);
  putchar (x);
}

void
putws (unsigned short *s)
{
  while (*s)
    putwchar (*s++);
}

void
strtows (w_char *u, unsigned char *e)
{
  int x;
  for (; *e;)
    {
      x = *e++;
      if (x & 0x80)
        x = (x << 8) | *e++;
      *u++ = x;
    }
  *u = 0;
}

void
err (void)
{
  printf (wnn_perror ());
  printf ("\n bye.\n");
  exit (1);
}

