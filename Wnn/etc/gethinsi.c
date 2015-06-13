/*
 *  $Id: gethinsi.c,v 1.7 2013/09/02 11:01:39 itisango Exp $
 */

/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright FreeWnn Project 1999, 2000, 2002
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

/*  品詞ファイルの構造に関する定義  */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#if STDC_HEADERS
#  include <string.h>
#elif HAVE_STRINGS_H
#  include <strings.h>
#endif /* STDC_HEADERS */
#include "commonhd.h"
#include "wnn_config.h"
#include "wnnerror.h"
#include "jslib.h"
#include "hinsi_file.h"
#include "wnn_os.h"
#include "wnn_string.h"
#include "etc.h"

    /*
       wnn_loadhinsi(NULL)                       品詞の情報を読み込む 

       wnn_find_hinsi_by_name(c)                 名前を与えて、品詞番号を取る 
       char *c;

       char *wnn_get_hinsi_name(k)       品詞番号から名前を取る 
       int k;

       int
       wnn_get_fukugou_component(k,str, )  複合品詞に対して 構成要素を求める 
       int k;                         構成要素の個数が返値として返され、
       unsigned short **str;          構成要素は str 以降に返される。

       #ifdef nodef
       int wnn_get_hinsi_list(area)  品詞のリストを得る。
       品詞は、品詞名の配列として管理されている。
       配列の先頭番地を area に入れ、配列の大きさを返す。
       w_char ***area;

       int wnn_get_fukugou_list(area, start) 複合品詞のリストを得る
       複合品詞は、複合品詞構造体の配列として管理されている。
       配列の先頭番地を area に入れ、配列の大きさを返す。
       n 番目の複合品詞の品詞番号は、FUKUGOU_START - n である。
       FUKUGOU_START の値を start に入れて返す。
       struct wnn_fukugou **area;
       int *start;
       int wnn_hinsi_node_component(name, area)
       w_char **area;
       w_char *name;
       品詞ノード名から、その子どものノードの名前の列を得る。
       個数が返る。
       もし、ノードではなく本当の品詞名(リーフ)なら、0 が返る。
       ノードでも品詞名でもないとき、-1 が返る。
       #endif
     */

extern int wnn_errorno;

#ifdef JSERVER
#include "de_header.h"
#define error1 log_debug
#endif

/*
 *here start the real program 
 */


int wnnerror_hinsi;

static int hinsi_loaded = 0;

static int line_no = 0;

static w_char heap[HEAP_LEN];
static w_char *hp = heap;

static unsigned short wheap[WHEAP_LEN];
static unsigned short *whp = wheap;

#define SIZE 1024

static w_char *hinsi[MAXHINSI];
static struct wnn_fukugou fukugou[MAXFUKUGOU];
static struct wnn_hinsi_node node[MAXNODE];

int mhinsi = 0;
int mfukugou = 0;
int mnode = 0;

static void
error_long ()
{
  wnnerror_hinsi = WNN_TOO_LONG_HINSI_FILE_LINE;
}

static void
error_no_heap ()
{
  wnnerror_hinsi = WNN_TOO_BIG_HINSI_FILE;
}

static int
get_char0 (fp)
     FILE *fp;
{
  int c, d;

  for (; (c = getc (fp)) == COMMENT_CHAR || c == CONTINUE_CHAR || c == IGNORE_CHAR1 || c == IGNORE_CHAR2;)
    {
      if (c == CONTINUE_CHAR)
        {
          if ((d = getc (fp)) == EOF)
            {
              break;
            }
          if (d == '\n')
            {
              line_no += 1;
              continue;
            }
          else
            {
              ungetc (d, fp);
              break;
            }
        }
      else if (c == COMMENT_CHAR)
        {
          for (;;)
            {
              if ((c = getc (fp)) == EOF)
                {
                  return (EOF);
                }
              if (c == '\n')
                {
                  ungetc (c, fp);
                  line_no += 1;
                  break;
                }
            }
        }
    }
  if (c == '\n')
    line_no += 1;
  return (c);
}

static int
get_char (fp)                   /* remove null lines */
     FILE *fp;
{
  static int c = -1;
  int d;
  static int fufufu = 0;

  if (c != -1)
    {
      d = c;
      c = -1;
      return (d);
    }
  else
    {
      if (fufufu == 0)
        {                       /* remove all new lines in the head of the file */
          for (; (d = get_char0 (fp)) == '\n';);
          fufufu = 1;
        }
      else
        {
          d = get_char0 (fp);
        }
      if (d == '\n')
        {
          while ((c = get_char0 (fp)) == '\n');
        }
      return (d);
    }
}

