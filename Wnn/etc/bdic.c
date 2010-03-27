/*
 *  $Id: bdic.c,v 1.13 2005/06/12 17:14:22 aonoto Exp $
 */

/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright FreeWnn Project 1999, 2000, 2002, 2003, 2004, 2005
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

/*
  Binary (Updatable, Stable) dictionary common routine.
*/
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#if defined (JS)
# define JS_STATIC	static
#else /* !JS */
# define JS_STATIC
#endif /* !JS */

#if defined(JSERVER) || defined(JS)
# ifdef WRITE_CHECK
#  define BDIC_WRITE_CHECK
# endif
#endif /* defined(JSERVER) || defined(JS) */

#include <sys/types.h>
#include <stdio.h>
#if STDC_HEADERS
#  include <string.h>
#elif HAVE_STRINGS_H
#  include <strings.h>
#endif /* STDC_HEADERS */

#ifndef JS
# include <sys/stat.h>
# if HAVE_UNISTD_H
#  include <unistd.h>
# endif
# include "commonhd.h"
# include "jslib.h"
# include "jh.h"
#endif /* JS */
#include "jdata.h"
#include "wnn_os.h"
#include "wnn_string.h"

#ifdef JSERVER
# ifndef BDIC_WRITE_CHECK
#  define vputc(X, pt) ((pt)? putc((X), (pt)):xputc_cur(X))
# endif /* !BDIC_WRITE_CHECK */
# define vgetc(pt) ((pt)? getc((pt)):xgetc_cur())
extern int xgetc_cur ();
#else /* !JSERVER */
# ifdef JS
#  ifndef BDIC_WRITE_CHECK
#   define vputc(X, pt) putc((X), (pt))
#  endif /* !BDIC_WRITE_CHECK */
#  define vgetc(pt) getc(pt)
# else /* !JS */
#  define vputc(X, pt) putc((X), (pt))
#  define vgetc(pt) getc(pt)
# endif /* !JS */
#endif /* !JSERVER */

#ifndef min
# define min(a, b) ((a > b)? b:a)
# define max(a, b) ((a < b)? b:a)
#endif

/* XXX: function prototypes to be moved to some header file. */
/* bdic.c */
/* following functions are only used in bdic.c
 * put_null, put_nstring, put_int
 * get_null, get_nstring
 */
JS_STATIC int put_null (FILE* ofpter, int n);
JS_STATIC int put_nstring (FILE* ofpter, char* c, int n);
JS_STATIC int put_n_EU_str (FILE* ofpter, w_char* c, int n);
JS_STATIC int put_short (FILE* ofpter, int i);
JS_STATIC int put_int (FILE* ofpter, int i);
JS_STATIC int get_null (FILE* ifpter, int n);
JS_STATIC int get_nstring (FILE* ifpter, int n, char* st);
JS_STATIC int get_int (int* ip, FILE* ifpter);

#ifndef JS
void udytoS (w_char* yomi, int tnum, char* hostart, struct uind1 * tary);
#endif /* !JS */

#ifdef BDIC_WRITE_CHECK
JS_STATIC void check_backup (char* n);
JS_STATIC void delete_tmp_file (char* n);
JS_STATIC char* make_backup_file (char* n);
JS_STATIC char* make_tmp_file (char* n, int copy, FILE** ret_fp);
JS_STATIC void move_tmp_to_org (char* tmp_name, char* org_name, int copy);
#endif /* BDIC_WRITE_CHECK */

JS_STATIC int create_file_header (FILE* ofpter, int file_type, char* file_passwd);
JS_STATIC int output_file_header (FILE* ofpter, struct wnn_file_head* hp);
JS_STATIC int input_file_header (FILE* ifpter,  struct wnn_file_head* hp);
JS_STATIC int output_file_uniq (struct wnn_file_uniq* funiq, FILE* ofpter);
JS_STATIC int input_file_uniq (struct wnn_file_uniq* funiq, FILE* ifpter);
JS_STATIC int check_inode (FILE* f, struct wnn_file_head* fh);
JS_STATIC int change_file_uniq (struct wnn_file_head* fh, char* n);
JS_STATIC int change_file_uniq1 (
	FILE* ofpter, int file_type, char* file_passwd,
	struct wnn_file_uniq* file_uniq);

#ifdef JSERVER
int f_uniq_cmp (struct wnn_file_uniq *a, struct wnn_file_uniq* b);
#endif /* JSERVER */

#if     !defined(JS) && !defined(JSERVER)
void vputs (char* c, FILE* fp);
int  vputws (w_char* w, FILE* fp);
int  put_yomi_str (w_char* yomi, FILE* ofpter);
#endif /* !JS && !JSERVER */

#ifndef JS
void Get_knj2 (UCHAR* kptr, int kanji2, w_char* kouho, w_char* yomi, w_char* comment);
void get_kanji_str (UCHAR* kptr, w_char* kanji, w_char* yomi, w_char* comment);
void Get_kanji_str_r (UCHAR* kptr, w_char** tmpk, w_char** tmpy, w_char** tmpc);
UCHAR kanjiaddr (UCHAR* d0, w_char* kanji, w_char* yomi, w_char* comment);
#endif /* !JS */

JS_STATIC int create_null_dic (
	char* fn, w_char* comm, char* passwd, char* hpasswd, int which);
