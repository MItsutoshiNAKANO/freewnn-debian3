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

/*
  (Updatable, Stable) dictionary read routine.
*/
static char rcs_id[] = "$Id: readfile.c,v 1.9 2003/06/07 02:23:58 hiroo Exp $";

#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#include <stdio.h>
#if STDC_HEADERS
#  include <stdlib.h>
#  include <string.h>
#else
#  if HAVE_MALLOC_H
#    include <malloc.h>
#  endif
#  if HAVE_STRINGS_H
#    include <strings.h>
#  endif
#endif /* STDC_HEADERS */

#if defined(HAVE_SYS_TYPES_H)
#include <sys/types.h>
#endif

#include "commonhd.h"
#include "de_header.h"
#include "jdata.h"

#ifdef WRITE_CHECK
static int vfwrite (void*, int, int, FILE *);
#endif
static struct JT *readdict (FILE *);
static struct JT *copy_dict (struct JT *);
static int write_file_real (struct wnn_file *, FILE *, int);
static int writedict (struct JT *, FILE *);
static int write_hindo_of_dict (struct JT *, FILE *);
static struct JT * free_dict (struct JT *);
static struct HJT *readhindo (FILE *);
static int writehindo (struct HJT *, FILE *);
static struct HJT * free_hindo (struct HJT *);
static int alloc_dict (struct JT *);
static int check_and_change_pwd (char *, char *, char *);

#define vfread(A, B, C, fp) ((fp)?fread((A),(B),(C),(fp)):fread_cur((A),(B),(C)))
#ifndef WRITE_CHECK
#define vfwrite(A, B, C, fp) {if(fp) {fwrite((A),(B),(C),(fp));}else\
                                     {fwrite_cur((A),(B),(C));}}
#endif
#define vclose(fp) {if(fp) {fclose(fp);}else{fclose_cur();}}

#ifdef WRITE_CHECK
static int
vfwrite (void *ptr, int size, int nitems, FILE *fp)
{
  if (fp)
    {
      if (fwrite (ptr, size, nitems, fp) != nitems)
        return (-1);
    }
  else
    {
      fwrite_cur (ptr, size, nitems);
    }
  return (0);
}
#endif

int
read_file (struct wnn_file *wf)
{
  FILE *fp;
  struct wnn_file_head fh;
  if (wf->localf == REMOTE)
    {
      if (fopen_read_cur (wf->name) == NULL)
        {
          wnn_errorno = WNN_FILE_READ_ERROR;
          log_err ("read_file:could not open file %s.", wf->name);
          return (-1);
        }
      fp = NULL;
    }
  else
    {
#ifdef WRITE_CHECK
      check_backup (wf->name);
#endif
      if ((fp = fopen (wf->name, "r")) == NULL)
        {
          wnn_errorno = WNN_FILE_READ_ERROR;
          log_err("read_file:could not open file %s.", wf->name);
          return (-1);
        }
    }
  if (input_file_header (fp, &fh) == -1)
    {
      wnn_errorno = WNN_NOT_A_FILE;
      goto ERROR_RET;
    }
  bcopy ((char *) &(fh.file_uniq), (char *) &(wf->f_uniq), WNN_F_UNIQ_LEN);
  bcopy ((char *) &(fh.file_uniq_org), (char *) &(wf->f_uniq_org), WNN_F_UNIQ_LEN);
  strncpy (wf->passwd, fh.file_passwd, WNN_PASSWD_LEN);
  wf->file_type = fh.file_type;
  wf->ref_count = 0;

  switch (fh.file_type)
    {
    case WNN_FT_DICT_FILE:
      wf->area = (char *) readdict (fp);
      if (wf->area == NULL)
        goto ERROR_RET;
      break;
    case WNN_FT_HINDO_FILE:
      wf->area = (char *) readhindo (fp);
      if (wf->area == NULL)
        goto ERROR_RET;
      break;
    case WNN_FT_FUZOKUGO_FILE:
      wf->area = (char *) fzk_read (fp);
      if (wf->area == NULL)
        goto ERROR_RET;
      break;
    }
  vclose (fp);
  return (0);
ERROR_RET:
  vclose (fp);
  return (-1);
}

