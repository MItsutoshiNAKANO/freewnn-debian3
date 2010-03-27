/*
 * $Id: init_w.c,v 1.2 2001/06/14 18:16:16 ura Exp $
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
/*      Version 4.0
 */
#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "sdefine.h"
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Xmu/SysUtil.h>
#include "xim.h"
#ifdef X11R5
#include "Xlocaleint.h"
#endif /* X11R5 */
#include "proto.h"
#include "sheader.h"
#include "ext.h"

Display *dpy = 0;
Atom xim_id;
XPoint button;

Atom wm_id = 0;                 /* Window Manager */
Atom wm_id1 = 0;                /* Window Manager */

Cursor cursor1, cursor2, cursor3;

XSetWindowAttributes attributes;
XGCValues xgcv;

#define HOSTNAME_SIZE   128
char hostname[HOSTNAME_SIZE];

wchar_t dol_wchar_t;
wchar_t cursor_wchar_t;

struct _sub_resource
{
  Pixel fore_color_res;
  Pixel back_color_res;
  Pixel border_color_res;
  char *geometry_res;
  char *icon_geometry_res;
  int border_width_res;
  Boolean iconic_start_res;
  char *font_set_res;
  Boolean root_visible_res;
}
sub_resource;

struct _resource
{
  char *servername_res;
  char *username_res;
  char *ximrc_file_res;
  char *cvt_key_file_res;
  char *cvt_fun_file_res;
  char *cvt_meta_file_res;
  char *display_name_res;
#ifdef  USING_XJUTIL
  char *xjutil_res;
#endif                          /* USING_XJUTIL */
  char *lang_name_res;
  Boolean exit_menu_res;
  Boolean iconic_start_res;
  char *geometry_res;
  int border_width_res;
  Boolean save_under_res;
}
resource;

#define DEFAULT_WIDTH   1280
#define DEFAULT_BORDERWIDTH     4
#define DEFAULT_X       0
#define DEFAULT_Y       0

static int defaultBorderWidth = DEFAULT_BORDERWIDTH;
static Boolean defaultTRUE = TRUE;
static Boolean defaultFALSE = FALSE;

#define offset(field)   XtOffset(struct _sub_resource *, field)
static XtResource sub_resources[] = {
  {XtNforeground, XtCForeground, XtRPixel, sizeof (Pixel),
   offset (fore_color_res), XtRString, "XtDefaultForeground"}
  ,
  {XtNbackground, XtCBackground, XtRPixel, sizeof (Pixel),
   offset (back_color_res), XtRString, "XtDefaultBackground"}
  ,
  {XtNborderColor, XtCBorderColor, XtRPixel, sizeof (Pixel),
   offset (border_color_res), XtRString, "XtDefaultForeground"}
  ,
  {XtNiconic, XtCIconic, XtRBoolean, sizeof (Boolean),
   offset (iconic_start_res), XtRBoolean, (caddr_t) & defaultFALSE}
  ,
  {XtNgeometry, XtCGeometry, XtRString, sizeof (char *),
   offset (geometry_res), XtRString, (caddr_t) NULL},
  {"iconGeometry", "IconGeometry", XtRString, sizeof (char *),
   offset (icon_geometry_res), XtRString, (caddr_t) NULL},
  {XtNborderWidth, XtCBorderWidth, XtRInt, sizeof (int),
   offset (border_width_res), XtRInt, (caddr_t) & defaultBorderWidth},
  {"fontSet", "FontSet", XtRString, sizeof (char *),
   offset (font_set_res), XtRString, NULL},
  {"rootVisible", "RootVisible", XtRBoolean, sizeof (Boolean),
   offset (root_visible_res), XtRBoolean, (caddr_t) & defaultFALSE}
  ,
};
#undef offset

#define offset(field)   XtOffset(struct _resource *, field)
static XtResource app_resources[] = {
  {"serverName", "ServerName", XtRString, sizeof (char *),
   offset (servername_res), XtRString, NULL},
  {"userName", "UserName", XtRString, sizeof (char *),
   offset (username_res), XtRString, NULL},
  {"ximrcName", "XimrcName", XtRString, sizeof (char *),
   offset (ximrc_file_res), XtRString, NULL},
  {"cvtximName", "CvtximName", XtRString, sizeof (char *),
   offset (cvt_key_file_res), XtRString, NULL},
  {"cvtfunName", "CvtfunName", XtRString, sizeof (char *),
   offset (cvt_fun_file_res), XtRString, NULL},
  {"cvtmetaName", "CvtmetaName", XtRString, sizeof (char *),
   offset (cvt_meta_file_res), XtRString, NULL},
  {"display", "Display", XtRString, sizeof (char *),
   offset (display_name_res), XtRString, NULL},
#ifdef  USING_XJUTIL
  {"xjutilName", "XjutilName", XtRString, sizeof (char *),
   offset (xjutil_res), XtRString, NULL},
#endif /* USING_XJUTIL */
  {"langName", "LangName", XtRString, sizeof (char *),
   offset (lang_name_res), XtRString, (caddr_t) NULL},
  {"exitMenu", "ExitMenu", XtRBoolean, sizeof (Boolean),
   offset (exit_menu_res), XtRBoolean, (caddr_t) & defaultTRUE}
  ,
  {XtNiconic, XtCIconic, XtRBoolean, sizeof (Boolean),
   offset (iconic_start_res), XtRBoolean, (caddr_t) & defaultFALSE}
  ,
  {XtNgeometry, XtCGeometry, XtRString, sizeof (char *),
   offset (geometry_res), XtRString, (caddr_t) NULL},
  {XtNborderWidth, XtCBorderWidth, XtRInt, sizeof (int),
   offset (border_width_res), XtRInt, (caddr_t) NULL},
  {"saveUnder", "SaveUnder", XtRBoolean, sizeof (Boolean),
   offset (save_under_res), XtRBoolean, (caddr_t) & defaultFALSE}
  ,
};
#undef  offset

/*
static char *fallback_resources[] = {
};
*/

