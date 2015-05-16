/*
 *  $Id: fzk.c,v 1.7 2003/05/11 19:01:06 hiroo Exp $
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

static char rcs_id[] = "$Id: fzk.c,v 1.7 2003/05/11 19:01:06 hiroo Exp $";

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
#endif /* STDC_HEADERS */

#include "commonhd.h"
#include "de_header.h"
#include "fzk.h"
#include "kaiseki.h"

#define vgetc(pt) ((pt)? getc((pt)):xgetc_cur())
#define vungetc(k, pt) {if(pt) {ungetc((k),(pt));}else{xungetc_cur(k);}}

#ifndef NO_FZK
static void link_job (int, struct FT *);
static int fzk_ken (w_char *, w_char *, word_vector *, struct fzkken *);
static int setfzk (struct fzkentry *, struct fzkken **, word_vector *, int);
static struct fzkentry *bsrch (w_char *);
#endif
static int kng_ckvt (word_vector *, int);
static int fzk_ck_vector (word_vector *);
static void fzk_orvt (int *, int *);
static int bittest (int *, int);
static int error_fzk (void);
static int error_eof (void);
static int get_decimal (FILE *);
static int get_hexsa (FILE *);
static int get_string (FILE *, unsigned char *);
static int check_eof (FILE *);

#ifndef NO_FZK
static int fzkvect_kosuu;       /* 付属語ベクタ数 */
#endif /* NO_FZK */
static int kango_vect_kosuu;    /* 幹語ベクタの数 */
static int kango_length;        /* 幹語数 */

/*
 *      付属語ファイルのフォーマット

        付属語の個数            (ft->fzklength)
        接続ベクタの長さ        (fzkvect_l)
        付属語ベクタの個数      (fzkvect_kosuu)
        幹語ベクタの長さ        (kango_vect_l)          V4.0
        幹語の品詞数            (kango_length)          V4.0
        幹語ベクタの数          (kango_vect_kosuu)      V4.0

        付属語読み文字列        付属語ベクタ数
                No.     接続ベクタ
                …      …      
        付属語読み文字列        付属語ベクタ数
                No.     接続ベクタ
                …      …
                …      …      
        …
                …      …      
        〜

        終端ベクタ
        …
        …
        〜

        幹語接続ベクタNo.       幹語接続ベクタ          V4.0
                幹語接続ベクタ kango_vect_l     付属語接続ベクタ fzk_vect_l
                [31,30,...,1][63,62,....32]...
        …      …
        …      …
        〜

        幹語No. 幹語接続ベクタNo.                       V4.0
        …      …
        …      …
        〜

        疑似品詞No. 幹語接続ベクタNo. 疑似品詞ベクタ    (JKTVECT_KOSUU) V4.0
 */


struct FT *
fzk_read (FILE *fp)
{
  struct FT *fzk_tbl;

  fzk_tbl = fzk_ld (fp);
  if (fzk_tbl != NULL)
    {
#ifndef NO_FZK
      link_job (fzk_tbl->fzklength, fzk_tbl);
#endif
    }
  return (fzk_tbl);
}

