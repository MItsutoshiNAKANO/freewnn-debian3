/*
 * $Id: rk_macros.h,v 1.2.2.1 1999/02/08 05:55:04 yamasita Exp $
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
                        rk_macros.h
                                                87.11.17  改 正

    本変換で使っているマクロ関数群。rk_header.h にincludeされている。
    これをincludeすると、自動的にrk_spclval.hもincludeされる。ユーザの
    プログラムでこれをincludeすれば、to_upperなどのマクロが使える。 
***********************************************************************/
/*      Version 3.0
 */
#ifndef RKMCRO

#define RKMCRO
#include "rk_spclval.h"         /* ctype.hを使わないマクロは、この中で定義してある */

#ifndef MVUX
#       include <ctype.h>
#endif

        /* マクロ関数群（引数を複数回評価するものも多いので注意） */

 /* 7ビットコードchar用マクロ */
#define isoctal(c) (isdigit(c) && (c) < '8')    /* 8進の数字か */
#define isnulsp(c) (isspace(c) || (c) == '\0')  /* EOL又は空白文字であるか */

  /* 大文字←→小文字変換。定義域は
     SYSVR2定義時  toupdown   7ビットchar（isasciiの成り立つ範囲）
     _toupdown  英文字（isalphaの成り立つ範囲）
     その他の場合   toupdown                〃                          */
#ifdef SYSVR2
#       define _toupdown(c) (isupper(c) ? _tolower(c) : _toupper(c))
#       define toupdown(c) (isupper(c) ? _tolower(c) : toupper(c))
#else
#       define toupdown(c) (isupper(c) ? tolower(c) : toupper(c))
#endif

 /* ctypeマクロの定義域を拡張したもの。letter型引き数にも適用可。
    is_eolspだけは独自のもの。 */
/*
#ifdef OMRON
#define is_lower(l) (islower(l))
#define is_upper(l) (isupper(l))
#define is_alpha(l) (isalpha(l))
#define is_alnum(l) (isalnum(l))
#define is_digit(l) (isdigit(l))
#define is_octal(l) (isoctal(l))
#define is_xdigit(l) (isxdigit(l))
#define is_space(l) (isspace(l))
#define is_cntrl(l) (iscntrl(l))
#define is_nulsp(l) (isnulsp(l))
#else
*/
#define is_lower(l) (isascii(l) && islower(l))
#define is_upper(l) (isascii(l) && isupper(l))
#define is_alpha(l) (isascii(l) && isalpha(l))
#define is_alnum(l) (isascii(l) && isalnum(l))
#define is_digit(l) (isascii(l) && isdigit(l))
#define is_octal(l) (isascii(l) && isoctal(l))
#define is_xdigit(l) (isascii(l) && isxdigit(l))
#define is_space(l) (isascii(l) && isspace(l))
#define is_cntrl(l) (isascii(l) && iscntrl(l))
#define is_nulsp(l) (isascii(l) && isnulsp(l))
/*
#endif
*/
#define is_eolsp(l) (is_space(l) || (l) == EOLTTR)
#ifdef SYSVR2
#       define  to_upper(l) (is_lower(l) ? _toupper(l) : (l))
#       define  to_lower(l) (is_upper(l) ? _tolower(l) : (l))
#       define  to_updown(l) (is_alpha(l) ? _toupdown(l) : (l))
#else
#       define  to_upper(l) (is_lower(l) ? toupper(l) : (l))
#       define  to_lower(l) (is_upper(l) ? tolower(l) : (l))
#       define  to_updown(l) (is_alpha(l) ? toupdown(l) : (l))
#endif

#endif /* RKMCRO */
