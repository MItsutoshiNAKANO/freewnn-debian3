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

INSTALL      = $/usr/bin/install -c
INSTALLFLAGS = $(INSTINCFLAGS)

all::

RKS = mode 	2A_CTRL

install::
	@for flag in ${MAKEFLAGS} ''; do \
	case "$$flag" in *=*) ;; *[i]*) set +e;; esac; done; \
	for i in $(TWNNWNNDIR)/rk_z; do if [ -d $(DESTDIR)$$i ]; then \
	set +x; else (set -x; $(MKDIRHIER) $(DESTDIR)$$i); fi; \
	done

install:: $(RKS)
	@if [ -d $(DESTDIR)$(TWNNWNNDIR)/rk_z ]; then set +x; \
	else (set -x; $(MKDIRHIER) $(DESTDIR)$(TWNNWNNDIR)/rk_z); fi
	@for flag in ${MAKEFLAGS} ''; do \
	case "$$flag" in *=*) ;; *[i]*) set +e;; esac; done; \
	for i in $(RKS); do \
	(set -x; $(INSTALL) $(INSTALLFLAGS) $(INSTDATFLAGS) $$i $(DESTDIR)$(TWNNWNNDIR)/rk_z); \
	done