/* get one phrase and return the separater */
static int
get_phrase (s0, size, fp)
     UCHAR *s0;
     int size;
     FILE *fp;
{
  UCHAR *s = s0;
  int c;
  static int eof = 0;

  if (eof)
    {
      *s0 = 0;
      return (EOF);
    }
  while ((c = get_char (fp)) != '\n' && c != DEVIDE_CHAR && c != NODE_CHAR && c != HINSI_SEPARATE_CHAR && c != EOF)
    {
      if (s - s0 >= size)
        {
          error_long ();
          return (HINSI_ERR);
        }
      *s++ = c;
    }
  if (c == EOF)
    eof = 1;
  if (s - s0 >= size - 1)
    {
      error_long ();
      return (HINSI_ERR);
    }
  *s++ = '\0';
  return (c);
}

static int
stradd (cp, str)
     w_char **cp;
     char *str;
{
  int len = strlen (str);

  if (hp + len + 1 >= heap + HEAP_LEN)
    {
      error_no_heap ();
      return (-1);
    }
  *cp = hp;
  wnn_Sstrcpy (hp, str);
  hp += wnn_Strlen (hp) + 1;
  return (0);
}

static int
w_stradd (cp, str)
     unsigned short **cp;
     unsigned short *str;
{

  *cp = whp;
  for (; *str != TERMINATE; str++, whp++)
    {
      if (whp >= wheap + WHEAP_LEN)
        {
          error_no_heap ();
          return (-1);
        }
      *whp = *str;
    }
  *whp++ = TERMINATE;
  return (0);
}

int
wnn_loadhinsi (fname)
     unsigned char *fname;
{
  FILE *fp;
  UCHAR buf[SIZE];
  unsigned short fukugou_str[MAXHINSI];
  int sep;
  int h;
  unsigned short *c;
  char tmp[256];

  if (fname == NULL)
    {
#ifdef  JSERVER
      if (hinsi_loaded)
        return (0);
#endif /* JSERVER */
      strcpy (tmp, LIBDIR);
      strcat (tmp, HINSIDATA_FILE);
      fname = (unsigned char *) tmp;
    }

#ifdef  JSERVER
  error1 ("Read HINSI DATA FILE %s\n", fname);
#endif /* JSERVER */

  if ((fp = fopen ((char *) fname, "r")) == NULL)
    {
      wnnerror_hinsi = WNN_NO_HINSI_DATA_FILE;
      goto err_1;
    }
  hinsi_loaded = 1;

  while ((sep = get_phrase (buf, SIZE, fp)) != EOF)
    {
      if (sep == HINSI_ERR)
        {
          goto err;             /* wnnerror_hinsi set in get_phrase */
        }
      if (buf[0] == YOYAKU_CHAR)
        {                       /* yoyaku */
          if (sep != '\n')
            {
              wnnerror_hinsi = WNN_BAD_HINSI_FILE;
              goto err;
            }
          hinsi[mhinsi++] = NULL;
        }
      else if (sep == '\n')
        {                       /* hinsi */
          if (stradd (&hinsi[mhinsi++], buf))
            goto err;
        }
      else if (sep == DEVIDE_CHAR)
        {                       /* fukugou */
          if (stradd (&fukugou[mfukugou].name, buf))
            goto err;
          c = fukugou_str;
          while ((sep = get_phrase (buf, SIZE, fp)) != EOF)
            {
              if (sep == -1)
                {
                  goto err;     /* wnnerror_hinsi set in get_phrase */
                }
              if (sep != EOF && sep != HINSI_SEPARATE_CHAR && sep != '\n')
                {
                  wnnerror_hinsi = WNN_BAD_HINSI_FILE;
                  goto err;
                }
              if ((h = wnn_find_hinsi_by_name (buf)) == -1 || h >= mhinsi)
                {
                  wnnerror_hinsi = WNN_BAD_HINSI_FILE;
                  goto err;
                }
              *c++ = h;
              if (sep == '\n' || sep == EOF)
                break;
            }
          *c = TERMINATE;
          if (w_stradd (&fukugou[mfukugou++].component, fukugou_str))
            goto err;
        }
      else if (sep == NODE_CHAR)
        {
          int first = 1;
          w_char *dummy;

          node[mnode].kosuu = 0;
          if (stradd (&node[mnode].name, buf))
            goto err;
          while ((sep = get_phrase (buf, SIZE, fp)) != EOF)
            {
              if (sep == -1)
                {
                  goto err;     /* wnnerror_hinsi set in get_phrase */
                }
              if (sep != EOF && sep != HINSI_SEPARATE_CHAR && sep != '\n')
                {
                  wnnerror_hinsi = WNN_BAD_HINSI_FILE;
                  goto err;
                }
              node[mnode].kosuu++;
              if (first)
                {
                  if (stradd (&node[mnode].son, buf))
                    goto err;
                  first = 0;
                }
              else
                {
                  if (stradd (&dummy, buf))
                    goto err;
                }
              if (sep == '\n' || sep == EOF)
                break;
            }
          mnode++;
        }
    }
  fclose (fp);
  return (0);
err:
  fclose (fp);
err_1:
#ifdef  JSERVER
  error1 ("Error reading HINSI DATA FILE %s\n", fname);
#endif /* JSERVER */
  return (HINSI_ERR);
}

