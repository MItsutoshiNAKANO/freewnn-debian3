;
;   $Id: ./bsd/cvt_key_tbl.kt $
;

;
; FreeWnn is a network-extensible Kana-to-Kanji conversion system.
; This file is part of FreeWnn.
; 
; Copyright Kyoto University Research Institute for Mathematical Sciences
;                 1987, 1988, 1989, 1990, 1991, 1992
; Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
; Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991, 1992
; Copyright FreeWnn Project 1999, 2000
; 
; Maintainer:  FreeWnn Project   <freewnn@tomo.gr.jp>
; 
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
; 
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
; 
; You should have received a copy of the GNU General Public License
; along with this program; if not, write to the Free Software
; Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
;

; key code conversion table for KTERM
;----------------------------------
; ↓、次候補
kd	\x92
; ←
kl	\x91
; →
kr	\x90
; ↑、前候補
ku	\x93
;
; ひらがな/カタカナ PF1 PF25
k1	\x81
FF	\x81
; 全角/半角
k2	\x82
FG	\x82
; ローマ字
k3	\x83
FH	\x83
;
; 辞書操作
k4	\x84
; ローマ字かな 数字全角
k5	\x85
; 罫線 細い
k6	\x86
; 罫線 太い
k7	\x87
;
k8	\x88
k9	\x89
k;	\x8A
;
; 消去
F1	\x8B
;2呼出し
F2	\x8C
;2文節縮め
F3	\x8D 
; 文節伸ばし
F4	\x94
;
; 変換
F5	\x9E
; 確定
F6	\x9F
;
; 登録
F7	\x95
; 辞書操作
F8	\x96
; 区点
F9	\x97
; ＪＩＳ
FA	\x98
;
; 行頭
FB	\x9A
; 消去、逆変換
FC	\x99
; 行末
FD	\x9B
; 全候補、解除
FE	\x9C
