/*	$LAAS$ */

/* 
 * Copyright (c) 2003 LAAS/CNRS                       --  Thu Aug 21 2003
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
#include "config.h"
__RCSID("$LAAS$");

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "genom.h"
#include "configureGen.h"

int
configureGen(FILE *out,
	     const char *codelsDir, const char *cmdLine,
	     const char *genomBin, const char *genomFile, const char *genomWd,
	     int genTcl, int genPropice, int genSpy)
{
   const char **p;
   char *str, *str1, *str2, *str3;
   EXEC_TASK_LIST *lt;
   EXEC_TASK_STR *t;
   ID_LIST *ln;
   int i;

   /* --- `configure' scripts ---------------------------------------- */

   const char *configProtos[] = {
      PROTO_CONFIGURE,
      PROTO_INSTALL,
      PROTO_CONFIGSUB,
      PROTO_CONFIGGUESS,
      PROTO_LTCONFIG,
      PROTO_LTCFC,
      PROTO_LTMAIN,
      PROTO_CONFIG_MK,
      NULL
   };
      
   for(p = configProtos; *p; p++) {
      script_open(out);
      subst_begin(out, *p);
      print_sed_subst(out, "module", module->name);
      print_sed_subst(out, "genTcl", genTcl?"yes":"no");
      print_sed_subst(out, "genomBin", genomBin);
      subst_end(out);
      script_close_exec(out, *p);
   }

   /* --- genom.mk --------------------------------------------------- */
   script_open(out);
   subst_begin(out, PROTO_GENOM_MK);
   print_sed_subst(out, "genomBin", genomBin);
   print_sed_subst(out, "genomFile", genomFile);
   print_sed_subst(out, "genomWd", genomWd);

   print_sed_subst(out, "genTcl", genTcl ? "" : "#");
   print_sed_subst(out, "genPropice", genPropice ? "" : "#");
   print_sed_subst(out, "genSpy", genSpy ? "" : "#");

   /* GenoM options */
   str = NULL;
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

   str = NULL;
   for (i = 0; i < nCppOptions; i++) {
      if (strncmp(cppOptions[i], "-I", 2) == 0) {
	 bufcat(&str, "%s ", cppOptions[i]);
      }
   } /* for */
   if (str != NULL) {
      print_sed_subst(out, "genomIncludes", str);
      free(str);
   } else {
      print_sed_subst(out, "genomIncludes", "");
   }

   subst_end(out);
   script_close(out, PROTO_GENOM_MK);


   /* --- bootstrap makefile ----------------------------------------- */
   script_open(out);
   subst_begin(out, PROTO_MAKEFILE_BS);
   subst_end(out);
   script_close(out, PROTO_MAKEFILE_BS);


   /* --- main makefile ---------------------------------------------- */
   script_open(out);
   subst_begin(out, PROTO_MAKEFILE_TOP);

   print_sed_subst(out, "module", module->name);
   print_sed_subst(out, "codelsDir", codelsDir);
   print_sed_subst(out, "cmdLine", cmdLine);

   subst_end(out);
   script_close(out, PROTO_MAKEFILE_TOP);

   
   /* --- auto makefile ---------------------------------------------- */
   script_open(out);
   subst_begin(out, PROTO_MAKEFILE_AUTO);

   print_sed_subst(out, "module", module->name);
   print_sed_subst(out, "MODULE", module->NAME);

   print_sed_subst(out, "genTcl", genTcl ? "" : "#");
   print_sed_subst(out, "genPropice", genPropice ? "" : "#");
   print_sed_subst(out, "genSpy", genSpy ? "" : "#");

   /* servers */
   str = str1 = str2 = str3 = NULL;
   for (lt = taches; lt != NULL; lt = lt->next) {
      t = lt->exec_task;
      /* cs client */
      for (ln = t->cs_client_from; ln != NULL; ln = ln->next) {
	 bufcatIfNotIn(&str, "-I\\$(%s) ", ln->NAME);
	 bufcatIfNotIn(&str1, "\\$(%s)/auto/\\$(TARGET)/%sClient.a ", 
		       ln->NAME, ln->name);
      } /* for */
	/* poster client */
      for (ln = t->poster_client_from; ln != NULL; ln = ln->next) {
	 bufcatIfNotIn(&str, "-I\\$(%s) ", ln->NAME);
	 bufcatIfNotIn(&str1, "\\$(%s)/auto/\\$(TARGET)/%sClient.a ", 
		       ln->NAME, ln->name);
	 bufcatIfNotIn(&str3, "\\$(%s)/auto/\\$(TARGET)-shared/%sEndian.o ", 
		       ln->NAME, ln->name);
      } /* for */
   } /* for */
   /* other */
   for (ln = externLibs; ln != NULL; ln = ln->next) {
      bufcatIfNotIn(&str, "-I\\$(%s) ", ln->NAME);
      bufcatIfNotIn(&str1, "\\$(%s)/auto/\\$(TARGET)/%sClient.a ", 
		    ln->NAME, ln->name);
      bufcatIfNotIn(&str2, "\\$(%s)/auto/\\$(TARGET)/lib%sScope.so ", 
		    ln->NAME, ln->name);
      bufcatIfNotIn(&str3, "\\$(%s)/auto/\\$(TARGET)-shared/%sEndian.o ", 
		    ln->NAME, ln->name);
   }
   for (ln = externPathMacro; ln != NULL; ln = ln->next) {
      bufcatIfNotIn(&str, "-I\\$(%s) ", ln->name);
   } /* for */

   if (str != NULL) {
      print_sed_subst(out, "serversDir", str);
      print_sed_subst(out, "serversLib", str1);
      print_sed_subst(out, "serversEndianLib", str3);
      free(str);
      free(str1);
      free(str3);
   } else {
      print_sed_subst(out, "serversDir", "");
      print_sed_subst(out, "serversLib", "");
      print_sed_subst(out, "serversEndianLib", "");
   }
   if (str2 != NULL) {
      print_sed_subst(out, "externScopeLibs", str2);
      free(str2);
   } else {
      print_sed_subst(out, "externScopeLibs", "");
   }

   /* GenoM options */
   str = NULL;
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
   script_close(out, PROTO_MAKEFILE_AUTO);


   /* --- codels makefile -------------------------------------------- */
   script_open(out);
   subst_begin(out, PROTO_MAKEFILE_CODELS);

   print_sed_subst(out, "module", module->name);
   print_sed_subst(out, "MODULE", module->NAME);

   /* servers */
   str = str1 = NULL;
   for (lt = taches; lt != NULL; lt = lt->next) {
      t = lt->exec_task;
      for (ln = t->cs_client_from; ln != NULL; ln = ln->next) {
	 bufcatIfNotIn(&str, "-I\\$(%s) ", ln->NAME);
	 bufcatIfNotIn(&str1, "\\$(%s)/auto/\\$(TARGET)/%sClient.a ", 
		       ln->NAME, ln->name);
      } /* for */
      for (ln = t->poster_client_from; ln != NULL; ln = ln->next) {
	 bufcatIfNotIn(&str, "-I\\$(%s) ", ln->NAME);
	 bufcatIfNotIn(&str1, "\\$(%s)/auto/\\$(TARGET)/%sClient.a ", 
		       ln->NAME, ln->name);
      } /* for */
   } /* for */
   for (ln = externLibs; ln != NULL; ln = ln->next) {
      bufcatIfNotIn(&str, "-I\\$(%s) ", ln->NAME);
      bufcatIfNotIn(&str1, "\\$(%s)/auto/\\$(TARGET)/%sClient.a ", 
		    ln->NAME, ln->name);
   }
   if (str != NULL) {
      print_sed_subst(out, "serversDir", str);
      print_sed_subst(out, "serversLib", str1);
      free(str);
      free(str1);
   } else {
      print_sed_subst(out, "serversLib", "");
      print_sed_subst(out, "serversDir", "");
   }

   /* GenoM options */
   str = NULL;
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

   return 0;
}
