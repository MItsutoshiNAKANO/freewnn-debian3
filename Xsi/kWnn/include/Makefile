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

INSTALL      = $/usr/bin/install -c
INSTALLFLAGS = $(INSTINCFLAGS)

LINKS = commonhd.h wnn_config.h copyright.h demcom.h hinsi_file.h jd_sock.h     jdata.h jh.h jlib.h jllib.h jslib.h msg.h rk_spclval.h wnn_malloc.h     wnn_string.h wnnerror.h cplib.h wnn_os.h hinsi.h

HEADERS = jllib.h 	jslib.h 	rk_spclval.h 	wnnerror.h 	commonhd.h 	cplib.h 	msg.h

all:: $(LINKS)

includes:: $(LINKS)

commonhd.h:  $(WNNINCLUDESRC)/commonhd.h
	$(RM) $@
	$(LN) $? $@

includes:: commonhd.h

depend:: commonhd.h

clean::
	$(RM) commonhd.h

wnn_config.h:  $(WNNINCLUDESRC)/wnn_config.h
	$(RM) $@
	$(LN) $? $@

includes:: wnn_config.h

depend:: wnn_config.h

clean::
	$(RM) wnn_config.h

copyright.h:  $(WNNINCLUDESRC)/copyright.h
	$(RM) $@
	$(LN) $? $@

includes:: copyright.h

depend:: copyright.h

clean::
	$(RM) copyright.h

demcom.h:  $(WNNINCLUDESRC)/demcom.h
	$(RM) $@
	$(LN) $? $@

includes:: demcom.h

depend:: demcom.h

clean::
	$(RM) demcom.h

hinsi_file.h:  $(WNNINCLUDESRC)/hinsi_file.h
	$(RM) $@
	$(LN) $? $@

includes:: hinsi_file.h

depend:: hinsi_file.h

clean::
	$(RM) hinsi_file.h

jd_sock.h:  $(WNNINCLUDESRC)/jd_sock.h
	$(RM) $@
	$(LN) $? $@

includes:: jd_sock.h

depend:: jd_sock.h

clean::
	$(RM) jd_sock.h

jdata.h:  $(WNNINCLUDESRC)/jdata.h
	$(RM) $@
	$(LN) $? $@

includes:: jdata.h

depend:: jdata.h

clean::
	$(RM) jdata.h

jh.h:  $(WNNINCLUDESRC)/jh.h
	$(RM) $@
	$(LN) $? $@

includes:: jh.h

depend:: jh.h

clean::
	$(RM) jh.h

jlib.h:  $(WNNINCLUDESRC)/jlib.h
	$(RM) $@
	$(LN) $? $@

includes:: jlib.h

depend:: jlib.h

clean::
	$(RM) jlib.h

jllib.h:  $(WNNINCLUDESRC)/jllib.h
	$(RM) $@
	$(LN) $? $@

includes:: jllib.h

depend:: jllib.h

clean::
	$(RM) jllib.h

jslib.h:  $(WNNINCLUDESRC)/jslib.h
	$(RM) $@
	$(LN) $? $@

includes:: jslib.h

depend:: jslib.h

clean::
	$(RM) jslib.h

msg.h:  $(WNNINCLUDESRC)/msg.h
	$(RM) $@
	$(LN) $? $@

includes:: msg.h

depend:: msg.h

clean::
	$(RM) msg.h

rk_spclval.h:  $(WNNINCLUDESRC)/rk_spclval.h
	$(RM) $@
	$(LN) $? $@

includes:: rk_spclval.h

depend:: rk_spclval.h

clean::
	$(RM) rk_spclval.h

wnn_malloc.h:  $(WNNINCLUDESRC)/wnn_malloc.h
	$(RM) $@
	$(LN) $? $@

includes:: wnn_malloc.h

depend:: wnn_malloc.h

clean::
	$(RM) wnn_malloc.h

wnn_string.h:  $(WNNINCLUDESRC)/wnn_string.h
	$(RM) $@
	$(LN) $? $@

includes:: wnn_string.h

depend:: wnn_string.h

clean::
	$(RM) wnn_string.h

wnnerror.h:  $(WNNINCLUDESRC)/wnnerror.h
	$(RM) $@
	$(LN) $? $@

includes:: wnnerror.h

depend:: wnnerror.h

clean::
	$(RM) wnnerror.h

cplib.h:  $(WNNINCLUDESRC)/cplib.h
	$(RM) $@
	$(LN) $? $@

includes:: cplib.h

depend:: cplib.h

clean::
	$(RM) cplib.h

wnn_os.h:  $(WNNINCLUDESRC)/wnn_os.h
	$(RM) $@
	$(LN) $? $@

includes:: wnn_os.h

depend:: wnn_os.h

clean::
	$(RM) wnn_os.h

hinsi.h:  $(WNNINCLUDESRC)/hinsi.h
	$(RM) $@
	$(LN) $? $@

includes:: hinsi.h

depend:: hinsi.h

clean::
	$(RM) hinsi.h

instlib:: install

install::  $(HEADERS)
	@if [ -d $(DESTDIR)$(KWNNINCDIR) ]; then set +x; \
	else (set -x; $(MKDIRHIER) $(DESTDIR)$(KWNNINCDIR)); fi
	@cont=0; \
	for flag in ${MAKEFLAGS} ''; do \
	case "$$flag" in *=*) ;; *[ik]*) cont=1;; esac; done; \
	for i in  $(HEADERS) ; do \
	(set -x; $(INSTALL) $(INSTALLFLAGS) $(INSTDATFLAGS) $$i $(DESTDIR)$(KWNNINCDIR)) \
	|| if [ $$cont -eq 0 ]; then exit 1; fi; \
	done


depend::

clean::
	$(RM) $(LINKS)
