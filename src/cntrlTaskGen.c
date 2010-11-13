/* 
 * Copyright (c) 1993-2010 LAAS/CNRS
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
#include "parser.tab.h"
#include "cntrlTaskGen.h"

/***
 *** Ge'ne'ration de la ta^che de controle
 ***/

static char *
tabRequestFunc(int nbRqst)
{
    RQST_LIST *l;
    RQST_STR *r;
    char *msg = NULL;

    for (l = requetes; l != NULL; l = l->next) {
        r = l->rqst;
	bufcat(&msg,  
		"static void %sCntrl%s(SERV_ID servId, int rqstId);\n", 
		module->name, r->name);
    } /* for */
    bufcat(&msg, "\nstatic void (*%sTabRequestFunc[])() = {\n",
	   module->name); 
    for (l = requetes; l != NULL; l = l->next) {
        r = l->rqst;
	bufcat(&msg, "%sCntrl%s", module->name, r->name);
	if (l->next != NULL)
	    bufcat(&msg, ",\n");
    }
    bufcat(&msg, "\n};");
    bufcat(&msg, "\nstatic int %sTabRequestNum[] = {\n",
	   module->name);
    for (l = requetes; l != NULL; l = l->next) {
	r = l->rqst;
	bufcat(&msg, "%d", r->rqst_num);
	if (l->next != NULL) {
	    bufcat(&msg, ", ");
	}
    }
    bufcat(&msg, "};\n");

    return(msg);
}

