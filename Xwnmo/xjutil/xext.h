/*
 * $Id: xext.h,v 1.2 2001/06/14 18:16:13 ura Exp $
 */
/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright 1991, 1992 by Massachusetts Institute of Technology
 *
 * Author: OMRON SOFTWARE Co., Ltd. <freewnn@rd.kyoto.omronsoft.co.jp>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Emacs; see the file COPYING.  If not, write to the
 * Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * Commentary:
 *
 * Change log:
 *
 * Last modified date: 8,Feb.1999
 *
 * Code:
 *
 */
/*      Version 4.0
 */
/*
 * basic_op.c
 */
extern int redraw_nisemono ();
extern int buffer_in ();
extern int kana_in ();
extern int delete_char1 ();
extern int t_delete_char ();
extern int kuten ();
extern int jis ();
extern int input_a_char_from_function ();
extern int t_rubout ();
extern int t_kill ();
#ifdef  CONVERT_by_STROKE
extern int errort_kill ();
#endif /*  CONVERT_by_STROKE */
extern int t_yank ();
extern int t_ret ();
extern int t_quit ();
extern int c_top ();
extern int c_end ();
extern int c_end_nobi ();
extern int t_jmp_backward ();
extern int t_jmp_forward ();
extern int forward_char ();
extern int backward_char ();
extern int forward ();
extern int backward ();
extern int insert_char ();
extern int insert_string ();
extern int quote ();
extern void redraw_line ();

/*
 * box.c
 */
extern BoxRec *create_box ();
extern int remove_box ();
extern void map_box ();
extern void unmap_box ();
extern void freeze_box ();
extern void unfreeze_box ();
extern void moveresize_box ();
extern void changecolor_box ();
extern void redraw_box ();
extern void reverse_box ();
extern void change_reverse_box ();

/*
 * cursor.c
 */
extern void throw_col ();
extern void h_r_on ();
extern void h_r_off ();
extern void u_s_on ();
extern void u_s_off ();
extern void kk_cursor_invisible ();
extern void kk_cursor_normal ();
extern void reset_cursor_status ();
extern void set_cursor_status ();
extern void clr_line_all ();
extern void push_hrus ();
extern void pop_hrus ();
extern void set_hanten_ul ();
extern void set_bold ();
extern void reset_bold ();

/*
 * display.c
 */
extern void JWmark_cursor ();
extern void JW1Mputwc ();
extern void JWMflushw_buf ();
extern void jw_disp_mode ();
extern void JWMline_clear ();
extern void JWcursor_visible ();
extern void JWcursor_invisible ();
extern void JWcursor_move ();
extern void redraw_text ();
extern void redraw_note ();
extern void check_scroll ();

/*
 * functions.c
 */
extern int bytcntfn ();
extern void conv_ltr_to_ieuc ();
extern int call_t_redraw_move_normal ();
extern int call_t_redraw_move ();
extern int call_t_redraw_move_1_normal ();
extern int call_t_redraw_move_1 ();
extern int call_t_redraw_move_2_normal ();
extern int call_t_redraw_move_2 ();
extern int call_t_print_l_normal ();
extern int call_t_print_l ();
extern int c_top_normal ();
extern int c_end_normal ();
extern int c_end_nobi_normal ();
extern int char_q_len_normal ();
extern int char_len_normal ();
extern int t_redraw_move_normal ();
extern int t_print_l_normal ();
extern int call_jl_yomi_len ();
#ifdef  CHINESE
extern int call_t_redraw_move_yincod ();
extern int call_t_redraw_move_1_yincod ();
extern int call_t_redraw_move_2_yincod ();
extern int call_t_print_l_yincod ();
extern int input_yincod ();
extern int redraw_when_chmsig_yincod ();
extern int c_top_yincod ();
extern int c_end_yincod ();
extern int c_end_nobi_yincod ();
extern int print_out_yincod ();
extern int char_q_len_yincod ();
extern int char_len_yincod ();
extern int t_redraw_move_yincod ();
extern int t_print_l_yincod ();
extern int not_call_jl_yomi_len ();
#endif /* CHINESE */

/*
 * hinsi.c
 */
extern int get_default_hinsi ();
extern int hinsi_in ();

/*
 * ichiran.c
 */