static XrmOptionDescRec optionDescList[] = {
  {"-D", "*serverName", XrmoptionSepArg, (caddr_t) NULL},
  {"-n", "*userName", XrmoptionSepArg, (caddr_t) NULL},
  {"-X", "*cvtximName", XrmoptionSepArg, (caddr_t) NULL},
  {"-F", "*cvtfunName", XrmoptionSepArg, (caddr_t) NULL},
  {"-M", "*cvtmetaName", XrmoptionSepArg, (caddr_t) NULL},
#ifdef  USING_XJUTIL
  {"-ju", "*xjutilName", XrmoptionSepArg, (caddr_t) NULL},
#endif /* USING_XJUTIL */
  {"-lc", "*langName", XrmoptionSepArg, (caddr_t) NULL},
  {"-fs", "*fontSet", XrmoptionSepArg, (caddr_t) NULL},
  {"#", "*iconGeometry", XrmoptionStickyArg, (caddr_t) NULL},
  {"-RV", "*rootVisible", XrmoptionNoArg, (caddr_t) "on"},
  {"-EM", "*exitMenu", XrmoptionNoArg, (caddr_t) "on"},
  {"+EM", "*exitMenu", XrmoptionNoArg, (caddr_t) "off"},
  {"-SU", "*saveUnder", XrmoptionNoArg, (caddr_t) "on"},
};

Widget toplevel;

int
get_application_resources (argc, argv)
     int argc;
     char **argv;
{
  XtAppContext app;

  toplevel = XtAppInitialize (&app, "Xwnmo", optionDescList, XtNumber (optionDescList), &argc, (String *) argv, (String *) NULL, (ArgList) NULL, (Cardinal) 0);
  XtGetApplicationResources (toplevel, (XtPointer) & resource, app_resources, XtNumber (app_resources), NULL, 0);

  root_def_servername = root_def_reverse_servername = resource.servername_res;
  root_username = resource.username_res;
  ximrc_file = resource.ximrc_file_res;
  cvt_key_file = resource.cvt_key_file_res;
  cvt_fun_file = resource.cvt_fun_file_res;
  cvt_meta_file = resource.cvt_meta_file_res;
  display_name = resource.display_name_res;
#ifdef  USING_XJUTIL
  xjutil_name = resource.xjutil_res;
#endif /* USING_XJUTIL */
  def_lang = resource.lang_name_res;
  return (argc);
}

static int
xerror_handler (d, ev)
     Display *d;
     register XErrorEvent *ev;
{
#ifdef  USING_XJUTIL
  register XIMLangDataBase *ld;

  for (ld = language_db; ld; ld = ld->next)
    {
      if (ev->resourceid == ld->xjutil_id)
        {
          if (ld->xjutil_use)
            ld->xjutil_use->cur_xl->working_xjutil = 0;
          break;
        }
    }
#endif /* USING_XJUTIL */
  return (0);
}

static int
xioerror_handler (d)
     Display *d;
{
  perror ("xwnmo");
  epilogue ();
  XCloseDisplay (dpy);
  exit (2);
  return (0);
}

static void
write_ximid (port, root)
     short port;
     XIMRootRec *root;
{
  unsigned char buffer[HOSTNAME_SIZE + sizeof (long) * 4];
  register unsigned char *p;
  unsigned short pnumber;
  long major_version;
  long minor_version;

  if (!(xim_id = XInternAtom (dpy, XIM_INPUTMETHOD, True)))
    {
      xim_id = XInternAtom (dpy, XIM_INPUTMETHOD, False);
    }
  major_version = htonl (XIM_MAJOR_VERSION);
  minor_version = htonl (XIM_MINOR_VERSION);
  pnumber = (unsigned short) htons (port);
  XmuGetHostname (hostname, sizeof hostname);
  p = buffer;
  bcopy (hostname, p, HOSTNAME_SIZE);
  bcopy (&pnumber, (p += HOSTNAME_SIZE), sizeof (unsigned short));
  bcopy (&major_version, (p += sizeof (unsigned short)), sizeof (long));
  bcopy (&minor_version, (p += sizeof (long)), sizeof (long));
  *(p += sizeof (long)) = '\0';

  XChangeProperty (dpy, root->root_window, xim_id, XA_STRING, 8, PropModeReplace, buffer, (p - buffer));

  XSetSelectionOwner (dpy, xim_id, root->ximclient->w, 0L);
}

static void
clear_xl (x)
     register XIMLangRec *x;
{
  x->linefeed[0] = x->linefeed[1] = x->linefeed[2] = 0;
  x->vst = 0;
  x->max_l1 = x->max_l2 = x->max_l3 = 0;
  x->note[0] = x->note[1] = 0;
  x->currentcol0 = x->currentcol = 0;
  x->max_pos0 = x->max_pos = 0;
  x->max_cur = 0;
  x->del_x = 0;
  x->cursor_flag = 0;
  x->mark_flag = 0;
  x->u_line_flag = x->r_flag = x->b_flag = x->visible = 0;
  x->visible_line = 0;
  x->m_cur_flag = 0;
}