struct FT *
fzk_ld (FILE *fp)
{
  struct FT *fzk_tbl;
  int k, l, m;
  int vect_count = 0;
#ifndef NO_FZK
  struct fzkentry *ptr;
  unsigned char charyomi[(YOMI_L + 1) * 2];
  int fzklength;
#endif /* NO_FZK */
  int fzkvect_l;
  int kango_vect_l;

#ifndef NO_FZK
  fzklength = get_decimal (fp); /* 付属語の個数 */
#endif /* NO_FZK */
  fzkvect_l = get_decimal (fp); /* 付属語ベクタの長さ */
#ifndef NO_FZK
  fzkvect_kosuu = get_decimal (fp);     /* 付属語ベクタの個数 */
#endif /* NO_FZK */
  kango_vect_l = get_decimal (fp);      /* 幹語ベクタの長さ */
  kango_vect_kosuu = get_decimal (fp);  /* 幹語ベクタの個数 */
  kango_length = get_decimal (fp);      /* 幹語の品詞数 */

  if (fzkvect_l > VECT_L)
    {
      wnn_errorno = WNN_FZK_TOO_DEF;
      log_err ("Sorry, your fuzokugo bit data has too many id.");
      log_err ("Please change define VECT_L %d and compile again.", fzkvect_l);
      return (NULL);
    }

  if (kango_vect_l > KANGO_VECT_L)
    {
      wnn_errorno = WNN_FZK_TOO_DEF;
      log_err ("Sorry, your kango bit data has too many id.");
      log_err ("Please change define KANGO_VECT_L %d and compile again.", kango_vect_l);
      return (NULL);
    }

  if (kango_length > KANGO_HINSI_MX)
    {
      wnn_errorno = WNN_FZK_TOO_DEF;
      log_err ("Sorry, your kango bit data has too many id.");
      log_err ("Please change define KANGO_MX %d and compile again.", kango_length);
      return (NULL);
    }
  if (kango_vect_kosuu > kango_length)
    {
      wnn_errorno = WNN_FZK_TOO_DEF;
      log_err ("KANGO vector kosuu is more than the number of KANGO hinsi.");
      return (NULL);
    }
  if ((fzk_tbl = (struct FT *) malloc (sizeof (struct FT))) == NULL)
    {
      wnn_errorno = WNN_MALLOC_ERR;
      log_err ("fzk_ld: malloc error.");
      return (NULL);
    }
  /* clear struct */
  bzero (fzk_tbl, sizeof (struct FT));

  fzk_tbl->kango_hinsi_area = NULL;
  fzk_tbl->kango_vect_area = NULL;
  fzk_tbl->fzkvect_l = fzkvect_l;
  fzk_tbl->kango_vect_l = kango_vect_l;
#ifndef NO_FZK
  fzk_tbl->vect_area = NULL;
  fzk_tbl->tablefuzokugo = NULL;
  fzk_tbl->fzklength = fzklength;

  if ((fzk_tbl->vect_area = (fzkvect *) calloc (fzkvect_kosuu, sizeof (fzkvect))) == NULL)
    {
      wnn_errorno = WNN_MALLOC_ERR;
      log_err ("fzk_ld: malloc error.");
      fzk_discard (fzk_tbl);
      return (NULL);
    }
#endif /* NO_FZK */

  if ((fzk_tbl->kango_hinsi_area = (int *) calloc (kango_length, sizeof (int))) == NULL)
    {
      wnn_errorno = WNN_MALLOC_ERR;
      error1 ("malloc error in fzk\n");
      fzk_discard (fzk_tbl);
      return (NULL);
    }
  if ((fzk_tbl->kango_vect_area = (word_vector *) calloc ((kango_vect_kosuu + SV_KOSUU), sizeof (word_vector))) == NULL)
    {
      wnn_errorno = WNN_MALLOC_ERR;
      log_err ("fzk_ld: malloc error.");
      fzk_discard (fzk_tbl);
      return (NULL);
    }

#ifndef NO_FZK
  if ((fzk_tbl->tablefuzokugo = (struct fzkentry *) calloc (fzk_tbl->fzklength, sizeof (struct fzkentry))) == NULL)
    {
      wnn_errorno = WNN_MALLOC_ERR;
      log_err ("fzk_ld: malloc error.");
      fzk_discard (fzk_tbl);
      return (NULL);
    }
  ptr = fzk_tbl->tablefuzokugo;

  for (m = 0; m < fzk_tbl->fzklength; m++, ptr++)
    {
      get_string (fp, charyomi);
      (void) Sstrcpy (ptr->yomi, charyomi);
      ptr->yomi_su = Strlen (ptr->yomi);
      ptr->kosu = get_decimal (fp);
      ptr->pter = &fzk_tbl->vect_area[vect_count];
      ptr->link = NULL;
      for (l = 0; l < ptr->kosu; l++, vect_count++)
        {
          fzk_tbl->vect_area[vect_count].no = get_decimal (fp);
          for (k = 0; k < fzkvect_l; k++)
            {
              fzk_tbl->vect_area[vect_count].vector[k] = get_hexsa (fp);
            }
        }
    }
#endif /* !NO_FZK */
/* 終端 vector を SV_KOSUU 個読み取る */

  for (l = 0; l < SV_KOSUU; l++)
    {
      for (k = 0; k < fzkvect_l; k++)
        {
          fzk_tbl->kango_vect_area[l].vector[k] = get_hexsa (fp);
        }
    }
/* 幹語接続ベクタを読み取る */
  /* 幹語前端接続ベクタNo. - 幹語前端接続ベクタ */
  for (m = 0; m < kango_vect_kosuu; m++)
    {
      vect_count = get_decimal (fp);
      for (k = 0; k < fzkvect_l; k++)
        {
          fzk_tbl->kango_vect_area[SV_KOSUU + vect_count].vector[k] = get_hexsa (fp);
        }
    }
  /* 幹語品詞No. - 幹語前端接続ベクタNo. */
  for (m = 0; m < kango_length; m++)
    {
      vect_count = get_decimal (fp);
      if ((k = get_decimal (fp)) != -1)
        {
          fzk_tbl->kango_hinsi_area[vect_count] = SV_KOSUU + k;
        }
      else
        {
          fzk_tbl->kango_hinsi_area[vect_count] = -1;
        }
    }

  if (check_eof (fp) < 0)
    {
      wnn_errorno = WNN_BAD_FZK_FILE;
      return (NULL);
    }

  /* 疑似品詞番号を調べる */
#ifdef  nodef
  if ((sentou_no = wnn_find_hinsi_by_name (WNN_SENTOU_MEI)) == -1)
    {
      giji_hinsi_err (WNN_SENTOU_MEI);
      return (NULL);
    }
  if ((suuji_no = wnn_find_hinsi_by_name (WNN_SUUJI_MEI)) == -1)
    {
      giji_hinsi_err ("SENTOU");
      return (NULL);
    }
  if ((katakanago_no = wnn_find_hinsi_by_name (WNN_KANA_MEI)) == -1)
    {
      giji_hinsi_err ("KANA");
      return (NULL);
    }
  if ((eisuu_no = wnn_find_hinsi_by_name (WNN_EISUU_MEI)) == -1)
    {
      giji_hinsi_err ("EISUU");
      return (NULL);
    }
  if ((kigou_no = wnn_find_hinsi_by_name (WNN_KIGOU_MEI)) == -1)
    {
      giji_hinsi_err ("KIGOU");
      return (NULL);
    }
  if ((toji_kakko_no = wnn_find_hinsi_by_name (WNN_TOJIKAKKO_MEI)) == -1)
    {
      giji_hinsi_err ("TOJI_KKAKO");
      return (NULL);
    }
  if ((fuzokugo_no = wnn_find_hinsi_by_name (WNN_FUZOKUGO_MEI)) == -1)
    {
      giji_hinsi_err ("FUZOKUGO");
      return (NULL);
    }
  if ((kai_kakko_no = wnn_find_hinsi_by_name (WNN_KAIKKAKO_MEI)) == -1)
    {
      giji_hinsi_err ("KAIKAKKO");
      return (NULL);
    }
  if ((giji_no = wnn_find_hinsi_by_name (WNN_GIJI_MEI)) == -1)
    {
      giji_hinsi_err ("GIJI");
      return (NULL);
    }
#endif /* nodef */
  return (fzk_tbl);
}

