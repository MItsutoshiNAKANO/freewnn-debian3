/*
 * $Id: ximdispt.c,v 1.2 2001/06/14 18:16:18 ura Exp $
 */

/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
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

/*
 *    Author Name         : Li Yuhong
 *    File Name           : XimDispt.c
 *    Module Description  : This module is to be corresponding to all
 *                          protocol requests of X11R6 IM Version 1.0.
 *    Modification History: 
 *      Jul 1, 1994       : initial version.
 */
#ifndef X11R5
#include <stdio.h>
#include <X11/Xlib.h>
#ifndef NEED_EVENTS
#define NEED_EVENTS
#include <X11/Xproto.h>         /* for xEvent */
#undef NEED_EVENTS
#endif /* !NEED_EVENTS */
#include "XIMproto.h"           /* for ximICValuesReq... */
#include "Ximprot.h"
#include "sdefine.h"
#include "xim.h"
#include "sheader.h"
#include "ext.h"

/*--------------------------------------------------------------------*
 *                                                                    *
 *                         Definition of Constant                     *
 *                                                                    *
 *--------------------------------------------------------------------*/

/*
 * name of "@im" modifier, which is compatible with R5.  The reasonable name
 * might be "xwnmo".
 */
#define XIM_SERVER_NAME         "_XWNMO"

#define BASE_IM_ID              0       /* valid XIMID > 0 */
#define BUFFER_MAX              128

#define ICPreeditAttributes     28      /* ic attr id. */
#define ICStatusAttributes      29
#define ICSeparatorOfNestedList 30

#define XIM_EXT_SET_EVENT_MASK  128     /* extension must be >= 128 */
#if defined(EXT_MOVE) || defined(SPOT)  /* SPOT is name in Xsi R5 */
#define XIM_EXT_MOVE            130
#endif /* defined(EXT_MOVE) || defined(SPOT) */

#define NO_NEST                 (XIMAttrList)NULL
#define END_OF_XIMATTRLIST      {0, 0, (char *)NULL, ((XIMAttrList)NULL)}

#define XIMNoString             (1L)
#define XIMNoFeedback           (2L)


/*--------------------------------------------------------------------*
 *                                                                    *
 *                         Definition of Local Data Type              *
 *                                                                    *
 *--------------------------------------------------------------------*/

#define PIXMAP  CARD32          /* for CALLBACKS only */

/*
 * XIM attribute structure for both IM and IC value.
 */
typedef struct _XIMAttrRec
{
  CARD16 attr_id;               /* attr id */
  CARD16 type;                  /* type of attr */
  char *name;                   /* name of attr */
  struct _XIMAttrRec *nest;     /* nested list */
}
XIMAttrRec, *XIMAttrList;

/*
 * XIM client structure connected to this server.
 */
typedef struct _XIMContextRec
{
  XIMID im_id;                  /* IM id */
  struct _XIMContextRec *next;  /* next of client */
}
XIMContextRec, *XIMContextList;

/*
 * extension.
 */
typedef struct
{
  char *name;                   /* ext name */
  CARD8 major_code;             /* ext major opcode */
  CARD8 minor_code;             /* ext minor opcode */
}
XIMQueryExtRec;

#ifdef DYNAMIC_EVENT_FLOW_MODEL
/*
 * just example for Dynamic Event Flow Model.
 */
typedef struct
{
  KeySym key;
  KeySym modifier;
  unsinged long modifier_mask;
}
TriggerKeysRec, *TriggerKeysList;

static TriggerKeyRec trigger_keys_list[] = {
  {XK_backslash, XK_Control_L, ControlMask}
}
#endif                          /* DYNAMIC_EVENT_FLOW_MODEL */


/*--------------------------------------------------------------------*
 *                                                                    *
 *                         Definition of Macro                        *
 *                                                                    *
 *--------------------------------------------------------------------*/

/*
 * assign a unique id.
 */
#define ALLOC_ID(n)             (++n)

/*
 * shared CT string buffer.
 */
#define GET_SHARED_CT_BUFFER()                  (ct_buf)

/*
 * swap data of protocol.
 */
#define GET_CARD16(s)           \
        ((CARD16)((need_byteswap() == True)? byteswap_s(s): s))
#define GET_CARD32(s)           \
        ((CARD32)((need_byteswap() == True)? byteswap_l(s): s))

/*
 * Pad(n) for protocol data on 4-byte word boundory.
 */
#define PAD(n)                  ((4 - (n) % 4) % 4)

/*
 * pre-make memory for frequent allocation of Request/Reply protocol data.
 */
#define MALLOC_REQUEST(n)       \
        (((n) > (sizeof buffer_request))? (CARD8 *)Malloc(n): &buffer_request[0])
#define MALLOC_REPLY(n)         \
        (((n) > (sizeof buffer_reply))? (CARD8 *)Malloc(n): &buffer_reply[0])
#define FREE(p)                 \
        {if ((p) != &buffer_request[0] && (p) != &buffer_reply[0]) XFree(p);}

#define XIM_SERIALNUMBER(serial)  \
                (CARD16)(((unsigned long)serial >> 16) & 0xffff)
#define XIM_SET_SERIALNUMBER(serial)    \
        (((unsigned long)serial & 0xffff) << 16)

#define LENGTH_KEYSYM(f, k)     \
        (((f)&XimLookupKeySym)? (sizeof(CARD16)+k): 0)
#define LENGTH_STRING_WITH_FLAG(f, n)   \
        (((f)&XimLookupChars)? (sizeof(CARD16)+n+PAD(n)): 0)

#define LENGTH_STRING(n)        ((n > 0) ?                                  \
         (sizeof(CARD16) +                      /* 2 n string length */     \
          n +                                   /* n string */              \
          PAD(sizeof(CARD16) + n)): 0)  /* p Pad(2 + n) */
#define LENGTH_FEEDBACK(n)      ((n > 0)?                                   \
         (sizeof(CARD16) +                      /* 2 m byte length */       \
          sizeof(CARD16) +                      /* 2 unused */              \
          sizeof(CARD32) * n): 0)       /* m LISTofXIMFEEDBACK */

#define CUR_IM_ID()             (cur_x->im_id)
#define CUR_IC_ID()             (cur_x->number)
#define CUR_CLIENT_EVENT()      ((XEvent *)&(cur_x->cur_event))

#define LENGTH_TEXT(t, sl, fn)  ((t == XIMTextType)?                        \
        (LENGTH_STRING(sl) + LENGTH_FEEDBACK(fn)): 0)
#define LENGTH_PIXMAP(t)        ((t != XIMTextType)? sizeof(PIXMAP): 0)


/*--------------------------------------------------------------------*
 *                                                                    *
 *                         Definition of Local Variable               *
 *                                                                    *
 *--------------------------------------------------------------------*/

static CARD8 buffer_request[BUFSIZ];    /* for protocol request */
static CARD8 buffer_reply[BUFSIZ];      /* for protocol reply */

/*
 * X selection atom, target and property in pre-connection.
 */
static Atom transport_target;
                                        /* atom name of TRANSPORT target */
static Atom locale_target;      /* atom name of LOCALE target */
static unsigned char *transport_prop;
                                        /* string prop of TRANSPORT target */
static unsigned char *locale_prop;      /* string prop of LOCALE target */

/*
 * comma-separated list of locales supported by this server.
 */
static unsigned char *locale_supported;

static XIMContextList ximlist;  /* list of comming IM client */
static int cur_status;          /* current protol status */
static XIMID cur_im_id;         /* current client IM id */
static XICID cur_ic_id;         /* current client IC id */
static char cur_use_lang[BUFFER_MAX];
                                        /* current lang from XIM_OPEN */

/*
 * list of IM attributes.
 */
static XIMAttrRec im_attr[] = {
  {ICInputStyle, XimType_XIMStyles, XNQueryInputStyle, NO_NEST},
  END_OF_XIMATTRLIST
};

/*
 * list of IC preedit attributes. 
 */
static XIMAttrRec ic_attr_pre_area[] = {
  {ICArea, XimType_XRectangle, XNArea, NO_NEST},
  {ICAreaNeeded, XimType_XRectangle, XNAreaNeeded, NO_NEST},
  {ICSpotLocation, XimType_XPoint, XNSpotLocation, NO_NEST},
  {ICColormap, XimType_CARD32, XNColormap, NO_NEST},
  {ICStdColormap, XimType_CARD32, XNStdColormap, NO_NEST},
  {ICForeground, XimType_CARD32, XNForeground, NO_NEST},
  {ICBackground, XimType_CARD32, XNBackground, NO_NEST},
  {ICBackgroundPixmap, XimType_CARD32, XNBackgroundPixmap, NO_NEST},
  {ICFontSet, XimType_XFontSet, XNFontSet, NO_NEST},
  {ICLineSpace, XimType_CARD32, XNLineSpace, NO_NEST},
  {ICCursor, XimType_CARD32, XNCursor, NO_NEST},
  {ICSeparatorOfNestedList,
   XimType_SeparatorOfNestedList,
   XNSeparatorofNestedList, NO_NEST},
  END_OF_XIMATTRLIST
};

/*
 * list of IC status attributes.
 */
static XIMAttrRec ic_attr_sts_area[] = {
  {ICArea, XimType_XRectangle, XNArea, NO_NEST},
  {ICAreaNeeded, XimType_XRectangle, XNAreaNeeded, NO_NEST},
  {ICColormap, XimType_CARD32, XNColormap, NO_NEST},
  {ICStdColormap, XimType_CARD32, XNStdColormap, NO_NEST},
  {ICForeground, XimType_CARD32, XNForeground, NO_NEST},
  {ICBackground, XimType_CARD32, XNBackground, NO_NEST},
  {ICBackgroundPixmap, XimType_CARD32, XNBackgroundPixmap, NO_NEST},
  {ICFontSet, XimType_XFontSet, XNFontSet, NO_NEST},
  {ICLineSpace, XimType_CARD32, XNLineSpace, NO_NEST},
  {ICCursor, XimType_CARD32, XNCursor, NO_NEST},
  {ICSeparatorOfNestedList,
   XimType_SeparatorOfNestedList,
   XNSeparatorofNestedList, NO_NEST},
  END_OF_XIMATTRLIST
};

/*
 * list of IC attributes.
 */
static XIMAttrRec ic_attr[] = {
  {ICInputStyle, XimType_CARD32, XNInputStyle, NO_NEST},
  {ICClientWindow, XimType_Window, XNClientWindow, NO_NEST},
  {ICFocusWindow, XimType_Window, XNFocusWindow, NO_NEST},
  {ICFilterEvents, XimType_CARD32, XNFilterEvents, NO_NEST},
  {ICPreeditAttributes, XimType_NEST, XNPreeditAttributes, ic_attr_pre_area},
  {ICStatusAttributes, XimType_NEST, XNStatusAttributes, ic_attr_sts_area},
  END_OF_XIMATTRLIST
};

/*
 * list of extension supported.
 */
static XIMQueryExtRec extension_list[] = {
  {"XIM_EXT_SET_EVENT_MASK", XIM_EXT_SET_EVENT_MASK, 0},
#if defined(EXT_MOVE) || defined(SPOT)
  {"XIM_EXT_MOVE", XIM_EXT_MOVE, 0}
#endif /* defined(EXT_MOVE) || defined(SPOT) */
};

static CARD8 imattr_list[BUFSIZ];       /* packed im_attr list */
static CARD8 icattr_list[BUFSIZ];       /* packed ic_attr list */
static INT16 imattr_list_size;  /* byte length */
static INT16 icattr_list_size;  /* byte length */


/*--------------------------------------------------------------------*
 *                                                                    *
 *               Declaration of Extenal Variable and Function         *
 *                                                                    *
 *--------------------------------------------------------------------*/

extern XIMCmblk *cur_cblk;      /* current client block */

extern int XmuGetHostname ();   /* in SysUtil.h of Xmu */

/*--------------------------------------------------------------------*
 *                                                                    *
 *               Definition of Public Variable and Function           *
 *                                                                    *
 *--------------------------------------------------------------------*/

Atom server_id;                 /* atom name of this server "@im" */

int xim_send_nofilter ();       /* used before its definition. */

/*--------------------------------------------------------------------*
 *                                                                    *
 *                         Private Functions                          *
 *                                                                    *
 *--------------------------------------------------------------------*/

/*
 *    Function Name : _WriteToClientFlush()
 *    Description   : write data to client immediately with flush action.
 *                    same as do_socket.c: _WriteToClient() if no flush.
 *    Parameter     :
 *      p           : pointer to data.
 *      num         : byte length of data.
 *    Returned Value:
 *        >= 0      : byte length of data written to client.
 *        < 0       : error in writing.
 */
static int
_WriteToClientFlush (p, num)
     char *p;
     int num;
{
  int ret;

  if ((ret = _WriteToClient (p, num)) >= 0)
    {
      if (_Send_Flush () == 0)
        return ret;             /* success */
    }
  return ret;
}

/*
 *    Function Name : _XimWireToEvent()
 *    Description   : convert xEvent to XEvent, modified from Xlibint.c:
 *                    _XWireToEvent().
 *    Parameter     :
 *      dpy         : pointer to X display.
 *      serial      : serial number of event.
 *      event       : pointer to data of X protocol xEvent
 *      re          : pointer to data of XEvent, returned.
 *    Returned Value:
 *      True        : conversion is successful.
 *      False       : error.
 */
static Bool
_XimWireToEvent (dpy, serial, event, re)
     register Display *dpy;     /* pointer to display structure */
     register int serial;       /* serial number of event */
     register xEvent *event;    /* wire protocol event */
     register XEvent *re;       /* pointer to where event should be reformatted */
{

  re->type = event->u.u.type & 0x7f;
  ((XAnyEvent *) re)->serial = XIM_SET_SERIALNUMBER (serial);
  ((XAnyEvent *) re)->send_event = ((event->u.u.type & 0x80) != 0);
  ((XAnyEvent *) re)->display = dpy;

  /* Ignore the leading bit of the event type since it is set when a
     client sends an event rather than the server. */

  switch (event->u.u.type & 0177)
    {
    case KeyPress:
    case KeyRelease:
      {
        register XKeyEvent *ev = (XKeyEvent *) re;
        ev->root = event->u.keyButtonPointer.root;
        ev->window = event->u.keyButtonPointer.event;
        ev->subwindow = event->u.keyButtonPointer.child;
        ev->time = event->u.keyButtonPointer.time;
        ev->x = cvtINT16toInt (event->u.keyButtonPointer.eventX);
        ev->y = cvtINT16toInt (event->u.keyButtonPointer.eventY);
        ev->x_root = cvtINT16toInt (event->u.keyButtonPointer.rootX);
        ev->y_root = cvtINT16toInt (event->u.keyButtonPointer.rootY);
        ev->state = event->u.keyButtonPointer.state;
        ev->same_screen = event->u.keyButtonPointer.sameScreen;
        ev->keycode = event->u.u.detail;
      }
      break;
    case ButtonPress:
    case ButtonRelease:
      {
        register XButtonEvent *ev = (XButtonEvent *) re;
        ev->root = event->u.keyButtonPointer.root;
        ev->window = event->u.keyButtonPointer.event;
        ev->subwindow = event->u.keyButtonPointer.child;
        ev->time = event->u.keyButtonPointer.time;
        ev->x = cvtINT16toInt (event->u.keyButtonPointer.eventX);
        ev->y = cvtINT16toInt (event->u.keyButtonPointer.eventY);
        ev->x_root = cvtINT16toInt (event->u.keyButtonPointer.rootX);
        ev->y_root = cvtINT16toInt (event->u.keyButtonPointer.rootY);
        ev->state = event->u.keyButtonPointer.state;
        ev->same_screen = event->u.keyButtonPointer.sameScreen;
        ev->button = event->u.u.detail;
      }
      break;
    default:
      return False;
    }
  return True;
}

/*
 *    Function Name : _XimEventToWire()
 *    Description   : convert XEvent to xEvent, modified from EvToWire.c:
 *                    _XEventToWire().
 *    Parameter     :
 *      re          : pointer to data of XEvent.
 *      event       : pointer to data of X protocol xEvent, returned
 *    Returned Value:
 *      True        : conversion is successful.
 *      False       : error.
 */
static Bool
_XimEventToWire (re, event)
     register XEvent *re;       /* pointer to where event should be reformatted */
     register xEvent *event;    /* wire protocol event */
{
  switch (event->u.u.type = re->type)
    {
    case KeyPress:
    case KeyRelease:
      {
        register XKeyEvent *ev = (XKeyEvent *) re;
        event->u.keyButtonPointer.root = ev->root;
        event->u.keyButtonPointer.event = ev->window;
        event->u.keyButtonPointer.child = ev->subwindow;
        event->u.keyButtonPointer.time = ev->time;
        event->u.keyButtonPointer.eventX = ev->x;
        event->u.keyButtonPointer.eventY = ev->y;
        event->u.keyButtonPointer.rootX = ev->x_root;
        event->u.keyButtonPointer.rootY = ev->y_root;
        event->u.keyButtonPointer.state = ev->state;
        event->u.keyButtonPointer.sameScreen = ev->same_screen;
        event->u.u.detail = ev->keycode;
      }
      break;
    case ButtonPress:
    case ButtonRelease:
      {
        register XButtonEvent *ev = (XButtonEvent *) re;
        event->u.keyButtonPointer.root = ev->root;
        event->u.keyButtonPointer.event = ev->window;
        event->u.keyButtonPointer.child = ev->subwindow;
        event->u.keyButtonPointer.time = ev->time;
        event->u.keyButtonPointer.eventX = ev->x;
        event->u.keyButtonPointer.eventY = ev->y;
        event->u.keyButtonPointer.rootX = ev->x_root;
        event->u.keyButtonPointer.rootY = ev->y_root;
        event->u.keyButtonPointer.state = ev->state;
        event->u.keyButtonPointer.sameScreen = ev->same_screen;
        event->u.u.detail = ev->button;
      }
      break;
    default:
      return False;
    }
  return True;
}

