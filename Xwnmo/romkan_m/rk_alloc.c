/*
 * $Id: rk_alloc.c,v 1.2 2001/06/14 18:16:09 ura Exp $
 */

/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991, 1992
 *
 * Author: OMRON SOFTWARE Co., Ltd. <freewnn@rd.kyoto.omronsoft.co.jp>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Emacs; see the file COPYING.  If not, write to the
 * Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * Commentary:
 *
 * Change log:
 *
 * Last modified date: 8,Feb.1999
 *
 * Code:
 *
 */
/***********************************************************************
                        rk_alloc.c
                                                88. 6.13  作 成

                モード定義表、対応表の各要素数について、 限界値を
                越えても大丈夫なように、テーブルのサイズを可変長に
                したための新しい関数群
***********************************************************************/
/*      Version 3.1     88/06/14        H.HASHIMOTO
 */
#ifndef OMRON_LIB
#include "rk_header.h"
#include "rk_extvars.h"
#ifdef  MULTI
#include "rk_multi.h"
#endif /*MULTI*/
#endif
#include "rext.h"
  static void
ERRALLOC (n)
     unsigned int n;
{
  fprintf (stderr, "\r\nromkan:  ---\r\n");
  fprintf (stderr, "\tError No.%d: Can't memory alloc.\r\n", n);
  longjmp (cur_rk->env0, 1);
}

/* モード定義表の読み込みのための各種の管理テーブルの領域の獲得 */
#ifdef OMRON_LIB
static
#endif
  void
malloc_for_modetable_struct (ptr, maxsize, memsize)
     fast modetable *ptr;
     int maxsize;
     int memsize;
{
  if ((ptr->max = maxsize) > 0)
    {
      if ((ptr->point = (char **) malloc (ptr->max * sizeof (char *))) == NULL)
        {
          ERRALLOC (0);
        }
      ptr->point[0] = NULL;
    }
  else
    {
      ptr->point = NULL;
    }
  ptr->count = 0;
  if ((ptr->size = maxsize) > 0)
    {
      if ((ptr->org = ptr->next = (char *) malloc (ptr->size)) == NULL)
        {
          ERRALLOC (0);
        }
      ptr->org[0] = '\0';
    }
  else
    {
      ptr->org = ptr->next = NULL;
    }
}

/* モード定義表の読み込みのための各種の管理テーブルの領域の開放 */
#ifdef OMRON_LIB
static
#endif
  void
free_for_modetable_struct (ptr)
     fast modetable *ptr;
{
  if (ptr->point != NULL)
    {
      free (ptr->point);
      ptr->max = ptr->count = 0;
      ptr->point = NULL;
    }
  if (ptr->org != NULL)
    {
      free (ptr->org);
      ptr->size = 0;
      ptr->org = ptr->next = NULL;
    }
}

/* モード定義表の読み込みのための各種の管理テーブルの領域のチェック */
#ifdef OMRON_LIB
static
#endif
  void
check_and_realloc_for_modetable_struct (ptr, n, s, maxsize, memsize)
     fast modetable *ptr;
     int n;
     char *s;
     int maxsize;
     int memsize;
{
  fast int i;
  fast int ptrdiff;
  fast char *oldptr;

  if ((ptr->count = n + 1) > ptr->max)
    {
      if ((ptr->max += maxsize) > 0)
        {
          if (ptr->point == NULL)
            {
              if ((ptr->point = (char **) malloc (ptr->max * sizeof (char *))) == NULL)
                {
                  ERRALLOC (0);
                }
            }
          else
            {
              if ((ptr->point = (char **) realloc (ptr->point, ptr->max * sizeof (char *))) == NULL)
                {
                  ERRALLOC (0);
                }
            }
        }
    }
  while ((ptr->org + ptr->size) < (ptr->next + strlen (s) + 1))
    {
      if ((ptr->org = (char *) realloc (oldptr = ptr->org, ptr->size += memsize)) == NULL)
        {
          ERRALLOC (0);
        }
      ptr->next += (ptrdiff = ptr->org - oldptr);
      for (i = 0; i < n; i++)
        {
          ptr->point[i] += ptrdiff;
        }
    }
}