#ifdef  nodef
void
giji_hinsi_err (char *str)
{
  wnn_errorno = WNN_GIJI_HINSI_ERR;
  log_err ("GIJI hinsi (%s) is not defined in hinsi data file.", str);
}
#endif /* nodef */

void
fzk_discard (struct FT *fzk_tbl)
{
  if (fzk_tbl->kango_hinsi_area != NULL)
    free (fzk_tbl->kango_hinsi_area);
  if (fzk_tbl->kango_vect_area != NULL)
    free (fzk_tbl->kango_vect_area);
#ifndef NO_FZK
  if (fzk_tbl->vect_area != NULL)
    free (fzk_tbl->vect_area);
  if (fzk_tbl->tablefuzokugo != NULL)
    free (fzk_tbl->tablefuzokugo);
#endif
  free (fzk_tbl);
}

#ifndef NO_FZK
static void
link_job (int x, struct FT *fzk_tbl)
{
  int n;
  struct fzkentry *pter_a, *pter_b;

  for (pter_a = fzk_tbl->tablefuzokugo, x--, n = 0; n < x; n++, pter_a++)
    {
      for (pter_b = pter_a + 1; pter_b <= fzk_tbl->tablefuzokugo + x && Strncmp (pter_a->yomi, pter_b->yomi, pter_a->yomi_su) == 0; pter_b++)
        pter_b->link = pter_a;
    }
}
#endif


