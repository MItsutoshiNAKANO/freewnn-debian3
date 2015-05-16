/*
 * $Id: xlc_util.c,v 1.2 2001/06/14 18:16:18 ura Exp $
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
#include <string.h>
#include "commonhd.h"
#include "sdefine.h"
#ifdef  XJUTIL
#include "xjutil.h"
#include "sxheader.h"
#include "xext.h"
#else /* XJUTIL */
#include "xim.h"
#include "sheader.h"
#include "ext.h"
#endif /* XJUTIL */

#ifdef X11R5
extern int _XConvertMBToWC (), _XConvertMBToCT (), _XConvertWCToCT ();
#ifndef X_WCHAR
extern int _XConvertWCToMB ();
#endif
#else
#include "Xlibint.h"
#include "XlcPubI.h"

#define BadBuffer       (-1)

static int
_XConvertWCToMB (lcd, wstr, wc_len, str, mb_bytes, scanned_len)
     XLCd lcd;
     wchar *wstr;
     int wc_len;
     unsigned char *str;
     int *mb_bytes;
     int *scanned_len;
{
  /* modified from _Xlcwcstombs in lcStd.c */
  XlcConv conv;
  XPointer from, to;
  int from_left, to_left, ret;
  int len = *mb_bytes;

  if (lcd == NULL)
    {
      lcd = _XlcCurrentLC ();
      if (lcd == NULL)
        return -1;
    }

  conv = _XlcOpenConverter (lcd, XlcNWideChar, lcd, XlcNMultiByte);
  if (conv == NULL)
    return -1;

  from = (XPointer) wstr;
  from_left = wc_len;
  to = (XPointer) str;
  to_left = len;

  if (_XlcConvert (conv, &from, &from_left, &to, &to_left, NULL, 0) < 0)
    ret = -1;
  else
    {
      ret = len - to_left;
      if (str && to_left > 0)
        str[ret] = '\0';
    }

  _XlcCloseConverter (conv);

  *mb_bytes = ret;
  if (scanned_len)
    *scanned_len = wc_len - from_left;
  return ret;

}

/* ARGSUSED */
static int
_XConvertMBToWC (lcd, str, mb_bytes, wstr, wc_len, scanned_bytes, state)
     XLCd lcd;
     unsigned char *str;
     int mb_bytes;
     wchar *wstr;
     int *wc_len;
     int *scanned_bytes;
     int *state;
{
  /* modified from _Xlcmbstowcs in lcStd.c */
  XlcConv conv;
  XPointer from, to;
  int from_left, to_left, ret;
  int len = *wc_len;

  if (lcd == NULL)
    {
      lcd = _XlcCurrentLC ();
      if (lcd == NULL)
        return -1;
    }

  conv = _XlcOpenConverter (lcd, XlcNMultiByte, lcd, XlcNWideChar);
  if (conv == NULL)
    return -1;

  from = (XPointer) str;
  from_left = mb_bytes;
  to = (XPointer) wstr;
  to_left = len;

  if (_XlcConvert (conv, &from, &from_left, &to, &to_left, NULL, 0) < 0)
    ret = -1;
  else
    {
      ret = len - to_left;
      if (wstr && to_left > 0)
        wstr[ret] = (wchar_t) 0;
    }

  _XlcCloseConverter (conv);

  *wc_len = ret;
  if (scanned_bytes)
    *scanned_bytes = mb_bytes - from_left;
  return 0;
}

/* ARGSUSED */
static int
_XConvertMBToCT (lcd, str, mb_bytes, ctstr, ct_bytes, scanned_bytes, state)
     XLCd lcd;
     unsigned char *str;
     int mb_bytes;
     unsigned char *ctstr;
     int *ct_bytes;
     int *scanned_bytes;
     int *state;
{
  /* modified from _Xlcmbstowcs in lcStd.c */
  XlcConv conv;
  XPointer from, to;
  int from_left, to_left, ret;
  int len = *ct_bytes;

  if (lcd == NULL)
    {
      lcd = _XlcCurrentLC ();
      if (lcd == NULL)
        return -1;
    }

  conv = _XlcOpenConverter (lcd, XlcNMultiByte, lcd, XlcNCompoundText);
  if (conv == NULL)
    return -1;

  from = (XPointer) str;
  from_left = mb_bytes;
  to = (XPointer) ctstr;
  to_left = len;

  if (_XlcConvert (conv, &from, &from_left, &to, &to_left, NULL, 0) < 0)
    ret = -1;
  else
    {
      ret = len - to_left;
      if (ctstr && to_left > 0)
        ctstr[ret] = '\0';
    }

  _XlcCloseConverter (conv);

  *ct_bytes = ret;
  if (scanned_bytes)
    *scanned_bytes = mb_bytes - from_left;
  return 0;
}