/* モードの on, off のスイッチテーブルの獲得 */
#ifdef OMRON_LIB
static
#endif
  void
malloc_for_modesw (ptr, size)
     fast modeswtable *ptr;
     int size;
{
  if ((ptr->max = size) > 0)
    {
      if ((ptr->point = (int *) malloc (ptr->max * sizeof (int))) == NULL)
        {
          ERRALLOC (0);
        }
    }
  else
    {
      ptr->point = NULL;
    }
  ptr->count = 0;
}

/* モードの on, off のスイッチテーブルの解放 */
#ifdef OMRON_LIB
static
#endif
  void
free_for_modesw (ptr)
     fast modeswtable *ptr;
{
  if (ptr->point != NULL)
    {
      free (ptr->point);
      ptr->max = ptr->count = 0;
      ptr->point = NULL;
    }
}

/* モードの on, off のスイッチテーブルのサイズチェック */
#ifdef OMRON_LIB
static
#endif
  void
check_and_realloc_for_modesw (ptr, n, size)
     fast modeswtable *ptr;
     int n;
     int size;
{
  if ((ptr->count = n + 1) > ptr->max)
    {
      if ((ptr->max += size) > 0)
        {
          if (ptr->point == NULL)
            {
              if ((ptr->point = (int *) malloc (ptr->max * sizeof (int))) == NULL)
                {
                  ERRALLOC (0);
                }
            }
          else
            {
              if ((ptr->point = (int *) realloc (ptr->point, ptr->max * sizeof (int))) == NULL)
                {
                  ERRALLOC (0);
                }
            }
        }
    }
}

/* モード定義表の読み込みのための領域の獲得 */
#ifdef OMRON_LIB
static
#endif
  void
malloc_for_modebuf (ptr, size)
     fast modebuftable *ptr;
     int size;
{
  if ((ptr->org = (char *) malloc (ptr->size = size + 1)) == NULL)
    {
      ERRALLOC (0);
    }
}

/* モード定義表の読み込みのための領域の解放 */
#ifdef OMRON_LIB
static
#endif
  void
free_for_modebuf (ptr)
     fast modebuftable *ptr;
{
  if (ptr->org != NULL)
    {
      free (ptr->org);
      ptr->size = 0;
      ptr->org = NULL;
    }
}

/* モード表の内部形式のテーブルの獲得 */
#ifdef OMRON_LIB
static
#endif
  void
malloc_for_modenaibu (ptr, size)
     fast modenaibutable *ptr;
     int size;
{
  if ((ptr->org = ptr->next = (int *) malloc ((ptr->size = size + 1) * sizeof (int))) == NULL)
    {
      ERRALLOC (0);
    }
}

/* モード表の内部形式のテーブルのサイズ変更 */
#ifdef OMRON_LIB
static
#endif
  void
realloc_for_modenaibu (ptr)
     fast modenaibutable *ptr;
{
  if ((ptr->org = (int *) realloc (ptr->org, (ptr->size = ptr->next - ptr->org + 1) * sizeof (int))) == NULL)
    {
      ERRALLOC (0);
    }
}

/* モード表の内部形式のテーブルの解放 */
#ifdef OMRON_LIB
static
#endif
  void
free_for_modenaibu (ptr)
     fast modenaibutable *ptr;
{
  if (ptr->org != NULL)
    {
      free (ptr->org);
      ptr->size = 0;
      ptr->org = ptr->next = NULL;
    }
}

/* 対応表の管理テーブルの領域の獲得 */
#ifdef OMRON_LIB
static
#endif
  void
