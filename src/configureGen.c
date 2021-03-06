/*
 * Copyright (c) 2004
 *      Autonomous Systems Lab, Swiss Federal Institute of Technology.
 * Copyright (c) 2003 LAAS/CNRS
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
#include "genom-config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <ctype.h>

#include "genom.h"
#include "configureGen.h"

static void output(FILE* out, char const* name, char* value)
{
   if (value != NULL) {
      print_sed_subst(out, name, value);
      free(value);
   } else {
      print_sed_subst(out, name, "");
   }
}

static void genomDefines(FILE* out)
{
   char* str = NULL;
   int i;

   for (i = 0; i < nCppOptions; i++) {
      if (strncmp(cppOptions[i], "-D", 2) == 0) {
	 bufcat(&str, "%s ", cppOptions[i]);
      }
   } /* for */
   if (str != NULL) {
      print_sed_subst(out, "genomDefines", str);
      free(str);
   } else {
      print_sed_subst(out, "genomDefines", "");
   }
}

static void definePackage(char** conf_in, char** conf_mk_in, char const* NAME, char const* name)
{
    bufcatIfNotIn(conf_mk_in, "\n%s_CFLAGS   = \t\\@%s_CFLAGS\\@", NAME, NAME);
    bufcatIfNotIn(conf_mk_in, "\n%s_LIBS     = \t\\@%s_LIBS@", NAME, NAME);

    bufcatIfNotIn(conf_in,
	"\nPKG_CHECK_MODULES(%s, %s)\n"
	"AC_SUBST(%s_CFLAGS)\n"
	"AC_SUBST(%s_LIBS)\n\n",
	NAME, name, NAME, NAME);
}

#ifdef FOR_OPRS
static void defineExternOprsPackage(char** conf_in, char** conf_mk_in, char const* NAME, char const* name)
{
    bufcatIfNotIn(conf_mk_in, "\n%s_OPRS_CFLAGS   = \t\\@%s_OPRS_CFLAGS\\@", NAME, NAME);

    bufcatIfNotIn(conf_in,
	"\nPKG_CHECK_MODULES(%s_OPRS, %s-oprs)\n"
	"AC_SUBST(%s_OPRS_CFLAGS)\n"
	"AC_SUBST(%s_OPRS_LIBS)\n\n",
	NAME, name, NAME, NAME);
}
#endif

static void
defineExternTclservClientPackage(char** conf_in, char** conf_mk_in,
							     char const* NAME, char const* name)
{
    bufcatIfNotIn(conf_mk_in, "\n%s_TCLSERV_CLIENT_LIB_CFLAGS   = \t\\@%s_TCLSERV_CLIENT_LIB_CFLAGS\\@", NAME, NAME);
    bufcatIfNotIn(conf_mk_in, "\n%s_TCLSERV_CLIENT_LIB_LIBS   = \t\\@%s_TCLSERV_CLIENT_LIB_LIBS\\@", NAME, NAME);

    bufcatIfNotIn(conf_in,
	"\nPKG_CHECK_MODULES(%s_TCLSERV_CLIENT_LIB, %s-tclserv_client)\n"
	"AC_SUBST(%s_TCLSERV_CLIENT_LIB_CFLAGS)\n"
	"AC_SUBST(%s_TCLSERV_CLIENT_LIB_LIBS)\n\n",
	NAME, name, NAME, NAME);
}

/*----------------------------------------------------------------------*/
/*
 * format a module name into a pkgconfig name
 */
static char *
pkgconfigName(char *name)
{
	size_t size = strlen(name)+1;
	char *buf = malloc(size);
#if 0
	/* all lowercase, no dashes */
	char *tmp;
	int i;

	if (buf == NULL)
		return NULL;
	for (i = 0, tmp = buf; name[i] != '\0'; i++) {
		if (name[i] != '-')
			*tmp++ = tolower(name[i]);
	} /* for */
	*tmp = '\0';
#else
	/* This version doesn't touch the package name. */
	if (buf == NULL)
		return NULL;
	strncpy(buf, name,  size);
#endif
	return buf;
}

/*----------------------------------------------------------------------*/

