/*
 * $Id: SEP_README.j,v 1.1.2.1 1999/02/08 08:08:36 yamasita Exp $
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
 */
	SEP - SpotLocation拡張プロトコル

SEPは、スポットロケーションの高速移動を実現するプロトコルです。

通常、アプリケーションにおけるカーソル移動の処理は以下ようになります。

	nested_list = XVaCreateNestedList(dummy, XNSpotLocation, spot NULL);
	XSetICValues(ic, XNPreeditAttributes, nested_list, NULL);
	XFree(dpy, nested_list);

ターミナルエミュレータのようなアプリケーションの場合、頻繁にカーソルを移動する
必要があります。それゆえ、このような処理は、アプリケーションの性能を著しく落して
しまいます。
SEPは、この問題を解決します。アプリケーションは、SEPを使うか、以下の関数を呼ぶ
だけです。

	_XipChangeSpot(ic, spot_x, spot_y);

_XipChangeSpot()の詳細は、xwnmoのマニュアルのSEP部分を参照下さい。

-------------------------------------------------------------------------
XIMライブラリとXWNMO間のソケットを使った通信データ

    要求
	バイト数	値		内容
    XIM -> XWNMO (ximChangeSpotReq)
	1	    XIM_ChangeSpot(20)	リクエストタイプ
	1				未使用
	2		12		データの長さ
	4		CARD32		xic
	2		INT16		spot location x
	2		INT16		spot location y

    応答
	バイト数	値		内容
    XIM <- XWNMO (sz_ximEventReply)
	2				応答
			0		 正常
			-1		 異常
	6				未使用