/*
 * get list of locale names as the format:
 *            "locale_1,locale_2,...,locale_n"
 * and return the length of the string above.
 */
static char *
GetLocaleSupported ()
{
  return (!xim->supported_language) ? "C" : xim->supported_language;
}

/*
 *    Function Name : _CheckLocaleName()
 *    Description   : check if the client locale is supported in this server.
 *    Parameter     :
 *      buf         : string buffer of client locale.
 *      loc_name    : comma-separated list of locales supported in this server.
 *    Returned Value:
 *      True        : it is supported.
 *      False       : not supproted.
 */
static Bool
_CheckLocaleName (buf, loc_name)
     CARD8 *buf;
     char *loc_name;
{
  int str_len = (int) buf[0];
  int len;
  char *ptr = (char *) locale_supported;
  char *str = (char *) &buf[1];
  register char *p;

  loc_name[0] = '\0';
  for (;;)
    {
      for (p = ptr; ((*p != ',') && (*p)); p++);
      len = (int) (p - ptr);
      if ((str_len == len) && (!strncmp (str, ptr, len)))
        {
          (void) strncpy (loc_name, str, len);
          loc_name[len] = '\0';
          return True;
        }
      if (!(*p))
        break;
      ptr = p + 1;
    }
  return False;
}

/*
 *    Function Name : GetIMWithId()
 *    Description   : search a client context with IM id.
 *    Parameter     :
 *      im_id       : client input-method-id.
 *    Returned Value:
 *      XIMContextRec *
 *                  : pointer to the client context found.
 *      NULL        : not found.
 */
static XIMContextRec *
GetIMWithId (im_id)
     register XIMID im_id;
{
  register XIMContextRec *im;

  for (im = ximlist; im; im = im->next)
    {
      if (im->im_id == im_id)
        return im;
    }
  return (XIMContextRec *) NULL;
}

/*
 *    Function Name : RemoveIMWithId()
 *    Description   : remove a IM client context.
 *    Parameter     :
 *      im_id       : client input-method-id.
 *    Returned Value:
 *      True        : the IM client context with im_id is removed from list
 *                    and is freed its memory.
 *      False       : not found with this im_id.
 */
static Bool
RemoveIMWithId (im_id)
     XIMID im_id;
{
  register XIMContextRec *im, *pre_im;

  for (im = pre_im = ximlist; im; pre_im = im, im = im->next)
    {
      if (im->im_id == im_id)
        {
          if (pre_im != im)
            {
              pre_im->next = im->next;
            }
          else
            {
              ximlist = im->next;
            }
          break;
        }
    }
  if (!im)
    return False;
  XFree ((char *) im);
  return True;
}

/*
 *    Function Name : GetICWithId()
 *    Description   : search a client structure with IC id.
 *    Parameter     :
 *      ic_id       : client input-context-id.
 *    Returned Value:
 *      XIMClientRec*
 *                  : pointer to the client structure found.
 *      NULL        : not found.
 */
static XIMClientRec *
GetICWithId (ic_id)
     register int ic_id;
{
  register XIMClientRec *xc;

  for (xc = ximclient_list; xc != NULL; xc = xc->next)
    {
      if (xc->number == ic_id)
        return xc;
    }
  return (XIMClientRec *) NULL;
}

/*
 *    Function Name : AddHead()
 *    Description   : fill head data to a protocol reply which will be
 *                    sent to client.  The head data to be filed is as follows:
 *                        CARD8  major_opcode
 *                        CARD8  minor_opcode
 *                        CARD16 data length in 4-byte word
 *    Parameter     :
 *      reply       : pointer to a protocol reply structure, which is 
 *                    to be filled head data.
 *      major_code  : major opcode of this protocol.
 *      minor_code  : minor opcode of this protocol.
 *      len         : data length of protocol except head.
 *    Returned Value:
 *      > 0         : byte length of head data, always XIM_HEADER_SIZE.
 */
static int
AddHead (reply, major_code, minor_code, len)
     CARD8 *reply;
     CARD8 major_code;
     CARD8 minor_code;
     int len;
{
  CARD8 *buf_8 = reply;
  CARD16 *buf_16 = (CARD16 *) (reply + sizeof (CARD8) * 2);

  buf_8[0] = major_code;
  buf_8[1] = minor_code;
  buf_16[0] = GET_CARD16 (len / 4);
  return XIM_HEADER_SIZE;
}

/*
 *    Function Name : SetClientSync()
 *    Description   : set the client to a sync flag.
 *    Parameter     :
 *      xc          : pointer to a client structure.
 *      sync        : flag of sync.
 *    Returned Value:
 *      <none> 
 */
#ifdef NEED_FUNCTION

static void
SetClientSync (xc, sync)
     XIMClientRec *xc;
     Bool sync;
{
  xc->sync_needed = sync;
}

static Bool
AllSyncDone (im_id, ic_id)
     XIMID im_id;
     XICID im_cd;
{
  return True;
}

static void
SyncAll (im_id, ic_id)
     XIMID im_id;
     XICID im_cd;
{
}

#else

#define SetClientSync(xc, sync)                 ((xc)->sync_needed = sync)
#define AllSyncDone(im_id, ic_id)               (True)
#define SyncAll(im_id, ic_id)

#endif /* NEED_FUNCTION */

/*
 *    Function Name : ClientSyncDone()
 *    Description   : If this client has True flag needed to do sync, do it
 *                    and turn off the sync flag (False).
 *    Parameter     :
 *      xc          : pointer to a client structure.
 *    Returned Value:
 *      True        : yes, sync has done.
 *      False       : no sync.
 */
static Bool
ClientSyncDone (xc)
     XIMClientRec *xc;
{
  Bool sync_done;

  sync_done = xc->sync_needed;
  xc->sync_needed = False;
  return sync_done;
}

/*
 *    Function Name : GetAuth()
 *    Description   : get names of authentification from IM library.
 *    Parameter     :
 *      list        : buffer list.
 *      num         : number of list elements.
 *      length      : byte length of buffer list.
 *      names_returned
 *                  : auth name returned.
 *    Returned Value:
 *      True        : successed.
 *      False       : wrong auth name.
 */

 /*ARGSUSED*/ static Bool
GetAuth (list, num, length, names_return)
     CARD8 *list;
     int num;
     int length;
     char *names_return;
{
  /* it is free authentification so far. */
  return True;
}

/*
 *    Function Name : CreateAttrList()
 *    Description   : pack IM/IC attribute list to a compact data.
 *    Parameter     :
 *      table       : list of IM/IC attributes, end with the empty name.
 *      list        : returned data that contains packed list of attribute.
 *    Returned Value:
 *      >= 0        : byte length of packed list.
 */
static int
CreateAttrList (table, list)
     XIMAttrList table;
     CARD8 *list;
{
  CARD16 *buf_16;
  INT16 total = 0;
  int len, i;

  for (i = 0; table[i].name; i++)
    {
      buf_16 = (CARD16 *) list;
      len = strlen (table[i].name);
      buf_16[0] = GET_CARD16 (table[i].attr_id);        /* 2 attr ID */
      buf_16[1] = GET_CARD16 (table[i].type);   /* 2 type */
      buf_16[2] = GET_CARD16 (len);     /* 2 length */
      memcpy ((char *) &buf_16[3], table[i].name, len); /* n attr */
      len = sizeof (CARD16) + sizeof (CARD16) + sizeof (CARD16) + len + PAD (len + 2);
      list += len;
      total += len;
      if (table[i].nest)
        {
          len = CreateAttrList (table[i].nest, list);
          list += len;
          total += len;
        }
    }
  return total;
}

/*
 *    Function Name : InitiateProtocol()
 *    Description   : do some initial works.
 *    Parameter     :
 *      <none>
 *    Returned Value:
 *      <none>
 */
static void
InitiateProtocol ()
{
  ximlist = (XIMContextList) NULL;

  imattr_list_size = CreateAttrList (im_attr, imattr_list);
  icattr_list_size = CreateAttrList (ic_attr, icattr_list);
}

/*
 *    Function Name : CheckInputStyle()
 *    Description   : check if the passed input style is supported.
 *    Parameter     :
 *      input_style : the input style to be checked.
 *    Returned Value:
 *      True        : supported.
 *      False       : not supported.
 */
static Bool
CheckInputStyle (input_style)
     INT32 input_style;
{
  int i;

  for (i = 0; i < MAX_SUPPORT_STYLE; i++)
    {
      if (xim->supported_style[i] == input_style)
        {
          return True;
        }
    }
  return False;
}

/*
 *    Function Name : ConvErrorCode()
 *    Description   : convert error code used in Xsi R5 IM to R6 IM.
 *    Parameter     :
 *      x11r5_code  : error code of Xsi R5 IM.
 *    Returned Value:
 *      > 0         : errro code of R6 IM 
 */
static int
ConvErrorCode (x11r5_code)
     short x11r5_code;
{
  switch (x11r5_code)
    {
    case AllocError:
      return XIM_BadAlloc;
    case BadStyle:
      return XIM_BadStyle;
    case BadClientWindow:
      return XIM_BadClientWindow;
    case BadFocusWindow:
      return XIM_BadFocusWindow;
    case BadLanguage:
      return XIM_LocaleNotSupported;
    case BadSpotLocation:
      return XIM_BadSpotLocation;
    case BadFontSet:
      return XIM_BadName;       /* or XIM_BadStyle */
    default:
      return XIM_BadSomething;
    }
}

/*
 *    Function Name : ConvAreaAttr()
 *    Description   : convert byte data to IC attribute of Area.
 *    Parameter     :
 *      value       : the byte data.
 *      size        : byte length of the data.
 *      ic_attr_req : contain Area attribute to be filled.
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
static int
ConvAreaAttr (value, size, ic_attr_req)
     CARD8 *value;
     int size;
     ximICAttributesReq *ic_attr_req;
{
  CARD16 *buf_16 = (CARD16 *) value;

  if (size != (sizeof (CARD16) << 2))
    return XIM_BadAlloc;

  ic_attr_req->area_x = (INT16) GET_CARD16 (buf_16[0]);
  ic_attr_req->area_y = (INT16) GET_CARD16 (buf_16[1]);
  ic_attr_req->area_width = GET_CARD16 (buf_16[2]);
  ic_attr_req->area_height = GET_CARD16 (buf_16[3]);
  return 0;
}

/*
 *    Function Name : ConvAreaNeededAttr()
 *    Description   : convert byte data to IC attribute of AreaNeeded.
 *    Parameter     :
 *      value       : the byte data.
 *      size        : byte length of the data.
 *      ic_attr_req : contain AreaNeeded attribute to be filled.
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
static int
ConvAreaNeededAttr (value, size, ic_attr_req)
     CARD8 *value;
     int size;
     ximICAttributesReq *ic_attr_req;
{
  CARD16 *buf_16 = (CARD16 *) value;

  if (size != (sizeof (CARD16) << 2))
    return XIM_BadAlloc;

  ic_attr_req->areaneeded_width = GET_CARD16 (buf_16[2]);
  ic_attr_req->areaneeded_height = GET_CARD16 (buf_16[3]);
  return 0;
}

/*
 *    Function Name : ConvSpotAttr()
 *    Description   : convert byte data to IC attribute of Spot.
 *    Parameter     :
 *      value       : the byte data.
 *      size        : byte length of the data.
 *      ic_attr_req : contain Spot attribute to be filled.
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
static int
ConvSpotAttr (value, size, ic_attr_req)
     CARD8 *value;
     int size;
     ximICAttributesReq *ic_attr_req;
{
  CARD16 *buf_16 = (CARD16 *) value;

  if (size != (sizeof (CARD16) + sizeof (CARD16)))
    return XIM_BadAlloc;

  ic_attr_req->spot_x = (INT16) GET_CARD16 (buf_16[0]);
  ic_attr_req->spot_y = (INT16) GET_CARD16 (buf_16[1]);
  return 0;
}

/*
 *    Function Name : ConvFontSetAttr()
 *    Description   : convert byte data to IC attribute of FontSet name.
 *    Parameter     :
 *      value       : the byte data.
 *      size        : byte length of the data.
 *      ic_attr_req : contain FontSet attribute to be filled.
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
static int
ConvFontSetAttr (value, size, base_font_name)
     CARD8 *value;
     int size;
     char **base_font_name;
{
  CARD16 *buf_16 = (CARD16 *) value;
  char *fname;
  int len;

  len = (int) GET_CARD16 (buf_16[0]);
  if (len > size)
    return XIM_BadAlloc;
  if (!(fname = Malloc (len + 1)))
    return XIM_BadAlloc;
  (void) strncpy (fname, (char *) &buf_16[1], len);
  fname[len] = '\0';
  *base_font_name = fname;
  return 0;
}

/*
 *    Function Name : DecodeICNestedAttributes()
 *    Description   : decode IC nested attributes
 *    Parameter     :
 *      list        : list of IC attribute, packed data. 
 *      size        : byte length of list.
 *      ic_attr_req : IC attribute structure to be filled.
 *      font        : base font name to be returned.  The caller should
 *                    free it after use.
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
static int
DecodeICNestedAttributes (list, size, mask, ic_attr_req, font)
     CARD8 *list;
     int size;
     CARD32 *mask;
     ximICAttributesReq *ic_attr_req;
     char **font;
{
  CARD16 *buf_16;
  CARD32 *buf_32;
  CARD32 mask_base, mask_ret;
  int ret, have_more;
  int len, tmpl, attr_id;

  ret = 0;
  mask_base = *mask;
  mask_ret = 0;
  have_more = 1;
  while (size > 0 && have_more)
    {
      buf_16 = (CARD16 *) list;
      attr_id = (int) GET_CARD16 (buf_16[0]);   /* 2 attribute id */
      len = (int) GET_CARD16 (buf_16[1]);       /* n length of value */
      tmpl = sizeof (CARD16) + sizeof (CARD16);
      list += tmpl;
      size -= tmpl;
      buf_16 = (CARD16 *) list;
      buf_32 = (CARD32 *) list;
      switch (attr_id)
        {
        case ICArea:
          mask_ret |= (1 << (ICArea + mask_base));
          ret = ConvAreaAttr (list, len, ic_attr_req);
          break;
        case ICAreaNeeded:
          mask_ret |= (1 << (ICAreaNeeded + mask_base));
          ret = ConvAreaNeededAttr (list, len, ic_attr_req);
          break;
        case ICSpotLocation:
          mask_ret |= (1 << (ICSpotLocation + mask_base));
          ret = ConvSpotAttr (list, len, ic_attr_req);
          break;
        case ICColormap:
          mask_ret |= (1 << (ICColormap + mask_base));
          ic_attr_req->colormap = (Colormap) GET_CARD32 (buf_32[0]);
          break;
        case ICStdColormap:
          mask_ret |= (1 << (ICStdColormap + mask_base));
          ic_attr_req->std_colormap = (Atom) GET_CARD32 (buf_32[0]);
          break;
        case ICForeground:
          mask_ret |= (1 << (ICForeground + mask_base));
          ic_attr_req->foreground = GET_CARD32 (buf_32[0]);
          break;
        case ICBackground:
          mask_ret |= (1 << (ICBackground + mask_base));
          ic_attr_req->background = GET_CARD32 (buf_32[0]);
          break;
        case ICBackgroundPixmap:
          mask_ret |= (1 << (ICBackgroundPixmap + mask_base));
          ic_attr_req->pixmap = (Pixmap) GET_CARD32 (buf_32[0]);
          break;
        case ICFontSet:
          mask_ret |= (1 << (ICFontSet + mask_base));
          ret = ConvFontSetAttr (list, len, font);
          break;
        case ICLineSpace:
          mask_ret |= (1 << (ICLineSpace + mask_base));
          ic_attr_req->line_space = (INT16) GET_CARD32 (buf_32[0]);
          break;
        case ICCursor:
          mask_ret |= (1 << (ICCursor + mask_base));
          ic_attr_req->cursor = (Cursor) GET_CARD32 (buf_32[0]);
          break;
        case ICSeparatorOfNestedList:
          have_more = 0;
          break;
        default:
          have_more = 0;
          continue;
        }
      if (ret)
        return ret;
      len = len +               /* n value */
        PAD (len);              /* p Pad(n) */
      list += len;
      size -= len;
    }
  *mask = mask_ret;
  return 0;
}

