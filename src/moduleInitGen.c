/*	$LAAS$ */

/*
 * Copyright (c) 1996-2003 LAAS/CNRS
 * Matthieu Herrb - Thu Jul 22 1993
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
#include "genom.h"
#include "moduleInitGen.h"

/***
 *** Ge'ne'ration de l'initialisation du module
 ***/

int moduleInitGen(FILE *out)
{
    EXEC_TASK_LIST *lt;
    char *execTask;
    int periodic;

    script_open(out);
	subst_begin(out, PROTO_MODULE_INIT);
    /* Nom du  module */
    print_sed_subst(out, "module", module->name);
    print_sed_subst(out, "MODULE", module->NAME);

    /* type de donne'es interne */
    print_sed_subst(out, "internalDataType", "struct %s",
		    module->internal_data->name);

    periodic = 0;
    for (lt = taches; lt != NULL; lt = lt->next)
      if (lt->exec_task->period > 0) {
	periodic = 1;
	break;
      }
    print_sed_subst(out, "periodic", periodic?"1":"0");

    /* Liste des taches d'execution */
    execTask = NULL;
    for (lt = taches; lt != NULL; lt = lt->next) {
	bufcat(&execTask, "extern void %s%s(%s_CNTRL_STR *, struct %s *);\n",
	       module->name, lt->exec_task->name, module->NAME,
	       module->internal_data->name);
    }
    bufcat(&execTask, "\ntypedef struct {\n\tchar *name;\n");
    bufcat(&execTask,
	   "\tint priority;\n\tint size;\n\tvoid (*func)();\n} ");
    bufcat(&execTask, "%s_EXEC_TASK_STR;\n\n", module->NAME);
    bufcat(&execTask, "%s_EXEC_TASK_STR %sExecTaskTab[] = {\n",
	   module->NAME, module->name);
    for (lt = taches; lt != NULL; lt = lt->next) {
	if (lt->exec_task->stack_size == 0) {
	    fprintf(stderr, "%s: warning: No stack size specified for %s\n",
		    nomfic, lt->exec_task->name);
	    lt->exec_task->stack_size = 5000;
	}
	bufcat(&execTask, "\t{ \"%s%s\", %d, %d, %s%s }", module->name,
	lt->exec_task->name, lt->exec_task->priority,
	lt->exec_task->stack_size,
	module->name, lt->exec_task->name);
	if (lt->next != NULL) {
	    bufcat(&execTask, ",\n");
	}
    }
    bufcat(&execTask, " };\n");

    print_sed_subst(out, "execTaskTabDescribe", execTask);
    free(execTask);

    /* Fin */
    subst_end(out);
    script_close(out, "server/%sModuleInit.c", module->name);

    return(0);

}