static int
create_ichi (root)
     XIMRootRec *root;
{
  register Ichiran *ichi = root->ichi;
  register XIMLangRec *xl = root->ximclient->xl[0];
  register unsigned int width, height;
  unsigned long fg, bg;
  int i;
  char name[32], class[32];

  ichi->save_buf = NULL;
  ichi->max_columns = MAX_ICHI_COLUMNS;
  if (alloc_for_save_buf (ichi, MAX_ICHI_LINES) < 0)
    return (-1);
  ichi->max_button = MAX_BUTTON;
  ichi->select_mode = '\0';

  width = root->ximclient->client_area.width;
  height = root->ximclient->client_area.height;
  fg = root->ximclient->pe.fg;
  bg = root->ximclient->pe.bg;

  ichi->w = XCreateSimpleWindow (dpy, root->root_window, 0, 0, width, height, root->bw, fg, bg);
  ichi->w0 = XCreateSimpleWindow (dpy, ichi->w, 0, FontWidth (xl) * 4, width, height, 1, fg, bg);
  ichi->w1 = XCreateSimpleWindow (dpy, ichi->w0, 0, FontWidth (xl) * 4, width, height, 0, fg, bg);
  ichi->nyuu_w = XCreateSimpleWindow (dpy, ichi->w, 0, 0, FontWidth (xl) * 5, FontHeight (xl), 1, fg, bg);
  if (!ichi->w || !ichi->w1 || !ichi->nyuu_w)
    {
      print_out ("Could not create X resources for ichiran");
      return (-1);
    }

  XSetTransientForHint (dpy, ichi->w, root->root_window);
  if (xim->save_under)
    {
      attributes.save_under = True;
      XChangeWindowAttributes (dpy, ichi->w, CWSaveUnder, &attributes);
    }
  XDefineCursor (dpy, ichi->w, cursor3);
  XSelectInput (dpy, ichi->w, KeyPressMask | StructureNotifyMask);
  XSelectInput (dpy, ichi->w1, ExposureMask);
  XSelectInput (dpy, ichi->nyuu_w, ExposureMask);

  ichi->title = create_box (ichi->w, 0, 0, FontWidth (xl), FontHeight (xl), 1, fg, bg, fg, 0, '\0');
  ichi->subtitle = create_box (ichi->w, 0, 0, FontWidth (xl), FontHeight (xl), 1, fg, bg, fg, 0, '\0');
  ichi->comment = create_box (ichi->w, 0, 0, FontWidth (xl), FontHeight (xl), 1, fg, bg, fg, 0, '\0');
  if (!ichi->title || !ichi->subtitle || !ichi->comment)
    return (-1);
  ichi->title->redraw_cb = set_j_c;
  ichi->subtitle->redraw_cb = set_j_c;
  for (i = 0; i < ichi->max_button; i++)
    {
      ichi->button[i] = create_box (ichi->w, 0, 0, FontWidth (xl), FontHeight (xl), 2, fg, bg, fg, cursor2, 1);
      if (!ichi->button[i])
        return (-1);
      ichi->button[i]->redraw_cb = set_j_c;
    }
  ichi->button[CANCEL_W]->do_ret = ichi->button[EXEC_W]->do_ret = True;
  ichi->button[CANCEL_W]->sel_ret = -2;
  ichi->button[EXEC_W]->sel_ret = 1;
  ichi->button[CANCEL_W]->cb = ichi->button[EXEC_W]->cb = cur_cl_change3;
  ichi->button[MOUSE_W]->cb = xw_mouse_select;
  ichi->button[NEXT_W]->cb = next_ichi;
  ichi->button[BACK_W]->cb = back_ichi;
  map_box (ichi->title);
  map_box (ichi->button[CANCEL_W]);
  XMapWindow (dpy, ichi->w1);

  sprintf (name, "list:%d", root->screen);
  sprintf (class, "List:%d", root->screen);
  set_wm_properties (ichi->w, 0, 0, width, height, SouthWestGravity, name, class, True, 0, InputHint, 0, 0);

  XFlush (dpy);
  return (0);
}

static int
create_inspect (root)
     XIMRootRec *root;
{
  Inspect *ins = root->inspect;
  XIMLangRec *xl = root->ximclient->xl[0];
  unsigned int width, height;
  unsigned long fg, bg;
  int i;
  char name[32], class[32];

  ins->max_button = MAX_BUTTON;

  width = root->ximclient->client_area.width;
  height = root->ximclient->client_area.height;
  fg = root->ximclient->pe.fg;
  bg = root->ximclient->pe.bg;

  ins->w = XCreateSimpleWindow (dpy, root->root_window, 0, 0, width, height, root->bw, fg, bg);
  ins->w1 = XCreateSimpleWindow (dpy, ins->w, 0, FontHeight (xl) * 2, width, height, 1, fg, bg);
  XSetTransientForHint (dpy, ins->w, root->root_window);
  if (xim->save_under)
    {
      attributes.save_under = True;
      XChangeWindowAttributes (dpy, ins->w, CWSaveUnder, &attributes);
    }
  XDefineCursor (dpy, ins->w, cursor3);
  XSelectInput (dpy, ins->w, KeyPressMask | StructureNotifyMask);
  XSelectInput (dpy, ins->w1, ExposureMask);

  ins->title = create_box (ins->w, 0, 0, FontWidth (xl), FontHeight (xl), 1, fg, bg, fg, 0, '\0');
  if (!ins->title)
    return (-1);
  ins->title->redraw_cb = set_j_c;
  for (i = 0; i < ins->max_button; i++)
    {
      ins->button[i] = create_box (ins->w, 0, 0, FontWidth (xl), FontHeight (xl), 2, fg, bg, fg, cursor2, 1);
      if (!ins->button[i])
        return (-1);
      ins->button[i]->redraw_cb = set_j_c;
    }
  ins->button[CANCEL_W]->cb = ins->button[DELETE_W]->cb = ins->button[USE_W]->cb = cur_cl_change3;
  ins->button[CANCEL_W]->do_ret = ins->button[DELETE_W]->do_ret = ins->button[USE_W]->do_ret = True;
  ins->button[CANCEL_W]->sel_ret = -2;
  ins->button[DELETE_W]->sel_ret = 0;
  ins->button[USE_W]->sel_ret = 1;
  ins->button[NEXT_W]->cb = xw_next_inspect;
  ins->button[BACK_W]->cb = xw_back_inspect;
  ins->button[NEXT_W]->cb_data = ins->button[BACK_W]->cb_data = NULL;

  sprintf (name, "inspect:%d", root->screen);
  sprintf (class, "Inspect:%d", root->screen);
  set_wm_properties (ins->w, 0, 0, width, height, SouthWestGravity, name, class, True, 0, InputHint, 0, 0);
  XMapSubwindows (dpy, ins->w);
  XFlush (dpy);
  return (0);
}

