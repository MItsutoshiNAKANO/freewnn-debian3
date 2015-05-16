/*
 * $Id: do_xjplib.c,v 1.2 2001/06/14 18:16:15 ura Exp $
 */

/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
 * Copyright 1991, 1992 by Massachusetts Institute of Technology
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

#ifdef  XJPLIB

#include "do_xjplib.h"

#define XJPLIB_PORT_IN  (0x9494)
#define UNIX_PATH       "/tmp/xwnmo.V2"
#define UNIX_ACPT       0
#define INET_ACPT       1

#define XIMCLIENT       (xim->root_pointer[xim->default_screen]->ximclient)

static Atom xjp_select_id = (Atom) 0;

XJpClientRec *xjp_clients = NULL;
XJpClientRec *xjp_cur_client = NULL;
XJpInputRec *xjp_inputs = NULL;
XJpInputRec *xjp_cur_input = NULL;

char *def_escape[4] = {
  "(J",
  "$B",
  "(I",
  "$0"
};

#define vname_size      128
#define hname_size      128
#define XWNMO_XJP_NAME  "xwnmo.V2"
void
XJp_init ()
{
  char name[hname_size];
  unsigned char buffer[hname_size * 2 + 10];
  register unsigned char *p;
  long pnumber = 0L;
  extern int gethostname ();

  if (!(xjp_select_id = XInternAtom (dpy, "XJPFRONTEND", True)))
    {
      xjp_select_id = XInternAtom (dpy, "XJPFRONTEND", False);
    }
#ifdef  XJPLIB_DIRECT
  pnumber = (long) XJp_get_xjp_port ();
#endif /* XJPLIB_DIRECT */
  gethostname (name, hname_size);
  p = buffer;
  strcpy (p, XWNMO_XJP_NAME);
  p += vname_size;
  strcpy (p, name);
  bcopy (&pnumber, (p += hname_size), sizeof (long));
  *(p += sizeof (long)) = '\0';
  XChangeProperty (dpy, xim->root_pointer[xim->default_screen]->root_window, xjp_select_id, XA_STRING, 8, PropModeReplace, buffer, p - buffer);
  XSetSelectionOwner (dpy, xjp_select_id, XIMCLIENT->w, 0L);
}

static void
xjp_send_open (xjp, w)
     XJpClientRec *xjp;
     Window w;
{
  XEvent event;

  event.type = ClientMessage;
  event.xclient.format = 32;
  event.xclient.window = w;
  event.xclient.data.l[0] = XJP_OPEN;
  event.xclient.data.l[1] = w;
  if (xjp->dispmode == XJP_ROOT)
    {
      event.xclient.data.l[2] = FontWidth (XIMCLIENT->xl[0]);
      event.xclient.data.l[3] = FontHeight (XIMCLIENT->xl[0]);
    }
  else
    {
      event.xclient.data.l[2] = FontWidth (xjp->xim_client->xl[0]);
      event.xclient.data.l[3] = FontHeight (xjp->xim_client->xl[0]);
    }
  XSendEvent (dpy, xjp->w, False, NoEventMask, &event);
  XFlush (dpy);
}

static void
xjp_send_err (w, err)
     Window w;
     int err;
{
  XEvent event;

  event.type = ClientMessage;
  event.xclient.format = 32;
  event.xclient.window = XIMCLIENT->w;
  event.xclient.data.l[0] = XJP_ERROR;
  event.xclient.data.l[1] = err;
  XSendEvent (dpy, w, False, NoEventMask, &event);
  XFlush (dpy);
}

void
XJp_end ()
{
  XEvent event;
  XJpClientRec *p;

  if (dpy)
    {
      event.type = ClientMessage;
      event.xclient.format = 32;
      event.xclient.message_type = xjp_select_id;
      event.xclient.data.l[0] = XJP_ERROR;
      event.xclient.data.l[1] = 0;
      for (p = xjp_clients; p; p = p->next)
        {
#ifdef  XJPLIB_DIRECT
          if (p->direct_fd == -1)
            {
#endif /* XJPLIB_DIRECT */
              XSendEvent (dpy, p->w, False, NoEventMask, &event);
              XFlush (dpy);
#ifdef  XJPLIB_DIRECT
            }
#endif /* XJPLIB_DIRECT */
        }
    }
  XDeleteProperty (dpy, XIMCLIENT->w, xjp_select_id);
}