int
configureGen(FILE *out,
	     const char *codelsDir, const char *cmdLine,
	     const char *genomBin, const char *genomFile, const char *genomWd,
	     int genTcl, int genOpenprs, int genServer, int genTclservClient)
{
   const char **p;
   EXEC_TASK_LIST *lt;
   ID_LIST *ln;

   char *pkg_conf_in, *pkg_conf_mk;
   char *genomIncludes, *serverLibs, *codelLibs, *codel_files;
   char *tclserv_client_cflags, *tclserv_client_libs;
   char *at_sign, *pkgname;
#define VERSION_TO_STR_LENGTH 200
   char version_to_str[VERSION_TO_STR_LENGTH];

   /* --- `configure' scripts ---------------------------------------- */

   const char *configProtosExec[] = {
       PROTO_INSTALL,
       PROTO_MKINSTALLDIRS,
       PROTO_CONFIGSUB,
       PROTO_CONFIGGUESS,
       PROTO_LTMAIN,
       NULL
   };

   const char *configProtos[] = {
       PROTO_USER_ACINCLUDE_M4,
       PROTO_AUTOGEN,
       PROTO_ROBOTS_M4,
       PROTO_LIBTOOL_M4,
       PROTO_PKG_M4,
       PROTO_CONFIG_LOCAL_MK,
       PROTO_CONFIG_POSIX_MK,
       PROTO_CONFIG_RTAI_MK,
       PROTO_CONFIG_XENOMAI_MK,
       PROTO_CONFIG_CODELS_MK,
       PROTO_CONFIGURE_AC_USER,
       NULL
   };

   for(p = configProtosExec; *p; p++) {
	   copy_script(out, *p);
   }

   for(p = configProtos; *p; p++) {
      script_open(out);
      subst_begin(out, *p);
      print_sed_subst(out, "module", module->name);
      print_sed_subst(out, "genTcl", genTcl ? "yes":"no");
      print_sed_subst(out, "genOpenprs", genOpenprs ? "yes":"no");
      print_sed_subst(out, "genTclservClient", genTclservClient ? "yes":"no");
      print_sed_subst(out, "genServer", genServer ? "yes":"no");
      print_sed_subst(out, "genomBin", genomBin);
      print_sed_subst(out, "codelsDir", codelsDir);
      subst_end(out);
      script_close(out, *p);
   }

   /* --- main makefile ---------------------------------------------- */
   pkgname = pkgconfigName(module->name);
   script_open(out);
   subst_begin(out, PROTO_MAKEFILE_TOP);

   print_sed_subst(out, "module", module->name);
   print_sed_subst(out, "pkgname", pkgname);
   print_sed_subst(out, "codelsDir", codelsDir);
   print_sed_subst(out, "genTcl", genTcl ? "" : "#");
   print_sed_subst(out, "genOpenprs", genOpenprs ? "":"#");
   print_sed_subst(out, "genTclservClient", genTclservClient ? "":"#");
   print_sed_subst(out, "genServer", genServer ? "":"#");

   subst_end(out);
   /* PROTO_MAKEFILE_TOP is Makefile.top.in while we want the autogenerated
    file to be Makefile.in */
   script_close(out, "Makefile.in");


   /* --- codels makefile -------------------------------------------- */
   script_open(out);
   subst_begin(out, PROTO_MAKEFILE_CODELS);

   print_sed_subst(out, "module", module->name);
   print_sed_subst(out, "MODULE", module->NAME);

   /* executions tasks */
   codel_files = NULL;
   if (module->codel_files == NULL) {
      for (lt = taches; lt != NULL; lt = lt->next) {
	 bufcat(&codel_files, "\t%s%sCodels.%s \\\\\n",
		module->name, lt->exec_task->name, langFileExt(module->lang));
      }
      bufcat(&codel_files, "\t%sCntrlTaskCodels.%s",
	     module->name, langFileExt(module->lang));
   } else
      for (ln = module->codel_files; ln; ln = ln->next) {
	 bufcat(&codel_files, "\t%s \\\\%s", ln->name, ln->next?"\n":"");
      }
   print_sed_subst(out, "codel_files", codel_files);
   free(codel_files);

   /* done */
   subst_end(out);
   script_close(out, PROTO_MAKEFILE_CODELS);

   /* --- build the configuration strings for needed packages  -----------------
    They are required because of -P
   */
   pkg_conf_in = NULL;
   pkg_conf_mk = NULL;

   for (ln = requires; ln != NULL; ln = ln->next)
       definePackage(&pkg_conf_in, &pkg_conf_mk, ln->NAME, ln->name);

#ifdef FOR_OPRS
   if (genOpenprs)
     for (ln = imports; ln != NULL; ln = ln->next)
       defineExternOprsPackage(&pkg_conf_in, &pkg_conf_mk, ln->NAME, ln->name);
#endif

   if (genTclservClient)
     for (ln = imports; ln != NULL; ln = ln->next)
       defineExternTclservClientPackage(&pkg_conf_in, &pkg_conf_mk, ln->NAME, ln->name);

   if (genServer)
     for (ln = codels_requires; ln != NULL; ln = ln->next)
       definePackage(&pkg_conf_in, &pkg_conf_mk, ln->NAME, ln->name);

   /* --- configure.ac --------------------------------------------------- */
   script_open(out);
   subst_begin(out, PROTO_CONFIGURE_AC);
   print_sed_subst(out, "version",   module->version);

   version_to_str[0] = 0;
   if (module->iface_version)
   {
       snprintf(version_to_str, VERSION_TO_STR_LENGTH, "%i:%i:%i",
	       module->iface_version->current, module->iface_version->revision, module->iface_version->age);
   }
   print_sed_subst(out, "iface_version", version_to_str);

   /* Quote the @ in email */
   at_sign = strchr(module->email, '@');
   if (! at_sign)
       print_sed_subst(out, "email",     "%s", module->email);
   else
   {
       *at_sign = 0;
       print_sed_subst(out, "email", "%s\\@%s", module->email, at_sign + 1);
       *at_sign = '@';
   }
   print_sed_subst(out, "lang_c",    module->lang == MODULE_LANG_C ? "yes" : "no" );
   print_sed_subst(out, "lang_cxx",  module->lang == MODULE_LANG_CXX ? "yes" : "no" );
   print_sed_subst(out, "module",    module->name);
   print_sed_subst(out, "pkgname",   pkgname);
   print_sed_subst(out, "genTcl",    genTcl?     "yes":"no");
   print_sed_subst(out, "genOpenprs", genOpenprs? "yes":"no");
   print_sed_subst(out, "genTclservClient", genTclservClient ? "yes":"no");
   print_sed_subst(out, "genServer", genServer? "yes":"no");
   print_sed_subst(out, "genomBin",  genomBin);
   print_sed_subst(out, "codelsDir", codelsDir);

   output(out, "genomPackages", pkg_conf_in);

   subst_end(out);
   script_close(out, PROTO_CONFIGURE_AC);

   /* --- config.mk --------------------------------------------------- */
   script_open(out);
   subst_begin(out, PROTO_CONFIG_MK);
   print_sed_subst(out, "module",    module->name);
   print_sed_subst(out, "pkgname",   pkgname);
   print_sed_subst(out, "genomBin",  genomBin);
   print_sed_subst(out, "codelsDir", codelsDir);
   print_sed_subst(out, "genTcl",    genTcl?     "":"# ");
   print_sed_subst(out, "genOpenprs", genOpenprs? "":"# ");
   print_sed_subst(out, "genTclservClient", genTclservClient? "":"# ");

   /* GenoM -D options */
   genomDefines(out);

   genomIncludes = NULL;
   serverLibs = NULL;
   codelLibs = NULL;
   tclserv_client_cflags = NULL;
   tclserv_client_libs = NULL;

   /* Include all packages by default */
   for (ln = requires; ln != NULL; ln = ln->next)
       bufcatIfNotIn(&genomIncludes, " \\$(%s_CFLAGS)", ln->NAME);

   for (ln = codels_requires; ln != NULL; ln = ln->next) {
     bufcatIfNotIn(&codelLibs, " \\$(%s_LIBS)", ln->NAME);
     if (genServer)
       bufcatIfNotIn(&genomIncludes, " \\$(%s_CFLAGS)", ln->NAME);
   }

   for (ln = imports; ln != NULL; ln = ln->next)
   {
	bufcatIfNotIn(&genomIncludes, " \\$(%s_CFLAGS)", ln->NAME);
		bufcatIfNotIn(&serverLibs, " \\$(%s_LIBS)", ln->NAME);
		if (genTclservClient) {
			bufcatIfNotIn(&tclserv_client_cflags,
					" \\$(%s_TCLSERV_CLIENT_LIB_CFLAGS)", ln->NAME);
			bufcatIfNotIn(&tclserv_client_libs,
					" \\$(%s_TCLSERV_CLIENT_LIB_LIBS)", ln->NAME);
		}
   }
   output(out, "genomPackages", pkg_conf_mk);
   output(out, "serverLibs", serverLibs);
   output(out, "codelLibs", codelLibs);
   output(out, "tclserv_client_cflags", tclserv_client_cflags);
   output(out, "tclserv_client_libs", tclserv_client_libs);

   /* -I options */
   for (ln = externPath; ln != NULL; ln = ln->next)
       bufcatIfNotIn(&genomIncludes, " -I%s", ln->name);
   output(out, "genomIncludes", genomIncludes);

   subst_end(out);
   script_close(out, PROTO_CONFIG_MK);
   free(pkgname);

   return 0;
}

