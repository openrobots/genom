/*	$LAAS$ */

/* 
 * Copyright (c) 1993-2003 LAAS/CNRS
 * Matthieu Herrb - Fri Jul 16 1993
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
#include "genom.h"
#include "parser.tab.h"
#include "headerGen.h"
#include "typeGen.h"

static void gen_c_exec_proto(char **str, RQST_STR *r, ID_STR *id);

/*** 
 *** Generation du header du module
 ***/

int headerGen(FILE *out)
{
    char *str, *type, *var;
    EXEC_TASK_LIST *lt;
    EXEC_TASK_STR *t;
    RQST_LIST *lr;
    RQST_STR *r;
    ID_LIST *ln;
    POSTER_LIST *p;
    int i;
    
    script_open(out);
	subst_begin(out, PROTO_HEADER);
    /* Nom du  module */
    print_sed_subst(out, "module", module->name);
    print_sed_subst(out, "MODULE", module->NAME);

    /* Nombre de requetes */
    print_sed_subst(out, "nbRequest", "%d", nbRequest);

    /* type de donne'es interne */
    print_sed_subst(out, "internalDataType", "struct %s", 
                    module->internal_data->name);

    /* Liste des numeros des execTasks */
    str = NULL;
    i = 0;
    for (lt = taches; lt != NULL; lt = lt->next) {
	t = lt->exec_task;
	bufcat(&str, "#define %s_%s_NUM (%d)\n", module->NAME, t->NAME, i++);
    } /* for */
    print_sed_subst(out, "listExecTaskNum", str);
    free(str);

    /* 
     * fichiers include 
     */
    /* csLib.h */
    str = NULL;
    for (lt = taches; lt != NULL; lt = lt->next) {
	t = lt->exec_task;
	for (ln = t->cs_client_from; ln != NULL; ln = ln->next) {
	    bufcat(&str, "#include \"auto/%sMsgLib.h\"\n", ln->name);
	} /* for */
    } /* for */
    
    if (str != NULL) {
	print_sed_subst(out, "listServerHeader", str);
	free(str);
    } else {
	print_sed_subst(out, "listServerHeader", "");
    }
    /* posterLib.h */
    str = NULL;
    for (lt = taches; lt != NULL; lt = lt->next) {
	t = lt->exec_task;
	for (ln = t->poster_client_from; ln != NULL; ln = ln->next) {
	    bufcat(&str, "#include \"auto/%sPosterLib.h\"\n", ln->name);
	} /* for */
    } /* for */
    if (str != NULL) {
	print_sed_subst(out, "listPosterHeader", str);
	free(str);
    } else {
	print_sed_subst(out, "listPosterHeader", "");
    }
    
    /* Liste des Ids des Clients */
    str = NULL;
    for (lt = taches; lt != NULL; lt = lt->next) {
	t = lt->exec_task;
	i = 0;
	if (t->cs_client_from == NULL) {
	    continue;
	}
	for (ln = t->cs_client_from; ln != NULL; ln = ln->next) {
	    bufcat(&str, "#define %s_%s_%s_CLIENT_ID "
		   "(%sCntrlStrId->execTaskTab[%s_%s_NUM].clientId[%d])\n",
		   module->NAME, t->NAME, ln->NAME, module->name,
		   module->NAME, t->NAME, i++);
	} /* for */
	bufcat(&str, "\n");
    } /* for */
    if (str != NULL) {
	print_sed_subst(out, "listExecTaskClientId", str);
	free(str);
    } else {
	print_sed_subst(out, "listExecTaskClientId", "");
    }

    /* Liste des Ids des Posters */
    str = NULL;
    for (lt = taches; lt != NULL; lt = lt->next) {
	t = lt->exec_task;
	i = 0;
	for (p = posters; p!= NULL; p = p->next) {
	    if (p->exec_task == t) {
		bufcat(&str, "#define %s_%s_POSTER_ID "
		       "(%sCntrlStrId->execTaskTab[%s_%s_NUM]"
		       ".posterId[%d])\n", module->NAME, p->NAME,
		       module->name, module->NAME, t->NAME, i++);
	    }
	} /* for */
    } /* for */
    if (str != NULL) {
	print_sed_subst(out, "listPosterId", str);
	free(str);
    } else {
	print_sed_subst(out, "listPosterId", "");
    }
    
    /* 
     *  Prototypes des fonctions utilisateurs 
     */
    str = NULL;
    for (lr = requetes; lr != NULL; lr = lr->next) {
	r = lr->rqst;
	if (r->c_control_func != NULL) {
	    bufcat(&str, "extern STATUS %s(", r->c_control_func);
	    if (r->input != NULL) {
		dcl_nom_decl(r->input->dcl_nom, &type, &var);
		bufcat(&str, "%s %s(%s), ", type, 
		       r->input->dcl_nom->ndimensions == 0 ? "*" : "",
		       var);
		free(type);
		free(var);
	    }
	    bufcat(&str, "int *bilan);\n");
	}
	gen_c_exec_proto(&str, r, r->c_exec_func_start);
	gen_c_exec_proto(&str, r, r->c_exec_func_end);
	gen_c_exec_proto(&str, r, r->c_exec_func_fail);
	gen_c_exec_proto(&str, r, r->c_exec_func_inter);
	gen_c_exec_proto(&str, r, r->c_exec_func);
    } /* for */
    for (lt = taches; lt != NULL; lt = lt->next) {
	t = lt->exec_task;
	if (t->c_init_func != NULL) {
	    bufcat(&str, "extern STATUS %s(int *bilan);\n", t->c_init_func);
	}
	if (t->c_end_func != NULL) {
          bufcat(&str, "extern STATUS %s(void);\n", t->c_end_func);
	}
	if (t->c_func != NULL) {
	    bufcat(&str, "extern STATUS %s(int *bilan);\n", t->c_func);
	}
    } /* for */
    /* Prototypes des fonctions de creation des posters */
    for (p = posters; p!= NULL; p = p->next) {
	if (p->create_func != NULL) {
	    bufcat(&str, "extern STATUS %s(char *name, int size, "
		   "POSTER_ID *pPosterId);\n", p->create_func);
	}
    } /* for */
    print_sed_subst(out, "listUserFuncProto", str);
    free(str);

    /* Fin */
    subst_end(out);
    script_close(out, "%sHeader.h", module->name);

    return(0);

} /* headerGen */
		   
/*----------------------------------------------------------------------*/

/**
 ** Generation des prototypes des fonctions de traitement 
 ** des requetes d'execution 
 **/
static void 
gen_c_exec_proto(char **str, RQST_STR *r, ID_STR *id)
{
    char *type, *var;

    if (id == NULL) {
	return;
    }
    bufcat(str, "extern ACTIVITY_EVENT %s(", id);
    if (r->input != NULL) {
	dcl_nom_decl(r->input->dcl_nom, &type, &var);
	bufcat(str, "%s %s(in_%s), ", type, 
	       r->input->dcl_nom->ndimensions == 0 ? "*" : "",
	       var);
	free(type);
	free(var);
    }
    if (r->output != NULL) {
	dcl_nom_decl(r->output->dcl_nom, &type, &var);
	bufcat(str, "%s %s(out_%s), ", type, 
	       r->output->dcl_nom->ndimensions == 0 ? "*" : "",
	       var);
	free(type);
	free(var);
    }
    bufcat(str, "int *bilan);\n");

} /* gen_c_exec_proto */
