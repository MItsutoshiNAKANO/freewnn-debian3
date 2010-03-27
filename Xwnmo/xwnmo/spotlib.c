/*
 * $Id: spotlib.c,v 1.2 2001/06/14 18:16:17 ura Exp $
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

#include <X11/Xlib.h>
#include "Xi18nint.h"
#include "XIMlibint.h"
#include "SpotProto.h"

void
_XipChangeSpot (ic, x, y)
     XIC ic;
     short x, y;
{
  XipIM im = ipIMofIC ((XipIC) ic);
  ximChangeSpotReq req;
  ximEventReply reply;

  if (im->fd < 0)
    {
      return;
    }
  req.reqType = XIM_ChangeSpot;
  req.length = sz_ximChangeSpotReq;
  req.xic = ((XipIC) ic)->icid;
  req.spot_x = x;
  req.spot_y = y;
  if ((_XipWriteToIM (im, (char *) &req, sz_ximChangeSpotReq) >= 0) && (_XipFlushToIM (im) >= 0))
    {
      reply.state = 0;
      _XipReadFromIM (im, (char *) &reply, sz_ximEventReply);
    }
  return;
}
