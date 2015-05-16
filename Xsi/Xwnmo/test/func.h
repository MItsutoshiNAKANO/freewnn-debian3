/*
 * $Id: func.h,v 1.2.2.1 1999/02/08 06:02:56 yamasita Exp $
 */

/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
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

/* using functions on xresource.c */
void createxres ();             /* CREATE X RESources */
void mktwin ();                 /* MaKe Test WINdow */
void mkmwin ();                 /* MaKe Menu WINdow */
void mapwin ();                 /* MAP WINdow */
void dstrwin ();                /* Draw STRing on WINdow */
void cwcolor ();                /* Change Window COLOR */
void freexres ();               /* FREE X RESources */

/* using function on chkim.c */
XIM openim ();                  /* check of XOpenIM */
void closeim ();                /* CLOSE IM */
void fclim ();                  /* log File CLose for IM */

/* using functions on chkic.c */
XIC createic ();                /* check of XCreateIC */
XIC onmust ();                  /* for ON-the-spot mode (MUST) */
XIC onmay ();                   /* for ON-the-spot mode (MAY) */
XIC overmust ();                /* for OVER-the-spot mode (MUST) */
XIC overmay ();                 /* for OVER-the-spot mode (MAY) */
XIC offmust ();                 /* for OFF-the-spot mode (MUST) */
XIC offmay ();                  /* for OFF-the-spot mode (MAY) */
XIC rootmust ();                /* for ROOT-window mode (MUST) */
XIC rootmay ();                 /* for ROOT-window mode (MAY) */
void destic ();                 /* DESTroy IC */
void fclic ();                  /* log File Close for IC */

/* using function on chkutil.c */
void chkutil ();                /* CHecK UTILity functions */

/* using function on keyev.c */
FLAG keyev ();                  /* transaction of KEY EVent */

/* using function on chkfe.c */
FLAG fltev ();                  /* check of FiLTer EVent */

/* using function on chkxls.c */
void xlstr ();                  /* check of X*LookupSTRing */

/* using functions on chkcset.c */
FLAG xsuicf ();                 /* check of XSet/UnsetICFocus */
FLAG sfocus ();                 /* Set FOCUS window */
FLAG sresource ();              /* Set RESOURCE name/class */

/* using functions on chkpsset.c */
FLAG sarea ();                  /* Set AREA */
FLAG sneeded ();                /* Set area-NEEDED */
FLAG sspot ();                  /* Set SPOTlocation */
FLAG scmap ();                  /* Set ColorMAP */
FLAG sground ();                /* Set fore/backGROUND */
FLAG spixmap ();                /* Set bg/PIXMAP */
FLAG sfontset ();               /* Set FONTSET */
FLAG slinespc ();               /* Set LINESPaCing */
FLAG scursor ();                /* Set CURSOR */

/* using function on callback.c */
void geometry ();
int pe_start ();
void pe_done ();
void pe_draw ();
void pe_caret ();
void st_start ();
void st_done ();
void st_draw ();
void wlogcb ();
void wlogtext ();
void chgcaretval ();
int trnpebuf ();

/* using function on chkreset.c */
FLAG preset ();                 /* Preedit area RESET */

/* using function on final.c */
void dtictoex ();               /* DesTroy IC TO EXit */
void dticonly ();               /* DesTroy IC ONLY */
void dtictofc ();               /* DesTroy IC TO File Close */
void fcictoex ();               /* log File Close for IC TO EXit */
void climtoex ();               /* CLose IM TO EXit */
void fcimtoex ();               /* log File Close for IM TO EXit */

/* using functions on curses.c */
void initmwin ();               /* INITialization of Main WINdow */
void prtitle ();                /* PRint TITLE */
u_short psmenu ();              /* Print and Selest main MENU */
void prprint ();                /* PRint string on PRINT part */
void prerrfunc ();              /* PRint ERRor message for FUNCtion */
void prerrld ();                /* PRe-ERRor for Log file Directory */
void prveres ();                /* PRint VErify RESult */
void prverlog ();               /* PRint VErify result for LOG file */
void prtorm ();                 /* PRint string of Title OR Menu on print part */
void prmode ();                 /* PRint string on MODE part */
int outchar ();                 /* OUTput CHARacter */
void prstatus ();               /* PRint string on STATUS part */
void prpok ();                  /* PRint Prompt for OK */
void prpng ();                  /* PRint Prompt for No Good */
void cls ();                    /* CLear Screen */
void freemwin ();               /* FREE Main WINdow */
void prcaret ();                /* PRint caret */

/* using functions on funcv.c */
VALUABLE *mkstruct ();          /* MaKe STRUCT valuable */
FLAG verval ();                 /* VERify VALuable */
