/*
 * $Id: README.j,v 1.1.2.1 1999/02/08 07:43:31 yamasita Exp $
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
		XJUTIL - 辞書ユーティリティ・マネージャ


このディレクトリには、XJUTILのソースがあります。

XJUTILは、Wnnの辞書を操作する辞書ユーティリティ・マネージャです。
XJUTILは、XWNMOより自動的に起動されます。

XJUTILの処理
	辞書追加:
	辞書一覧:
	登録:
	検索:
	パラメータ変更:
	頻度セーブ:
	辞書情報:
	附属語変更:

マニュアルをインストールする場合、デフォルトは英語のマニュアルです。
もし、日本語のマニュアル(EUC)が表示できるシステムなら、Imakefile の
LOCALMAN を "man.en" から "man.ja" に変更して下さい。

[XJUTIL の作り方]

    xwnmo/README の [XWNMO の作り方] を参照下さい。

[XJUTIL の使い方]

    XWNMO をコンパイル時に、USING_XJUTILをデファインすれば、XWNMOは自動的に
    XJUTILを起動します。何もする必要はありません。
