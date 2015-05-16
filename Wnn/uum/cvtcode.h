/*
 *  $Id: cvtcode.h,v 1.3 2001/06/14 18:16:06 ura Exp $
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

#define ctolc(c)        ((short)(unsigned char)(c))     /* chat to long_char */
                                                   /* where char is ascii */
#define lctoc(c)        ((unsigned char)(c))    /* lonag_char to char */
                                                   /* where char is ascii */

#define SS2_CODE        (0x8e)
#define SS3_CODE        (0x8f)

extern long_char *strtoStr ();  /* string to long string */
extern unsigned char *Strtostr ();      /* long string to string */
extern int Charlen ();          /* kanji code char length */
