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

       INCLUDES = -I$(CWNNINCLUDESRC) -I$(TOP)
        DEFINES = $(CWNNDEFINES) -DWNNDEFAULT -DSUPPORT_TWODIGIT_FUNCTIONS

LINKS = cvt_head.h cvt_key.c cvt_read.c

SRCS= cvt_key.c 	cvt_read.c

OBJS= cvt_key.o 	cvt_read.o

all::

includes::

.c.o:
	$(RM) $@
	$(CC) -c $(CFLAGS) $(_NOOP_) $*.c

clean::

includes:: $(LINKS)

cvt_head.h:  $(WNNCONVSRC)/cvt_head.h
	$(RM) $@
	$(LN) $? $@

includes:: cvt_head.h

depend:: cvt_head.h

clean::
	$(RM) cvt_head.h

cvt_key.c:  $(WNNCONVSRC)/cvt_key.c
	$(RM) $@
	$(LN) $? $@

includes:: cvt_key.c

depend:: cvt_key.c

clean::
	$(RM) cvt_key.c

cvt_read.c:  $(WNNCONVSRC)/cvt_read.c
	$(RM) $@
	$(LN) $? $@

includes:: cvt_read.c

depend:: cvt_read.c

clean::
	$(RM) cvt_read.c

all:: $(LINKS) libconvkey.a

libconvkey.a: $(OBJS) $(EXTRALIBRARYDEPS)
	$(RM) $@
	$(AR) $@ $(OBJS)
	$(RANLIB) $@

lintlib:: llib-lconvkey.ln

llib-lconvkey.ln: $(SRCS) $(EXTRALIBRARYDEPS)
	$(RM) $@
	$(LINT) $(LINTLIBFLAG)convkey $(LINTFLAGS) $(SRCS)

depend:: .depend

lint:
	$(LINT) $(LINTFLAGS) $(SRCS) $(LINTLIBS)
lint1:
	$(LINT) $(LINTFLAGS) $(FILE) $(LINTLIBS)

clean::
	$(RM) $(LINKS)
