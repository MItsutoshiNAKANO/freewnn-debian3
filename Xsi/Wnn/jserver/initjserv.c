/*
 *  $Id: initjserv.c $
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

#include <config.h>

#include <stdio.h>
#include <ctype.h>

#if defined(HAVE_SYS_TYPES_H)
#include <sys/types.h>
#endif

#include "commonhd.h"
#include "de_header.h"
#include "wnn_malloc.h"
#include "wnn_config.h"
#include "kaiseki.h"

struct wnn_param default_para = {
  2, 10,
/* Ｎ 小文節数 */
  2, 34, 5, 80, 5, 1, 20, 0,
/* 頻度 小文節長 幹語長 今使ったよ 辞書 小文節評価 大文節長 小文節数 */
  400, -100, 400, 80, 200, 2, 200
/* 数字 カナ 英数 記号 閉括弧 付属語 開括弧 */
};

static int read_default_file (), change_ascii_to_int (), get_bcksla ();
#ifndef CHINESE
static int expand_argument ();
#endif

static int
expand_expr (s)
 /**    @LIBDIRの展開(但し、文字列の先頭のみ)。できない時は-1が
        返り、その場合sの中身は着々とそのまんま。sの長さ＜256と仮定してる。*/
 /**    @USR (env名、logname), @LANG の展開 */
     char *s;
{
  char tmp[EXPAND_PATH_LENGTH];
  register char *p, *s1;
  int noerr, expandsuc;

  if (*s != '@')
    {
      strcpy (tmp, s);
      *s = '\0';
      noerr = 1;
    }
  else
    {
      if ((int) strlen (s) >= EXPAND_PATH_LENGTH)
        return (-1);

      s1 = s;
#ifdef BSD42
      if (NULL != (p = index (++s1, '/')))
        {
#else
      if (NULL != (p = strchr (++s1, '/')))
        {
#endif
          strcpy (tmp, p);
          *p = '\0';
        }
      else
        *tmp = '\0';
      /* ここまでは準備。s…先頭、s1…２文字目、p…最初の'/'のあったところ
         （ここで一旦切る）、tmp…それ以後のコピー。 */

      if (!strcmp (s1, "LIBDIR"))
        {
          noerr = expandsuc = ((int) (strlen (p = LIBDIR) + strlen (tmp)) < EXPAND_PATH_LENGTH);
        }
      else
        {                       /* @LIBDIR igai ha kaenai */
          noerr = 1;
          expandsuc = 0;
        }
      if (expandsuc)
        strcpy (s, p);
    }

  if (noerr)
    {
      int len1 = strlen ("@LANG");
      p = tmp;
      for (; *p; p++)
        {
          if (!strncmp (p, "@LANG", len1))
            {
              if ((int) (strlen (lang_dir) + strlen (p) + strlen (s) - len1) < EXPAND_PATH_LENGTH)
                {
                  strcat (s, lang_dir);
                  p += len1 - 1;
                }
              else
                {
                  return (-1);
                }
            }
          else
            {
              strncat (s, p, 1);
            }
        }
    }
  return (noerr ? 0 : -1);
}

/* daemon initialize routin */
int
read_default ()
{
  FILE *fp, *fopen ();
  char data[EXPAND_PATH_LENGTH];
  char code[EXPAND_PATH_LENGTH];
  char s[20][EXPAND_PATH_LENGTH];
  register int num;
#ifndef CHINESE
  register int i;
#endif

  strcpy (jserver_dir, JSERVER_DIR);

  if ((fp = fopen (jserverrcfile, "r")) == NULL)
    {
      perror ("");
      printf ("Error can't open %s\n", jserverrcfile);
      return (-1);
    }

  while (fgets (data, EXPAND_PATH_LENGTH, fp) != NULL)
    {
      num = sscanf (data,
                    "%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s",
                    code, s[0], s[1], s[2], s[3], s[4], s[5], s[6], s[7], s[8], s[9], s[10], s[11], s[12], s[13], s[14], s[15], s[16], s[17], s[18], s[19]);
/*
        if(strcmp(code, "jt_len") == 0){
            jt_len = atoi(s[0]);
        }else if(strcmp(code, "hjt_len") == 0){
            hjt_len = atoi(s[0]);
        }
*/
      if (strcmp (code, "max_client") == 0)
        {
          sscanf (data, "%s %d ", code, &max_client);
          error1 ("max_client=%d\n", max_client);
        }
      else if (strcmp (code, "max_sticky_env") == 0)
        {
          sscanf (data, "%s %d ", code, &max_sticky_env);
          error1 ("max_sticky_env=%d\n", max_sticky_env);
        }
      else if (strcmp ((code + 1), "server_dir") == 0)
        {
          if (expand_expr (s[0]) != 0)
            {
              printf ("Error can't expand %s\n", s[0]);
            }
          strcpy (jserver_dir, s[0]);
        }
      else if (strcmp (code, "def_param") == 0)
        {
          change_ascii_to_int (s[0], &default_para.n);
          change_ascii_to_int (s[1], &default_para.nsho);
          change_ascii_to_int (s[2], &default_para.p1);
          change_ascii_to_int (s[3], &default_para.p2);
          change_ascii_to_int (s[4], &default_para.p3);
          change_ascii_to_int (s[5], &default_para.p4);
          change_ascii_to_int (s[6], &default_para.p5);
          change_ascii_to_int (s[7], &default_para.p6);
          change_ascii_to_int (s[8], &default_para.p7);
          change_ascii_to_int (s[9], &default_para.p8);
          change_ascii_to_int (s[10], &default_para.p9);

          change_ascii_to_int (s[11], &default_para.p10);
          change_ascii_to_int (s[12], &default_para.p11);
          change_ascii_to_int (s[13], &default_para.p12);
          change_ascii_to_int (s[14], &default_para.p13);
          change_ascii_to_int (s[15], &default_para.p14);
          change_ascii_to_int (s[16], &default_para.p15);
#ifndef CHINESE
        }
      else if (strcmp (code, "set_giji_eisuu") == 0 && num >= 2)
        {
          for (i = 0; i < num - 1; i++)
            {
              giji_eisuu[i] = expand_argument (s[i]);
            }
          for (; i < 20; i++)
            {
              giji_eisuu[i] = 0xffff;
            }
#endif
        }
    }
  fclose (fp);
  return (0);
}

int
read_default_files ()
{
  FILE *fp, *fopen ();
  char data[256];
  int num;
  char code[256];
  char file[256];

  if ((fp = fopen (jserverrcfile, "r")) == NULL)
    {
      perror ("");
      printf ("Error can't open %s\n", jserverrcfile);
      return (-1);
    }
  while (fgets (data, 256, fp) != NULL)
    {
      num = sscanf (data, "%s %s", code, file);
      if (strcmp (code, "readfile") == 0 && num == 2)
        {
          read_default_file (file, 256);
        }
    }
  fclose (fp);

  printf ("Finished Reading Files\n");

  return (0);
}

/*
dummy_env()
{
    int i = 0;
    struct cnv_env *ne;

    if((ne= (struct cnv_env *)malloc(sizeof(struct cnv_env)))==0) return -1;
    env[i]=ne; new_env(i,"dummy_env");
}
*/

static int
read_default_file (buffer, buffer_size)
     char *buffer;
     size_t buffer_size;
{
  int fid;

  buffer = expand_file_name (buffer, buffer_size);
  if (!buffer)
    {
      printf ("filename too long. %s\n", buffer);
      return (-1);
    }

  fid = file_loaded (buffer);
  if (fid == -1)
    {                           /* Not correct file */
      printf ("Error reading %s\n", buffer);
      return (-1);
    }
  if (FILE_NOT_READ != fid)
    {                           /* already read */
      return (-1);
    }
  if ((fid = get_new_fid ()) == -1)
    {                           /* no more file */
      printf ("Error reading %s\n", buffer);
      return (-1);
    }

  files[fid].localf = LOCAL;
  strcpy (files[fid].name, buffer);
  printf ("Reading %s\t Fid = %d\n", buffer, fid);
  if (read_file (&files[fid]) == -1)
    {
      printf ("Error reading %s\n", buffer);
      files[fid].ref_count = -1;        /* fail */
      return (-1);
    }
  files[fid].ref_count = 1;
  return (fid);
}

static int
change_ascii_to_int (st, dp)
     char *st;
     int *dp;
{
  register int total, flag;

  total = 0;
  flag = 0;
  while (*st != '\0')
    {
      if (isdigit (*st))
        {
          total = total * 10 + (*st - '0');
        }
      else if (*st == '+')
        {
          if (flag != 0)
            {
              return (-1);
            }
          flag = 1;
        }
      else if (*st == '-')
        {
          if (flag != 0)
            {
              return (-1);
            }
          flag = -1;
        }
      else
        {
          return (-1);
        }
      st++;
    }
  if (flag == 0)
    {
      flag = 1;
    }
  *dp = total * flag;
  return (1);
}


#ifndef CHINESE
static int
expand_argument (st)
     register unsigned char *st;
{
  int num;

  if (*st == '^')
    {
      return ((*(st + 1)) & 0x1f);
    }
  else if (*st == '\\')
    {
      return (get_bcksla (st + 1));
    }
  else if (*st == '0')
    {
      if (*(st + 1) == 'x' || *(st + 1) == 'X')
        {
          sscanf ((char *) (st + 2), "%x", &num);
          return (num);
        }
      else
        {
          sscanf ((char *) st, "%o", &num);
          return (num);
        }
    }
  else if (*st == '\'')
    {
      if (*(st + 1) < 0x80)
        return (*(st + 1));
      else
        return (*(st + 1) * 0x100 + *(st + 2));
    }
  else if (!('0' <= *st && *st <= '9'))
    {
      return (-1);
    }
  else
    {
      return (atoi (st));
    }
}
#endif

static int
get_bcksla (st)
     register char *st;
{
  int num;

  switch (*st)
    {
    case 'n':
      return ('\n');
    case 't':
      return ('\t');
    case 'b':
      return ('\b');
    case 'r':
      return ('\r');
    case 'f':
      return ('\f');
    case 'x':
    case 'X':
      sscanf (st + 1, "%x", &num);
      return (num);
    case 'd':
    case 'D':
      return (atoi (*(st + 1)));
    case 'o':
    case 'O':
      sscanf (st + 1, "%o", &num);
      return (num);
    }
  return (-1);
}

/*
is_g_digit(c)
w_char c;
{
    register int i;

    if (isascii(c) && isdigit(c) || isjdigit(c)) {
        return(YES);
    } else {
        for (i = 0; i < MAX_GIJI_CHAR; i++) {
            if (giji_digit[i] == 0xffff)
                return (NO);
            else if (giji_digit[i] == c)
                return(YES);
        }
    }
    return(NO);
}
*/
