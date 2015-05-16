/*
 *  $Id: dispatch.c,v 1.11 2002/05/12 22:51:16 hiroo Exp $
 */

/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright FreeWnn Project 1999, 2000, 2001, 2002
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
 *      Command Dispatch routine
 */

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
#  endif /* HAVE_STRINGS_H */
#endif /* STDC_HEADERS */
#include <sys/types.h>

#include "commonhd.h"
#include "demcom.h"
#include "de_header.h"

extern int cur_clp;

void
do_command ()
{
  register int command;

  wnn_errorno = 0;
  command = get4_cur ();        /* get command */

#if defined(__STDC__)
#define CASE(X) case (X): log_debug(#X "(%d): cur_clp = %d", command, cur_clp);
#else
#define CASE(X) case (X): log_debug("X(%d): cur_clp = %d", command, cur_clp);
#endif

  switch (command)
    {
      CASE (JS_VERSION) put4_cur (JSERVER_VERSION);
      putc_purge ();
      break;

      CASE (JS_OPEN) js_open ();
      break;

      CASE (JS_CLOSE) js_close ();
      break;
/* */
      CASE (JS_CONNECT) js_connect ();
      break;

      CASE (JS_DISCONNECT) js_disconnect ();
      break;

      CASE (JS_ENV_EXIST) js_env_exist ();
      break;

      CASE (JS_ENV_UN_STICKY) js_env_un_sticky ();
      break;

      CASE (JS_ENV_STICKY) js_env_sticky ();
      break;
/* */
      CASE (JS_PARAM_SET) js_param_set ();
      break;

      CASE (JS_PARAM_GET) js_param_get ();
      break;
/* */
      CASE (JS_MKDIR) js_mkdir ();
      break;

      CASE (JS_ACCESS) js_access ();
      break;

      CASE (JS_FILE_STAT) js_file_stat ();
      break;

      CASE (JS_FILE_INFO) js_file_info ();
      break;

      CASE (JS_FILE_LIST_ALL) js_file_list_all ();
      break;

      CASE (JS_FILE_LIST) js_file_list ();
      break;

      CASE (JS_FILE_LOADED) js_file_loaded ();
      break;

      CASE (JS_FILE_LOADED_LOCAL) js_file_loaded_local ();
      break;

      CASE (JS_FILE_READ) js_file_read ();
      break;

      CASE (JS_FILE_WRITE) js_file_write ();
      break;

      CASE (JS_FILE_SEND) js_file_send ();
      break;

      CASE (JS_FILE_RECEIVE) js_file_receive ();
      break;

      CASE (JS_HINDO_FILE_CREATE) js_hindo_file_create ();
      break;

      CASE (JS_DIC_FILE_CREATE) js_dic_file_create ();
      break;

      CASE (JS_FILE_REMOVE) js_file_remove ();
      break;

      CASE (JS_FILE_DISCARD) js_file_discard ();
      break;
      CASE (JS_FILE_COMMENT_SET) js_file_comment_set ();
      break;
      CASE (JS_FILE_PASSWORD_SET) js_file_password_set ();
      break;

/* */
      CASE (JS_DIC_ADD) js_dic_add ();
      break;

      CASE (JS_DIC_DELETE) js_dic_delete ();
      break;

      CASE (JS_DIC_USE) js_dic_use ();
      break;

      CASE (JS_DIC_LIST) js_dic_list ();
      break;

      CASE (JS_DIC_LIST_ALL) js_dic_list_all ();
      break;

      CASE (JS_DIC_INFO) js_dic_info ();
      break;
/* */
      CASE (JS_FUZOKUGO_SET) js_fuzokugo_set ();
      break;

      CASE (JS_FUZOKUGO_GET) js_fuzokugo_get ();
      break;
/* */
      CASE (JS_WORD_ADD) js_word_add ();
      break;

      CASE (JS_WORD_DELETE) js_word_delete ();
      break;

      CASE (JS_WORD_SEARCH) js_word_search ();
      break;

      CASE (JS_WORD_SEARCH_BY_ENV) js_word_search_by_env ();
      break;

      CASE (JS_WORD_INFO) js_word_info ();
      break;
      CASE (JS_WORD_COMMENT_SET) js_word_comment_set ();
      break;
/* */
      CASE (JS_KANREN) do_kanren ();
      putc_purge ();
      break;

      CASE (JS_KANTAN_DAI) do_kantan_dai ();
      putc_purge ();
      break;

      CASE (JS_KANTAN_SHO) do_kantan_sho ();
      putc_purge ();
      break;

      CASE (JS_KANZEN_DAI) do_kanzen_dai ();
      putc_purge ();
      break;

      CASE (JS_KANZEN_SHO) do_kanzen_sho ();
      putc_purge ();
      break;

      CASE (JS_HINDO_SET) js_hindo_set ();
      break;

/* */
      CASE (JS_WHO) js_who ();
      break;

      CASE (JS_ENV_LIST) js_env_list ();
      break;
      CASE (JS_KILL) js_kill ();
      break;
      CASE (JS_HINDO_FILE_CREATE_CLIENT) js_hindo_file_create_client ();
      break;
      CASE (JS_HINSI_LIST) js_hinsi_list ();
      break;
      CASE (JS_HINSI_NAME) js_hinsi_name ();
      break;
      CASE (JS_HINSI_NUMBER) js_hinsi_number ();
      break;
      CASE (JS_HINSI_DICTS) js_hinsi_dicts ();
      break;
      CASE (JS_HINSI_TABLE_SET) js_hinsi_table_set ();
      break;
    default:
      error1 ("unknown command %x", command);
    }
}