JS_STATIC int create_hindo_file (
	struct wnn_file_uniq* funiq,
	char* fn, w_char* comm, char* passwd, int serial);

/* XXX: input_* are not used in JSlib */
JS_STATIC int input_header_jt (FILE* ifpter, struct JT* jt1);
JS_STATIC int output_header_jt (FILE* ofpter, struct JT* jt1);
JS_STATIC int input_header_hjt (FILE* ifpter, struct HJT* hjt1);
JS_STATIC int output_header_hjt (FILE* ofpter, struct HJT* hjt1);

#if     !defined(JS) && !defined(JSERVER)
/* Only used in JUTIL */
void Print_entry (
	w_char* yomi, w_char* kstr, w_char* cstr,
	int hindo, int ima, int hinsi, int serial,
	FILE* ofpter, int esc_exp);
#endif /* !JS && !JSERVER */

/* XXX: prototype of functions in other files.
 *      should be replaced to including some header files. */
JS_STATIC void new_pwd (char*, char*);	/* in etc/pwd.c */
char *wnn_get_hinsi_name (int);		/* in etc/gethinsi.c */
#ifdef JSERVER
void xputc_cur (unsigned char);		/* in jserver/snd_rcv.c */
#endif /* JSERVER */


#ifdef BDIC_WRITE_CHECK

static int
vputc (char c, FILE* fp)
{
# ifdef JSERVER
  if (fp)
    {
      if (fwrite (&c, sizeof (char), 1, fp) <= 0)
        return (-1);
    }
  else
    {
      xputc_cur (c);
    }
# else /* !JSERVER */
  if (fwrite (&c, sizeof (char), 1, fp) <= 0)
    return (-1);
# endif /* !JSERVER */

  return (0);
}
#endif /* BDIC_WRITE_CHECK */

/* XXX					note 2004.07.08  Hiroo Ono
 * It used to return int only if BDIC_WRITE_CHECK was defined,
 * otherwise it was a void function.
 * For the sake of source code simplicity, I unified the funtion to always
 * return int: It returns the write status when BDIC_WRITE_CHECK is defined
 * and just 0 when not. No testing on performace was performed (though I
 * believe it does not have significant effect).
 * Similar changes are also applied to following functions.
 */
JS_STATIC int
put_null (FILE* ofpter, int n)
{
  for (; n; n--)
    {
#ifdef BDIC_WRITE_CHECK
      if (vputc ('\0', ofpter) == -1)
        return (-1);
#else /* !BDIC_WRITE_CHECK */
      vputc ('\0', ofpter);
#endif /* !BDIC_WRITE_CHECK */
    }

  return (0);
}

/* XXX was void when BDIC_WRITE_CHECK was not defined. cf put_null() above. */
JS_STATIC int
put_nstring (FILE* ofpter, char* c, int n)
{
  for (; n; n--)
    {
#ifdef BDIC_WRITE_CHECK
      if (vputc (*c++, ofpter) == -1)
        return (-1);
#else /* !BDIC_WRITE_CHECK */
      vputc (*c++, ofpter);
#endif /* !BDIC_WRITE_CHECK */
    }

  return (0);
}

/* XXX was void when BDIC_WRITE_CHECK was not defined. cf put_null() above. */
JS_STATIC int
put_n_EU_str (FILE* ofpter, w_char* c, int n)
{
  for (; n; n--)
    {
#ifdef BDIC_WRITE_CHECK
      if (put_short (ofpter, (int) (*c++)) == -1)
        return (-1);
#else  /* !BDIC_WRITE_CHECK */
      put_short (ofpter, (int) (*c++));
#endif /* !BDIC_WRITE_CHECK */
    }

  return (0);
}

/* XXX was void when BDIC_WRITE_CHECK was not defined. cf put_null() above. */
JS_STATIC int
put_short (FILE* ofpter, int i)
{
#ifdef BDIC_WRITE_CHECK
  if ((vputc (i >> 8, ofpter) == -1) || (vputc (i, ofpter) == -1))
    return (-1);
#else /* BDIC_WRITE_CHECK */
  vputc (i >> 8, ofpter);
  vputc (i, ofpter);
#endif /* BDIC_WRITE_CHECK */

  return (0);
}

/* XXX was void when BDIC_WRITE_CHECK was not defined. cf put_null() above. */
JS_STATIC int
put_int (FILE* ofpter, int i)
{
#ifdef BDIC_WRITE_CHECK
  if ((vputc (i >> 24, ofpter) == -1)
      || (vputc (i >> 16, ofpter) == -1)
      || (vputc (i >> 8, ofpter) == -1)
      || (vputc (i, ofpter) == -1))
    return (-1);
#else /* BDIC_WRITE_CHECK */
  vputc (i >> 24, ofpter);
  vputc (i >> 16, ofpter);
  vputc (i >> 8, ofpter);
  vputc (i, ofpter);
#endif /* BDIC_WRITE_CHECK */

  return (0);
}

JS_STATIC int
get_null (FILE* ifpter, int n)
{
  int k;

  for (; n; n--)
    {
      k = vgetc (ifpter);
      if (k == -1)
        return (-1);
    }

  return (0);
}

JS_STATIC int
get_nstring (FILE* ifpter, int n, char* st)
{
  int k;

  for (; n; n--)
    {
      k = vgetc (ifpter);
      *st++ = k;
      if (k == -1)
        return (-1);
    }
  return (0);
}


