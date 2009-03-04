
/* 
 * Copyright (c) 1993-2005 LAAS/CNRS
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "genom.h"
#include "genom/genomError.h"
#include "parser.tab.h"
#include "errorGen.h"

/*** 
 *** Ge'ne'ration liste des erreurs
 ***/

static const H2_ERROR const genomH2errMsgs[] = GENOM_H2_ERR_MSGS;


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
    ID_LIST *m, *cntrlFailList = NULL, *execFailList = NULL, *tmp;
    int n, err, i;
    char *cntrlFail, *execFail, *stdFail;
    char *localFail = NULL;
    char *othersReports=NULL;
    EXEC_TASK_LIST *lt;
    short genSrc, genErr;
    int genomNbErrs;

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
	for (m = lt->exec_task->fail_reports; m != NULL; m = m->next) {
	    if (!id_member(m, execFailList)) {
		tmp = STR_ALLOC(ID_LIST);
		tmp->name = m->name;
		tmp->next = execFailList;
		execFailList = tmp;
	    }
	} /* for */
    } /* for */

    /* list des requetes */
    for (l = requetes; l != NULL; l = l->next) {
	r = l->rqst;
	for (m = r->fail_reports; m != NULL; m = m->next) {
	    if (!id_member(m, cntrlFailList) && !id_member(m, execFailList)) {
		tmp = (ID_LIST *)xalloc(sizeof(ID_LIST));
		tmp->name = m->name;
		if (r->type == CONTROL) {
		    tmp->next = cntrlFailList;
		    cntrlFailList = tmp;
		} else {
		    tmp->next = execFailList;
		    execFailList = tmp;
		}
	    }
	}
    }

    /* generation liste */
    cntrlFail = NULL;
    n = 1;
    for (m = cntrlFailList; m != NULL; m = m->next) {
	if (!(err = H2_ENCODE_ERR(module->number, n))) {
	  printf ("genom: error code invalide :\n" 
		  "genom: M_%s=%d or S_%s_%s=%d not in [1, 2^16=65536]\n", 
		  module->name, module->number,
		  module->name, m->name, n);
	}
	bufcat(&cntrlFail, 
	       "#define S_%s_%-20s H2_ENCODE_ERR(M_%-10s, %d)\n",
	       module->name, m->name, module->name, n);
	bufcat(&localFail,
	       "\n    {\"%s\", %d}, \\\\", m->name, n);
	n++;
    }

    /* Liberation liste */
    for (m = cntrlFailList; m != NULL; tmp = m->next, free(m), m = tmp);

    execFail = NULL;
    for (m = execFailList; m != NULL; m = m->next) {
	if (!(err = H2_ENCODE_ERR(module->number, n))) {
	  printf ("genom: error code invalide :" 
		  "genom: M_%s=%d or S_%s_%s=%d not in [1, 2^16=65536]\n", 
		  module->name, module->number, 
		  module->name, m->name, n);
	}
	bufcat(&execFail, 
	       "#define S_%s_%-20s H2_ENCODE_ERR(M_%-10s, %d)\n",
	       module->name, m->name, module->name, n);
	bufcat(&localFail,
	       "\n    {\"%s\", %d},\\\\", m->name, n);
	n++;
    }
    /* liberation liste */
    for (m = execFailList; m != NULL; tmp = m->next, free(m), m = tmp);

    /* erreurs standards */
    stdFail = NULL;
    
    genomNbErrs = sizeof(genomH2errMsgs)/sizeof(H2_ERROR);
    for (i=0; i<genomNbErrs; i++) {

      if (!(err = H2_ENCODE_ERR(module->number, genomH2errMsgs[i].num))) {
	printf ("genom: error code invalide :" 
		"genom: M_%s=%d or S_stdGenoM_%s=%d not in [1, 2^16=65536]\n", 
		module->name, module->number, 
		genomH2errMsgs[i].name, genomH2errMsgs[i].num);
      }

      genSrc = H2_SOURCE_STD_ERR(genomH2errMsgs[i].num);
      genErr = H2_NUMBER_STD_ERR(genomH2errMsgs[i].num);

      bufcat(&stdFail, "#define S_%s_stdGenoM_%-26s %d   /* %d<<16 | 0x8000 | %d<<8 | %d */\n", 
	     module->name, genomH2errMsgs[i].name, 
	     err, 
	     module->number, genSrc, genErr);
    }

    print_sed_subst(out, "listCntrlFailures", cntrlFail);
    print_sed_subst(out, "listExecFailures", execFail);
    print_sed_subst(out, "listStdFailures", stdFail);
    print_sed_subst(out, "listTabFailures", localFail);
    
    free(cntrlFail);
    free(execFail);
    free(stdFail);
    free(localFail);

    subst_end(out);
    script_close(out, "server/%sError.h", module->name);

    /* ------ error.c ---------------------------------------- */
    script_open(out);
    subst_begin(out, PROTO_ERROR_C);

    /* Nom du  module */
    print_sed_subst(out, "module", module->name);
    print_sed_subst(out, "MODULE", module->NAME);

    for (m = imports; m != NULL; m = m->next)
      bufcat(&othersReports, "  %sRecordH2errMsgs();\n", m->name);

    print_sed_subst(out, "h2recordOtherModules", othersReports);
    free(othersReports);
    subst_end(out);
    script_close(out, "server/%sError.c", module->name);

    return(0);
}