/*
 *    Function Name : DecodeICAttributes()
 *    Description   : parse all IC attributes
 *    Parameter     :
 *      list        : list of IC attributes, packed data.
 *      size        : byte length of list.
 *      ic_attr_req : IC attribute structure to be filled.
 *      pre_req     : preedit attributes to be filled.
 *      st_req      : status attributes to be filled.
 *      pre_font    : preedit base font name to be returned.  The caller should
 *                    free it after use.
 *      st_font     : status base font name to be returned.  The caller should
 *                    free it after use.
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
static int
DecodeICAttributes (list, size, ic_req, pre_req, st_req, pre_font, st_font)
     CARD8 *list;
     int size;
     ximICValuesReq *ic_req;
     ximICAttributesReq *pre_req;
     ximICAttributesReq *st_req;
     char **pre_font;
     char **st_font;
{
  CARD16 *buf_16;
  CARD32 *buf_32;
  CARD32 mask;
  int ret, len, attr_id;
  int tmpl;

  ret = 0;
  ic_req->mask = (CARD32) 0;
  while (size > 0)
    {
      buf_16 = (CARD16 *) list;
      attr_id = (int) GET_CARD16 (buf_16[0]);
      len = (int) GET_CARD16 (buf_16[1]);
      tmpl = sizeof (CARD16) +  /* 2 attribute id */
        sizeof (CARD16);        /* 2 value length */
      list += tmpl;
      size -= tmpl;
      buf_16 = (CARD16 *) list;
      buf_32 = (CARD32 *) list;
      switch (attr_id)
        {
        case ICInputStyle:
          ic_req->mask |= (1 << ICInputStyle);
          ic_req->input_style = (INT32) GET_CARD32 (buf_32[0]);
          break;
        case ICClientWindow:
          ic_req->mask |= (1 << ICClientWindow);
          ic_req->c_window = (Window) GET_CARD32 (buf_32[0]);
          break;
        case ICFocusWindow:
          ic_req->mask |= (1 << ICFocusWindow);
          ic_req->focus_window = (Window) GET_CARD32 (buf_32[0]);
          break;
        case ICFilterEvents:
          ic_req->mask |= (1 << ICFilterEvents);
          ic_req->filter_events = GET_CARD32 (buf_32[0]);
          break;
        case ICPreeditAttributes:
          mask = 0;
          ret = DecodeICNestedAttributes (list, len, &mask, pre_req, pre_font);
          ic_req->mask |= mask;
          break;
        case ICStatusAttributes:
          mask = StatusOffset;
          ret = DecodeICNestedAttributes (list, len, &mask, st_req, st_font);
          ic_req->mask |= mask;
          break;
        default:
          ret = XIM_BadProtocol;
          break;
        }
      if (ret)
        return ret;
      len = len +               /* n value */
        PAD (len);              /* p Pad(n) */
      list += len;
      size -= len;
    }
  return 0;
}

/*
 *    Function Name : SetClientICValues()
 *    Description   : set IC values to a client structure.
 *    Parameter     :
 *      list        : list of IC attributes, packed data.
 *      size        : byte length of list.
 *      xc          : pointer to client structure.
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
static int
SetClientICValues (list, size, xc)
     CARD8 *list;
     int size;
     XIMClientRec *xc;
{
  ximICValuesReq ic_req;
  ximICAttributesReq pre_req, st_req;
  XIMNestLangRec *p = (XIMNestLangRec *) NULL;
  char *pre_font = (char *) NULL;
  char *st_font = (char *) NULL;
  int cur_is_world = 0;
  int ret;
  short detail;
  extern int change_client ();

  /* parse IC attributes */
  ret = DecodeICAttributes (list, size, &ic_req, &pre_req, &st_req, &pre_font, &st_font);
  if (ret)
    return ret;

  /* refer to util: ChangeIC() */

  ret = change_client (xc, &ic_req, &pre_req, &st_req, p, cur_is_world, pre_font, st_font, &detail);
  if (pre_font)
    XFree (pre_font);
  if (st_font)
    XFree (st_font);
  if (p)
    free_langlist (p);
  if (ret == -1)
    return ConvErrorCode (detail);
  return 0;
}

/*
 *    Function Name : EncodeRectangleAttr()
 *    Description   : encode XRectangle to byte data.
 *    Parameter     :
 *      rect        : pointer to the rectangle structure.
 *      value       : encoded byte data.
 *      size_return : returned byte length of the data.
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
static int
EncodeRectangleAttr (rect, value, size_return)
     XRectangle *rect;
     CARD8 *value;
     int *size_return;
{
  CARD16 *buf_16 = (CARD16 *) value;

  buf_16[0] = GET_CARD16 (rect->x);
  buf_16[1] = GET_CARD16 (rect->y);
  buf_16[2] = GET_CARD16 (rect->width);
  buf_16[3] = GET_CARD16 (rect->height);
  *size_return = sizeof (CARD16) +      /* 2 X */
    sizeof (CARD16) +           /* 2 Y */
    sizeof (CARD16) +           /* 2 width */
    sizeof (CARD16);            /* 2 height */
  return 0;
}

/*
 *    Function Name : EncodePointAttr()
 *    Description   : encode XPoint to byte data.
 *    Parameter     :
 *      rect        : pointer to the XPoint structure.
 *      value       : encoded byte data.
 *      size_return : returned byte length of the data.
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
static int
EncodePointAttr (xc, value, size_return)
     XIMClientRec *xc;
     CARD8 *value;
     int *size_return;
{
  CARD16 *buf_16 = (CARD16 *) value;
  short spot_x, spot_y;

  spot_x = xc->point.x;
  spot_y = xc->point.y + FontAscent (xc->cur_xl);
  buf_16[0] = GET_CARD16 (spot_x);
  buf_16[1] = GET_CARD16 (spot_y);
  *size_return = sizeof (CARD16) +      /* 2 X */
    sizeof (CARD16);            /* 2 Y */
  return 0;
}

/*
 *    Function Name : EncodeFontSetAttr()
 *    Description   : encode name of base font set to byte data.
 *    Parameter     :
 *      rect        : pointer to the name.
 *      value       : encoded byte data.
 *      size_return : returned byte length of the data.
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
static int
EncodeFontSetAttr (base_font_name, value, size_return)
     char *base_font_name;
     CARD8 *value;
     int *size_return;
{
  CARD16 *buf_16 = (CARD16 *) value;
  int len;

  len = strlen ((char *) base_font_name);
  buf_16[0] = GET_CARD16 (len);
  (void) strncpy ((char *) &buf_16[1], (char *) base_font_name, len);
  *size_return = sizeof (CARD16) + len + PAD (sizeof (CARD16) + len);
  return 0;
}

/*
 *    Function Name : EncodeICNestedAttributes()
 *    Description   : encode IC nested attribute structure to byte data.
 *    Parameter     :
 *      attr_id_list: list of IC attribute id.
 *      size        : byte lenth of the list.
 *      xc          : pointer to client structure.
 *      ic_attr     : pointer to IC nested attribute structure.
 *      value       : encoded byte data.
 *      size_return : returned byte length of the data.
 *      num_return  : returned number of IC attributes.
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
static int
EncodeICNestedAttributes (attr_id_list, size, xc, ic_attr, value, size_return, num_return)
     CARD8 *attr_id_list;
     int size;
     XIMClientRec *xc;
     XIMAttributes *ic_attr;
     CARD8 *value;
     int *size_return;
     int *num_return;
{
  CARD8 *data;
  CARD16 *buf_16;
  CARD32 *buf_32;
  int ret, len, total, num, attr_id;
  int have_more;

  ret = 0;
  total = num = 0;
  have_more = 1;
  while (size > 0 && have_more)
    {
      buf_16 = (CARD16 *) attr_id_list;
      attr_id = (int) GET_CARD16 (buf_16[0]);
      data = value + sizeof (CARD16) + sizeof (CARD16);
      buf_16 = (CARD16 *) data;
      buf_32 = (CARD32 *) data;
      switch (attr_id)
        {
        case ICArea:
          ret = EncodeRectangleAttr (&(ic_attr->area), data, &len);
          break;
        case ICAreaNeeded:
          ret = EncodeRectangleAttr (&(ic_attr->area_needed), data, &len);
          break;
        case ICSpotLocation:
          ret = EncodePointAttr (xc, data, &len);
          break;
        case ICColormap:
          len = sizeof (CARD32);
          buf_32[0] = GET_CARD32 (ic_attr->colormap);
          break;
        case ICStdColormap:
          len = sizeof (CARD32);
          buf_32[0] = GET_CARD32 (ic_attr->std_colormap);
          break;
        case ICForeground:
          len = sizeof (CARD32);
          buf_32[0] = GET_CARD32 (ic_attr->fg);
          break;
        case ICBackground:
          len = sizeof (CARD32);
          buf_32[0] = GET_CARD32 (ic_attr->bg);
          break;
        case ICBackgroundPixmap:
          len = sizeof (CARD32);
          buf_32[0] = GET_CARD32 (ic_attr->bg_pixmap);
          break;
        case ICFontSet:
          ret = EncodeFontSetAttr (ic_attr->fontset, data, &len);
          break;
        case ICLineSpace:
          len = sizeof (CARD32);
          buf_32[0] = GET_CARD32 (ic_attr->line_space);
          break;
        case ICCursor:
          len = sizeof (CARD32);
          buf_32[0] = GET_CARD32 (ic_attr->cursor);
          break;
        case ICSeparatorOfNestedList:
          len = 0;
          have_more = 0;
          break;
        default:
          have_more = 0;
          continue;
        }
      if (ret)
        return ret;
      buf_16 = (CARD16 *) value;
      buf_16[0] = GET_CARD16 (attr_id);
      buf_16[1] = GET_CARD16 (len);
      len = sizeof (CARD16) +   /* 2 attribute-id */
        sizeof (CARD16) +       /* 2 byte length of value */
        len +                   /* n value */
        PAD (len);              /* p Pad(n) */
      value += len;
      total += len;
      attr_id_list += sizeof (CARD16);
      size -= sizeof (CARD16);
      num++;
    }
  *size_return = total;
  *num_return = num;
  return 0;
}

/*
 *    Function Name : EncodeICAttributes()
 *    Description   : encode IC attribute structure to byte data.
 *    Parameter     :
 *      attr_id_list: list of IC attribute id.
 *      size        : byte lenth of the list.
 *      xc          : pointer to client structure.
 *      value       : encoded byte data.
 *      size_return : returned byte length of the data.
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
static int
EncodeICAttributes (attr_id_list, size, xc, value, size_return)
     CARD8 *attr_id_list;
     int size;
     XIMClientRec *xc;
     CARD8 *value;
     int *size_return;
{
  CARD8 *data;
  CARD16 *buf_16;
  CARD32 *buf_32;
  int ret, len, total, attr_id, num;
  int nested_size;

  ret = 0;
  total = 0;
  while (size > 0)
    {
      buf_16 = (CARD16 *) attr_id_list;
      attr_id = (int) GET_CARD16 (buf_16[0]);
      data = value + sizeof (CARD16) + sizeof (CARD16);
      buf_16 = (CARD16 *) data;
      buf_32 = (CARD32 *) data;
      switch (attr_id)
        {
        case ICInputStyle:
          len = sizeof (CARD32);
          buf_32[0] = GET_CARD32 (xc->input_style);
          break;
        case ICClientWindow:
          len = sizeof (CARD32);
          buf_32[0] = GET_CARD32 (xc->w);
          break;
        case ICFocusWindow:
          len = sizeof (CARD32);
          buf_32[0] = GET_CARD32 (xc->focus_window);
          break;
        case ICFilterEvents:
          len = sizeof (CARD32);
          buf_32[0] = GET_CARD32 (xc->filter_events);
          break;
        case ICPreeditAttributes:
          ret = EncodeICNestedAttributes ((attr_id_list + sizeof (CARD16)), (size - sizeof (CARD16)), xc, &(xc->pe), data, &len, &num);
          if (!ret)
            {
              nested_size = sizeof (CARD16) * num;
              attr_id_list += nested_size;
              size -= nested_size;
            }
          break;
        case ICStatusAttributes:
          ret = EncodeICNestedAttributes ((attr_id_list + sizeof (CARD16)), (size - sizeof (CARD16)), xc, &(xc->st), data, &len, &num);
          if (!ret)
            {
              nested_size = sizeof (CARD16) * num;
              attr_id_list += nested_size;
              size -= nested_size;
            }
          break;
        default:
          ret = XIM_BadProtocol;
        }
      if (ret)
        return ret;
      buf_16 = (CARD16 *) value;
      buf_16[0] = GET_CARD16 (attr_id);
      buf_16[1] = GET_CARD16 (len);
      len = sizeof (CARD16) +   /* 2 attribute-id */
        sizeof (CARD16) +       /* 2 byte length of value */
        len +                   /* n value */
        PAD (len);              /* p Pad(n) */
      value += len;
      total += len;
      attr_id_list += sizeof (CARD16);
      size -= sizeof (CARD16);
    }
  *size_return = total;
  return 0;
}

/*
 *    Function Name : DecodeIMAttributes()
 *    Description   : decode IM attributes.
 *    Parameter     :
 *      list        : list of IM attribute, packed data.
 *      size        : byte length of list.
 *      extension   : not used.
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
 /*ARGSUSED*/ static int
DecodeIMAttributes (list, size, extension)
     CARD8 *list;
     int size;
     XPointer extension;        /* for extenstion in future */
{
  CARD16 *buf_16;
  int ret, len, attr_id;

  ret = 0;
  while (size > 0)
    {
      buf_16 = (CARD16 *) list;
      attr_id = (int) GET_CARD16 (buf_16[0]);
      len = (int) GET_CARD16 (buf_16[1]);
      switch (attr_id)
        {
        case ICInputStyle:
          /* we don't allow to set InputStyle so far. */
          ret = XIM_BadStyle;
          break;
        default:
          /* no more supported so far */
          ret = XIM_BadSomething;
        }
      if (ret)
        return ret;
      len += sizeof (CARD16) +  /* 2 attribute-id */
        sizeof (CARD16) +       /* 2 n byte lenght of attr */
        len +                   /* n value of attr */
        PAD (len);              /* p Pad(n) */
      list += len;
      size -= len;
    }
  return 0;
}

/*
 *    Function Name : SetClientIMValues()
 *    Description   : decode IM values to client context.
 *    Parameter     :
 *      value       : IM values, packed data.
 *      size        : byte length of list.
 *      im          : client IM context.
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
static int
SetClientIMValues (value, size, im)
     CARD8 *value;
     int size;
     XIMContextRec *im;
{
  /* no any attribute can be set so far */
  return DecodeIMAttributes (value, size, (XPointer) NULL);
}

/*
 *    Function Name : EncodeInputStyleList()
 *    Description   : encode Input Style list to byte data.
 *    Parameter     :
 *      im          : pointer to client IM context.
 *      value       : encoded byte data.
 *      size_return : returned byte length of the data.
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
static int
EncodeInputStyleList (im, value, size_return)
     XIMContextRec *im;
     CARD8 *value;
     int *size_return;
{
  CARD16 *buf_16 = (CARD16 *) value;
  CARD32 *buf_32 = (CARD32 *) & buf_16[2];
  int num, i;

  num = MAX_SUPPORT_STYLE - 1;
  buf_16[0] = GET_CARD16 (num);
  for (i = 0; i <= num; i++)
    {
      buf_32[i] = GET_CARD32 (xim->supported_style[i]);
    }
  *size_return = sizeof (CARD16) +      /* 2 number of list */
    sizeof (CARD16) +           /* 2 unused */
    sizeof (CARD32) * MAX_SUPPORT_STYLE;
  return 0;
}

/*
 *    Function Name : EncodeIMAttributes()
 *    Description   : encode IM attribute structure to byte data.
 *    Parameter     :
 *      attr_id_list: list of attribute id.
 *      size        : byte length of the list.
 *      im          : pointer to client IM context.
 *      value       : encoded byte data.
 *      size_return : returned byte length of the data.
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
static int
EncodeIMAttributes (attr_id_list, size, im, value, size_return)
     CARD8 *attr_id_list;
     int size;
     XIMContextRec *im;
     CARD8 *value;
     int *size_return;
{
  CARD8 *data;
  CARD16 *buf_16;
  int ret, len, total, attr_id;

  ret = 0;
  total = 0;
  while (size > 0)
    {
      buf_16 = (CARD16 *) attr_id_list;
      attr_id = (int) GET_CARD16 (buf_16[0]);
      data = value + sizeof (CARD16) + sizeof (CARD16);
      switch (attr_id)
        {
        case ICInputStyle:
          ret = EncodeInputStyleList (im, data, &len);
          break;
        default:
          ret = XIM_BadSomething;
        }
      if (ret)
        return ret;
      buf_16 = (CARD16 *) value;
      buf_16[0] = GET_CARD16 (attr_id);
      buf_16[1] = GET_CARD16 (len);
      len = sizeof (CARD16) +   /* 2 attribute-id */
        sizeof (CARD16) +       /* 2 byte length of value */
        len +                   /* n value */
        PAD (len);              /* p Pad(n) */
      value += len;
      total += len;
      attr_id_list += sizeof (CARD16);
      size -= sizeof (CARD16);
    }
  *size_return = total;
  return 0;
}

