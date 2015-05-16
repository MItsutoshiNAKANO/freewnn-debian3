/*
 * $Id: ext.h,v 1.2 2001/06/14 18:16:15 ura Exp $
 */

/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
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
extern int t_rubout ();
extern int delete_char1 ();
extern int t_delete_char ();
extern int kuten ();
extern int jis ();
extern int input_a_char_from_function ();
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
extern int reset_line ();
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

/*
 * byte_swap.c
 */
unsigned long byteswap_l ();
unsigned short byteswap_s ();
void byteswap_xevent ();
void byteswap_GetIMReply ();
void byteswap_CreateICReply ();
void byteswap_NormalReply ();
void byteswap_GetICReply ();
void byteswap_ICValuesReq ();
void byteswap_ICAttributesReq ();
void byteswap_EventReply ();
void byteswap_ReturnReply ();
#ifdef  CALLBACKS
void byteswap_StatusDrawReply ();
void byteswap_PreDrawReply ();
void byteswap_PreCaretReply ();
#endif /* CALLBACKS */

/*
 * change.c
 */
extern void reset_preedit ();
extern void reset_status ();
extern unsigned long update_ic ();
extern unsigned long update_spotlocation ();
extern int change_current_language ();
extern void change_client_area ();
extern void change_focus_area ();
extern int change_client ();
#ifdef  SPOT
extern int change_spotlocation ();
#endif /* SPOT */

/*
 * client.c
 */
extern int new_client ();
extern void del_client ();
extern void epilogue ();
extern char env_state ();
extern int set_cur_env ();
extern int get_new_env ();
extern void free_env ();

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
extern void redraw_window3 ();
extern void redraw_lines ();
extern void redraw_note ();
extern void redraw_window0 ();
extern void redraw_xj_all ();
extern void visual_window1 ();
extern void visual_window ();
extern void invisual_window1 ();
extern void invisual_window ();
extern void JWMflushw_buf ();
extern void jw_disp_mode ();
extern void JWMline_clear ();
extern void JWMline_clear1 ();
extern void visual_status ();
extern void invisual_status ();
extern void JWcursor_visible ();
extern void JWcursor_invisible ();
extern void JWcursor_move ();
extern void check_scroll ();
extern void check_root_mapping ();
extern void visible_root ();
extern void invisible_root ();

/*
 * do_socket.c
 */
extern int _Send_Flush ();
extern int _WriteToClient ();
extern int _ReadFromClient ();
extern short init_net ();
extern void close_net ();
extern int wait_for_socket ();
extern void close_socket ();
extern int get_cur_sock ();
extern int get_rest_len ();
extern int read_requestheader ();
extern int read_strings ();
extern Bool need_byteswap ();
extern int get_client_screen ();

/*
 * do_xjutil.c
 */
#ifdef  USING_XJUTIL
extern void xjutil_start ();
extern void kill_xjutil ();
extern void kill_all_xjutil ();
extern void set_xjutil_id ();
extern void reset_xjutil_fs_id ();
extern int isstart_xjutil ();
extern void end_xjutil_work ();
extern int xjutil_destroy ();
extern void xjutil_send_key_event ();
extern void xw_jishoadd ();
extern void xw_kensaku ();
extern void xw_select_one_dict9 ();
extern void xw_paramchg ();
extern void xw_dicinfoout ();
extern void xw_fuzoku_set ();
extern void xw_touroku ();
extern void call_kill_old_xjutil ();
#endif /* USING_XJUTIL */

/*
 * ev_dispatch.c
 */
extern void X_flush ();
extern void XEventDispatch ();
extern int get_yes_no_event ();

/*
 * functions.c
 */
extern int bytcntfn ();
extern void conv_ltr_to_ieuc ();
/*
extern int letterpickfn();
*/
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
#ifdef CHINESE
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
 * history.c
 */
extern int init_history ();
extern int make_history ();
extern void get_end_of_history ();
extern int previous_history1 ();
extern int next_history1 ();
extern void destroy_history ();

/*
 * ichiran.c
 */
