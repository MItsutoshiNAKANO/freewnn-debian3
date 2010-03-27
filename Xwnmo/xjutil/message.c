/*
 * $Id: message.c,v 1.2 2001/06/14 18:16:13 ura Exp $
 */

/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
 * Copyright 1991 by Massachusetts Institute of Technology
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

char *default_message[] = {
/* 0 */ "PREVIOUS",
/* 1 */ "Select Hinsi",
/* 2 */ "  CANCEL  ",
/* 3 */ "  NEXT  ",
/* 4 */ "PREVIOUS",
/* 5 */ " YES  ",
/* 6 */ "  NO  ",
/* 7 */ " OK ? ",
/* 8 */ "Exec",
/* 9 */ "Change para",
/* 10 */ "Bunsetsu number",
/* 11 */ "Max of small Bunsetsu",
/* 12 */ "Frequency weight",
/* 13 */ "Small Bunsetsu length weight",
/* 14 */ "Jiritsu word weight",
/* 15 */ "Recnetly used weight",
/* 16 */ "Jisho priority weight",
/* 17 */ "Evaluation value weight",
/* 18 */ "Large Bunsetsu length weight",
/* 19 */ "small Bunsetsu number weight",
/* 20 */ "Frequency of numeral",
/* 21 */ "Frequency of Kana",
/* 22 */ "Frequency of ascii",
/* 23 */ "Frequency of mark",
/* 24 */ "Frequency of )",
/* 25 */ "Frequency of Fuzokugo",
/* 26 */ "Frequency of (",
/* 27 */ "Recently,",
/* 28 */ "Bunsetsu number.",
/* 29 */ "How number do you use?",
/* 30 */ "Recently, ",
/* 31 */ " is ",
/* 32 */ ". ",
/* 33 */ "How number do you use? ",
/* 34 */ "Please, input a number.",
/* 35 */ "Please, input a plus number.",
/* 36 */ "Search Dictionary",
/* 37 */ "Delete Dictionary",
/* 38 */ "Use/Unuse",
/* 39 */ "Comment",
/* 40 */ "Sorry, number of dictionaries is too large.",
/* 41 */ "List of Dictonaries",
/* 42 */ "There is no dictionary.",
/* 43 */ "Use",
/* 44 */ "Unuse",
/* 45 */ "Change Fuzokugo",
/* 46 */ "Please, input Fuzokugo file name.",
/* 47 */ "Add Dictionary",
/* 48 */ "Nxext",
/* 49 */ "Please, input new dictionary name.",
/* 50 */ "Please, input frequency file name.",
/* 51 */ "Please, input number of dictionary priority.",
/* 52 */ "Do you use this dictionary as update mode.?",
/* 53 */ "Do you use this dictionary's frequency as update mode.?",
/* 54 */ "No.",
/* 55 */ "Type",
/* 56 */ "Comment",
/* 57 */ "File name",
/* 58 */ "Frequency file name",
/* 59 */ "Number of words",
/* 60 */ "Priority",
/* 61 */ "Enable register",
/* 62 */ "Enable update frequency",
/* 63 */ "Use/Unuse",
/* 64 */ "Password of register",
/* 65 */ "Password of update frequency",
/* 66 */ "There isn't this.",
/* 67 */ "Delete",
/* 68 */ "Use/Unuse",
/* 69 */ "Search",
/* 70 */ "Yomi",
/* 71 */ "There isn't this word.",
/* 72 */ "Can't open default uumkey.",
/* 73 */ "can't be expanded.",
/* 74 */ "is duplicate definition at key",
/* 75 */ "Malloc Error!!!",
/* 76 */ "¡ÖHOME¡× is wrong.",
/* 77 */ "Dictionary",
/* 78 */ " will be removed.",
/* 79 */ " will become disable.",
/* 80 */ " will become enable.",
/* 81 */ "Please, input some comment.",
/* 82 */ "You can't delete, because the dictionary is read-only.",
/* 83 */ "You can't delete entry, because the dictionary is read-only.",
/* 84 */ "{",
/* 85 */ "} will be remove.",
/* 86 */ "You can't change, because the frequency is read-only.",
/* 87 */ "} will become enable.",
/* 88 */ "} will become disable.",
/* 89 */ "You can't comment, because the dictionary is read-only.",
/* 90 */ "",
/* 91 */ "Please, input some comment.",
/* 92 */ "REGISTER",
/* 93 */ "Start of register:",
/* 94 */ "End of register:",
/* 95 */ "Yomi:",
/* 96 */ "Hinsi:",
/* 97 */ "Dictionary:",
/* 98 */ " Dictionary ",
/* 99 */ "There is no registerable dictionary.",
/* 100 */ "Range of register is too long.",
/* 101 */ "Can't open default uumrc.",
/* 102 */ "Dictionary file",
/* 103 */ "doesn't exist.",
/* 104 */ "Would you like to create it?",
/* 105 */ "Frequency file",
/* 106 */ "Dictionary",
/* 107 */ "is not consistent with the frequency. Would you like to re-create it?",
/* 108 */ "Password file",
/* 109 */ "was created.",
/* 110 */ "Directory",
/* 111 */ "  MOUSE   ",
/* 112 */ "   KEY    ",
/* 113 */ "JIS code input",
/* 114 */ "Please input JIS code",
/* 115 */ "KUTEN input",
/* 116 */ "Please input KUTEN",
/* 117 */ "KU",
/* 118 */ "You can't set frequency, because the frequency is read-only.",
/* 119 */ "Recentry, the frequency is ",
/* 120 */ "Please, input a valid number.",
/* 121 */ "Change frequency",
/* 122 */ "Turn on the Recnetly used bit",
/* 123 */ "Turn off the Recnetly used bit",
/* 124 */ "Add one to the frequency",
/* 125 */ "Take one from the frequency",
/* 126 */ "Disable this entry",
/* 127 */ "Change the frequency as value",
};
