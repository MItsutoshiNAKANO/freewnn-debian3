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

INSTALL         = $/usr/bin/install -c
INSTALLFLAGS = $(INSTDATFLAGS)

SUBDIRS = rk

CLIENTJDS = uum.msg 	uumkey  	uumkey_e  	uumrc

all::

instclient:: install

install::  $(CLIENTJDS)
	@if [ -d $(DESTDIR)$(LWNNWNNDIR) ]; then set +x; \
	else (set -x; $(MKDIRHIER) $(DESTDIR)$(LWNNWNNDIR)); fi
	@for flag in ${MAKEFLAGS} ''; do \
	case "$$flag" in *=*) ;; *[i]*) set +e;; esac; done; \
	for i in  $(CLIENTJDS); do \
	(set -x; $(INSTALL) $(INSTALLFLAGS) $(INSTDATFLAGS) $$i $(DESTDIR)$(LWNNWNNDIR)); \
	done

depend::

clean::