static struct JT *
readdict (FILE *fp)
{
  struct JT *jt1;
  long x;

  jt1 = (struct JT *) malloc (sizeof (struct JT));
  jt1->node = 0;
  if (input_header_jt (fp, jt1) == -1)
    {
      wnn_errorno = WNN_NOT_A_FILE;
      free (jt1);
      return (NULL);
    }
  if (jt1->syurui == WNN_UD_DICT)
    {
      jt1->bufsize_serial = (jt1->maxserial + MAXSERIAL);
      jt1->bufsize_kanji = (jt1->maxkanji + MAXKANJI);
      jt1->bufsize_hontai = (jt1->maxhontai + MAXHONTAI);
      jt1->bufsize_table = (jt1->maxtable + MAXTABLE);
      jt1->bufsize_ri1[D_YOMI] = 0;
      jt1->bufsize_ri1[D_KANJI] = 0;
#if     defined(CONVERT_by_STROKE) || defined(CONVERT_with_SiSheng)
    }
  else if ((jt1->syurui & 0xff) == WNN_REV_DICT)
    {
#else
    }
  else if (jt1->syurui == WNN_REV_DICT)
    {
#endif /* CONVERT_by_STROKE || CONVERT_with_SiSheng */
      jt1->bufsize_serial = (jt1->maxserial + MAXSERIAL);
      jt1->bufsize_kanji = (jt1->maxkanji + MAXKANJI);
      jt1->bufsize_hontai = (jt1->maxhontai + MAXHONTAI);
      jt1->bufsize_table = 0;
      jt1->bufsize_ri1[D_YOMI] = (jt1->maxri1[D_YOMI] + MAXTABLE);
      jt1->bufsize_ri1[D_KANJI] = (jt1->maxri1[D_KANJI] + MAXTABLE);
    }
  else if (jt1->syurui == WNN_STATIC_DICT)
    {                           /* WNN_STATIC_DICT */
      jt1->bufsize_serial = jt1->maxserial;
      jt1->bufsize_kanji = jt1->maxkanji;
      jt1->bufsize_hontai = jt1->maxhontai;
      jt1->bufsize_table = 0;
      jt1->bufsize_ri1[D_YOMI] = 0;
      jt1->bufsize_ri1[D_KANJI] = 0;
    }
  else
    {
      wnn_errorno = WNN_NOT_A_DICT;
      log_err ("not a correct dictionary.");
      free (jt1);
      return (NULL);
    }
  if (alloc_dict (jt1) == -1)
    {
      free (jt1);
      return (NULL);
    }
  if (vfread (jt1->comment, 2, jt1->maxcomment, fp) != jt1->maxcomment)
    {
      wnn_errorno = WNN_NOT_A_DICT;
      log_err ("not a correct dictionary.");
      goto error;
    }

  if (vfread (jt1->hinsi_list, 2, jt1->maxhinsi_list, fp) != jt1->maxhinsi_list)
    {
      wnn_errorno = WNN_NOT_A_DICT;
      log_err ("not a correct dictionary.");
      goto error;
    }

  if (vfread (jt1->hindo, 1, jt1->maxserial, fp) != jt1->maxserial)
    {
      wnn_errorno = WNN_NOT_A_DICT;
      log_err ("not a correct dictionary.");
      goto error;
    }
  if (vfread (jt1->hinsi, 2, jt1->maxserial, fp) != jt1->maxserial)
    {
      wnn_errorno = WNN_NOT_A_DICT;
      log_err ("not a correct dictionary.");
      goto error;
    }
#ifdef  CONVERT_with_SiSheng
  if (jt1->syurui == CWNN_REV_DICT)
    if (vfread (jt1->sisheng, 2, jt1->maxserial, fp) != jt1->maxserial)
      {
        wnn_errorno = WNN_NOT_A_DICT;
        log_err ("not a correct dictionary.");
        goto error;
      }
#endif /* CONVERT_with_SiSheng */
  if (vfread (jt1->kanji, 1, jt1->maxkanji, fp) != jt1->maxkanji)
    {
      wnn_errorno = WNN_NOT_A_DICT;
      log_err ("not a correct dictionary.");
      goto error;
    }
  if (vfread (jt1->table, sizeof (struct uind1), jt1->maxtable, fp) != jt1->maxtable)
    {
      wnn_errorno = WNN_NOT_A_DICT;
      log_err ("not a correct dictionary.");
      goto error;
    }
  if (vfread (jt1->ri1[D_YOMI], sizeof (struct rind1), jt1->maxri1[D_YOMI], fp) != jt1->maxri1[D_YOMI])
    {
      wnn_errorno = WNN_NOT_A_DICT;
      log_err ("not a correct dictionary.");
      goto error;
    }
  if (vfread (jt1->ri1[D_KANJI], sizeof (struct rind1), jt1->maxri1[D_KANJI], fp) != jt1->maxri1[D_KANJI])
    {
      wnn_errorno = WNN_NOT_A_DICT;
      log_err ("not a correct dictionary.");
      goto error;
    }
  if (vfread (jt1->hontai, 1, jt1->maxhontai, fp) != jt1->maxhontai)
    {
      wnn_errorno = WNN_NOT_A_DICT;
      log_err ("not a correct dictionary.");
      goto error;
    }
  if (vfread (jt1->ri2, sizeof (struct rind2), jt1->maxri2, fp) != jt1->maxri2)
    {
      wnn_errorno = WNN_NOT_A_DICT;
      log_err ("not a correct dictionary.");
      goto error;
    }

  if (fp != NULL)
    {
      x = ftell (fp);
      fseek (fp, 0, 2);
      if (x != ftell (fp))
        {
          wnn_errorno = WNN_NOT_A_DICT;
	  log_err ("not a correct dictionary.");
          goto error;
        }
    }

  make_hinsi_list (jt1);

  if (jt1->maxhontai == 0 && (jt1->syurui == WNN_UD_DICT || jt1->syurui == WNN_STATIC_DICT))
    {
      jt1->maxhontai = 4;
    }
  if (little_endian ())
    {
      revdic (jt1, 0);
    }
  jt1->dirty = 0;
  jt1->hdirty = 0;
#ifdef  CONVERT_by_STROKE
  if (jt1->syurui == BWNN_REV_DICT)
    if ((jt1->max_bnode = create_b_index (jt1)) == -1)
      {
        goto error;
      }
#endif /* CONVERT_by_STROKE */
  return (jt1);
error:
  jt1 = free_dict (jt1);
  return (NULL);
}

