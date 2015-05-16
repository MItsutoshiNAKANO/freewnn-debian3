/*
 *  $Id: atod.c,v 1.13 2004/08/12 09:03:19 aono Exp $
 */

/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright FreeWnn Project 1999, 2000, 2002, 2004
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
UJIS 形式を、辞書登録可能形式, 及び固定形式辞書に変換するプログラム。
*/

#ifndef lint
static char *rcs_id = "$Id: atod.c,v 1.13 2004/08/12 09:03:19 aono Exp $";
#endif /* lint */

#ifdef HAVE_CONFIG_H
#  include <config.h>
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
#  endif
#endif /* STDC_HEADERS */
#if HAVE_UNISTD_H
#  include <unistd.h>
#endif

#include "commonhd.h"
#include "wnn_config.h"
#include "jslib.h"
#include "jh.h"
#include "jdata.h"
#ifdef CHINESE
#include "cplib.h"
#endif
#include "getopt.h"	/* GNU getopt in the stock */
#include "wnn_string.h"
#include "wnn_os.h"

extern int wnn_loadhinsi (), init_heap (), little_endian (), Sorted (), asshuku (), revdic ();
extern void ujis_header (), read_ujis (), reverse_yomi (), create_rev_dict (),
uniq_je (), sort_if_not_sorted (), output_header (), udytoS (), output_ri (), exit1 (), new_pwd (), rev_short_fun (), rev_w_char ();
extern
#ifdef BDIC_WRITE_CHECK
  int
#else
  void
#endif
  put_n_EU_str ();
static void ujistoud (), output_dic_index (), usage (), sdic_sort (),
ujistosd (), not_enough_area (), get_pwd (), output_hindo (), output_hinsi (), output_kanji (), rev_dic_data (), set_pter1 (), output_comment (), output_hinsi_list ();
void upd_kanjicount (), output_dic_data ();

/* Switcher variable between UD and SD */

int which_dict = WNN_REV_DICT;
#ifdef CHINESE
extern int pzy_flag;
static void output_sisheng ();
#endif

/* Variables both for UD and SD */
int reverse_dict = NORMAL;

char *com_name;
int maxserial = MAX_ENTRIES;
int kanjicount = 0;
char outfile[LINE_SIZE];
struct wnn_file_head file_head;
struct JT jt;
UCHAR *hostart, *hoend;         /* index 2 */
int to_esc = 0;

/* For SD */
#define HONTAI_PER_ENTRY 20
int node_count = 0;
UCHAR *hopter;


/* For UD */
struct uind1 *tary;             /* index 1 */
int tnum = 0;
struct uind2 *uhopter;

char *hinsi_file_name = NULL;


void
init (int argc, char **argv)
{
  int c;

  maxserial = MAX_ENTRIES;
  while ((c = getopt (argc, argv, "SURrs:P:p:Nneh:")) != EOF)
    {
      switch (c)
        {
        case 'S':
          which_dict = WNN_STATIC_DICT;
          break;
        case 'U':
          which_dict = WNN_UD_DICT;
          break;
        case 'R':
          which_dict = WNN_REV_DICT;
          break;
        case 'r':
          reverse_dict = REVERSE;
          break;
        case 'P':
          get_pwd (optarg, file_head.file_passwd);
          break;
        case 'p':
          get_pwd (optarg, jt.hpasswd);
          break;
        case 'N':
          strcpy (file_head.file_passwd, "*");
          break;
        case 'n':
          strcpy (jt.hpasswd, "*");
          break;
        case 'e':
          to_esc = 1;
          break;
        case 'h':
          hinsi_file_name = optarg;
          break;
        case 's':
          if (sscanf (optarg, "%d", &maxserial) == 0)
            {
              usage ();
              exit (1);
            }
          break;
        }
    }
  if (to_esc && which_dict == WNN_REV_DICT)
    {
      fprintf (stderr, "You can't make the kanji component of reverse dictionary compact.\n");
      exit (1);
    }
  if (optind)
    {
      optind--;
      argc -= optind;
      argv += optind;
    }
  if (argc != 2)
    {
      usage ();
      exit (1);
    }
  strncpy (outfile, argv[1], LINE_SIZE-1);
  outfile[LINE_SIZE-1] = '\0';
  if (wnn_loadhinsi (hinsi_file_name) != 0)
    {
      fprintf (stderr, "Can't Open hinsi_file.\n");
      exit (1);
    }
  if (init_heap (DEF_ENTRIES * HEAP_PER_LINE, DEF_ENTRIES * YOMI_PER_LINE, maxserial, DEF_ENTRIES, stdin) == -1)
    exit (1);
}