/* 
 * Note for get_n_EU_str() / get_short() :
 * Moved to dtoa.c for now (currently we don't use them in other files) ...
 */

JS_STATIC int
get_int (int* ip, FILE* ifpter)
{
  int i = 0;
  int k;

  i |= (k = vgetc (ifpter)) << 24;
  if (k == -1)
    return (-1);
  i |= (k = vgetc (ifpter)) << 16;
  if (k == -1)
    return (-1);
  i |= (k = vgetc (ifpter)) << 8;
  if (k == -1)
    return (-1);
  i |= (k = vgetc (ifpter));
  if (k == -1)
    return (-1);
  *ip = i;
  return (0);
}

#ifndef JS
void
udytoS (w_char* yomi, int tnum, char* hostart, struct uind1 * tary)
{
  struct uind2 *hop;
  int len;

  hop = (struct uind2 *) ((char *) hostart + tary[tnum].pter);
  yomi[0] = tary[tnum].yomi1 >> 16;
  yomi[1] = tary[tnum].yomi1 & 0xffff;
  if (yomi[1])
    {
      yomi[2] = tary[tnum].yomi2 >> 16;
      if (yomi[2])
        {
          yomi[3] = tary[tnum].yomi2 & 0xffff;
        }
    }
  len = hop->yomi[0];
  /*Strncpy(yomi + 4, (hop->yomi) + 1, len - 4);
     In order not to use ../etc/strings.c */
  bcopy ((char *) ((hop->yomi) + 1), (char *) (yomi + 4), max (0, ((len - 4)) * 2));
  yomi[len] = 0;
}
#endif /* !JS */

#ifdef BDIC_WRITE_CHECK
static char *
make_tmp_name (char* n)
{
  static char buf[256];

  if (n == NULL || *n == '\0')
    return NULL;
  sprintf (buf, "%s~", n);
  return (buf);
}

static char *
make_backup_name (char* n)
{
  static char buf[256];

  if (n == NULL || *n == '\0')
    return NULL;
  sprintf (buf, "%s#", n);
  return (buf);
}


JS_STATIC void
check_backup (char* n)
{
  char *p;

  if ((p = make_tmp_name (n)) && (access (p, F_OK) != -1))
    unlink (p);
  if ((p = make_backup_name (n)) && (access (p, F_OK) != -1))
    {
      unlink (n);
      link (p, n);
      unlink (p);
    }
}

static int
copy_file_to_file (FILE* from, FILE* to)
{
  char buf[1024];
  int r_len, w_len, i;

  for (;;)
    {
      r_len = fread (buf, 1, 1024, from);
      if (r_len <= 0)
        break;
      for (i = 0; i < r_len; i += w_len)
        {
          w_len = fwrite (&buf[i], 1, r_len - i, to);
          if (w_len <= 0)
            {
              fseek (from, 0, 0);
              fseek (to, 0, 0);
              return (-1);
            }
          if (w_len >= r_len)
            break;
        }
    }
  fseek (from, 0, 0);
  fseek (to, 0, 0);
  return (0);
}

JS_STATIC void
delete_tmp_file (char* n)
{
  if (n && *n)
    unlink (n);
}

JS_STATIC char*
make_backup_file (char* n)
{
  FILE *fp, *fp2;
  char *p, *p2;

  if ((p = make_backup_name (n)) == NULL)
    return (NULL);
  if ((p2 = make_tmp_name (n)) == NULL)
    return (NULL);
  if (((fp = fopen (p2, "w+")) == NULL) || ((fp2 = fopen (n, "r")) == NULL))
    {
      return NULL;
    }
  if (copy_file_to_file (fp2, fp) == -1)
    {
      fclose (fp);
      fclose (fp2);
      return (NULL);
    }
  fclose (fp);
  fclose (fp2);
  if (access (p, F_OK) != -1)
    unlink (p);
  link (p2, p);
  unlink (p2);
  return (p);
}

JS_STATIC char*
make_tmp_file (char* n, int copy, FILE** ret_fp)
{
  FILE *fp, *fp2;
  struct wnn_file_head fh;
  char *p;

  p = make_tmp_name (n);
  if ((fp = fopen (p, "w+")) == NULL)
    {
      *ret_fp = NULL;
      return NULL;
    }
  if (copy && (fp2 = fopen (n, "r")) != NULL)
    {
      input_file_header (fp2, &fh);
      fseek (fp2, 0, 0);
      if ((copy_file_to_file (fp2, fp) == -1) || (create_file_header (fp, fh.file_type, fh.file_passwd) == -1))
        {
          fclose (fp);
          fclose (fp2);
          *ret_fp = NULL;
          return NULL;
        }
      fseek (fp, 0, 0);
      fclose (fp2);
    }
  *ret_fp = fp;
  return p;
}

JS_STATIC void
move_tmp_to_org (char* tmp_name, char* org_name, int copy)
{
  FILE *org, *tmp;

  if (copy)
    {
      if (((tmp = fopen (tmp_name, "r")) == NULL) || ((org = fopen (org_name, "w+")) == NULL))
        {
          return;
        }
      copy_file_to_file (tmp, org);
      fclose (tmp);
      fclose (org);
    }
  else
    {
      unlink (org_name);
      link (tmp_name, org_name);
    }
  unlink (tmp_name);
}
#endif /* BDIC_WRITE_CHECK */