extern void JW3Mputc ();
extern void set_wm_properties ();
extern int init_ichiran ();
extern void end_ichiran ();
extern void check_map ();
extern void lock_inspect ();
extern void unlock_inspect ();
extern int init_inspect ();
extern void end_inspect ();
extern int xw_next_inspect ();
extern int xw_back_inspect ();
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
extern int set_j_c ();
extern int cur_cl_change3 ();
extern int cur_cl_change4 ();
extern void xw_select_button ();
extern void xw_select_jikouho_button ();
extern void xw_mouseleave ();
extern int xw_mouse_select ();
extern void draw_nyuu_w ();
extern void clear_nyuu_w ();
extern void nyuu_w_cursor ();
extern int yes_or_no_lock ();
extern int yes_or_no ();

/*
 * init.c
 */
extern void add_ximclientlist ();
extern void remove_ximclientlist ();
extern void add_inputlist ();
extern void remove_inputlist ();
extern int allocate_wnn ();
extern void free_wnn ();
extern int initialize_wnn ();

/*
 * init_w.c
 */
extern int get_application_resources ();
extern int create_yes_no ();
extern void read_wm_id ();
extern int create_xim_window ();
extern int create_xim ();
extern void xw_end ();
extern int create_preedit ();
extern int create_status ();

/*
 * inspect.c
 */
extern int inspect ();
extern int inspect_kouho ();
extern int sakujo_kouho ();
extern int inspectdel ();
extern int inspectuse ();
extern unsigned char *next_inspect ();
extern unsigned char *previous_inspect ();

/*
 * jhlp.c
 */
extern void do_end ();
extern void in_put ();
extern void main ();

/*
 * jikouho.c
 */
extern int dai_top ();
extern int dai_end ();
extern void set_escape_code ();
extern int jikouho_c ();
extern int zenkouho_c ();
extern int jikouho_dai_c ();
extern int zenkouho_dai_c ();
extern int select_jikouho ();
extern int select_jikouho_dai ();
#ifdef  CONVERT_by_STROKE
extern int select_question ();
#endif /* CONVERT_by_STROKE */

/*
 * jis_in.c
 */
extern int in_jis ();

/*
 * jutil.c
 */
extern int jutil ();
extern int dicsv ();
#ifdef  USING_XJUTIL
extern int paramchg ();
extern int dicinfoout ();
extern int select_one_dict9 ();
extern int fuzoku_set ();
extern int jishoadd ();
extern int kensaku ();
#endif /* USING_XJUTIL */

/*
 * key_bind.c
 */
extern int init_key_table ();

/*
 * keyin.c
 */
extern int key_input ();
int ifempty ();
extern int RequestDispatch ();
/*
 * kuten.c
 */
extern int in_kuten ();

/*
 * localalloc.c
 */
extern char *Malloc ();
extern char *Realloc ();
extern char *Calloc ();
extern void Free ();
extern char *alloc_and_copy ();

/*
 * multi_lang.c
 */
extern char *get_default_font_name ();
extern int load_font_set ();
extern int add_locale_to_xl ();
extern void remove_locale_to_xl ();
extern int add_lang_env ();
extern void default_xc_set ();
extern XIMClientRec *create_client ();
extern int lang_set ();
extern int lang_set_ct ();
extern void change_lang ();

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
extern int allocate_areas ();
extern void free_areas ();
extern int connect_server ();
extern int init_xcvtkey ();

/*
 * readximrc.c
 */
int read_ximrc ();
int read_ximconf ();

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

/*
 * select_ele.c
 */
extern int xw_select_one_element ();
/*
extern int xw_select_one_jikouho();
*/
extern int forward_select ();
extern int backward_select ();
extern int lineend_select ();
extern int linestart_select ();
extern int select_select ();
extern int quit_select ();
extern int previous_select ();
extern int next_select ();
extern int redraw_select ();

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
extern int touroku ();

/*
 * uif.c
 */
