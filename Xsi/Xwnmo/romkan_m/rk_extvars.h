/*
 * $Id: rk_extvars.h,v 1.2.2.1 1999/02/08 05:55:02 yamasita Exp $
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
                        rk_extvars.h
                                                87.11.12  訂 補

        二つ以上のファイルにまたがって使われる変数のextern宣言。
        本体はrk_vars.cで定義。
***********************************************************************/
/*      Version 3.1     88/06/13        H.HASHIMOTO
 */
#ifndef MULTI
extern char rk_errstat;

extern int flags;
extern jmp_buf env0;

extern char *dspmod[][2];

struct modetable
{
  int max;
  int count;
  char **point;
  int size;
  char *org;
  char *next;
};

extern struct modetable rk_defmode;
extern struct modetable rk_dspmode;
extern struct modetable rk_taiouhyo;
extern struct modetable rk_path;

struct modeswtable
{
  int max;
  int count;
  int *point;
};
extern struct modeswtable rk_modesw;

struct modenaibutable
{
  int size;
  int *org;
  int *next;
};
extern struct modenaibutable rk_modenaibu;

struct modebuftable
{
  int size;
  char *org;
};
extern struct modebuftable rk_modebuf;

struct dat
{
  letter *code[3];
};

struct hyo
{
  int hyoshu;
  struct dat *data;
  letter **hensudef;
  int size;
  letter *memory;
};

struct hyotable
{
  int size;
  struct hyo *point;
};
extern struct hyotable rk_hyo;

struct usehyotable
{
  int size;
  int *usemaehyo;
  int *usehyo;
  int *useatohyo;
};
extern struct usehyotable rk_usehyo;

struct hyobuftable
{
  int size;
  char *org;
  char *next;
};
extern struct hyobuftable rk_hyobuf;

struct henikitable
{
  int size;
  letter *org;
  letter *next;
};
extern struct henikitable rk_heniki;

struct hensuset
{
  unsigned regdflg:1;           /* 既登録の変数を表す */
  unsigned curlinflg:1;         /* 現在行に既出の変数を表す */
  letter *name;
  letter *range;
};

struct hensuutable
{
  int max;
  int count;
  struct hensuset *point;
  int size;
  letter *org;
  letter *next;
};
extern struct hensuutable rk_hensuu;

struct hensuudeftable
{
  int max;
  int count;
  letter **point;
};
extern struct hensuudeftable rk_hensuudef;

struct matchpair
{
  int hennum;
  letter ltrmch;
};
struct matchtable
{
  int size;
  struct matchpair *point;
};
extern struct matchtable rk_henmatch;
#endif /* MULTI */

extern letter *lptr;