extern void JW3Mputc ();
extern void set_wm_properties ();
extern void decide_position ();
extern int init_ichiran ();
extern void end_ichiran ();
extern void lock_ichiran ();
extern void unlock_ichiran ();
extern void xw_move_hilite ();
extern void xw_jikouho_move_hilite ();
extern int alloc_for_save_buf ();
extern void redraw_ichi_w ();
extern int next_ichi ();
extern int back_ichi ();
extern int find_hilite ();
extern int find_ji_hilite ();
extern void xw_forward_select ();
extern void xw_backward_select ();
extern int xw_next_select ();
extern int xw_previous_select ();
extern void xw_linestart_select ();
extern void xw_lineend_select ();
extern int init_yes_or_no ();
extern void end_yes_or_no ();
extern void xw_select_button ();
extern void xw_select_jikouho_button ();
extern void xw_mouseleave ();
extern int xw_mouse_select ();
extern void draw_nyuu_w ();
extern void clear_nyuu_w ();
extern void change_ichi_buf ();
extern void insert_space_in_ichi_buf ();

/*
 * init_w.c
 */
extern char env_state ();
extern int set_cur_env ();
extern void get_new_env ();
extern void return_error ();
extern void read_wm_id ();
extern int create_text ();
extern int create_jutil ();
extern int create_ichi ();
extern int create_yes_no ();
extern void xw_end ();
extern XjutilFSRec *add_fontset_list ();
extern XjutilFSRec *get_fontset_list ();
extern int create_xjutil ();
extern void send_end_work ();

/*
 * jhlp.c
 */
extern int keyin ();
extern void terminate_handler ();
extern void main ();

/*
 * jutil.c
 */
extern int sStrcpy_and_ck ();
extern void if_dead_disconnect ();
extern int yes_or_no ();
extern void paramchg ();
extern int update_dic_list ();
extern void dic_nickname ();
extern int find_dic_by_no ();
extern void dicinfoout ();
extern void select_one_dict9 ();
extern void fuzoku_set ();
extern void jishoadd ();
extern int dic_delete_e ();

/*
 * kensaku.c
 */
extern void fill_space ();
extern void kensaku ();

/*
 * key_bind.c
 */
extern int init_key_table ();

/*
 * keyin.c
 */
extern int get_env ();
extern int get_touroku_data ();
extern int xw_read ();

/*
 * localalloc.c
 */
extern char *Malloc ();
extern char *Realloc ();
extern char *Calloc ();
extern void Free ();
extern char *alloc_and_copy ();

/*
 * printf.c
 */
extern int char_q_len ();
extern void put_char ();
extern void flushw_buf ();
extern int w_putchar ();
extern void put_char1 ();
extern void putchar_norm ();
extern void errorkeyin ();
#ifdef  CONVERT_by_STROKE
void errorkeyin_q ();
#endif /* CONVERT_by_STROKE */
extern void malloc_error ();
extern void print_out7 ();
extern void print_out3 ();
extern void print_out2 ();
extern void print_out1 ();
extern void print_out ();
extern void print_msg_getc ();
extern void print_msg_wait ();

/*
 * prologue.c
 */
extern int open_romkan ();
extern int init_wnn ();
extern int connect_server ();
extern int init_xcvtkey ();

/*
 * screen.c
 */
extern void throw ();
extern int char_len ();
extern int cur_ichi ();
extern void t_print_line ();
extern void t_redraw_one_line ();
extern void init_screen ();
extern void t_redraw_move ();
extern void t_move ();
extern void t_print_l ();
extern char *get_rk_modes ();
extern void disp_mode ();
extern void display_henkan_off_mode ();
extern int char_len_normal ();

/*
 * xselectele.c
 */
extern int xw_select_one_element_call ();
extern int xw_select_one_element_keytable ();
extern int kdicdel ();
extern int kdicuse ();
extern int kdiccom ();
extern int kworddel ();
extern int kworduse ();
extern int kwordcom ();
extern int kwordhindo ();
extern int kwordima ();
extern int kworddel_op ();

/*
 * termio.c
 */
extern void clr_end_screen ();
extern void throw_cur_raw ();
extern void h_r_on_raw ();
extern void h_r_off_raw ();
extern void u_s_on_raw ();
extern void u_s_off_raw ();
extern void b_s_on_raw ();
extern void b_s_off_raw ();
extern void ring_bell ();
extern void cursor_invisible_raw ();
extern void cursor_normal_raw ();

/*
 * touroku.c
 */
extern void touroku ();
extern int hani_settei_normal ();
extern int hani_settei_yincod ();
extern int t_markset ();
extern int xw_previous_message ();
extern int xw_next_message ();

/*
 * uif.c
 */