malloc_for_hyo (ptr, size)
     fast hyotable *ptr;
     int size;                  /* 対応表の総数 */
{
  hyo *hyoptr;
  fast int i;

  if ((ptr->size = size) > 0)
    {
      if ((ptr->point = (hyo *) malloc (ptr->size * sizeof (hyo))) == NULL)
        {
          ERRALLOC (0);
        }
      for (i = 0, hyoptr = ptr->point; i < size; i++, hyoptr++)
        {
          hyoptr->hyoshu = 0;
          hyoptr->data = NULL;
          hyoptr->hensudef = NULL;
          hyoptr->size = 0;
          hyoptr->memory = NULL;
        }
    }
  else
    {
      ptr->point = NULL;
    }
}

/* 対応表の管理テーブルの領域の解放 */
#ifdef OMRON_LIB
static
#endif
  void
free_for_hyo (ptr)
     fast hyotable *ptr;
{
  if (ptr->point != NULL)
    {
      free (ptr->point);
      ptr->size = 0;
      ptr->point = NULL;
    }
}

/* 前・本・後処理の対応表の管理テーブルの領域の獲得 */
#ifdef OMRON_LIB
static
#endif
  void
malloc_for_usehyo (ptr, size)
     fast usehyotable *ptr;
     int size;                  /* 対応表の総数 */
{
  if ((ptr->usemaehyo = (int *) malloc (((ptr->size = size) + 1) * sizeof (int) * 3)) == NULL)
    {
      ERRALLOC (0);
    }
  ptr->usehyo = (int *) (ptr->usemaehyo) + ptr->size + 1;
  ptr->useatohyo = (int *) (ptr->usehyo) + ptr->size + 1;
}

/* 前・本・後処理の対応表の管理テーブルの領域の解放 */
#ifdef OMRON_LIB
static
#endif
  void
free_for_usehyo (ptr)
     fast usehyotable *ptr;
{
  if (ptr->usemaehyo != NULL)
    {
      free (ptr->usemaehyo);
      ptr->size = 0;
      ptr->usemaehyo = ptr->usehyo = ptr->useatohyo = NULL;
    }
}

/* 対応表の実データテーブルの領域の獲得 */
#ifdef OMRON_LIB
static
#endif
  void
malloc_for_hyo_area (ptr, n, datasize, memsize, dataptrp, memptrp)
     fast hyotable *ptr;
     fast int n;                /* 対応表の番号 */
     int datasize;
     int memsize;
     dat **dataptrp;
     letter **memptrp;
{
  if ((*dataptrp = ptr->point[n].data = (dat *) malloc ((datasize + 1) * sizeof (dat))) == NULL)
    {
      ERRALLOC (0);
    }
  if ((*memptrp = ptr->point[n].memory = (letter *) malloc ((ptr->point[n].size = memsize + 1) * sizeof (letter))) == NULL)
    {
      ERRALLOC (0);
    }
}

/* 対応表の実データテーブルの領域の獲得 */
#ifdef OMRON_LIB
static
#endif
  void
free_for_hyo_area (ptr, n)
     fast hyotable *ptr;
     fast int n;                /* 対応表の番号 */
{
  if (ptr->point[n].data != NULL)
    {
      free (ptr->point[n].data);
      ptr->point[n].data = NULL;
    }
  if (ptr->point[n].memory != NULL)
    {
      free (ptr->point[n].memory);
      ptr->point[n].memory = NULL;
    }
}

/* 全対応表の変数の管理テーブルの獲得 */
#ifdef OMRON_LIB
static
#endif
  void
malloc_for_hensuudef (ptr, size)
     fast hensuudeftable *ptr;
     int size;
{
  if ((ptr->max = size) > 0)
    {
      if ((ptr->point = (letter **) malloc (ptr->max * sizeof (letter *))) == NULL)
        {
          ERRALLOC (0);
        }
    }
  else
    {
      ptr->point = NULL;
    }
  ptr->count = 0;
}

/* 全対応表の変数の管理テーブルの解放 */
#ifdef OMRON_LIB
static
#endif
  void