static int
_XConvertWCToCT (lcd, wstr, wc_len, str, ct_bytes, scanned_len)
     XLCd lcd;
     wchar *wstr;
     int wc_len;
     unsigned char *str;
     int *ct_bytes;
     int *scanned_len;
{
  /* modified from _Xlcwcstombs in lcStd.c */
  XlcConv conv;
  XPointer from, to;
  int from_left, to_left, ret;
  int len = *ct_bytes;

  if (lcd == NULL)
    {
      lcd = _XlcCurrentLC ();
      if (lcd == NULL)
        return -1;
    }

  conv = _XlcOpenConverter (lcd, XlcNWideChar, lcd, XlcNCompoundText);
  if (conv == NULL)
    return -1;

  from = (XPointer) wstr;
  from_left = wc_len;
  to = (XPointer) str;
  to_left = len;

  if (_XlcConvert (conv, &from, &from_left, &to, &to_left, NULL, 0) < 0)
    ret = -1;
  else
    {
      ret = len - to_left;
      if (str && to_left > 0)
        str[ret] = '\0';
    }

  _XlcCloseConverter (conv);

  *ct_bytes = ret;
  if (scanned_len)
    *scanned_len = wc_len - from_left;
  return ret;

}

#endif /* X11R5 */

int
alloc_all_buf ()
{
  char *wc, *ct, *c;
#ifndef X_WCHAR
  char *w;
#endif /* !X_WCHAR */
  if (((wc = Malloc (sizeof (wchar) * BUF_ALLOC_SIZE)) == NULL) || ((ct = Malloc (sizeof (unsigned char) * BUF_ALLOC_SIZE)) == NULL) || ((c = Malloc (sizeof (unsigned char) * BUF_ALLOC_SIZE)) == NULL)
#ifndef X_WCHAR
      || ((w = Malloc (sizeof (wchar_t) * BUF_ALLOC_SIZE)) == NULL)
#endif /* !X_WCHAR */
    )
    {
      malloc_error ("allocation of temporary buffer");
      return (-1);
    }
  wc_buf = (wchar *) wc;
  ct_buf = (unsigned char *) ct;
  c_buf = (unsigned char *) c;
  wc_buf_max = ct_buf_max = c_buf_max = BUF_ALLOC_SIZE;
#ifndef X_WCHAR
  wt_buf = (wchar_t *) w;
  wt_buf_max = BUF_ALLOC_SIZE;
#endif /* !X_WCHAR */
  return (0);
}

int
realloc_wc_buf ()
{
  register char *p;

  if ((p = Malloc (sizeof (wchar) * (wc_buf_max + BUF_ALLOC_SIZE))) == NULL)
    {
      malloc_error ("re-allocation of temporary buffer");
      return (-1);
    }
  Free ((char *) wc_buf);
  wc_buf = (wchar *) p;
  wc_buf_max += BUF_ALLOC_SIZE;
  return (0);
}

int
realloc_ct_buf ()
{
  register char *p;
  if ((p = Malloc (sizeof (unsigned char) * (ct_buf_max + BUF_ALLOC_SIZE))) == NULL)
    {
      malloc_error ("re-allocation of temporary buffer");
      return (-1);
    }
  Free ((char *) ct_buf);
  ct_buf = (unsigned char *) p;
  ct_buf_max += BUF_ALLOC_SIZE;
  return (0);
}

int
realloc_c_buf ()
{
  register char *p;

  if ((p = Malloc (sizeof (unsigned char) * (c_buf_max + BUF_ALLOC_SIZE))) == NULL)
    {
      malloc_error ("re-allocation of temporary buffer");
      return (-1);
    }
  Free ((char *) c_buf);
  c_buf = (unsigned char *) p;
  c_buf_max += BUF_ALLOC_SIZE;
  return (0);
}

#ifndef X_WCHAR
int
realloc_wt_buf ()
{
  register char *p;

  if ((p = Malloc (sizeof (wchar_t) * (wt_buf_max + BUF_ALLOC_SIZE))) == NULL)
    {
      malloc_error ("re-allocation of temporary buffer");
      return (-1);
    }
  Free ((char *) wt_buf);
  wt_buf = (wchar_t *) p;
  wt_buf_max += BUF_ALLOC_SIZE;
  return (0);
}
#endif /* !X_WCHAR */

int
get_columns_wchar (from)
     register wchar *from;
{
  register wchar *wc = from;

  while (*wc == PENDING_WCHAR)
    wc++;
  if (*wc)
    wc++;
  return (wc - from);
}

