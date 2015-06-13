/*
 *  $Id: do_henkan.c,v 1.6 2013/09/02 11:01:39 itisango Exp $
 */

/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright FreeWnn Project 1999, 2000
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

#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#if defined(HAVE_SYS_TYPES_H)
#include <sys/types.h>
#endif

#include "commonhd.h"
#include "jslib.h"
#include "kaiseki.h"
#include "jdata.h"
#include "de_header.h"

static void ret_sho_x (register struct DSD_SBN *, register int);
static void ret_sho_kanji (register struct DSD_SBN *, register int);
static void ret_kanji (register struct DSD_SBN *);
static int count_sho (struct DSD_DBN *, int);
static int count_d_kanji (struct DSD_DBN *, int);
static int count_s_kanji (struct DSD_SBN *, int);
static int count_kanji (struct DSD_SBN *);

/*      DAI BUN */
void
ret_dai (dai_list, cnt)
     struct DSD_DBN *dai_list;
     int cnt;
{
  register int i;
  register struct DSD_DBN *dp;
  if (dai_list == NULL)
    {
      put4_cur (0);
      return;
    }
  put4_cur (cnt);

  put4_cur (count_sho (dai_list, cnt));
  put4_cur (count_d_kanji (dai_list, cnt));

  for (i = 0, dp = dai_list; i < cnt; dp++, i++)
    {
#ifdef  DEBUG
      out ("\tbun_jc:%d, sbncnt:%d, bun_m:%d ,sbn:%x\n", dp->bun_jc, dp->sbncnt, dp->bun_m, dp->sbn);
#endif
#ifdef  CONVERT_from_TOP
      put4_cur (dp->bun_jc);    /* end */
      put4_cur (dp->bun_m);     /* start */
#else
      put4_cur (maxchg - 1 - dp->bun_m);        /* end */
      put4_cur (maxchg - 1 - dp->bun_jc);       /* start */
#endif
      put4_cur (dp->sbncnt);
      put4_cur (dp->v_jc);
    }
  for (i = 0, dp = dai_list; i < cnt; dp++, i++)
    {
      ret_sho_x (dp->sbn, dp->sbncnt);
    }
  for (i = 0, dp = dai_list; i < cnt; dp++, i++)
    {
      ret_sho_kanji (dp->sbn, dp->sbncnt);
    }
}

static int
count_sho (dai_list, cnt)
     struct DSD_DBN *dai_list;
     int cnt;
{
  register int sho_sum, i;
  register struct DSD_DBN *dp;
  for (sho_sum = i = 0, dp = dai_list; i < cnt; dp++, i++)
    {
      sho_sum += dp->sbncnt;
    }
  return sho_sum;
}


static int
count_d_kanji (dp, cnt)
     struct DSD_DBN *dp;
     int cnt;
{
  register int k_sum, i;
  for (k_sum = i = 0; i < cnt; dp++, i++)
    {
      k_sum += count_s_kanji (dp->sbn, dp->sbncnt);
    }
  return k_sum;
}

static int
count_s_kanji (sp, cnt)
     struct DSD_SBN *sp;
     int cnt;
{
  register int i, sho_sum;
  if (sp == NULL)
    cnt = 0;
  sho_sum = 0;
  for (i = 0; i < cnt; sp++, i++)
    {
      sho_sum += count_kanji (sp);
    }
  return sho_sum;
}

static int
count_kanji (sho)
     struct DSD_SBN *sho;
{
  register int tmp, k_sum;
  w_char kouho[1000];

  if (sho->jentptr)
    {
      Get_knj (sho->jentptr, sho->t_jc, kouho, bun + sho->i_jc, sho->bun_jc - sho->i_jc + 1);
    }
  else
    {
      get_giji_knj (sho->bun_jc, sho->i_jc, sho->t_jc, kouho);
    }