extern int empty_modep ();
extern int insert_modep ();
extern int redraw_nisemono_c ();
extern int isconect_jserver ();
extern int kill_c ();
#ifdef  CONVERT_by_STROKE
extern int errorkill_c ();
#endif /* CONVERT_by_STROKE */
extern int delete_c ();
extern int rubout_c ();
extern int backward_c ();
extern int reconnect_server ();
extern int disconnect_server ();
extern int henkan_off ();
extern int reset_c_b ();

/*
 * uif1.c
 */
extern int jutil_c ();
extern int touroku_c ();
extern int reconnect_jserver_body ();
extern int lang_c ();
extern int push_unget_buf ();
extern int *get_unget_buf ();
extern int if_unget_buf ();

/*
 * w_string.c
 */
extern int wchartochar ();
extern int sStrcpy ();
extern int Sstrcpy ();
extern w_char *Strcat ();
extern w_char *Strncat ();
extern int Strncmp ();
extern w_char *Strcpy ();
extern w_char *Strncpy ();
extern int Strlen ();
extern void delete_ss2 ();
extern void delete_w_ss2 ();
extern int byte_count ();

/*
 * wnnrc_op.c
 */
extern int expand_expr ();
extern void uumrc_get_entries ();

/*
 * xcvtkey.c
 */
extern int comment_char ();
extern int cvt_key_setup ();
extern int cvt_meta_and_fun_setup ();
extern int cvt_key_fun ();

/*
 * xlc_util.c
 */
extern int alloc_all_buf ();
extern int realloc_wc_buf ();
extern int XwcGetColumn ();
extern int check_mb ();
extern int w_char_to_char ();
extern int skip_pending_wchar ();
extern int char_to_wchar ();
extern int w_char_to_wchar ();
extern int w_char_to_ct ();
extern void JWOutput ();
extern XCharStruct *get_base_char ();
extern XFontSet create_font_set ();

/*
 * xw_touroku.c
 */
extern void xw_jutil_write_msg ();
extern void init_jutil ();
extern void end_jutil ();
extern void change_cur_jutil ();
extern void hanten_jutil_mes_title ();
extern void change_mes_title ();

/*
 * xichiran.c
 */
extern void resize_text ();
extern void xw_mousemove ();
extern void init_keytable ();
extern void end_keytable ();
extern void xw_expose ();
extern Status xw_buttonpress ();
extern int jutil_mode_set ();
extern void xw_enterleave ();

/*
 * kuten.c
 */
extern int in_kuten ();

/*
 * jis_in.c
 */
extern int in_jis ();

/*
 * select_ele.c
 */
extern int xw_select_one_element ();
extern int forward_select ();
extern int backward_select ();
extern int lineend_select ();
extern int linestart_select ();
extern int select_select ();
extern int quit_select ();

/*
 * For etc
 */
#include "wnn_string.h"
#ifdef  JAPANESE
extern int iujis_to_eujis ();
#endif /* JAPANESE */
#ifdef  CHINESE
extern int icns_to_ecns ();
extern int iugb_to_eugb ();
#endif /* CHINESE */
extern int through ();
extern int ibit8_to_ebit8 ();
extern int ieuc_to_eeuc ();
extern int eeuc_to_ieuc ();
extern int columnlen ();
extern void set_cswidth ();
extern int get_cswidth_by_char ();
extern int get_cswidth ();
extern int get_cs_mask ();

/*
 * Standard functions
 */
extern int fclose ();
extern int fflush ();
extern int atoi ();
extern void bcopy ();

/*
 * Wnn jslib functions
 */
extern int js_hinsi_list ();
extern int js_hinsi_number ();
extern int js_dic_list ();
extern int js_fuzokugo_get ();
extern int js_file_send ();
extern int js_file_read ();
extern int js_fuzokugo_set ();
extern int js_access ();
extern int js_mkdir ();
extern int js_hindo_file_create_client ();
extern int js_hindo_file_create ();
extern int js_dic_file_create_client ();
extern int js_dic_file_create ();
extern int js_file_remove_client ();
extern int js_file_remove ();
extern int js_file_discard ();
extern int js_dic_add ();
extern int js_dic_info ();
extern int js_dic_delete ();
extern int js_word_search_by_env ();
extern int js_hinsi_name ();
extern int js_dic_use ();
extern int js_file_info ();
extern int js_file_comment_set ();
extern int js_word_delete ();
extern int js_word_info ();
extern int js_hindo_set ();
extern int js_word_comment_set ();
extern int js_word_add ();
extern int js_isconnect ();
extern char *wnn_perror_lang ();

#ifdef  CHINESE
/*
 * yincoding functions
 */
extern int cwnn_pzy_yincod ();
extern int cwnn_yincod_pzy_str ();
#endif /* CHINESE */