/*
 * user_jisho_realloc: ユーザー辞書に語を登録し過ぎて一杯になった時reallocする
 *   return value: success:SUCCESS (non zero), failure:NULL
 */
int
ud_realloc_hontai (struct JT *jt)
{
  size_t new_bufsize;
  UCHAR *tp;

  log_debug ("hontai realloc occured.");
  new_bufsize = jt->maxhontai + MAXHONTAI;
  if ((tp = (UCHAR *) realloc (jt->hontai, new_bufsize)) == NULL)
    {
      wnn_errorno = WNN_MALLOC_ERR;
      log_err ("could not make the jisho area bigger.");
      return (NULL);
    }
  jt->bufsize_hontai = new_bufsize;
  jt->hontai = tp;
  return (SUCCESS);
}

int
ud_realloc_kanji (struct JT *jt)          /* Also for rd */
{
  size_t new_bufsize;
  UCHAR *tp;

  log_debug ("kanji realloc occured.");
  new_bufsize = jt->maxkanji + MAXKANJI;
  if ((tp = (UCHAR *) realloc (jt->kanji, new_bufsize)) == NULL)
    {
      wnn_errorno = WNN_MALLOC_ERR;
      log_err ("could not make the jisho area bigger.");
      return (NULL);
    }
  jt->bufsize_kanji = new_bufsize;
  jt->kanji = tp;
  return (SUCCESS);
}

int
ud_realloc_serial (struct JT *jt)         /* Also for rd */
{
  size_t new_bufsize;
  UCHAR *tp_hindo;
  unsigned short *tp_hinsi;
#ifdef  CONVERT_with_SiSheng
  unsigned short *tp_sisheng;
#endif
  struct rind2 *tp_ri2;

  log_debug ("serial realloc occured.");
  new_bufsize = jt->maxserial + MAXSERIAL;

  tp_hindo = (UCHAR *) realloc (jt->hindo, new_bufsize * sizeof (*jt->hindo));
  tp_hinsi = (unsigned short *) realloc (jt->hinsi, new_bufsize * sizeof (*jt->hinsi));

#ifdef  CONVERT_with_SiSheng
  tp_sisheng = (unsigned short *) realloc (jt->sisheng,
					   new_bufsize * sizeof (*jt->sisheng));
#endif /* CONVERT_with_SiSheng */

  if (tp_hindo == NULL || tp_hinsi == NULL
#ifdef  CONVERT_with_SiSheng
     || tp_sisheng == NULL
#endif /* CONVERT_with_SiSheng */
    )
    {
      free (tp_hindo);
      free (tp_hinsi);
#ifdef  CONVERT_with_SiSheng
      free (tp_sisheng);
#endif /* CONVERT_with_SiSheng */

      wnn_errorno = WNN_MALLOC_ERR;
      log_err ("could notmake the jisho area bigger.");
      return (NULL);
    }
  jt->bufsize_serial = new_bufsize;
  jt->hindo = tp_hindo;
  jt->hinsi = tp_hinsi;
#ifdef  CONVERT_with_SiSheng
  jt->sisheng = tp_sisheng;
#endif /* CONVERT_with_SiSheng */


#if defined(CONVERT_by_STROKE) || defined(CONVERT_with_SiSheng)
  if ((jt->syurui & 0xff) == WNN_REV_DICT)
#else
  if (jt->syurui == WNN_REV_DICT)
#endif /* CONVERT_by_STROKE || CONVERT_with_SiSheng */
    {
      tp_ri2 = (struct rind2 *) realloc (jt->ri2,
					 new_bufsize * sizeof (struct rind2));
      if (tp_ri2 == NULL)
        {
          wnn_errorno = WNN_MALLOC_ERR;
          log_err ("could not make the jisho area bigger.");
          return (NULL);
        }
      jt->ri2 = tp_ri2;
    }

  return (SUCCESS);
}

