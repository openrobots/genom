/*	$LAAS$ */

/* 
 * Copyright (c) 1993-2003 LAAS/CNRS
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

#include "genom.h"
#include "parser.tab.h"
#include "errorGen.h"

/*** 
 *** Ge'ne'ration liste des erreurs
 ***/

static int id_member(ID_LIST *m, ID_LIST *l)
{
    for (; l != NULL; l = l->next) {
	if (!strcmp(m->name, l->name)) {
	    return(1);
	}
    }
    return(0);
}


int errorGen(FILE *out)
{
    RQST_LIST *l;
    RQST_STR *r;
    EXEC_TASK_LIST *lt;
    ID_LIST *m, *cntrlFailList = NULL, *execFailList = NULL,  *tmp;
    ID_LIST *execTaskListTmpXXX = NULL;
    int n;
    char *cntrlFail, *execFail;

    script_open(out);
	subst_begin(out, PROTO_ERROR_H);

    /* Nom du  module */
    print_sed_subst(out, "module", module->name);
    print_sed_subst(out, "MODULE", module->NAME);

    /* Nume'ro du module */
    print_sed_subst(out, "numModule", "%d", module->number);

    /* Liste des erreurs */
    /* 1. Construction d'une liste unique */

    /* Liste des taches d'exec */
    for (lt = taches; lt != NULL; lt = lt->next) {
	for (m = lt->exec_task->fail_msg; m != NULL; m = m->next) {
	    if (!id_member(m, execFailList)) {
		tmp = STR_ALLOC(ID_LIST);
		tmp->name = m->name;
		tmp->next = execFailList;
		execFailList = tmp;

		/* XXX tmp for compatibility purpose */
		tmp = (ID_LIST *)xalloc(sizeof(ID_LIST));
		tmp->name = lt->exec_task->name;
		tmp->next = execTaskListTmpXXX;
		execTaskListTmpXXX = tmp;
	    }
	} /* for */
    } /* for */

    /* erreur pour les reque^tes */
    for (l = requetes; l != NULL; l = l->next) {
	r = l->rqst;
	for (m = r->fail_msg; m != NULL; m = m->next) {
	    if (!id_member(m, cntrlFailList) && !id_member(m, execFailList)) {
		tmp = (ID_LIST *)xalloc(sizeof(ID_LIST));
		tmp->name = m->name;
		if (r->type == CONTROL) {
		    tmp->next = cntrlFailList;
		    cntrlFailList = tmp;
		} else {
		    tmp->next = execFailList;
		    execFailList = tmp;

		    /* XXX tmp for compatibility purpose */
		    tmp = (ID_LIST *)xalloc(sizeof(ID_LIST));
		    tmp->name = r->exec_task_name;
		    tmp->next = execTaskListTmpXXX;
		    execTaskListTmpXXX = tmp;
		}
	    }
	}
    }

    /* generation liste */
    cntrlFail = NULL;
    n = 1;
    for (m = cntrlFailList; m != NULL; m = m->next) {
	bufcat(&cntrlFail, 
	       "#define S_%s_%s \t\t(M_%s << 16 | %d )\n",
	       module->name, m->name, module->name, n);
	n++;
    }

    execFail = NULL;
    for (m = execFailList; m != NULL; m = m->next) {
	bufcat(&execFail, "#define S_%s_%s \t\t(M_%s << 16 | %d )\n",
	       module->name, m->name, module->name, n);
	n++;
    }

    /* XXXXX generation liste for compatibility purpose */
    bufcat(&execFail,  "\n\n/* XXXXX temporary for compatibility purpose */\n");
    for (m = cntrlFailList; m != NULL; m = m->next) {
	bufcat(&execFail, 
	       "#define S_%sCntrlTask_%s \t\tS_%s_%s\n",
	       module->name, m->name, module->name, m->name);
    }
    bufcat(&execFail,  "\n");

    for (m = execFailList; m != NULL; m = m->next) {
	bufcat(&execFail, "#define S_%s%s_%s \t\tS_%s_%s\n",
	       module->name, execTaskListTmpXXX->name, m->name, 
	       module->name, m->name);
	tmp = execTaskListTmpXXX;
	execTaskListTmpXXX = execTaskListTmpXXX->next;
	free(tmp);
    }

    /* Liberation liste */
    for (m = cntrlFailList; m != NULL; tmp = m->next, free(m), m = tmp);

    /* liberation liste */
    for (m = execFailList; m != NULL; tmp = m->next, free(m), m = tmp);

    print_sed_subst(out, "listCntrlFailures", cntrlFail);
    print_sed_subst(out, "listExecFailures", execFail);
    
    free(cntrlFail);
    free(execFail);

    /* Fin */
    subst_end(out);
    script_close(out, "server/%sError.h", module->name);
    return(0);

}