#ifndef X11R5
/*
 * wchar encoding is for EUC in X11R6.
 * WARNING:  This is rough alorithm, unless we can call ANSI C function.
 */
static int
_XcwGetLength (wc)
     wchar wc;
{
  if (!(wc & ~0xFF))
    return 1;
  else
    return 2;
}
#endif /* !X11R5 */

int
XwcGetColumn (wc)
     wchar wc;
{
  if (wc == PENDING_WCHAR)
    return (0);
#if defined(__STDC__) && defined(_I18N_)
  return wcwidth (wc);
#else
  return _XcwGetLength (wc);
#endif /* defined(__STDC__) && defined(_I18N_) */
}

int
check_mb (buf, x)
     register wchar *buf;
     register int x;
{
  if (*(buf + x) == PENDING_WCHAR)
    return (1);
  if (XwcGetColumn (*(buf + x)) > 1)
    return (2);
  return (0);
}

int
w_char_to_char (w, c, n)
     register w_char *w;
     register char *c;
     register int n;
{
#ifndef XJUTIL
  set_cswidth (cur_lang->cswidth_id);
#endif /* !XJUTIL */
  return (ieuc_to_eeuc (c, w, (n * sizeof (w_char))));
}

int
skip_pending_wchar (to, from)
     register wchar *to, *from;
{
  register wchar *wc = to;
  for (; *from;)
    {
      if (*from == PENDING_WCHAR)
        from++;
      *wc++ = *from++;
    }
  *wc = 0;
  return (wc - to);
}

int
put_pending_wchar_and_flg (wc, buf, att, flg)
     register wchar wc;
     wchar *buf;
     unsigned char *att, flg;
{
  register wchar *p = buf;
  register unsigned char *f = att;
  register int mb_len, i;

  mb_len = XwcGetColumn (wc);
  for (i = 1; i < mb_len; i++)
    {
      *p++ = PENDING_WCHAR;
      *f++ = flg;
    }
  *p++ = wc;;
  *f++ = flg;
  return (mb_len);
}

#ifdef  CALLBACKS
int
XwcGetChars (wc, end, left)
     register wchar *wc;
     int end;
     int *left;
{
  register int i, cnt;

  *left = 0;
  for (i = 0, cnt = 0; i < end && *wc; i++, cnt++, wc++)
    {
      if (*wc == PENDING_WCHAR)
        {
          wc++;
          i++;
          if (!(i < end))
            {
              for (*left = 1; *wc == PENDING_WCHAR; wc++)
                *left += 1;
              cnt++;
              return (cnt);
            }
        }
    }
  return (cnt);
}
#endif /* CALLBACKS */

int
char_to_wchar (xlc, c, wc, len, wc_len)
#ifdef X11R5
     XLocale xlc;
#else
     XLCd xlc;
#endif /* X11R5 */
     unsigned char *c;
     wchar *wc;
     int len;
     int wc_len;
{
  int wc_str_len = wc_len;
  int scanned_byte = 0;
  int ret;

  ret = _XConvertMBToWC (xlc, c, len, wc, &wc_str_len, &scanned_byte, NULL);
  if (ret < 0)
    {
      if (ret == BadBuffer)
        {
          return (-1);
        }
      else
        {
          return (-2);
        }
    }
  return (wc_str_len);
}

int
w_char_to_wchar (xlc, w, wc, len, wc_len)
#ifdef X11R5
     XLocale xlc;
#else
     XLCd xlc;
#endif /* X11R5 */
     w_char *w;
     wchar *wc;
     int len;
     int wc_len;
{
  int ret;

  while (c_buf_max < (len * 2))
    {
      if (realloc_c_buf () < 0)
        return (0);
    }
  ret = w_char_to_char (w, c_buf, len);
  if (ret <= 0)
    return (0);

  return (char_to_wchar (xlc, (unsigned char *) c_buf, wc, ret, wc_len));
}

int
w_char_to_ct (xlc, w, ct, len, ct_len)
#ifdef X11R5
     XLocale xlc;
#else
     XLCd xlc;
#endif /* X11R5 */
     w_char *w;
     unsigned char *ct;
     int len;
     int ct_len;
{
  int ret;
  int ct_str_len = ct_len;
  int scanned_byte = 0;

  while (c_buf_max < (len * 2))
    {
      if (realloc_c_buf () < 0)
        return (0);
    }
  ret = w_char_to_char (w, c_buf, len);
  if (ret <= 0)
    return (0);

  ret = _XConvertMBToCT (xlc, c_buf, ret, ct, &ct_str_len, &scanned_byte, NULL);
  if (ret < 0)
    {
      if (ret == BadBuffer)
        {
          return (-1);
        }
      else
        {
          return (-2);
        }
    }
  return (ct_str_len);
}

