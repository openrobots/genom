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

int
configureGen(FILE *out,
	     const char *codelsDir, const char *cmdLine,
	     const char *genomBin, const char *genomFile, const char *genomWd,
	     int genTcl, int genOpenprs)
{
   const char **p;
   char *str, *str2, *str3, *str4;
   EXEC_TASK_LIST *lt;
   ID_LIST *ln, *ln2;

   /* --- `configure' scripts ---------------------------------------- */

   const char *configProtosExec[] = {
      PROTO_CONFIGURE,
      PROTO_INSTALL,
      PROTO_MKINSTALLDIRS,
      PROTO_CONFIGSUB,
      PROTO_CONFIGGUESS,
      PROTO_LTMAIN,
      NULL
   };

   const char *configProtos[] = {
      PROTO_CONFIGURE_IN,
      PROTO_ROBOTS_M4,
      PROTO_LIBTOOL_M4,
      PROTO_ACLOCAL_M4,
      PROTO_ACINCLUDE_M4,
      PROTO_CONFIG_LOCAL_MK,
      PROTO_CONFIG_POSIX_MK,
      PROTO_CONFIG_RTAI_MK,
      PROTO_CONFIG_CODELS_MK,
      NULL
   };
      
   for(p = configProtosExec; *p; p++) {
      script_open(out);
      subst_begin(out, *p);
      print_sed_subst(out, "module", module->name);
      print_sed_subst(out, "genTcl", genTcl?"yes":"no");
      print_sed_subst(out, "genomBin", genomBin);
      subst_end(out);
      script_close_exec(out, *p);
   }
   for(p = configProtos; *p; p++) {
      script_open(out);
      subst_begin(out, *p);
      print_sed_subst(out, "module", module->name);
      print_sed_subst(out, "genTcl", genTcl?"yes":"no");
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
   print_sed_subst(out, "cmdLine", cmdLine);

   subst_end(out);
   script_close(out, "Makefile.in");


   /* --- codels makefile -------------------------------------------- */
   script_open(out);
   subst_begin(out, PROTO_MAKEFILE_CODELS);

   print_sed_subst(out, "module", module->name);
   print_sed_subst(out, "MODULE", module->NAME);

   /* executions tasks */
   str = NULL;
   if (module->codel_files == NULL) {
      for (lt = taches; lt != NULL; lt = lt->next) {
	 bufcat(&str, "\t%s%sCodels.c \\\\\n",
		module->name, lt->exec_task->name);
      }
      bufcat(&str, "\t%sCntrlTaskCodels.c", module->name);
   } else
      for (ln = module->codel_files; ln; ln = ln->next) {
	 bufcat(&str, "\t%s \\\\%s", ln->name, ln->next?"\n":"");
      }
   print_sed_subst(out, "codel_files", str);
   free(str);
    
   /* done */
   subst_end(out);
   script_close(out, PROTO_MAKEFILE_CODELS);

   /* --- config.mk --------------------------------------------------- */
   script_open(out);
   subst_begin(out, PROTO_CONFIG_MK);
   print_sed_subst(out, "module",    module->name);
   print_sed_subst(out, "genTcl",    genTcl?"yes":"no");
   print_sed_subst(out, "genomBin",  genomBin);
   print_sed_subst(out, "codelsDir", codelsDir);

   /* GenoM -D options */
   genomDefines(out);

   /* GenoM -J options */
   str = NULL; /* is used for genomIncludes ALL THE WAY */
   str2 = NULL;
   str3 = NULL;
   str4 = NULL;
   for (ln = externPathMacro, ln2 = externPathMacroPath; ln != NULL; ln = ln->next, ln2 = ln2->next) {
       bufcatIfNotIn(&str2, "\n%sDIR=\t%s", ln->NAME, ln2->name);
   }

   /* import from */
   for (ln = externLibs; ln != NULL; ln = ln->next) {
	/* test if $(MACRO)DIR has already been defined with -J
         * else, use the default value $(prefix)*/
	char* check_macro = NULL;
	bufcat(&check_macro, "\n%sDIR=", ln->NAME);
	if (str2 == NULL || strstr(str2, check_macro) == NULL)
        {
            bufcat(&str2, "\n%sDIR=\\$(prefix)", ln->NAME, ln->name);
	}
        bufcatIfNotIn(&str, " -I\\$(%sDIR)/include/%s", ln->NAME, ln->name);
	free(check_macro);

	bufcatIfNotIn(&str4, "-L\\$(%sDIR)/lib -l%sClient ", 
		      ln->NAME, ln->name); 
   }

   output(out, "genomJOptionsAndServers", str2);
   output(out, "serversClientLib", str4);

   /* -I options */
   for (ln = externPath; ln != NULL; ln = ln->next) {
       bufcatIfNotIn(&str, " -I%s", ln->name);
   }

   output(out, "genomIncludes", str);

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
configureServerGen(FILE *out, 
		   const char *genomBin, const char *genomFile,
		   int genTcl, int genOpenprs)
{
   char *str;
   EXEC_TASK_LIST *lt;
   ID_LIST *ln, *ln2;

   /* --- genom.mk --------------------------------------------------- */
   script_open(out);
   subst_begin(out, PROTO_GENOM_MK);
   print_sed_subst(out, "genomBin", genomBin);
   print_sed_subst(out, "genomFile", genomFile);

   print_sed_subst(out, "genTcl", genTcl ? "" : "#");
   print_sed_subst(out, "genOpenprs", genOpenprs ? "" : "#");

   /* GenoM options */

   /* -D */
   genomDefines(out);

   /* GenoM -J options */
   str = NULL;
   for (ln = externPathMacro, ln2 = externPathMacroPath; 
	ln != NULL; 
	ln = ln->next, ln2 = ln2->next) {
     bufcatIfNotIn(&str, " -J%s=%s", ln->name, ln2->name);
   }

   /* GenoM -I options */
   for (ln = externPath; ln != NULL; ln = ln->next) {
       bufcatIfNotIn(&str, " -I%s", ln->name);
   }

   output(out, "genomIncludes", str);

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