/*
 *    Function Name : CreateClientIC()
 *    Description   : create an IC context and assign a new IC id.
 *    Parameter     :
 *      im_id       : client input-method-id.
 *      list        : list of IC attributes, packed data.
 *      size        : byte length of list.
 *      ic_id_return: new IC id of the create IC context.
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
static int
CreateClientIC (im_id, list, size, ic_id_return)
     XIMID im_id;
     CARD8 *list;
     int size;
     XICID *ic_id_return;
{
  XIMClientRec *xc;
  ximICValuesReq ic_req;
  ximICAttributesReq pre_req, st_req;
  char *pre_font = (char *) NULL;
  char *st_font = (char *) NULL;
  XIMNestLangList lc_list = (XIMNestLangList) NULL;
  XIMNestLangList cur_lc = (XIMNestLangList) NULL;
  XIMNestLangList p;
  int have_world = 0, cur_is_world = 0;
  char *use_langs, *cur_l, *c_data;
  int xic, ret, i;
  short detail;
  extern XIMClientRec *create_client ();

  /*
   * parse attributes
   */
  ret = DecodeICAttributes (list, size, &ic_req, &pre_req, &st_req, &pre_font, &st_font);
  if (ret)
    return ret;
  ic_req.max_keycode = 0;       /* set 0 as default */

  use_langs = cur_use_lang;     /* use locale from XIM_OPEN */
  cur_l = (char *) NULL;        /* current lang not specified yet. */
  c_data = (char *) NULL;       /* default user name. */

  /* refer to util.c: CreateIC() */

  /*
   * check resources.
   */
  if (CheckInputStyle (ic_req.input_style) == False)
    return XIM_BadStyle;
  if ((ic_req.input_style & (XIMPreeditArea | XIMPreeditPosition)) && (!(ic_req.mask & (1 << ICFontSet)) || !pre_font || !*pre_font))
    return XIM_BadName;
  if ((ic_req.input_style & XIMStatusArea) && (!(ic_req.mask & (1 << (ICFontSet + StatusOffset))) || !st_font || !*st_font))
    return XIM_BadName;
  if ((ic_req.input_style & XIMPreeditPosition) && (!(ic_req.mask & (1 << ICSpotLocation))))
    return XIM_BadSpotLocation;
  if (!(ic_req.input_style & XIMPreeditNothing))
    {
      if ((have_world = get_langlist (use_langs, &lc_list)) < 0)
        {
          return XIM_BadAlloc;
        }
      if (!lc_list)
        {
          return XIM_LocaleNotSupported;
        }
      if (cur_l && *cur_l)
        {
          for (p = lc_list; p != NULL; p = p->next)
            {
              if (!strcmp (p->lc_name, cur_l) || (p->alias_name && !strcmp (p->alias_name, cur_l)))
                break;
            }
          if (p == NULL)
            {
              if ((i = get_langlist (cur_l, &cur_lc)) < 0)
                {
                  return XIM_BadAlloc;
                }
              else if (i > 0)
                {
                  cur_is_world++;
                  free_langlist (cur_lc);
                  cur_lc = lc_list;
                }
              else
                {
                  if (cur_lc)
                    {
                      cur_lc->next = lc_list;
                    }
                  else
                    {
                      cur_lc = lc_list;
                    }
                }
            }
          else
            {
              cur_lc = p;
            }
        }
      else
        {
          if (have_world == 2)
            cur_is_world++;
          cur_lc = lc_list;
        }
    }
  xc = create_client (&ic_req, &pre_req, &st_req, lc_list, cur_lc, have_world, cur_is_world, pre_font, st_font, c_data, &detail);
  if (pre_font)
    XFree (pre_font);
  if (st_font)
    XFree (st_font);
  if (lc_list)
    free_langlist (lc_list);
  if (!xc)
    {
      return ConvErrorCode (detail);
    }
  xic = xim->client_count++;
  xc->number = xic;
  xc->im_id = (int) im_id;
  if (IsPreeditNothing (xc))
    {
      check_root_mapping (xc->root_pointer);
    }

  *ic_id_return = (XICID) xic;
  return 0;
}

/*
 *    Function Name : CreateIM()
 *    Description   : create a new IM context and register in the list.
 *    Parameter     :
 *      <none>
 *    Returned Value:
 *      > 0         : IM id of the new IM context created.
 *      = 0         : error
 */
static XIMID
CreateIM ()
{
  static XIMID base_im_id = BASE_IM_ID;
  XIMContextRec *im;

  im = (XIMContextRec *) Malloc (sizeof (XIMContextRec));
  if (!im)
    {
      malloc_error ("no memory to open an IM");
      return 0;
    }
  im->im_id = ALLOC_ID (base_im_id);

  /* add to IM list */
  im->next = ximlist;
  ximlist = im;
  return im->im_id;
}

/*
 *    Function Name : GetCompoundTextIndex()
 *    Description   : search the index of string named COMPOUND_TEXT in
 *                    LISTofSTR. 
 *    Parameter     :
 *      list        : list of string names LISTofSTR.
 *      size        : byte length of the list.
 *    Returned Value:
 *      int         : returned index in this list. If not found, it is
 *                    the default XIM_Default_Encoding_IDX.
 */
static int
GetCompoundTextIndex (list, size)
     register CARD8 *list;
     register int size;
{
  register int index, len;

  /* get index of COMPOUNT_TEXT which is only supported by us. */
  index = 0;
  while (size > 0)
    {
      len = (int) list[0];
      if (!strncmp (&list[1], "COMPOUND_TEXT", len))
        break;
      index++;
      list += (sizeof (CARD8) + len);
      size -= (sizeof (CARD8) + len);
    };
  if (size == 0)
    index = XIM_Default_Encoding_IDX;   /* use default */
  return index;
}

/*
 *    Function Name : FindExtensionByName()
 *    Description   : search an extension item with its name in extension
 *                    list supported by this server.
 *    Parameter     :
 *      name        : ext string name.
 *      len         : length of ext name.
 *    Returned Value:
 *      >= 0        : found item index to this list.
 *      = -1        : not found.
 */
static int
FindExtensionByName (name, len)
     char *name;
     int len;
{
  int ind;

  for (ind = 0; ind < XIMNumber (extension_list); ind++)
    {
      if (!strncmp (extension_list[ind].name, name, len))
        return ind;
    }
  return -1;
}

/*
 *    Function Name : EncodeExtension()
 *    Description   : encode an extension item to byte data.
 *    Parameter     :
 *      ind         : the item index to extension list.
 *      value       : byte data to store the packed ext item.
 *      size_return : returned byte size of the packed ext item
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
static int
EncodeExtension (ind, value, size_return)
     int ind;
     CARD8 *value;
     int *size_return;
{
  CARD16 *buf_16 = (CARD16 *) & value[2];
  int len;

  len = strlen (extension_list[ind].name);
  value[0] = extension_list[ind].major_code;
  value[1] = extension_list[ind].minor_code;
  buf_16[0] = GET_CARD16 (len);
  (void) strncpy ((char *) &buf_16[1], extension_list[ind].name, len);
  *size_return = sizeof (CARD8) +       /* 1 major_code */
    sizeof (CARD8) +            /* 1 minor_code */
    sizeof (CARD16) +           /* 2 n length of name */
    len +                       /* n name */
    PAD (len);                  /* p Pad(n) */
  return 0;
}

/*
 *    Function Name : GetExtensionList()
 *    Description   : get extensions supported by the server. 
 *    Parameter     :
 *      list        : list of extension names asked by client.
 *      size        : byte length of the list, if it is zero, client asks
 *                    all extensions supported by the server.
 *      value       : the returned exension names supported by server. It is
 *                    packed in byte.
 *      size_return : returned byte size of the packed ext names.
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
static int
GetExtensionList (list, size, value, size_return)
     CARD8 *list;
     int size;
     CARD8 *value;
     int *size_return;
{
  int len, ext_len, ind, total;

  total = 0;
  if (size == 0)
    {
      for (ind = 0; ind < XIMNumber (extension_list); ind++)
        {
          (void) EncodeExtension (ind, value, &ext_len);
          value += ext_len;
          total += ext_len;
        }
    }
  else
    {
      while (size > 0)
        {
          len = (int) list[0];
          if ((ind = FindExtensionByName ((char *) &list[1], len)) >= 0)
            {
              (void) EncodeExtension (ind, value, &ext_len);
              value += ext_len;
              total += ext_len;
            }
          list += (sizeof (CARD8) + len);
          size -= (sizeof (CARD8) + len);
        }
    }
  *size_return = total;
  return 0;
}

/*
 *    Function Name : RecvRequest()
 *    Description   : receive a request of XIM protocol from IM library.
 *    Parameter     :
 *      req         : pointer to a request structure, returned.
 *    Returned Value:
 *      >=0         : byte length of request data except request head.
 *      < 0         : error in reading data.
 */
static int
RecvRequest (req)
     XimRequestPacket *req;
{
  CARD8 *buf, *read_ptr;
  CARD8 byteOrder;
  int read_size;
  extern char my_byteOrder;

  if (_ReadFromClient ((char *) req, XIM_HEADER_SIZE) == -1)
    return -1;
  if (req->length == 0)
    return 0;
  read_size = req->length * 4;
  read_ptr = buf = MALLOC_REQUEST (read_size);
  bzero ((char *) buf, read_size);
  if (req->major_opcode == XIM_CONNECT)
    {
      _ReadFromClient ((char *) &byteOrder, 1);
      if (my_byteOrder != (char) byteOrder)
        cur_cblk->byteOrder = True;
      else
        cur_cblk->byteOrder = False;
      buf[0] = byteOrder;
      read_ptr = &buf[1];
      read_size--;
    }
  if (_ReadFromClient (read_ptr, read_size) == -1)
    {
      FREE (buf);
      return -1;
    }
  req->data = buf;
  return req->length * 4;
}

/*
 *    Function Name : SendIdOnly()
 *    Description   : send a request that is the format of:
 *                        CARD16 input-method-id
 *                        CARD16 input-context-id
 *    Parameter     :
 *      im_id       : client input-method-id.
 *      ic_id       : client input-context-id.
 *      major_code  : major opcode of this request.
 *      minor_code  : minor opcode of this request.
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
static int
SendIdOnly (im_id, ic_id, major_code, minor_code)
     XIMID im_id;
     XICID ic_id;
     CARD8 major_code;
     CARD8 minor_code;
{
  CARD16 *buf_16;
  CARD8 *reply;
  int len;

  /* reply */
  len = sizeof (CARD16) +       /* 2 input-method-ID */
    sizeof (CARD16);            /* 2 input-context-ID */
  reply = MALLOC_REPLY (XIM_HEADER_SIZE + len);
  len += AddHead (reply, major_code, minor_code, len);
  buf_16 = (CARD16 *) (reply + XIM_HEADER_SIZE);
  buf_16[0] = GET_CARD16 (im_id);
  buf_16[1] = GET_CARD16 (ic_id);
  if (_WriteToClientFlush (reply, len) < 0)
    {
      FREE (reply);
      return XIM_BadSomething;
    }
  FREE (reply);
  return 0;
}

/*
 *    Function Name : SendSyncReply()
 *    Description   : If server has not syncronized, do sync. Then
 *                    send a XIM_SYNC_REPLY to IM library.
 *    Parameter     :
 *      im_id       : client input-method-id.
 *      ic_id       : client input-context-id.
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
static int
SendSyncReply (im_id, ic_id)
     XIMID im_id;
     XICID ic_id;
{
  if (!AllSyncDone (im_id, ic_id))
    {
      SyncAll (im_id, ic_id);
    }
  return SendIdOnly (im_id, ic_id, XIM_SYNC_REPLY, 0);
}

/*
 *    Function Name : RecvSync()
 *    Description   : process of client request XIM_SYNC.
 *    Parameter     :
 *      req         : pointer to a request structure.
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
static int
RecvSync (req)
     XimRequestPacket *req;
{
  CARD16 *buf_16 = (CARD16 *) req->data;
  XIMID im_id;
  XICID ic_id;

  im_id = cur_im_id = (XIMID) GET_CARD16 (buf_16[0]);   /* 2 input-method-ID */
  if (!GetIMWithId (im_id))
    return XIM_BadProtocol;

  ic_id = cur_ic_id = (XICID) GET_CARD16 (buf_16[1]);   /* 2 input-context-ID */
  if (!GetICWithId (ic_id))
    return XIM_BadProtocol;

  return SendSyncReply (im_id, ic_id);
}

/*
 *    Function Name : RecvSyncReply()
 *    Description   : process of client request XIM_SYNC_REPLY.
 *    Parameter     :
 *      req         : pointer to a request structure.
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
static int
RecvSyncReply (req)
     XimRequestPacket *req;
{
  /* no action. */
  return 0;
}

/*
 *    Function Name : RecvError()
 *    Description   : process of client request XIM_ERROR.
 *    Parameter     :
 *      req         : pointer to a request structure.
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
static int
RecvError (req)
     XimRequestPacket *req;
{
  CARD16 *buf_16 = (CARD16 *) req->data;
  XIMID im_id;                  /* not used yet */
  XICID ic_id;                  /* not used yet */
  int eflag, ecode, esnum;

  im_id = (XIMID) GET_CARD16 (buf_16[0]);       /* 2 input-method-ID */
  ic_id = (XICID) GET_CARD16 (buf_16[1]);       /* 2 input-context-ID */
  eflag = (int) GET_CARD16 (buf_16[2]); /* 2 flag */
  ecode = (int) GET_CARD16 (buf_16[3]); /* 2 error code */
  esnum = (int) GET_CARD16 (buf_16[4]); /* 2 n length */
  if (esnum > 0)
    {
      char *emsg;
      if (!(emsg = Malloc (esnum + 1)))
        {
          return XIM_BadAlloc;
        }
      (void) strncpy (emsg, (char *) &buf_16[6], esnum);
      emsg[esnum] = '\0';
      print_out3 ("XIM Protocol Error: %d %s\n", ecode, emsg);
      XFree (emsg);
    }
  else
    {
      print_out2 ("XIM Protocol Error: %d\n", ecode);
    }
  return 0;
}

/*
 *    Function Name : SendError()
 *    Description   : send error message to IM library with the request
 *                    XIM_ERROR.
 *    Parameter     :
 *      err_code    : error code.
 *    Returned Value:
 *      <none>
 */
static void
SendError (err_code)
     CARD16 err_code;
{
  CARD16 *buf_16;
  CARD8 *reply;
  int len;

  len = sizeof (CARD16) +       /* 2 input-method-ID */
    sizeof (CARD16) +           /* 2 input-context-ID */
    sizeof (BITMASK16) +        /* 2 flag */
    sizeof (CARD16) +           /* 2 error code */
    sizeof (INT16) +            /* 2 byte length of error detail */
    sizeof (CARD16);            /* 2 type of error detail */
/* empty *//* n error detail */

  reply = MALLOC_REPLY (XIM_HEADER_SIZE + len);
  len += AddHead (reply, XIM_ERROR, 0, len);
  buf_16 = (CARD16 *) (reply + XIM_HEADER_SIZE);
  buf_16[0] = GET_CARD16 (cur_im_id);
  buf_16[1] = GET_CARD16 (cur_ic_id);
  buf_16[2] = GET_CARD16 (cur_status);
  buf_16[3] = GET_CARD16 (err_code);
  buf_16[4] = 0;                /* no error detail */
  buf_16[5] = 0;                /* no type */
  (void) _WriteToClientFlush (reply, len);
  FREE (reply);
}

/*
 *    Function Name : SendAuthRequired()
 *    Description   : reply XIM_AUTH_REQUIRED as client ask auth.
 *    Parameter     :
 *      auth_index  : index of list of auth names.
 *      auth_data   : auth data. 
 *      length_data : byte length of data.
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
static int
SendAuthRequired (auth_index, auth_data, length_data)
     CARD8 auth_index;
     CARD8 *auth_data;
     INT16 length_data;
{
  CARD8 *buf_8;
  CARD16 *buf_16;
  CARD8 *reply;
  int len;

  len = sizeof (CARD8) +        /* 1 auth-protocol_index */
    sizeof (CARD8) * 3 +        /* 3 unused */
    sizeof (CARD16) +           /* 2 length of auth data */
    sizeof (CARD16) +           /* 2 unused */
    length_data +               /* n data */
    PAD (length_data);          /* p unused, p = Pad(n) */

  reply = MALLOC_REPLY (XIM_HEADER_SIZE + len);
  len += AddHead (reply, XIM_AUTH_REQUIRED, 0, len);
  buf_8 = (CARD8 *) (reply + XIM_HEADER_SIZE);
  buf_16 = (CARD16 *) (reply + XIM_HEADER_SIZE + sizeof (CARD8) * 4);
  buf_8[0] = (CARD8) auth_index;
  buf_16[0] = GET_CARD16 (length_data);
  if (auth_data && length_data > 0)
    {
      buf_8 = (CARD8 *) (reply + XIM_HEADER_SIZE + sizeof (CARD8) * 4 + sizeof (CARD16) * 2);
      memcpy ((char *) buf_8, (char *) auth_data, length_data);
    }
  if (_WriteToClientFlush (reply, len) < 0)
    {
      FREE (reply);
      return XIM_BadSomething;
    }
  FREE (reply);
  return 0;
}

/*
 *    Function Name : SendConnectReply()
 *    Description   : reply connection from IM library.
 *    Parameter     :
 *      major_version
 *                  : major version of XIM protocol
 *      minor_version
 *                  : minor version of XIM protocol
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
static int
SendConnectReply (major_version, minor_version)
     CARD16 major_version;
     CARD16 minor_version;
{
  CARD16 *buf_16;
  CARD8 *reply;
  int len;

  len = sizeof (CARD16) +       /* 2 server-major-protocol-version */
    sizeof (CARD16);            /* 2 server-minor-protocol-version */
  reply = MALLOC_REPLY (XIM_HEADER_SIZE + len);
  len += AddHead (reply, XIM_CONNECT_REPLY, 0, len);
  buf_16 = (CARD16 *) (reply + XIM_HEADER_SIZE);
  buf_16[0] = GET_CARD16 (major_version);
  buf_16[1] = GET_CARD16 (minor_version);
  if (_WriteToClientFlush (reply, len) < 0)
    {
      FREE (reply);
      return XIM_BadSomething;
    }
  FREE (reply);
  return 0;
}