int
ud_realloc_table (struct JT *jt)
{
  size_t new_bufsize;              
  struct uind1 *tp;

  log_debug ("table realloc occured.");
  new_bufsize = jt->maxtable + MAXTABLE;
  tp = (struct uind1 *) realloc (jt->table, new_bufsize * sizeof (struct uind1));
  if (tp == NULL)
    {
      wnn_errorno = WNN_MALLOC_ERR;
      log_err ("could not make the jisho area bigger.");
      return (NULL);
    }
  jt->bufsize_table = new_bufsize;
  jt->table = tp;
  return (SUCCESS);
}

int
rd_realloc_ri1 (struct JT *jt, int which)
{
  size_t new_bufsize;
  struct rind1 *tp;

  log_debug ("ri1 reallocation occured.");
  new_bufsize = jt->maxri1[which] + MAXTABLE;
  tp = (struct rind1 *) realloc (jt->ri1[which],
				 new_bufsize * sizeof (struct rind1));
  if (tp == NULL)
    {
      wnn_errorno = WNN_MALLOC_ERR;
      log_err ("could not make the jisho area bigger.");
      return (NULL);
    }
  jt->bufsize_ri1[which] = new_bufsize;
  jt->ri1[which] = tp;
  return (SUCCESS);
}


int
hindo_file_realloc (struct HJT *hjt)
{
  size_t new_bufsize;
  UCHAR *tp;

  log_debug ("hindo file reallocation occured.");
  new_bufsize = hjt->maxserial + MAXSERIAL;
  tp = (UCHAR *) realloc (hjt->hindo, new_bufsize * sizeof (*hjt->hindo));
  if (tp == NULL)
    {
      wnn_errorno = WNN_MALLOC_ERR;
      log_err ("could not make the hindo file area bigger.");
      return (NULL);
    }
  hjt->bufsize_serial = new_bufsize;
  hjt->hindo = tp;
  return (SUCCESS);
}

#ifdef CONVERT_by_STROKE
int
rd_realloc_bind (struct JT *jt)
{
  size_t new_bufsize;
  struct b_node *tp;

  log_debug ("jt->bind reallocation occured.");
  new_bufsize = jt->bufsize_bnode + MAXBIND;
  tp = (struct b_node *) realloc (jt->bind, new_bufsize * sizeof (struct b_node));
  if (tp == NULL)
    {
      wnn_errorno = WNN_MALLOC_ERR;
      log_err ("could not make the jisho area bigger.");
      return (NULL);
    }
  jt->bufsize_bnode = new_bufsize;
  jt->bind = tp;
  return (SUCCESS);
}
#endif /* CONVERT_by_STROKE */

static struct JT *
copy_dict (struct JT *jt1)
{
  struct JT *jt2;

  jt2 = (struct JT *) malloc (sizeof (struct JT));
  *jt2 = *jt1;

  if (alloc_dict (jt2) == -1)
    {
      free (jt2);
      return (NULL);
    }
  bcopy (jt1->hindo, jt2->hindo, jt1->maxserial);
  bcopy (jt1->hinsi, jt2->hinsi, jt1->maxserial * 2);
#ifdef  CONVERT_with_SiSheng
  if (jt1->syurui == CWNN_REV_DICT)     /* Chinese PinYin dic only  */
    bcopy (jt1->sisheng, jt2->sisheng, jt1->maxserial * 2);
#endif /* CONVERT_with_SiSheng */
  bcopy (jt1->kanji, jt2->kanji, jt1->maxkanji);
  bcopy (jt1->table, jt2->table, jt1->maxtable * sizeof (struct uind1));
  bcopy (jt1->hontai, jt2->hontai, jt1->maxhontai);
  bcopy (jt1->comment, jt2->comment, jt1->maxcomment * sizeof (w_char));
  bcopy (jt1->hinsi_list, jt2->hinsi_list, jt1->maxhinsi_list * sizeof (w_char));
  bcopy (jt1->ri2, jt2->ri2, jt1->maxri2 * sizeof (struct rind2));
  bcopy (jt1->ri1[D_YOMI], jt2->ri1[D_YOMI], jt1->maxri1[D_YOMI] * sizeof (struct rind1));
  bcopy (jt1->ri1[D_KANJI], jt2->ri1[D_KANJI], jt1->maxri1[D_KANJI] * sizeof (struct rind1));
  return (jt2);
}


int
must_write_file (struct wnn_file *wf, struct wnn_file_uniq *uniq)
{
  if (f_uniq_cmp (&(wf->f_uniq), uniq))
    return (3);
  /* The File to write is not read. */
  switch (wf->file_type)
    {
    case WNN_FT_DICT_FILE:
      {
        struct JT *jt = (struct JT *) wf->area;
        if (jt->dirty)
          return 1;
        if (jt->hdirty)
          return 2;
        return 0;
      }
    case WNN_FT_HINDO_FILE:
      {
        struct HJT *hjt = (struct HJT *) wf->area;
        if (!(hjt->hdirty))
          return (0);
        else
          return 1;
      }
    case WNN_FT_FUZOKUGO_FILE:
      wnn_errorno = NOT_SUPPORTED_OPERATION;
      return (-1);
    }
  return (-1);
}