/*
 * --- configureServerGen -----------------------------------------------
 *
 * Generate makefile for server part
 */

int
configureServerGen(FILE *out, char const* cmdLine,
		   const char *genomBin, const char *genomFile,
		   int genTcl, int genOpenprs, int genServer,
		   int genTclservClient, int genPython)
{
   char *str;
   EXEC_TASK_LIST *lt;
   ID_LIST *ln;

   /* --- genom.mk --------------------------------------------------- */
   script_open(out);
   subst_begin(out, PROTO_GENOM_MK);
   print_sed_subst(out, "genomBin", genomBin);
   print_sed_subst(out, "genomFile", genomFile);
   print_sed_subst(out, "genomFlags", cmdLine);
   subst_end(out);
   script_close(out, PROTO_GENOM_MK);

   /* --- server makefile -------------------------------------------- */
   script_open(out);
   subst_begin(out, PROTO_MAKEFILE_SERVER);

   print_sed_subst(out, "module", module->name);
   print_sed_subst(out, "MODULE", module->NAME);

   print_sed_subst(out, "genTcl", genTcl ? "" : "#");
   print_sed_subst(out, "genOpenprs", genOpenprs ? "" : "#");
   print_sed_subst(out, "genServer", genServer ? "" : "#");
   print_sed_subst(out, "genTclservClient", genTclservClient ? "" : "#");
   print_sed_subst(out, "genPython", genPython? "" : "#");

   str = NULL;
   for (ln = allIncludeFiles; ln != NULL; ln = ln->next) {
     /* Will install only .h files used by .gen file that are located
	in the main module directory */
     if (strstr(ln->name, "..") == NULL
	     && ln->name[0] != '/'
	     && strcmp(ln->name + strlen(ln->name) - 2, ".h") == 0) {
       bufcat(&str, "\t%s \\\\\n", ln->name);
     }
    } /* for */
    print_sed_subst(out, "listIncludesInDotGen", str);
    free(str);

    /* execution tasks */
    str = NULL;
    for (lt = taches; lt != NULL; lt = lt->next) {
	bufcat(&str, "\t%s%s.c\t\\\\%s", module->name, lt->exec_task->name,
		lt->next?"\n":"");
    }
    print_sed_subst(out, "listExecTask_c", str);
    free(str);

    /* done */
    subst_end(out);
    script_close(out, PROTO_MAKEFILE_SERVER);

    return 0;
}
/*----------------------------------------------------------------------*/
int pkgconfigGen(FILE *out, const char* cmdLine, const char* genomFile, int genOpenprs, int genServer, int genTclservClient, char** cppOptions)
{
    char* require = 0;
    char* require2 = 0;
    ID_LIST* ln, *ln2;
    char *pkgname;
    char** cppOpt;
    unsigned int cppOptStrLen;
    char* cppOptStr;

    pkgname = pkgconfigName(module->name);
    if (pkgname == NULL) {
	    return -1;
    }

    script_open(out);
    subst_begin(out, PROTO_PKGCONFIG_IN);

    /* Build the Requires: field. We take into account all packages in the
     * require: field of the .gen. */
    for (ln = requires; ln != NULL; ln = ln->next) {
      bufcat(&require, ", ");
      bufcat(&require, ln->name);
    }
    print_sed_subst(out, "require", require);
    free(require);
    require = NULL;

    /* Check that all "import from" are listed in the requires */
    for (ln = imports; ln != NULL; ln = ln->next) {
      for (ln2 = requires; ln2 != NULL; ln2 = ln2->next)
	if (!strncmp(ln2->name, ln->name, strlen(ln->name)))
	  break;
      if (ln2 == NULL) {
	fprintf(stderr,
		"imported module %s is not listed in the required packages.\n",
		ln->name);
      }
    }

    cppOptStrLen = 0;
    for (cppOpt = cppOptions; *cppOpt; ++cppOpt)
	cppOptStrLen += strlen(*cppOpt) + 1;
    cppOptStr = malloc(cppOptStrLen + 1);
    *cppOptStr = 0;
    for (cppOpt = cppOptions; *cppOpt; ++cppOpt)
    {
	strcat(cppOptStr, " ");
	strcat(cppOptStr, *cppOpt);
    }

    print_sed_subst(out, "module", module->name);
    print_sed_subst(out, "pkgname", pkgname);
    print_sed_subst(out, "genomVersion", genomVersion);
    print_sed_subst(out, "genomFile",   genomFile);
    print_sed_subst(out, "genomFlags",  cmdLine);
    print_sed_subst(out, "cppOptions",  cppOptStr);
    free(cppOptStr);
    subst_end(out);
    script_close(out, "autoconf/%s.pc.in", pkgname);

    /* package -oprs.pc.in for prs */
    if (genOpenprs) {
      script_open(out);
      subst_begin(out, PROTO_PKGCONFIG_OPRS_IN);

      /* Build the Requires: field. We take into account all packages in the
       * import_from field of the .gen. */
      for (ln = imports; ln != NULL; ln = ln->next) {
	for (ln2 = requires; ln2 != NULL; ln2 = ln2->next) {
	  size_t s = strcspn(ln2->name, " \t");
	  if (s > 0 && ln2->name[s] == '\0') s = 0;
	  if (s > 0) ln2->name[s] = '\0';

	  if (!strcmp(ln2->name, ln->name)) {
	    bufcat(&require, ", ");
	    bufcat(&require, ln2->name);
	    bufcat(&require, "-oprs");
	    if (s > 0) {
	      bufcat(&require, " ");
	      bufcat(&require, &ln2->name[s+1]);
	    }
	  }

	  if (s > 0) ln2->name[s] = ' ';
	}
      }
      print_sed_subst(out, "require", require);
      free(require);
      require = NULL;

      print_sed_subst(out, "module", module->name);
      print_sed_subst(out, "pkgname", pkgname);
      print_sed_subst(out, "genomVersion", genomVersion);
      subst_end(out);
      script_close(out, "autoconf/%s-oprs.pc.in", pkgname);
    } /* -oprs.pc.in */

    /* package -tclserv_client.pc.in for tclserv_client */
    if (genTclservClient) {
      script_open(out);
      subst_begin(out, PROTO_PKGCONFIG_TCLSERV_CLIENT_IN);

      /* Build the Requires: field. We take into account all packages in the
       * import_from field of the .gen. */
      for (ln = imports; ln != NULL; ln = ln->next) {
	for (ln2 = requires; ln2 != NULL; ln2 = ln2->next) {
	  size_t s = strcspn(ln2->name, " \t");
	  if (s > 0 && ln2->name[s] == '\0') s = 0;
	  if (s > 0) ln2->name[s] = '\0';

	  if (!strcmp(ln2->name, ln->name)) {
	    bufcat(&require, ", ");
	    bufcat(&require, ln2->name);
	    bufcat(&require, "-tclserv_client");
	    if (s > 0) {
	      bufcat(&require, " ");
	      bufcat(&require, &ln2->name[s+1]);
	    }
	  }

	  if (s > 0) ln2->name[s] = ' ';
	}
      }
      print_sed_subst(out, "require", require);
      free(require);
      require = NULL;

      print_sed_subst(out, "module", module->name);
      print_sed_subst(out, "pkgname", pkgname);
      print_sed_subst(out, "genomVersion", genomVersion);
      subst_end(out);
      script_close(out, "autoconf/%s-tclserv_client.pc.in", pkgname);
    } /* -tclserv_client.pc.in */

    free(pkgname);
    return 0;
}