int
create_yes_no (xc)
     XIMClientRec *xc;
{
  XIMLangRec *xl = xc->xl[0];
  XIMRootRec *root = xc->root_pointer;
  YesOrNo *yes_no = xc->yes_no;
  register unsigned int width, height;
  unsigned long fg, bg;

  width = 1;
  height = 1;
  fg = xc->pe.fg;
  bg = xc->pe.bg;

  yes_no->root_pointer = root;
  if (!(yes_no->w = XCreateSimpleWindow (dpy, root->root_window, 0, 0, width, height, root->bw, fg, bg)))
    {
      print_out ("Could not create X resource for yes_no");
      return (-1);
    }
  XSetTransientForHint (dpy, yes_no->w, root->root_window);
  if (xim->save_under)
    {
      attributes.save_under = True;
      XChangeWindowAttributes (dpy, yes_no->w, CWSaveUnder, &attributes);
    }
  XDefineCursor (dpy, yes_no->w, cursor3);
  XSelectInput (dpy, yes_no->w, (StructureNotifyMask | KeyPressMask));
  if (!(yes_no->title = create_box (yes_no->w, 0, 0, width, height, 0, fg, bg, fg, 0, '\0')))
    return (-1);
  if (!(yes_no->button[0] = create_box (yes_no->w, (FontWidth (xl) * 2),
                                        (FontHeight (xl) * 2 + FontHeight (xl) / 2), (FontWidth (xl) * 4 + IN_BORDER * 2), (FontHeight (xl) + IN_BORDER * 2), root->bw, fg, bg, fg, cursor2, 1)))
    {
      remove_box (yes_no->title);
      return (-1);
    }
  if (!(yes_no->button[1] = create_box (yes_no->w, (FontWidth (xl) * 10),
                                        (FontHeight (xl) * 2 + FontHeight (xl) / 2), (FontWidth (xl) * 6 + IN_BORDER * 2), (FontHeight (xl) + IN_BORDER * 2), root->bw, fg, bg, fg, cursor2, 1)))
    {
      remove_box (yes_no->title);
      remove_box (yes_no->button[0]);
      return (-1);
    }
  yes_no->title->redraw_cb = cur_cl_change4;
  yes_no->title->redraw_cb_data = (int *) xc;
  map_box (yes_no->title);
  yes_no->button[0]->redraw_cb = cur_cl_change3;
  yes_no->button[1]->redraw_cb = cur_cl_change3;
  yes_no->button[0]->redraw_cb_data = (int *) xc;
  yes_no->button[1]->redraw_cb_data = (int *) xc;
  yes_no->button[0]->sel_ret = 1;
  yes_no->button[1]->sel_ret = 0;
  yes_no->button[0]->cb = yes_no->button[1]->cb = cur_cl_change3;
  yes_no->button[0]->cb_data = yes_no->button[1]->cb_data = (int *) xc;
  yes_no->button[0]->do_ret = yes_no->button[1]->do_ret = True;

  set_wm_properties (yes_no->w, 0, 0, width, height, CenterGravity, "yesNo", "YesNo", True, 0, InputHint, 0, 0);
  XFlush (dpy);
  return (0);
}

void
read_wm_id ()
{
  register int i;
  register XIMRootRec *root;

  if ((wm_id = XInternAtom (dpy, "WM_PROTOCOLS", True)))
    {
      wm_id1 = XInternAtom (dpy, "WM_DELETE_WINDOW", True);
      for (i = 0; i < xim->screen_count; i++)
        {
          root = xim->root_pointer[i];
          XChangeProperty (dpy, root->ximclient->w, wm_id, XA_INTEGER, 32, PropModeReplace, (unsigned char *) &wm_id1, sizeof (Atom));
          XChangeProperty (dpy, root->ximclient->yes_no->w, wm_id, XA_INTEGER, 32, PropModeReplace, (unsigned char *) &wm_id1, sizeof (Atom));
          XChangeProperty (dpy, root->ichi->w, wm_id, XA_INTEGER, 32, PropModeReplace, (unsigned char *) &wm_id1, sizeof (Atom));
          XChangeProperty (dpy, root->inspect->w, wm_id, XA_INTEGER, 32, PropModeReplace, (unsigned char *) &wm_id1, sizeof (Atom));
        }
    }
  else
    {
      wm_id = wm_id1 = 0;
    }
}

int
create_xim_window (root, xc)
     XIMRootRec *root;
     XIMClientRec *xc;
{
  int icon_x = 0, icon_y = 0;
  unsigned int icon_width, icon_height;
  int icon_mask = 0, state;
  long flags;
  char name[32], class[32];

  if (sub_resource.icon_geometry_res)
    {
      icon_mask = XParseGeometry (sub_resource.icon_geometry_res, &icon_x, &icon_y, &icon_width, &icon_height);
    }

  xc->w = XCreateSimpleWindow (dpy, root->root_window, ClientX (xc), ClientY (xc), ClientWidth (xc), ClientHeight (xc), root->bw, xc->pe.fg, xc->pe.bg);
  if (!xc->w)
    {
      print_out ("Could not create X resource for main window.");
      return (-1);
    }
/*
    XSetTransientForHint(dpy, xc->w, root->root_window);
*/
  if (xim->save_under)
    {
      attributes.save_under = True;
      XChangeWindowAttributes (dpy, xc->w, CWSaveUnder, &attributes);
    }
  XSetWindowBorder (dpy, xc->w, root->bc);
  XDefineCursor (dpy, xc->w, cursor3);

  flags = InputHint | StateHint;
  if ((icon_mask & XValue) && (icon_mask & YValue))
    {
      flags |= IconPositionHint;
    }
  if (sub_resource.iconic_start_res && !sub_resource.root_visible_res)
    {
      state = IconicState;
    }
  else
    {
      state = NormalState;
    }
  sprintf (name, "xwnmo:%d", root->screen);
  sprintf (class, "Xwnmo:%d", root->screen);
  set_wm_properties (xc->w, ClientX (xc), ClientY (xc), ClientWidth (xc), ClientHeight (xc), SouthWestGravity, name, class, False, state, flags, icon_x, icon_y);
  XClearWindow (dpy, xc->w);
  XSelectInput (dpy, xc->w, (ButtonPressMask | ButtonReleaseMask | ResizeRedirectMask | StructureNotifyMask));
  return (0);
}