free_for_hensuudef (ptr)
     fast hensuudeftable *ptr;
{
  if (ptr->point != NULL)
    {
      free (ptr->point);
      ptr->max = ptr->count = 0;
      ptr->point = NULL;
    }
}

/* 全対応表の変数の管理テーブルの解放 */
#ifdef OMRON_LIB
static
#endif
  void
check_and_realloc_for_hensuudef (ptr, n, size)
     fast hensuudeftable *ptr;
     int n;
     int size;
{
  if ((ptr->count = n + 1) > ptr->max)
    {
      if ((ptr->max += size) > 0)
        {
          if (ptr->point == NULL)
            {
              if ((ptr->point = (letter **) malloc (ptr->max * sizeof (letter *))) == NULL)
                {
                  ERRALLOC (0);
                }
            }
          else
            {
              if ((ptr->point = (letter **) realloc (ptr->point, ptr->max * sizeof (letter *))) == NULL)
                {
                  ERRALLOC (0);
                }
            }
        }
    }
}

/* 対応表の変数の管理テーブルの獲得 */
#ifdef OMRON_LIB
static
#endif
  void
malloc_for_hensuu (ptr, maxsize, memsize)
     fast hensuutable *ptr;
     int maxsize;
     int memsize;
{
  if ((ptr->max = maxsize) > 0)
    {
      if ((ptr->point = (hensuset *) malloc (ptr->max * sizeof (hensuset))) == NULL)
        {
          ERRALLOC (0);
        }
    }
  else
    {
      ptr->point = NULL;
    }
  ptr->count = 0;
  if ((ptr->org = ptr->next = (letter *) malloc (((ptr->size = memsize) + 1) * sizeof (letter))) == NULL)
    {
      ERRALLOC (0);
    }
}

/* 対応表の変数の管理テーブルの開放 */
#ifdef OMRON_LIB
static
#endif
  void
free_for_hensuu (ptr)
     fast hensuutable *ptr;
{
  if (ptr->point != NULL)
    {
      free (ptr->point);
      ptr->max = ptr->count = 0;
      ptr->point = NULL;
    }
  if (ptr->org != NULL)
    {
      free (ptr->org);
      ptr->size = 0;
      ptr->org = ptr->next = NULL;
    }
}

/* 対応表の変数の管理テーブルのサイズチェック */
#ifdef OMRON_LIB
static
#endif
  void
check_and_realloc_for_hensuu (ptr, n, name, maxsize, memsize)
     fast hensuutable *ptr;
     int n;
     letter *name;
     int maxsize;
     int memsize;
{
  fast int i;                   /* V3.1 */
  fast int ptrdiff;             /* V3.1 */
  fast letter *oldptr;          /* V3.1 */

  if ((ptr->count = n + 1) > ptr->max)
    {
      if ((ptr->max += memsize) > 0)
        {
          if (ptr->point == NULL)
            {
              if ((ptr->point = (hensuset *) malloc (ptr->max * sizeof (hensuset))) == NULL)
                {
                  ERRALLOC (0);
                }
            }
          else
            {
              if ((ptr->point = (hensuset *) realloc (ptr->point, ptr->max * sizeof (hensuset))) == NULL)
                {
                  ERRALLOC (0);
                }
            }
        }
    }
  while ((ptr->org + ptr->size) < (ptr->next + (ltrlen (name) + 1 + 1)))
    {

      if ((ptr->org = (letter *) realloc (oldptr = ptr->org, ((ptr->size += memsize) + 1) * sizeof (letter))) == NULL)
        {
          ERRALLOC (0);
        }
      ptr->next += (ptrdiff = ptr->org - oldptr);
      for (i = 0; i < n; i++)
        {
          ptr->point[i].name += ptrdiff;
        }
    }
}

/* マッチした変数の管理テーブルの獲得 */
#ifdef OMRON_LIB
static
#endif
  void
