/*	$LAAS$ */

/* 
 * Copyright (c) 1993-2003 LAAS/CNRS
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
#include "parser.tab.h"
#include "execTaskGen.h"

/***
 *** Ge'ne'ration des taches d'execution
 ***/

int 
execTaskGen(FILE *out)
{
    char *str, *str2;
    EXEC_TASK_LIST *lt;
    EXEC_TASK_STR *t;
    RQST_LIST *lr;
    RQST_STR *r;
    STR_REF_LIST *lm;
    ID_LIST *ln;
    DCL_NOM_STR *n;
    POSTER_LIST *p;
    char *type, *var;
    int i;
    
    for (lt = taches; lt != NULL; lt = lt->next) {
	t = lt->exec_task;
	
	script_open(out);
	subst_begin(out, PROTO_EXEC_TASK);
	/* Nom du  module */
	print_sed_subst(out, "module", module->name);
	print_sed_subst(out, "MODULE", module->NAME);
	print_sed_subst(out, "numModule", "%d", module->number);

	/* nom de la tache */
	print_sed_subst(out, "execTaskName", t->name);
	print_sed_subst(out, "EXECTASKNAME", t->NAME);
	print_sed_subst(out, "execTaskNum", "%d", t->num);

	/* type SDI */
	n = STR_ALLOC(DCL_NOM_STR);
	n->type = module->internal_data;
	n->name = "x";
	dcl_nom_decl(n, &type, &var);
	print_sed_subst(out, "internalDataType", type);
	free(type);
	free(var);
	free(n);

	/* periode */
	if (t->period > 0) {
	    print_sed_subst(out, "periodFlag", "1");
	    print_sed_subst(out, "period", "%d", t->period);
	    print_sed_subst(out, "delay", "%d", t->delay);
	} else {
	    print_sed_subst(out, "periodFlag", "0");
	    print_sed_subst(out, "period", "0");
	    print_sed_subst(out, "delay", "0");
	}
	    
	/* init func */
	if (t->codel_task_start != NULL) {
	    print_sed_subst(out, "cFuncExecInitFlag", "1");
	    print_sed_subst(out, "cFuncExecInitName", t->codel_task_start);
	} else {
	    print_sed_subst(out, "cFuncExecInitFlag", "0");
	    print_sed_subst(out, "cFuncExecInitName", "dummy");
	}

	/* end func */
	if (t->codel_task_end != NULL) {
	    print_sed_subst(out, "cFuncExecEndFlag", "1");
	    print_sed_subst(out, "cFuncExecEndName", t->codel_task_end);
	} else {
	    print_sed_subst(out, "cFuncExecEndFlag", "0");
	    print_sed_subst(out, "cFuncExecEndName", "dummy");
	}

	/* codel_task_main */
	if (t->codel_task_main != NULL) {
	    print_sed_subst(out, "cFuncExecFlag", "1");
	    print_sed_subst(out, "cFuncExecName", t->codel_task_main);
	} else {
	    print_sed_subst(out, "cFuncExecFlag", "0");
	    print_sed_subst(out, "cFuncExecName", "dummy");
	}
	
	/* 
	 * Creation des posters 
	 */
	str = NULL;
	str2 = NULL;
	i = 0;
	for (p = posters; p != NULL; p = p->next) {
	    if (p->exec_task != t) {
		continue;
	    }
	    if (p->address == NULL) {
	      
	      /* create posters */
		if (p->codel_poster_create == NULL) {
		    /* appel standard de posterCreate */
		    bufcat(&str, "  if (posterCreate(%s_%s_POSTER_NAME,\n",
			   module->NAME, p->NAME);
		} else {
		    /* Appel d'une fonction utilisateur pour creer le 
		       poster */
		    bufcat(&str, "  if (%s(%s_%s_POSTER_NAME,\n",
			   p->codel_poster_create, module->NAME, p->NAME);
		}
		bufcat(&str, "    sizeof(%s),\n", p->type->name);
		bufcat(&str, "    &(EXEC_TASK_POSTER_ID(%d)[%d])) != OK) {\n"
		       "     logMsg(\"%s%sInitTaskFunc: cannot create poster %s\\\\n\");\n"
		       "     return(ERROR);\n    }\n", 
		       t->num, i,
		       module->name, t->name, p->name);

		/* init posters created automatically */
		if (p->codel_poster_create == NULL) {
		  bufcat(&str, 
		         "    {\n"
			 "       int size = sizeof(%s);\n"
		         "       char *tmp = malloc(size);\n",
			 p->type->name);
		  bufcat(&str, 
			 "       if (tmp == NULL) {\n"
			 "          fprintf (stderr, \"%s%sInitTaskFunc : not enough mem to init poster %s\\\\n\");\n"
			 "       }\n",
			 module->name, t->name, p->name);
		  bufcat(&str, 
			 "       else {\n"
			 "          memset(tmp, 0, size);\n"
			 "          if (posterWrite(EXEC_TASK_POSTER_ID(%d)[%d], 0, tmp, size) != size) {\n"
			 "             fprintf (stderr, \"%s%sInitTaskFunc : cannot init poster %s\\\\n\");\n"
			 "             free(tmp);\n"
			 "             return(ERROR);\n"
			 "          }\n" 
			 "          free(tmp);\n"
			 "       }\n"
			 "    }\n",
			 t->num, i,
			 module->name, t->name, p->name);
		}

 		/* also func to delete poster */
		bufcat(&str2, "  if (EXEC_TASK_POSTER_ID(%d)[%d] != (POSTER_ID)NULL)\n",
		       t->num, i);
		bufcat(&str2,
		       "    posterDelete(EXEC_TASK_POSTER_ID(%d)[%d]);\n",
		       t->num, i);

		    
	    } else {
		/* Definition des posters avec adresse */
		bufcat(&str, "  if (posterMemCreate(%s_%s_POSTER_NAME,\n",
		       module->NAME, p->NAME);
		bufcat(&str, "    %s, (void *)0x%x,\n", 
		       p->bus_space, p->address);
		bufcat(&str, "    sizeof(%s),\n", p->type->name);
		bufcat(&str, "    &(EXEC_TASK_POSTER_ID(%d)[%d])) != OK)\n"
		       "    return(ERROR);\n", t->num, i);

		bufcat(&str2, "  if (EXEC_TASK_POSTER_ID(%d)[%d] != (POSTER_ID)NULL)\n",
		       t->num, i);
		bufcat(&str2,
		       "    posterDelete(EXEC_TASK_POSTER_ID(%d)[%d]);\n",
		       t->num, i);
	    }
	    i++;
	    if (i > MAX_POSTERS) {
		fprintf(stderr, "Error: Trop de posters pour %s\n", t->name);
		break;
	    }
	}
	if (str != NULL) {
	    print_sed_subst(out, "listPosterCreate", str);
	    free(str);
	    print_sed_subst(out, "listPosterDelete", str2);
	    free(str2);
	} else {
	    print_sed_subst(out, "listPosterCreate", "/* aucun poster */\n");
	    print_sed_subst(out, "listPosterDelete", "/* aucun poster */\n");
	}
	
	/*
	 * liste des serveurs 
	 */
	if (t->cs_client_from == NULL) {
	    print_sed_subst(out, "csServersFlag", "0");
	    print_sed_subst(out, "maxMboxReplySize", "");
	    print_sed_subst(out, "listServerClientInit", "");
	    print_sed_subst(out, "listServerClientEnd", "");
	} else {
	    print_sed_subst(out, "csServersFlag", "1");
	    /* taille max des repliques */
	    str = NULL;
	    for (ln = t->cs_client_from; ln != NULL; ln = ln->next) {
		bufcat(&str, "max(%s_CLIENT_MBOX_REPLY_SIZE, ",
		       ln->NAME);
	    }
	    bufcat(&str, "0");
	    for (ln = t->cs_client_from; ln != NULL; ln = ln->next) {
		bufcat(&str, ")");
	    }
	    print_sed_subst(out, "maxMboxReplySize", str);
	    free(str);
	    /* liste des fonctions d'Init */
	    str = NULL;
	    str2 = NULL;
	    for (ln = t->cs_client_from; ln != NULL; ln = ln->next) {
		bufcat(&str, 
		       "    if (%sClientInit(&(%s_%s_%s_CLIENT_ID))!=OK) {\n"
		       "       h2perror(\"%s%sInitTaskFunc: %sClientInit\");\n"
		       "       return ERROR;\n"
		       "    }\n",
		       ln->name, module->NAME, t->NAME, ln->NAME,
		       module->name, t->name, ln->name);
		bufcat(&str2, "    %sClientEnd(%s_%s_%s_CLIENT_ID);\n",
		       ln->name, module->NAME, t->NAME, ln->NAME);
	    }
	    print_sed_subst(out, "listServerClientInit", str);
	    free(str);
	    print_sed_subst(out, "listServerClientEnd", str2);
	    free(str2);
	}

	/*
	 * liste des posters lus
	 */
	if (t->poster_client_from == NULL) {
	    print_sed_subst(out, "listPosterInit", "");
	} else {
	    /* liste des posterInit */
	    str = NULL;
	    for (ln = t->poster_client_from; ln != NULL; ln = ln->next) {
		bufcat(&str, "  if (%sPosterInit() == ERROR) {\n"
		       "     logMsg(\"%s%sInitTaskFunc: %sPosterInit failed\\\\n\");\n"
		       "     return(ERROR);\n   }\n", 
		       ln->name, module->name, t->name, ln->name);
	    }
	    print_sed_subst(out, "listPosterInit", str);
	    free(str);
	    str = NULL;
	}

	/*
	 * liste des execodel_task_main 
	 */
	str = NULL;
	bufcat(&str, "static ACTIVITY_EVENT (*%s%sExecFuncTab[])() = {\n", 
	       module->name, t->name);
	for (lr = requetes; lr != NULL; lr = lr->next) {
	    r = lr->rqst;
	    if (r->codel_main != NULL && r->exec_task == t) {
		bufcat(&str, "  %s,\n", r->codel_main);
	    } else {
		bufcat(&str, "  NULL,\n");
	    }
	} /* for */
	bufcat(&str, "  NULL};\n");
	print_sed_subst(out, "execFuncTabDeclare", str);
	free(str);

	/* 
	 * liste des execodel_task_main_start 
	 */
	str = NULL;
	bufcat(&str, "static ACTIVITY_EVENT (*%s%sExecFuncStartTab[])()"
	       " = {\n", module->name, t->name);
	for (lr = requetes; lr != NULL; lr = lr->next) {
	    r = lr->rqst;
	    if (r->codel_start != NULL && r->exec_task == t) {
		bufcat(&str, "  %s,\n", r->codel_start);
	    } else {
		bufcat(&str, "  NULL,\n");
	    }
	} /* for */
	bufcat(&str, "  NULL};\n");
	print_sed_subst(out, "execFuncTabStartDeclare", str);
	free(str);

	/* 
	 * liste des execodel_task_main_end 
	 */
	str = NULL;
	bufcat(&str, "static ACTIVITY_EVENT (*%s%sExecFuncEndTab[])()"
	       " = {\n", module->name, t->name);
	for (lr = requetes; lr != NULL; lr = lr->next) {
	    r = lr->rqst;
	    if (r->codel_end != NULL && r->exec_task == t) {
		bufcat(&str, "  %s,\n", r->codel_end);
	    } else {
		bufcat(&str, "  NULL,\n");
	    }
	} /* for */
	bufcat(&str, "  NULL};\n");
	print_sed_subst(out, "execFuncTabEndDeclare", str);
	free(str);

	/* 
	 * liste des execodel_task_main_fail 
	 */
	str = NULL;
	bufcat(&str, "static ACTIVITY_EVENT (*%s%sExecFuncFailTab[])()"
	       " = {\n", module->name, t->name);
	for (lr = requetes; lr != NULL; lr = lr->next) {
	    r = lr->rqst;
	    if (r->codel_fail != NULL && r->exec_task == t) {
		bufcat(&str, "  %s,\n", r->codel_fail);
	    } else {
		bufcat(&str, "  NULL,\n");
	    }
	} /* for */
	bufcat(&str, "  NULL};\n");
	print_sed_subst(out, "execFuncTabFailDeclare", str);
	free(str);

	/* 
	 * liste des execodel_task_main_inter 
	 */
	str = NULL;
	bufcat(&str, "static ACTIVITY_EVENT (*%s%sExecFuncInterTab[])()"
	       " = {\n", module->name, t->name);
	for (lr = requetes; lr != NULL; lr = lr->next) {
	    r = lr->rqst;
	    if (r->codel_inter != NULL && r->exec_task == t) {
		bufcat(&str, "  %s,\n", r->codel_inter);
	    } else {
		bufcat(&str, "  NULL,\n");
	    }
	} /* for */
	bufcat(&str, "  NULL};\n");
	print_sed_subst(out, "execFuncTabInterDeclare", str);
	free(str);

	/* 
	 * Fonction de mise a jour des posters
	 */
	str = NULL;
	i = 0;
	for (p = posters; p != NULL; p = p->next) {
	    if (p->exec_task != t || p->data == NULL || p->update == USER) {
		continue;
	    }
	    bufcat(&str, "  {\n");
	    bufcat(&str, "  %s x;\n", p->type->name);
	    bufcat(&str, "  int offset, size;\n\n");
	    for (lm = p->data; lm != NULL; lm = lm->next) {
		bufcat(&str, "  offset = (int)&(x.%s) - (int)&(x);\n", 
		       lm->str_ref->dcl_nom->name);
		bufcat(&str, "  size = sizeof(x.%s);\n",
		       lm->str_ref->dcl_nom->name);
		bufcat(&str, "  if (posterWrite(%s_%s_POSTER_ID, offset, "
		       "(char *)&(%sDataStrId->",
		       module->NAME, p->NAME, module->name);
		for (ln = lm->str_ref->sdi_ref; ln != NULL; ln = ln->next) {
		    bufcat(&str, "%s%c", ln->name,
			   ln->next != NULL ? '.' : ')');
		} /* for */
		bufcat(&str, ", size) != size) %s%sSuspend (TRUE);\n",
		       module->name, t->name);
	    } /* for */
	    bufcat(&str, "  }");
	    i++;
	} /* for */
	
	print_sed_subst(out, "listPosterUpdateFunc", str);
	free(str);
	
	subst_end(out);
	script_close(out, "server/%s%s.c", module->name, t->name);

    } /* for */
    return(0);
} /* execTaskGen */