static char *
xjp_get_xim_fontname (xjp, p, cs)
     XJpClientRec *xjp;
     register char *p;
     XCharStruct *cs;
{
  register char *s, *ss, *xim_font;
  char save_escape[10];
  int char_set;
  register int i;
  int get_height = 0;
  XFontStruct *font;

  if ((xim_font = Malloc (strlen (p))) == NULL)
    {
      malloc_error ("allocation of work area");
      return (NULL);
    }

  s = xim_font;
  for (; *p; s++)
    {
      for (i = 0; *p && *p != 0x09; i++, p++)
        save_escape[i] = *p;
      save_escape[i] = '\0';
      p++;
      ss = s;
      for (; *p && *p != 0x0a; s++, p++)
        {
          *s = *p;
        }
      p++;
      if (!get_height)
        {
          *s = '\0';
          if (font = XLoadQueryFont (dpy, ss))
            {
              cs->ascent = font->ascent;
              cs->descent = font->descent;
              cs->width = font->max_bounds.width;
              get_height = 1;
              XFreeFont (dpy, font);
            }
        }
      *s = ',';
      if (save_escape[0] == '(')
        {
          if (save_escape[1] == 'I')
            {
              char_set = 2;
            }
          else
            {
              char_set = 0;
            }
        }
      else if (save_escape[0] == '$')
        {
          if (save_escape[1] == 'B')
            {
              char_set = 1;
            }
          else
            {
              char_set = 3;
            }
        }
      else
        {
          char_set = 0;
        }
      strcpy (xjp->escape[char_set], save_escape);
    }
  *s = '\0';
  return (xim_font);
}

void
XJp_xjp_to_xim (xjp, ic_req, pre_req, st_req, cs)
     XJpClientRec *xjp;
     ximICValuesReq *ic_req;
     ximICAttributesReq *pre_req, *st_req;
     XCharStruct *cs;
{
  XWindowAttributes attr;
  int height = cs->ascent + cs->descent;
  int min_keycode, max_keycode;

  XDisplayKeycodes (dpy, &min_keycode, &max_keycode);
  ic_req->c_window = ic_req->focus_window = xjp->w;
  ic_req->max_keycode = max_keycode;
  ic_req->mask = (1 << ICClientWindow);
  ic_req->mask |= (1 << ICFocusWindow);

  switch (xjp->dispmode)
    {
    case XJP_UNDER:
      ic_req->input_style = (XIMPreeditArea | XIMStatusArea);
      ic_req->mask |= (1 << (ICInputStyle));
      ic_req->mask |= (1 << (ICArea));
      ic_req->mask |= (1 << (ICForeground));
      ic_req->mask |= (1 << (ICBackground));
      ic_req->mask |= (1 << (ICFontSet));
      ic_req->mask |= (1 << (ICArea + StatusOffset));
      ic_req->mask |= (1 << (ICForeground + StatusOffset));
      ic_req->mask |= (1 << (ICBackground + StatusOffset));
      ic_req->mask |= (1 << (ICFontSet + StatusOffset));
      pre_req->area_x = xjp->x + cs->width * MHL0;
      pre_req->area_y = xjp->y + (xjp->height - height) / 2;
      pre_req->area_width = xjp->width - cs->width * MHL0;
      pre_req->area_height = xjp->height;
      pre_req->foreground = xjp->fg;
      pre_req->background = xjp->bg;
      st_req->area_x = xjp->x;
      st_req->area_y = xjp->y + (xjp->height - height) / 2;
      st_req->area_width = cs->width * MHL0;
      st_req->area_height = xjp->height;
      st_req->foreground = xjp->fg;
      st_req->background = xjp->bg;
      break;
    case XJP_XY:
      ic_req->input_style = (XIMPreeditPosition | XIMStatusArea);
      ic_req->mask |= (1 << (ICInputStyle));
      ic_req->mask |= (1 << (ICArea));
      ic_req->mask |= (1 << (ICForeground));
      ic_req->mask |= (1 << (ICBackground));
      ic_req->mask |= (1 << (ICSpotLocation));
      ic_req->mask |= (1 << (ICFontSet));
      ic_req->mask |= (1 << (ICArea + StatusOffset));
      ic_req->mask |= (1 << (ICForeground + StatusOffset));
      ic_req->mask |= (1 << (ICBackground + StatusOffset));
      ic_req->mask |= (1 << (ICFontSet + StatusOffset));
      XGetWindowAttributes (dpy, xjp->w, &attr);
      pre_req->area_x = attr.x;
      pre_req->area_y = attr.y;
      pre_req->area_width = attr.width;
      pre_req->area_height = attr.height;
      pre_req->spot_x = xjp->x;
      pre_req->spot_y = xjp->y + cs->ascent;
      pre_req->foreground = xjp->fg;
      pre_req->background = xjp->bg;
      st_req->area_x = xjp->status_x;
      st_req->area_y = xjp->status_y;
      st_req->area_width = cs->width * MHL0;
      st_req->area_height = xjp->height;
      st_req->foreground = xjp->fg;
      st_req->background = xjp->bg;
      break;
    case XJP_ROOT:
      ic_req->input_style = (XIMPreeditNothing | XIMStatusNothing);
      ic_req->mask |= (1 << (ICInputStyle));
      break;
    }
}

