/*
 *  $Id: dtoa.c $
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

/*
  辞書を ASCII 形式の辞書に変換するプログラム。
  */

#ifndef lint
static char *rcs_id = "$Id: dtoa.c $";
#endif /* lint */

#include <stdio.h>
#include "commonhd.h"
#include "wnn_config.h"
#include "jslib.h"
#include "jh.h"
#include "jdata.h"
#include "wnn_string.h"

#ifdef CONVERT_with_SiSheng
#include "cplib.h"
extern int pzy_flag;            /* Pinyin or Zhuyin */
#endif

extern void parse_options (), usage (), input_dic (), udtoujis (), kset (), sdtoujis (), ghindo (), sort (), output_ujis (), exit1 (), init_jeary (), get_kanji_str ();
extern int input_header (), input_comment (), input_hinsi_list (), little_endian (), revdic (), wnn_loadhinsi (), init_heap (), w_stradd (), input_hindo_header (), motoni2 (), get_n_EU_str ();
#ifdef CONVERT_with_SiSheng
extern int cwnn_yincod_pzy_str (), input_sisheng ();
#endif
static int rdtoujis (), set_hinsi (), input_hindo (), add_hindo ();

int which_dict;

char *com_name;
char infile[LINE_SIZE];
int esc_expand = 1;

int serial_out = 0;
int need_sort = 1;
struct JT jt;
struct wnn_file_head file_head;

char *hinsi_file_name = NULL;

int
main (argc, argv)
     int argc;
     char **argv;
{
  FILE *ifpter;
  char *cswidth_name;
  extern char *get_cswidth_name ();
  extern void set_cswidth ();

  com_name = argv[0];

  if (cswidth_name = get_cswidth_name (WNN_DEFAULT_LANG))
    set_cswidth (create_cswidth (cswidth_name));
  parse_options (&argc, &argv);

  if ((ifpter = fopen (infile, "r")) == NULL)
    {
      fprintf (stderr, "Can't open the input file %s.\n", infile);
      perror ("");
      exit (1);
    }
  if (input_header (ifpter, &jt, &file_head) == -1)
    exit (1);
  which_dict = jt.syurui;
  input_comment (ifpter);
  input_hinsi_list (ifpter);
  input_dic (ifpter);
  if (little_endian ())
    {
      revdic (&jt, 0);
    }
  set_hinsi ();
  ghindo (argc - 2, argv + 2);  /* Gather Hindo */
  if (which_dict == WNN_STATIC_DICT)
    {
      kset ();
      sdtoujis (jt.hontai, 0);
#ifdef CHINESE
    }
  else if ((which_dict & 0x00ff) == WNN_REV_DICT)
    {
#else
    }
  else if (which_dict == WNN_REV_DICT)
    {
#endif
      rdtoujis ();
    }
  else
    {
      udtoujis ();
    }
  if (need_sort)
    {
      sort ();
    }
  output_ujis (stdout, serial_out, esc_expand);
  exit (0);
}


void
parse_options (argc, argv)
     int *argc;
     char ***argv;
{
  int c;
  extern int optind;
  extern char *optarg;

#ifdef CONVERT_with_SiSheng
#  define OPTSTRING "nseEzh:"
#else
#  define OPTSTRING "nseEh:"
#endif

  while ((c = getopt (*argc, *argv, OPTSTRING)) != EOF)
    {
      switch (c)
        {
        case 'n':
          need_sort = 0;
          break;
        case 's':
          serial_out = 1;
          break;
        case 'e':
          esc_expand = 1;
          break;
        case 'E':
          esc_expand = 0;
          break;
        case 'h':
          hinsi_file_name = optarg;
          break;
#ifdef CONVERT_with_SiSheng
        case 'z':
          pzy_flag = CWNN_ZHUYIN;
          break;
#endif
        default:
          usage ();
        }
    }
  if (optind)
    {
      optind--;
      *argc -= optind;
      *argv += optind;
    }
  if (*argc < 2)
    {
      usage ();
    }
  strcpy (infile, (*argv)[1]);
  if (wnn_loadhinsi (hinsi_file_name) != 0)
    {
      fprintf (stderr, "Can't Open hinsi_file.\n");
      exit (1);
    }
}