/*
 *    Function Name : RecvConnect()
 *    Description   : do connection with IM client.
 *    Parameter     :
 *      req         : pointer to a request structure.
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
static int
RecvConnect (req)
     XimRequestPacket *req;
{
  CARD16 *buf_16 = (CARD16 *) (req->data + 2);
  CARD16 major_version, minor_version, auth_num;
  int len;

  len = req->length * 4;
/* we have touched in RecvRequest *//* 1 byte order */
/* not used *//* 1 unused */
  major_version = GET_CARD16 (buf_16[0]);       /* 2 major protocol version */
  minor_version = GET_CARD16 (buf_16[1]);       /* 2 minor protocol version */
  auth_num = GET_CARD16 (buf_16[2]);    /* 2 number of auth. names */

  if (major_version != PROTOCOLMAJORVERSION || minor_version != PROTOCOLMINORVERSION)
    return XIM_BadProtocol;

  len -= sizeof (CARD8) + sizeof (CARD8) + sizeof (CARD16) + sizeof (CARD16) + sizeof (CARD16);

  if (auth_num > 0)
    {
      if (GetAuth (&buf_16[3], auth_num, &len, NULL) == False)
        {
          return XIM_BadSomething;      /* error in get auth. */
        }
      return SendAuthRequired ((CARD8) 0, (CARD8 *) NULL, (INT16) 0);
    }
  else
    {
      return SendConnectReply (PROTOCOLMAJORVERSION, PROTOCOLMINORVERSION);
    }
}

/*
 *    Function Name : RecvDisconnect()
 *    Description   : disconnect with IM client.
 *    Parameter     :
 *      req         : pointer to a request structure.
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
static int
RecvDisconnect (req)
     XimRequestPacket *req;
{
  CARD8 *reply;
  int len;
  extern int cur_sock;

  close_socket (cur_sock);

  /* reply */
  len = 0;                      /* no data */
  reply = MALLOC_REPLY (XIM_HEADER_SIZE + len);
  len += AddHead (reply, XIM_DISCONNECT_REPLY, 0, len);
  if (_WriteToClientFlush (reply, len) < 0)
    {
      FREE (reply);
      return XIM_BadSomething;
    }
  FREE (reply);
  return 0;
}

#ifdef DYNAMIC_EVENT_FLOW_MODEL

/*
 *    Function Name : SendRegisterTriggerKeys()
 *    Description   : ask client to register trigger keys which will
 *                    turn on or off input method. 
 *    Parameter     :
 *      im_id       : client input-method-id.
 *    Returned Value:
 *      <none>
 */
static void
SendRegisterTriggerKeys (im_id)
     XIMID im_id;
{
  CARD8 *reply;
  CARD16 *buf_16;
  CARD32 *buf_32;
  CARD32 onkeys[3];
  int onkeys_len;
  int len;

  onkeys[0] = GET_CARD32 (trigger_keys_list[0]);
  onkeys[1] = GET_CARD32 (trigger_keys_list[1]);
  onkeys[2] = GET_CARD32 (trigger_keys_list[2]);
  onkeys_len = sizeof (CARD32) * 3;

  /* send */
  len = sizeof (CARD16) +       /* 2 input-method-ID */
    sizeof (CARD16) +           /* 2 unused */
    sizeof (CARD32) +           /* 4 n byte length */
    onkeys_len +                /* n LISTofXIMTRIGGERKEY */
    sizeof (CARD32) +           /* 4 m byte length */
    onkeys_len;                 /* m LISTofXIMTRIGGERKEY */

  reply = MALLOC_REPLY (XIM_HEADER_SIZE + len);
  len += AddHead (reply, XIM_REGISTER_TRIGGERKEYS, 0, len);
  buf_16 = (CARD16 *) (reply + XIM_HEADER_SIZE);
  buf_16[0] = GET_CARD16 (im_id);
  buf_32 = (CARD32 *) & buf_16[2];
  buf_32[0] = GET_CARD32 (onkeys_len);
  (void) memcpy ((char *) &buf_32[1], (char *) onkeys, onkeys_len);
  buf_32 = (CARD32 *) (reply + sizeof (CARD16) + sizeof (CARD16) + sizeof (CARD32) + onkeys_len);
  buf_32[0] = GET_CARD32 (onkeys_len);  /* off keys same as on-keys */
  (void) memcpy ((char *) &buf_32[1], (char *) onkeys, onkeys_len);
  (void) _WriteToClientFlush (reply, len);
  FREE (reply);
}

/*
 *    Function Name : ToggleHenkan()
 *    Description   : turn on or off kanji henkan.
 */
static void
ToggleHenkan (keys_index, mask)
     int keys_index;
     unsigned long mask;
{
  /* not yet */
}

/*
 *    Function Name : RecvTriggerNotify()
 *    Description   : Some trigger key has pressed by client, it means
 *                    client asks some input method. do it.
 *    Parameter     :
 *      req         : pointer to a request structure.
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
static int
RecvTriggerNotify (req)
     XimRequestPacket *req;
{
  CARD16 *buf_16 = (CARD16 *) req->data;
  CARD32 *buf_32;
  CARD8 *reply;
  XIMID im_id;
  XICID ic_id;
  int len;
  int ret, n;
  int keys_flag, keys_index;
  unsigned long mask;
  XIMClientRec *xc;

  im_id = cur_im_id = (XIMID) GET_CARD16 (buf_16[0]);   /* 2 input-method-ID */
  if (!GetIMWithId (im_id))
    return XIM_BadProtocol;

  ic_id = cur_ic_id = (XICID) GET_CARD16 (buf_16[1]);   /* 2 input-context-ID */
  if (!(xc = GetICWithId (ic_id)))
    return XIM_BadProtocol;

  buf_32 = (CARD32 *) & buf_16[2];
  keys_flag = (int) GET_CARD32 (buf_32[0]);
  keys_index = (int) GET_CARD32 (buf_32[1]);
  mask = (unsigned long) GET_CARD32 (buf_32[2]);

  if (!keys_flag)
    {
      /* trigger on */
      ToggleHenkan (keys_index, mask);
    }
  else
    {
      /* trigger off */
      ToggleHenkan (keys_index, mask);
    }

  /* reply */
  return SendIdOnly (im_id, ic_id, XIM_TRIGGER_NOTIFY_REPLY, 0);
}

#endif /* DYNAMIC_EVENT_FLOW_MODEL */

/*
 *    Function Name : RecvOpen()
 *    Description   : open an IM context for client.
 *    Parameter     :
 *      req         : pointer to a request structure.
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
static int
RecvOpen (req)
     XimRequestPacket *req;
{
  CARD8 *reply;
  CARD16 *buf_16;
  XIMID im_id;
  int len;

  if (_CheckLocaleName (req->data, cur_use_lang) == False)
    return XIM_LocaleNotSupported;

  if (!(im_id = CreateIM ()))
    return XIM_BadAlloc;

#ifdef DYNAMIC_EVENT_FLOW_MODEL
  SendRegisterTriggerKeys (im_id);
#endif /* DYNAMIC_EVENT_FLOW_MODEL */

  /* reply */
  len = sizeof (CARD16) +       /* 2 input-method-id */
    sizeof (CARD16) +           /* 2 byte length of IM attr. */
    imattr_list_size +          /* n IM attr. supported */
    sizeof (CARD16) +           /* 2 byte length of IC attr. */
    sizeof (CARD16) +           /* 2 unused */
    icattr_list_size;           /* m IC attr. supported */
  reply = MALLOC_REPLY (XIM_HEADER_SIZE + len);
  len += AddHead (reply, XIM_OPEN_REPLY, 0, len);
  buf_16 = (CARD16 *) (reply + XIM_HEADER_SIZE);
  buf_16[0] = GET_CARD16 (im_id);
  buf_16[1] = GET_CARD16 (imattr_list_size);
  memcpy ((char *) &buf_16[2], (char *) imattr_list, imattr_list_size);

  buf_16 = (CARD16 *) (reply + XIM_HEADER_SIZE + sizeof (CARD16) + sizeof (CARD16) + imattr_list_size);
  buf_16[0] = GET_CARD16 (icattr_list_size);
  memcpy ((char *) &buf_16[2], (char *) icattr_list, icattr_list_size);

  if (_WriteToClientFlush (reply, len) < 0)
    {
      FREE (reply);
      return XIM_BadSomething;
    }
  FREE (reply);
  return 0;
}

/*
 *    Function Name : RecvClose()
 *    Description   : close an IM client.
 *    Parameter     :
 *      req         : pointer to a request structure.
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
static int
RecvClose (req)
     XimRequestPacket *req;
{
  CARD16 *buf_16 = (CARD16 *) req->data;
  CARD8 *reply;
  XIMID im_id;
  int len;

  im_id = cur_im_id = GET_CARD16 (buf_16[0]);   /* 2 input-method-ID */
  if (RemoveIMWithId (im_id) == False)
    return XIM_BadProtocol;
  cur_im_id = 0;

  /* reply */
  len = sizeof (CARD16) +       /* 2 input-method-ID */
    sizeof (CARD16);            /* 2 unused */
  reply = MALLOC_REPLY (XIM_HEADER_SIZE + len);
  len += AddHead (reply, XIM_CLOSE_REPLY, 0, len);
  buf_16 = (CARD16 *) (reply + XIM_HEADER_SIZE);
  buf_16[0] = GET_CARD16 (im_id);
  if (_WriteToClientFlush (reply, len) < 0)
    {
      FREE (reply);
      return XIM_BadSomething;
    }
  FREE (reply);
  return 0;
}

/*
 *    Function Name : RecvQueryExtension()
 *    Description   : send extension lists supported by server as the
 *                    client asks.
 *    Parameter     :
 *      req         : pointer to a request structure.
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
static int
RecvQueryExtension (req)
     XimRequestPacket *req;
{
  CARD16 *buf_16 = (CARD16 *) (req->data);
  CARD8 *reply;
  XIMID im_id;
  CARD8 value[BUFSIZ];
  int len, value_len;
  int ret, n;

  im_id = cur_im_id = (XIMID) GET_CARD16 (buf_16[0]);   /* 2 input-method-ID */
  if (!GetIMWithId (im_id))
    {
      return XIM_BadProtocol;
    }

  /* get extetions supported by us. */
  n = GET_CARD16 (buf_16[1]);   /* 2 n byte length */
  ret = GetExtensionList ((CARD8 *) & buf_16[2], n, value, &value_len);
  if (ret)
    return ret;

  /* reply */
  len = sizeof (CARD16) +       /* 2 input-method-ID */
    sizeof (CARD16) +           /* 2 n byte length */
    value_len;                  /* n LISTofEXT */
  reply = MALLOC_REPLY (XIM_HEADER_SIZE + len);
  len += AddHead (reply, XIM_QUERY_EXTENSION_REPLY, 0, len);
  buf_16 = (CARD16 *) (reply + XIM_HEADER_SIZE);
  buf_16[0] = GET_CARD16 (im_id);
  buf_16[1] = GET_CARD16 (value_len);
  (void) memcpy ((char *) &buf_16[2], (char *) value, value_len);
  if (_WriteToClientFlush (reply, len) < 0)
    {
      FREE (reply);
      return XIM_BadSomething;
    }
  FREE (reply);
  return 0;
}

/*
 *    Function Name : RecvEncodingNegotiation()
 *    Description   : decide COMPOUND_TEXT as the result of encoding
 *                    negotiation between IM client and server.
 *    Parameter     :
 *      req         : pointer to a request structure.
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
static int
RecvEncodingNegotiation (req)
     XimRequestPacket *req;
{
  CARD16 *buf_16 = (CARD16 *) req->data;
  CARD8 *reply;
  XIMID im_id;
  int len, n, index;

  im_id = cur_im_id = (XIMID) GET_CARD16 (buf_16[0]);   /* 2 input-method-ID */
  if (!GetIMWithId (im_id))
    return XIM_BadProtocol;

  /* we care only the index of COMPOUNT_TEXT */
  n = GET_CARD16 (buf_16[1]);   /* 2 byte length */
  index = GetCompoundTextIndex ((CARD8 *) & buf_16[2], n);

  /* reply */
  len = sizeof (CARD16) +       /* 2 input-method-ID */
    sizeof (CARD16) +           /* 2 category of the encoding */
    sizeof (CARD16) +           /* 2 index of the encoding */
    sizeof (CARD16);            /* 2 unused */
  reply = MALLOC_REPLY (XIM_HEADER_SIZE + len);
  len += AddHead (reply, XIM_ENCODING_NEGOTIATION_REPLY, 0, len);
  buf_16 = (CARD16 *) (reply + XIM_HEADER_SIZE);
  buf_16[0] = GET_CARD16 (im_id);
  buf_16[1] = GET_CARD16 (XIM_Encoding_NameCategory);
  buf_16[2] = GET_CARD16 (index);
  if (_WriteToClientFlush (reply, len) < 0)
    {
      FREE (reply);
      return XIM_BadSomething;
    }
  FREE (reply);
  return 0;
}

/*
 *    Function Name : SendSetEventMask()
 *    Description   : send XEvent mask to client which will foward any
 *                    event to the server.
 *    Parameter     :
 *      req         : pointer to a request structure.
 *    Returned Value:
 *      <none>
 */
static void
SendSetEventMask (im_id, ic_id)
     XIMID im_id;
     XICID ic_id;
{
  CARD8 *reply;
  CARD16 *buf_16;
  CARD32 *buf_32;
  int len;

  /* send */
  len = sizeof (CARD16) +       /* 2 input-method-ID */
    sizeof (CARD16) +           /* 2 input-context-ID */
    sizeof (BITMASK32) +        /* 4 forw-event-mask */
    sizeof (BITMASK32);         /* 4 sync-event-mask */
  reply = MALLOC_REPLY (XIM_HEADER_SIZE + len);
  len += AddHead (reply, XIM_SET_EVENT_MASK, 0, len);
  buf_16 = (CARD16 *) (reply + XIM_HEADER_SIZE);
  buf_16[0] = GET_CARD16 (im_id);
  buf_16[1] = GET_CARD16 (ic_id);
  buf_32 = (CARD32 *) & buf_16[2];
  buf_32[0] = (KeyPressMask | StructureNotifyMask);
  buf_32[1] = (KeyPressMask | StructureNotifyMask);
  (void) _WriteToClientFlush (reply, len);
  FREE (reply);
}

/*
 *    Function Name : RecvCreateIC()
 *    Description   : create an IC for the connected client.
 *    Parameter     :
 *      req         : pointer to a request structure.
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
static int
RecvCreateIC (req)
     XimRequestPacket *req;
{
  CARD16 *buf_16 = (CARD16 *) req->data;
  XIMID im_id;
  XICID ic_id;
  int ret, len;

  im_id = cur_im_id = (XIMID) GET_CARD16 (buf_16[0]);   /* 2 input-method-ID */
  if (!GetIMWithId (im_id))
    return XIM_BadProtocol;

  /* create IC. */
  len = (int) GET_CARD16 (buf_16[1]);
  ret = CreateClientIC (im_id, (CARD8 *) & buf_16[2], len, &ic_id);
  if (ret)
    return ret;
  cur_ic_id = ic_id;

  /* reply */
  ret = SendIdOnly (im_id, ic_id, XIM_CREATE_IC_REPLY, 0);
  if (ret)
    return ret;

  SendSetEventMask (im_id, ic_id);
  return 0;
}

/*
 *    Function Name : RecvSetICValues()
 *    Description   : set IC values to internal client context.
 *    Parameter     :
 *      req         : pointer to a request structure.
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
static int
RecvSetICValues (req)
     XimRequestPacket *req;
{
  CARD16 *buf_16 = (CARD16 *) req->data;
  XIMID im_id;
  XICID ic_id;
  int ret, n;

  XIMClientRec *xc;

  im_id = cur_im_id = (XIMID) GET_CARD16 (buf_16[0]);   /* 2 input-method-ID */
  if (!GetIMWithId (im_id))
    return XIM_BadProtocol;

  ic_id = cur_ic_id = (XICID) GET_CARD16 (buf_16[1]);   /* 2 input-context-ID */
  if (!(xc = GetICWithId (ic_id)))
    return XIM_BadProtocol;

  n = (int) GET_CARD16 (buf_16[2]);     /* 2 n byte length */
  ret = SetClientICValues ((CARD8 *) & buf_16[4], n, xc);
  if (ret)
    return ret;

  /* reply */
  return SendIdOnly (im_id, ic_id, XIM_SET_IC_VALUES_REPLY, 0);
}

/*
 *    Function Name : RecvGetICValues()
 *    Description   : encode IC values of client context, then send to client.
 *    Parameter     :
 *      req         : pointer to a request structure.
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
static int
RecvGetICValues (req)
     XimRequestPacket *req;
{
  CARD16 *buf_16 = (CARD16 *) req->data;
  CARD8 *reply;
  CARD8 value[BUFSIZ];
  XIMID im_id;
  XICID ic_id;
  int len, value_len, n;
  int ret;

  XIMClientRec *xc;

  im_id = cur_im_id = (XIMID) GET_CARD16 (buf_16[0]);   /* 2 input-method-ID */
  if (!GetIMWithId (im_id))
    return XIM_BadProtocol;

  ic_id = cur_ic_id = (XICID) GET_CARD16 (buf_16[1]);   /* 2 input-context-ID */
  if (!(xc = GetICWithId (ic_id)))
    return XIM_BadProtocol;

  n = GET_CARD16 (buf_16[2]);
  ret = EncodeICAttributes ((CARD8 *) & buf_16[3], n, xc, value, &value_len);
  if (ret)
    return ret;

  /* reply */
  len = sizeof (CARD16) +       /* 2 input-method-ID */
    sizeof (CARD16) +           /* 2 input-context-ID */
    sizeof (INT16) +            /* 2 byte length of ic-attr */
    sizeof (CARD16) +           /* 2 unused */
    value_len;                  /* n list of ic-attr */

  /* reply */
  reply = MALLOC_REPLY (XIM_HEADER_SIZE + len);
  len += AddHead (reply, XIM_GET_IC_VALUES_REPLY, 0, len);
  buf_16 = (CARD16 *) (reply + XIM_HEADER_SIZE);
  buf_16[0] = GET_CARD16 (im_id);
  buf_16[1] = GET_CARD16 (ic_id);
  buf_16[2] = GET_CARD16 (value_len);
  (void) memcpy ((char *) &buf_16[4], (char *) value, value_len);
  if (_WriteToClientFlush (reply, len) < 0)
    {
      FREE (reply);
      return XIM_BadSomething;
    }
  FREE (reply);
  return 0;
}