/*
        communication routine
 */

char *
get_file_name (buffer, buffer_size)
     char *buffer;
     size_t buffer_size;
{
  buffer = gets_cur (buffer, buffer_size);

  if (!buffer || *buffer == '\0')
    return NULL;

  return expand_file_name (buffer, buffer_size);
}

char *
expand_file_name (buffer, buffer_size)
     char *buffer;
     size_t buffer_size;
{
  char *q;

#ifdef WNN_ALLOW_UNSAFE_PATH
  if (*buffer != '/')
#else
    /* XXX: path under jserver_dir only */
  if (*buffer != '/' 
      || strncmp(buffer, jserver_dir, strlen(jserver_dir)) != 0)
#endif
    {
      char *path;
      size_t path_len;

      path_len = strlen (jserver_dir) + strlen (buffer) + 2;
      if (path_len > buffer_size)
        return NULL;

      path = malloc (path_len);
      if (!path)
        return NULL;

      strcpy (path, jserver_dir);
/*      strcat(path,c_c->user_name);   */
#ifndef WNN_ALLOW_UNSAFE_PATH
      if (path[strlen(path)-1] != '/' && *buffer != '/')
#endif
      strcat (path, "/");
      strcat (path, buffer);
      strcpy (buffer, path);

      free (path);
    }

#ifndef WNN_ALLOW_UNSAFE_PATH
 {
   char *p, *pp, *p0;
   p = pp = p0 = malloc(strlen(buffer)+1);
   for  (q = buffer; *q; p++, q++) {
     if (*q == '/') {
       while (*(q+1) == '/')
	 q++;
       if (strncmp(q, "/..", 3) == 0 && (*(q+3) == '/' || (*(q+3) == '\0'))) {
	 q += 2;
	 p = pp-1;
	 continue;
       }
       pp = p;
     }
     *p = *q;
   }
   *p = '\0';
   strcpy(buffer, p0);
   free(p0);
 }
 if (strncmp(buffer, jserver_dir, strlen(jserver_dir)) != 0) {
   /* unsafe path */
   return NULL; /* XXX */
 }
#endif
  for (q = buffer; *q++;)
    ;
  q -= 2;

  for (;;)
    {
      if (*q != '/')
        break;
      *q-- = '\0';
    }
/*
fprintf(stderr,"file_name=%s\n",p);
*/

  return buffer;
}

void
error_ret ()
{
  extern char *wnn_perror ();

  put4_cur (-1);
  if (wnn_errorno == 0)
    {
      put4_cur (WNN_SOME_ERROR);
    }
  else
    {
      error1 ("Error %s(%d): cur_clp = %d\n", wnn_perror (), wnn_errorno, cur_clp);
      put4_cur (wnn_errorno);
    }
  putc_purge ();
}