void
usage ()
{
  fprintf (stderr, "Usage: %s [-nse -h hinsi_file_name] <dictionary file> <hindo file>*\n", com_name);
  fprintf (stderr, "Output the Text dictionary to stdout\n");
#ifdef CONVERT_with_SiSheng
  fprintf (stderr, "-z : Output Zhuyin (Default is Pinyin)\n");
#endif
  exit1 ();
}

void
input_dic (ifpter)
     FILE *ifpter;
{

  init_heap (0,                 /* jt.maxkanji, */
             jt.maxserial * YOMI_PER_LINE * 5,  /* may be enough */
             jt.maxserial, jt.maxserial, stdin);
  jt.table = (struct uind1 *) malloc (jt.maxtable * sizeof (struct uind1));
  jt.hontai = (UCHAR *) malloc (jt.maxhontai);
  jt.ri1[D_YOMI] = (struct rind1 *) malloc (jt.maxri1[D_YOMI] * sizeof (struct rind1));
  jt.ri1[D_KANJI] = (struct rind1 *) malloc (jt.maxri1[D_KANJI] * sizeof (struct rind1));
  jt.kanji = (UCHAR *) malloc (jt.maxkanji + 1);
  jt.ri2 = (struct rind2 *) malloc (jt.maxserial * sizeof (struct rind2));
  jt.hinsi = (w_char *) malloc (jt.maxserial * sizeof (w_char));
  if (!jt.table || !jt.hontai || !jt.ri1[D_YOMI] || !jt.ri1[D_KANJI] || !jt.kanji || !jt.ri2 || !jt.hinsi)
    {
      fprintf (stderr, "Malloc Failed.\n");
      exit (1);
    }
  init_jeary ();

  input_hindo (ifpter);
  fread (jt.hinsi, 2, jt.maxserial, ifpter);
#ifdef  CONVERT_with_SiSheng
  if (which_dict == CWNN_REV_DICT)
    input_sisheng (ifpter);
#endif
  fread (jt.kanji, 1, jt.maxkanji, ifpter);
/*  input_kanji(ifpter); */
  fread (jt.table, sizeof (struct uind1), jt.maxtable, ifpter);
  fread (jt.ri1[D_YOMI], sizeof (struct rind1), jt.maxri1[D_YOMI], ifpter);
  fread (jt.ri1[D_KANJI], sizeof (struct rind1), jt.maxri1[D_KANJI], ifpter);
  fread (jt.hontai, 1, jt.maxhontai, ifpter);
  fread (jt.ri2, sizeof (struct rind2), jt.maxserial, ifpter);
}

void
udtoujis ()
{

  w_char yomi[LENGTHYOMI];      /* 2 byte yomi */
  w_char kan[LENGTHYOMI];       /* 2 byte kanji */
  w_char comm[LENGTHYOMI];
  w_char ryomi[LENGTHYOMI];
  register struct uind2 *hop;
  register int i, k;
  register int serial;
  int len;
  UCHAR *c;

  for (i = 0; i < jt.maxtable; i++)
    {
      for (hop = (struct uind2 *) (jt.table[i].pter + (char *) jt.hontai); hop != (struct uind2 *) jt.hontai; hop = (struct uind2 *) (hop->next + (char *) jt.hontai))
        {
          /* Though each hop has the same yomi, but calculate yomi each time. */
          yomi[0] = jt.table[i].yomi1 >> 16;
          yomi[1] = jt.table[i].yomi1 & 0xffff;
          if (yomi[1])
            {
              yomi[2] = jt.table[i].yomi2 >> 16;
              if (yomi[2])
                {
                  yomi[3] = jt.table[i].yomi2 & 0xffff;
                }
            }
          len = hop->yomi[0];
          wnn_Strncpy (yomi + 4, (hop->yomi) + 1, len - 4);
          yomi[len] = 0;
          serial = hop->serial;
          for (k = hop->kosuu, c = hop->kanjipter + jt.kanji; k > 0; k--, serial++)
            {
              get_kanji_str (c, kan, NULL, comm);
              jeary[serial]->kanji = c;
              c += *c;
              jeary[serial]->serial = serial;
#ifdef  CONVERT_with_SiSheng
              wnn_Strcpy (ryomi, yomi);
#else
              wnn_Sreverse (ryomi, yomi);
#endif
              w_stradd (ryomi, &jeary[serial]->yomi);
              w_stradd (kan, &jeary[serial]->kan);
              w_stradd (comm, &jeary[serial]->comm);
            }
        }
    }
}

