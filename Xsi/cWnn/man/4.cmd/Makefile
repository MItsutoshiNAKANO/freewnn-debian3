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

include ../../../makerule.mk

MANSUFFIX = 1

INSTALL      = $/usr/bin/install -c

install.man:: cserver.man
	@if [ -d $(DESTDIR)$(MANDIR) ]; then set +x; \
	else (set -x; $(MKDIRHIER) $(DESTDIR)$(MANDIR)); fi
	$(RM) $(DESTDIR)$(MANDIR)/cserver.$(MANSUFFIX)*
	$(INSTALL) -c $(INSTMANFLAGS) cserver.man \
 $(DESTDIR)$(MANDIR)/cserver.$(MANSUFFIX)
	-$(COMPRESSMANCMD) $(DESTDIR)$(MANDIR)/cserver.$(MANSUFFIX)

install.man:: cuum.man
	@if [ -d $(DESTDIR)$(MANDIR) ]; then set +x; \
	else (set -x; $(MKDIRHIER) $(DESTDIR)$(MANDIR)); fi
	$(RM) $(DESTDIR)$(MANDIR)/cuum.$(MANSUFFIX)*
	$(INSTALL) -c $(INSTMANFLAGS) cuum.man \
 $(DESTDIR)$(MANDIR)/cuum.$(MANSUFFIX)
	-$(COMPRESSMANCMD) $(DESTDIR)$(MANDIR)/cuum.$(MANSUFFIX)

install.man:: cwnnkill.man
	@if [ -d $(DESTDIR)$(MANDIR) ]; then set +x; \
	else (set -x; $(MKDIRHIER) $(DESTDIR)$(MANDIR)); fi
	$(RM) $(DESTDIR)$(MANDIR)/cwnnkill.$(MANSUFFIX)*
	$(INSTALL) -c $(INSTMANFLAGS) cwnnkill.man \
 $(DESTDIR)$(MANDIR)/cwnnkill.$(MANSUFFIX)
	-$(COMPRESSMANCMD) $(DESTDIR)$(MANDIR)/cwnnkill.$(MANSUFFIX)

install.man:: cwnnstat.man
	@if [ -d $(DESTDIR)$(MANDIR) ]; then set +x; \
	else (set -x; $(MKDIRHIER) $(DESTDIR)$(MANDIR)); fi
	$(RM) $(DESTDIR)$(MANDIR)/cwnnstat.$(MANSUFFIX)*
	$(INSTALL) -c $(INSTMANFLAGS) cwnnstat.man \
 $(DESTDIR)$(MANDIR)/cwnnstat.$(MANSUFFIX)
	-$(COMPRESSMANCMD) $(DESTDIR)$(MANDIR)/cwnnstat.$(MANSUFFIX)