int 
cntrlTaskGen(FILE *out)
{
    char buf[1024], buf1[80];
    char *trf;
    char *type, *var;
    int i;
    RQST_LIST *rqst, *li;
    ID_LIST *il;
    int *tabCompat;
    EXEC_TASK_LIST *lt;
    char *execTask;

    script_open(out);
    subst_begin(out, PROTO_CNTRL_TASK_C);
    
    /* Nom du  module */
    print_sed_subst(out, "module", module->name);
    print_sed_subst(out, "MODULE", module->NAME);
    print_sed_subst(out, "numModule", "%d", module->number);
	
    /* Compte des taches d'execution */
    print_sed_subst(out, "nbExecTask", "%d", nbExecTask);

    /* numero de la requete d'abort */
    print_sed_subst(out, "abortRequestNum", "%d", trouve_abort_num());
    
    /* tableau des fonctions de gestion des requetes */
    trf = tabRequestFunc(nbRequest);
    print_sed_subst(out, "tabRequestFuncDeclare", trf);
    free(trf);
    
    /* type de donne'es interne */
    print_sed_subst(out, "internalDataType", "struct %s", 
		    module->internal_data->name);
    
    /* Requete d'init attendue */
    if(initRequest == 0) {
      print_sed_subst(out, "initRqst", "-1");
    }
    /* Recherche de la requet d'init */
    else {
      for (rqst = requetes; rqst != NULL; rqst = rqst->next) {
	if(rqst->rqst->type == INIT) {
	  print_sed_subst(out, "initRqst", "%d", rqst->rqst->num);
	  break;
	}
      }
    }

    /* Liste des taches d'execution */
    execTask = NULL;
    bufcat(&execTask, "\nstatic char *%sExecTaskNameTab[] = {\n",
	   module->name); 
    for (lt = taches; lt != NULL; lt = lt->next) {
	bufcat(&execTask, "\"%s\"", lt->exec_task->name);
	if (lt->next != NULL)
	    bufcat(&execTask, ",\n");
    } 
    bufcat(&execTask, "\n};");
    print_sed_subst(out, "execTaskNameTabDeclare", execTask);
    free(execTask);

    /* Fin */
    subst_end(out);

    
    for (rqst = requetes; rqst != NULL; rqst = rqst->next) {

      /* Selection canevas */
	subst_begin(out, rqst->rqst->type == CONTROL ?  
		    /* Requete CONTROL */
		    PROTO_RQST_CNTRL_C : 
		    /* Requete INIT ou EXEC */
		    PROTO_RQST_EXEC_C);

	/* Nom du  module */
	print_sed_subst(out, "module", module->name);
	print_sed_subst(out, "MODULE", module->NAME);
	/* Nom de la requete */
	print_sed_subst(out, "request", rqst->rqst->name);
	/* Numero de la requete */
	print_sed_subst(out, "requestNum", "%d", rqst->rqst->num);

	/* Requete d'init */
	if (rqst->rqst->type == INIT)
	  print_sed_subst(out, "initRqst", "0");
	else
	  print_sed_subst(out, "initRqst", "1");

	/* inputs */
	if (rqst->rqst->input != NULL) {
	    print_sed_subst(out, "inputFlag", "(1)");
	    dcl_nom_decl(rqst->rqst->input->dcl_nom, &type, &var);
	    print_sed_subst(out, "inputDeclare", "%s %s;", type, var);
	    free(type);
	    free(var);
	    if (rqst->rqst->input->dcl_nom->flags & STRING ||
		rqst->rqst->input->dcl_nom->flags & ARRAY)
	      print_sed_subst(out, "inputNamePtr", "%s",
			      rqst->rqst->input->dcl_nom->name);
	    else
	      print_sed_subst(out, "inputNamePtr", "&%s",
			      rqst->rqst->input->dcl_nom->name);
	    sprintf(buf, "sizeof((*%sDataStrId)", module->name);
	    for (il = rqst->rqst->input->sdi_ref; il != NULL;
		 il = il->next) {
		strcat(buf, ".");
		strcat(buf, il->name);
	    } 
	    strcat(buf, ")");
	    print_sed_subst(out, "inputSize", buf);
	    sprintf(buf, "&((*%sDataStrId)", module->name);
	    for (il = rqst->rqst->input->sdi_ref; il != NULL;
		 il = il->next) {
		strcat(buf, ".");
		strcat(buf, il->name);
	    }
	    strcat(buf, ")");
	    print_sed_subst(out, "inputRefPtr", buf);
	    if (rqst->rqst->codel_control != NULL) {
		print_sed_subst(out, "cControlFunc", 
				rqst->rqst->codel_control);
	    }
	} else {
	    print_sed_subst(out, "inputFlag", "(0)");
	    print_sed_subst(out, "inputDeclare", "");
	    print_sed_subst(out, "inputSize", "0");
	    print_sed_subst(out, "inputNamePtr", "NULL");
	    print_sed_subst(out, "inputRefPtr", "NULL");
	}
	/* outputs */
	if (rqst->rqst->output != NULL && rqst->rqst->type != INIT) {
	    print_sed_subst(out, "outputFlag", "(1)");
	    dcl_nom_decl(rqst->rqst->output->dcl_nom, &type, &var);
	    print_sed_subst(out, "outputDeclare", "%s %s;", type, var);
	    free(type);
	    free(var);
	    print_sed_subst(out, "outputNamePtr", "&%s", 
			    rqst->rqst->output->dcl_nom->name);
	    sprintf(buf, "sizeof((*%sDataStrId)", module->name);
	    for (il = rqst->rqst->output->sdi_ref; il != NULL;
		 il = il->next) {
		strcat(buf, ".");
		strcat(buf, il->name);
	    } 
	    strcat(buf, ")");
	    print_sed_subst(out, "outputSize", buf);
	    sprintf(buf, "&((*%sDataStrId)", module->name);
	    for (il = rqst->rqst->output->sdi_ref; il != NULL;
		 il = il->next) {
		strcat(buf, ".");
		strcat(buf, il->name);
	    }
	    strcat(buf, ")");
	    print_sed_subst(out, "outputRefPtr", buf);
	}
	else {
	    if (rqst->rqst->output != NULL)
	      fprintf(stderr, "%s: error: output not taken into account for init request (%s)\n",
		      nomfic, rqst->name);

	    print_sed_subst(out, "outputNamePtr", "NULL");
	    print_sed_subst(out, "outputFlag", "(0)");
	    print_sed_subst(out, "outputDeclare", "");
	    print_sed_subst(out, "outputRefPtr", "NULL");
	    print_sed_subst(out, "outputSize", "0");
	}
	/* tableau de compatibilite' */
	sprintf(buf, "static int %s%sCompatibility[] = { ", 
		module->name, rqst->rqst->name);
	tabCompat = (int *)xalloc(nbRequest * sizeof(int));

	for (i = 0; i < nbRequest; i++) {
	    tabCompat[i] = 1;
	}
	for (li = rqst->rqst->interrupt_activity; li != NULL; li = li->next) {
	    if (li->rqst != NULL) {
		tabCompat[li->rqst->num] = 0;
	    }
	}

	for (i = 0; i < nbRequest - 1; i++) {
	    sprintf(buf1, "%d, ", tabCompat[i]);
	    strcat(buf, buf1);
	}
	sprintf(buf1, "%d };", tabCompat[nbRequest - 1]);
	strcat(buf, buf1);
	
	print_sed_subst(out, "tabCompatibilityDeclare", buf);
	print_sed_subst(out, "tabCompatibility", "%s%sCompatibility", 
			module->name, rqst->name);
	/* Flag pour indiquer si fct reentrante */
	if (tabCompat[rqst->rqst->num] == 1) {
	    print_sed_subst(out, "reentrantFlag", "1");
	} else {
	    print_sed_subst(out, "reentrantFlag", "0");
	}
	free(tabCompat);


	/* Fonction de controle */
	if (rqst->rqst->codel_control != NULL) {
	    print_sed_subst(out, "controlFuncFlag", "1");
	    print_sed_subst(out, "cControlFunc", 
			    rqst->rqst->codel_control);
	    if (rqst->rqst->input != NULL) {
		dcl_nom_decl(rqst->rqst->input->dcl_nom, &type, &var);
		print_sed_subst(out, "cControlFuncProto", "int %s(%s *%s);", 
				rqst->rqst->codel_control, type, var);
		free(type);
		free(var);
	    } else {
		print_sed_subst(out, "cControlFuncProto", "int %s(void *);", 
				rqst->rqst->codel_control);
	    }
	} else {
	    print_sed_subst(out, "controlFuncFlag", "0");
	    print_sed_subst(out, "cControlFunc", "dummy");
	    print_sed_subst(out, "cControlFuncProto", "");
	}
	    
	/* Exec task pour INIT et EXEC */
	if (rqst->rqst->exec_task != NULL) {
	    print_sed_subst(out, "execTaskNum", "%d", 
			    rqst->rqst->exec_task->num);
	} else if (rqst->rqst->type != CONTROL) {
	    fprintf(stderr, 
		    "%s: Error: no exec_task for exec request %s\n",
		    nomfic, rqst->name);
	    return(-1);
	}
	subst_end(out);
    } /* for */
    script_close(out, "server/%sCntrlTask.c", module->name);
    return(0);
}