JS_STATIC int
create_file_header (FILE* ofpter, int file_type, char* file_passwd)
{
  struct stat buf;
  char hostname[WNN_HOSTLEN];
  struct wnn_file_head fh;

  if (fstat (fileno (ofpter), &buf) == -1)
    {
      return (-1);
    }
  gethostname (hostname, WNN_HOSTLEN);
  hostname[WNN_HOSTLEN - 1] = '\0';

  fh.file_uniq.time = (int) buf.st_ctime;
  fh.file_uniq.dev = (int) buf.st_dev;
  fh.file_uniq.inode = (int) buf.st_ino;
  bzero (fh.file_uniq.createhost, WNN_HOSTLEN);
  strcpy (fh.file_uniq.createhost, hostname);

  /* file_uniq_org */
  fh.file_uniq_org.time = (int) buf.st_ctime;
  fh.file_uniq_org.dev = (int) buf.st_dev;
  fh.file_uniq_org.inode = (int) buf.st_ino;
  bzero (fh.file_uniq_org.createhost, WNN_HOSTLEN);
  strcpy (fh.file_uniq_org.createhost, hostname);

  fh.file_type = file_type;
  if (file_passwd)
    {
      strncpy (fh.file_passwd, file_passwd, WNN_PASSWD_LEN);
    }
  else
    {
      bzero (fh.file_passwd, WNN_PASSWD_LEN);
    }
  if (output_file_header (ofpter, &fh) == -1)
    return (-1);
  return (0);
}

/* 128 Bytes File Header */

JS_STATIC int
output_file_header (FILE* ofpter, struct wnn_file_head* hp)
{
#ifdef BDIC_WRITE_CHECK
  if ((put_nstring (ofpter, WNN_FILE_STRING, WNN_FILE_STRING_LEN) == -1)
      || (put_int (ofpter, hp->file_type) == -1)
      || (output_file_uniq (&hp->file_uniq, ofpter) == -1)
      || (output_file_uniq (&hp->file_uniq_org, ofpter) == -1)
      || (put_nstring (ofpter, hp->file_passwd, WNN_PASSWD_LEN) == -1)
      || (put_null (ofpter, 36) == -1))
    return (-1);                /* Future Use */
#else /* BDIC_WRITE_CHECK */
  put_nstring (ofpter, WNN_FILE_STRING, WNN_FILE_STRING_LEN);
  put_int (ofpter, hp->file_type);
  output_file_uniq (&hp->file_uniq, ofpter);
  output_file_uniq (&hp->file_uniq_org, ofpter);
  put_nstring (ofpter, hp->file_passwd, WNN_PASSWD_LEN);
  put_null (ofpter, 36);         /* Future Use */
#endif /* BDIC_WRITE_CHECK */

  return (0);
}

JS_STATIC int
input_file_header (FILE* ifpter,  struct wnn_file_head* hp)
{
  char wnn_file_string[WNN_FILE_STRING_LEN + 1];
  int err = 0;

  get_nstring (ifpter, WNN_FILE_STRING_LEN, wnn_file_string);
  if (strncmp (wnn_file_string, WNN_FILE_STRING, WNN_FILE_STRING_LEN))
    err = -1;
  if (get_int ((&hp->file_type), ifpter) == -1)
    err = -1;
  if (input_file_uniq (&(hp->file_uniq), ifpter) == -1)
    err = -1;
  if (input_file_uniq (&(hp->file_uniq_org), ifpter) == -1)
    err = -1;
  get_nstring (ifpter, WNN_PASSWD_LEN, hp->file_passwd);
  get_null (ifpter, 36);

  return (err);
}

/* XXX was void when BDIC_WRITE_CHECK was not defined. cf put_null() above. */
JS_STATIC int
output_file_uniq (struct wnn_file_uniq* funiq, FILE* ofpter)
{
#ifdef BDIC_WRITE_CHECK
  if ((put_int (ofpter, funiq->time) == -1)
      || (put_int (ofpter, funiq->dev) == -1)
      || (put_int (ofpter, funiq->inode) == -1)
      || (put_nstring (ofpter, funiq->createhost, strlen (funiq->createhost)) == -1)
      || (put_null (ofpter, WNN_HOSTLEN - strlen (funiq->createhost)) == -1))
    return (-1);
#else /* BDIC_WRITE_CHECK */
  put_int (ofpter, funiq->time);
  put_int (ofpter, funiq->dev);
  put_int (ofpter, funiq->inode);
  put_nstring (ofpter, funiq->createhost, strlen (funiq->createhost));
  put_null (ofpter, WNN_HOSTLEN - strlen (funiq->createhost));
#endif /* BDIC_WRITE_CHECK */

  return (0);
}

JS_STATIC int
input_file_uniq (struct wnn_file_uniq* funiq, FILE* ifpter)
{
  if (get_int (&(funiq->time), ifpter) == -1
      || get_int (&(funiq->dev), ifpter) == -1
      || get_int (&(funiq->inode), ifpter) == -1
      || get_nstring (ifpter, WNN_HOSTLEN, funiq->createhost) == -1)
    return (-1);

  return (0);
}

JS_STATIC int
check_inode (FILE* f, struct wnn_file_head* fh)
{
  struct stat buf;
  if (fstat (fileno (f), &buf) == -1)
    {
      return (-1);
    }
  if ((int) buf.st_ino != fh->file_uniq.inode)
    {
      return (-1);
    }
  return (0);
}

