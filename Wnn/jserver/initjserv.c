/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright FreeWnn Project 1999, 2000, 2002, 2003
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

static char rcs_id[] = "$Id: initjserv.c,v 1.16 2003/05/11 18:43:15 hiroo Exp $";

#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#include <stdio.h>
#include <ctype.h>
#if STDC_HEADERS
#  include <stdlib.h>
#  include <string.h>
#else
#  if HAVE_MALLOC_H
#    include <malloc.h>
#  endif
#  if HAVE_STRINGS_H
#    include <strings.h>
#  endif /* HAVE_STRINGS_H */
#endif /* STDC_HEADERS */
#include <sys/types.h>

#include "commonhd.h"
#include "de_header.h"
#include "wnn_config.h"
#include "wnn_os.h"
#include "kaiseki.h"

struct wnn_param default_para = {
  2, 10,
/* Ｎ 小文節数 */
  2, 34, 5, 80, 5, 1, 20, 0,
/* 頻度 小文節長 幹語長 今使ったよ 辞書 小文節評価 大文節長 小文節数 */
  400, -100, 400, 80, 200, 2, 200
/* 数字 カナ 英数 記号 閉括弧 付属語 開括弧 */
};

static int expand_expr (char *s);
static int read_default_file (char *buffer, size_t buffer_size);
#ifndef CHINESE
static int expand_argument (unsigned char *st);
static int get_bcksla (char *st);
#endif /* !CHINESE */

static int
expand_expr (char *s)
 /**    @LIBDIRの展開(但し、文字列の先頭のみ)。できない時は-1が
        返り、その場合sの中身は着々とそのまんま。sの長さ＜256と仮定してる。*/
 /**    @USR (env名、logname), @LANG の展開 */
{
  char tmp[1+EXPAND_PATH_LENGTH];
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
      if (NULL != (p = strchr (++s1, '/')))
        {
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

/* daemon initialize routine */

/* Read [cjkt]server configuration file and set the parameters. */
/* RETURN VALUE: 0: success; -1: failure */
int
read_default (void)
{
  static const char sep[] = " \t";
  FILE *fp;
  char data[EXPAND_PATH_LENGTH +1];
  char code[EXPAND_PATH_LENGTH +1];
  char param[EXPAND_PATH_LENGTH +1];
  char *word;
  int i, num, *v[17];

  strcpy (jserver_dir, JSERVER_DIR);

  if ((fp = fopen (jserverrcfile, "r")) == NULL)
    {
      log_err ("can't open %s\n", jserverrcfile);
      return (-1);
    }

  /* copy the pointer for looping */
  v[0]  = &default_para.n;
  v[1]  = &default_para.nsho;
  v[2]  = &default_para.p1;
  v[3]  = &default_para.p2;
  v[4]  = &default_para.p3;
  v[5]  = &default_para.p4;
  v[6]  = &default_para.p5;
  v[7]  = &default_para.p6;
  v[8]  = &default_para.p7;
  v[9]  = &default_para.p8;
  v[10] = &default_para.p9;
  v[11] = &default_para.p10;
  v[12] = &default_para.p11;
  v[13] = &default_para.p12;
  v[14] = &default_para.p13;
  v[15] = &default_para.p14;
  v[16] = &default_para.p15;

  while (fgets (data, EXPAND_PATH_LENGTH, fp) != NULL)
    {
      num = sscanf (data, "%s", code);
      if (num <= 0)
	{
	  /* no command on the line */
	  continue;
	}
      code[EXPAND_PATH_LENGTH] = '\0';
      if (code[0] == ';')
	{
	  /* the line is commented out */
	  continue;
	}

/*
        if(strcmp(code, "jt_len") == 0){
            jt_len = atoi(s[0]);
        }else if(strcmp(code, "hjt_len") == 0){
            hjt_len = atoi(s[0]);
        }
*/

      if (strcmp (code, "max_client") == 0)
        {
          num = sscanf (data, "%s %d ", code, &max_client);
	  if (num != 2)
	    {
	      log_err ("command %s invalid.", code);
	      continue;
	    }
          log_debug ("max_client=%d", max_client);
        }
      else if (strcmp (code, "max_sticky_env") == 0)
        {
          num = sscanf (data, "%s %d ", code, &max_sticky_env);
	  if (num != 2)
	    {
	      log_err ("command %s invalid.", code);
	      continue;
	    }
          log_debug ("max_sticky_env=%d", max_sticky_env);
        }
      else if (strcmp ((code + 1), "server_dir") == 0)
        {
	  num = sscanf (data , "%s %s", code, param);
	  if (num != 2)
	    {
	      log_err ("command %s invalid.", code);
	      continue;
	    }
          if (expand_expr (param) != 0)
            {
              log_err ("command %s: can't expand %s\n", code, param);
            }
          strcpy (jserver_dir, param);
	  log_debug ("jserver_dir=%s", jserver_dir);
        }
      else if (strcmp (code, "def_param") == 0)
        {
	  word = strtok (data, sep); /* discard first word "def_param" */
	  for (i = 0; (i <= 16); i++) {
	    word = strtok (NULL, sep);
	    if (word == NULL)
	      {
		log_err ("command %s has only %d parameters.", code, i);
	        return (-1);
	      }
	    *v[i] = atoi (word); /* XXX: default to 0 if error */
	  }
	  log_debug ("command %s has %d parameters", code, i);
        }
#ifndef CHINESE
      /* else if (strcmp (code, "set_giji_eisuu") == 0 && num >= 2) */
      else if (strcmp (code, "set_giji_eisuu") == 0)
        {
	  word = strtok (data, sep); /* discard first word "set_giji_eisuu" */
	  for (i = 0; (word || i < 20); i++)
	    {
	      word = strtok (NULL, sep);
	      if (word == NULL)
		{
		  break;
		}
              giji_eisuu[i] = expand_argument (word);
	    }
	  log_debug ("command %s has %d parameters.", code, i);
          for (; i < 20; i++)
            {
              giji_eisuu[i] = 0xffff;
            }
        }
#endif
    }
  fclose (fp);
  return (0);
}

int
read_default_files (void)
{
  FILE *fp;
  char data[EXPAND_PATH_LENGTH+1];
  int num;
  char code[EXPAND_PATH_LENGTH+1];
  char file[EXPAND_PATH_LENGTH+1];

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
          read_default_file (file, EXPAND_PATH_LENGTH+1);
        }
    }
  fclose (fp);

  printf ("Finished Reading Files\n");

  return (0);
}

static int
read_default_file (char* buffer, size_t buffer_size)
{
  int fid;

  buffer = expand_file_name (buffer, buffer_size);
  if (!buffer)
    {
      log_err ("read_default_file: filename too long. %s", buffer);
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

#ifndef CHINESE
static int
expand_argument (unsigned char *st)
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

static int
get_bcksla (char *st)
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
      return (atoi (st + 1));
    case 'o':
    case 'O':
      sscanf (st + 1, "%o", &num);
      return (num);
    }
  return (-1);
}
#endif /* !CHINESE */