/*
 *    Function Name : JWMline_get()
 *    Description   : put preedit string into a common shared CT buffer which
 *                    can be accessed by GET_COMMON_CT_BUFFER, i.e., "ct_buf". 
 *    Parameter     :
 *      base        : beginning position of preedit string.
 *      pre_len_returned
 *                  : returned length of the string got.
 *    Returned Value:
 *      <none>
 */
static void
JWMline_get (base, pre_len_returned)
     int base;
     int *pre_len_returned;
{
  register XIMLangRec *xl;
  register int wc_len, send_len;
  register wchar *JW_buf;

  *pre_len_returned = 0;
  xl = cur_p->cur_xl;
  JW_buf = xl->buf + base;
  send_len = wc_len = xl->max_pos - base;
  if (wc_len > 0)
    {
      while (1)
        {
          send_len = wchar_to_ct (cur_p->cur_xl->xlc, JW_buf, ct_buf, wc_len, ct_buf_max);
          if (send_len < -1)
            {
              return;
            }
          else if (send_len == -1)
            {
              if (realloc_ct_buf () < 0)
                return;
            }
          else
            {
              break;
            }
        }
    }
  *pre_len_returned = ((send_len >= 0) ? send_len : 0);
}

/*
 *    Function Name : ResetClientIC()
 *    Description   : reset IC context of client.
 *    Parameter     :
 *      xc          : pointer to client structure.
 *      pre_str_filled
 *                  : returned preedit string. 
 *                    The pre_str_filled parameter is only for readability,
 *                    it is used in a global variable "ct_buf".
 *      pre_len_returned
 *                  : length of preedit string 
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
 /*ARGSUSED*/ static int
ResetClientIC (xc, pre_str_filled, pre_len_returned)
     XIMClientRec *xc;
     unsigned char *pre_str_filled;     /* only for readable ct_buf */
     int *pre_len_returned;
{
  WnnClientRec *save_c_c;
  XIMClientRec *save_cur_p, *save_cur_x;
  int ret;

  /* refer to util.c: ResetIC() */
  {
    save_c_c = c_c;
    save_cur_p = cur_p;
    save_cur_x = cur_x;
    cur_x = xc;
    if (IsPreeditNothing (xc))
      {
        cur_p = xc->root_pointer->ximclient;
      }
    else
      {
        cur_p = cur_x;
      }
    c_c = cur_p->cur_xl->w_c;
    ret = reset_c_b ();
    JWMline_get (0, pre_len_returned);
    JWMline_clear (0);
    c_c = save_c_c;
    cur_p = save_cur_p;
    cur_x = save_cur_x;
    if (ret == -1)
      return XIM_BadProtocol;
  }
  return 0;
}

/*
 *    Function Name : RecvResetIC()
 *    Description   : reset IC context as client asks.
 *    Parameter     :
 *      req         : pointer to a request structure.
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
static int
RecvResetIC (req)
     XimRequestPacket *req;
{
  CARD16 *buf_16 = (CARD16 *) req->data;
  CARD8 *reply;
  XIMID im_id;
  XICID ic_id;
  int len, pre_len;
  int ret;
  unsigned char *pre_str;

  XIMClientRec *xc;

  im_id = cur_im_id = (XIMID) GET_CARD16 (buf_16[0]);   /* 2 input-method-ID */
  if (!GetIMWithId (im_id))
    return XIM_BadProtocol;

  ic_id = cur_ic_id = (XICID) GET_CARD16 (buf_16[1]);   /* 2 input-context-ID */
  if (!(xc = GetICWithId (ic_id)))
    return XIM_BadProtocol;

  pre_str = GET_SHARED_CT_BUFFER ();
  ret = ResetClientIC (xc, pre_str, &pre_len);
  if (ret)
    return ret;

  /* reply */
  len = sizeof (CARD16) +       /* 2 input-method-ID */
    sizeof (CARD16) +           /* 2 input-context-ID */
    sizeof (INT16) +            /* 2 byte length of string */
    pre_len +                   /* n LISTofBYTE */
    PAD (sizeof (INT16) + pre_len);     /* p Pad(2 + n) */

  /* reply */
  reply = MALLOC_REPLY (XIM_HEADER_SIZE + len);
  len += AddHead (reply, XIM_RESET_IC_REPLY, 0, len);
  buf_16 = (CARD16 *) (reply + XIM_HEADER_SIZE);
  buf_16[0] = GET_CARD16 (im_id);
  buf_16[1] = GET_CARD16 (ic_id);
  buf_16[2] = GET_CARD16 (pre_len);
  if (pre_len > 0)
    (void) memcpy ((char *) &buf_16[3], (char *) pre_str, pre_len);

  if (_WriteToClientFlush (reply, len) < 0)
    {
      FREE (reply);
      return XIM_BadSomething;
    }
  FREE (reply);
  return 0;
}

/*
 *    Function Name : RecvDestroyIC()
 *    Description   : destroy an IC context.
 *    Parameter     :
 *      req         : pointer to a request structure.
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
static int
RecvDestroyIC (req)
     XimRequestPacket *req;
{
  CARD16 *buf_16 = (CARD16 *) req->data;
  XIMID im_id;
  XICID ic_id;

  XIMClientRec *xc;

  im_id = cur_im_id = (XIMID) GET_CARD16 (buf_16[0]);   /* 2 input-method-ID */
  if (!GetIMWithId (im_id))
    return XIM_BadProtocol;

  ic_id = cur_ic_id = (XICID) GET_CARD16 (buf_16[1]);   /* 2 input-context-ID */
  if (!(xc = GetICWithId (ic_id)))
    return XIM_BadProtocol;
  (void) destroy_client (xc);
  cur_ic_id = 0;

  /* reply */
  return SendIdOnly (im_id, ic_id, XIM_DESTROY_IC_REPLY, 0);
}

/*
 *    Function Name : SetClientICFocus()
 *    Description   : set IC focus to the specified client.
 *    Parameter     :
 *      xc          : pointer to client structure.
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
static int
SetClientICFocus (xc)
     XIMClientRec *xc;
{
  register XIMInputRec *xi;
  XIMClientRec *save_cur_p, *save_cur_x;
  WnnClientRec *save_c_c;

  /* modified from util.c: SetICFocus() */
  {
    for (xi = input_list; xi != NULL; xi = xi->next)
      {
        if (xi->w == xc->focus_window)
          break;
      }
    if (xi == NULL)
      {
        if (!(xi = (XIMInputRec *) Malloc (sizeof (XIMInputRec))))
          {
            malloc_error ("allocation of Focus data struct");
            return XIM_BadAlloc;
          }
        xi->w = xc->focus_window;
        xi->pclient = xc;
        xi->next = NULL;
        add_inputlist (xi);
      }
    save_cur_p = cur_p;
    save_cur_x = cur_x;
    save_c_c = c_c;
    cur_x = xc;
    if (IsPreeditNothing (xc))
      {
        cur_p = xc->root_pointer->ximclient;
      }
    else
      {
        cur_p = cur_x;
      }
    c_c = cur_p->cur_xl->w_c;
    cur_rk = c_c->rk;
    cur_rk_table = cur_rk->rk_table;
#ifdef  CALLBACKS
    if (IsPreeditPosition (xc) || IsPreeditArea (xc) || IsStatusCallbacks (xc))
      {
#else /* CALLBACKS */
    if (IsPreeditPosition (xc) || IsPreeditArea (xc))
      {
#endif /* CALLBACKS */
        reset_preedit (xc);
      }
#ifdef  CALLBACKS
    if (IsStatusArea (xc) || IsStatusCallbacks (xc))
      {
#else /* CALLBACKS */
    if (IsStatusArea (xc))
      {
#endif /* CALLBACKS */
        visual_status ();
      }
    cur_p = save_cur_p;
    cur_x = save_cur_x;
    c_c = save_c_c;
    if (c_c)
      {
        cur_rk = c_c->rk;
        cur_rk_table = cur_rk->rk_table;
      }
  }
  return 0;
}

/*
 *    Function Name : UnsetClientICFocus()
 *    Description   : unset IC focus to the specified client.
 *    Parameter     :
 *      xc          : pointer to client structure.
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
static int
UnsetClientICFocus (xc)
     XIMClientRec *xc;
{
  register XIMInputRec *xi;
  XIMClientRec *save_cur_p, *save_cur_x;
  WnnClientRec *save_c_c;

  /* modified from util.c: UnsetICFocus() */
  {
    for (xi = input_list; xi != NULL; xi = xi->next)
      {
        if (xi->w == xc->focus_window)
          break;
      }
    if (xi == NULL)
      {
        return XIM_BadFocusWindow;
      }
    else
      {
        remove_inputlist (xi);
        if (cur_input == xi)
          cur_input = 0;
        Free ((char *) xi);
        save_cur_p = cur_p;
        save_cur_x = cur_x;
        save_c_c = c_c;
        cur_x = xc;
        if (IsPreeditNothing (xc))
          {
            cur_p = xc->root_pointer->ximclient;
          }
        else
          {
            cur_p = cur_x;
          }
        c_c = cur_p->cur_xl->w_c;
        cur_rk = c_c->rk;
        cur_rk_table = cur_rk->rk_table;
#ifdef  CALLBACKS
        if (IsPreeditPosition (xc) || IsPreeditArea (xc) || IsPreeditCallbacks (xc))
          {
#else /* CALLBACKS */
        if (IsPreeditPosition (xc) || IsPreeditArea (xc))
          {
#endif /* CALLBACKS */
            invisual_window ();
          }
#ifdef  CALLBACKS
        if (IsStatusArea (xc) || IsStatusCallbacks (xc))
          {
#else /* CALLBACKS */
        if (IsStatusArea (xc))
          {
#endif /* CALLBACKS */
            invisual_status ();
          }
        cur_p = save_cur_p;
        cur_x = save_cur_x;
        c_c = save_c_c;
        if (c_c)
          {
            cur_rk = c_c->rk;
            cur_rk_table = cur_rk->rk_table;
          }
      }
  }
  return 0;
}

/*
 *    Function Name : RecvSetICFocus()
 *    Description   : set IC focus. 
 *    Parameter     :
 *      req         : pointer to a request structure.
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
static int
RecvSetICFocus (req)
     XimRequestPacket *req;
{
  CARD16 *buf_16 = (CARD16 *) req->data;
  XIMID im_id;
  XICID ic_id;

  XIMClientRec *xc;

  im_id = cur_im_id = (XIMID) GET_CARD16 (buf_16[0]);   /* 2 input-method-ID */
  if (!GetIMWithId (im_id))
    return XIM_BadProtocol;

  ic_id = cur_ic_id = (XICID) GET_CARD16 (buf_16[1]);   /* 2 input-context-ID */
  if (!(xc = GetICWithId (ic_id)))
    return XIM_BadProtocol;

  return SetClientICFocus (xc);

  /* asynchoronous request, no reply */
}

/*
 *    Function Name : RecvUnsetICFocus()
 *    Description   : unset IC focus.
 *    Parameter     :
 *      req         : pointer to a request structure.
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
static int
RecvUnsetICFocus (req)
     XimRequestPacket *req;
{
  CARD16 *buf_16 = (CARD16 *) req->data;
  XIMID im_id;
  XICID ic_id;

  XIMClientRec *xc;

  im_id = cur_im_id = (XIMID) GET_CARD16 (buf_16[0]);   /* 2 input-method-ID */
  if (!GetIMWithId (im_id))
    return XIM_BadProtocol;

  ic_id = cur_ic_id = (XICID) GET_CARD16 (buf_16[1]);   /* 2 input-context-ID */
  if (!(xc = GetICWithId (ic_id)))
    return XIM_BadProtocol;

  return UnsetClientICFocus (xc);

  /* asynchoronous request, no reply */
}

/*
 *    Function Name : RecvGetIMValues()
 *    Description   : encode IM values to byte data, then send it to client.
 *    Parameter     :
 *      req         : pointer to a request structure.
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
static int
RecvGetIMValues (req)
     XimRequestPacket *req;
{
  CARD16 *buf_16 = (CARD16 *) req->data;
  CARD8 *reply;
  XIMID im_id;
  CARD8 value[BUFSIZ];
  int len, value_len, n;
  int ret;
  XIMContextRec *im;

  im_id = cur_im_id = (XIMID) GET_CARD16 (buf_16[0]);   /* 2 input-method-ID */
  if (!(im = GetIMWithId (im_id)))
    return XIM_BadProtocol;

  n = GET_CARD16 (buf_16[1]);
  ret = EncodeIMAttributes ((CARD8 *) & buf_16[2], n, im, value, &value_len);
  if (ret)
    return ret;

  /* reply */
  len = sizeof (CARD16) +       /* 2 input-method-ID */
    sizeof (INT16) +            /* 2 byte length of im-attr */
    value_len;                  /* n list of im-attr */

  reply = MALLOC_REPLY (XIM_HEADER_SIZE + len);
  len += AddHead (reply, XIM_GET_IM_VALUES_REPLY, 0, len);
  buf_16 = (CARD16 *) (reply + XIM_HEADER_SIZE);
  buf_16[0] = GET_CARD16 (im_id);
  buf_16[1] = GET_CARD16 (value_len);
  (void) memcpy ((char *) &buf_16[2], (char *) value, value_len);
  if (_WriteToClientFlush (reply, len) < 0)
    {
      FREE (reply);
      return XIM_BadSomething;
    }
  FREE (reply);
  return 0;
}

/*
 *    Function Name : RecvSetIMValues()
 *    Description   : decode IM values, then send it to client context.
 *    Parameter     :
 *      req         : pointer to a request structure.
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
static int
RecvSetIMValues (req)
     XimRequestPacket *req;
{
  CARD16 *buf_16 = (CARD16 *) req->data;
  CARD8 *reply;
  XIMID im_id;
  int len, n;
  int ret;
  XIMContextRec *im;

  im_id = cur_im_id = (XIMID) GET_CARD16 (buf_16[0]);   /* 2 input-method-ID */
  if (!(im = GetIMWithId (im_id)))
    return XIM_BadProtocol;

  n = (int) GET_CARD16 (buf_16[1]);     /* 2 n byte length */
  ret = SetClientIMValues ((CARD8 *) & buf_16[2], n, im);
  if (ret)
    return ret;

  /* reply */
  len = sizeof (CARD16) +       /* 2 input-method-ID */
    sizeof (CARD16);            /* 2 unused */
  reply = MALLOC_REPLY (XIM_HEADER_SIZE + len);
  len += AddHead (reply, XIM_SET_IM_VALUES_REPLY, 0, len);
  buf_16 = (CARD16 *) (reply + XIM_HEADER_SIZE);
  buf_16[0] = GET_CARD16 (im_id);
  if (_WriteToClientFlush (reply, len) < 0)
    {
      FREE (reply);
      return XIM_BadSomething;
    }
  FREE (reply);
  return 0;
}

/*
 *    Function Name : SendCommit()
 *    Description   : commit a string to client, the string is converted
 *                    by input method, such kanji, and is encoded in CT.
 *    Parameter     :
 *      im_id       : client input-method-id.
 *      ic_id       : client input-context-id.
 *      flag        : flag of XimLookupKeySym, XimLookupChars or both.
 *      keysym      : keysym if committed.
 *      ct_str      : CT string if committed.
 *      ct_len      : length of string.
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
static int
SendCommit (im_id, ic_id, flag, keysym, ct_str, ct_len)
     XIMID im_id;
     XICID ic_id;
     int flag;
     KeySym keysym;
     char *ct_str;
     int ct_len;
{
  CARD16 *buf_16;
  CARD32 *buf_32;
  CARD8 *reply;
  int len;

  XIMClientRec *xc;

  if (!(xc = GetICWithId (ic_id)) || xc->im_id != im_id)
    return XIM_BadProtocol;

  flag |= XimSYNCHRONUS;        /* ask sync */

  /* send */
  len = sizeof (CARD16) +       /* 2 input-method-id */
    sizeof (CARD16) +           /* 2 input-context-id */
    sizeof (BITMASK16) +        /* 2 flag */
    LENGTH_KEYSYM (flag, sizeof (CARD32)) +     /* 2+6 KeySym */
    LENGTH_STRING_WITH_FLAG (flag, ct_len);     /* 2+n+Pad(2+n) string */

  reply = MALLOC_REPLY (XIM_HEADER_SIZE + len);
  len += AddHead (reply, XIM_COMMIT, 0, len);
  buf_16 = (CARD16 *) (reply + XIM_HEADER_SIZE);
  buf_16[0] = GET_CARD16 (im_id);
  buf_16[1] = GET_CARD16 (ic_id);
  buf_16[2] = GET_CARD16 (flag);
  buf_16 = (CARD16 *) & buf_16[3];
  if (flag & XimLookupKeySym)
    {
      buf_32 = (CARD32 *) & buf_16[1];
      buf_32[0] = GET_CARD32 (keysym);
      buf_16 = (CARD16 *) & buf_32[1];
    }
  if (flag & XimLookupChars)
    {
      buf_16[0] = GET_CARD16 (ct_len);
      (void) memcpy ((char *) &buf_16[1], ct_str, ct_len);
    }
  if (_WriteToClientFlush (reply, len) < 0)
    {
      FREE (reply);
      return XIM_BadSomething;
    }
  FREE (reply);

  if (ClientSyncDone (xc) == True)
    return SendSyncReply (im_id, ic_id);
  else
    return 0;
}

