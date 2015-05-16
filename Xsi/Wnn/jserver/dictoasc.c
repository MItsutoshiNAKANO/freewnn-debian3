/*
 *  $Id: dictoasc.c $
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

#include <stdio.h>
#include "commonhd.h"
#include "jslib.h"
#include "jh.h"
#include "jdata.h"
#include "de_header.h"

output_entry (jtl, hjtl, kanji1, kanji2, serial, yomi, ofpter)
     struct JT *jtl;
     struct HJT *hjtl;
     int kanji1, kanji2;
     int serial;
     w_char *yomi;
     FILE *ofpter;
{
  w_char comm[LENGTHKANJI];
  w_char kanji[LENGTHKANJI];
  int ima, hindo;
  UCHAR *kptr;

  ima = (hjtl ? hjtl->hindo[serial] : jtl->hindo[serial]) & 0x80;
  hindo = motoni2 ((hjtl ? hjtl->hindo[serial] : jtl->hindo[serial]) & 0x7f);

  kptr = jtl->kanji + kanji1;
  get_kanji_str (kptr, kanji, NULL, comm);

  Print_entry (yomi, kanji, comm, hindo, ima, jtl->hinsi[serial], serial, ofpter);
}

dictoasc (dic_no, ofpter)
     int dic_no;
     FILE *ofpter;
{
  if (((struct JT *) (files[dic_table[dic_no].body].area))->syurui == WNN_UD_DICT)
    {
      return (udtoasc ((struct JT *) (files[dic_table[dic_no].body].area), (dic_table[dic_no].hindo == -1) ? NULL : (struct HJT *) (files[dic_table[dic_no].hindo].area), ofpter));
    }
  else
    {
      return (sdtoasc ((struct JT *) (files[dic_table[dic_no].body].area), (dic_table[dic_no].hindo == -1) ? NULL : (struct HJT *) (files[dic_table[dic_no].hindo].area), ofpter));
    }
}


udtoasc (jtl, hjtl, ofpter)
     struct JT *jtl;
     struct HJT *hjtl;
     FILE *ofpter;
{
  struct uind2 *hop;
  int i, k;
  int len;
  int serial;
  UCHAR *hostart = jtl->hontai;
  struct uind1 *tary = jtl->table;
  static w_char yomi[LENGTHYOMI];       /* 2 byte yomi */

  for (i = 0; i < jtl->maxtable; i++)
    {
      for (hop = (struct uind2 *) (tary[i].pter + (char *) hostart); hop != (struct uind2 *) hostart; hop = (struct uind2 *) (hop->next + (char *) hostart))
        {
/* Though each hop has the same yomi, but calculate yomi each time. */
          yomi[0] = tary[i].yomi1 >> 16;
          yomi[1] = tary[i].yomi1 & 0xffff;
          if (yomi[1])
            {
              yomi[2] = tary[i].yomi2 >> 16;
              if (yomi[2])
                {
                  yomi[3] = tary[i].yomi2 & 0xffff;
                }
            }
          len = hop->yomi[0];
          Strncpy (yomi + 4, (hop->yomi) + 1, len - 4);
          yomi[len] = 0;
          serial = hop->serial;
          for (k = 0; k < hop->kosuu; k++, serial++)
            {
              output_entry (jtl, hjtl, hop->kanjipter, k, serial, yomi, ofpter);
            }
        }
    }
  return (0);
}

sdtoasc (jtl, hjtl, ofpter)
     struct JT *jtl;
     struct HJT *hjtl;
     FILE *ofpter;
{
  return (sdtoasc1 (jtl, hjtl, ofpter, jtl->hontai, 0));
}

sdtoasc1 (jtl, hjtl, ofpter, hopter, level)
     UCHAR *hopter;
     struct JT *jtl;
     struct HJT *hjtl;
     FILE *ofpter;
     int level;
{
  int tsize;
  w_char *charst;
  w_char *sumst;
  int *ptrst;
  int j, k;
  int serial;
  int kanji1;
  static w_char yomi[LENGTHYOMI];       /* 2 byte yomi */

  yomi[level + 1] = 0;
  switch (*(w_char *) hopter)
    {
    case ST_NORMAL:
      tsize = *(w_char *) (hopter + 2);
      serial = *(int *) (hopter + 4);
      kanji1 = *(int *) (hopter + 8);
      charst = (w_char *) (hopter + 12);
      sumst = ((w_char *) charst + tsize + 2);  /* + 2 keeps two zero words */
      ptrst = (int *) ((w_char *) sumst + tsize);
      for (k = 0; k < tsize; k++)
        {
          if (sumst[k] == sumst[k - 1])
            {
              yomi[level] = charst[k];
              for (j = sumst[k - 1] + 1; j <= sumst[k]; j++)
                {
                  output_entry (jtl, hjtl, kanji1, j, serial, yomi, ofpter);
                }
            }
        }
      for (k = 0; k < tsize; k++)
        {
          if (ptrst[k] != ENDPTR)
            {
              yomi[level] = charst[k];
              sdtoasc1 (jtl, hjtl, ofpter, jtl->hontai + ptrst[k], level + 1);
            }
        }
      break;
    case ST_NOENT:
      tsize = *(w_char *) (hopter + 2);
      charst = (w_char *) (hopter + 4);
      ptrst = (int *) AL_INT ((w_char *) charst + tsize);
      for (k = 0; k < tsize; k++)
        {
          if (ptrst[k] != ENDPTR)
            {
              yomi[level] = charst[k];
              sdtoasc1 (jtl, hjtl, ofpter, jtl->hontai + ptrst[k], level + 1);
            }
        }
      break;
    case ST_NOPTER:
      tsize = *(w_char *) (hopter + 2);
      serial = *(int *) (hopter + 4);
      kanji1 = *(int *) (hopter + 8);
      charst = (w_char *) (hopter + 12);
      sumst = ((w_char *) charst + tsize + 2);  /* + 2 keeps two zero words */
      for (k = 0; k < tsize; k++)
        {
          if (sumst[k] == sumst[k - 1])
            {
              yomi[level] = charst[k];
              for (j = sumst[k - 1] + 1; j <= sumst[k]; j++)
                {
                  output_entry (jtl, hjtl, kanji1, j, serial, yomi, ofpter);
                }
            }
        }
      break;
    case ST_SMALL:
      yomi[level] = *(w_char *) (hopter + 2);
      sdtoasc1 (jtl, hjtl, ofpter, hopter + 4, level + 1);
      break;
    }
  return (0);
}
