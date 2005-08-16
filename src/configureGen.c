/*	$LAAS$ */

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
__RCSID("$LAAS$");

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <ctype.h>

#include "genom.h"
#include "configureGen.h"

/*
static char* strtoupper(char const* from)
{
    char* str = NULL, *str1 = NULL;
    bufcat(&str, "%s", from);
    for(str1 = str; *str1; ++str1)
        *str1 = toupper(*str1);
    return str;
}
*/

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

int
configureGen(FILE *out,
	     const char *codelsDir, const char *cmdLine,
	     const char *genomBin, const char *genomFile, const char *genomWd,
	     int genTcl, int genOpenprs)
{
   const char **p;
   EXEC_TASK_LIST *lt;
   ID_LIST *ln;

   char *pkg_conf_in, *pkg_conf_mk;
   char *genomIncludes, *serverLibs, *codel_files;
   char *at_sign;

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
       PROTO_CONFIG_LOCAL_MK,
       PROTO_CONFIG_POSIX_MK,
       PROTO_CONFIG_RTAI_MK,
       PROTO_CONFIG_CODELS_MK,
       PROTO_CONFIGURE_AC_USER,
       NULL
   };
      
   for(p = configProtosExec; *p; p++) {
      script_open(out);
      subst_begin(out, *p);
      print_sed_subst(out, "module", module->name);
      print_sed_subst(out, "genTcl", genTcl ? "yes":"no");
      print_sed_subst(out, "genOpenprs", genOpenprs ? "yes":"no");
      print_sed_subst(out, "genomBin", genomBin);
      subst_end(out);
      script_close_exec(out, *p);
   }
   for(p = configProtos; *p; p++) {
      script_open(out);
      subst_begin(out, *p);
      print_sed_subst(out, "module", module->name);
      print_sed_subst(out, "genTcl", genTcl ? "yes":"no");
      print_sed_subst(out, "genOpenprs", genOpenprs ? "yes":"no");
      print_sed_subst(out, "genomBin", genomBin);
      print_sed_subst(out, "codelsDir", codelsDir);
      subst_end(out);
      script_close(out, *p);
   }

   /* --- main makefile ---------------------------------------------- */
   script_open(out);
   subst_begin(out, PROTO_MAKEFILE_TOP);

   print_sed_subst(out, "module", module->name);
   print_sed_subst(out, "codelsDir", codelsDir);
   print_sed_subst(out, "genTcl", genTcl ? "" : "#");
   print_sed_subst(out, "genOpenprs", genOpenprs ? "":"#");

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
	 bufcat(&codel_files, "\t%s%sCodels.c \\\\\n",
		module->name, lt->exec_task->name);
      }
      bufcat(&codel_files, "\t%sCntrlTaskCodels.c", module->name);
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

   /* --- configure.ac.begin.in --------------------------------------------------- */
   script_open(out);
   subst_begin(out, PROTO_CONFIGURE_AC);
   print_sed_subst(out, "version",   module->version);
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
   print_sed_subst(out, "use_cxx",   module->use_cxx ? "1" : "0" );
   print_sed_subst(out, "module",    module->name);
   print_sed_subst(out, "genTcl",     genTcl?     "yes":"no");
   print_sed_subst(out, "genOpenprs", genOpenprs? "yes":"no");
   print_sed_subst(out, "genomBin",  genomBin);
   print_sed_subst(out, "codelsDir", codelsDir);

   output(out, "genomPackages", pkg_conf_in);

   subst_end(out);
   script_close(out, PROTO_CONFIGURE_AC);

   /* --- config.mk --------------------------------------------------- */
   script_open(out);
   subst_begin(out, PROTO_CONFIG_MK);
   print_sed_subst(out, "module",    module->name);
   print_sed_subst(out, "genomBin",  genomBin);
   print_sed_subst(out, "codelsDir", codelsDir);

   /* GenoM -D options */
   genomDefines(out);

   genomIncludes = NULL;
   serverLibs = NULL;

   /* Include all packages by default */
   for (ln = requires; ln != NULL; ln = ln->next)
       bufcatIfNotIn(&genomIncludes, " \\$(%s_CFLAGS)", ln->NAME);
   for (ln = imports; ln != NULL; ln = ln->next)
   {
        bufcatIfNotIn(&genomIncludes, " \\$(%s_CFLAGS)", ln->NAME);
	bufcatIfNotIn(&serverLibs, " \\$(%s_LIBS)", ln->NAME); 
   }
   output(out, "genomPackages", pkg_conf_mk);
   output(out, "serverLibs", serverLibs);

   /* -I options */
   for (ln = externPath; ln != NULL; ln = ln->next)
       bufcatIfNotIn(&genomIncludes, " -I%s", ln->name);
   output(out, "genomIncludes", genomIncludes);

   subst_end(out);
   script_close(out, PROTO_CONFIG_MK);

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
		   int genTcl, int genOpenprs)
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

   str = NULL;
   for (ln = includeFiles; ln != NULL; ln = ln->next) {
     /* Will install only .h files used by .gen file that are located 
	in the main module directory */
     if (strstr(ln->name, "..") == NULL && ln->name[0] != '/') {
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

int pkgconfigGen(FILE *out, int genOpenprs)
{
    char* require = 0;
    char* require2 = 0;
    ID_LIST* ln, *ln2;

    script_open(out);
    subst_begin(out, PROTO_PKGCONFIG_IN);

    /* Build the require field 
     * We take into account only the packages 
     * used in "import from" statements */
    for (ln = imports; ln != NULL; ln = ln->next)
    {
	for (ln2 = requires; ln2 != NULL; ln2 = ln2->next)
	    if (! strcmp(ln2->name, ln->name))
	    {
		bufcat(&require, ", ");
		bufcat(&require, ln->name);
	    }
    }
    output(out, "require", require);

    print_sed_subst(out, "module", module->name);
    print_sed_subst(out, "genomMinor", genomMinor);
    print_sed_subst(out, "genomMajor", genomMajor);
    subst_end(out);
    script_close(out, "autoconf/%s.pc.in", module->name);

    /* package -oprs.pc.in for prs */
    if (genOpenprs) {
      script_open(out);
      subst_begin(out, PROTO_PKGCONFIG_OPRS_IN);
      
      
      /* Build the require field 
       * We take into account only the packages 
       * use in "import from" statements */
      for (ln = imports; ln != NULL; ln = ln->next)
	{
	  for (ln2 = requires; ln2 != NULL; ln2 = ln2->next)
	    if (! strcmp(ln2->name, ln->name))
	      {
		bufcat(&require2, ", ");
		bufcat(&require2, ln->name);
		bufcat(&require2, "-oprs");
	      }
	}
      output(out, "require", require2);
      
      print_sed_subst(out, "module", module->name);
      print_sed_subst(out, "genomMinor", genomMinor);
      print_sed_subst(out, "genomMajor", genomMajor);
      subst_end(out);
      script_close(out, "autoconf/%s-oprs.pc.in", module->name);
    } /* -oprs.pc.in */

    return 0;
}