extern void push_func ();
extern void pop_func ();
extern int insert_char_and_change_to_insert_mode ();
extern int kakutei ();
extern int return_it ();
extern int return_it_if_ascii ();
extern int redraw_nisemono_c ();
extern int kk ();
extern int insert_modep ();
extern int empty_modep ();
extern void clear_c_b ();
extern void make_kanji_buffer ();
extern int isconect_jserver ();
extern int ren_henkan ();
extern int kankana_ren_henkan ();
#ifdef  CONVERT_by_STROKE
extern int errorkaijo ();
#endif /* CONVERT_by_STROKE */
extern int tan_henkan ();
extern int tan_henkan_dai ();
extern int nobi_henkan ();
extern int nobi_henkan_dai ();
extern void henkan_if_maru ();
#ifdef  CONVERT_by_STROKE
extern int question_henkan ();
#endif /* CONVERT_by_STROKE */
extern int yank_c ();
extern int remember_me ();
extern int kill_c ();
#ifdef  CONVERT_by_STROKE
extern int errorkill_c ();
#endif /* CONVERT_by_STROKE */
extern int delete_c ();
extern int rubout_c ();
extern int end_bunsetsu ();
extern int top_bunsetsu ();
extern int forward_bunsetsu ();
extern int backward_bunsetsu ();
extern int kaijo ();
extern int enlarge_bunsetsu ();
extern int smallen_bunsetsu ();
extern int send_string ();
extern int tijime ();
extern int jmptijime ();
extern int henkan_forward ();
extern int henkan_backward ();
extern int backward_c ();
extern int insert_it_as_yomi ();
extern int previous_history ();
extern int next_history ();
extern int send_ascii ();
extern int not_send_ascii ();
extern int toggle_send_ascii ();
extern int pop_send_ascii ();
extern int send_ascii_e ();
extern int not_send_ascii_e ();
extern int toggle_send_ascii_e ();
extern int pop_send_ascii_e ();
extern int quote_send_ascii_e ();
extern int reconnect_server ();
extern int disconnect_server ();
extern int henkan_off ();
extern int reset_c_b ();
extern void errorkeyin_q ();

/*
 * uif1.c
 */
extern int jutil_c ();
extern int touroku_c ();
extern int reconnect_jserver_body ();
extern int lang_c ();
extern int push_unget_buf ();
extern unsigned int *get_unget_buf ();
extern int if_unget_buf ();

/*
 * util.c
 */
extern void GetIM ();
extern Status have_world ();
extern void CreateIC ();
extern void GetIC ();
extern void SetICFocus ();
extern void UnsetICFocus ();
extern void ChangeIC ();
extern void DestroyIC ();
extern void destroy_for_sock ();
extern void destroy_xl ();
extern void destroy_client ();
extern void ResetIC ();
extern void free_langlist ();
extern int get_langlist ();
#ifdef  SPOT
extern void ChangeSpot ();
#endif /* SPOT */

/*
 * w_string.c
 */
extern int wchartochar ();
extern int sStrcpy ();
extern w_char *Strcat ();
extern w_char *Strncat ();
extern int Strncmp ();
extern w_char *Strcpy ();
extern w_char *Strncpy ();
extern int Strlen ();
extern void delete_w_ss2 ();
extern int byte_count ();

/*
 * wnnrc_op.c
 */
extern int expand_expr ();
extern int uumrc_get_entries ();
extern int read_default_rk ();

/*
 * write.c
 */
extern int return_eventreply ();
extern int send_nofilter ();
extern int send_end ();
extern int return_cl_it ();
extern void xw_write ();

/*
 * xcvtkey.c
 */
extern int comment_char ();
extern int cvt_key_setup ();
extern int cvt_key_fun ();
extern int cvt_meta_and_fun_setup ();
/*
 * xlc_util.c
 */
extern int alloc_all_buf ();
extern int realloc_wc_buf ();
extern int realloc_ct_buf ();
extern int realloc_c_buf ();
#ifndef X_WCHAR
extern int realloc_w_buf ();
#endif /* !X_WCHAR */
extern int get_columns_wchar ();
extern int XwcGetColumn ();
extern int check_mb ();
extern int w_char_to_char ();
extern int skip_pending_wchar ();
extern int put_pending_wchar_and_flg ();
extern int char_to_wchar ();
extern int w_char_to_wchar ();
extern int w_char_to_ct ();
extern int wchar_to_ct ();
extern void JWOutput ();
extern XCharStruct *get_base_char ();
extern XFontSet create_font_set ();
#ifdef  CALLBACKS
extern int XwcGetChars ();

