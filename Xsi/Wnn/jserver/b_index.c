/*
 *  $Id: b_index.c $
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

/**  cWnn  Version 1.1   **/

#include <stdio.h>
#include "commonhd.h"
#include "de_header.h"
#include "jdata.h"

#ifdef CONVERT_by_STROKE
/*******  Create B_dic index    *********************************************/
static int delete_b_node (), creat_b_node (), bnode_alloc (), bnode_free ();

static int free_bnode = -1;     /* Initially no free b_node */
static int b_cnt;               /* Current member of b_node in b_nodes area */
                                /* Note,  b_cnt < jt->bufsize_bnode */

int
create_b_index (jt)             /* Create B_index for B_dic     */
     struct JT *jt;
{
  int i;
  int serial;
  w_char *yomi;                 /* The inputed character string */

  /* Make Space of b_nodes */
  jt->bind = (struct b_node *) malloc (2 * jt->bufsize_ri1[0] * sizeof (struct b_node));

  /*  If the bind size is determined in atod, we use the
     following statement instead of the above  
   */
  if (jt->bind == NULL)
    {
      error1 ("Error in creating head of b_index \n");
      return (-1);
    }
  /* Set the first one be a  dummy b_node */
  jt->bind[0].pter_next = -1;
  jt->bind[0].pter_son = -1;
  jt->bind[0].pter = -1;

/* Delete the following line when the bufsize of bnode is set at atod step */
  jt->bufsize_bnode = 2 * jt->bufsize_ri1[0];

  b_cnt = 0;

        /** For each tuple in ri1[0] create b_nodes */
  for (i = 0; i < jt->maxri1[0]; i++)
    {
      serial = (jt->ri1[0] + i)->pter;
      yomi = KANJI_str (jt->ri2[serial].kanjipter + jt->kanji, 0);
      b_index_add (jt, yomi, serial);
    }
  return (b_cnt);
}

/******************** For each character in a tuple creat the b_node *******/
int
b_index_add (jt, yomi, serial)
     struct JT *jt;
     w_char *yomi;
     int serial;
{
  int k;
  int p_node;                   /* Current b_node in No.  */

  p_node = 0;
  for (k = 0; k < Strlen (yomi); k++)
    {
      if ((p_node = creat_b_node (jt, yomi, k, p_node)) == -1)
        {
          error1 ("Error in creating b_index\n");
          return (-1);
        }
    }
  jt->bind[p_node].pter = serial;
  return (0);
}

void
b_index_delete (jt, serial)
     struct JT *jt;
     int serial;
{
  w_char *yomi;
  yomi = KANJI_str (jt->ri2[serial].kanjipter + jt->kanji, 0);
  delete_b_node (jt, yomi, 0, 0);
}

static int
delete_b_node (jt, yomi, level, p_node) /* return:   0   Having no son     */
     struct JT *jt;             /*           1   Having some sons  */
     w_char *yomi;
     int level;
     int p_node;
{
  int tmp_node;
  int buf_node1, buf_node2;
  w_char *yo_kanji = NULL;

  if (p_node == -1)
    return (0);

  buf_node1 = jt->bind[p_node].pter_son;        /*Initialize two tmp nodes */
  buf_node2 = jt->bind[p_node].pter_son;

  /* search if the node exists already */
  while (buf_node2 != -1)
    {
      tmp_node = buf_node2;
      while (jt->bind[tmp_node].pter == -1)
        {
          tmp_node = jt->bind[tmp_node].pter_son;
        }
      yo_kanji = KANJI_str (jt->ri2[jt->bind[tmp_node].pter].kanjipter + jt->kanji, 0);
      if (yomi[level] > yo_kanji[level])
        {
          buf_node1 = buf_node2;
          buf_node2 = jt->bind[buf_node2].pter_next;
        }
      else
        break;
    }
  if (yo_kanji == NULL || yomi[level] != yo_kanji[level])
    {
      /* Error case */
      error1 ("Error on b_index \n");
      return (-1);
    }

  if (level == (Strlen (yomi) - 1))
    {
      jt->bind[buf_node2].pter = -1;    /* HERE HERE */
      if (jt->bind[buf_node2].pter_son != -1)   /* having son */
        return (1);
    }
  if (level < (Strlen (yomi) - 1))
    {
      if ((delete_b_node (jt, yomi, level + 1, buf_node2) != 0) || (jt->bind[buf_node2].pter != -1))
        return (1);
    }
  /* 0 case: (bnode_free) below */
  if (buf_node1 == buf_node2)
    {                           /* only one b_node in this level */
      bnode_free (jt, buf_node2);
      return (0);
    }
  else
    {
      jt->bind[buf_node1].pter_next = jt->bind[buf_node2].pter_next;
      bnode_free (jt, buf_node2);
      return (1);
    }
}