malloc_for_henmatch (ptr, size)
     fast matchtable *ptr;
     int size;                  /* 変数の総数 */
{
  if ((ptr->size = size) > 0)
    {
      if ((ptr->point = (matchpair *) malloc (ptr->size * sizeof (matchpair))) == NULL)
        {
          ERRALLOC (0);
        }
    }
  else
    {
      ptr->point = NULL;
    }
}

/* マッチした変数の管理テーブルの解放 */
#ifdef OMRON_LIB
static
#endif
  void
free_for_henmatch (ptr)
     fast matchtable *ptr;
{
  if (ptr->point != NULL)
    {
      free (ptr->point);
      ptr->size = 0;
      ptr->point = NULL;
    }
}

/* 対応表を読み込むバッファの領域の獲得 */
#ifdef OMRON_LIB
static
#endif
  void
malloc_for_hyobuf (ptr, size)
     fast hyobuftable *ptr;
     int size;
{
  if ((ptr->size = size) > 0)
    {
      if ((ptr->org = ptr->next = (char *) malloc (ptr->size)) == NULL || (ptr->tmp = (char *) malloc (ptr->size)) == NULL)
        {
          ERRALLOC (0);
        }
    }
  else
    {
      ptr->org = NULL;
    }
}

/* 対応表を読み込むバッファの領域の開放 */
#ifdef OMRON_LIB
static
#endif
  void
free_for_hyobuf (ptr)
     fast hyobuftable *ptr;
{
  if (ptr->org != NULL)
    {
      free (ptr->org);
      ptr->size = 0;
      ptr->org = ptr->next = NULL;
    }
}

/* 対応表を読み込むバッファの領域のサイズチェック */
#ifdef OMRON_LIB
static
#endif
  void
check_and_realloc_for_hyobuf (ptr, size)
     fast hyobuftable *ptr;
     int size;
{
  /* 対応表の読み込み */
  if ((size + 2) > ptr->size)
    {
      if (ptr->org == NULL)
        {
          if ((ptr->org = (char *) malloc (ptr->size = size + 2)) == NULL || (ptr->tmp = (char *) malloc (ptr->size = size + 2)) == NULL)
            {
              ERRALLOC (0);
            }
        }
      else
        {
          if ((ptr->org = (char *) realloc (ptr->org, ptr->size = size + 2)) == NULL || (ptr->tmp = (char *) realloc (ptr->tmp, ptr->size = size + 2)) == NULL)
            {
              ERRALLOC (0);
            }
        }
    }
}

/* 変数の変域のワーク領域の獲得 */
#ifdef OMRON_LIB
static
#endif
  void
malloc_for_heniki (ptr, size)
     fast henikitable *ptr;
     int size;
{
  if ((ptr->org = (letter *) malloc ((ptr->size = size + 1) * sizeof (letter))) == NULL)
    {
      ERRALLOC (0);
    }
}

/* 変数の変域のワーク領域の開放 */
#ifdef OMRON_LIB
static
#endif
  void
free_for_heniki (ptr)
     fast henikitable *ptr;
{
  if (ptr->org != NULL)
    {
      free (ptr->org);
      ptr->size = 0;
      ptr->org = NULL;
    }
}

/* 変数の変域のワーク領域のサイズチェック */
#ifdef OMRON_LIB
static
#endif
  void
check_and_realloc_for_heniki (ptr, size)
     fast henikitable *ptr;
     int size;
{
  if ((size + 1) > ptr->size)
    {
      if (ptr->org == NULL)
        {
          if ((ptr->org = (letter *) malloc ((ptr->size = size + 1) * sizeof (letter))) == NULL)
            {
              ERRALLOC (0);
            }
        }
      else
        {
          if ((ptr->org = (letter *) realloc (ptr->org, (ptr->size = size + 1) * sizeof (letter))) == NULL)
            {
              ERRALLOC (0);
            }
        }
    }
}
