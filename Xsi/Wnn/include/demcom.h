/*
 *  $Id: demcom.h $
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

/*
        demcom.h
        entry functions definitions
*/

/*
 *      Demon Commands
 */
#define JS_VERSION      0x00
#define JS_OPEN         0x01
/*      #define JS_OPEN_IN      0x02    */
#define JS_CLOSE        0x03
#define JS_CONNECT      0x05
#define JS_DISCONNECT   0x06
#define JS_ENV_EXIST    0x07
#define JS_ENV_STICKY   0x08
#define JS_ENV_UN_STICKY        0x09


#define JS_KANREN       0x11
#define JS_KANTAN_SHO   0x12
#define JS_KANZEN_SHO   0x13
#define JS_KANTAN_DAI   0x14
#define JS_KANZEN_DAI   0x15
#define JS_HINDO_SET    0x18


#define JS_DIC_ADD      0x21
#define JS_DIC_DELETE   0x22
#define JS_DIC_USE      0x23
#define JS_DIC_LIST     0x24
#define JS_DIC_INFO     0x25

#define JS_FUZOKUGO_SET 0x29
#define JS_FUZOKUGO_GET 0x30


#define JS_WORD_ADD     0x31
#define JS_WORD_DELETE  0x32
#define JS_WORD_SEARCH  0x33
#define JS_WORD_SEARCH_BY_ENV   0x34
#define JS_WORD_INFO    0x35
#define JS_WORD_COMMENT_SET 0x36

#define JS_PARAM_SET    0x41
#define JS_PARAM_GET    0x42

#define JS_MKDIR        0x51
#define JS_ACCESS       0x52
#define JS_WHO          0x53
#define JS_ENV_LIST     0x55
#define JS_FILE_LIST_ALL        0x56
#define JS_DIC_LIST_ALL 0x57

#define JS_FILE_READ    0x61
#define JS_FILE_WRITE   0x62
#define JS_FILE_SEND    0x63
#define JS_FILE_RECEIVE 0x64

#define JS_HINDO_FILE_CREATE    0x65
#define JS_DIC_FILE_CREATE      0x66
#define JS_FILE_REMOVE  0x67

#define JS_FILE_LIST    0x68
#define JS_FILE_INFO    0x69
#define JS_FILE_LOADED  0x6A
#define JS_FILE_LOADED_LOCAL    0x6B
#define JS_FILE_DISCARD 0x6C
#define JS_FILE_COMMENT_SET 0x6D
#define JS_FILE_PASSWORD_SET 0x6E       /* 89/9/8 */

#define JS_FILE_STAT    0x6F
#define JS_KILL         0x70

#define JS_HINDO_FILE_CREATE_CLIENT     0x71
#define JS_HINSI_LIST                   0x72
#define JS_HINSI_NAME   0x73
#define JS_HINSI_NUMBER 0x74
#define JS_HINSI_DICTS  0x75
#define JS_HINSI_TABLE_SET 0x76
