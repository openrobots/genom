/*	$LAAS$ */

/* 
 * Copyright (c) 1993-2003 LAAS/CNRS
 * Matthieu Herrb - Tue Jul 20 1993
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

/***
 *** Ge'ne'ration du GNUmakefile
 ***/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "genom.h"
#include "userMakeGen.h"


int userMakeGen(FILE *out, char *codelsDir, char *cmdLine)
{
    char *str;
    EXEC_TASK_LIST *lt;
    EXEC_TASK_STR *t;
    ID_LIST *ln;
    int i;

    /*
     * Makefile VxWorks
     */

    script_open(out);
    subst_begin(out, PROTO_MAKEFILE_USER_VXWORKS);

    /* Nom du  module */
    print_sed_subst(out, "module", module->name);
    print_sed_subst(out, "MODULE", module->NAME);

    /* liste des serveurs */
    str = NULL;
    for (lt = taches; lt != NULL; lt = lt->next) {
	t = lt->exec_task;
	for (ln = t->cs_client_from; ln != NULL; ln = ln->next) {
	    bufcatIfNotIn(&str, "-I\\$(%s) ", ln->NAME);
	} /* for */
	for (ln = t->poster_client_from; ln != NULL; ln = ln->next) {
	    bufcatIfNotIn(&str, "-I\\$(%s) ", ln->NAME);
	} /* for */
    } /* for */
    for (ln = externLibs; ln != NULL; ln = ln->next) {
      bufcatIfNotIn(&str, "-I\\$(%s) ", ln->NAME);
    } /* for */
    if (str != NULL) {
	print_sed_subst(out, "serversDir", str);
	free(str);
    } else {
	print_sed_subst(out, "serversDir", "");
    }
    /* Options passe'es a` genom */
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

    /* liste des sources des taches d'exec */
    for (lt = taches; lt != NULL; lt = lt->next) {
	bufcat(&str, "\t%s%sCodels.c \\\\\n", 
	       module->name, lt->exec_task->name);
    }
    print_sed_subst(out, "listExecTaskFunc_c", str);
    free(str);
    
    /* Fin */
    subst_end(out);
    script_close(out, "user/Makefile.vxworks");
    
    return(0);
}

