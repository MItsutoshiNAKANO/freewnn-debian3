/*
 * $Id: README.j,v 1.3.2.1 1999/02/08 08:14:41 yamasita Exp $
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
		XWNMO - Ｘ・インプット・マネージャ
	     (+ XJUTIL - 辞書ユーティリティ・マネージャ)

このディレクトリには、XWNMOのソース、XJUTILのソース、および、マルチ・オート
マトンのソースがあります。

	XWNMO - XIM(X Input Method)のためのインプット・マネージャ
	XJUTIL - (XWNMOから起動される)辞書ユーティリティ・マネージャ

XWNMOは、XIMライブラリに対して多言語の入力を提供するインプット・マネージャです。
XWNMOとXIMライブラリ間のプロトコルは、X11R5の場合は Xsi Protocol により
規定されております。この Xsi Protocol は、バックエンド方式を採用しています。
なぜなら、バックエンド方式は、完全な同期通信であるため、キーの取りこぼしの
問題がないからです。X11R6の場合は XIM Protocol により規定されています。
この XIM Protocol は、X Consortium Standard です。XWNMOは、XIM Protocolの
バックエンド方式のみを採用しています。

これらについての詳細は、
	xwnmo/README と xwnmo のマニュアル	XWNMO について
	xjutil/README と xjutil のマニュアル	XJUTIL について
	X11R5/doc/I18N/Xsi/Xim/XimProto.man	Xsi Protocol について
	X11R6/doc/specs/XIM/xim.ms		XIM Protocol について
を参照下さい。

インプット・マネージャとXIMライブラリの構成

 +------------------------+         +----------------+  +----------------+
 |     Application        |     +---+     XWNMO      +--+     XJUTIL     |
 |   +--------------------+     |   +-------+--------+  +-------+--------+
 |   |     Widgets        |     |           |                   |
 |   +--------------------+     |           +-------------------+
 |   |      Toolkit       |     |           |
 +---+--------------------+     |           |   +---------+
 |       +------------+   |     |           +---+ Cserver |
 | Xlib  | XIM library+---+-----+           |  +---------++
 |       +------------+   |                 +--+ Jserver |
 +------------------------+                    +---------+


[X11R6での動作について]
・ロカール名について
  XWNMOは内部のロカールとして、EUCのサブセットである
      ja_JP.eucJP(日本語EUC)
      zh_CN.eucCN(中国語（簡体字）EUC)
      zh_TW.eucTW(中国語（繁体字）EUC)
      ko_KR.eucKR(韓国語EUC)
  を使用しています。しかし、OSによっては、同じ内容のロカールであっても名前が
  ことなるものがあるますので、上記のもの以外の場合は、ximconfを変更する必要
  があります。

・中国語の入力について
  XWNMO(cWnn)では中国語の処理で独自のコードセットであるSishengというものを
  使用しています。これを表示するために、Sisheng用のフォントを用意しました。
  Xsi/Xwnmo/X11R6/fontsの下に、26,24,18,16ドットのものがありますので、必要
  に応じてインストールして使用してください。また、ロカールデータベースの変更
  も必要です。Xsi/Xwnmo/X11R6/locale/zh を xc/nls/X11/locale/XLC_LOCALE/zh
  と置き換えて、コンパイルしなおして使用してください。

Mr. Bill McMahon (Hewlett Packard) に感謝の意を表します。彼には、XWNMOの移植性、
および、信頼性に大いに貢献していただきました。