/*
 * fzk_kai : fuzokugo kaiseki
 */

int
fzk_kai (w_char *start,		/* string start pointer */
	w_char *end,		/* string end   pointer */
	int syuutan_vect,	/* 文節終端 vector */
	int syuutan_vect1,	/* 文節終端 vector 1 */
	struct ICHBNP **ichbnp_p) /* ich-bunpou area 付属語候補 set pointer pointer */
{
#ifndef NO_FZK
  static word_vector fzkwk[STRK_L + 1];		/* 付属語解析 work area */
  static word_vector fzkwk1[STRK_L + 1];	/* 付属語解析 work area */
  static int maxpoint = STRK_L;

  struct fzkken fzkinf[YOMI_L + 1];     /* 検索 work area */
  struct fzkken fzkinf1[YOMI_L + 1];    /* 検索 work area */
  struct fzkken *fzkinfp;
  struct ICHBNP *ichbnptr = NULL;
  struct ICHBNP *wkptr = NULL;

  int point;           /* index */
  int i, j;            /* work index */
  int cnt, n;          /* counter */
  word_vector *endvect;			/* 文節終端 vector pointer */
  word_vector *endvect1;		/* 文節終端 vector 1 */

  int fzkvect_l = ft->fzkvect_l;
  int kango_vect_l = ft->kango_vect_l;

  endvect = (word_vector *) ft->kango_vect_area + syuutan_vect;
  if (syuutan_vect1 != WNN_VECT_NO)
    endvect1 = (word_vector *) ft->kango_vect_area + syuutan_vect1;
  else
    endvect1 = NULL;

  /*
   * initialize
   */
  for (i = maxpoint; i >= 0; i--)
    {
      for (j = VECT_L - 1; j >= 0; j--)
        {
          fzkwk[i].vector[j] = 0;
          fzkwk1[i].vector[j] = 0;
        }
    }
  maxpoint = 0;
  for (j = 0; j < fzkvect_l; j++)
    {
      fzkwk[0].vector[j] = endvect->vector[j];
      if (endvect1 != 0)
        fzkwk1[0].vector[j] = endvect1->vector[j];
    }
  /*
   * 付属語解析
   */

  for (point = 0; point <= maxpoint; point++)
    {
      if (point > STRK_L)
        {
          wnn_errorno = WNN_WKAREA_FULL;
          log_err ("fzk_kai: fuzokugo-kaiseki area is full.");
          return (-1);
        }
      if (fzk_ck_vector (&fzkwk[point]))
        {
          /* 付属語検索 vector */
          fzk_ken (start + point, end, &fzkwk[point], &fzkinf[0]);
          for (fzkinfp = &fzkinf[0]; fzkinfp->ent_ptr; fzkinfp++)
            {
              n = (fzkinfp->ent_ptr)->yomi_su;
              fzk_orvt (fzkwk[point + n].vector, fzkinfp->vector);
              maxpoint = (maxpoint < point + n) ? point + n : maxpoint;
            }
        }
      if (fzk_ck_vector (&fzkwk1[point]))
        {
          /* 付属語検索 vector 1 */
          fzk_ken (start + point, end, &fzkwk1[point], &fzkinf1[0]);
          for (fzkinfp = &fzkinf1[0]; fzkinfp->ent_ptr; fzkinfp++)
            {
              n = (fzkinfp->ent_ptr)->yomi_su;
              fzk_orvt (fzkwk1[point + n].vector, fzkinfp->vector);
              maxpoint = (maxpoint < point + n) ? point + n : maxpoint;
            }
        }
    }


  /* 
   * 付属語候補 set
   */
  for (point = cnt = 0, *ichbnp_p = NULL; point <= maxpoint; point++)
    {
      if (kng_ckvt (&fzkwk[point], kango_vect_l) ||
	  kng_ckvt (&fzkwk1[point], kango_vect_l))
        {
          if (!(n = cnt % FZKIBNO))
            {
              if ((wkptr = getibsp ()) == NULL)
                {
                  if (*ichbnp_p != NULL)
                    (void) freeibsp (*ichbnp_p);
                  *ichbnp_p = NULL;
                  log_err ("fzk_kai: error.");
                  return (-1);
                }
              else
                {
                  if (*ichbnp_p == NULL)
                    *ichbnp_p = wkptr;
                  else
                    ichbnptr->next_p = wkptr;
                  ichbnptr = wkptr;
                }
            }
          ichbnptr->fzkib[n].offset = point;
          for (i = 0; i < kango_vect_l; i++)
            {
              ichbnptr->fzkib[n].vector[i] = fzkwk[point].vector[i];
              ichbnptr->fzkib1[n].vector[i] = fzkwk1[point].vector[i];
            }
          cnt++;
        }
    }
  return (cnt);
#else /* NO_FZK */
  struct ICHBNP *wkptr;