/*** Create a son of the perient node p_node, when it does not exist *******/

static int
creat_b_node (jt, yomi, level, p_node)
     struct JT *jt;             /* Header of current dic */
     w_char *yomi;              /* Cureent character in this level */
     int level;                 /* level No     */
     int p_node;                /* cureent b_node */
{
  int new_node;
  int buf_node1, buf_node2;
  int tmp_node;
  w_char *yo_kanji = NULL;

  buf_node1 = jt->bind[p_node].pter_son;        /*Initialize two tmp nodes */
  buf_node2 = jt->bind[p_node].pter_son;

  /* search if the node exists already */
  while (buf_node2 != -1)
    {
      tmp_node = buf_node2;
      while (jt->bind[tmp_node].pter == -1)
        {
          tmp_node = jt->bind[tmp_node].pter_son;
        }
      yo_kanji = KANJI_str (jt->ri2[jt->bind[tmp_node].pter].kanjipter + jt->kanji, 0);

      if (yomi[level] > yo_kanji[level])
        {
          buf_node1 = buf_node2;
          buf_node2 = jt->bind[buf_node2].pter_next;
        }
      else
        break;
    }

  if (buf_node1 == -1)
    {                           /* the cur is the first */
      if (buf_node2 == -1)
        {
          if ((new_node = bnode_alloc (jt)) == -1)
            {
              error1 ("Error on Re-alloc area of Bindex\n");
              return (-1);
            }
          jt->bind[p_node].pter_son = new_node;

          jt->bind[new_node].pter_next = -1;
          jt->bind[new_node].pter_son = -1;
          jt->bind[new_node].pter = -1;
        }
      else
        return (-1);            /* Error case.  Impossible case  */
      return (new_node);

    }
  else if (buf_node2 == -1)
    {                           /* insert to last */
      new_node = bnode_alloc (jt);
      jt->bind[buf_node1].pter_next = new_node;

      jt->bind[new_node].pter_next = -1;
      jt->bind[new_node].pter_son = -1;
      jt->bind[new_node].pter = -1;
      return (new_node);

    }
  else if (yo_kanji == NULL)
    {
      return (-1);
    }
  else if (yomi[level] == yo_kanji[level])      /* exist already */
    return (buf_node2);

  /* insert  in before tnp_node2 */
  else if (yomi[level] < yo_kanji[level])
    {
      new_node = bnode_alloc (jt);
      jt->bind[new_node].pter_next = buf_node2;
      jt->bind[new_node].pter_son = -1;
      jt->bind[new_node].pter = -1;


      if (buf_node1 == buf_node2)       /*insert to first */
        jt->bind[p_node].pter_son = new_node;
      else
        jt->bind[buf_node1].pter_next = new_node;       /*to middle */
      return (new_node);
    }
  else
    return (-1);
}


/* bnode_alloc(jt) gets a b_node from free_bnode list if it exists. Otherwise
   it gets the b_node sequently from b_cnt++.  When b_cnt is greater than 
   bufsize of b_node. rallocation is will be performed                          */

static int
bnode_alloc (jt)
     struct JT *jt;
{
  int i;

  if (free_bnode != -1)
    {                           /* Use free b_nodes */
      i = free_bnode;
      free_bnode = jt->bind[free_bnode].pter_next;
      return (i);
    }
  if (b_cnt++ >= jt->bufsize_bnode)     /* Use new  b_nodes */
    if (rd_realloc_bind (jt) == -1)     /* re-alloc */
      return (-1);
  return (jt->bufsize_bnode = b_cnt);   /* Not re-alloc */
}

/************************************************************************/
                                         /* Free a b_node[k] from jt    */
static int
bnode_free (jt, k)
     struct JT *jt;
     int k;
{
  if (k <= 0 || k > jt->max_bnode)
    {
      error1 ("Error in free_bnode\n");
      return (-1);
    }
  jt->bind[k].pter = -1;        /* Initial this node   */
  jt->bind[k].pter_son = -1;
  jt->bind[k].pter_next = -1;

  if (k < jt->max_bnode)
    {
      jt->bind[k].pter_next = free_bnode;
      free_bnode = k;
    }
  else
    jt->max_bnode = --b_cnt;    /* case of k==b_cnt */
  return (0);
}
#endif /* CONVERT_by_STROKE */