static int
find_hinsi_by_name (c)
     register w_char *c;
{
  register int k;
  if (!hinsi_loaded)
    {
      if (wnn_loadhinsi (NULL) != 0)
        {
          return (-1);
        }
    }
  for (k = 0; k < mhinsi; k++)
    {
      if (hinsi[k] && wnn_Strcmp (hinsi[k], c) == 0)
        {
          return (k);
        }
    }
  for (k = 0; k < mfukugou; k++)
    {
      if (fukugou[k].name && wnn_Strcmp (fukugou[k].name, c) == 0)
        {
          return (FUKUGOU_START - k);
        }
    }
  return (-1);
}


int
wnn_find_hinsi_by_name (c)
     register char *c;
{
  w_char hin[WNN_HINSI_NAME_LEN];

  wnn_Sstrcpy (hin, c);
  return (find_hinsi_by_name (hin));
}


static w_char *
get_hinsi_name (k)
     int k;
{
  if (!hinsi_loaded)
    {
      if (wnn_loadhinsi (NULL) != 0)
        {
          return (NULL);
        }
    }
  if (k < mhinsi && k >= 0)
    {
      return (hinsi[k]);
    }
  else if (k > FUKUGOU_START - mfukugou)
    {
      return (fukugou[FUKUGOU_START - k].name);
    }
  return (NULL);
}

char *
wnn_get_hinsi_name (k)
     int k;
{
  w_char *s;
  static char hin[WNN_HINSI_NAME_LEN * 2];

  if ((s = get_hinsi_name (k)) == NULL)
    return (NULL);
  wnn_sStrcpy (hin, s);
  return (hin);
}

#ifndef JSERVER
static
#endif                          /* JSERVER */
  int
wnn_get_fukugou_component_body (k, shp)
     register int k;
     register unsigned short **shp;
{
  static unsigned short tmp;
  register unsigned short *s;
  int index;                    /* need for NEWS-OS 6.0 */
  if (k < mhinsi && k >= 0)
    {
      tmp = k;
      *shp = &tmp;
      return (1);
    }
  if (k > FUKUGOU_START - mfukugou && k <= FUKUGOU_START)
    {
      index = FUKUGOU_START - k;
      for (*shp = s = fukugou[index].component; *s != TERMINATE; s++);
/*
        If next line in NEWS-OS 6.0, jserver down when kanji henkan.
        for(*shp = s = fukugou[FUKUGOU_START - k].component;*s != TERMINATE;s++);
*/
      return (s - *shp);
    }
  return (-1);
}

int
wnn_get_fukugou_component (k, shp)
     register int k;
     register unsigned short **shp;
{
  if (!hinsi_loaded)
    {
      if (wnn_loadhinsi (NULL) != 0)
        {
          return (-1);
        }
    }
  return (wnn_get_fukugou_component_body (k, shp));
}


#ifdef JSERVER

w_char *
wnn_hinsi_name (no)
     int no;
{
  w_char *c;
  if ((c = get_hinsi_name (no)) == NULL)
    {
      wnn_errorno = WNN_BAD_HINSI_NO;
    }
  return (c);
}

int
wnn_hinsi_number (name)
     w_char *name;
{
  int n;
  if ((n = find_hinsi_by_name (name)) == -1)
    {
      wnn_errorno = WNN_BAD_HINSI_NAME;
    }
  return (n);
}

int
wnn_hinsi_list (name, c, mynode, mmynode)
     w_char *name;
     w_char **c;
     struct wnn_hinsi_node *mynode;
     int mmynode;
{
  int k;

  if (mynode == NULL)
    {
      mynode = node;
      mmynode = mnode;
    }
  if (!hinsi_loaded)
    wnn_loadhinsi (NULL);
  for (k = 0; k < mmynode; k++)
    {
      if (wnn_Strcmp (name, mynode[k].name) == 0)
        {
          *c = mynode[k].son;
          return (mynode[k].kosuu);
        }
    }
  if (find_hinsi_by_name (name) == -1)
    {
      wnn_errorno = WNN_BAD_HINSI_NAME;
      return (-1);
    }
  return (0);
}

int
wnn_has_hinsi (mynode, mmynode, name)
     struct wnn_hinsi_node *mynode;
     int mmynode;
     w_char *name;
{
  w_char *c;
  int k, j;
  if (mynode == NULL)
    {
      mynode = node;
      mmynode = mnode;
    }
  for (k = 0; k < mmynode; k++)
    {
      if (wnn_Strcmp (name, mynode[k].name) == 0)
        {
          return (1);
        }
      else
        {
          c = mynode[k].son;
          for (j = 0; j < mynode[k].kosuu; j++)
            {
              if (wnn_Strcmp (name, c) == 0)
                {
                  return (1);
                }
              else
                {
                  c += wnn_Strlen (c) + 1;
                }
            }
        }
    }
  return (0);
}

#endif
