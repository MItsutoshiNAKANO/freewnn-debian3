# Generated automatically from Makefile.in by configure.
#
#   $Id: ./Makefile.in $
#

#
# FreeWnn is a network-extensible Kana-to-Kanji conversion system.
# This file is part of FreeWnn.
# 
# Copyright Kyoto University Research Institute for Mathematical Sciences
#                 1987, 1988, 1989, 1990, 1991, 1992
# Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
# Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991, 1992
# Copyright FreeWnn Project 1999, 2000
# 
# Maintainer:  FreeWnn Project   <freewnn@tomo.gr.jp>
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

include ../../makerule.mk

         CC = gcc
  CCOPTIONS = -Dlinux -D_POSIX_SOURCE -D_BSD_SOURCE
CDEBUGFLAGS = -O2 -fno-strength-reduce

LIB_MT_DEFINES = LibraryMTDefines

       INCLUDES = -I$(CWNNINCLUDESRC)
        DEFINES = $(CWNNDEFINES) -DWNNDEFAULT $(CWNNLANGDEF)
       LINTLIBS =

HDRS = rk_extvars.h rk_fundecl.h rk_header.h rk_macros.h rk_spclval.h

SRCS= rk_bltinfn.c 	rk_main.c 	rk_modread.c 	rk_read.c 	rk_vars.c

OBJS= rk_bltinfn.o 	rk_main.o 	rk_modread.o 	rk_read.o 	rk_vars.o

all::

includes::

.c.o:
	$(RM) $@
	$(CC) -c $(CFLAGS) $(_NOOP_) $*.c

clean::

all:: $(HDRS) $(SRCS) $(OBJS)

includes:: $(HDRS) $(SRCS)

rk_extvars.h:  $(WNNROMKANSRC)/rk_extvars.h
	$(RM) $@
	$(LN) $? $@

includes:: rk_extvars.h

depend:: rk_extvars.h

clean::
	$(RM) rk_extvars.h

rk_fundecl.h:  $(WNNROMKANSRC)/rk_fundecl.h
	$(RM) $@
	$(LN) $? $@

includes:: rk_fundecl.h

depend:: rk_fundecl.h

clean::
	$(RM) rk_fundecl.h

rk_header.h:  $(WNNROMKANSRC)/rk_header.h
	$(RM) $@
	$(LN) $? $@

includes:: rk_header.h

depend:: rk_header.h

clean::
	$(RM) rk_header.h

rk_macros.h:  $(WNNROMKANSRC)/rk_macros.h
	$(RM) $@
	$(LN) $? $@

includes:: rk_macros.h

depend:: rk_macros.h

clean::
	$(RM) rk_macros.h

rk_spclval.h:  $(WNNROMKANSRC)/rk_spclval.h
	$(RM) $@
	$(LN) $? $@

includes:: rk_spclval.h

depend:: rk_spclval.h

clean::
	$(RM) rk_spclval.h

rk_bltinfn.c:  $(WNNROMKANSRC)/rk_bltinfn.c
	$(RM) $@
	$(LN) $? $@

includes:: rk_bltinfn.c

depend:: rk_bltinfn.c

clean::
	$(RM) rk_bltinfn.c

rk_main.c:  $(WNNROMKANSRC)/rk_main.c
	$(RM) $@
	$(LN) $? $@

includes:: rk_main.c

depend:: rk_main.c

clean::
	$(RM) rk_main.c

rk_modread.c:  $(WNNROMKANSRC)/rk_modread.c
	$(RM) $@
	$(LN) $? $@

includes:: rk_modread.c

depend:: rk_modread.c

clean::
	$(RM) rk_modread.c

rk_read.c:  $(WNNROMKANSRC)/rk_read.c
	$(RM) $@
	$(LN) $? $@

includes:: rk_read.c

depend:: rk_read.c

clean::
	$(RM) rk_read.c

rk_vars.c:  $(WNNROMKANSRC)/rk_vars.c
	$(RM) $@
	$(LN) $? $@

includes:: rk_vars.c

depend:: rk_vars.c

clean::
	$(RM) rk_vars.c

depend:: .depend

clean::
	$(RM) $(HDRS) $(SRCS)