/*
 *    Function Name : SendForwardEvent()
 *    Description   : usually it is to send event back to client, e.g.
 *                    when no filter in server.
 *    Parameter     :
 *      im_id       : client input-method-id.
 *      ic_id       : client input-context-id.
 *      serial      : serial number of event.
 *      event       : pointer to XEvent.
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
static int
SendForwardEvent (im_id, ic_id, serial, event)
     XIMID im_id;
     XICID ic_id;
     int serial;
     XEvent *event;
{
  CARD16 *buf_16;
  CARD8 *reply;
  int len;
  int flag;

  /* send */
  len = sizeof (CARD16) +       /* 2 input-method-ID */
    sizeof (CARD16) +           /* 2 input-context-ID */
    sizeof (BITMASK16) +        /* 2 flag */
    sizeof (CARD16) +           /* 2 serial number */
    sizeof (xEvent);            /* n XEVENT */

  flag = 0;                     /* no sync flag */
  reply = MALLOC_REPLY (XIM_HEADER_SIZE + len);
  len += AddHead (reply, XIM_FORWARD_EVENT, 0, len);
  buf_16 = (CARD16 *) (reply + XIM_HEADER_SIZE);
  buf_16[0] = GET_CARD16 (im_id);
  buf_16[1] = GET_CARD16 (ic_id);
  buf_16[2] = GET_CARD16 (flag);
  buf_16[3] = GET_CARD16 (serial);
  if (_XimEventToWire (event, (xEvent *) & buf_16[4]) == False)
    return XIM_BadSomething;
  if (_WriteToClientFlush (reply, len) < 0)
    {
      FREE (reply);
      return XIM_BadSomething;
    }
  FREE (reply);
  return 0;
}

/*
 *    Function Name : ExecInputMethod()
 *    Description   : execute input method to do some henkan (conversion).
 *                    This is referred to keyin.c: RequestDispatch() on
 *                    the case of XIM_Event.
 *    Parameter     :
 *      im_id       : client input-method-id.
 *      ic_id       : client input-context-id.
 *      xevp        : pointer to event.
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
static int
ExecInputMethod (im_id, ic_id, xevp)
     XIMID im_id;
     XICID ic_id;
     XEvent *xevp;
{
  int buff[32], in;
  int ret;
  register int i, n_bytes;

  ret = key_input (buff, xevp);
  if (ret == 0)
    {
      /* send_nofilter() */
      return xim_send_nofilter ();
    }
  else if (ret > 0)
    {
      for (i = 0; i < ret;)
        {
          n_bytes = get_cswidth_by_char (in = buff[i++]);
          for (; n_bytes > 1 && i < ret; n_bytes--, i++)
            {
              in = (in << 8) + buff[i];
            }
          in_put (in);
        }
    }
  return 0;
}

/*
 *    Function Name : RecvForwardEvent()
 *    Description   : filter event forwarded by client to execute input
 *                    method.
 *    Parameter     :
 *      req         : pointer to a request structure.
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
static int
RecvForwardEvent (req)
     XimRequestPacket *req;
{
  CARD16 *buf_16 = (CARD16 *) req->data;
  XIMID im_id;
  XICID ic_id;
  int ret;
  int flag, serial;
  XEvent event;

  XIMClientRec *xc;

  im_id = cur_im_id = (XIMID) GET_CARD16 (buf_16[0]);   /* 2 input-method-ID */
  if (!GetIMWithId (im_id))
    return XIM_BadProtocol;

  ic_id = cur_ic_id = (XICID) GET_CARD16 (buf_16[1]);   /* 2 input-context-ID */
  if (!(xc = GetICWithId (ic_id)))
    return XIM_BadProtocol;

  flag = (int) GET_CARD16 (buf_16[2]);
  serial = (int) GET_CARD16 (buf_16[3]);

  SetClientSync (xc, True);

  if (_XimWireToEvent (dpy, serial, (xEvent *) & buf_16[4], &event) == False)
    return XIM_BadProtocol;
  /*
   * save event which will back to client if no filter.
   */
  (void) memcpy ((char *) &(xc->cur_event), (char *) &event, sizeof (XEvent));

  ret = ExecInputMethod (im_id, ic_id, &event);
  /*
   * if it is async (no XimSYNCHRONUS), force 0 returned, so no anything
   * even no error is sent to client.
   */
  if (ret)
    return ((flag & XimSYNCHRONUS) ? ret : 0);

  if (ClientSyncDone (xc) == True)
    return SendSyncReply (im_id, ic_id);
  else
    return 0;
}

/*
 *    Function Name : RecvStrConversionReply()
 *    Description   : send a request that is the format of:
 *    Parameter     :
 *      req         : pointer to a request structure.
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
static int
RecvStrConversionReply (req)
     XimRequestPacket *req;
{
  /* don't care in this model. */
  return 0;
}

#ifdef CALLBACKS

/*
 *    Function Name : CopyText()
 *    Description   : copy string and feedback array to byte data
 *    Parameter     :
 *      bur         : pointer to the byte data.
 *      str         : string
 *      feedback    : array of feedback.
 *      feedback_num: number of feedback.
 *    Returned Value:
 *      <none>
 */
static void
CopyText (buf, str, str_len, feedback, feedback_num)
     CARD8 *buf;
     char *str;
     int str_len;
     XIMFeedback *feedback;
     int feedback_num;
{
  CARD16 *buf_16;
  CARD32 *buf_32;
  CARD32 *save_32;
  BITMASK32 status = 0;
  int i;

  save_32 = (CARD32 *) buf;
  buf_16 = (CARD16 *) & save_32[1];
  if (str_len > 0)
    {
      buf_16[0] = GET_CARD16 (str_len);
      (void) strncpy ((char *) &buf_16[1], str, str_len);
      buf_16 = (CARD16 *) ((CARD8 *) buf_16 + sizeof (CARD16) + PAD (sizeof (CARD16) + str_len));
    }
  else
    {
      status |= XIMNoString;
    }
  if (feedback_num > 0)
    {
      buf_16[0] = GET_CARD16 (feedback_num);
      buf_32 = (CARD32 *) & buf_16[2];
      for (i = 0; i < feedback_num; i++)
        {
          buf_32[i] = GET_CARD32 (feedback[i]);
        }
    }
  else
    {
      status |= XIMNoFeedback;
    }
  save_32[0] = GET_CARD32 (status);
}

/*
 *    Function Name : SendPreeditDraw()
 *    Description   : send the request of XIM_PREEDT_DRAW.
 *    Parameter     :
 *      im_id       : client input-method-id.
 *      ic_id       : client input-context-id.
 *      caret       : caret
 *      chg_first   : first of change.
 *      chg_len     : length of change.
 *      str         : string.
 *      str_len     : length of string.
 *      feedback    : array of feedback.
 *      feedback_num: number of feedback.
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
static int
SendPreeditDraw (im_id, ic_id, caret, chg_first, chg_len, str, str_len, feedback, feedback_num)
     XIMID im_id;
     XICID ic_id;
     INT32 caret;
     INT32 chg_first;
     INT32 chg_len;
     char *str;
     int str_len;
     XIMFeedback *feedback;
     int feedback_num;
{
  CARD8 *reply;
  CARD16 *buf_16;
  CARD32 *buf_32;
  int len;

  /* send */
  len = sizeof (CARD16) +       /* 2 input-method-ID */
    sizeof (CARD16) +           /* 2 input-context-ID */
    sizeof (INT32) +            /* 4 caret */
    sizeof (INT32) +            /* 4 chg_first */
    sizeof (INT32) +            /* 4 chg_length */
    sizeof (BITMASK32) +        /* 4 status */
    LENGTH_STRING (str_len) +   /* x string */
    LENGTH_FEEDBACK (feedback_num);     /* x feedback array */

  reply = MALLOC_REPLY (XIM_HEADER_SIZE + len);
  len += AddHead (reply, XIM_PREEDIT_DRAW, 0, len);
  buf_16 = (CARD16 *) (reply + XIM_HEADER_SIZE);
  buf_16[0] = GET_CARD16 (im_id);
  buf_16[1] = GET_CARD16 (ic_id);
  buf_32 = (CARD32 *) & buf_16[2];
  buf_32[0] = GET_CARD32 (caret);
  buf_32[1] = GET_CARD32 (chg_first);
  buf_32[2] = GET_CARD32 (chg_len);
  buf_16 = (CARD16 *) & buf_32[3];
  CopyText ((CARD8 *) & buf_32[3], str, str_len, feedback, feedback_num);
  if (_WriteToClientFlush (reply, len) < 0)
    {
      FREE (reply);
      return XIM_BadSomething;
    }
  FREE (reply);
  return 0;
}

/*
 *    Function Name : SendPreeditCaret()
 *    Description   : send the request of XIM_PREEDIT_CARET.
 *    Parameter     :
 *      im_id       : client input-method-id.
 *      ic_id       : client input-context-id.
 *      pos         : position
 *      direct      : direction of caret.
 *      style       : style of caret.
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
static int
SendPreeditCaret (im_id, ic_id, pos, direct, style)
     XIMID im_id;
     XICID ic_id;
     INT32 pos;
     CARD32 direct;
     CARD32 style;
{
  CARD8 *reply;
  CARD16 *buf_16;
  CARD32 *buf_32;
  int len;

  /* send */
  len = sizeof (CARD16) +       /* 2 input-method-ID */
    sizeof (CARD16) +           /* 2 input-context-ID */
    sizeof (INT32) +            /* 4 position */
    sizeof (CARD32) +           /* 4 direction */
    sizeof (CARD32);            /* 4 style */

  reply = MALLOC_REPLY (XIM_HEADER_SIZE + len);
  len += AddHead (reply, XIM_PREEDIT_CARET, 0, len);
  buf_16 = (CARD16 *) (reply + XIM_HEADER_SIZE);
  buf_16[0] = GET_CARD16 (im_id);
  buf_16[1] = GET_CARD16 (ic_id);
  buf_32 = (CARD32 *) & buf_16[2];
  buf_32[0] = GET_CARD32 (pos);
  buf_32[1] = GET_CARD32 (direct);
  buf_32[2] = GET_CARD32 (style);
  if (_WriteToClientFlush (reply, len) < 0)
    {
      FREE (reply);
      return XIM_BadSomething;
    }
  FREE (reply);
  return 0;
}

/*
 *    Function Name : SendStatusDraw()
 *    Description   : send the request of XIM_STATUS_DRAW.
 *    Parameter     :
 *      im_id       : client input-method-id.
 *      ic_id       : client input-context-id.
 *      st_type     : type of status.
 *      str         : string.
 *      str_len     : length of string.
 *      feedback    : array of feedback.
 *      feedback_num: number of feedback.
 *      pixmap      : pixmap data. 
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
static int
SendStatusDraw (im_id, ic_id, st_type, str, str_len, feedback, feedback_num, pixmap)
     XIMID im_id;
     XICID ic_id;
     CARD32 st_type;
     char *str;
     int str_len;
     XIMFeedback *feedback;
     int feedback_num;
     PIXMAP pixmap;
{
  CARD8 *reply;
  CARD16 *buf_16;
  CARD32 *buf_32;
  int len;

  /* send */
  len = sizeof (CARD16) +       /* 2 input-method-ID */
    sizeof (CARD16) +           /* 2 input-context-ID */
    sizeof (CARD32) +           /* 4 type */
    sizeof (BITMASK32) +        /* 4 status */
    LENGTH_TEXT (st_type, str_len, feedback_num) +      /* text */
    LENGTH_PIXMAP (st_type);    /* x pixmap */

  reply = MALLOC_REPLY (XIM_HEADER_SIZE + len);
  len += AddHead (reply, XIM_STATUS_DRAW, 0, len);
  buf_16 = (CARD16 *) (reply + XIM_HEADER_SIZE);
  buf_16[0] = GET_CARD16 (im_id);
  buf_16[1] = GET_CARD16 (ic_id);
  buf_32 = (CARD32 *) & buf_16[2];
  buf_32[0] = GET_CARD32 (st_type);
  if (st_type == XIMTextType)
    {
      CopyText ((CARD8 *) & buf_32[1], str, str_len, feedback, feedback_num);
    }
  else
    {
      buf_32[1] = GET_CARD32 (pixmap);
    }
  if (_WriteToClientFlush (reply, len) < 0)
    {
      FREE (reply);
      return XIM_BadSomething;
    }
  FREE (reply);
  return 0;
}

/*
 *    Function Name : RecvPreeditStartReply()
 *    Description   : receive a reply from client.
 *    Parameter     :
 *      req         : pointer to a request structure.
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
static int
RecvPreeditStartReply (req)
     XimRequestPacket *req;
{
  /* don't care. */
  return 0;
}

/*
 *    Function Name : RecvPreeditCaretReply()
 *    Description   : receive a reply from client.
 *    Parameter     :
 *      req         : pointer to a request structure.
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
static int
RecvPreeditCaretReply (req)
     XimRequestPacket *req;
{
  /* don't care. */
  return 0;
}

#endif /* CALLBACKS */

#if defined(EXT_MOVE) || defined(SPOT)

/*
 *    Function Name : RecvExtMove()
 *    Description   : change only the point of spot.
 *    Parameter     :
 *      req         : pointer to a request structure.
 *    Returned Value:
 *      <none>
 */
static int
RecvExtMove (req)
     XimRequestPacket *req;
{
  CARD16 *buf_16 = (CARD16 *) req->data;
  XIMID im_id;
  XICID ic_id;
  short x, y;
  XIMClientRec *xc;

  im_id = cur_im_id = (XIMID) GET_CARD16 (buf_16[0]);   /* 2 input-method-ID */
  if (!GetIMWithId (im_id))
    return XIM_BadProtocol;

  ic_id = cur_ic_id = (XICID) GET_CARD16 (buf_16[1]);   /* 2 input-context-ID */
  if (!(xc = GetICWithId (ic_id)))
    return XIM_BadProtocol;

  x = (short) GET_CARD16 (buf_16[2]);
  y = (short) GET_CARD16 (buf_16[3]);
  if (change_spotlocation (xc, x, y) != 0)
    return XIM_BadSpotLocation;
  else
    return 0;

  /* asynchoronous request, no reply */
}

#endif /* defined(EXT_MOVE) || defined(SPOT) */

/*
 *    Function Name : AppendServerToProperty()
 *    Description   : append server id to IM_SERVERS property.
 *                    Don't use PropModeAppend that will propagate a same
 *                    server id more and more in the property.
 *    Parameter     :
 *      disp        : pointer to X display.
 *      win         : window that holds the property.
 *      imserver    : our server id to be appended.
 *    Returned Value:
 *      True        : success.
 *      False       : error.
 */
static Bool
AppendServerToProperty (disp, win, imserver)
     Display *disp;
     Window win;
     Atom imserver;
{
  Atom server_atom, actual_type;
  Atom *atom_list, *new_list;
  int actual_format;
  unsigned long nitems, bytes_after;
  unsigned char *prop_return;
  int i;

  server_atom = XInternAtom (disp, XIM_SERVERS, False);
  (void) XGetWindowProperty (disp, win, server_atom, 0L, 1000000L, False, XA_ATOM, &actual_type, &actual_format, &nitems, &bytes_after, &prop_return);

  if (nitems > 0 && (actual_type != XA_ATOM || actual_format != 32))
    {
      return False;
    }
  if (nitems > 0)
    {
      /*
       * search whether our server id has been registered or not.
       */
      atom_list = (Atom *) prop_return;
      for (i = 0; i < nitems; i++)
        {
          if (atom_list[i] == imserver)
            return True;
        }
      new_list = (Atom *) Malloc ((nitems + 1) * sizeof (Atom));
      if (!new_list)
        {
          malloc_error ("no memory for atom list.");
          return False;
        }
      (void) memcpy ((char *) new_list, (char *) prop_return, nitems * sizeof (Atom));
      new_list[nitems] = imserver;
    }
  else
    {
      new_list = &imserver;
    }

  /*
   * append our server id to IM_SERVERS property.
   */
  XChangeProperty (disp, win, server_atom, XA_ATOM, 32, PropModeReplace, (unsigned char *) new_list, (nitems + 1));
  if (nitems > 0)
    {
      XFree ((char *) prop_return);
      XFree ((char *) new_list);
    }
  return True;
}

/*--------------------------------------------------------------------*
 *                                                                    *
 *                         Public Functions                           *
 *                                                                    *
 *--------------------------------------------------------------------*/

/*
 *    Function Name : XimError()
 *    Description   : take the breakpoint of error when it occurs at any time.
 *    Parameter     :
 *      fd          : socket id of IM client.
 *    Returned Value:
 *      <none>
 */
#if NeedFunctionPrototypes
void
XimError (int fd)
#else
void
XimError (fd)
     int fd;
