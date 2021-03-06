
/* 
 * Copyright (c) 1999-2003 LAAS/CNRS
 * Sara Fleury - Fri Jun  4 1999
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

/***
 *** Ge'ne'ration du GNUmakefile
 ***/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include"genom.h"
#include "parser.tab.h"

#include "openprsGen.h"

/*** 
 *** Génération des fichiers de compilation (Makefiles) des bibliotheques
 *** d'interface avec openprs
 ***/

int openprsMakeGen(FILE *out)
{
    return 0;
}
#if 0
    char *str;
    ID_LIST *ln;
    EXEC_TASK_LIST *lt;
    EXEC_TASK_STR *t;
    int i;

    /*----------------------------------------------------------------------
     * Makefile Unix
     */
    script_open(out);
    subst_begin(out, PROTO_OPENPRS_MAKEFILE);

    /* Nom du  module */
    print_sed_subst(out, "module", module->name);
    print_sed_subst(out, "MODULE", module->NAME);

    /* liste des serveurs */
    str = NULL;
    for (lt = taches; lt != NULL; lt = lt->next) {
	t = lt->exec_task;
	for (ln = t->cs_client_from; ln != NULL; ln = ln->next) {
	    bufcatIfNotIn(&str, "-I\\$(%s) -I\\$(%s)/server ", ln->NAME, ln->NAME);
	} /* for */
	for (ln = t->poster_client_from; ln != NULL; ln = ln->next) {
	    bufcatIfNotIn(&str, "-I\\$(%s) -I\\$(%s)/server ", ln->NAME, ln->NAME);
	} /* for */
    } /* for */
    for (ln = imports; ln != NULL; ln = ln->next) {
	bufcatIfNotIn(&str, "-I\\$(%s) -I\\$(%s)/server ", ln->NAME, ln->NAME);
    }
    /* Options -J et -I passe'es a` genom */
    for (ln = externPathMacro; ln != NULL; ln = ln->next) {
        bufcatIfNotIn(&str, "-I\\$(%s) ", ln->name);
    } 
    /* for */
    for (i = 0; i < nCppOptions; i++) {
	if (strncmp(cppOptions[i], "-I", 2) == 0) {
	    bufcatIfNotIn(&str, "%s ", cppOptions[i]);
	}
    } /* for */
    
    if (str != NULL) {
	print_sed_subst(out, "serversDir", str);
	free(str);
    } else {
	print_sed_subst(out, "serversDir", "");
    }

    str = NULL;

    /* Options passe'es a` genom */
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
    
    /* Fin */
    subst_end(out);
    script_close(out, "server/openprs/Makefile.in");

    return(0);

}
#endif

