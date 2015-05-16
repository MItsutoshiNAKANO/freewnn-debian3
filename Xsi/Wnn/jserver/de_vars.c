/*
 *  $Id: de_vars.c $
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

#include "commonhd.h"
#include "de_header.h"

/*
        external global variables of demon
*/

char SER_VERSION[] = _SERVER_VERSION;

int max_client = CL_MAX;
CLIENT *client;                 /* ask about Mr. Takeoka */
/*CLIENT client[CL_MAX];*//* ask about Mr. Takeoka */
CLIENT *c_c = 0;                /* this means current client */
int cur_client;
int clientp;

int max_sticky_env = ST_MAX;
struct cnv_env *c_env;          /* current env  */

int wnn_errorno = 0;
int noisy;

char jserver_dir[128];
char jserverrcfile[128];

char *hinsi_file_name = NULL;
/*
struct f_stamp stamp_org;
*/
char lang_dir[128];