static void
xjp_open (ev)
     XClientMessageEvent *ev;
{
  XJpClientRec *xjp, *p;
  XIMClientRec *xc = NULL;
  Atom client_atom = (Atom) ev->data.l[2];
  Window client_window = (Window) ev->data.l[1];
  Atom actual_type;
  int actual_format;
  unsigned long nitems, leftover;
  unsigned char *data;
  ximICValuesReq ic_req;
  ximICAttributesReq pre_req, st_req;
  XIMNestLangList lc_list = NULL;
  char *font = NULL;
  XCharStruct cs;
  short detail;
  int err_code = 0, i;

  if ((xjp = (XJpClientRec *) Malloc (sizeof (XJpClientRec))) == NULL)
    {
      malloc_error ("allocation of client data(XJp)");
      return;
    }
  XGetWindowProperty (dpy, client_window, client_atom, 0L, 1000000L, False, XA_STRING, &actual_type, &actual_format, &nitems, &leftover, &data);
  if (nitems < 184)
    {
      print_out ("XJp_open data was bad format.");
      return;
    }
  bcopy ((char *) data, (char *) xjp, 184);
  data += 184;
  for (p = xjp_clients; p; p = p->next)
    {
      if (p->w == xjp->w)
        {
          p->ref_count++;
          if (xjp->dispmode == XJP_ROOT)
            {
              xjp_send_open (xjp, XIMCLIENT->xl[0]->wp[0]);
            }
          else
            {
              xjp_send_open (xjp, p->xim_client->xl[0]->wp[0]);
            }
          Free ((char *) xjp);
          return;
        }
    }
  for (i = 0; i < 4; i++)
    {
      xjp->escape[i][0] = '\0';
    }
  if (xjp->dispmode != XJP_ROOT)
    {
      font = xjp_get_xim_fontname (xjp, data, &cs);
      if ((xjp->mask & XJP_FG) == 0)
        xjp->fg = XIMCLIENT->pe.fg;
      if ((xjp->mask & XJP_BG) == 0)
        xjp->bg = XIMCLIENT->pe.bg;
      if (xjp->dispmode == XJP_UNDER)
        {
          if ((xjp->mask & XJP_BP) == 0)
            xjp->bp = 1;
          if ((xjp->mask & XJP_WIDTH) == 0)
            xjp->width = xjp->p_width;
          if ((xjp->mask & XJP_HEIGHT) == 0)
            xjp->height = cs.ascent + cs.descent;
          if ((xjp->mask & XJP_X) == 0)
            xjp->x = 0;
          if ((xjp->mask & XJP_Y) == 0)
            xjp->y = xjp->p_height - (xjp->height + xjp->bp);
        }
      else if (xjp->dispmode == XJP_XY)
        {
          if ((xjp->mask & XJP_BP) == 0)
            xjp->bp = 1;
          if ((xjp->mask & XJP_HEIGHT) == 0)
            xjp->height = cs.ascent + cs.descent;
        }
    }
  for (i = 0; i < 4; i++)
    {
      if (xjp->escape[i][0] == '\0')
        strcpy (xjp->escape[i], def_escape[i]);
    }
  XJp_xjp_to_xim (xjp, &ic_req, &pre_req, &st_req, &cs);
  if (get_langlist (def_locale, &lc_list) >= 0 && lc_list)
    {
      xc = create_client (&ic_req, &pre_req, &st_req, lc_list, lc_list, 0, 0, font, font, xjp->c_data, &detail);
    }
  if (font)
    Free (font);
  if (lc_list)
    free_langlist (lc_list);
  if (xc == NULL)
    {
      switch (detail)
        {
        case BadStyle:
          err_code = XJP_F_OPEN_NOT_SUPPORT;
          break;
        case BadFontSet:
          err_code = XJP_F_OPEN_BAD_FN;
          break;
        case BadClientWindow:
        case BadFocusWindow:
          err_code = XJP_F_OPEN_BAD_WID;
          break;
        case BadSpotLocation:
          err_code = XJP_F_OPEN_BAD_SZ;
          break;
        case BadSomething:
        default:
          err_code = XJP_F_OPEN_NOT_WIN;
          break;
        }
      xjp_send_err (xjp->w, err_code);
      Free ((char *) xjp);
      return;
    }
  else
    {
      xc->xjp = 1;
      xjp->xim_client = xc;
#ifdef  XJPLIB_DIRECT
      xjp->direct_fd = -1;
#endif /* XJPLIB_DIRECT */
      xjp->next = xjp_clients;
      xjp->ref_count = 0;
      xjp_clients = xjp;
      XSelectInput (dpy, xjp->w, StructureNotifyMask);
      if (xjp->dispmode == XJP_ROOT)
        {
          xjp_send_open (xjp, XIMCLIENT->xl[0]->wp[0]);
        }
      else
        {
          xjp_send_open (xjp, xc->xl[0]->wp[0]);
        }
    }
}