int
wchar_to_ct (xlc, w, ct, len, ct_len)
#ifdef X11R5
     XLocale xlc;
#else
     XLCd xlc;
#endif /* X11R5 */
     wchar *w;
     unsigned char *ct;
     int len;
     int ct_len;
{
  int scanned_byte;
  int ct_str_len = ct_len;
  int ret;

  ret = _XConvertWCToCT (xlc, w, len, ct, &ct_str_len, &scanned_byte, NULL);
  if (ret < 0)
    {
      if (ret == BadBuffer)
        {
          return (-1);
        }
      else
        {
          return (-2);
        }
    }
  return (ct_str_len);
}

void
JWOutput (w, fs, gc, start_col, col, flg, offset_x, offset_y, cs, text, text_len)
     Window w;
     XFontSet fs;
     GC gc;
     short start_col, col;
     unsigned char flg;
     short offset_x, offset_y;
     XCharStruct *cs;
     wchar *text;
     int text_len;
{
  register int start_x, start_y;
  wchar_t *w_text;
  int w_text_len;
#ifndef X_WCHAR
  int mb_buf_len;
  int scanned_byte;
  int ret;
#endif /* !X_WCHAR */

#ifdef  X_WCHAR
  w_text = (wchar_t *) text;
  w_text_len = text_len;
#else /* X_WCHAR */
  while (1)
    {
      mb_buf_len = c_buf_max;
      ret = _XConvertWCToMB (
#ifdef  XJUTIL
                              NULL,
#else
                              cur_p->cur_xl->xlc,
#endif
                              text, text_len, c_buf, &mb_buf_len, &scanned_byte, NULL);
      if (ret < 0)
        {
          if (ret == BadBuffer)
            {
              if (realloc_c_buf () < 0)
                return;
            }
          else
            {
              return;
            }
        }
      else
        {
          break;
        }
    }
  w_text = wt_buf;
  while (1)
    {
      w_text_len = mbstowcs (wt_buf, (char *) c_buf, wt_buf_max);
      if (w_text_len == wt_buf_max)
        {
          if (realloc_wt_buf () < 0)
            return;
        }
      else
        {
          break;
        }
    }
#endif /* X_WCHAR */

  start_x = (cs->width * start_col) + offset_x;
  start_y = cs->ascent + offset_y;
  if (flg & REV_FLAG)
    {
      XwcDrawImageString (dpy, w, fs, gc, start_x, start_y, w_text, w_text_len);
    }
  else
    {
      XClearArea (dpy, w, start_x, offset_y, (cs->width * col), (cs->ascent + cs->descent), False);
      XwcDrawString (dpy, w, fs, gc, start_x, start_y, w_text, w_text_len);
    }
  if (flg & BOLD_FLAG)
    XwcDrawString (dpy, w, fs, gc, (start_x + 1), start_y, w_text, w_text_len);
  if (flg & UNDER_FLAG)
    XDrawLine (dpy, w, gc, start_x, (start_y + 1), (start_x + cs->width * col), start_y + 1);
  return;
}

XCharStruct *
get_base_char (fs)
     XFontSet fs;
{
  XCharStruct *cs;
  XFontStruct **fs_list;
  char **fn_list;

  if (XFontsOfFontSet (fs, &fs_list, &fn_list) < 1)
    {
      print_out ("I could not get base char struct.");
      return (NULL);
    }
  if (!(cs = (XCharStruct *) Malloc (sizeof (XCharStruct))))
    {
      malloc_error ("allocation of base char struct");
      return (NULL);
    }
  cs->width = fs_list[0]->max_bounds.width;
  cs->ascent = fs_list[0]->ascent;
  cs->descent = fs_list[0]->descent;
  return ((XCharStruct *) cs);
}

/* *INDENT-OFF* */
XFontSet
create_font_set (s)
     char *s;
/* *INDENT-ON* */

{
  XFontSet xfontset;
  char **missing_charset_list;
  int missing_charset_count;
  char *def_string;

  xfontset = XCreateFontSet (dpy, s, &missing_charset_list, &missing_charset_count, &def_string);
  if ((xfontset == NULL) /* || (missing_charset_count > 0) */ )
    {
      print_out ("Can not create FontSet\n");
      return ((XFontSet) NULL);
    }
  if (missing_charset_count > 0)
    {
      XFreeStringList (missing_charset_list);
    }
  return ((XFontSet) xfontset);
}