void
clear_dirty_bit (struct wnn_file *wf)
{
  switch (wf->file_type)
    {
    case WNN_FT_DICT_FILE:
      {
        struct JT *jt = (struct JT *) wf->area;
        jt->dirty = 0;
        jt->hdirty = 0;
      }
      break;
    case WNN_FT_HINDO_FILE:
      {
        struct HJT *hjt = (struct HJT *) wf->area;
        hjt->hdirty = 0;
      }
      break;
    case WNN_FT_FUZOKUGO_FILE:
      break;
    }
}


int
rcv_file (struct wnn_file *wf, int mode)
{
  FILE *fp;
  int x;

/*    if(wf->localf == REMOTE){
      }
*/
  if (fopen_write_cur (wf->name) == NULL)
    {
      log_err ("receive_file:No file %s.", wf->name);
      wnn_errorno = WNN_FILE_WRITE_ERROR;
      return (-1);
    }
  fp = NULL;
  x = write_file_real (wf, fp, mode);
  vclose (fp);
  return (x);
}

int
write_file (struct wnn_file *wf, char *n)
{
  FILE *fp;
  int mode = 3;
  struct wnn_file_head fh;
#ifdef WRITE_CHECK
  char *tmp = NULL, *backup = NULL;

  check_backup (n);
#endif
  if ((fp = fopen (n, "r")) != NULL)
    {                           /* Old File Exist */
      if (input_file_header (fp, &fh) == -1)
        {
          wnn_errorno = WNN_NOT_A_FILE;
          fclose (fp);
          return (-1);
        }
      mode = must_write_file (wf, &(fh.file_uniq));
      fclose (fp);
      if (mode == -1)
        return -1;
    }

  if (mode == 0)
    {
      return (0);               /* Need Not Write */
    }
  else if (mode == 1 || mode == 3)
    {                           /* 3 when the file is not the one to be read. */
#ifdef WRITE_CHECK
      backup = make_backup_file (n);
      if ((tmp = make_tmp_file (n, 0, &fp)) == NULL)
        {
          delete_tmp_file (backup);
#else /* WRITE_CHECK */
      if ((fp = fopen (n, "w+")) == NULL)
        {
#endif /* WRITE_CHECK */
          wnn_errorno = WNN_FILE_WRITE_ERROR;
          return (-1);
        }
    }
  else if (mode == 2)
    {
#ifdef WRITE_CHECK
      backup = make_backup_file (n);
      if ((tmp = make_tmp_file (n, 1, &fp)) == NULL)
        {
          delete_tmp_file (backup);
#else /* WRITE_CHECK */
      if ((fp = fopen (n, "r+")) == NULL)
        {                       /* New File */
#endif /* WRITE_CHECK */
          wnn_errorno = WNN_FILE_WRITE_ERROR;
          return (-1);
        }
    }
  if (write_file_real (wf, fp, mode) == -1)
    {
      fclose (fp);
#ifdef WRITE_CHECK
      delete_tmp_file (tmp);
      delete_tmp_file (backup);
#endif /* WRITE_CHECK */
      return -1;
    }
  fclose (fp);
#ifdef WRITE_CHECK
  move_tmp_to_org (tmp, n, 1);
  delete_tmp_file (backup);
#endif /* WRITE_CHECK */
  if ((mode == 1) || (mode == 2))
    {
      clear_dirty_bit (wf);
    }
  return (0);
}

static int
write_file_real (struct wnn_file *wf,
		 FILE *fp,
		 int mode	/* 1 For All, 2 For only hindo */ )
{
  struct wnn_file_head fh;

  if (fp)
    rewind (fp);
  bcopy ((char *) &wf->f_uniq, (char *) &(fh.file_uniq), WNN_F_UNIQ_LEN);
  bcopy ((char *) &wf->f_uniq_org, (char *) &(fh.file_uniq_org), WNN_F_UNIQ_LEN);
  bcopy (wf->passwd, fh.file_passwd, WNN_PASSWD_LEN);
  fh.file_type = wf->file_type;

  if (output_file_header (fp, &fh) == -1)
    {
      wnn_errorno = WNN_FILE_WRITE_ERROR;
      goto ERROR_RET;
    }
  switch (fh.file_type)
    {
    case WNN_FT_DICT_FILE:
      {
        struct JT *jt2;
        struct JT *jt = (struct JT *) wf->area;
        if (little_endian () && jt->dirty)
          {
            if ((jt2 = copy_dict ((struct JT *) wf->area)) == NULL)
              goto ERROR_RET;
            revdic (jt2, 1);
            if (writedict (jt2, fp) == -1)
              goto ERROR_RET;
            jt2 = free_dict (jt2);
          }
        else
          {
/*            if(writedict(wf->area, fp) == -1)goto ERROR_RET; */
            if (mode == 2)
              {
                if (write_hindo_of_dict (wf->area, fp) == -1)
                  goto ERROR_RET;
              }
            else
              {
                if (writedict (wf->area, fp) == -1)
                  goto ERROR_RET;
              }
          }
      }
      break;
    case WNN_FT_HINDO_FILE:
      if (writehindo (wf->area, fp) == -1)
        goto ERROR_RET;
      break;
    case WNN_FT_FUZOKUGO_FILE:
      wnn_errorno = NOT_SUPPORTED_OPERATION;
      goto ERROR_RET;
    }
  return (0);
ERROR_RET:
  return (-1);
}

static int
writedict (struct JT *jt1, FILE *fp)
{

  if (output_header_jt (fp, jt1) == -1)
    return (-1);
#ifdef WRITE_CHECK
  if ((vfwrite (jt1->comment, 2, jt1->maxcomment, fp) == -1) ||
      (vfwrite (jt1->hinsi_list, 2, jt1->maxhinsi_list, fp) == -1) || (vfwrite (jt1->hindo, 1, jt1->maxserial, fp) == -1) || (vfwrite (jt1->hinsi, 2, jt1->maxserial, fp) == -1))
    return (-1);
#ifdef  CONVERT_with_SiSheng
  if (jt1->syurui == CWNN_REV_DICT)     /* for Chinese PinYin dic only */
    if (vfwrite (jt1->sisheng, 2, jt1->maxserial, fp) == -1)
      return (-1);
#endif /* CONVERT_with_SiSheng */
  if ((vfwrite (jt1->kanji, 1, jt1->maxkanji, fp) == -1) ||
      (vfwrite (jt1->table, sizeof (struct uind1), jt1->maxtable, fp) == -1) ||
      (vfwrite (jt1->ri1[D_YOMI], sizeof (struct rind1),
                jt1->maxri1[D_YOMI], fp) == -1) ||
      (vfwrite (jt1->ri1[D_KANJI], sizeof (struct rind1),
                jt1->maxri1[D_KANJI], fp) == -1) || (vfwrite (jt1->hontai, 1, jt1->maxhontai, fp) == -1) || (vfwrite (jt1->ri2, sizeof (struct rind2), jt1->maxri2, fp) == -1))
    return (-1);
#else /* WRITE_CHECK */
  vfwrite (jt1->comment, 2, jt1->maxcomment, fp);
  vfwrite (jt1->hinsi_list, 2, jt1->maxhinsi_list, fp);
  vfwrite (jt1->hindo, 1, jt1->maxserial, fp);
  vfwrite (jt1->hinsi, 2, jt1->maxserial, fp);
#ifdef  CONVERT_with_SiSheng
  if (jt1->syurui == CWNN_REV_DICT)     /* for Chinese PinYin dic only */
    vfwrite (jt1->sisheng, 2, jt1->maxserial, fp);
#endif /* CONVERT_with_SiSheng */
  vfwrite (jt1->kanji, 1, jt1->maxkanji, fp);
  vfwrite (jt1->table, sizeof (struct uind1), jt1->maxtable, fp);
  vfwrite (jt1->ri1[D_YOMI], sizeof (struct rind1), jt1->maxri1[D_YOMI], fp);
  vfwrite (jt1->ri1[D_KANJI], sizeof (struct rind1), jt1->maxri1[D_KANJI], fp);
  vfwrite (jt1->hontai, 1, jt1->maxhontai, fp);
  vfwrite (jt1->ri2, sizeof (struct rind2), jt1->maxri2, fp);
#endif /* WRITE_CHECK */

  return (0);
}

static int
write_hindo_of_dict (struct JT *jt1, FILE *fp)
{
  if (output_header_jt (fp, jt1) == -1)
    return (-1);
#ifdef WRITE_CHECK
  if ((vfwrite (jt1->comment, 2, jt1->maxcomment, fp) == -1) || (vfwrite (jt1->hindo, 1, jt1->maxserial, fp) == -1))
    return (-1);
#else /* WRITE_CHECK */
  vfwrite (jt1->comment, 2, jt1->maxcomment, fp);
  vfwrite (jt1->hindo, 1, jt1->maxserial, fp);
#endif /* WRITE_CHECK */
  return (0);
}



int
discardfile (struct wnn_file *wf)
{
#ifdef nodef
  FILE *fp;
  if (wf->localf == LOCAL)
    {
      if ((fp = fopen (wf->name, "r")) == NULL)
        {
          log_err ("discardfile:No file %s.", wf->name);
          return (-1);
        }
      fclose (fp);
    }
#endif
  switch (wf->file_type)
    {
    case WNN_FT_DICT_FILE:
      wf->area = free_dict (wf->area);
      break;
    case WNN_FT_HINDO_FILE:
      wf->area = free_hindo (wf->area);
      break;
    case WNN_FT_FUZOKUGO_FILE:
/*
        fzk_discard(wf->area);
*/
      break;
    }
  return (0);
}

static struct JT *
free_dict (struct JT *jt)
{
  if (jt)
    {
      free (jt->hindo);
      free (jt->hinsi);
#ifdef  CONVERT_with_SiSheng
      free (jt->sisheng);
#endif /* CONVERT_with_SiSheng */
      free (jt->kanji);
      free (jt->table);
      free (jt->hontai);
      free (jt->comment);
      free (jt->hinsi_list);
      free (jt->ri1[D_YOMI]);
      free (jt->ri1[D_KANJI]);
      free (jt->ri2);
      free (jt->node);
#ifdef  CONVERT_by_STROKE
      free (jt->bind);
#endif /* CONVERT_by_STROKE */
      free (jt);
    }
  return (NULL);
}



static struct HJT *
readhindo (FILE *fp)
{
  struct HJT *hjt1;

  hjt1 = (struct HJT *) malloc (sizeof (struct HJT));
  if (input_header_hjt (fp, hjt1) == -1)
    {
      wnn_errorno = WNN_NOT_A_FILE;
      free (hjt1);
      return (NULL);
    }

  hjt1->bufsize_serial = (hjt1->maxserial + MAXSERIAL);

  hjt1->hindo = (UCHAR *) NULL;
  hjt1->comment = (w_char *) NULL;
  if ((hjt1->hindo = (UCHAR *) malloc (hjt1->bufsize_serial)) == NULL || (hjt1->comment = (w_char *) malloc (hjt1->maxcomment * sizeof (w_char))) == NULL)
    {
      wnn_errorno = WNN_MALLOC_ERR;
      free (hjt1->hindo);
      free (hjt1->comment);
      free (hjt1);
      log_err ("could not allocate hindo area.");
      return (NULL);
    }

  if (vfread (hjt1->comment, 2, hjt1->maxcomment, fp) != hjt1->maxcomment)
    {
      wnn_errorno = WNN_NOT_HINDO_FILE;
      log_err ("not a correct hindo file.");
      goto error;
    }

  if (vfread (hjt1->hindo, 1, hjt1->maxserial, fp) != hjt1->maxserial)
    {
      wnn_errorno = WNN_NOT_HINDO_FILE;
      log_err ("not a correct hindo file.");
      goto error;
    }
  hjt1->hdirty = 0;
  return (hjt1);
error:
  hjt1 = free_hindo (hjt1);
  return (NULL);
}


static int
writehindo (struct HJT *hjt1, FILE *fp)
{
  if (output_header_hjt (fp, hjt1) == -1)
    return (-1);
#ifdef WRITE_CHECK
  if (vfwrite (hjt1->hindo, 1, hjt1->maxserial, fp) == -1)
    return (-1);
#else
  vfwrite (hjt1->hindo, 1, hjt1->maxserial, fp);
#endif
  return (0);
}

static struct HJT *
free_hindo (struct HJT *hjt)
{
  if (hjt)
    {
      free (hjt->hindo);
      free (hjt->comment);
      free (hjt);
    }
  return (NULL);
}


int
create_hindo_file1 (struct wnn_file *wf,
		    char *fn,
		    w_char *comm,
		    char *passwd)
{

  return (create_hindo_file (&(wf->f_uniq_org), fn, comm, passwd, ((struct JT *) (wf->area))->maxserial));
}

int
match_dic_and_hindo_p (struct wnn_file *wfp, struct wnn_file *whfp)
{
  struct HJT *hjtp;
  hjtp = (struct HJT *) (whfp->area);
  if (bcmp ((char *) &wfp->f_uniq_org, (char *) (&(hjtp->dic_file_uniq)), WNN_F_UNIQ_LEN) == 0)
    return (1);
  return (0);
}

static int
alloc_dict (struct JT *jt1)
{
  jt1->hindo = (UCHAR *) NULL;
  jt1->hinsi = (unsigned short *) NULL;
#ifdef  CONVERT_with_SiSheng
  jt1->sisheng = (unsigned short *) NULL;
#endif /* CONVERT_with_SiSheng */
  jt1->kanji = (UCHAR *) NULL;
  jt1->table = (struct uind1 *) NULL;
  jt1->hontai = (UCHAR *) NULL;
  jt1->comment = (w_char *) NULL;
  jt1->hinsi_list = (w_char *) NULL;
  jt1->ri1[D_YOMI] = (struct rind1 *) NULL;
  jt1->ri1[D_KANJI] = (struct rind1 *) NULL;
  jt1->ri2 = (struct rind2 *) NULL;
  jt1->node = (struct wnn_hinsi_node *) NULL;
#ifdef  CONVERT_by_STROKE
  jt1->bind = (struct b_node *) NULL;
#endif /* CONVERT_by_STROKE */
  if (((jt1->hindo = (UCHAR *) malloc (jt1->bufsize_serial)) == NULL) || ((jt1->hinsi = (unsigned short *) (malloc (jt1->bufsize_serial * sizeof (short)))) == NULL) ||
#ifdef  CONVERT_with_SiSheng
      ((jt1->sisheng = (unsigned short *) (malloc (jt1->bufsize_serial * sizeof (short)))) == NULL) ||
#endif /* CONVERT_with_SiSheng */
      ((jt1->kanji = (UCHAR *) malloc (jt1->bufsize_kanji)) == NULL) ||
      ((jt1->table = (struct uind1 *) malloc (jt1->bufsize_table * sizeof (struct uind1))) == NULL) ||
      ((jt1->hontai = (UCHAR *) malloc (jt1->bufsize_hontai)) == NULL) ||
      ((jt1->comment = (w_char *) malloc (jt1->maxcomment * sizeof (w_char))) == NULL) ||
      ((jt1->hinsi_list = (w_char *) malloc (jt1->maxhinsi_list * sizeof (w_char))) == NULL) ||
      ((jt1->ri1[D_YOMI] = (struct rind1 *) malloc (jt1->bufsize_ri1[D_YOMI] * sizeof (struct rind1))) == NULL) ||
      ((jt1->ri1[D_KANJI] = (struct rind1 *) malloc (jt1->bufsize_ri1[D_KANJI] * sizeof (struct rind1))) == NULL) ||
      ((jt1->ri2 = (struct rind2 *) malloc (jt1->bufsize_serial * sizeof (struct rind2))) == NULL))
    {
      wnn_errorno = WNN_MALLOC_ERR;
      free (jt1->hindo);
      free (jt1->hinsi);
#ifdef  CONVERT_with_SiSheng
      free (jt1->sisheng);
#endif /* CONVERT_with_SiSheng */
      free (jt1->kanji);
      free (jt1->table);
      free (jt1->hontai);
      free (jt1->comment);
      free (jt1->hinsi_list);
      free (jt1->ri1[D_YOMI]);
      free (jt1->ri1[D_KANJI]);
      free (jt1->ri2);
      log_err ("could not allocate jisho area.");
      return (-1);
    }
  return (0);
}

int
file_comment_set (struct wnn_file *wf, w_char *com)
{
  struct JT *jt;
  struct HJT *hjt;
  size_t new_size;
  w_char *tp;

  switch (wf->file_type)
    {
    case WNN_FT_DICT_FILE:
      jt = (struct JT *) wf->area;
      new_size = Strlen (com) + 1;
      if ((tp = (w_char *) realloc (jt->comment, jt->maxcomment * sizeof (w_char))) == NULL)
	{
	  return (NULL);
	}
      jt->maxcomment = new_size;
      jt->comment = tp;
      Strcpy (jt->comment, com);
      jt->dirty = 1;
      break;
    case WNN_FT_HINDO_FILE:
      hjt = (struct HJT *) wf->area;
      new_size = Strlen (com) + 1;
      if ((tp = (w_char *) realloc (hjt->comment, new_size * sizeof (w_char))) == NULL)
	{
	  return (NULL);
	}
      hjt->maxcomment = new_size;
      hjt->comment = tp;
      Strcpy (hjt->comment, com);
      hjt->hdirty = 1;
      break;
    case WNN_FT_FUZOKUGO_FILE:
      wnn_errorno = NOT_SUPPORTED_OPERATION;
      return (NULL);
    }
  return (SUCCESS);
}

int
file_password_set (struct wnn_file *wf,
		   int which,
		   char *old,
		   char *new)
{
  struct JT *jt;
  struct HJT *hjt;

  if (which == 0)
    which = 3;
  if (wf->file_type == WNN_FT_FUZOKUGO_FILE)
    {
      wnn_errorno = NOT_SUPPORTED_OPERATION;
      return (-1);
    }
  if (which & 0x01 || wf->file_type == WNN_FT_HINDO_FILE)
    {
      if (check_and_change_pwd (wf->passwd, old, new) == -1)
        return (-1);
      if (wf->file_type == WNN_FT_DICT_FILE)
        {
          jt = (struct JT *) wf->area;
          jt->dirty = 1;
        }
      else
        {
          hjt = (struct HJT *) wf->area;
          hjt->hdirty = 1;
        }
    }
  if (wf->file_type == WNN_FT_DICT_FILE && (which & 0x02))
    {
      jt = (struct JT *) wf->area;
      if (check_and_change_pwd (jt->hpasswd, old, new) == -1)
        return (-1);
      jt->dirty = 1;
    }
  return (0);
}

static int
check_and_change_pwd (char *pwd, char *old, char *new)
{
  if (!check_pwd (old, pwd))
    {
      wnn_errorno = WNN_INCORRECT_PASSWD;
      return (-1);
    }
  new_pwd (new, pwd);
  return (0);
}