  k_sum = 0;
  k_sum += Strlen (kouho) + 1;  /* kanji */
  /* yomi */
  for (tmp = sho->bun_jc;; tmp--)
    {
      if (tmp < sho->bun_m)
        break;
      k_sum++;                  /* put2_cur(bun[tmp]); */
      if (tmp == sho->i_jc)
        break;
    }
  k_sum++;                      /* put2_cur(0); */
  /* fuzoku */
  for (tmp = sho->i_jc - 1;; tmp--)
    {
      if (tmp < sho->bun_m)
        break;
      k_sum++;                  /* put2_cur(bun[tmp]); */
    }
  k_sum++;                      /* put2_cur(0); */
  return (k_sum);
}


static void
ret_sho_x (sho_list, cnt)
     register struct DSD_SBN *sho_list;
     register int cnt;
{
  if (sho_list == NULL)
    {
      cnt = 0;
    }
  for (; cnt > 0; cnt--)
    {
#ifdef  DEBUG
      out ("\tbun_jc: %d, i_jc: %d, bun_m: %d \n", sho_list->bun_jc, sho_list->i_jc, sho_list->bun_m);
      out ("\tjdata:%x, kangovect:%d, t_jc:%d, hinsi: %d:%s, status:%d\n",
           sho_list->jentptr, sho_list->kangovect, sho_list->t_jc, sho_list->hinsi, wnn_get_hinsi_name (sho_list->hinsi), sho_list->status);
#endif

#ifdef  CONVERT_from_TOP
      put4_cur (sho_list->bun_jc);      /*end */
      put4_cur (sho_list->bun_m);       /*start */
      put4_cur (sho_list->bun_jc);      /*jiriend */
#else
      put4_cur (maxchg - 1 - sho_list->bun_m);  /*end */
      put4_cur (maxchg - 1 - sho_list->bun_jc); /*start */
      put4_cur (maxchg - 1 - sho_list->i_jc);   /*jiriend */
#endif

      if (sho_list->jentptr != NULL)
        {
          int hindo;
          put4_cur (sho_list->jentptr->jishono);        /*jisho No */
          put4_cur (sho_list->jentptr->serial + sho_list->t_jc);
          /* entry + t_jc */
          /* Internally, we use "kasou-hindo"
           * so return kasou-hindo as hindo-value"*/
          hindo = *(sho_list->jentptr->hindo + sho_list->t_jc) & 0x7F;
          if (sho_list->jentptr->hindo_in)
            {
              hindo += *(sho_list->jentptr->hindo_in + sho_list->t_jc) & 0x7F;
              /* BUG */
              if (hindo == 0x7f)
                hindo--;
            }
          put4_cur (hindo);
          /* hindo (u_char) */
          put4_cur (*(sho_list->jentptr->hindo + sho_list->t_jc) >> 7);
          /* ima */
        }
      else
        {
          put4_cur (-1);
          put4_cur (sho_list->t_jc);
          put4_cur (-1);
          put4_cur (-1);
        }
      put4_cur (sho_list->hinsi);       /* hinshi */
      put4_cur (sho_list->status);
      put4_cur (sho_list->status_bkwd);
      put4_cur (sho_list->kangovect);
      put4_cur (sho_list->v_jc);
      sho_list++;
    }
}

static void
ret_sho_kanji (sho_list, cnt)
     register struct DSD_SBN *sho_list;
     register int cnt;
{
  if (sho_list == NULL)
    {
      cnt = 0;
    }
  for (; cnt > 0; sho_list++, cnt--)
    {
      ret_kanji (sho_list);
    }
}