/* SD routine */

w_char yomi[LENGTHYOMI];
int je = 0;

void
kset ()
{
  int i;
  UCHAR *c;
  w_char kan[LENGTHYOMI];       /* 2 byte kanji */
  w_char comm[LENGTHYOMI];      /* 2 byte kanji */


  c = jt.kanji;
  for (i = 0; i < jt.maxserial; i++)
    {
      get_kanji_str (c, kan, NULL, comm);
      w_stradd (kan, &jeary[i]->kan);
      w_stradd (comm, &jeary[i]->comm);
      jeary[i]->kanji = c;
      c += *c;
    }
}

void
sdtoujis (hopter, level)
     char *hopter;
     register int level;
{
  register int k, j;
  register int tsize;
  register w_char *charst;
  register w_char *sumst;
  int *ptrst;
  static w_char ry[LENGTHYOMI];

  yomi[level + 1] = 0;
  switch (*(w_char *) hopter)
    {
    case ST_NORMAL:
      tsize = *(w_char *) (hopter + 2);
      charst = (w_char *) (hopter + 12);
      sumst = ((w_char *) charst + tsize + 2);  /* + 2 keeps two zero words */
      ptrst = (int *) ((w_char *) sumst + tsize);
      for (k = 0; k < tsize; k++)
        {
          j = sumst[k] - sumst[k - 1];
          if (j)
            {
              yomi[level] = charst[k];
              for (; j > 0; j--)
                {
                  jeary[je]->serial = je;
#ifdef  CONVERT_with_SiSheng
                  wnn_Strcpy (ry, yomi);
#else
                  wnn_Sreverse (ry, yomi);
#endif
                  w_stradd (ry, &jeary[je++]->yomi);
                }
            }
        }
      for (k = 0; k < tsize; k++)
        {
          if (ptrst[k] != ENDPTR)
            {
              yomi[level] = charst[k];
              sdtoujis ((char *) jt.hontai + ptrst[k], level + 1);
            }
        }
      break;
    case ST_NOENT:
      tsize = *(w_char *) (hopter + 2);
      charst = (w_char *) (hopter + 4);
      ptrst = (int *) AL_INT ((w_char *) charst + tsize);
      for (k = 0; k < tsize; k++)
        {
          if (ptrst[k] != ENDPTR)
            {
              yomi[level] = charst[k];
              sdtoujis ((char *) jt.hontai + ptrst[k], level + 1);
            }
        }
      break;
    case ST_NOPTER:
      tsize = *(w_char *) (hopter + 2);
      charst = (w_char *) (hopter + 12);
      sumst = ((w_char *) charst + tsize + 2);  /* + 2 keeps two zero words */
      for (k = 0; k < tsize; k++)
        {
          j = sumst[k] - sumst[k - 1];
          if (j)
            {
              yomi[level] = charst[k];
              for (; j > 0; j--)
                {
                  jeary[je]->serial = je;
#ifdef  CONVERT_with_SiSheng
                  wnn_Strcpy (ry, yomi);
#else
                  wnn_Sreverse (ry, yomi);
#endif
                  w_stradd (ry, &jeary[je++]->yomi);
                }
            }
        }
      break;
    case ST_SMALL:
      yomi[level] = *(w_char *) (hopter + 2);
      sdtoujis (hopter + 4, level + 1);
      break;
    }
}