  int i;               /* work index */
  word_vector *endvect;		/* 文節終端 vector pointer */
  word_vector *endvect1;	/* 文節終端 vector 1 */

  int kango_vect_l = ft->kango_vect_l;

  endvect = (word_vector *) ft->kango_vect_area + syuutan_vect;
  if (syuutan_vect1 != WNN_VECT_NO)
    endvect1 = (word_vector *) ft->kango_vect_area + syuutan_vect1;
  else
    endvect1 = NULL;

  *ichbnp_p = NULL;
  if (kng_ckvt (endvect, kango_vect_l) || kng_ckvt (endvect1, kango_vect_l))
    {
      if ((wkptr = getibsp ()) == NULL)
        {
          log_err ("fzk_kai: error.");
          return (-1);
        }
      else
        {
          *ichbnp_p = wkptr;
        }
      wkptr->fzkib[0].offset = 0;
      for (i = 0; i < kango_vect_l; i++)
        {
          wkptr->fzkib[0].vector[i] = endvect->vector[i];
          if (endvect1 != NULL)
            wkptr->fzkib1[0].vector[i] = endvect1->vector[i];
        }
    }
  return (1);
#endif /* NO_FZK */
}


/*
 * kng_ckvt     幹語ベクタのチェック    V4.0
 */

static int
kng_ckvt (word_vector *wv, int kango_vect_l)
{
  int i, rts;
  int *v = wv->vector;

  for (rts = 0, i = kango_vect_l; i > 0; i--)
    rts |= *v++;           /* OR cheak */
  return (rts);
}


/*
 * fzk_ckvt
 */
int
fzk_ckvt (int vector)
{
  return vector >= 0 &&
	 fzk_ck_vector (&ft->kango_vect_area[vector]);
}


static int
fzk_ck_vector (word_vector *wv)
{
  int i, rts;
  int *v = wv->vector;

  rts = *v++ & ~0x01;		/* 「先頭可」のビットを除く */
  for (i = ft->fzkvect_l - 1; i > 0; i--)
    rts |= *v++;		/* OR cheak */
  return (rts);
}


/*
 * fzk_orvt
 */

static void
fzk_orvt (int *vector1, int *vector2)
{
  int i;

  for (i = ft->fzkvect_l; i > 0; i--)
    *vector1++ |= *vector2++;   /* OR set */
}

#ifndef NO_FZK
static int
fzk_ken (w_char *start, w_char *end, word_vector *wv, struct fzkken *fzkptr)
{
  int yomicnt;
  int setno;
  struct fzkken *ansptr;
  struct fzkentry *search_ptr;
  w_char key[YOMI_L + 1];
  int fzkvect_l = ft->fzkvect_l;

  ansptr = fzkptr;
  for (yomicnt = end - start; yomicnt < YOMI_L; key[yomicnt++] = 0)
    ;
  for (yomicnt = 0; (yomicnt < YOMI_L) && (start < end); key[yomicnt++] = *start++)
    ;
  key[yomicnt] = 0;
  for (; yomicnt > 0; key[--yomicnt] = NULL)
    {
      search_ptr = (struct fzkentry *) bsrch (key);
      if (search_ptr != NULL)
        {
          setno = setfzk (search_ptr, &ansptr, wv, fzkvect_l);
          ansptr->ent_ptr = NULL;
          return (setno);
        }
    }
  fzkptr->ent_ptr = NULL;
  return (NO);
}