#endif /* NeedFunctionPrototypes */
{
  register XIMClientRec *xc;

  for (xc = ximclient_list; xc != NULL; xc = xc->next)
    {
      if (xc->fd == fd)
        {
          (void) RemoveIMWithId ((XIMID) xc->im_id);
        }
    }
}

/*
 *    Function Name : XimRequestDispatch()
 *    Description   : dispatch all requests of X11 IM protocol.
 *    Parameter     :
 *      <none>
 *    Returned Value:
 *      <none>
 */
#if NeedFunctionPrototypes
void
XimRequestDispatch (void)
#else
void
XimRequestDispatch ()
#endif                          /* NeedFunctionPrototypes */
{
  XimRequestPacket req_data;
  XimRequestPacket *req = &req_data;
  int ret;

  ret = cur_status = 0;
  if (RecvRequest (req) < 0)
    return;

  /* respond each request of protocol */
  switch (req->major_opcode)
    {
    case XIM_AUTH_NEXT:
      ret = SendAuthRequired ((CARD8) 0, (CARD8 *) NULL, (INT16) 0);
      break;
    case XIM_AUTH_REPLY:
      ret = SendConnectReply (PROTOCOLMAJORVERSION, PROTOCOLMINORVERSION);
      break;
    case XIM_CONNECT:
      ret = RecvConnect (req);
      break;
    case XIM_DISCONNECT:
      cur_status = XIM_IMID_VALID | XIM_ICID_VALID;
      ret = RecvDisconnect (req);
      break;
    case XIM_OPEN:
      cur_status = XIM_IMID_VALID;
      ret = RecvOpen (req);
      break;
    case XIM_CLOSE:
      cur_status = XIM_IMID_VALID | XIM_ICID_VALID;
      ret = RecvClose (req);
      break;
    case XIM_ENCODING_NEGOTIATION:
      cur_status = XIM_IMID_VALID;
      ret = RecvEncodingNegotiation (req);
      break;
    case XIM_QUERY_EXTENSION:
      cur_status = XIM_IMID_VALID;
      ret = RecvQueryExtension (req);
      break;
    case XIM_SET_IM_VALUES:
      cur_status = XIM_IMID_VALID;
      ret = RecvSetIMValues (req);
      break;
    case XIM_GET_IM_VALUES:
      cur_status = XIM_IMID_VALID;
      ret = RecvGetIMValues (req);
      break;
    case XIM_CREATE_IC:
      cur_status = XIM_IMID_VALID | XIM_ICID_VALID;
      ret = RecvCreateIC (req);
      break;
    case XIM_DESTROY_IC:
      cur_status = XIM_IMID_VALID | XIM_ICID_VALID;
      ret = RecvDestroyIC (req);
      break;
    case XIM_SET_IC_VALUES:
      cur_status = XIM_IMID_VALID | XIM_ICID_VALID;
      ret = RecvSetICValues (req);
      break;
    case XIM_GET_IC_VALUES:
      cur_status = XIM_IMID_VALID | XIM_ICID_VALID;
      ret = RecvGetICValues (req);
      break;
    case XIM_SET_IC_FOCUS:
      cur_status = XIM_IMID_VALID | XIM_ICID_VALID;
      (void) RecvSetICFocus (req);      /* asyncronous request */
      break;
    case XIM_UNSET_IC_FOCUS:
      cur_status = XIM_IMID_VALID | XIM_ICID_VALID;
      (void) RecvUnsetICFocus (req);    /* asyncronous request */
      break;
    case XIM_TRIGGER_NOTIFY:
      cur_status = XIM_IMID_VALID | XIM_ICID_VALID;
#ifdef DYNAMIC_EVENT_FLOW_MODEL
      ret = RecvTriggerNotify (req);
#else
      ret = 0;
#endif /* DYNAMIC_EVENT_FLOW_MODEL */
      break;
    case XIM_FORWARD_EVENT:
      cur_status = XIM_IMID_VALID | XIM_ICID_VALID;
      ret = RecvForwardEvent (req);
      break;
    case XIM_RESET_IC:
      cur_status = XIM_IMID_VALID | XIM_ICID_VALID;
      ret = RecvResetIC (req);
      break;
    case XIM_SYNC:
      cur_status = XIM_IMID_VALID | XIM_ICID_VALID;
      ret = RecvSync (req);
      break;
    case XIM_SYNC_REPLY:
      cur_status = XIM_IMID_VALID | XIM_ICID_VALID;
      (void) RecvSyncReply (req);       /* don't need send anything */
      break;
#ifdef CALLBACKS
    case XIM_PREEDIT_START_REPLY:
      cur_status = XIM_IMID_VALID | XIM_ICID_VALID;
      ret = RecvPreeditStartReply (req);
      break;
    case XIM_PREEDIT_CARET_REPLY:
      cur_status = XIM_IMID_VALID | XIM_ICID_VALID;
      ret = RecvPreeditCaretReply (req);
      break;
#endif /* CALLBACKS */
    case XIM_STR_CONVERSION_REPLY:
      cur_status = XIM_IMID_VALID | XIM_ICID_VALID;
      ret = RecvStrConversionReply (req);
      break;
#if defined(EXT_MOVE) || defined(SPOT)
    case XIM_EXT_MOVE:
      cur_status = XIM_IMID_VALID | XIM_ICID_VALID;
      (void) RecvExtMove (req); /* asyncronous request */
      break;
#endif /* defined(EXT_MOVE) || defined(SPOT) */
    case XIM_ERROR:
      cur_status = XIM_IMID_VALID | XIM_ICID_VALID;
      (void) RecvError (req);   /* asyncronous request */
      break;
    default:
      ret = XIM_BadProtocol;
    }
  if (ret)
    SendError (ret);
  FREE (req->data);
}

/*
 *    Function Name : XimPreConnect()
 *    Description   : make a base for preconnection with IM client. This
 *                    includes to create all selection atoms, names and
 *                    properties, plus some initial steps.
 *    Parameter     :
 *      port        : TCP/IP port number used in this IM protocol.
 *      root        : default root client.
 *    Returned Value:
 *      = 0         : successed.
 *      = -1        : error, failed to make preconnection.
 */
#if NeedFunctionPrototypes
int
XimPreConnect (unsigned short port,     /* port number 38036/tcp of xwnmo */
               XIMRootRec * root)
#else
int
XimPreConnect (port, root)
     unsigned short port;       /* port number 38036/tcp of xwnmo */
     XIMRootRec *root;
#endif /* NeedFunctionPrototypes */
{
  char atom_name[BUFFER_MAX];
  char hostname[BUFFER_MAX];
  int len;

  /*
   * make server atom:  @server=xwnmo
   */
  (void) sprintf (atom_name, "%s%s", XIM_SERVER_CATEGORY, XIM_SERVER_NAME);
  if ((server_id = XInternAtom (dpy, atom_name, True)) != (Atom) None)
    {
      if (XGetSelectionOwner (dpy, server_id) != (Window) None)
        {
          print_out ("Another server has been already running.");
          return -1;
        }
    }
  else
    {
      server_id = XInternAtom (dpy, atom_name, False);
    }

  if (AppendServerToProperty (dpy, root->root_window, server_id) == False)
    return -1;

  /*
   * make target XIM_LOCALES:  "@locale=ja,ja_JP,..."
   */
  (void) strcpy (atom_name, XIM_LOCAL_CATEGORY);
  locale_supported = (unsigned char *) GetLocaleSupported ();
  len = strlen (atom_name);
  if (!(locale_prop = (unsigned char *) Malloc (len + strlen (locale_supported) + 1)))
    {
      malloc_error ("allocation of locale property");
      return -1;
    }
  locale_target = XInternAtom (dpy, XIM_LOCALES, False);
  (void) strcpy (locale_prop, atom_name);
  (void) strcat (locale_prop, locale_supported);
  locale_supported = locale_prop + len;

  /*
   * make target XIM_TRANSPORT:  "tcp/hostname:port"
   */
  (void) XmuGetHostname (hostname, (sizeof hostname));
  (void) sprintf (atom_name, "%stcp/%s:%u", XIM_TRANSPORT_CATEGORY, hostname, port);
  len = strlen (atom_name);
  if (!(transport_prop = (unsigned char *) Malloc (len + 1)))
    {
      malloc_error ("allocation of transport property");
      return -1;
    }
  transport_target = XInternAtom (dpy, XIM_TRANSPORT, False);
  (void) strcpy (transport_prop, atom_name);

  XSetSelectionOwner (dpy, server_id, root->ximclient->w, 0L);

  InitiateProtocol ();

  return 0;
}

/*
 *    Function Name : XimConvertTarget()
 *    Description   : convert a selection target and send its property
 *                    to client.  This is in preconnection phase.
 *    Parameter     :
 *      dpy         : pointer to X display.
 *      event       : selection event.
 *    Returned Value:
 *      <none>
 */
#if NeedFunctionPrototypes
void
XimConvertTarget (Display * dpy, XSelectionRequestEvent * event)
#else
void
XimConvertTarget (dpy, event)
     Display *dpy;
     XSelectionRequestEvent *event;
#endif /* NeedFunctionPrototypes */
{
  XSelectionEvent new;
  unsigned char *string = (unsigned char *) NULL;

  bzero (&new, sizeof (XSelectionEvent));
  new.type = SelectionNotify;
  new.serial = event->serial;
  new.display = event->display;
  new.requestor = event->requestor;
  new.selection = event->selection;
  new.target = event->target;
  new.time = event->time;
  new.property = event->property;

  if (event->selection == server_id)
    {
      if (event->target == locale_target)
        string = locale_prop;
      else if (event->target == transport_target)
        string = transport_prop;
      if (string)
        {
          XChangeProperty (dpy, event->requestor, event->property, event->target, 8, PropModeReplace, string, strlen ((char *) string));
          XFlush (dpy);
        }
    }
  XSendEvent (dpy, event->requestor, False, NoEventMask, (XEvent *) & new);
}

/*
 *    Function Name : xim_send_ct()
 *    Description   : similar to Xsi IM send_ct().
 *    Parameter     :
 *      ct          : CT string
 *      ct_len      : length of string in byte.
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
#if NeedFunctionPrototypes
int
xim_send_ct (register char *ct, register int ct_len)
#else
int
xim_send_ct (ct, ct_len)
     register char *ct;
     register int ct_len;
#endif /* NeedFunctionPrototypes */
{
  return SendCommit (CUR_IM_ID (), CUR_IC_ID (), XimLookupChars, (KeySym) 0, ct, ct_len);
}

/*
 *    Function Name : xim_send_keysym()
 *    Description   : similar to Xsi IM send_keysym().
 *    Parameter     :
 *      keysym      : keysym code
 *      str         : keysym character.
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
#if NeedFunctionPrototypes
int
xim_send_keysym (KeySym keysym, char str)
#else
int
xim_send_keysym (keysym, str)
     KeySym keysym;
     char str;
#endif /* NeedFunctionPrototypes */
{
  if (keysym < 0x20)
    {
      keysym |= 0xff00;
      return SendCommit (CUR_IM_ID (), CUR_IC_ID (), XimLookupKeySym, keysym, (char *) NULL, 0);
    }
  else
    {
      str &= 0xff;
      return SendCommit (CUR_IM_ID (), CUR_IC_ID (), XimLookupBoth, keysym, &str, 1);
    }
}

/*
 *    Function Name : xim_send_nofilter()
 *    Description   : similar to Xsi IM send_nofilter().
 *    Parameter     :
 *      <none>
 *    Returned Value:
 *      = 0         : successed.
 *      > 0         : error, the number is error code.
 */
#if NeedFunctionPrototypes
int
xim_send_nofilter (void)
#else
int
xim_send_nofilter ()
#endif                          /* NeedFunctionPrototypes */
{
  XEvent *xev = CUR_CLIENT_EVENT ();
  int serial = XIM_SERIALNUMBER (((XAnyEvent *) xev)->serial);

  return SendForwardEvent (CUR_IM_ID (), CUR_IC_ID (), serial, xev);
}

#ifdef CALLBACKS

/*
 *    Function Name : xim_send_preeditstart()
 *    Description   : similar to Xsi IM send_preeditstart().
 *    Parameter     :
 *      <none>
 *    Returned Value:
 *      <none>
 */
#if NeedFunctionPrototypes
void
xim_send_preeditstart (void)
#else
void
xim_send_preeditstart ()
#endif                          /* NeedFunctionPrototypes */
{
  (void) SendIdOnly (CUR_IM_ID (), CUR_IC_ID (), XIM_PREEDIT_START, 0);
}

/*
 *    Function Name : xim_send_preeditdraw()
 *    Description   : similar to Xsi IM send_preeditdraw().
 *    Parameter     :
 *      caret       : caret
 *      first       : first of change.
 *      chg_len     : length of change.
 *      str         : wchar string.
 *      len         : length of string in wchar.
 *      flg         : flag
 *    Returned Value:
 *      <none>
 */
#if NeedFunctionPrototypes
void
xim_send_preeditdraw (int caret, int first, int chg_len, wchar * str, int len, int flg)
#else
void
xim_send_preeditdraw (caret, first, chg_len, str, len, flg)
     int caret;
     int first;
     int chg_len;
     wchar *str;
     int len;
     int flg;
#endif /* NeedFunctionPrototypes */
{
  unsigned short fb = 0;
  int send_len;
  XIMFeedback xim_fb;

  if (str)
    {
      while (1)
        {
          send_len = wchar_to_ct (cur_p->cur_xl->xlc, str, ct_buf, len, ct_buf_max);
          if (send_len < -1)
            {
              return;
            }
          else if (send_len == -1)
            {
              if (realloc_ct_buf () < 0)
                return;
            }
          else
            {
              break;
            }
        }
    }
  else
    {
      send_len = 0;
    }
  if (flg & REV_FLAG)
    fb |= XIMReverse;
  if (flg & UNDER_FLAG)
    fb |= XIMUnderline;
  if (flg & BOLD_FLAG)
    fb |= XIMHighlight;

  xim_fb = (XIMFeedback) fb;
  (void) SendPreeditDraw (CUR_IM_ID (), CUR_IC_ID (), caret, first, chg_len, ct_buf, send_len, (XIMFeedback *) & xim_fb, 1);
}

/*
 *    Function Name : xim_send_preeditcaret()
 *    Description   : similar to Xsi IM send_preeditcaret().
 *    Parameter     :
 *      pos         : position
 *      flg         : flag
 *    Returned Value:
 *      <none>
 */
#if NeedFunctionPrototypes
void
xim_send_preeditcaret (int flg, int pos)
#else
void
xim_send_preeditcaret (flg, pos)
     int flg;
     int pos;
#endif /* NeedFunctionPrototypes */
{
  int style;

  if (flg)
    {
      style = XIMIsPrimary;
    }
  else
    {
      style = XIMIsInvisible;
    }
  (void) SendPreeditCaret (CUR_IM_ID (), CUR_IC_ID (), pos, XIMAbsolutePosition, style);
}

/*
 *    Function Name : xim_send_preeditdone()
 *    Description   : similar to Xsi IM send_preeditdone().
 *    Parameter     :
 *      <none>
 *    Returned Value:
 *      <none>
 */
#if NeedFunctionPrototypes
void
xim_send_preeditdone (void)
#else
void
xim_send_preeditdone ()
#endif                          /* NeedFunctionPrototypes */
{
  (void) SendIdOnly (CUR_IM_ID (), CUR_IC_ID (), XIM_PREEDIT_DONE, 0);
}

/*
 *    Function Name : xim_send_statusstart()
 *    Description   : similar to Xsi IM send_statusstart().
 *    Parameter     :
 *      <none>
 *    Returned Value:
 *      <none>
 */
#if NeedFunctionPrototypes
void
xim_send_statusstart (void)
#else
void
xim_send_statusstart ()
#endif                          /* NeedFunctionPrototypes */
{
  (void) SendIdOnly (CUR_IM_ID (), CUR_IC_ID (), XIM_STATUS_START, 0);
}

/*
 *    Function Name : xim_send_statusdraw()
 *    Description   : similar to Xsi IM send_statusdraw().
 *    Parameter     :
 *      str         : wchar string.
 *      len         : length of string in wchar.
 *    Returned Value:
 *      <none>
 */
#if NeedFunctionPrototypes
void
xim_send_statusdraw (wchar * str, int len)
#else
void
xim_send_statusdraw (str, len)
     wchar *str;
     int len;
#endif /* NeedFunctionPrototypes */
{
  int send_len;

  /* refer to callback.c: send_statusdraw() */
  while (1)
    {
      send_len = wchar_to_ct (cur_p->cur_xl->xlc, str, ct_buf, len, ct_buf_max);
      if (send_len < -1)
        {
          return;
        }
      else if (send_len == -1)
        {
          if (realloc_ct_buf () < 0)
            return;
        }
      else
        {
          break;
        }
    }
  (void) SendStatusDraw (CUR_IM_ID (), CUR_IC_ID (), XIMTextType, ct_buf, send_len, (XIMFeedback *) NULL, 0, (PIXMAP) 0);
}

/*
 *    Function Name : xim_send_statusdone()
 *    Description   : similar to Xsi IM send_statusdone().
 *    Parameter     :
 *      <none>
 *    Returned Value:
 *      <none>
 */
#if NeedFunctionPrototypes
void
xim_send_statusdone (void)
#else
void
xim_send_statusdone ()
#endif                          /* NeedFunctionPrototypes */
{
  (void) SendIdOnly (CUR_IM_ID (), CUR_IC_ID (), XIM_STATUS_DONE, 0);
}

#endif /* CALLBACKS */

#endif /* !X11R5 */