static int
rdtoujis ()
{
  w_char yomi[LENGTHYOMI];      /* 2 byte yomi */
  w_char kan[LENGTHYOMI];       /* 2 byte yomi */
  w_char comm[LENGTHYOMI];      /* 2 byte yomi */
  register struct rind2 *hop;
  register int hopi;
  register int i;
  register int serial;

  for (i = 0; i < jt.maxri1[D_YOMI]; i++)
    {
      for (hopi = jt.ri1[D_YOMI][i].pter; hopi != RD_ENDPTR; hopi = hop->next[D_YOMI])
        {
          hop = hopi + jt.ri2;
          /* Though each hop has the same yomi, but calculate yomi each time. */
          get_kanji_str (hop->kanjipter + jt.kanji, kan, yomi, comm);
#ifdef CONVERT_with_SiSheng
          cwnn_yincod_pzy_str (yomi, yomi, wnn_Strlen (yomi), pzy_flag);
#endif
          serial = hopi;
          jeary[serial]->kanji = hop->kanjipter + jt.kanji;
          jeary[serial]->serial = serial;

          w_stradd (yomi, &jeary[serial]->yomi);
          w_stradd (kan, &jeary[serial]->kan);
          w_stradd (comm, &jeary[serial++]->comm);
        }
    }
  return (0);
}

void
ghindo (argc, argv)
     int argc;
     char **argv;
{
  FILE *fp;
  struct wnn_file_head h_file_head;
  struct HJT hjt;


  while (argc-- > 0)
    {
      if ((fp = fopen (*argv++, "r")) == NULL)
        {
          fprintf (stderr, "can't open %s\n", *(argv - 1));
          perror ("error in ghindo");
          exit (1);
        }
      if (input_hindo_header (fp, &hjt, &h_file_head) == -1)
        {
          perror ("error in ghindo 1");
          exit (1);
        }
      if (bcmp (&file_head.file_uniq_org, &hjt.dic_file_uniq, sizeof (struct wnn_file_uniq)))
        {
          fprintf (stderr, "%s(%s)Dic File and Hindo File mismatch.\n", infile, *(argv - 1));
          exit (1);
        }
      add_hindo (fp, hjt.maxserial);
    }
}

static int
add_hindo (ifpter, num)
     register FILE *ifpter;
     int num;
{
  register int i;
  register int k;
  for (i = 0; i < num; i++)
    {
      jeary[i]->hindo += motoni2 ((k = getc (ifpter)) & 0x7f);
      if (k == -1)
        return (-1);
    }
  return (0);
}

/* input dict routines */

int
input_comment (fp)
     register FILE *fp;
{
  return (get_n_EU_str (fp, jt.maxcomment, file_comment));
}

int
input_hinsi_list (fp)
     register FILE *fp;
{
  return (get_n_EU_str (fp, jt.maxhinsi_list, hinsi_list));
}


static int
input_hindo (ifpter)
     register FILE *ifpter;
{
  register int i;
  register int k;
  for (i = 0; i < jt.maxserial; i++)
    {
      jeary[i]->hindo = motoni2 ((k = getc (ifpter)) & 0x7f);
      if (k == -1)
        return (-1);
    }
  return (0);
}

static int
set_hinsi ()
{
  register int i;

  for (i = 0; i < jt.maxserial; i++)
    {
      jeary[i]->hinsi = jt.hinsi[i];
    }
  return (0);
}

#ifdef  CONVERT_with_SiSheng
int
input_sisheng (ifpter)
     register FILE *ifpter;
{
  register int i;
  unsigned short s;

  for (i = 0; i < jt.maxserial; i++)
    {
      if (get_short (&s, ifpter) == -1)
        return (-1);
      jeary[i]->ss = s;
    }
  return (0);
}

#endif
