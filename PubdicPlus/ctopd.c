/* Copyright 1994 Pubdic Project.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without
 * fee, provided that the above copyright notice appear in all copies
 * and that both that copyright notice and this permission notice
 * appear in supporting documentation, and that the name of Pubdic
 * Project not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior
 * permission.  Pubdic Project makes no representations about the
 * suitability of this software for any purpose.  It is provided "as
 * is" without express or implied warranty.
 *
 * PUBDIC PROJECT DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN 
 * NO EVENT SHALL PUBDIC PROJECT BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF 
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR 
 * OTHER TORTUOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR 
 * PERFORMANCE OF THIS SOFTWARE. 
 */

#ifndef lint
static char rcsid[] = "$Id: ctopd.c,v 1.3 2013/09/02 11:01:39 itisango Exp $";
#endif

#include <stdio.h>
#include <string.h>

static char *program;

#define READBUFSIZE 1024

char *
extstr (p, pp)
     char *p, **pp;
{
  char *res;

  while (*p == ' ' || *p == '\t')
    p++;
  res = p;
  while (*p && *p != ' ' && *p != '\t' && *p != '\n')
    p++;
  *p++ = '\0';
  if (pp)
    *pp = p;
  return res;
}

static void
ctop (file)
     FILE *file;
{
  char readbuf[READBUFSIZE], *p, *yomi, *hinshi, *kouho;
  int hindo;

  while (p = fgets (readbuf, sizeof (readbuf), file))
    {
      yomi = extstr (p, &p);
      hinshi = extstr (p, &p);
      kouho = extstr (p, &p);

      if (kouho[0] == '@' && !kouho[1])
        {
          kouho = yomi;
        }

      hindo = 0;
      for (p = hinshi; *p; p++)
        {
          if (*p == '*')
            {
              *p++ = '\0';
              hindo = atoi (p);
            }
        }

      printf ("%s %s %s %d\n", yomi, kouho, hinshi, hindo);
    }
}

main (argc, argv)
     int argc;
     char *argv[];
{
  FILE *ins = stdin;

  for (program = argv[0] + strlen (argv[0]); argv[0] < program; program--)
    {
      if (program[0] == '/')
        {
          program++;
          break;
        }
    }

  if (argc > 1)
    {
      ins = fopen (argv[1], "r");
      if (!ins)
        {
          fprintf (stderr, "%s: can not open file \"%s\".\n", program, argv[1]);
        }
    }
  ctop (ins);
  fclose (ins);
  exit (0);
}