static void
ret_kanji (sho)
     register struct DSD_SBN *sho;
{
  register int tmp;
  w_char kouho[1000];

  if (sho->jentptr)
    {
      Get_knj (sho->jentptr, sho->t_jc, kouho, bun + sho->i_jc, sho->bun_jc - sho->i_jc + 1);
    }
  else
    {
      get_giji_knj (sho->bun_jc, sho->i_jc, sho->t_jc, kouho);
    }
  putws_cur (kouho);            /* kanji */
  /* yomi */
#ifdef  CONVERT_from_TOP
  for (tmp = sho->i_jc; tmp <= sho->bun_jc; tmp++)
    {
#else
  for (tmp = sho->bun_jc; tmp >= sho->i_jc; tmp--)
    {
#endif
      put2_cur (bun[tmp]);
    }

  put2_cur (0);
  /* fuzoku */
  for (tmp = sho->i_jc - 1;; tmp--)
    {
      if (tmp < sho->bun_m)
        break;
      put2_cur (bun[tmp]);
    }
  put2_cur (0);
}

/*      SHO BUN */
void
ret_sho (sho_list, cnt)
     register struct DSD_SBN *sho_list;
     register int cnt;
{
  if (sho_list == NULL)
    {
      cnt = 0;
      put4_cur (cnt);
      return;
    }
  put4_cur (cnt);
  put4_cur (count_s_kanji (sho_list, cnt));

  ret_sho_x (sho_list, cnt);
  ret_sho_kanji (sho_list, cnt);
}

#ifdef  CONVERT_by_STROKE
extern struct b_koho *p_kohos[];        /* BWNN koho buffer */

static int
count_s_kanji_B (cnt)
     int cnt;
{
  register int i, sho_sum;
  sho_sum = 0;
  for (i = 0; i < cnt; i++)
    {
      sho_sum += Strlen (p_kohos[i]->p_kanji);
      sho_sum += Strlen (p_kohos[i]->p_yomi);
      sho_sum += 3;
    }
  return sho_sum;
}

static void
ret_sho_x_B (end, start, cnt, zenp)
     int end;
     int start;
     register int cnt;
     int zenp;
{
  int i;
  int hindo = 0;
  struct b_koho *b_pter;

  for (i = 0; i < cnt; i++)
    {
      put4_cur (end);           /*end */
      put4_cur (start);         /*start */
      put4_cur (end);           /*jiriend */
      if ((b_pter = p_kohos[i]) != NULL)
        {
          put4_cur (b_pter->dic_no);    /*jisho No */
          put4_cur (b_pter->serial);
          hindo = *(b_pter->p_hindo);
          put4_cur (hindo & 0x7F);      /* hindo (u_char) */
          put4_cur (hindo >> 7);        /* ima */
          put4_cur (*(b_pter->p_hinsi));        /* hinshi */
        }
      else
        {
          put4_cur (-1);
          put4_cur (0);
          put4_cur (-1);
          put4_cur (-1);
          put4_cur (0);
        }
      put4_cur (3);
      put4_cur (1);
      put4_cur (0x7);
      put4_cur (hindo * 5 - i);
    }
}

static void
ret_sho_kanji_B (end, start, cnt, zenp)
     int end;
     int start;
     register int cnt;
     int zenp;
{
  int i;
  if (zenp == WNN_ZENKOUHO || (cnt != 1))
    {
      for (i = 0; i < cnt; i++)
        {
          putws_cur (p_kohos[i]->p_kanji);      /* kanji */
          putws_cur (p_kohos[i]->p_yomi);       /* yomi */
          /* fuzoku */
          put2_cur (0);
        }
    }
  else
    {
      putws_cur (p_kohos[0]->p_kanji);
      for (i = end; i < start; i++)
        put2_cur (bun[i]);
      put2_cur (0);
      /* fuzoku */
      put2_cur (0);
    }
}

/* BWNN SHO BUN */
void
ret_B (end, start, cnt, zenp)
     int end;
     int start;
     register int cnt;
     int zenp;
{
  if (cnt == 0)
    {
      put4_cur (cnt);
      return;
    }
  put4_cur (cnt);
  put4_cur (count_s_kanji_B (cnt));

  ret_sho_x_B (end, start, cnt, zenp);
  ret_sho_kanji_B (end, start, cnt, zenp);
}

void
ret_daiB (end, start, cnt, zenp)
     int end;
     int start;
     register int cnt;
     int zenp;
{
  register int i;
  struct b_koho *b_pter;
  int hindo = 0;

  if (cnt == 0)
    {
      put4_cur (cnt);
      return;
    }
  put4_cur (cnt);
  put4_cur (cnt);
  put4_cur (count_s_kanji_B (cnt));

  for (i = 0; i < cnt; i++)
    {
      put4_cur (end);
      put4_cur (start);
      put4_cur (1);
      b_pter = p_kohos[i];
      hindo = *(b_pter->p_hindo);
      put4_cur (hindo * 5 - i);
    }
  ret_sho_x_B (end, start, cnt, zenp);
  ret_sho_kanji_B (end, start, cnt, zenp);

}
#endif /* CONVERT_by_STROKE */

#ifdef  DEBUG

#ifdef  putwchar
#undef  putwchar
#endif

static void _print_dlist (struct DSD_DBN *);
static void _print_dlist1 (struct DSD_SBN *);

void
print_dlist (dlist, cnt)
     struct DSD_DBN *dlist;
     int cnt;
{

  out ("大文節数:%d\n", cnt);
  if (dlist == 0)
    return;
  for (; cnt > 0; dlist++, cnt--)
    {
      _print_dlist (dlist);
      print_dlist1 (dlist->sbn, dlist->sbncnt);
    }
  fflush (stdout);
}

static void
_print_dlist (dlist)
     struct DSD_DBN *dlist;
{
  struct DSD_SBN *sbn;
  int i;
  int tmp;
  w_char kouho[512];

  out ("大文節:%x\t", dlist);
  if (dlist == 0)
    return;

  sbn = dlist->sbn;
  for (i = dlist->sbncnt; i > 0; i--)
    {
      if (sbn == 0)
        break;

      if (sbn->jentptr)
        Get_knj (sbn->jentptr, sbn->t_jc, kouho, bun + sbn->i_jc, sbn->bun_jc - sbn->i_jc + 1);
      else
        get_giji_knj (sbn->bun_jc, sbn->i_jc, sbn->t_jc, kouho);
      out (" ");
      wsputs (kouho);
      out (":");
      for (tmp = sbn->i_jc - 1; tmp >= sbn->bun_m; tmp--)
        putwchar (bun[tmp]);

      sbn++;
    }
  out ("\t\thyouka:%d\n", dlist->v_jc);
  fflush (stdout);
}

void
print_dlist1 (dlist1, cnt)
     struct DSD_SBN *dlist1;
     int cnt;
{
  unsigned int hindo;

  for (; cnt > 0; dlist1++, cnt--)
    {
      _print_dlist1 (dlist1);
      if (dlist1 == 0)
        return;
      if (dlist1->jentptr == 0)
        hindo = 0;
      else
        hindo = *(dlist1->jentptr->hindo + dlist1->t_jc);
      out ("  jdata:%x vect:%d t_jc:%d hinsi:%d:%s hindo:%x hyoka:%d status:%d status_bkwd:%d\n",
           dlist1->jentptr, dlist1->kangovect, dlist1->t_jc, dlist1->hinsi, wnn_get_hinsi_name (dlist1->hinsi), hindo, dlist1->v_jc, dlist1->status, dlist1->status_bkwd);
    }
  fflush (stdout);
}

static void
_print_dlist1 (dlist1)
     struct DSD_SBN *dlist1;
{
  int tmp;
  w_char kouho[512];

  out ("小文節:%x\t", dlist1);
  if (dlist1 == 0)
    {
      return;
    }
  for (tmp = dlist1->bun_jc; tmp >= dlist1->bun_m; tmp--)
    {
      putwchar (bun[tmp]);
      if (tmp == dlist1->i_jc)
        {
          if (dlist1->jentptr)
            Get_knj (dlist1->jentptr, dlist1->t_jc, kouho, bun + dlist1->i_jc, dlist1->bun_jc - dlist1->i_jc + 1);
          else
            get_giji_knj (dlist1->bun_jc, dlist1->i_jc, dlist1->t_jc, kouho);
          out (" ");
          wsputs (kouho);
          out (":");
        }
    }
  out ("\n");
  fflush (stdout);
}

#endif /* DEBUG */
