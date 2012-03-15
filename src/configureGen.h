
/* 
 * Copyright (c) 2003,2009 LAAS/CNRS                       --  Thu Aug 21 2003
 * All rights reserved.
 *
 * Redistribution and use  in source  and binary  forms,  with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *   1. Redistributions of  source  code must retain the  above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice,  this list of  conditions and the following disclaimer in
 *      the  documentation  and/or  other   materials provided  with  the
 *      distribution.
 *
 * THIS  SOFTWARE IS PROVIDED BY  THE  COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND  ANY  EXPRESS OR IMPLIED  WARRANTIES,  INCLUDING,  BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES  OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR  PURPOSE ARE DISCLAIMED. IN  NO EVENT SHALL THE COPYRIGHT
 * HOLDERS OR      CONTRIBUTORS  BE LIABLE FOR   ANY    DIRECT, INDIRECT,
 * INCIDENTAL,  SPECIAL,  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF  SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN  CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE   OF THIS SOFTWARE, EVEN   IF ADVISED OF   THE POSSIBILITY OF SUCH
 * DAMAGE.
 */
#ifndef H_CONFIGUREGEN
#define H_CONFIGUREGEN

#define PROTO_USER_ACINCLUDE_M4 "acinclude.user.m4"
#define PROTO_AUTOGEN       	"autogen" 
#define PROTO_CONFIGURE_AC      "autoconf/configure.ac"
#define PROTO_CONFIGURE_AC_USER "configure.ac.user"
#define PROTO_INSTALL		"autoconf/install-sh" 
#define PROTO_MKINSTALLDIRS	"autoconf/mkinstalldirs" 
#define PROTO_CONFIGSUB		"autoconf/config.sub" 
#define PROTO_CONFIGGUESS	"autoconf/config.guess" 
#define PROTO_LTMAIN		"autoconf/ltmain.sh" 

#define PROTO_ROBOTS_M4		"autoconf/robots.m4" 
#define PROTO_LIBTOOL_M4	"autoconf/libtool.m4"
#define PROTO_PKG_M4		"autoconf/pkg.m4"

#define PROTO_CONFIG_MK		"autoconf/config.mk.in"
#define PROTO_CONFIG_POSIX_MK	"autoconf/config.posix.mk"
#define PROTO_CONFIG_RTAI_MK	"autoconf/config.rtai.mk"
#define PROTO_CONFIG_XENOMAI_MK	"autoconf/config.xenomai.mk"
#define PROTO_CONFIG_LOCAL_MK   "local.mk.in"
#define PROTO_CONFIG_CODELS_MK	"autoconf/codels.mk"
#define PROTO_GENOM_MK		"autoconf/genom.mk"

#define PROTO_MAKEFILE_TOP	"Makefile.top.in"
#define PROTO_MAKEFILE_SERVER	"server/Makefile.in"
#define PROTO_MAKEFILE_CODELS	"codels/Makefile.in"

#define PROTO_PKGCONFIG_IN	"autoconf/module.pc.in"
#define PROTO_PKGCONFIG_OPRS_IN	"autoconf/module-oprs.pc.in"
#define PROTO_PKGCONFIG_TCLSERV_CLIENT_IN "autoconf/module-tclserv_client.pc.in"

int configureGen(FILE *out, 
        const char *codelsDir, 
	const char *cmdLine, const char *genomBin, const char *genomFile, 
        const char *genomWd,
		 int genTcl, int genOpenprs, int genServer, int genTclservClient);
int configureServerGen(FILE *out, 
        const char* cmdLine, const char *genomBin, const char *genomFile,
		       int genTcl, int genOpenprs, int genServer, int genTclservClient, int genPython);
int pkgconfigGen(FILE *out, const char* cmdLine, const char* genomFile, int genOpenprs, int genServer, int genTclservClient, char** cppOptions);

#endif /* H_CONFIGUREGEN */
