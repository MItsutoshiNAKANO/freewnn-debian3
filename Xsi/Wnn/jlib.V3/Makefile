# Generated automatically from Makefile.in by configure.
#
#   $Id: Makefile.in $
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

INSTALL      = $/usr/bin/install -c

LIB_MT_DEFINES = LibraryMTDefines

       INCLUDES = -I$(WNNINCLUDESRC)
        DEFINES = $(WNNDEFINES) $(WNNLANGDEF)
       LINTLIBS =

SRC1= jlv3.c
OBJ1= jlv3.o
SRC2= $(WNNETCSRC)/gethinsi.c
OBJ2= gethinsi.o
SRC3= $(WNNETCSRC)/sstrings.c
OBJ3= sstrings.o
SRC4= $(WNNETCSRC)/xutoj.c
OBJ4= xutoj.o
SRCS= $(SRC1) $(SRC2) $(SRC3) $(SRC4)
OBJS= $(OBJ1) $(OBJ2) $(OBJ3) $(OBJ4)

all::

includes::

.c.o:
	$(RM) $@
	$(CC) -c $(CFLAGS) $(_NOOP_) $*.c

clean::

all:: libjd.a

libjd.a: $(OBJS) $(WNNJLIB)
	cp $(WNNJLIB) $@
	ar rv $@ $(OBJS)
	$(RANLIB) $@

lintlib:: llib-ljd.ln

llib-ljd.ln: $(SRCS) $(EXTRALIBRARYDEPS)
	$(RM) $@
	$(LINT) $(LINTLIBFLAG)jd $(LINTFLAGS) $(SRCS)

install:: libjd.a
	@if [ -d $(DESTDIR)$(WNNLIBDIR) ]; then set +x; \
	else (set -x; $(MKDIRHIER) $(DESTDIR)$(WNNLIBDIR)); fi
	$(INSTALL) $(INSTALLFLAGS) $(INSTLIBFLAGS) libjd.a $(DESTDIR)$(WNNLIBDIR)
	$(RANLIB) $(RANLIBINSTFLAGS) $(DESTDIR)$(WNNLIBDIR)/libjd.a

install:: jlib.h
	@if [ -d $(DESTDIR)$(JWNNINCDIR) ]; then set +x; \
	else (set -x; $(MKDIRHIER) $(DESTDIR)$(JWNNINCDIR)); fi
	$(INSTALL) $(INSTALLFLAGS) $(INSTDATFLAGS) jlib.h $(DESTDIR)$(JWNNINCDIR)/jlib.h

lint:
	$(LINT) $(LINTFLAGS) $(SRCS) $(LINTLIBS)
lint1:
	$(LINT) $(LINTFLAGS) $(FILE) $(LINTLIBS)

$(WNNJLIB)::
	@for flag in ${MAKEFLAGS} ''; do \
	case "$$flag" in *=*) ;; *[ik]*) set +e;; esac; done; \
	for i in $(WNNJLIBSRC) ;\
	do \
	echo "making" $(WNNJLIB) "in $(CURRENT_DIR)/$$i..."; \
	(cd $$i && $(MAKE) $(MFLAGS) $(PARALLELMFLAGS) \
	'CDEBUGFLAGS=$(CDEBUGFLAGS)' all); \
	done

$(OBJ2) : $(SRC2)
	$(CC) -c $(CFLAGS)  $(WNNETCSRC)/`basename $@ .o`.c

$(OBJ3) : $(SRC3)
	$(CC) -c $(CFLAGS)  $(WNNETCSRC)/`basename $@ .o`.c

$(OBJ4) : $(SRC4)
	$(CC) -c $(CFLAGS)  $(WNNETCSRC)/`basename $@ .o`.c

depend:: .depend