static int
create_one_root (root, lc_name, screen)
     XIMRootRec *root;
     char *lc_name;
     int screen;
{
  register XIMClientRec *xc;
  register XIMLangRec *xl;
  register int i, num;
  int x, y;
  unsigned int width, height;
  int size_of_ximclient = sizeof (XIMClientRec);
  int size_of_yes_no = sizeof (YesOrNo);
  int size_of_ichiran = sizeof (Ichiran);
  int size_of_inspect = sizeof (Inspect);
  int all_size;
  int using_l_len = 0;
  int mask = 0;
  char *p;
#ifdef X11R5
  XLocale xlc;
#endif /* X11R5 */
  XIMNestLangList lc_list = NULL, lc;
  char *def_font;
  char sub_name[64], sub_class[64];
  Arg args[3];

  args[0].name = XtNscreen;
  args[0].value = (XtArgVal) ScreenOfDisplay (dpy, screen);
  args[1].name = XtNcolormap;
  args[1].value = (XtArgVal) DefaultColormap (dpy, screen);

  all_size = size_of_ximclient + size_of_ichiran + size_of_inspect + size_of_yes_no;
  if ((p = (char *) Malloc (all_size)) == NULL)
    {
      malloc_error ("allocation of the initial area");
      return (-1);
    }
  root->ximclient = xc = (XIMClientRec *) p;
  p += size_of_ximclient;
  root->ichi = (Ichiran *) p;
  p += size_of_ichiran;
  root->inspect = (Inspect *) p;
  p += size_of_inspect;
  xc->yes_no = (YesOrNo *) p;

  root->root_window = RootWindow (dpy, screen);
  root->screen = screen;

  xc->root_pointer = root;
  xc->cur_xl = NULL;
  xc->next = NULL;
  if (!(xc->user_name = alloc_and_copy (root_username)))
    {
      malloc_error ("allocation of the initial area");
      return (-1);
    }
  xc->fd = -1;
  xc->input_style = (XIMPreeditArea | XIMStatusArea);
  xc->w = (Window) 0;
  default_xc_set (xc);

  sprintf (sub_name, "screen%d", i);
  sprintf (sub_class, "Screen%d", i);
  XtGetSubresources (toplevel, (XtPointer) & sub_resource, sub_name, sub_class, sub_resources, XtNumber (sub_resources), args, 2);
  if (resource.iconic_start_res)
    sub_resource.iconic_start_res = True;
  if (resource.geometry_res)
    sub_resource.geometry_res = resource.geometry_res;
  if (resource.border_width_res)
    sub_resource.border_width_res = resource.border_width_res;

  root->root_visible = (char) (sub_resource.root_visible_res ? 1 : 0);
  root->root_visible_flag = (char) 0;

  xc->pe.fg = xc->st.fg = sub_resource.fore_color_res;
  xc->pe.bg = xc->st.bg = sub_resource.back_color_res;
  root->bc = sub_resource.border_color_res;
  root->bw = sub_resource.border_width_res;
  if (root->bw == 0)
    root->bw = DEFAULT_BORDER_WIDTH;

  /*      Create GC   */
  xgcv.foreground = xc->pe.fg;
  xgcv.background = xc->pe.bg;
  xc->pe.gc = xc->st.gc = XCreateGC (dpy, root->root_window, (GCForeground | GCBackground), &xgcv);
  /*      Create reverse GC   */
  xgcv.foreground = xc->pe.bg;
  xgcv.background = xc->pe.fg;
  xc->pe.reversegc = xc->st.reversegc = XCreateGC (dpy, root->root_window, (GCForeground | GCBackground), &xgcv);
  /*  Create invert GC */
  xgcv.foreground = xc->pe.fg;
  xgcv.function = GXinvert;
  xgcv.plane_mask = XOR (xc->pe.fg, xc->pe.bg);
  xc->pe.invertgc = XCreateGC (dpy, root->root_window, (GCForeground | GCFunction | GCPlaneMask), &xgcv);
  if (!xc->pe.gc || !xc->pe.reversegc || !xc->pe.invertgc)
    {
      print_out ("Could not create GC");
      return (-1);
    }

  if (sub_resource.geometry_res)
    {
      mask = XParseGeometry (sub_resource.geometry_res, &x, &y, &width, &height);
    }
  if (mask & WidthValue)
    {
      ClientWidth (xc) = width;
    }
  else
    {
      ClientWidth (xc) = DisplayWidth (dpy, root->screen) - (root->bw * 2);
    }

  if ((xc->have_world = get_langlist (lc_name, &lc_list)) < 0)
    return (-1);
  if (xc->have_world)
    xc->world_on = 1;
  else
    xc->world_on = 0;
  for (num = 0, lc = lc_list; lc != NULL; lc = lc->next)
    num++;
  if ((xc->xl = (XIMLangRec **) Malloc (sizeof (XIMLangRec *) * num)) == NULL)
    {
      malloc_error ("allocation of the initial area");
      return (-1);
    }
  for (i = 0, lc = lc_list; lc != NULL; lc = lc->next)
    {
      if (!setlocale (LC_ALL, lc->lang_db->lc_name))
        {
          print_out1 ("Could not set locale \"%s\" for my internal locale.", lc->lang_db->lc_name);
          print_out ("I give up creating of environment for such locale.");
          print_out1 ("Please modify internal locale of ximconf.");
          continue;
        }
      if ((xl = xc->xl[i] = (XIMLangRec *) Calloc (1, sizeof (XIMLangRec))) == NULL)
        {
          malloc_error ("allocation of the initial area");
          return (-1);
        }
      xc->cur_xl = xl;
      xl->lc_list = NULL;
      if (add_locale_to_xl (xl, lc) < 0)
        {
          malloc_error ("allocation of the initial area");
          return (-1);
        }
      xl->cur_lc = xl->lc_list;
      xl->lang_db = lc->lang_db;
      xl->w_c = NULL;
#ifdef X11R5
      if (!(xlc = _XFallBackConvert ()) || !(xl->xlc = _XlcDupLocale (xlc)))
        {
#else
      if (!(xl->xlc = _XlcCurrentLC ()))
        {
#endif /* X11R5 */
          print_out ("Could not create locale environment.");
          return (-1);
        }
#ifdef  USING_XJUTIL
      xl->xjutil_fs_id = -1;
      xl->working_xjutil = 0;
#endif /* USING_XJUTIL */
      /* Create Default Windows       */
      if (sub_resource.font_set_res)
        {
          if (load_font_set (xl, sub_resource.font_set_res, sub_resource.font_set_res) == -1)
            {
              print_out2 ("Could not create any fontset for \"%s\" in locale \"%s\".", sub_resource.font_set_res, lc->lc_name);
              print_out ("I will use defualt fontset.");
            }
        }
      if (xl->pe_fs == NULL)
        {
          if ((def_font = get_default_font_name (xl->lang_db->lang)) == NULL)
            {
              print_out1 ("Could not find default fontset for %s", xl->lang_db->lang);
              goto ERROR_OCCUR1;
            }
          if (load_font_set (xl, def_font, def_font) == -1)
            {
              print_out2 ("Could not create any fontset for \"%s\" in locale \"%s\".", def_font, xl->lang_db->lc_name);
              goto ERROR_OCCUR1;
            }
        }
      if (i == 0)
        {
          ClientHeight (xc) = FontHeight (xl) + FontHeight (xl) / 2;

          xc->columns = (int) ClientWidth (xc) / (int) FontWidth (xl);
          if (xc->columns <= 0)
            xc->columns = 1;
          xc->max_columns = xc->columns - MHL0;
          if (xc->max_columns <= 0)
            xc->max_columns = 1;

          PreeditWidth (xc) = xc->max_columns * FontWidth (xl);
          PreeditHeight (xc) = FontHeight (xl);
          PreeditX (xc) = FontWidth (xl) * (MHL0 + 1);
          PreeditY (xc) = (int) (ClientHeight (xc) - FontHeight (xl)) / 2;

          StatusWidth (xc) = FontWidth (xl) * MHL0;
          StatusHeight (xc) = FontHeight (xl);
          StatusX (xc) = 0;
          StatusY (xc) = PreeditY (xc);
          if (mask & XValue)
            {
              ClientX (xc) = x;
            }
          else
            {
              ClientX (xc) = 0;
            }
          if (mask & YValue)
            {
              ClientY (xc) = y;
            }
          else
            {
              ClientY (xc) = DisplayHeight (dpy, root->screen) - ClientHeight (xc) - (root->bw * 2);
            }
          if (create_xim_window (root, xc) < 0)
            goto ERROR_OCCUR2;
          if (create_yes_no (xc) == -1)
            {
              XDestroyWindow (dpy, xc->w);
              goto ERROR_OCCUR2;
            }
        }
      cur_p = cur_x = xc;
      if (new_client (xc, xl, (screen == 0) ? True : False) == -1)
        {
          print_out1 ("I give up creating of environment for locale \"%s\".", xl->lang_db->lc_name);
          goto ERROR_OCCUR3;
        }
      if (!root->root_visible)
        {
          XMapWindow (dpy, xc->w);
          root->root_visible_flag = (char) 1;
        }
      XFlush (dpy);
      i++;
      using_l_len += strlen (xl->lang_db->lc_name) + 1;
      continue;
    ERROR_OCCUR3:
      if (i == 0)
        {
          remove_box (xc->yes_no->title);
          remove_box (xc->yes_no->button[0]);
          remove_box (xc->yes_no->button[1]);
          Free ((char *) xc->yes_no);
          XDestroyWindow (dpy, xc->w);
        }
    ERROR_OCCUR2:
      XFreeFontSet (dpy, xl->pe_fs);
      Free ((char *) xl->pe_b_char);
      XFreeFontSet (dpy, xl->st_fs);
      Free ((char *) xl->st_b_char);
    ERROR_OCCUR1:
      remove_locale_to_xl (xl, lc);
#ifdef X11R5
      XFree ((char *) xl->xlc);
#endif
      Free (xl);
    }
  if (i == 0)
    {
      print_out ("Could not create any environment.");
      print_out ("I will abort soon.");
      return (-1);
    }
  xc->lang_num = i;
  if (!(xc->using_language = (char *) Malloc (sizeof (char) * using_l_len)))
    {
      malloc_error ("allocation of the initial area");
      return (-1);
    }
  xc->using_language[0] = '\0';
  for (i = 0; i < xc->lang_num; i++)
    {
      strcat (xc->using_language, xc->xl[i]->lang_db->lc_name);
      strcat (xc->using_language, ";");
    }
  xc->using_language[strlen (xc->using_language) - 1] = '\0';
  xc->cur_xl = xc->xl[0];
  cur_p = cur_x = root->ximclient;
  cur_lang = root->ximclient->cur_xl->lang_db;
  c_c = cur_p->cur_xl->w_c;
  cur_rk = c_c->rk;
  cur_rk_table = cur_rk->rk_table;

  add_ximclientlist (xc);

  XMapWindow (dpy, xc->cur_xl->ws);
  if (create_ichi (root) < 0 || create_inspect (root) < 0)
    return (-1);
  if (henkan_off_flag == 0)
    {
      disp_mode ();
    }
  else
    {
      display_henkan_off_mode ();
    }
  return (0);
}

int
create_xim (lc_name)
     char *lc_name;
{
  register XIMLcNameRec *ln;
  register int i;
  int screen_count;
  Atom ret;
  char displaybuf[128];
  short port;
  int lang_len = 0;
  char *p;
  int size_of_xim;
  int size_of_root_pointer;
  int size_of_root;
  int all_size;
#ifndef X11R3
  XTextProperty machineName;
#endif /* X11R3 */
  extern char *getenv ();

#ifdef  X_WCHAR
  dol_wchar_t = 0x20202024;
  cursor_wchar_t = 0x20202020;
#else /* X_WCHAR */
  mbtowc (&dol_wchar_t, "$", 1);
  mbtowc (&cursor_wchar_t, " ", 1);
#endif /* X_WCHAR */

  dpy = XtDisplay (toplevel);
  screen_count = ScreenCount (dpy);
  XSetErrorHandler (xerror_handler);
  XSetIOErrorHandler (xioerror_handler);

#if defined(X11R5) || defined(BC_X11R5)
  if (ret = XInternAtom (dpy, XIM_INPUTMETHOD, True))
    {
      if (XGetSelectionOwner (dpy, ret))
        {
          print_out ("I am already running.");
          return (-1);
        }
    }
#endif /* defined(X11R5) || defined(BC_X11R5) */

  if (DisplayString (dpy))
    {
      strcpy (displaybuf, DisplayString (dpy));
    }
  else if (getenv ("DISPLAY"))
    {
      strcpy (displaybuf, getenv ("DISPLAY"));
    }
  else
    {
      print_out ("I could not open socket.");
      return (-1);
    }
#ifdef USING_XJUTIL
  call_kill_old_xjutil ();
#endif
  if ((port = init_net (ConnectionNumber (dpy), displaybuf)) == (short) -1)
    {
      print_out ("I could not open socket.");
      return (-1);
    }

  cursor1 = XCreateFontCursor (dpy, 52);
  cursor2 = XCreateFontCursor (dpy, 60);
  cursor3 = XCreateFontCursor (dpy, 24);

  size_of_xim = sizeof (XInputManager);
  size_of_root_pointer = sizeof (XIMRootRec *) * screen_count;
  size_of_root = sizeof (XIMRootRec);
  all_size = size_of_xim + size_of_root_pointer + (size_of_root * screen_count);
  if ((p = (char *) Malloc (all_size)) == NULL)
    {
      malloc_error ("allocation of the initial area");
      return (-1);
    }
  xim = (XInputManager *) p;
  p += size_of_xim;
  xim->root_pointer = (XIMRootRec **) p;
  p += size_of_root_pointer;
  for (i = 0; i < screen_count; i++)
    {
      xim->root_pointer[i] = (XIMRootRec *) p;
      p += size_of_root;
    }

  xim->screen_count = screen_count;
  xim->default_screen = DefaultScreen (dpy);
  xim->j_c = NULL;
  xim->cur_j_c_root = NULL;
  xim->sel_ret = -1;
  xim->client_count = 1;
  xim->sel_button = (char) 0;
  xim->exit_menu = (char) 0;
  xim->exit_menu = (char) (resource.exit_menu_res ? 1 : 0);
  xim->exit_menu_flg = (char) 0;
  xim->save_under = (char) (resource.save_under_res ? 1 : 0);
  xim->supported_style[0] = SUPPORT_STYLE_1;
  xim->supported_style[1] = SUPPORT_STYLE_2;
  xim->supported_style[2] = SUPPORT_STYLE_3;
#ifdef  CALLBACKS
  xim->supported_style[3] = SUPPORT_STYLE_4;
  xim->supported_style[4] = SUPPORT_STYLE_5;
  xim->supported_style[5] = SUPPORT_STYLE_6;
  xim->supported_style[6] = SUPPORT_STYLE_7;
#endif /* CALLBACKS */

  ximclient_list = NULL;

  for (ln = lc_name_list; ln; ln = ln->next)
    {
      lang_len += (strlen (ln->lc_name) + 1);
    }
  if (lang_len)
    {
      if (!(xim->supported_language = Malloc (lang_len)))
        {
          malloc_error ("allocation of the initial area");
          return (-1);
        }
      *xim->supported_language = '\0';
      for (ln = lc_name_list; ln; ln = ln->next)
        {
          strcat (xim->supported_language, ln->lc_name);
          strcat (xim->supported_language, ",");
        }
      *(xim->supported_language + (lang_len - 1)) = '\0';
    }
  else
    {
      xim->supported_language = NULL;
    }

  for (i = 0; i < screen_count; i++)
    {
      if (create_one_root (xim->root_pointer[i], lc_name, i) == -1)
        return (-1);
    }
#if defined(X11R5) || defined(BC_X11R5)
  write_ximid (port, xim->root_pointer[xim->default_screen]);
#endif /* defined(X11R5) || defined(BC_X11R5) */
#ifndef X11R5
  if (XimPreConnect (port, xim->root_pointer[xim->default_screen]) < 0)
    return -1;
#endif /* !X11R5 */

  read_wm_id ();
#ifdef  XJPLIB
  XJp_init ();
#endif /* XJPLIB */

#ifndef X11R3
  machineName.encoding = XA_STRING;
  machineName.format = 8;
  machineName.nitems = XmuGetHostname (hostname, sizeof hostname);
  machineName.value = (unsigned char *) hostname;
  XSetWMClientMachine (dpy, xim->root_pointer[xim->default_screen]->ximclient->w, &machineName);
#endif /* X11R3 */
  XChangeProperty (dpy, xim->root_pointer[xim->default_screen]->ximclient->w, XA_WM_COMMAND, XA_STRING, 8, PropModeReplace, (unsigned char *) arg_string, strlen (arg_string));
  return (0);
}

void
xw_end ()
{
  extern int shutdown (), close ();

  close_net ();
  XCloseDisplay (dpy);
}

int
create_preedit (xc, xl, only_window)
     XIMClientRec *xc;
     XIMLangRec *xl;
     int only_window;
{
  register char *p;
  int total_size;
  int width, height, border_width = 0;
  Window parent;
  unsigned long mask = 0L;

  if (IsPreeditNothing (xc))
    return (0);

  if (!only_window)
    {
      if ((p = Malloc ((total_size = (sizeof (wchar) + sizeof (unsigned char)) * (maxchg * 2 + 1)))) == NULL)
        {
          malloc_error ("allocation of client's area");
          return (-1);
        }
      bzero (p, total_size);
      xl->buf = (wchar *) p;
      p += sizeof (wchar) * (maxchg * 2 + 1);
      xl->att = (unsigned char *) p;
      clear_xl (xl);

      xl->del_x = 0;
      xl->vst = 0;
      xl->note[0] = 0;
      xl->note[1] = 0;
      xc->maxcolumns = maxchg * 2;
    }

  if (IsPreeditArea (xc))
    {
      if (xc->pe.area_needed.width < FontWidth (xl) * 2)
        xc->pe.area_needed.width = FontWidth (xl) * 2;
      if (xc->pe.area_needed.height < FontHeight (xl))
        xc->pe.area_needed.height = FontHeight (xl);
      if (PreeditWidth (xc) <= 0)
        PreeditWidth (xc) = 1;
      if (PreeditHeight (xc) <= 0)
        PreeditHeight (xc) = 1;
      height = FontHeight (xl);

      mask = CWBorderPixel;
      attributes.border_pixel = xc->pe.fg;
      if (xc->pe.bg_pixmap)
        {
          mask |= CWBackPixmap;
          attributes.background_pixmap = xc->pe.bg_pixmap;
        }
      else
        {
          mask |= CWBackPixel;
          attributes.background_pixel = xc->pe.bg;
        }
      if (xc->pe.colormap)
        {
          mask |= CWColormap;
          attributes.colormap = xc->pe.colormap;
        }
      mask |= CWEventMask;
      attributes.event_mask = StructureNotifyMask;
      if (xim->save_under)
        {
          attributes.save_under = True;
          mask |= CWSaveUnder;
        }
      xl->wp[0] = XCreateWindow (dpy, xc->w, PreeditX (xc), PreeditY (xc), PreeditWidth (xc), PreeditHeight (xc), border_width, CopyFromParent, InputOutput, CopyFromParent, mask, &attributes);
      mask &= ~CWEventMask;
      if (xim->save_under)
        {
          mask &= ~CWSaveUnder;
        }

      xl->wp[1] = xl->wp[2] = 0;

      xl->wn[2] = XCreateWindow (dpy, xl->wp[0], PreeditWidth (xc) - FontWidth (xl), 0, FontWidth (xl), height, border_width, CopyFromParent, InputOutput, CopyFromParent, mask, &attributes);

      mask |= CWEventMask;
      attributes.event_mask = ExposureMask;
      xl->wn[0] = XCreateWindow (dpy, xl->wp[0], 0, 0, FontWidth (xl), height, border_width, CopyFromParent, InputOutput, CopyFromParent, mask, &attributes);
      xl->wn[1] = XCreateWindow (dpy, xl->wp[0], PreeditWidth (xc) - FontWidth (xl), 0, FontWidth (xl), height, border_width, CopyFromParent, InputOutput, CopyFromParent, mask, &attributes);
      xl->w[0] = XCreateWindow (dpy, xl->wp[0], 0, 0, FontWidth (xl) * (xc->maxcolumns + 1), height, border_width, CopyFromParent, InputOutput, CopyFromParent, mask, &attributes);
      xl->w[1] = xl->w[2] = 0;

      if (!xl->wp[0] || !xl->w[0] || !xl->wn[0] || !xl->wn[1] || !xl->wn[2])
        {
          return (-1);
        }

      return (0);
    }
  else if (IsPreeditPosition (xc))
    {

      width = FontWidth (xl) * (xc->maxcolumns + 1);
      height = FontHeight (xl);
      if (xc->focus_window)
        {
          parent = xc->focus_window;
        }
      else
        {
          parent = xc->w;
        }
      if (PreeditWidth (xc) <= 0)
        PreeditWidth (xc) = 1;

      mask = CWBorderPixel;
      attributes.border_pixel = xc->pe.fg;
      if (xc->pe.bg_pixmap)
        {
          mask |= CWBackPixmap;
          attributes.background_pixmap = xc->pe.bg_pixmap;
        }
      else
        {
          mask |= CWBackPixel;
          attributes.background_pixel = xc->pe.bg;
        }
      if (xc->pe.colormap)
        {
          mask |= CWColormap;
          attributes.colormap = xc->pe.colormap;
        }

      mask |= CWEventMask;
      attributes.event_mask = StructureNotifyMask;
      if (xim->save_under)
        {
          attributes.save_under = True;
          mask |= CWSaveUnder;
        }
      xl->wp[0] = XCreateWindow (dpy, parent, 0, 0, PreeditWidth (xc), height, border_width, CopyFromParent, InputOutput, CopyFromParent, mask, &attributes);
      xl->wp[1] = XCreateWindow (dpy, parent, 0, 0, PreeditWidth (xc), height, border_width, CopyFromParent, InputOutput, CopyFromParent, mask, &attributes);
      xl->wp[2] = XCreateWindow (dpy, parent, 0, 0, PreeditWidth (xc), height, border_width, CopyFromParent, InputOutput, CopyFromParent, mask, &attributes);
      xl->wn[2] = XCreateWindow (dpy, parent, 0, 0, FontWidth (xl), height, border_width, CopyFromParent, InputOutput, CopyFromParent, mask, &attributes);
      mask |= CWEventMask;
      attributes.event_mask = ExposureMask;
      xl->wn[0] = XCreateWindow (dpy, parent, 0, 0, FontWidth (xl), height, border_width, CopyFromParent, InputOutput, CopyFromParent, mask, &attributes);
      xl->wn[1] = XCreateWindow (dpy, parent, 0, 0, FontWidth (xl), height, border_width, CopyFromParent, InputOutput, CopyFromParent, mask, &attributes);
      if (xim->save_under)
        {
          mask &= ~CWSaveUnder;
        }
      mask |= CWBitGravity;
      attributes.bit_gravity = WestGravity;
      xl->w[0] = XCreateWindow (dpy, xl->wp[0], 0, 0, width, height, border_width, CopyFromParent, InputOutput, CopyFromParent, mask, &attributes);
      xl->w[1] = XCreateWindow (dpy, xl->wp[1], 0, 0, width, height, border_width, CopyFromParent, InputOutput, CopyFromParent, mask, &attributes);
      xl->w[2] = XCreateWindow (dpy, xl->wp[2], 0, 0, width, height, border_width, CopyFromParent, InputOutput, CopyFromParent, mask, &attributes);

      if (!xl->wp[0] || !xl->wp[1] || !xl->wp[2] || !xl->w[0] || !xl->w[1] || !xl->w[2] || !xl->wn[0] || !xl->wn[1] || !xl->wn[2])
        {
          return (-1);
        }

      xc->c0 = xc->c1 = xc->c2 = xc->maxcolumns;
      XMapWindow (dpy, xl->w[0]);
      XMapWindow (dpy, xl->w[1]);
      XMapWindow (dpy, xl->w[2]);
      XFlush (dpy);
      return (0);
#ifdef  CALLBACKS
    }
  else if (IsStatusCallbacks (xc))
    {
      return (0);
#endif /* CALLBACKS */
    }
  else
    {
      return (-1);
    }
}

int
create_status (xc, xl, only_window)
     XIMClientRec *xc;
     XIMLangRec *xl;
     int only_window;
{
  register char *p;
  int total_size;
  unsigned long mask = 0L;

  if (IsStatusNothing (xc))
    return (0);

  if (!only_window)
    {
      if ((p = Malloc ((total_size = (sizeof (wchar) + sizeof (unsigned char)) * (MHL0 * 2 + 1)))) == NULL)
        {
          malloc_error ("allocation of client's area");
          return (-1);
        }
      bzero (p, total_size);
      xl->buf0 = (wchar *) p;
      p += sizeof (wchar) * (MHL0 * 2 + 1);
      xl->att0 = (unsigned char *) p;
    }

  if (IsStatusArea (xc))
    {
      if (xc->st.area_needed.width < StatusFontWidth (xl) * MHL0)
        xc->st.area_needed.width = StatusFontWidth (xl) * MHL0;
      if (xc->st.area_needed.height < StatusFontHeight (xl))
        xc->st.area_needed.height = StatusFontHeight (xl);
      if (StatusWidth (xc) <= 0)
        StatusWidth (xc) = 1;
      if (StatusHeight (xc) <= 0)
        StatusHeight (xc) = 1;

      mask = (CWBorderPixel | CWEventMask | CWCursor);
      attributes.border_pixel = xc->pe.fg;
      attributes.event_mask = (ButtonPressMask | ButtonReleaseMask | EnterWindowMask | LeaveWindowMask | ExposureMask | StructureNotifyMask);
      attributes.cursor = cursor2;
      if (xc->st.bg_pixmap)
        {
          mask |= CWBackPixmap;
          attributes.background_pixmap = xc->st.bg_pixmap;
        }
      else
        {
          mask |= CWBackPixel;
          attributes.background_pixel = xc->st.bg;
        }
      if (xc->st.colormap)
        {
          mask |= CWColormap;
          attributes.colormap = xc->st.colormap;
        }
      if (xim->save_under)
        {
          attributes.save_under = True;
          mask |= CWSaveUnder;
        }
      if ((xl->ws = XCreateWindow (dpy, xc->w, StatusX (xc), StatusY (xc), StatusWidth (xc), StatusHeight (xc), 0, CopyFromParent, InputOutput, CopyFromParent, mask, &attributes)) == 0)
        {
          return (-1);
        }
      return (0);
#ifdef  CALLBACKS
    }
  else if (IsStatusCallbacks (xc))
    {
      return (0);
#endif /* CALLBACKS */
    }
  else
    {
      return (-1);
    }
}
