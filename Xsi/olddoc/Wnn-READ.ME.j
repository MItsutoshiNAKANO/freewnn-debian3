/*
 * $Id: READ.ME.j,v 1.10.2.1 1999/02/08 02:15:23 yamasita Exp $
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

●	Ｗｎｎ仮名漢字変換システム バージョン4.2

Ｗｎｎ仮名漢字変換システムは、京都大学数理解析研究所、オムロン株式会社、
株式会社アステックの３者によって開発されたネットワーク仮名漢字変換シス
テムです。

これは、Ｗｎｎ仮名漢字変換システムのバージョン4.2です。

今回のリリースでは、以下の機械で動作が確認されています。

SUN の SunOS 4.0.3、 SunOS 4.1.1、 SunOS 5.3

OMRON LUNA の UniOS-B (BSD4.3)、UniOS-U (SystemV R2.1)、UniOS-Mach (MACH)
              DG/UX 5.4.2



●	インストール

 ソースファイルには、リテラル文字列、および、コメントが、ECUコードで書かれて
 います。コンパイルの際には、8ビットが通るコンパイラで、make して下さい。

 ・Imakeでコンパイル
    (注)コンパイルを実行する前に、下記の「コンパイルの前に」をお読み下さい。

    (X11R6ソースツリーの場合)
      Xsiをcontrib/programs/Xsiとして置いた上で、
	% cd ..
	% make World -f Makefile.ini

    (X11R6がインストールされている場合)
      Xsiを好きなディレクトリに置いた上で、
	% cd ..
	% make World -f Makefile.inst

    (X11R5の場合)
      Xsiをcontrib/im/Xsiとして置いた上で、
	% cd ..
	% make World -f Makefile.R5ini

 ・Imakeでインストール

	% make install

 
[コンパイルの前に]

 Project.tmpl ($(TOP)/contrib/im/Xsi/configにある) に、インストールする
 ディレクトリに変数がいくつかあります(なお、X11R5の場合は、Project.tmpl
 は、$(TOP)/contrib/im/Xsi/configR5になります)。それぞれの意味は以下の
 通りです。
 
・コンパイルの環境

IMTOP = $(CONTRIBSRC)/programs/Xsi
   Wnnを置くソースツリー上の位置を指定します。

・インストールする部分を指定する変数
       変数                  インストールされるもの
----------------------------------------------------------
#define BuildWnn	       日本語のWnnの部分
#define BuildCWnn	       中国語のWnnの部分
#define BuildKWnn	       韓国語のWnnの部分
#define BuildXwnmo	       Xwnmo
#define BuildJserver	       日本語変換サーバ
#define BuildJlib	       Wnnのライブラリ
#define BuildWnnClients	       日本語用のクライアント
#define BuildWnnDicUtil	       日本語用の辞書コマンド
#define BuildPubdic	       日本語用の辞書(Pubdic)
#define BuildWnnConsDic	       日本語用の単漢字辞書
#define BuildWnnMan	       オンラインマニュアル
#define BuildJlibV3	       WnnV3互換用ライブラリ
#define BuildCserver	       中国語（簡体字）変換サーバ
#define BuildTserver	       中国語（繁体字）変換サーバ
#define BuildCWnnClients       中国語用のクライアント
#define BuildCWnnDicUtil       中国語用の辞書コマンド
#define BuildCDic	       中国語（簡体字）用の辞書
#define BuildTDic	       中国語（繁体字）用の辞書
#define BuildKserver	       韓国語変換サーバ
#define BuildKWnnClients       韓国語用のクライアント
#define BuildKWnnDicUtil       韓国語用の辞書コマンド
#define BuildKDic	       韓国語用の辞書

・インストールの場所
WNNWNNDIR = /usr/local/lib/wnn 設定ファイル、辞書などのインストール位置
WNNBINDIR = /usr/local/bin     コマンド類のインストール位置
                               このディレクトリの下にWnn4,cWnn4などのディ
                               レクトリが作成され、そこにインストールされる。
WNNINCDIR = $(INCROOT)         ヘッダファイルのインストール位置
WNNLIBDIR = $(USRLIBDIR)       Wnnのライブラリのインストール位置

 インストールの環境を変更したい場合は、この Project.tmpl を編集します。

 また、Project.tmpl ($(TOP)/contrib/im/Xsi/configにある) に 変数 WNNOWNER
 があります(なお、X11R5の場合は、Project.tmpl は、
 $(TOP)/contrib/im/Xsi/configR5になります)。
 これは、Wnn のプログラム(uum を除く) をどのユーザの権限で動かすか指定
 します。バージョン3まででは、jserver は、root の権限で動かしていたので
 すが、root で動かすのは危険性が高いため、wnn 用のユーザを作って、その
 権限で jserver だけは動かすこ とを推奨します。
 デフォルトでは、wnn になっています。

 uum は、オーナが root で、 sビットが立っている必要があります。

 すでに辞書がインストールされている環境にインストールした場合、以前の
 頻度ファイルが使用できなくなります。そのため、インストール先に辞書が
 すでに存在する場合は、辞書のインストールは行なわれないようになっています。
 もし、強制的に辞書を上書きしたい場合は、Project.tmpl あるいは他のconfig
 ファイルに下記の行を追加して下さい。

	#define WnnInstallDicForcibly	YES
     ( #の前に空白、TABを入れないで下さい。)
 

 使用するOSがファンクションキーを10より多くサポートしている場合は、
 Wnn/include/commonhd.hに下記の行を追加して下さい。

	#define SUPPORT_TWODIGIT_FUNCTIONS
     ( #の前に空白、TABを入れないで下さい。)

 以上[コンパイルの前に]終り

●	manual 以外のディレクトリは、以下の通りです。

conv
	コンバート・キー(ファンクションキーの吐くコード列を、適当なコー
	ドに変換する)のソースが存在します。
etc
	複数のディレクトリで共有されるソースファイルが存在します。
include 
	ヘッダーファイルが存在します。
jd
	uum, jserver, ローマ字仮名変換などの初期化ファイル、
	品詞ファイルが存在します。このディレクトリの内容は、
	/usr/local/lib/wnn/ja_JP にインストールされます。

	    jserverrc - jserver の立上りの設定ファイルです。
	    uumrc	uum の立上りの設定ファイルです。
	    uumkey	uum のキーバインド設定ファイルです。
	    wnnenvrc	uum 使用時の辞書等の設定ファイルです。
	jd の下には、いろいろな設定のものが用意されています。
	好みに合わせて御利用下さい。

jlib
	ライブラリのソースが存在します。
jlib.V3
	バージョン3に対する互換ライブラリのソースが存在します。
jserver
	jserver のソースが存在します。
jutil
	辞書ユーティリティのソースが存在します。
		atod	 辞書の作成
		dtoa	 辞書のテキストへの変更
		oldatonewa バージョン３までの辞書のテキスト形式を、
			バージョン４のテキスト形式に変更
		wnnstat	 jserver の状態(ユーザ、環境、ファイル、辞書)を調べる。
		wnnkill	 jserver を終了させる。
		wnntouch 辞書ファイルなどのファイルのFID が、ファイルのヘッダに
			持っているものと異なる時に一致させる。
		wddel	単語削除をバッチ的に行なう
		wdreg	単語登録をバッチ的に行なう
pubdic 			
	pubdic の辞書が存在します。
	辞書は、 逆引き形式で作ってあります。
romkan
	ローマ字仮名変換のソースが存在します。
uum
	フロントエンド・プロセッサ uum のソースが存在します。
wnncons
	Wnnコンソシアムよりの提供物が存在します。
