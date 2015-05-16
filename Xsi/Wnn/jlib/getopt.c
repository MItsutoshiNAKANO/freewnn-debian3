/*
 *  $Id: getopt.c $
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

 /*LINTLIBRARY*/
/***********************************************************************
                        getopt.c

        BSD用getopt関数。ライブラリファイルはlibgetopt.aです。
        システムに予めgetopt()が用意されていなかったら、これを
        リンクして下さい。そのためには、トップレベルの
        メークファイルのGETOPTLIBに、../jlib/libgetopt.aを
        加えて下さい。

***********************************************************************/
#define NULL    0
#define EOF     (-1)
#define ERR_PRINT(s, c)                                                 \
        if(opterr){                                                     \
                char    errbuf[2];                                      \
                                                                        \
                errbuf[0] = c;                                          \
                errbuf[1] = '\n';                                       \
                (void)write(2, argv[0], (unsigned)strlen(argv[0]));     \
                (void)write(2, s, (unsigned)strlen(s));                 \
                (void)write(2, errbuf, 2);                              \
        }
#ifdef  BSD42
#define strchr  index
#endif
extern int strcmp ();
extern char *strchr ();

int opterr = 1;
int optind = 1;
int optopt;
char *optarg;

int
getopt (argc, argv, opts)
     int argc;
     char **argv, *opts;
{
  static int sp = 1;
  register int c;
  register char *cp;

  if (sp == 1)
    {
      if (optind >= argc || argv[optind][0] != '-' || argv[optind][1] == '\0')
        {
          return (EOF);
        }
      else if (strcmp (argv[optind], "--") == NULL)
        {
          optind++;
          return (EOF);
        }
    }
  optopt = c = argv[optind][sp];
  if (c == ':' || (cp = strchr (opts, c)) == NULL)
    {
      ERR_PRINT (": unknown option, -", c);
      if (argv[optind][++sp] == '\0')
        {
          optind++;
          sp = 1;
        }
      return ('?');
    }
  if (*++cp == ':')
    {
      if (argv[optind][sp + 1] != '\0')
        {
          optarg = &argv[optind++][sp + 1];
        }
      else if (++optind >= argc)
        {
          ERR_PRINT (": argument missing for -", c);
          sp = 1;
          return ('?');
        }
      else
        {
          optarg = argv[optind++];
        }
      sp = 1;
    }
  else
    {
      if (argv[optind][++sp] == '\0')
        {
          sp = 1;
          optind++;
        }
      optarg = NULL;
    }
  return (c);
}

/*
  Local Variables:
  kanji-flag: t
  End:
*/