void
alloc_area (void)
{
  if (which_dict == WNN_STATIC_DICT)
    {
      if ((hostart = (UCHAR *) malloc (maxserial * HONTAI_PER_ENTRY)) == NULL)
        {
          fprintf (stderr, "Malloc Failed\n");
          exit (1);
        }
      hopter = hostart;
      hoend = (UCHAR *) hostart + maxserial * HONTAI_PER_ENTRY;
    }
  else
    {
      if ((tary = (struct uind1 *) malloc (jt.maxserial * sizeof (struct uind1))) == NULL ||
          /* Too large? */
          (hostart = (UCHAR *) malloc (jt.maxserial * (sizeof (struct uind2) + sizeof (w_char) * 2))) == NULL)
        {
          /* 2 must be enough? */
          fprintf (stderr, "Malloc Failed\n");
          exit (1);
        }
      hoend = (UCHAR *) ((char *) hostart + jt.maxserial * (sizeof (struct uind2) + sizeof (w_char) * 2));
    }
}

extern int sort_func_sdic ();
extern int sort_func_je ();
FILE *ofpter;

int
main (int argc, char** argv)
{
  char *cswidth_name;
  extern char *get_cswidth_name ();
  extern void set_cswidth ();

  com_name = argv[0];
  init (argc, argv);

  if (cswidth_name = get_cswidth_name (WNN_DEFAULT_LANG))
    set_cswidth (create_cswidth (cswidth_name));

#ifdef CHINESE
  ujis_header (&which_dict);    /* read header of UJIS dic  */
#else
  ujis_header ();               /* read header of UJIS dic  */
#endif
  /* like comment,total,hinsi  */
#ifdef CHINESE
  read_ujis (reverse_dict, to_esc, which_dict);

  if (which_dict != CWNN_REV_DICT && which_dict != BWNN_REV_DICT)
    reverse_yomi ();
#else
  read_ujis (reverse_dict, to_esc, (which_dict == WNN_REV_DICT) ? 1 : 0);
#ifndef CONVERT_from_TOP
  reverse_yomi ();
#endif
#endif

  if ((ofpter = fopen (outfile, "w")) == NULL)
    {
      fprintf (stderr, "Can't open the output file %s.\n", outfile);
      perror ("");
      exit (1);
    }

#ifdef CHINESE
  if ((which_dict & 0xff) == WNN_REV_DICT)
    {
#else
  if (which_dict == WNN_REV_DICT)
    {
#endif
      create_rev_dict ();
    }
  else
    {
      alloc_area ();
      if (which_dict == WNN_STATIC_DICT)
        {
          sdic_sort ();
          uniq_je (sort_func_sdic);
          output_dic_data ();
          ujistosd (0, 0);
        }
      else
        {
          sort_if_not_sorted ();
          uniq_je (sort_func_je);
          output_dic_data ();
          ujistoud ();
          set_pter1 ();
        }
    }
  output_dic_index ();
  rewind (ofpter);
  output_header (ofpter, &jt, &file_head);
#ifdef nodef
  output_comment (ofpter);      /* In order to change the byte order */
  output_hinsi_list (ofpter);   /* In order to change the byte order */
#endif
  exit (0);
}


w_char *
addyomient (int tn, w_char* yomi)
{
  int len = wnn_Strlen (yomi);
  tary[tn].yomi2 = 0;
  tary[tn].yomi1 = yomi[0] << 16;

  uhopter->yomi[0] = len;
  if (yomi[1])
    {
      tary[tn].yomi1 |= yomi[1];
      if (yomi[2])
        {
          tary[tn].yomi2 = yomi[2] << 16;
          if (yomi[3])
            {
              tary[tn].yomi2 |= yomi[3];
            }
          if (len > 4)
            {
              wnn_Strncpy (uhopter->yomi + 1, yomi + 4, len - 4);
              return (uhopter->yomi + 1 + len - 4);
            }
        }
    }
  return (uhopter->yomi + 1);
}

static void
ujistoud (void)
{
  w_char *yomi;
  w_char dummy = 0;             /* 2 byte yomi */
  w_char *pyomi;                /* maeno tangono yomi */
  w_char *wcp;
  int serial_count;
  w_char *kosuup = NULL;

  *(int *) hostart = 0;
  uhopter = (struct uind2 *) ((int *) hostart + 1);

  yomi = &dummy;

  for (serial_count = 0; serial_count < jt.maxserial; serial_count++)
    {
      pyomi = yomi;
      yomi = jeary[serial_count]->yomi;

      if (wnn_Strcmp (yomi, pyomi))
        {
          tary[tnum++].pter = (char *) uhopter - (char *) hostart;
          uhopter->next = 0;
          uhopter->serial = serial_count;
          uhopter->kanjipter = kanjicount;
          uhopter->kosuu = 1;
          kosuup = &uhopter->kosuu;
          wcp = addyomient (tnum - 1, yomi);
          uhopter = (struct uind2 *) (AL_INT (wcp));
        }
      else
        {
          if (kosuup)
            *kosuup += 1;
        }
      upd_kanjicount (serial_count);
    }
}

void
upd_kanjicount (int k)
{
  kanjicount += *jeary[k]->kanji;
}

static void
set_pter1 (void)
{
  int k;
  int len;
  w_char oyomi[LENGTHYOMI], nyomi[LENGTHYOMI];
  /* May be a little slow, but simple! */
  int lasts[LENGTHYOMI];        /* pter_to */

  for (k = 0; k < LENGTHYOMI; k++)
    {
      lasts[k] = -1;
    }

  for (k = 0; k < tnum; k++)
    {
      len = ((struct uind2 *) ((char *) hostart + tary[k].pter))->yomi[0];
      lasts[len] = k;
      for (len--; len; len--)
        {
          if (lasts[len] >= 0)
            {
              udytoS (oyomi, lasts[len], hostart, tary);
              udytoS (nyomi, k, hostart, tary);
              if (wnn_Substr (oyomi, nyomi))
                {
                  tary[k].pter1 = lasts[len];
                  break;
                }
            }
        }
      if (len == 0)
        tary[k].pter1 = -1;
    }
}

void
output_dic_data (void)
{

  fprintf (stderr, "%d words are in this dictionary\n", jt.maxserial);

  if (little_endian ())
    {
      rev_dic_data ();
    }

  jt.syurui = which_dict;
  jt.gosuu = jt.maxserial;
  output_header (ofpter, &jt, &file_head);      /* dummy; Will be rewitten. */
  output_comment (ofpter);
  output_hinsi_list (ofpter);
  output_hindo (ofpter);
  output_hinsi (ofpter);
#ifdef CONVERT_with_SiSheng
  if (which_dict == CWNN_REV_DICT)
    output_sisheng (ofpter);
#endif
  output_kanji (ofpter);
}

static void
output_dic_index (void)
{
  if (which_dict == WNN_UD_DICT)
    {
      fprintf (stderr, " tnum = %d\n ind2= %d\n kanji = %d\n", tnum, (char *) uhopter - (char *) hostart, kanjicount);
      jt.maxtable = tnum;
      jt.maxhontai = (char *) uhopter - (char *) hostart;
      jt.maxri2 = jt.maxri1[D_YOMI] = jt.maxri1[D_KANJI] = 0;
      jt.hontai = hostart;
      jt.table = tary;
      jt.ri1[D_YOMI] = NULL;
      jt.ri1[D_KANJI] = NULL;
      jt.ri2 = NULL;
#ifdef CHINESE
    }
  else if ((which_dict & 0xff) == WNN_REV_DICT)
    {
#else
    }
  else if (which_dict == WNN_REV_DICT)
    {
#endif
      fprintf (stderr, "kanji = %d\n", kanjicount);
      jt.maxtable = 0;
      jt.maxhontai = 0;
      jt.maxri2 = jt.maxserial;
      jt.hontai = NULL;
      jt.table = NULL;
      /* jt.ri1, jt.ri2 is set in create_rev_dict */
    }
  else
    {
      fprintf (stderr, "node_count = %d  ind= %d\n kanji = %d\n", node_count, (char *) hopter - (char *) hostart, kanjicount);
      jt.maxtable = 0;
      jt.maxhontai = (char *) hopter - (char *) hostart;
      jt.maxri2 = jt.maxri1[D_YOMI] = jt.maxri1[D_KANJI] = 0;
      jt.hontai = hostart;
      jt.table = NULL;
      jt.ri1[D_YOMI] = NULL;
      jt.ri1[D_KANJI] = NULL;
      jt.ri2 = NULL;
    }
  jt.maxkanji = kanjicount;

  if (little_endian ())
    {
      revdic (&jt, 1);
    }

  if (which_dict == WNN_UD_DICT)
    {
      fwrite ((char *) tary, sizeof (struct uind1), tnum, ofpter);
      fwrite (hostart, 1, (char *) uhopter - (char *) hostart, ofpter);
#ifdef CHINESE
    }
  else if ((which_dict & 0xff) == WNN_REV_DICT)
    {
#else
    }
  else if (which_dict == WNN_REV_DICT)
    {
#endif
      output_ri (ofpter);
    }
  else
    {
      fwrite (hostart, 1, (char *) hopter - (char *) hostart, ofpter);
    }
}


static void
usage (void)
{
  fprintf (stderr, "Usage : %s [-r -R -S -U -e -s maximum word count(default %d) -P passwd (or -N) -p hindo_passwd (or -n) -h hinsi_file_name] <dictonary filename>\n", com_name, MAX_ENTRIES);
  fprintf (stderr, "Input the ascii dictionary from stdin\n");
  fprintf (stderr, "-r is for creating dictionary with normal and reverse index\n");
  fprintf (stderr, "-R is for creating reverse (implies updatable) dictionary. (default)\n");
  fprintf (stderr, "-S is for creating static    dictionary.\n");
  fprintf (stderr, "-U is for creating updatable dictionary.\n");
  fprintf (stderr, "-e is for compacting kanji string.\n");
  exit1 ();
}

/* SD commands */

int
yStrcmp (w_char* a, w_char*b)
{
  register int c, d;
  for (; *a && *a == *b; a++, b++);
  if (*a == 0)
    return (-(int) *b);
  if (*b == 0)
    return ((int) *a);
  c = *(a + 1);
  d = *(b + 1);
  if (c == 0 && d == 0)
    return ((int) *a - (int) *b);
  if (c == 0)
    return (-1);
  if (d == 0)
    return (1);
  return ((int) *a - (int) *b);
}

int
sort_func_sdic (char* a, char* b)
{
  int tmp;
  struct je *pa, *pb;
  pa = *((struct je **) a);
  pb = *((struct je **) b);
  tmp = yStrcmp (pa->yomi, pb->yomi);
  if (tmp)
    return (tmp);
  /* Changed the order to sort, from yomi->kanji->hinsi 
   * to yomi->hinsi->kanji, in order to separate (struct jdata) by
   * hinsi'es.
   */
  if (pa->hinsi != pb->hinsi)
    return ((int) (pa->hinsi) - (int) (pb->hinsi));
  return (wnn_Strcmp (pa->kan, pb->kan));
}


static void
sdic_sort (void)
{
  if (!Sorted ((char *) jeary, (int) jt.maxserial, sizeof (struct je *), sort_func_sdic))
    {
      qsort ((char *) jeary, (int) jt.maxserial, sizeof (struct je *), sort_func_sdic);
    }
}


static w_char chartable[YOMI_KINDS];
static w_char numtable1[YOMI_KINDS];
static w_char *numtable = numtable1 + 1;
static int endt = 0;

int
analize_size (int start_je, int level,
	      int* statep, int* end_jep, int* mid_jep)
{
  w_char *c = jeary[start_je]->yomi;
  int je;
  int level1 = level + 1;
  int end_je, mid_je;
  w_char y1, y2;
  int je1, je2;

  for (; endt-- > 0;)
    {
      chartable[endt] = numtable[endt] = 0;
    }
  endt = 0;                     /* This line is needed when endt is originally 0 */
  for (je = start_je; je < jt.maxserial && wnn_Strncmp (c, jeary[je]->yomi, level) == 0 && jeary[je]->yomi[level1] == 0; je++);
  *mid_jep = mid_je = je;
  for (je = start_je; je < jt.maxserial && wnn_Strncmp (c, jeary[je]->yomi, level) == 0; je++);
  *end_jep = end_je = je;

  je1 = start_je;
  je2 = mid_je;
  y1 = (je1 >= mid_je) ? 0xffff : jeary[je1]->yomi[level];
  y2 = (je2 >= end_je) ? 0xffff : jeary[je2]->yomi[level];
  for (;;)
    {
      if (y1 > y2)
        {
          chartable[endt] = y2;
          for (numtable[endt] = numtable[endt - 1]; je2 < end_je && jeary[je2]->yomi[level] == y2; je2++);
          y2 = (je2 >= end_je) ? 0xffff : jeary[je2]->yomi[level];
          endt++;
        }
      else if (y1 < y2)
        {
          chartable[endt] = y1;
          for (numtable[endt] = numtable[endt - 1]; je1 < end_je && jeary[je1]->yomi[level] == y1; je1++, numtable[endt]++);
          y1 = (je1 >= mid_je) ? 0xffff : jeary[je1]->yomi[level];
          endt++;
        }
      else
        {
          if (y1 >= 0xffff && y2 >= 0xffff)
            break;
          chartable[endt] = y1;
          for (; je2 < end_je && jeary[je2]->yomi[level] == y2; je2++);
          for (numtable[endt] = numtable[endt - 1]; je1 < mid_je && jeary[je1]->yomi[level] == y1; je1++, numtable[endt]++);
          y2 = (je2 >= end_je) ? 0xffff : jeary[je2]->yomi[level];
          y1 = (je1 >= mid_je) ? 0xffff : jeary[je1]->yomi[level];
          endt++;
        }
    }
  if (mid_je == start_je)
    {
      if (endt == 1)
        *statep = ST_SMALL;
      else
        *statep = ST_NOENT;
    }
  else if (mid_je == end_je)
    *statep = ST_NOPTER;
  else
    *statep = ST_NORMAL;

  return (endt);
}



static void
ujistosd (int start_je, int level)
{
  int state;
  int tsize;
  w_char *charst;
  w_char *sumst;
  int *ptrst;
  int mid_je, end_je;
  int je;
  int k;

  node_count++;

  tsize = analize_size (start_je, level, &state, &end_je, &mid_je);

  *(w_char *) hopter = state;
  hopter += 2;
  switch (state)
    {
    case ST_NORMAL:
      if (hopter + tsize * 8 + 12 + 16 >= hoend)
        not_enough_area ();
      *(w_char *) hopter = tsize;
      hopter += 2;
      *(int *) hopter = start_je;
      hopter += 4;
      *(int *) hopter = kanjicount;
      hopter += 4;
      charst = (w_char *) hopter;
      sumst = ((w_char *) hopter + tsize + 2);  /* + 2 keeps two zero words */
      charst[tsize] = charst[tsize + 1] = 0;
      ptrst = (int *) ((w_char *) sumst + tsize);
      hopter = (UCHAR *) (ptrst + tsize);
      for (k = 0; k < tsize; k++)
        {
          charst[k] = chartable[k];
          sumst[k] = numtable[k];
        }
      for (k = start_je; k < mid_je; k++)
        {
          upd_kanjicount (k);
        }
      for (je = mid_je, k = 0; je < end_je;)
        {
          for (; k < tsize && charst[k] < jeary[je]->yomi[level]; k++)
            ptrst[k] = ENDPTR;
          if (k == tsize)
            fprintf (stderr, "Error\n");
          ptrst[k] = (char *) hopter - (char *) hostart;
          ujistosd (je, level + 1);
          for (; je < end_je && jeary[je]->yomi[level] == charst[k]; je++);
          k++;
        }
      break;
    case ST_NOENT:
      if (hopter + tsize * 8 + 4 + 16 >= hoend)
        not_enough_area ();
      *(w_char *) hopter = tsize;
      hopter += 2;
      charst = (w_char *) hopter;
      ptrst = (int *) AL_INT ((w_char *) charst + tsize);
      hopter = (UCHAR *) (ptrst + tsize);
      for (k = 0; k < tsize; k++)
        {
          charst[k] = chartable[k];
        }
      for (je = mid_je, k = 0; je < end_je;)
        {
          for (; k < tsize && charst[k] < (jeary[je]->yomi)[level]; k++)
            ptrst[k] = ENDPTR;
          if (k == tsize)
            fprintf (stderr, "Error\n");
          ptrst[k] = (char *) hopter - (char *) hostart;
          ujistosd (je, level + 1);
          for (; je < end_je && (jeary[je]->yomi)[level] == charst[k]; je++);
          k++;
        }
      break;
    case ST_NOPTER:
      if (hopter + tsize * 4 + 12 + 16 >= hoend)
        not_enough_area ();
      *(w_char *) hopter = tsize;
      hopter += 2;
      *(int *) hopter = start_je;
      hopter += 4;
      *(int *) hopter = kanjicount;
      hopter += 4;
      charst = (w_char *) hopter;
      sumst = ((w_char *) hopter + tsize + 2);  /* + 2 keeps two zero words */
      charst[tsize] = charst[tsize + 1] = 0;
      hopter = (UCHAR *) ((w_char *) sumst + tsize);
      for (k = 0; k < tsize; k++)
        {
          charst[k] = chartable[k];
          sumst[k] = numtable[k];
        }
      for (k = start_je; k < mid_je; k++)
        {
          upd_kanjicount (k);
        }
      break;
    case ST_SMALL:
      if (hopter + 4 + 16 >= hoend)
        not_enough_area ();
      *(w_char *) hopter = chartable[0];
      hopter += 2;
      ujistosd (start_je, level + 1);
      break;
    }
}

#define INCR_HO 150

static void
not_enough_area (void)
{
  /*
     UCHAR *oldstart = hostart;
     int size = hoend - hostart + INCR_HO * HONTAI_PER_ENTRY;
   */
  fprintf (stderr, "Sorry allocated area is exhausted.node_count = %d\n", node_count);
  fprintf (stderr, "Retry with option -S <dict_size>.\n");
  exit (1);

/*
 * Only for Sdic 
 * Can't Realloc!! many pointeres exist on the stack!!
 */

/*
    fprintf(stderr, "Realloc.\n");
    if((hostart = (char *)realloc(hostart, size)) == NULL){
        fprintf(stderr, "Ralloc Failed\n");
        exit(1);
    }
    hopter = hostart + (hopter - oldstart);
    hoend = (char *)((char *)hostart + size);
*/
}


static void
get_pwd (char* fname, char* crypted)
{
  char pwd[WNN_PASSWD_LEN];
  FILE *fp;

  if ((fp = fopen (fname, "r")) == NULL)
    {
      fprintf (stderr, "No password_file %s.\n", fname);
      exit (1);
    }
  fgets (pwd, WNN_PASSWD_LEN, fp);
  fclose (fp);
  new_pwd (pwd, crypted);
}

/* output dict routine's */
static void
output_comment (FILE* fp)
{
  put_n_EU_str (fp, file_comment, jt.maxcomment);
}

static void
output_hinsi_list (FILE* fp)
{
  put_n_EU_str (fp, hinsi_list, jt.maxhinsi_list);
}

static void
output_hindo (FILE* ofpter)
{
  register int i;
  for (i = 0; i < jt.maxserial; i++)
    {
      putc (asshuku (jeary[i]->hindo), ofpter);
    }
}

static void
output_hinsi (FILE* ofpter)
{
  int i;
  short k;
  int little_endianp = little_endian ();
  /* hinsi ha koko de hikkuri kaesu */

  for (i = 0; i < jt.maxserial; i++)
    {
      k = jeary[i]->hinsi;
      if (little_endianp)
        {
          rev_short_fun (&k);
        }
      fwrite (&k, 1, 2, ofpter);
    }
}

static void
output_kanji (FILE* ofpter)
{
  int i;

  for (i = 0; i < jt.maxserial; i++)
    {
      fwrite (jeary[i]->kanji, 1, *(jeary[i]->kanji), ofpter);
    }
}

static void
rev_dic_data (void)
{
  int i;

  /* Comment and Hinsi list do not need to reverse. */
  for (i = 0; i < jt.maxserial; i++)
    {
      rev_w_char ((w_char *) (jeary[i]->kanji + 2), *(jeary[i]->kanji) / 2 - 1);
    }
}

#ifdef  CONVERT_with_SiSheng
extern void put_short ();

static void
output_sisheng (FILE* ofpter)
{
  int i;

  for (i = 0; i < jt.maxserial; i++)
    {
      put_short (ofpter, jeary[i]->ss);
    }
}
#endif /* CONVERT_with_SiSheng */