static void
xjp_close (ev)
     XClientMessageEvent *ev;
{
  Window client_window = ev->window;
  XJpClientRec *p, **prev_p;
  XJpInputRec *i, **prev_i;

  for (prev_p = &xjp_clients; p = *prev_p; prev_p = &p->next)
    {
      if (client_window == p->w)
        {
          if (p->ref_count)
            {
              p->ref_count--;
              return;
            }
          if (p == xjp_cur_client)
            {
              xjp_cur_client = NULL;
              xjp_cur_input = NULL;
            }
          destroy_client (p->xim_client);
          for (prev_i = &xjp_inputs; i = *prev_i;)
            {
              if (i->pclient == p)
                {
                  XSelectInput (dpy, i->w, NoEventMask);
                  *prev_i = i->next;
                  Free ((char *) i);
                }
              else
                {
                  prev_i = &i->next;
                }
            }
          XSelectInput (dpy, p->w, NoEventMask);
          *prev_p = p->next;
          Free ((char *) p);
          return;
        }
    }
}

static void
xjp_begin (ev)
     XClientMessageEvent *ev;
{
  register XJpClientRec *xjp = NULL, *p;
  register XJpInputRec *i;
  Window grab_window = ev->data.l[2];
  Window client_window = ev->window;

  for (p = xjp_clients; p; p = p->next)
    {
      if (p->w == client_window)
        {
          xjp = p;
          break;
        }
    }
  if (p == NULL)
    return;

  for (p = xjp_clients; p; p = p->next)
    {
      if (p->w == grab_window)
        {
          XSelectInput (dpy, grab_window, (KeyPressMask | StructureNotifyMask));
          break;
        }
    }
  if (p == NULL)
    {
      XSelectInput (dpy, grab_window, KeyPressMask);
    }
  XFlush (dpy);
  for (i = xjp_inputs; i; i = i->next)
    {
      if (i->w == grab_window)
        {
          if (i->pclient != xjp)
            {
              i->pclient = xjp;
              cur_input = NULL;
            }
          return;
        }
    }
  if ((i = (XJpInputRec *) Malloc (sizeof (XJpInputRec))) == NULL)
    {
      if (p)
        {
          XSelectInput (dpy, grab_window, StructureNotifyMask);
        }
      else
        {
          XSelectInput (dpy, grab_window, NoEventMask);
        }
      return;
    }
  i->w = grab_window;
  i->pclient = xjp;
  i->save_event = 0;
  i->next = xjp_inputs;
  xjp_inputs = i;
}

