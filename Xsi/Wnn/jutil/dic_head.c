/*
 *  $Id: dic_head.c $
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

#include <stdio.h>
#include "commonhd.h"
#include "jslib.h"
#include "jh.h"
#include "jdata.h"

extern int create_file_header (), output_header_jt (), input_file_header (), input_header_jt (), input_header_hjt ();

int
output_header (ofpter, jtp, fhp)
     FILE *ofpter;
     struct JT *jtp;
     struct wnn_file_head *fhp;
{
  create_file_header (ofpter, WNN_FT_DICT_FILE, fhp->file_passwd);
  output_header_jt (ofpter, jtp);
  return (0);
}

int
input_header (ifpter, jtp, fhp)
     FILE *ifpter;
     struct JT *jtp;
     struct wnn_file_head *fhp;
{

  if (input_file_header (ifpter, fhp) == -1 || input_header_jt (ifpter, jtp) == -1)
    {
      fprintf (stderr, "Not a Wnn file\n");
      return (-1);
    }
  return (0);
}

int
input_hindo_header (ifpter, hjtp, fhp)
     FILE *ifpter;
     struct wnn_file_head *fhp;
     struct HJT *hjtp;
{
  if (input_file_header (ifpter, fhp) == -1 || input_header_hjt (ifpter, hjtp) == -1)
    {
      return (-1);
    }
  return (0);
}