/*
 * For callback.c
 */
extern void CBStatusDraw ();
extern void CBStatusStart ();
extern void CBStatusDone ();
extern void CBPreeditDraw ();
extern void CBPreeditStart ();
extern void CBPreeditDone ();
extern void CBPreeditRedraw ();
extern void CBPreeditClear ();
extern void CBCursorMove ();
extern void CBCursorMark ();
extern void SendCBRedraw ();
#endif /* CALLBACKS */

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
extern unsigned int create_cswidth ();
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
 * Wnn jllib functions
 */
extern int jl_isconnect_e ();
extern int jl_dic_save_all_e ();
extern void jl_close ();
extern int jl_dic_list_e ();
extern int wnn_get_area ();
extern int jl_kanji_len ();
extern int jl_yomi_len ();
extern int jl_word_delete_e ();
extern int jl_word_use_e ();
extern int jl_zenkouho ();
extern int jl_set_jikouho ();
extern void jl_env_set ();
extern int jl_zenkouho_dai ();
extern int jl_set_jikouho_dai ();
extern void jl_get_zenkouho_kanji ();
extern void jl_get_zenkouho_yomi ();
extern int jl_zenkouho_q ();
extern int js_env_sticky ();
extern int jl_kill ();
extern int jl_update_hindo ();
extern int jl_ren_conv ();
extern int jl_tan_conv ();
extern int jl_nobi_conv ();
extern int jl_nobi_conv_e2 ();
extern int jl_q_conv ();
extern void jl_disconnect ();

#ifdef  CHINESE
/*
 * yincoding functions
  */
extern int cwnn_pzy_yincod ();
extern int cwnn_yincod_pzy_str ();
#endif /* CHINESE */

#ifdef  XJPLIB
extern void XJp_init ();
extern void XJp_end ();
extern void XJp_xjp_to_xim ();
extern void XJp_event_dispatch ();
extern XIMClientRec *XJp_cur_cl_set ();
extern int XJp_check_cur_input ();
extern void XJp_check_send_cl_key ();
extern void XJp_check_save_event ();
extern void XJp_return_cl_it ();
extern void XJp_xw_write ();
extern int XJp_xw_destroy ();
extern int XJp_check_map ();
extern int XJp_get_client_cnt ();
#ifdef  XJPLIB_DIRECT
extern int XJp_get_xjp_port ();
extern void XJp_init_net ();
extern void XJp_close_net ();
extern int XJp_return_sock ();
extern int XJp_wait_for_socket ();
extern void XJp_destroy_for_sock ();
extern void XJp_Direct_Dispatch ();
extern void XJp_save_sockbuf ();
extern void XJp_direct_send_cl_key ();
#endif /* XJPLIB_DIRECT */
#endif /* XJPLIB */

#ifndef X11R5

extern void XimError (
#if NeedFunctionPrototypes
                       int      /* fd */
#endif
  );

extern void XimRequestDispatch (
#if NeedFunctionPrototypes
                                 void
#endif
  );

extern void XimConvertTarget (
#if NeedFunctionPrototypes
                               Display * /* dpy */ ,
                               XSelectionRequestEvent * /* event */
#endif
  );

extern int XimPreConnect (
#if NeedFunctionPrototypes
                           unsigned short /* port */ ,
                           XIMRootRec * /* root */
#endif
  );

extern int xim_send_keysym (
#if NeedFunctionPrototypes
                             KeySym /* keysym */ ,
                             char       /* str */
#endif
  );

extern int xim_send_ct (
#if NeedFunctionPrototypes
                         register char * /* ct */ ,
                         register int   /* ct_len */
#endif
  );

extern int xim_send_nofilter (
#if NeedFunctionPrototypes
                               void
#endif
  );

#ifdef CALLBACKS
extern void xim_send_preeditstart ();
extern void xim_send_preeditdraw ();
extern void xim_send_preeditcaret ();
extern void xim_send_preeditdone ();
extern void xim_send_statusstart ();
extern void xim_send_statusdraw ();
extern void xim_send_statusdone ();
#endif /* CALLBACKS */

#endif /* !X11R5 */