static void
xjp_end (ev)
     XClientMessageEvent *ev;
{
  register XJpClientRec *p;
  Window grab_window = ev->data.l[2];

  for (p = xjp_clients; p; p = p->next)
    {
      if (p->w == grab_window)
        {
          XSelectInput (dpy, grab_window, StructureNotifyMask);
          break;
        }
    }
  if (p == NULL)
    XSelectInput (dpy, grab_window, NoEventMask);
  cur_input = NULL;
  XFlush (dpy);
}

static void
xjp_change (ev)
     XClientMessageEvent *ev;
{
  register XJpClientRec *xjp = NULL, *p;
  register XIMClientRec *xc = NULL;
  Window pre_window = (Window) ev->data.l[1];
  XJpClientRec savexjp;
  Atom client_atom;
  Atom actual_type;
  int actual_format;
  unsigned long nitems, leftover;
  unsigned char *data;
  ximICValuesReq ic_req;
  ximICAttributesReq pre_req, st_req;
  short detail;
  unsigned long mask, xim_mask = 0;

  for (p = xjp_clients; p; p = p->next)
    {
      if (IsPreeditNothing (p->xim_client))
        continue;
      if (p->xim_client->xl[0]->wp[0] == pre_window)
        {
          xjp = p;
          xc = p->xim_client;
          break;
        }
    }
  if (xjp == NULL)
    return;
  if (!(client_atom = XInternAtom (dpy, "XJPCLIENT", True)))
    return;
  XGetWindowProperty (dpy, p->w, client_atom, 0L, 1000000L, False, XA_STRING, &actual_type, &actual_format, &nitems, &leftover, &data);
  if (nitems < 184)
    return;

  bcopy ((char *) data, (char *) &savexjp, 184);
  mask = savexjp.mask;

  if (mask & XJP_FG)
    {
      if (xjp->fg != savexjp.fg)
        {
          xjp->fg = savexjp.fg;
          xim_mask |= ((1 << ICForeground) | (1 << (ICForeground + StatusOffset)));
        }
      xjp->mask |= XJP_FG;
    }
  if (mask & XJP_BG)
    {
      if (xjp->bg != savexjp.bg)
        {
          xjp->bg = savexjp.bg;
          xim_mask |= ((1 << ICBackground) | (1 << (ICBackground + StatusOffset)));
        }
      xjp->mask |= XJP_BG;
    }
  if ((mask & XJP_WIDTH) /* && xjp->dispmode == XJP_XY */ )
    {
      if (xjp->width != savexjp.width)
        {
          xjp->width = savexjp.width;
          xim_mask |= (1 << ICArea);
        }
      xjp->mask |= XJP_WIDTH;
    }
  if (mask & XJP_HEIGHT)
    {
      if (xjp->height != savexjp.height)
        {
          xjp->height = savexjp.height;
          xim_mask |= (1 << ICArea);
        }
      xjp->mask |= XJP_HEIGHT;
    }
  if ((mask & XJP_X) && xjp->dispmode == XJP_XY)
    {
      if (xjp->x != savexjp.x)
        {
          xjp->x = savexjp.x;
          xim_mask |= (1 << ICSpotLocation);
        }
      xjp->mask |= XJP_X;
    }
  if ((mask & XJP_Y) && xjp->dispmode == XJP_XY)
    {
      if (xjp->y != savexjp.y)
        {
          xjp->y = savexjp.y;
          xim_mask |= (1 << ICSpotLocation);
        }
      xjp->mask |= XJP_Y;
    }
  if (mask & XJP_PWIDTH)
    {
      if (xjp->p_width != savexjp.p_width)
        {
          xjp->p_width = savexjp.p_width;
          if (xjp->dispmode == XJP_UNDER && !(xjp->mask & XJP_WIDTH))
            {
              xjp->width = xjp->p_width;
              xim_mask |= (1 << ICArea);
            }
        }
      xjp->mask |= XJP_PWIDTH;
    }
  if (mask & XJP_PHEIGHT)
    {
      if (xjp->p_height != savexjp.p_height)
        {
          xjp->p_height = savexjp.p_height;
          if (xjp->dispmode == XJP_UNDER /* && !(xjp->mask & XJP_HEIGHT) */ )
            {
              xjp->y = xjp->p_height - xjp->height;
              xim_mask |= ((1 << ICArea) | (1 << (ICArea + StatusOffset)));
            }
        }
      xjp->mask |= XJP_HEIGHT;
    }
  if (mask & XJP_STATUS)
    {
      if (xjp->dispmode == XJP_XY)
        {
          xjp->status_x = savexjp.status_x;
          xjp->status_y = savexjp.status_y;
          xim_mask |= (1 << (ICArea + StatusOffset));
        }
      xjp->mask |= XJP_STATUS;
    }

  XJp_xjp_to_xim (xjp, &ic_req, &pre_req, &st_req, xc->xl[0]->pe_b_char);
  ic_req.mask = xim_mask;
  change_client (xc, &ic_req, &pre_req, &st_req, NULL, NULL, NULL, &detail);
}

