;
;   $Id: ./sysV/cvt_key_tbl.ST $
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

;
; key code conversion table for ST880S, ST141, PIC80I
;----------------------------------
; ¢­¡¢¼¡¸õÊä
kcud1	\x92
; ¢«
kcub1	\x91
; ¢ª
kcuf1	\x90
; ¢¬¡¢Á°¸õÊä
kcuu1	\x93
;
; ¤Ò¤é¤¬¤Ê/¥«¥¿¥«¥Ê
kf1	\x81
; Á´³Ñ/È¾³Ñ
kf2	\x82
; ¥í¡¼¥Þ»ú
kf3	\x83
;
; ¼­½ñÁàºî
kf4	\x84
;
khome	\x8A