/******************************************/
static int
setfzk (struct fzkentry *entry_ptr,
	struct fzkken **answer_ptr,
	word_vector *wv,
	int fzkvect_l)
{
  int setno;
  int vectroop;
  int wcnt, setflg;
  fzkvect *vect_ptr;

  if (entry_ptr != NULL)
    {
      setno = setfzk (entry_ptr->link, answer_ptr, wv, fzkvect_l);
      for (wcnt = 0; wcnt < fzkvect_l; (*answer_ptr)->vector[wcnt++] = 0);
      setflg = 0;
      for (vectroop = entry_ptr->kosu, vect_ptr = entry_ptr->pter; vectroop > 0; vectroop--, vect_ptr++)
        {
          if (bittest (wv->vector, vect_ptr->no) > 0)
            {
              setflg = 1;
              for (wcnt = 0; wcnt < fzkvect_l; wcnt++)
                (*answer_ptr)->vector[wcnt] |= vect_ptr->vector[wcnt];
            }
        }
      if (setflg != 0)
        {
          (*answer_ptr)->ent_ptr = entry_ptr;
          (*answer_ptr)++;
          return (setno + 1);
        }
      else
        return (setno);
    }
  else
    return (0);
}
#endif /* NO_FZK */

/***************************************************/
static int
bittest (int vector[], int no)
{
  int wvect;

  wvect = vector[no / (sizeof (int) * 8)];
  wvect >>= (int) (no % (sizeof (int) * 8));
  if ((wvect & 0x00000001) == 1)
    return (1);
  else
    return (-1);
}

/*
int Strncmp(w_char *s1, w_char *s2, int n)
{
  if(n == 0)return(0);
  for (;n > 0 && *s1++ == *s2++;n--);
  return (int)(*--s1 - *--s2);
}
*/

#ifndef NO_FZK
static struct fzkentry *
bsrch (w_char *key_yomi)
{
  int low, high, j, flg;

  for (low = 0, high = ft->fzklength; low < high;)
    {
      j = (high + low) >> 1;
      /*
         flg = Strncmp(key_yomi, (ft->tablefuzokugo + j)->yomi, YOMI_L);
       */
      {
        int n;
        w_char *s1, *s2;
        s1 = key_yomi;
        s2 = (ft->tablefuzokugo + j)->yomi;
        for (n = YOMI_L; n > 0 && *s1++ == *s2++; n--);
        flg = (int) (*--s1 - *--s2);
      }
      if (flg > 0)
        low = j + 1;
      else if (flg < 0)
        high = j;
      else
        return (ft->tablefuzokugo + j);
    }
  return (0);
}
#endif

static int
error_fzk (void)
{
  wnn_errorno = WNN_NOT_FZK_FILE;
  log_err ("Bad format in fzk_file.");
  return (-1);
}

static int
error_eof (void)
{
  wnn_errorno = WNN_NOT_FZK_FILE;
  log_err ("Unecpected EOF in reading fzk_file.");
  return (-1);
}

static int
get_decimal (FILE *fp)
{
  unsigned char buf[24];
  int k;
  if (get_string (fp, buf) == EOF)
    {
      return (error_eof ());
    }
  if (sscanf ((char *) buf, "%d", &k) != 1)
    {
      return (error_fzk ());
    }
  return (k);
}

static int
get_hexsa (FILE *fp)
{
  unsigned char buf[24];
  int k;
  if (get_string (fp, buf) == EOF)
    {
      return (error_eof ());
    }
  if (sscanf ((char *) buf, "%x", &k) != 1)
    {
      return (error_fzk ());
    }
  return (k);
}

static int
get_string (FILE *fp, unsigned char *buf)
{
  unsigned char *c = buf;
  int k;
  for (; (k = vgetc (fp)) == ';' || k == '\n' || k == '\t' || k == ' ';)
    {
      if (k == ';')
        {
          for (; (k = vgetc (fp)) != '\n';)
            {
              if (k == EOF)
                {
                  return (EOF);
                }
            }
        }
    }
  if (k == EOF)
    {
      return (EOF);
    }
  vungetc (k, fp);
  for (; (k = vgetc (fp)) != ';' && k != '\n' && k != '\t' && k != ' ' && k != EOF;)
    {
      *c++ = k;
    }
  *c = '\0';
  return (0);                   /* not EOF */
}

static int
check_eof (FILE *fp)
{
  unsigned char buf[24];
  if (get_string (fp, buf) != EOF)
    {
      wnn_errorno = WNN_NOT_FZK_FILE;
      log_err ("Not at the end of fzk_file.");
      return (-1);
    }
  return (0);
}