static void
xjp_move (ev)
     XClientMessageEvent *ev;
{
  register XJpClientRec *p;
  Window pre_window = (Window) ev->data.l[1];
  int x = (int) ev->data.l[2];
  int y = (int) ev->data.l[3];

  for (p = xjp_clients; p; p = p->next)
    {
      if (IsPreeditNothing (p->xim_client))
        continue;
      if (p->xim_client->xl[0]->wp[0] == pre_window)
        {
          p->x = x;
          p->y = y + FontAscent (p->xim_client->cur_xl);
          change_spotlocation (p->xim_client, p->x, p->y);
          return;
        }
    }
}

static void
xjp_visible (ev, visible)
     XClientMessageEvent *ev;
     int visible;
{
  Window visible_window = ev->window;
  register XJpClientRec *p;
  register XIMClientRec *xc;
  XIMClientRec *save_cur_p, *save_cur_x;
  WnnClientRec *save_c_c;

  for (p = xjp_clients; p; p = p->next)
    {
      if (p->w == visible_window)
        {
          break;
        }
    }
  if (p == NULL)
    return;
  xc = p->xim_client;
  save_cur_p = cur_p;
  save_cur_x = cur_x;
  save_c_c = c_c;
  cur_x = xc;
  if (IsPreeditNothing (xc))
    {
      cur_p = XIMCLIENT;
    }
  else
    {
      cur_p = cur_x;
    }
  c_c = cur_p->cur_xl->w_c;
  cur_rk = c_c->rk;
  cur_rk_table = cur_rk->rk_table;
  if (IsPreeditPosition (xc) || IsPreeditArea (xc))
    {
      if (visible)
        {
          reset_preedit (xc);
        }
      else
        {
          invisual_window ();
        }
    }
  if (IsStatusArea (xc))
    {
      if (visible)
        {
          visual_status ();
        }
      else
        {
          invisual_status ();
        }
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

static void
xjp_indicator (ev)
     XClientMessageEvent *ev;
{
  register XJpClientRec *p;
  Window pre_window = (Window) ev->data.l[1];
  Window window = (Window) ev->data.l[2];
  int x = (int) ev->data.l[3];
  int y = (int) ev->data.l[4];

  for (p = xjp_clients; p; p = p->next)
    {
      if (p->dispmode == XJP_ROOT)
        continue;
      if (p->xim_client->xl[0]->wp[0] == pre_window)
        {
          p->x = x;
          p->y = y;
          XReparentWindow (dpy, p->xim_client->xl[0]->ws, window, x, y);
          return;
        }
    }
}

void
XJp_event_dispatch (ev)
     XClientMessageEvent *ev;
{
  switch (ev->data.l[0])
    {
    case XJP_OPEN:
      xjp_open (ev);
      break;
    case XJP_CLOSE:
      xjp_close (ev);
      break;
    case XJP_BEGIN:
      xjp_begin (ev);
      break;
    case XJP_END:
      xjp_end (ev);
      break;
    case XJP_CHANGE:
      xjp_change (ev);
      break;
    case XJP_MOVE:
      xjp_move (ev);
      break;
    case XJP_VISIBLE:
      xjp_visible (ev, 1);
      break;
    case XJP_INVISIBLE:
      xjp_visible (ev, 0);
      break;
    case XJP_INDICATOR:
      xjp_indicator (ev);
      break;
    }

}

XIMClientRec *
XJp_cur_cl_set (set_window)
     Window set_window;
{
  register XJpInputRec *i;

  if (set_window == (Window) 0)
    {
      xjp_cur_input = NULL;
      xjp_cur_client = NULL;
      return (NULL);
    }
  for (i = xjp_inputs; i; i = i->next)
    {
      if (i->w == set_window)
        {
          xjp_cur_input = i;
          xjp_cur_client = i->pclient;
          cur_input = NULL;
          return (i->pclient->xim_client);
        }
    }
  return (NULL);
}

int
XJp_check_cur_input (w)
     Window w;
{
  if (xjp_cur_input && w == xjp_cur_input->w)
    return (0);
  return (1);
}

static void
xjp_send_cl_key (ev)
     XKeyEvent *ev;
{
#ifdef  XJPLIB_DIRECT
  if (xjp_cur_client && xjp_cur_client->direct_fd != -1)
    {
      XJp_direct_send_cl_key ();
      return;
    }
#endif /* XJPLIB_DIRECT */
  if (!xjp_cur_input)
    return;
  ev->window = xjp_cur_input->w;
  XSendEvent (dpy, xjp_cur_input->w, False, NoEventMask, (XEvent *) ev);
  XFlush (dpy);
}

void
XJp_check_send_cl_key (ev)
     XKeyEvent *ev;
{
  if (xjp_cur_input == NULL || xjp_cur_client == NULL)
    return;
  if (ifempty () || henkan_off_flag)
    xjp_send_cl_key (ev);
}

static void
xjp_save_event (ev)
     XKeyEvent *ev;
{
  if (!xjp_cur_input)
    return;
  ev->window = xjp_cur_input->w;
  bcopy ((char *) ev, (char *) &(xjp_cur_input->ev), sizeof (XKeyEvent));
  xjp_cur_input->save_event = 1;
}

void
XJp_check_save_event (ev, must)
     XKeyEvent *ev;
     int must;
{
  if (xjp_cur_input == NULL || xjp_cur_client == NULL)
    return;
  if (must || ifempty () || henkan_off_flag)
    xjp_save_event (ev);
}

void
XJp_return_cl_it ()
{
#ifdef  XJPLIB_DIRECT
  if (xjp_cur_client && xjp_cur_client->direct_fd != -1)
    {
      XJp_direct_send_cl_key ();
      return;
    }
#endif /* XJPLIB_DIRECT */
  if (!xjp_cur_input || !xjp_cur_input->save_event)
    return;
  xjp_send_cl_key (&(xjp_cur_input->ev));
  xjp_cur_input->save_event = 0;
}

static void
xjp_send_buf (ptr, len, cs)
     register unsigned char *ptr;
     int len;
     int cs;
{
  XEvent ev;
  register int i;

  ev.type = ClientMessage;
  ev.xclient.format = 8;
  ev.xclient.message_type = xjp_select_id;
  ev.xclient.window = xjp_cur_input->w;
  bcopy (xjp_cur_client->escape[cs], ev.xclient.data.b, 2);
  while (len > 0)
    {
      for (i = 4; i < 4 + len && i < 20; i++, ptr++)
        {
          ev.xclient.data.b[i] = *ptr;
        }
      ev.xclient.data.b[3] = (char) (i - 4);
      XSendEvent (dpy, xjp_cur_input->w, False, NoEventMask, &ev);
      len -= 16;
    }
}

void
XJp_xw_write (w_buf, size)
     register w_char *w_buf;
     register int size;
{
  unsigned char *ptr, *send_ptr, *save_ptr, buf[512], send_buf[512];
  int len, send_len = 0;
  int cs = 0;

  if (henkan_off_flag)
    {
      XJp_return_cl_it ();
      return;
    }

  if ((len = w_char_to_char (w_buf, buf, size)) <= 0)
    return;
  buf[len] = '\0';

#ifdef  XJPLIB_DIRECT
  if (xjp_cur_client->direct_fd != -1)
    {
      XJp_return_sock ((short) 0, (short) len, 0, (KeySym) 0, buf);
      return;
    }
#endif /* XJPLIB_DIRECT */

  for (ptr = buf, send_ptr = save_ptr = send_buf; *ptr;)
    {
      if (*ptr >= 0xa1 && *ptr <= 0xfe)
        {
          if (cs != 1 && send_len > 0)
            {
              xjp_send_buf (save_ptr, send_len, cs);
              save_ptr = send_ptr;
              send_len = 0;
            }
          *send_ptr++ = *ptr++ & 0x7f;
          *send_ptr++ = *ptr++ & 0x7f;
          cs = 1;
          send_len += 2;
        }
      else if (*ptr == SS2)
        {
          if (cs != 2 && send_len > 0)
            {
              xjp_send_buf (save_ptr, send_len, cs);
              save_ptr = send_ptr;
              send_len = 0;
            }
          ptr++;
          *send_ptr++ = *ptr++ &= 0x7f;
          cs = 2;
          send_len += 1;
        }
      else if (*ptr == SS3)
        {
          if (cs != 3 && send_len > 0)
            {
              xjp_send_buf (save_ptr, send_len, cs);
              save_ptr = send_ptr;
              send_len = 0;
            }
          ptr++;
          *send_ptr++ = *ptr++ &= 0x7f;
          *send_ptr++ = *ptr++ &= 0x7f;
          cs = 2;
          send_len += 2;
        }
      else
        {
          if (*ptr == 0xd || *ptr == 0xa)
            {
              if (send_len > 0)
                {
                  xjp_send_buf (save_ptr, send_len, cs);
                  save_ptr = send_ptr;
                  send_len = 0;
                }
              XJp_return_cl_it ();
              ptr++;
            }
          else
            {
              if (cs != 0 && send_len > 0)
                {
                  xjp_send_buf (save_ptr, send_len, cs);
                  save_ptr = send_ptr;
                  send_len = 0;
                }
              *send_ptr++ = *ptr++;
              cs = 0;
              send_len += 1;
            }
        }
    }
  xjp_send_buf (save_ptr, send_len, cs);
}

int
XJp_xw_destroy (ev)
     register XDestroyWindowEvent *ev;
{
  Window client_window = ev->window;
  XJpClientRec *p, **prev_p;
  XJpInputRec *i, **prev_i;

  for (prev_p = &xjp_clients; p = *prev_p; prev_p = &p->next)
    {
      if (client_window == p->w)
        {
          if (p == xjp_cur_client)
            {
              xjp_cur_client = NULL;
              xjp_cur_input = NULL;
            }
          destroy_client (p->xim_client);
          for (prev_i = &xjp_inputs; i = *prev_i;)
            {
              if (i->pclient == p)
                {
                  XSelectInput (dpy, i->w, NoEventMask);
                  *prev_i = i->next;
                  Free ((char *) i);
                }
              else
                {
                  prev_i = &i->next;
                }
            }
          XSelectInput (dpy, p->w, NoEventMask);
          *prev_p = p->next;
          Free ((char *) p);
          return (1);
        }
    }
  return (0);
}

int
XJp_check_map (xc)
     register XIMClientRec *xc;
{
  register XJpInputRec *i;

  for (i = xjp_inputs; i != NULL; i = i->next)
    {
      if (i->pclient && i->pclient->xim_client == xc)
        return (1);
    }
  return (0);
}

int
XJp_get_client_cnt ()
{
  register XJpClientRec *p;
  register int i;

  for (i = 0, p = xjp_clients; p; p = p->next)
    i++;
  return (i);
}
#endif