JS_STATIC int
change_file_uniq (struct wnn_file_head* fh, char* n)
{
  int x;
  FILE *fp;
#ifdef BDIC_WRITE_CHECK
  char *tmp, *backup = NULL;
  backup = make_backup_file (n);
  if ((tmp = make_tmp_file (n, 1, &fp)) == NULL)
    {
      delete_tmp_file (backup);
#else /* BDIC_WRITE_CHECK */
  if ((fp = fopen (n, "r+")) == NULL)
    {
#endif /* BDIC_WRITE_CHECK */
      return (-1);
    }
#ifdef BDIC_WRITE_CHECK
  if (change_file_uniq1 (fp, fh->file_type, fh->file_passwd, &(fh->file_uniq)) == -1)
    {
      fclose (fp);
      delete_tmp_file (tmp);
      delete_tmp_file (backup);
      return (-1);
    }
#else /* BDIC_WRITE_CHECK */
  change_file_uniq1 (fp, fh->file_type, fh->file_passwd, &(fh->file_uniq));
#endif /* BDIC_WRITE_CHECK */
  fseek (fp, 0, 0);
  x = input_file_header (fp, fh);       /* It must not occur. */
  fclose (fp);

  if (x == -1)
    {
#ifdef BDIC_WRITE_CHECK
      delete_tmp_file (tmp);
      delete_tmp_file (backup);
#endif /* BDIC_WRITE_CHECK */
      return (-1);
    }

#ifdef BDIC_WRITE_CHECK
  move_tmp_to_org (tmp, n, 0);
  delete_tmp_file (backup);
#endif /* BDIC_WRITE_CHECK */

  return (0);
}

JS_STATIC int
change_file_uniq1 (
	FILE* ofpter,
	int file_type,
	char* file_passwd,
	struct wnn_file_uniq* file_uniq)
{
  struct stat buf;
  char hostname[WNN_HOSTLEN];
  struct wnn_file_head fh;

  if (fstat (fileno (ofpter), &buf) == -1)
    {
      return (-1);
    }
  gethostname (hostname, WNN_HOSTLEN);
  hostname[WNN_HOSTLEN - 1] = '\0';

  fh.file_uniq.time = (int) buf.st_ctime;
  fh.file_uniq.dev = (int) buf.st_dev;
  fh.file_uniq.inode = (int) buf.st_ino;
  bzero (fh.file_uniq.createhost, WNN_HOSTLEN);
  strcpy (fh.file_uniq.createhost, hostname);

  /* file_uniq_org */
  fh.file_uniq_org.time = file_uniq->time;
  fh.file_uniq_org.dev = file_uniq->dev;
  fh.file_uniq_org.inode = file_uniq->inode;
  bzero (fh.file_uniq_org.createhost, WNN_HOSTLEN);
  strcpy (fh.file_uniq_org.createhost, file_uniq->createhost);

  fh.file_type = file_type;
  if (file_passwd)
    {
      strncpy (fh.file_passwd, file_passwd, WNN_PASSWD_LEN);
    }
  else
    {
      bzero (fh.file_passwd, WNN_PASSWD_LEN);
    }
  if (output_file_header (ofpter, &fh) == -1)
    return (-1);
  return (0);
}



#ifdef JSERVER
int
f_uniq_cmp (struct wnn_file_uniq *a, struct wnn_file_uniq* b)
{
  return bcmp (a, b, sizeof (struct wnn_file_uniq));
}
#endif

#if     !defined(JS) && !defined(JSERVER)
void
vputs (char* c, FILE* fp)
{
  while (*c)
    {
      vputc (*c++, fp);
    }
}

int
vputws (w_char* w, FILE* fp)
{
  int n;
  UCHAR tmp[LENGTHYOMI * 3];

  n = wnn_sStrcpy (tmp, w);
  vputs (tmp, fp);
  return (n);
}
#endif /* !JS && !JSERVER */


#if     !defined(JS) && !defined(JSERVER)
int
put_yomi_str (w_char* yomi, FILE* ofpter)
{
  int c;
  int i = 0;
  UCHAR tmp[LENGTHYOMI * 3], *p;

  wnn_sStrcpy (tmp, yomi);
  for (p = tmp; *p; p++)
    {
      c = (int) *p;
      if (c == '\\')
        {
          vputc ('\\', ofpter);
          vputc ('\\', ofpter);
          i += 2;
        }
      else if (c > 0x20)
        {
          vputc (c, ofpter);
          i += 1;
        }
      else
        {
          char tmp[8];
          sprintf (tmp, "\\0%o\\", c);
          vputs (tmp, ofpter);
          i += strlen (tmp);
        }
    }
  return (i);
}
#endif /* !JS && !JSERVER */

#ifndef JS
void
Get_knj2 (UCHAR* kptr, int kanji2, w_char* kouho, w_char* yomi, w_char* comment)
{
  int tcnt;
  for (tcnt = 0; tcnt < kanji2; tcnt++)
    {
      kptr += *kptr;
    }
  get_kanji_str (kptr, kouho, yomi, comment);

  return;
}

void
get_kanji_str (UCHAR* kptr, w_char* kanji, w_char* yomi, w_char* comment)
{
  w_char *tmpy;
  w_char *tmpk;
  w_char *tmpc;
  extern void Get_kanji_str_r ();

  Get_kanji_str_r (kptr, &tmpk, &tmpy, &tmpc);

# ifdef CONVERT_from_TOP	/* Don't worry. Only used in server and jutil */
  if (kanji && tmpk)
    wnn_Strcpy (kanji, tmpk);
  if (yomi && tmpy)
    wnn_Strcpy (yomi, tmpy);
# else /* !CONVERT_from_TOP */
  if (kanji && tmpk)
    wnn_Sreverse (kanji, tmpk);
  if (yomi && tmpy)
    wnn_Sreverse (yomi, tmpy);
# endif /* !CONVERT_from_TOP */
  if (comment && tmpc)
    wnn_Strcpy (comment, tmpc);
}


void
Get_kanji_str_r (UCHAR* kptr, w_char** tmpk, w_char** tmpy, w_char** tmpc)
{
  int state = *(kptr + 1);
  static w_char dmy = 0;        /* Must not be allocated on Stack!! */
  int adres;

  if (state & FORWARDED)
    {
      /* Forward occures when comment-set is used */
      adres = (*(w_char *) (kptr + 2)) << 16 | (*(w_char *) (kptr + 4));
      Get_kanji_str_r (kptr + adres, tmpk, tmpy, tmpc);
      return;
    }
/*    get_kanji_str_r(kptr, tmpk, tmpy, comment); */
  if (state & HAS_YOMI)
    {
      *tmpy = ((w_char *) (kptr + 2));
      *tmpk = *tmpy + wnn_Strlen (*tmpy) + 1;
    }
  else
    {
      *tmpy = &dmy;
      *tmpk = ((w_char *) (kptr + 2));
    }
  if (state & HAS_COMMENT)
    {
      *tmpc = *tmpk + wnn_Strlen (*tmpk) + 1;
    }
  else
    {
      *tmpc = &dmy;
    }
}

# ifdef nodef
int
Get_kanji_len (UCHAR* kptr, int which)
{
  w_char tmp[LENGTHYOMI];

  if (which == D_KANJI)
    {
      get_kanji_str_r (kptr, tmp, NULL, NULL);
    }
  else
    {
      get_kanji_str_r (kptr, NULL, tmp, NULL);
    }
  return (wnn_Strlen (tmp));
}

void
get_kanji_str_r (UCHAR* kptr, w_char* kanji, w_char* yomi, w_char* comment)
{
  w_char *c;
  int n, k;

  n = *kptr++ - 1;
  c = kanji;
  if (comment)
    *comment = 0;
  if (yomi)
    *yomi = 0;
  for (k = 0; k < n;)
    {
      if (*kptr == DIC_COMMENT_CHAR)
        {
          if (c)
            *c = 0;
          c = comment;
          kptr++;
          k++;
        }
      else if (*kptr == DIC_YOMI_CHAR)
        {
          if (c)
            *c = 0;
          c = yomi;
          kptr++;
          k++;
        }
      else if (*kptr & 0x80)
        {                       /* kanji char */
          if (c)
            *c++ = ((int) *kptr << 8 | *(kptr + 1));
          kptr += 2;
          k += 2;
        }
      else
        {
          if (c)
            *c++ = *kptr;
          kptr++;
          k++;
        }
    }
  if (c)
    *c = 0;
}
# endif /* nodef */

UCHAR
kanjiaddr (UCHAR* d0, w_char* kanji, w_char* yomi, w_char* comment)
{
  w_char *dest = (w_char *) (d0 + 2);
  int state = 0;
  w_char *pt;

  if (yomi && *yomi)
    {
# ifdef CONVERT_from_TOP	/* Don't worry. Only used in server and jutil */
      pt = yomi;
      for (; *pt;)
        {
          *dest++ = *pt++;
        }
# else /* !CONVERT_from_TOP */
      pt = yomi + wnn_Strlen (yomi) - 1;
      for (; pt >= yomi;)
        {
          *dest++ = *pt--;
        }
# endif /* !CONVERT_from_TOP */
      state |= HAS_YOMI;
      *dest++ = 0;
    }

# ifdef CONVERT_from_TOP	/* Don't worry. Only used in server and jutil */
  pt = kanji;
  for (; *pt;)
    {
      *dest++ = *pt++;
    }
# else /* !CONVERT_from_TOP */
  pt = kanji + wnn_Strlen (kanji) - 1;
  for (; pt >= kanji;)
    {
      *dest++ = *pt--;
    }
# endif /* !CONVERT_from_TOP */
  *dest++ = 0;

  if (comment && *comment)
    {
      pt = comment;
      for (; *pt;)
        {
          *dest++ = *pt++;
        }
      state |= HAS_COMMENT;
      *dest++ = 0;
    }
  *d0 = (UCHAR) ((UCHAR *) dest - d0);
  *(d0 + 1) = state;
  return (*d0);
}

#endif /* JS */


JS_STATIC int
create_null_dic (
	char* fn,
	w_char* comm,
	char* passwd,
	char* hpasswd,	/* not encoded */
	int which)
{
  FILE *fp;
  struct JT jt;
  char epasswd[WNN_PASSWD_LEN];

  jt.total = 0;
  jt.gosuu = 0;
  if (hpasswd)
    {
      new_pwd (hpasswd, jt.hpasswd);
    }
  else
    {
      bzero (jt.hpasswd, WNN_PASSWD_LEN);
    }
  jt.syurui = which;
  jt.maxserial = 0;
  jt.maxtable = 0;
  jt.maxhontai = (which == WNN_UD_DICT) ? 4 : 0;
  jt.maxkanji = 0;
  if (comm)
    {
      jt.maxcomment = wnn_Strlen (comm);
    }
  else
    {
      jt.maxcomment = 0;
    }
  jt.maxhinsi_list = 0;
  jt.maxri1[D_YOMI] = 0;
  jt.maxri1[D_KANJI] = 0;
  jt.maxri2 = 0;

  if ((fp = fopen (fn, "w")) == NULL)
    {
      /*      error1( "Jserver:create_null_ud:No file %s.\n", fn); */
      return (-1);
    }
  if (passwd)
    {
      new_pwd (passwd, epasswd);
    }
  else
    {
      bzero (epasswd, WNN_PASSWD_LEN);
    }
  if (create_file_header (fp, WNN_FT_DICT_FILE, epasswd) == -1
      || output_header_jt (fp, &jt) == -1)
    {
      fclose (fp);
      return (-1);
    }
#ifdef BDIC_WRITE_CHECK
  if (put_n_EU_str (fp, comm, jt.maxcomment) == -1)
    {
      fclose (fp);
      return (-1);
    }
#else /* BDIC_WRITE_CHECK */
  put_n_EU_str (fp, comm, jt.maxcomment);
#endif /* BDIC_WRITE_CHECK */
  if (which == WNN_UD_DICT)
    {
#ifdef BDIC_WRITE_CHECK
      if (put_int (fp, 0) == -1)
        {                       /* hontai[0] */
          fclose (fp);
          return (-1);
        }
#else /* BDIC_WRITE_CHECK */
      put_int (fp, 0);           /* hontai[0] */
#endif /* BDIC_WRITE_CHECK */
    }

#if HAVE_FCHMOD
  fchmod (fileno (fp), 0664);
  fclose (fp);
#else /* !HAVE_FCHMOD */
  fclose (fp);
  chmod (fn, 0664);
#endif /* !HAVE_FCHMOD */
  return (0);
}


JS_STATIC int
create_hindo_file (
	struct wnn_file_uniq* funiq,
	char* fn,
	w_char* comm,
	char* passwd,	/* Not encoded */
	int serial)
{
  FILE *fp;
  struct HJT hjt;
  char epasswd[WNN_PASSWD_LEN];
  w_char tmp[1];

  tmp[0] = 0;
  if (comm == NULL)
    comm = tmp;
  bcopy (funiq, &(hjt.dic_file_uniq), WNN_F_UNIQ_LEN);
  hjt.maxcomment = wnn_Strlen (comm);

  hjt.maxserial = serial;

  if ((fp = fopen (fn, "w")) == NULL)
    {
      return (-1);
    }
  if (passwd)
    {
      new_pwd (passwd, epasswd);
    }
  else
    {
      bzero (epasswd, WNN_PASSWD_LEN);
    }
  if (create_file_header (fp, WNN_FT_HINDO_FILE, epasswd) == -1)
    {
      fclose (fp);
      return (-1);
    }
#ifdef BDIC_WRITE_CHECK
  if ((output_header_hjt (fp, &hjt) == -1)
      || (put_n_EU_str (fp, comm, hjt.maxcomment) == -1)
      || (put_null (fp, serial) == -1))
    {
      fclose (fp);
      return (-1);
    }
#else /* BDIC_WRITE_CHECK */
  output_header_hjt (fp, &hjt);
  put_n_EU_str (fp, comm, hjt.maxcomment);
  put_null (fp, serial);
#endif /* BDIC_WRITE_CHECK */

#if HAVE_FCHMOD
  fchmod (fileno (fp), 0664);
  fclose (fp);
#else /* !HAVE_FCHMOD */
  fclose (fp);
  chmod (fn, 0664);
#endif /* !HAVE_FCHMOD */
  return (0);
}

/* Header Total 128 Bytes */

JS_STATIC int
input_header_jt (FILE* ifpter, struct JT* jt1)
{
  if (get_int (&jt1->syurui, ifpter) == -1
      || get_int (&jt1->maxcomment, ifpter) == -1
      || get_int (&jt1->maxhinsi_list, ifpter) == -1
      || get_int (&jt1->maxserial, ifpter) == -1
      || get_int (&jt1->maxkanji, ifpter) == -1
      || get_int (&jt1->maxtable, ifpter) == -1
      || get_int (&jt1->maxhontai, ifpter) == -1
      || get_int (&jt1->gosuu, ifpter) == -1
      || get_nstring (ifpter, WNN_PASSWD_LEN, jt1->hpasswd) == -1
      || get_int (&jt1->total, ifpter) == -1
      || get_int (&jt1->maxri1[D_YOMI], ifpter) == -1
      || get_int (&jt1->maxri1[D_KANJI], ifpter) == -1
      || get_int (&jt1->maxri2, ifpter) == -1
      || get_null (ifpter, 56) == -1)
    return (-1);

  return (0);
}

JS_STATIC int
output_header_jt (FILE* ofpter, struct JT* jt1)
{
#ifdef BDIC_WRITE_CHECK
  if ((put_int (ofpter, jt1->syurui) == -1)
      || (put_int (ofpter, jt1->maxcomment) == -1)
      || (put_int (ofpter, jt1->maxhinsi_list) == -1)
      || (put_int (ofpter, jt1->maxserial) == -1)
      || (put_int (ofpter, jt1->maxkanji) == -1)
      || (put_int (ofpter, jt1->maxtable) == -1)
      || (put_int (ofpter, jt1->maxhontai) == -1)
      || (put_int (ofpter, jt1->gosuu) == -1)
      || (put_nstring (ofpter, jt1->hpasswd, WNN_PASSWD_LEN) == -1)
      || (put_int (ofpter, jt1->total) == -1)
      || (put_int (ofpter, jt1->maxri1[D_YOMI]) == -1)
      || (put_int (ofpter, jt1->maxri1[D_KANJI]) == -1)
      || (put_int (ofpter, jt1->maxri2) == -1)
      || (put_null (ofpter, 56) == -1))
    return (-1);
#else /* BDIC_WRITE_CHECK */
  put_int (ofpter, jt1->syurui);
  put_int (ofpter, jt1->maxcomment);
  put_int (ofpter, jt1->maxhinsi_list);
  put_int (ofpter, jt1->maxserial);
  put_int (ofpter, jt1->maxkanji);
  put_int (ofpter, jt1->maxtable);
  put_int (ofpter, jt1->maxhontai);
  put_int (ofpter, jt1->gosuu);
  put_nstring (ofpter, jt1->hpasswd, WNN_PASSWD_LEN);
  put_int (ofpter, jt1->total);
  put_int (ofpter, jt1->maxri1[D_YOMI]);
  put_int (ofpter, jt1->maxri1[D_KANJI]);
  put_int (ofpter, jt1->maxri2);
  put_null (ofpter, 56);
#endif /* BDIC_WRITE_CHECK */

  return (0);
}

/* Header 64 Byte */
JS_STATIC int
input_header_hjt (FILE* ifpter, struct HJT* hjt1)
{
  if (input_file_uniq (&hjt1->dic_file_uniq, ifpter) == -1	/* 7 * 4 */
      || get_int (&hjt1->maxcomment, ifpter) == -1
      || get_int (&hjt1->maxserial, ifpter) == -1
      || get_null (ifpter, 28) == -1)
    return (-1);

  return (0);
}

JS_STATIC int
output_header_hjt (FILE* ofpter, struct HJT* hjt1)
{
#ifdef BDIC_WRITE_CHECK
  if ((output_file_uniq (&hjt1->dic_file_uniq, ofpter) == -1)
      || (put_int (ofpter, hjt1->maxcomment) == -1)
      || (put_int (ofpter, hjt1->maxserial) == -1)
      || (put_null (ofpter, 28) == -1))
    return (-1);
#else /* BDIC_WRITE_CHECK */
  output_file_uniq (&hjt1->dic_file_uniq, ofpter);
  put_int (ofpter, hjt1->maxcomment);
  put_int (ofpter, hjt1->maxserial);
  put_null (ofpter, 28);
#endif /* BDIC_WRITE_CHECK */

  return (0);
}


#if     !defined(JS) && !defined(JSERVER)
/* Only used in JUTIL */
void
Print_entry (
	w_char* yomi,
	w_char* kstr,
	w_char* cstr,
	int hindo,
	int ima,
	int hinsi,
	int serial,
	FILE* ofpter,
	int esc_exp)
{
  int len;
  char *k;
  char buf[32];
  static w_char revy[LENGTHKANJI];
  extern void kanji_esc_str ();

  if (serial != -1)
    {
      sprintf (buf, "%d\t", serial);
      vputs (buf, ofpter);
    }

  len = put_yomi_str (yomi, ofpter);
  if (len < 8)
    vputc ('\t', ofpter);
  if (len < 16)
    vputc ('\t', ofpter);
  vputc ('\t', ofpter);

  if (esc_exp)
    {
#ifdef CONVERT_from_TOP         /* Don't warry. Only use in jutil */
      wnn_Strcpy (revy, yomi);
#else /* conver from bottom */
      wnn_Sreverse (revy, yomi);
#endif /* CONVERT_from_TOP */
      kanji_esc_str (kstr, revy, wnn_Strlen (yomi));
      len = put_yomi_str (kstr, ofpter);
    }
  else
    {
      len = vputws (kstr, ofpter);
    }
  if (len < 8)
    vputc ('\t', ofpter);
  if (len < 16)
    vputc ('\t', ofpter);
  vputc ('\t', ofpter);

  k = wnn_get_hinsi_name (hinsi);
  if (k)
    {
      vputs (k, ofpter);
      if ((int) strlen (k) < 8)
        vputc ('\t', ofpter);
      vputc ('\t', ofpter);
    }
  else
    {
      sprintf (buf, "%d\t\t", hinsi);
      vputs (buf, ofpter);
    }

  if (ima)
    vputc ('*', ofpter);
  if (hindo == -1)
    {                           /*  Real hindo == -1 means Not to use it */
      vputs ("-", ofpter);
    }
  else
    {
      sprintf (buf, "%d", hindo);
      vputs (buf, ofpter);
    }
  if (cstr && cstr[0])
    {
      vputc ('\t', ofpter);
      len = vputws (cstr, ofpter);
    }
  vputc ('\n', ofpter);
}
#endif /* !JS && !JSERVER */

#ifdef BDIC_WRITE_CHECK
#undef BDIC_WRITE_CHECK
#endif
