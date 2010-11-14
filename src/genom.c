/*
 * Copyright (c) 1993-2003,2009 LAAS/CNRS                      Tue Jul 13 1993
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

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <sys/param.h>

#include "genom.h"
#include "extern_processes.h"
#include <unistd.h>
#include "parser.tab.h"

/* Ces includes sont nécessaires pour récurérer SERV_NMAX_RQST_ID */
#include <portLib.h>
#include <csLib.h>


POSTERS_INPUT_LIST *posters_input=NULL;

ID_LIST*
push_back(ID_LIST* list, ID_LIST* item)
{
    ID_LIST* last;

    item->next = NULL;
    if (list == NULL)
	return item;

    for (last = list; last->next != NULL; last = last->next);
    last->next = item;
    return list;
}

/***
 *** Fonctions d'usage ge'ne'ral dans genom
 ***/

/*----------------------------------------------------------------------*/

static char*
strcpytoupper(char const* value)
{
    char* result = (char*)xalloc(strlen(value) + 1);
    int i;
    for (i = 0; value[i] != '\0'; ++i)
    {
	if (value[i] == '-' || value[i] == '.')
	    result[i] = '_';
	else
	    result[i] = toupper(value[i]);
    }
    result[i] = '\0';
    return result;
}

/**
 ** Conversion des noms de module, de taches, de posters et de requetes
 ** en majuscules
 **/
void
upCaseNames(void)
{
    int i;
    EXEC_TASK_LIST *lt;
    EXEC_TASK_STR *t;
    RQST_LIST *lr;
    RQST_STR *r;
    POSTER_LIST *p;
    POSTERS_INPUT_LIST *p_in;
    ID_LIST *ln;

    /* nom du module */
    if (module->name != NULL) {
	module->NAME = (ID_STR *)xalloc(strlen(module->name) + 1);
	for (i = 0; module->name[i] != '\0';
		    module->NAME[i] = toupper(module->name[i]), i++);
	module->NAME[i] = '\0';
    } else {
	fprintf(stderr, "%s: no module name\n", nomfic);
	exit(2);
    }

    /* Import from */
    for (ln = imports; ln != NULL; ln = ln->next)
	ln->NAME = strcpytoupper(ln->name);

    /* Required packages */
    for (ln = requires; ln != NULL; ln = ln->next) {
	ln->NAME = strcpytoupper(ln->name);
	/* stop at first blank */
	{ char *tmp = ln->NAME; strsep(&tmp, " \t"); }
    }

    for (ln = codels_requires; ln != NULL; ln = ln->next) {
	ln->NAME = strcpytoupper(ln->name);
	/* stop at first blank */
	{ char *tmp = ln->NAME; strsep(&tmp, " \t"); }
    }

    /* Noms des taches d'execution */
    for (lt = taches; lt != NULL; lt = lt->next) {
	t = lt->exec_task;
	t->NAME = strcpytoupper(t->name);
    } /* for */

    /* Noms des requetes */
    for (lr = requetes; lr != NULL; lr = lr->next) {
	r = lr->rqst;
	r->NAME = strcpytoupper(r->name);
    } /* for */

    /* Noms des posters */
    for (p = posters; p != NULL; p = p->next)
	p->NAME = strcpytoupper(p->name);

    /* Posters_input */
    for (p_in = posters_input; p_in != NULL; p_in = p_in->next)
	p_in->NAME = strcpytoupper(p_in->name);


} /* upCaseNames */

void
genom_get_requires(char* filename, char* cppOptions[])
{
    /* Call cpp first, to use #ifdef's */
    char* processed_file = callCpp(filename, cppOptions, 1);

    extern FILE *yyin;
    extern YYSTYPE yylval;
    extern int yylex(void);
    extern int num_ligne;
    extern char nomfic[];

    int token, pkg;
    ID_LIST* il;

    yyin = fopen(processed_file, "r");
    if (!yyin)
    {
	printf("genom: Unable to open %s for reading\n", filename);
	unlink(processed_file);
	exit(1);
    }

    /* Go for the primitive parsing ... :p */
    while ((token = yylex())) switch(token) {
      case REQUIRE:
      case CODELS_REQUIRE:
	if (yylex() != ':') {
	  fprintf(stderr, "%s:%d: syntax error, missing ':'",
	      nomfic, num_ligne);
	  break;
	}

	while ((pkg = yylex()) != ';') switch(pkg) {
	  case ',': break;

	  case PACKAGENAME:
	  case IDENTIFICATEUR:
	  case QUOTED_STRING:
	    il = STR_ALLOC(ID_LIST);
	    il->name = strdup(yylval.idStr);
	    il->next = 0;
	    if (token == REQUIRE)
	      requires = push_back(requires, il);
	    else
	      codels_requires = push_back(codels_requires, il);

	    /* XXX don't we leak some memory allocated by yylex in yylval? */
	    break;

	  default:
	    fprintf(stderr, "%s:%d: syntax error", nomfic, num_ligne);
	    break;
	}

	break;
    }

    unlink(processed_file);
    fclose(yyin);
}

/*----------------------------------------------------------------------*/

/*
 * Affectation d'un membre a` la structure du module
 * a` partir d'une paire (attribut valeur)
 */
MODULE_STR *
ajout_av_module(MODULE_AV_STR *av, MODULE_STR *module)
{
    switch (av->attribut) {
      case INTERNAL_DATA:
	if (module->internal_data) {
	  fprintf(stderr,
		  "genom %s: Error: twice field 'internal_data' in module %s\n",
		  nomfic, module->name);
	  exit(2);
	}
	module->internal_data = av->value.internal_data;
	break;
      case NUMBER:
	if (module->number) {
	  fprintf(stderr,
		  "genom %s: Error: twice field 'number' in module %s\n",
		  nomfic, module->name);
	  exit(2);
	}
	module->number = av->value.number;
	break;
      case CODEL_FILES:
	if (module->codel_files) {
	  fprintf(stderr,
		  "genom %s: Error: twice field 'codel_files' in module %s\n",
		  nomfic, module->name);
	  exit(2);
	}
	module->codel_files = av->value.codel_files;
	break;
      case VERSION:
	if (module->version) {
	  fprintf(stderr,
		  "genom %s: Error: twice field 'version' in module %s\n",
		  nomfic, module->name);
	  exit(2);
	}
	module->version = av->value.version;
	break;
      case IFACE_VERSION:
	if (av->value.iface_version->current < av->value.iface_version->age)
	{
	    fprintf(stderr, "error: iface_version should have CURRENT >= AGE. Please check GenoM manual\n");
	    exit(1);
	}
	if (module->iface_version) {
	  fprintf(stderr,
		  "genom %s: Error: twice field 'iface_version' in module %s\n",
		  nomfic, module->name);
	  exit(2);
	}
	module->iface_version = av->value.iface_version;
	break;
      case EMAIL:
	if (module->email) {
	  fprintf(stderr,
		  "genom %s: Error: twice field 'email' in module %s\n",
		  nomfic, module->name);
	  exit(2);
	}
	module->email = av->value.email;
	break;
      case LANG:
	if (module->lang != MODULE_LANG_DEFAULT) {
	  fprintf(stderr,
		  "genom %s: Error: duplicate field 'lang' in module %s\n",
		  nomfic, module->name);
	  exit(2);
	}
	module->lang = av->value.lang;
	break;
      case CLKRATE:
	if (module->clkrate) {
	  fprintf(stderr,
		  "genom %s: Error: duplicate field 'clkrate' in module %s\n",
		  nomfic, module->name);
	  exit(2);
	}
	module->clkrate = av->value.clkrate;
	break;
    }
    return(module);
} /* ajout_av_module */

/*----------------------------------------------------------------------*/

/**
 ** Affectation d'un membre a` une structure de reque^te
 ** a` partir d'une paire (attribut valeur)
 **/
RQST_STR *
ajout_av_requete(RQST_AV_STR *av, RQST_STR *rqst)
{
    switch (av->attribut) {
      case TYPE:
	if (rqst->type) {
	  fprintf(stderr,
		  "genom %s: Error: twice field 'type:' in request %s",
		  nomfic, rqst->name);
	  exit(2);
	}
	rqst->type = av->value.type;
	break;
      case INPUT:
	if (rqst->input) {
	  fprintf(stderr,
		  "genom %s: Error: twice field 'input:' in request %s",
		  nomfic, rqst->name);
	  exit(2);
	}
	rqst->input = av->value.input;
	break;
      case POSTERS_INPUT:
	if (rqst->posters_input_types) {
	  fprintf(stderr,
		  "genom %s: Error: twice field 'posters_input_types:' in request %s",
		  nomfic, rqst->name);
	  exit(2);
	}
	rqst->posters_input_types = av->value.posters_input_types;
	break;
      case OUTPUT:
	if (rqst->output) {
	  fprintf(stderr,
		  "genom %s: Error: twice field 'output:' in request %s",
		  nomfic, rqst->name);
	  exit(2);
	}
	rqst->output = av->value.input;
	break;
      case CODEL_CONTROL:
	if (rqst->codel_control) {
	  fprintf(stderr,
		  "genom %s: Error: twice field 'codel_control:' in request %s",
		  nomfic, rqst->name);
	  exit(2);
	}
	rqst->codel_control = av->value.codel_control;
	break;
      case CODEL_MAIN:
	if (rqst->codel_main) {
	  fprintf(stderr,
		  "genom %s: Error: twice field 'codel_main:' in request %s",
		  nomfic, rqst->name);
	  exit(2);
	}
	rqst->codel_main = av->value.codel_main;
	break;
      case CODEL_START:
	if (rqst->codel_start) {
	  fprintf(stderr,
		  "genom %s: Error: twice field 'codel_start:' in request %s",
		  nomfic, rqst->name);
	  exit(2);
	}
	rqst->codel_start = av->value.codel_start;
	break;
      case CODEL_END:
	if (rqst->codel_end) {
	  fprintf(stderr,
		  "genom %s: Error: twice field 'codel_end:' in request %s",
		  nomfic, rqst->name);
	  exit(2);
	}
	rqst->codel_end = av->value.codel_end;
	break;
      case CODEL_FAIL:
	if (rqst->codel_fail) {
	  fprintf(stderr,
		  "genom %s: Error: twice field 'codel_fail:' in request %s",
		  nomfic, rqst->name);
	  exit(2);
	}
	rqst->codel_fail = av->value.codel_fail;
	break;
      case CODEL_INTER:
	if (rqst->codel_inter) {
	  fprintf(stderr,
		  "genom %s: Error: twice field 'codel_inter:' in request %s",
		  nomfic, rqst->name);
	  exit(2);
	}
	rqst->codel_inter = av->value.codel_inter;
	break;
      case INTERRUPT_ACTIVITY:
	if (rqst->interrupt_activity) {
	  fprintf(stderr,
		  "genom %s: Error: twice field 'interrupt_activity:' in request %s",
		  nomfic, rqst->name);
	  exit(2);
	}
	rqst->interrupt_activity = av->value.interrupt_activity;
	break;
      case EXEC_TASK:
	rqst->exec_task = NULL;
	if (rqst->exec_task_name) {
	  fprintf(stderr,
		  "genom %s: Error: twice field 'exec_task_name:' in request %s",
		  nomfic, rqst->name);
	  exit(2);
	}
	rqst->exec_task_name = av->value.exec_task_name;
	break;
      case FAIL_REPORTS:
	if (rqst->fail_reports) {
	  fprintf(stderr,
		  "genom %s: Error: twice field 'fail_reports:' in request %s",
		  nomfic, rqst->name);
	  exit(2);
	}
	rqst->fail_reports = av->value.fail_reports;
	break;
      case RESOURCES:
	if (rqst->resource_list) {
	  fprintf(stderr,
		  "genom %s: Error: twice field 'resource_list:' in request %s",
		  nomfic, rqst->name);
	  exit(2);
	}
	rqst->resource_list = av->value.resource_list;
	break;
    case RQST_NUM:
	if (rqst->rqst_num) {
	  fprintf(stderr,
		  "genom %s: Error: twice field 'rqst_num:' in request %s",
		  nomfic, rqst->name);
	  exit(2);
	}
	rqst->rqst_num = av->value.rqst_num;
	break;
    case RQST_DOC:
	if (rqst->doc) {
	  fprintf(stderr,
		  "genom %s: Error: twice field 'doc:' in request %s",
		  nomfic, rqst->name);
	  exit(2);
	}
	rqst->doc = av->value.doc;
	break;
    case RQST_INPUT_INFO:
	if (rqst->input_info) {
	  fprintf(stderr,
		  "genom %s: Error: twice field 'input_info:' in request %s",
		  nomfic, rqst->name);
	  exit(2);
	}
	rqst->input_info = av->value.input_info;
	break;
    }
    return(rqst);
} /* ajout_av_requete */

/*----------------------------------------------------------------------*/

/**
 ** Affectation d'un membre a` une structure de ta^che
 ** a` partir d'une paire (attribut valeur)
 **/
EXEC_TASK_STR *
ajout_av_tache(EXEC_TASK_AV_STR *av, EXEC_TASK_STR *task)
{
    switch (av->attribut) {
      case PERIOD:
	if (task->period) {
	  fprintf(stderr,
		  "genom %s: Error: twice filed 'period' in task %s\n",
		  nomfic, task->name);
	  exit(2);
	}
	task->period = av->value.period;
	break;
      case T_DELAY:
	if (task->delay) {
	  fprintf(stderr,
		  "genom %s: Error: twice field 'delay' in task %s\n",
		  nomfic, task->name);
	  exit(2);
	}
	task->delay = av->value.delay;
	break;
      case PRIORITY:
	if (task->priority) {
	  fprintf(stderr,
		  "genom %s: Error: twice field 'priority' in task %s\n",
		  nomfic, task->name);
	  exit(2);
	}
	task->priority = av->value.priority;
	break;
      case STACK_SIZE:
	if (task->stack_size) {
	  fprintf(stderr,
		  "genom %s: Error: twice field 'stack_size' in task %s\n",
		  nomfic, task->name);
	  exit(2);
	}
	task->stack_size = av->value.stack_size;
	break;
      case CODEL_TASK_START:
	if (task->codel_task_start) {
	  fprintf(stderr,
		  "genom %s: Error: twice field 'codel_task_start' in task %s\n",
		  nomfic, task->name);
	  exit(2);
	}
	task->codel_task_start = av->value.codel_task_start;
	break;
      case CODEL_TASK_END:
	if (task->codel_task_end) {
	  fprintf(stderr,
		  "genom %s: Error: twice field 'codel_task_end' in task %s\n",
		  nomfic, task->name);
	  exit(2);
	}
	task->codel_task_end = av->value.codel_task_end;
	break;
      case CODEL_TASK_MAIN:
	if (task->codel_task_main) {
	  fprintf(stderr,
		  "genom %s: Error: twice field 'codel_task_main' in task %s\n",
		  nomfic, task->name);
	  exit(2);
	}
	task->codel_task_main = av->value.codel_task_main;
	break;
      case CODEL_TASK_MAIN2:
	if (task->codel_task_main2) {
	  fprintf(stderr,
		  "genom %s: Error: twice field 'codel_task_main2' in task %s\n",
		  nomfic, task->name);
	  exit(2);
	}
	task->codel_task_main2 = av->value.codel_task_main2;
	break;
      case POSTERS_INPUT:
	if (task->posters_input_types) {
	  fprintf(stderr,
		  "genom %s: Error: twice field 'posters_input_types' in task %s\n",
		  nomfic, task->name);
	  exit(2);
	}
	task->posters_input_types = av->value.posters_input_types;
	break;
      case CS_CLIENT_FROM:
	task->cs_client_from = av->value.cs_client_from;
	fprintf(stderr,
		"genom %s: warning: old fashion declaration \"cs_client_from: %s\"\n",
		nomfic, av->value.cs_client_from->name);
	break;
      case POSTER_CLIENT_FROM:
	task->poster_client_from = av->value.poster_client_from;
	fprintf(stderr,
		"genom %s: warning: old fashion declaration \"poster_client_from: %s\"\n",
		nomfic, av->value.poster_client_from->name);
	break;
      case RESOURCES:
	if (task->resource_list) {
	  fprintf(stderr,
		  "genom %s: Error: twice field 'resource_list' in task %s\n",
		  nomfic, task->name);
	  exit(2);
	}
	task->resource_list = av->value.resource_list;
	break;
      case FAIL_REPORTS:
	if (task->fail_reports) {
	  fprintf(stderr,
		  "genom %s: Error: twice field 'fail_reports' in task %s\n",
		  nomfic, task->name);
	  exit(2);
	}
	task->fail_reports = av->value.fail_reports;
	break;
    }
    return(task);
} /* ajout_av_tache */

/*----------------------------------------------------------------------*/

/**
 ** Recherche de la de'finition d'un type par le nom d'un TYPEDEF
 **/
TYPE_STR *
trouve_type(TYPE_STR *t)
{
    TYPE_LIST *lt;
    TYPE_STR *tr;
    DCL_NOM_LIST *ltypedef;

    if (t == NULL) {
	return(NULL);
    }
    if (t->type != TYPEDEF) {
	return(t);
    }

    tr = NULL;

    /* recheche dans la liste des typedefs */
    for (ltypedef = typedefs; ltypedef != NULL; ltypedef = ltypedef->next) {
	if (!strcmp(t->name, ltypedef->dcl_nom->name)) {
	    tr = ltypedef->dcl_nom->type;
	    markUsed(tr);
	    break;
	}
    }
    if (tr != NULL) {
	return(tr);
    }

    /* recherche dans la liste des types */
    for (lt = types; lt != NULL; lt = lt -> next) {
	if (!strcmp(t->name, lt->type->name)) {
	    tr = lt->type;
	    markUsed(tr);
	    break;
	}
    }
    return(tr);
} /* trouve_type */

/*----------------------------------------------------------------------*/

/**
 ** Recherche de la de'finition d'un type par le nom d'un TYPEDEF
 **/
DCL_NOM_STR *
trouve_typedef(TYPE_STR *t)
{
    DCL_NOM_STR *tr;
    DCL_NOM_LIST *ltypedef;

    if (t == NULL) {
	return(NULL);
    }
    if (t->type != TYPEDEF) {
	return(NULL);
    }

    tr = NULL;

    /* recheche dans la liste des typedefs */
    for (ltypedef = typedefs; ltypedef != NULL; ltypedef = ltypedef->next) {
	if (!strcmp(t->name, ltypedef->dcl_nom->name)) {
	    tr = ltypedef->dcl_nom;
	    markUsed(tr->type);
	    break;
	}
    }
    return(tr);
} /* trouve_typedef */

/*----------------------------------------------------------------------*/

/**
 ** Indique si une requete est reentrante
 ** (si elle est compatible avec elle-meme)
 **/
int
reentrant(RQST_STR *r)
{
    RQST_LIST *l;

    for (l = r->interrupt_activity; l != NULL; l = l->next) {
	if (l->rqst == r) {
	    break;
	}
    }
    if (l == NULL) {
	return(1);
    } else {
	return(0);
    }
}

/*----------------------------------------------------------------------*/

/**
 ** Parcours les structures de donne'es pour instancier les types
 **/
void
resolveTypes(void)
{
    RQST_LIST *lr;
    RQST_STR *r;
    TYPE_STR *tr, *t_poster, *t_sdi, *t_rqst, *t_tmp;
    DCL_NOM_LIST *lt;
    TYPE_LIST *ltt;
    TYPE_LIST *t;
    char buf[80];
    DCL_NOM_LIST *m, *last, *lastData = 0;
    DCL_NOM_STR *n;
    STR_REF_LIST *lm, *lastType = 0;
    int i;
    EXEC_TASK_LIST *le;
    POSTER_LIST *p;
    POSTERS_INPUT_LIST *p_in;

    /* Type de la structure interne du module */
    t_sdi = module->internal_data;
    tr = trouve_type(t_sdi);
    if (tr == NULL) {
	fprintf(stderr,
		"genom %s: Error: SDI type %s unknown\n",
		nomfic, module->internal_data->name);
	exit(2);
    } else {
	if (tr != t_sdi) {
	    free(t_sdi->name);
	    free(t_sdi->filename);
	    free(t_sdi);
	}
	markUsed(tr);
	tr->flags |= TYPE_INTERNAL_DATA;
	module->internal_data = tr;
    }

    /* types des parametres des requetes */
    for (lr = requetes; lr != NULL; lr = lr->next) {
	r = lr->rqst;

	/* inputs */
	if (r->input != NULL) {
	  if ((n = trouve_sdi_ref(r->input)) == NULL) {
	    fprintf(stderr, "genom %s: Error: no element %s in SDI for requete %s\n",
		    nomfic, r->input->sdi_ref->name, r->name);
		/* XXXX On ne sait pas mieux reprendre cette erreur  */
		exit(2);
	    } else {
		if (r->input->dcl_nom->type == NULL) {
		    free(r->input->dcl_nom->name);
		    free(r->input->dcl_nom);
		    r->input->dcl_nom = n;
		} else {
		    r->input->dcl_nom->ndimensions = n->ndimensions;
		    if (n->ndimensions != 0) {
			r->input->dcl_nom->dimensions
			    = xalloc(sizeof(int)* n->ndimensions);
		    } else {
			r->input->dcl_nom->dimensions = NULL;
		    }
		    for (i=0; i<n->ndimensions; i++) {
			r->input->dcl_nom->dimensions[i] = n->dimensions[i];
		    }
		    r->input->dcl_nom->pointeur = n->pointeur;
		}
	    }
	    t_rqst = r->input->dcl_nom->type;
	    tr = trouve_type(t_rqst);
	    if (tr != NULL && tr != t_rqst) {
		markUsed(tr);
		free(t_rqst->name);
		free(t_rqst->filename);
		free(t_rqst);
		t_rqst = tr;
	    }
	} /* input */

	/* outputs */
	if (r->output != NULL) {
	    if ((n = trouve_sdi_ref(r->output)) == NULL) {
		fprintf(stderr, "genom %s: Error: no element %s in SDI for requete %s\n", nomfic, r->output->sdi_ref->name, r->name);
		exit(2);
	    } else {
		if (r->output->dcl_nom->type == NULL) {
		    free(r->output->dcl_nom->name);
		    free(r->output->dcl_nom);
		    r->output->dcl_nom = n;
		} else {
		    r->output->dcl_nom->ndimensions = n->ndimensions;
		    if (n->ndimensions != 0) {
			r->output->dcl_nom->dimensions
			    = xalloc(sizeof(int)* n->ndimensions);
		    } else {
			r->output->dcl_nom->dimensions = NULL;
		    }
		    for (i=0; i<n->ndimensions; i++) {
			r->output->dcl_nom->dimensions[i] = n->dimensions[i];
		    }
		    r->output->dcl_nom->pointeur = n->pointeur;
		}
	    }
	    t_rqst = r->output->dcl_nom->type;
	    tr = trouve_type(t_rqst);
	    if (tr != NULL && tr != t_rqst) {
		markUsed(tr);
		free(t_rqst->name);
		free(t_rqst->filename);
		free(t_rqst);
		t_rqst = tr;
	    }
	} /* output */


	/* posters inputs */
	if (r->posters_input_types) {

	    /* Identifie/Enregistre chaque type de structure de poster */
	    for (ltt = r->posters_input_types; ltt != NULL; ltt = ltt->next) {

	      /* already done */
	      if (trouve_poster_input(ltt->type->name))
		continue;

	      /* Look for this data type */
	      t_tmp = ltt->type;
	      tr = trouve_type(t_tmp);
	      if (tr == NULL) {
		fprintf(stderr,
			"genom %s: Error: type of poster input of request %s: %s unknown\n",
			nomfic, r->name, t_tmp->name);
		exit(2);
	      }
	      if (tr != t_tmp) {
		  free(t_tmp->name);
		  free(t_tmp->filename);
		  free(t_tmp);
		  ltt->type = tr;
	      }
	      markUsed(tr);
	      /* tr->flags |= TYPE_POSTER; */

	      /* allocate a new poster description */
	      p_in = STR_ALLOC(POSTERS_INPUT_LIST);

	      p_in->type = tr;
	      p_in->name = strdup(tr->name);

	      /* add in poster input list */
	      if (!posters_input) {
		posters_input = p_in;
		posters_input->next = NULL;
	      }
	      else {
		p_in->next = posters_input;
		posters_input = p_in;
	      }

	    } /* for each type */

	} /* posters input */

    } /* for each request */

    /* typedefs */
    if (typedefs == NULL) {
	fprintf(stderr, "genom Warning: no new typedef\n");
	return;
    }

    for (last = typedefs; last != NULL && last->next != NULL;
	 last = last->next);

    /* Types des posters */
    for (p = posters; p != NULL; p = p->next) {

	/* On cree la structure du poster */
	t_poster = STR_ALLOC(TYPE_STR);
	t_poster->members = NULL;
	t_poster->type = STRUCT;
	sprintf(buf, "%s_%s_POSTER_STR", module->NAME, p->NAME);
	t_poster->name = strdup(buf);
	markUsed(t_poster);
	t_poster->flags = TYPE_POSTER;

	/* Liste de donnees refrencees dans la SDI */
	if (p->data != NULL) {

	    /* Identifie/Enregistre chaque element du poster */
	    for (lm = p->data; lm != NULL; lm = lm->next) {
		m = STR_ALLOC(DCL_NOM_LIST);

		/* Trouve la variable correspondante dans SDI */
		if ((n = trouve_sdi_ref(lm->str_ref)) == NULL) {
		  fprintf(stderr,
			  "genom %s: Error: no element %s in SDI for poster %s\n",
			  nomfic, lm->str_ref->sdi_ref->name, p->name);
		  exit(2);
		}

		/* Trouve le type correspondant */
		else {

		  /* Il faut recupere le type, MAIS AUSSI les caractéristiques
		     de la variable, MAIS sans ecraser le nom.
		     -> enregistrement du type est insufisant
			et recuperation directe de n erronee
			(sara Oct 96) */
		    lm->str_ref->dcl_nom->type = n->type;
		    lm->str_ref->dcl_nom->pointeur = n->pointeur;
		    lm->str_ref->dcl_nom->dimensions = n->dimensions;
		    lm->str_ref->dcl_nom->ndimensions = n->ndimensions;
		    lm->str_ref->dcl_nom->flags = n->flags;

		    if (trouve_type(lm->str_ref->dcl_nom->type) == NULL) {
			fprintf(stderr, "genom %s: Error: unknown type %s\n",
				nomfic, lm->str_ref->dcl_nom->type->name);
			exit(2);
		    }
		}

		/* Enregistrement, Ajout en queue de liste */
		m->dcl_nom = lm->str_ref->dcl_nom;
		m->next = NULL;
		if (t_poster->members == NULL)
		  t_poster->members = m;
		else
		  lastData->next = m;
		lastData = m;
	    } /* for each data */

	} /* poster with data */

	/* Poster user sans structure definie (data=null) */
	else {

	  /* Identifie/Enregistre chaque element du poster */
	  for (lt = p->types; lt != NULL; lt = lt->next) {

	    t_tmp = lt->dcl_nom->type;
	    tr = trouve_type(t_tmp);
	    if (tr == NULL) {
	      fprintf(stderr,
		      "genom %s: Error: type of poster %s: %s unknown\n",
		      nomfic, p->name, t_tmp->name);
	      exit(2);
	    } else {
	      if (tr != t_tmp) {
		free(t_tmp->name);
		free(t_tmp->filename);
		free(t_tmp);
		lt->dcl_nom->type = tr;
	      }
	      markUsed(tr);
	      /* tr->flags |= TYPE_POSTER; */
	    }

	    /* Enregistrement des types membre du poster Ajout en queue de liste */
	    if (t_poster->members == NULL)
	      t_poster->members = lt;

	    /* Enregistrement donnees */
	    if (p->data == NULL) {
	      p->data = STR_ALLOC(STR_REF_LIST);
	      lastType = p->data;
	    }
	    else {
	      lastType->next = STR_ALLOC(STR_REF_LIST);
	      lastType = lastType->next;
	    }
	    lastType->next = NULL;
	    lastType->str_ref = STR_ALLOC(STR_REF_STR);
	    lastType->str_ref->sdi_ref = NULL;
	    lastType->str_ref->dcl_nom = lt->dcl_nom;

	  } /* for */

	} /* p->data == NULL */

	/* Ajout de la structure du poster dans la liste des typedef */
	m = STR_ALLOC(DCL_NOM_LIST);
	n = STR_ALLOC(DCL_NOM_STR);
	n->name = strdup(t_poster->name);
	n->type = t_poster;
	m->dcl_nom = n;
	last->next = m;
	last = m;

	/* Ajout dans la liste des types */
	ajout_type(t_poster);
	/* Enregistrement dans la structure poster */
	p->type = t_poster;

    } /* for poster */


    /* posters input des taches d'exec */
    for (le = taches; le != NULL; le = le->next) {

	/* posters inputs */
	if (le->exec_task->posters_input_types) {

	    /* Identifie/Enregistre chaque type de structure de poster */
	    for (ltt = le->exec_task->posters_input_types; ltt != NULL; ltt = ltt->next) {

	      /* already done */
	      if (trouve_poster_input(ltt->type->name))
		continue;

	      /* Look for this data type */
	      t_tmp = ltt->type;
	      tr = trouve_type(t_tmp);
	      if (tr == NULL) {
		fprintf(stderr,
			"%s: Error: type of poster input of exec task %s: %s unknown\n",
			nomfic, le->exec_task->name, t_tmp->name);
		exit(2);
	      }
	      if (tr != t_tmp) {
		  free(t_tmp->name);
		  free(t_tmp->filename);
		  free(t_tmp);
		  ltt->type = tr;
	      }
	      markUsed(tr);
	      /* tr->flags |= TYPE_POSTER; */

	      /* allocate a new poster description */
	      p_in = STR_ALLOC(POSTERS_INPUT_LIST);

	      p_in->type = tr;
	      p_in->name = strdup(tr->name);

	      /* add in poster input list */
	      if (!posters_input) {
		posters_input = p_in;
		posters_input->next = NULL;
	      }
	      else {
		p_in->next = posters_input;
		posters_input = p_in;
	      }

	    } /* for each type */

	} /* posters input */

    } /* for each exec task */



    /* module internal data */
    if (module->internal_data) {
	tr = module->internal_data;

	for (last = tr->members; last != NULL && last->next != NULL;
	     last = last->next);

	/* creation des tableaux des structures d'E/S pour les fonctions
	   reentrantes */
	for (lr = requetes; lr != NULL; lr = lr->next) {
	    r = lr->rqst;
	    if (r->type == EXEC && reentrant(r)) {
		if (r->input != NULL) {
		    m = STR_ALLOC(DCL_NOM_LIST);
		    n = STR_ALLOC(DCL_NOM_STR);
		    m->dcl_nom = n;
		    sprintf(buf, "%sInput", r->name);
		    n->name = strdup(buf);
		    n->type = r->input->dcl_nom->type;
		    n->ndimensions = 1;
		    n->dimensions = STR_ALLOC(int);
		    n->dimensions[0] = SERV_NMAX_RQST_ID;
		    n->flags |= ARRAY;
		    m->next = NULL;
		    last->next = m;
		    last = m;
		    fprintf(stderr,
			    "genom %s: info: array %sInput added in SDI "
			    "for request %s\n",
			    nomfic, r->name, r->name);

		    /* Je suggere de REMPLACER la variable par le
		       tableau de variable (car normalement la variable
		       ne doit plus etre utilisee)
		       (Pb1: verifier qu'on a pas deja remplacer une fois
			Pb2: cas d'utilisation de la variable par des
			    requetes reentrante ET non-reentrante):

		    int nDim=r->input->dcl_nom->ndimensions;
		    DCL_NOM_STR *n=r->input->dcl_nom;
		    n->flags |= ARRAY;
		    n->dimensions = n->dimensions == NULL ?
				    xalloc(sizeof(int)) :
			     xrealloc(n->dimensions, (nDim+1)*sizeof(int));
		    for(i=0;i<nDim;i++)
		      n->dimensions[i+1]=n->dimensions[i];
		    n->dimensions[0]=SERV_NMAX_RQST_ID;
		    (n->ndimensions)++;*/
		}
		if (r->output != NULL) {
		    m = STR_ALLOC(DCL_NOM_LIST);
		    n = STR_ALLOC(DCL_NOM_STR);
		    m->dcl_nom = n;
		    sprintf(buf, "%sOutput", r->name);
		    n->name = strdup(buf);
		    n->type = r->output->dcl_nom->type;
		    n->ndimensions = 1;
		    n->dimensions = STR_ALLOC(int);
		    n->dimensions[0] = SERV_NMAX_RQST_ID;
		    n->flags |= ARRAY;
		    m->next = NULL;
		    last->next = m;
		    last = m;
		    fprintf(stderr,
			    "genom %s: info: array %sOutput added in SDI "
			    "for request %s\n",
			    nomfic, r->name, r->name);
		}

		/* Donnees d'entree et de sortie physiquement les memes */
		if (r->input != NULL && r->output != NULL &&
		    r->input->dcl_nom == r->output->dcl_nom) {
		  fprintf(stderr,
			  "genom %s: warning: input and output of "
			  "request %s "
			  "will no more refered to the "
			  "same data %s in SDI !\n",
			  nomfic, r->name, r->input->dcl_nom->name);
		}
	    }
	} /* for each request */

    } /* module internal data */

    /* types non utilise's */
    for (t = types; t != NULL; t = t->next) {
	if (t->type->used == 0 && ((t->type->flags & TYPE_IMPORT) == 0)) {
	    fprintf(stderr, "genom %s:%d: warning: type %s unused\n",
		    t->type->filename,
		    t->type->linenum,
		    t->type->name);
	}
    }
} /* resolveTypes */

/*----------------------------------------------------------------------*/

/**
 ** Recherche d'une requete par nom
 **/
RQST_STR *
trouve_requete(char *name)
{
    RQST_LIST *lr;

    for (lr = requetes; lr != NULL; lr = lr->next) {
	if (!strcmp(lr->name, name)) {
	    return(lr->rqst);
	}
    } /* for */
    return(NULL);
} /* trouve_requete */

/**
 ** Recherche d'un posters_input par nom
 **/
POSTERS_INPUT_LIST *
trouve_poster_input(char *name)
{
    POSTERS_INPUT_LIST *p_in;

    /* Noms des posters IN */
    for (p_in = posters_input; p_in != NULL; p_in = p_in->next) {
      if (!strcmp(p_in->name, name))
	return (p_in);
    } /* for */

    return NULL;
}
/*----------------------------------------------------------------------*/

/**
 ** Parcours des listes d'incompatibilite' pour instancier les requetes
 **/
void
resolveRequests(void)
{
    RQST_LIST *lr, *li;
    RQST_STR *r;
    int n = 0;

    for (lr = requetes; lr != NULL; lr = lr->next) {
	r = lr->rqst;
	r->num = n++;
	if (r->rqst_num < 0) r->rqst_num = r->num;
	/* Init requests are incompatible with all */
	if (r->type == INIT) {
	    if (r->interrupt_activity != NULL
		&& r->interrupt_activity->flags != ALL) {
		fprintf(stderr, "genom %s: warning: forcing "
			"init request to be incompatible with ALL\n", nomfic);
		free(r->interrupt_activity);
	    }
	    r->interrupt_activity = requetes;
	    continue;
	}
	if (r->interrupt_activity == NULL) {
	    continue;
	}
	if (r->interrupt_activity->flags == ALL) {
	    /* traite le cas particulier ALL */
	    free(r->interrupt_activity);
	    r->interrupt_activity = requetes;
	} else {
	    /* liste "normale" */
	    for (li = r->interrupt_activity; li != NULL; li = li->next) {
		r = trouve_requete(li->name);
		if (r == NULL) {
		    fprintf(stderr, "genom %s: warning: unknown request %s\n",
			    nomfic, li->name);
		} else {
		    li->rqst = r;
		}
	    } /* for */
	}
    } /* for */
    nbRequest = n;
} /* resolveRequests */

/*----------------------------------------------------------------------*/
/**
 ** Trouve le numero pour la requete d'abort
 **/
int
trouve_abort_num(void)
{
    RQST_LIST *l;
    RQST_STR *r;
    int i;

    /* Cherche un numero de requete en partant de 0*/
    i = 0;
    while (1) {
	/* Est-il deja utilise ? */
	for (l = requetes;  l != NULL; l = l->next) {
	    r = l->rqst;
	    /* perdu... numero utilise': on tente le suivant */
	    if (r->rqst_num == i) {
		i++;
		break;
	    }
	} /* for */
	/* Gagne' ! nume'ro pas utilise' */
	if (l == NULL) {
	    return(i);
	}
    } /* while */

} /* trouve_abort_num */

/*----------------------------------------------------------------------*/

/**
 ** Recheche d'une exec_task par nom
 **/
EXEC_TASK_STR *
trouve_exec_task(char *name)
{
    EXEC_TASK_LIST *lt;

    for (lt = taches; lt != NULL; lt = lt->next) {
	if (!strcmp(lt->exec_task->name, name)) {
	    return(lt->exec_task);
	}
    } /* for */
    return(NULL);
} /* trouve_exec_task */

/*----------------------------------------------------------------------*/

/**
 ** Fermeture des listes de taches
 **/
void
resolveTasks(void)
{
    RQST_LIST *lr;
    EXEC_TASK_LIST *lt;
    EXEC_TASK_STR *t;
    POSTER_LIST *p;
    int n = 0;

    /* Nume'rotation des Exec Tasks */
    for (lt = taches; lt != NULL; lt = lt->next) {
	lt->exec_task->num = n++;
    } /* for */
    nbExecTask = n;

    /* Instanciation des exec tasks dans les requetes */
    for (lr = requetes; lr != NULL; lr = lr->next) {
	if (lr->rqst->exec_task_name != NULL) {
	    lr->rqst->exec_task = trouve_exec_task(lr->rqst->exec_task_name);
	    if (lr->rqst->exec_task == NULL) {
		fprintf(stderr, "genom %s: Error: exec_task %s unknown\n",
			nomfic, lr->rqst->exec_task_name);
		exit(2);
	    }
	}
    } /* for requests */

    /* Instanciation des exec tasks dans les posters */
    for (p = posters; p != NULL; p = p->next) {
	if (p->exec_task->name != NULL) {
	    t = trouve_exec_task(p->exec_task->name);
	    if (t != NULL) {
		free(p->exec_task->name);
		free(p->exec_task);
		p->exec_task = t;
	    } else {
		fprintf(stderr, "genom %s: Error: exec_task %s unknown\n",
			nomfic, p->exec_task->name);
		exit(2);
	    }
	}
    } /* for posters */

} /* resolveTasks */

/*----------------------------------------------------------------------*/

/**
 ** Trouve la valeur d'un symbole (enum)
 **/
int
trouve_value(char *symbole, int *pVal)
{
    TYPE_LIST *lt;
    TYPE_STR *t;
    DCL_NOM_LIST *l;
    DCL_NOM_STR *m;
    int val;

    for (lt = types; lt != NULL; lt = lt->next) {
	t = lt->type;
	if (t->type == ENUM) {
	    val = 0;
	    for (l = t->members; l != NULL; l = l->next) {
		m = l->dcl_nom;
		if (strcmp(m->name, symbole) == 0 && m->flags == ENUM_VALUE) {
		    /* trouve' */
		    *pVal = m->pointeur;
		    return(1);
		}
	    } /* for */
	}
    } /* for */
    /* pas trouve' */
    return(0);
} /* enum_values */


/*----------------------------------------------------------------------*/

/**
 ** Ge'ne'ration d'un nouveau nom pour une structure ou une union anonyme
 **/
char *
new_name(void)
{
    char *p = xalloc(11*sizeof(char));
    static int unamed_compt = 0;

    fprintf(stderr, "genom %s: %d: warning: type without name\n",
	    nomfic, num_ligne);
    sprintf(p, "unamed%04d", unamed_compt++);
    return(p);
} /* new_name */

/*----------------------------------------------------------------------*/

/**
 **  Ge'ne'ration de la de'claration d'un type
 **/
void
dcl_nom_decl(DCL_NOM_STR *n, char **pType, char **pVar)
{
    TYPE_STR *t = n->type;
    DCL_NOM_LIST *l;
    int i;
    int vrai;

    /* recherche dans la liste des typedefs un nom pour le type */
    for (l = typedefs; l != NULL; l = l->next) {
	if (l->dcl_nom != n &&
	    l->dcl_nom->type == t) {
	    if (l->dcl_nom->pointeur == n->pointeur &&
		l->dcl_nom->ndimensions == n->ndimensions) {
		vrai = 1;
		for (i = 0; i < n->ndimensions; i++) {
		    vrai = vrai &&
			(n->dimensions[i] == l->dcl_nom->dimensions[i]);
		}
		if (vrai) {
		    /* trouve' un nom pour ce type exact */
		    *pType = strdup(l->dcl_nom->name);
		    *pVar = strdup(n->name);
		    return;
		}
	    } else if (l->dcl_nom->pointeur == 0
		       && l->dcl_nom->ndimensions == 0) {
		*pType = strdup(l->dcl_nom->name);

		*pVar = NULL;
		for (i = 0; i < n->pointeur; i++) {
		    bufcat(pVar, "*");
		}
		bufcat(pVar, n->name);
		for (i = 0; i < n->ndimensions; i++) {
		    bufcat(pVar, "[%d]", n->dimensions[i]);
		};
		return;
	    }
	}
    } /* for */

    /* pas trouve' de nom exact */

    /* type */
    *pType = nom_type(t);

    /* variable */
    *pVar = NULL;
    for (i = 0; i < n->pointeur; i++) {
	bufcat(pVar, "*");
    }
    bufcat(pVar, n->name);
    for (i = 0; i < n->ndimensions; i++) {
	bufcat(pVar, "[%d]", n->dimensions[i]);
    };
} /* dcl_nom_decl */

/*----------------------------------------------------------------------*/

/**
 ** Calcul de la taille d'un objet
 **/
int
taille_obj(DCL_NOM_STR *t)
{
    TYPE_STR *t1;
    DCL_NOM_LIST *m;
    DCL_NOM_STR *o;
    int tt, tot, max_taille;
    int i;

    if (t == NULL)
	return(0);

    /* compte le nombre d'elements tt dans le cas d'un tableau */
    tt = 1;
    for (i = 0; i < t->ndimensions; i++) {
	tt *= t->dimensions[i];
    }
    if (! t->pointeur) {
	/* types simples */
	switch (t->type->type) {
	  case CHAR:
	    return(tt);
	  case SHORT:
	    return(tt*sizeof(short));
	  case INT:
		  if (t->type->flags & SHORT_INT)
			  return(tt*sizeof(short));
		  else if (t->type->flags & LONG_INT)
			  return(tt*sizeof(long));
		  else if (t->type->flags & LONG_LONG_INT)
			  return(tt*sizeof(long long));
		  else
			  return(tt*sizeof(int));
	  case ENUM:
	    return(tt*sizeof(int));
	  case FLOAT:
	    return(tt*sizeof(float));
	  case DOUBLE:
	    return(tt*sizeof(double));
	  case STRUCT:
	    /* structures -> somme de la taille des membres */
	    tot = 0;
	    for (m = t->type->members; m != NULL; m = m->next) {
		tot += taille_obj(m->dcl_nom);
	    }
	    return(tt*tot);
	  case UNION:
	    /* unions -> max de la taille des membres */
	    max_taille = 0;
	    for (m = t->type->members; m != NULL; m = m->next) {
		tot = taille_obj(m->dcl_nom);
		if (tot > max_taille)
		    max_taille = tot;
	    }
	    return(tt*max_taille);
	  case TYPEDEF:
	    /* types nomme's */
	    t1 = trouve_type(t->type);
	    if (t1 == NULL) {
		/* type inconnu - estime la taille a celle d'un pointeur */
		return(tt*sizeof(void *));
	    } else {
		/* type connu - taille du type */
		o = STR_ALLOC(DCL_NOM_STR);
		o->type = t1;
		tt *= taille_obj(o);
		free(o);
		return(tt);
	    }
	  default:
	    /* type inconnu */
	    fprintf(stderr, "genom Error: taille_obj: unknown type %s\n", t->name);
	    return(tt* sizeof(void *));
	} /* switch */
    } else {
	/* pointeurs */
	return(tt* sizeof(void *));
    }
} /* taille_obj */

/*----------------------------------------------------------------------*/

/**
 ** Genere le calcul de la taille d'un e'le'ment
 **/
char *
genSizeof(DCL_NOM_STR *n)
{
    char *str = NULL;
    char *type, *var;
    int i, tt;

    /* compte le nombre d'elements tt dans le cas d'un tableau */
    tt = 1;
    for (i = 0; i < n->ndimensions; i++) {
	tt *= n->dimensions[i];
    }
    if (tt > 1) {
	bufcat(&str, "%d * ", tt);
    }
    dcl_nom_decl(n, &type, &var);
    if (!n->pointeur) {
	bufcat(&str, "sizeof(%s)", type);
    } else {
	bufcat(&str, "sizeof(%s *)", type);
    }
    free(type);
    free(var);

    return(str);
} /* genSizeof */

/*----------------------------------------------------------------------*/

/**
 ** retourne une chaine de caracteres representant le nom d'un type
 **/
char *
nom_type(TYPE_STR *type)
{
    char *res = NULL;

    switch (type->type) {
    case INT:
	if (type->flags & UNSIGNED_TYPE) {
	    bufcat(&res, "unsigned ");
	}
	if (type->flags & SHORT_INT) {
	    bufcat(&res, "short ");
	}
	if (type->flags & LONG_INT) {
	    bufcat(&res, "long ");
	}
	if (type->flags & LONG_LONG_INT) {
	    bufcat(&res, "long long ");
	}
	bufcat(&res, "int");
	break;

    case CHAR:
	if (type->flags & UNSIGNED_TYPE) {
	    bufcat(&res, "unsigned ");
	}
	bufcat(&res, "char");
	break;

    case FLOAT:
	bufcat(&res, "float");
	break;

    case DOUBLE:
	bufcat(&res, "double");
	break;
    case STRUCT:
	bufcat(&res, "struct %s", type->name);
	break;
    case UNION:
	bufcat(&res, "union %s", type->name);
	break;
    case ENUM:
	bufcat(&res, "enum %s", type->name);
	break;
    default:
	bufcat(&res, "inconnu");
	break;
    }
    return(res);
} /* nom_type */

/*----------------------------------------------------------------------*/
/**
 ** retourne une chaine de caracteres representant
 ** le nom d'un type en un seul mot
 **/
char *
nom_type1(TYPE_STR *type)
{
    char *res = NULL;

    switch (type->type) {
    case INT:
	if (type->flags & UNSIGNED_TYPE) {
	    bufcat(&res, "unsigned_");
	}
	if (type->flags & SHORT_INT) {
	    bufcat(&res, "short_");
	}
	if (type->flags & LONG_INT) {
	    bufcat(&res, "long_");
	}
	if (type->flags & LONG_LONG_INT) {
	    bufcat(&res, "long_long_");
	}
	bufcat(&res, "int");
	break;

    case CHAR:
	if (type->flags & STRING) {
	  bufcat(&res, "string");
	  break;
	}
	if (type->flags & UNSIGNED_TYPE) {
	    bufcat(&res, "unsigned_");
	}
	bufcat(&res, "char");
	break;

    case FLOAT:
	bufcat(&res, "float");
	break;
    case DOUBLE:
	bufcat(&res, "double");
	break;
    case STRUCT:
	bufcat(&res, "struct_%s", type->name);
	break;
    case UNION:
	bufcat(&res, "union_%s", type->name);
	break;
    case ENUM:
	bufcat(&res, "enum_%s", type->name);
	break;
    default:
	bufcat(&res, "inconnu");
	break;
    }
    return(res);
} /* nom_type */

/*----------------------------------------------------------------------*/

/**
 ** copie d'un type
 **/
TYPE_STR *
copy_type(TYPE_STR *t)
{
    DCL_NOM_LIST *l1, *l2, **pl;

    TYPE_STR *r = STR_ALLOC(TYPE_STR);
    r->name = t->name;
    r->type = t->type;
    r->used = t->used;
    r->linenum = t->linenum;
    r->filename = strdup(t->filename);

    pl = &(r->members);
    for (l1 = t->members; l1 != NULL; l1 = l1->next) {
	l2 = STR_ALLOC(DCL_NOM_LIST);
	l2->dcl_nom = l1->dcl_nom;
	*pl = l2;
	pl = &(l2->next);
    } /* for */
    return(r);
} /* copy_type */

/*----------------------------------------------------------------------*/

/**
 ** ajout d'un type dans la liste ``types''
 **/
void
ajout_type(TYPE_STR *t)
{
    TYPE_LIST *l = STR_ALLOC(TYPE_LIST);

    l->type = t;
    l->next = types;
    types = l;
} /* ajout_type */

/*----------------------------------------------------------------------*/

/**
 ** Recherche de la de'finition complete
 ** d'une structure ou union incomplete
 **/
int
trouve_members(TYPE_STR *t)
{
    TYPE_LIST *l;

    for (l = types; l != NULL; l = l->next) {
	if (l->type->type == t->type && !strcmp(l->type->name, t->name)) {
	    /* trouve' */
	    t->members = l->type->members;
	    return(1);
	}
    } /* for */
    fprintf(stderr, "genom %s:%d: error: %s %s not declared\n",
	    t->filename, t->linenum, nom_type(t), t->name);
    return(0);
} /* trouve_members */

/*----------------------------------------------------------------------*/

/**
 ** Recherche d'un element de la structure de donne'es interne
 **/
DCL_NOM_STR *
trouve_sdi_ref(STR_REF_STR *ref)
{
    TYPE_STR *t = module->internal_data;
    DCL_NOM_LIST *lm;
    ID_LIST *l;
    int good;

    if (ref->sdi_ref == NULL) {
	/* all */
	ref->dcl_nom->type = module->internal_data;
	return(ref->dcl_nom);
    }
    for (l = ref->sdi_ref; l != NULL; l = l->next) {
	good = 0;
	for (lm = t->members; lm != NULL; lm = lm->next) {
	    if (strcmp(lm->dcl_nom->name, l->name) == 0) {
		good = 1;
		break;
	    }
	} /* for */
	if (good) {
	    t = lm->dcl_nom->type;
	} else {
	    break;
	}
    } /* for */
    if (good) {
	return(lm->dcl_nom);
    } else {
	return(NULL);
    }
} /* trouve_sdi_ref */

/*----------------------------------------------------------------------*/

/**
 **  Construction de la liste des fichiers
 ** contenant des definitions de type
 **/
void
construitIncludeList(void)
{
    TYPE_LIST *l;
    TYPE_STR *t;
    ID_LIST *il;

    for (l = types; l != NULL; l = l->next) {
	t = l->type;
	/* Ne traite que les types fournis par l'utilisateur */
	if (t->flags & (TYPE_INTERNAL_DATA | TYPE_POSTER)) {
	    continue;
	}
	/* marque les types definis dans le source genom */
	if (t->filename == NULL) {
	  fprintf(stderr, "genom %s:%d: warning filename unknown for "
		  "type %s\n", nomfic, t->linenum, t->name);
	}
	else if (strcmp(t->filename, nomfic) == 0) {
	    fprintf(stderr, "genom %s:%d: warning: unknown type %s\n",
		    nomfic, t->linenum, t->name);
	    t->flags |= TYPE_GENOM;
	    continue;
	}
	/* Le fichier est-it deja dans la liste ? */
	for (il = includeFiles; il != NULL; il = il->next) {
	    if (il->name != NULL && !strcmp(il->name, t->filename)) {
		break;
	    }
	} /* for */
	if (il == NULL) {
	    /* pas trouve' */
	    il = STR_ALLOC(ID_LIST);
	    if (t->filename != NULL)
	      il->name = strdup(t->filename);
	    else
	      il->name = NULL;
	    il->next = includeFiles;
	    includeFiles = il;
	}
    } /* for */

} /* construitIncludeList */


/* --- langFileExt --------------------------------------------------- */

/** Return file extension for programming language */

const char *
langFileExt(MODULE_LANG lang)
{
  switch(lang) {
    case MODULE_LANG_DEFAULT:
    case MODULE_LANG_C:
      return "c";

    case MODULE_LANG_CXX:
      return "cc";
  }

  /* sane fallback - we shouldn't ever end up here */
  return "c";
}


/*----------------------------------------------------------------------*/

/**
 ** Construction incrementale de chaine
 **/
void
bufcat(char **buf, char *fmt, ...)
{
#ifdef HAVE_VASPRINTF
    va_list ap;
    char *r = NULL;
    int s;

    va_start(ap, fmt);
    s = vasprintf(&r, fmt, ap);
    va_end(ap);
    if (s == -1) {
	fprintf(stderr, "vasprintf failed\n");
	_exit(2);
    }
    if (*buf != NULL) {
	s = asprintf(buf, "%s%s", *buf, r);
	free(r);
    } else {
	*buf = r;
    }
#else
    va_list ap;
    static char buf1[1024];
    int taille;

    if (*buf == NULL) {
	*buf = xalloc(sizeof(char));
	taille = sizeof(char);
    } else {
	taille = strlen(*buf) + 1;
    }

    va_start(ap, fmt);
    vsprintf(buf1, fmt, ap);
    taille += strlen(buf1);

    *buf = xrealloc(*buf, taille);
    strcat(*buf, buf1);
#endif
} /* bufcat */


/**
 ** Appel buffcat si la chaine a ajouter n'est pas deja dans buf
 ** Sinon retourne 0
 **/

int
bufcatIfNotIn(char **buf, char *fmt, ...)
{
#ifdef HAVE_VASPRINTF
  va_list ap;
  static char *buf1 = NULL;

  va_start(ap, fmt);
  vasprintf(&buf1, fmt, ap);

  if (*buf == NULL || strstr(*buf, buf1) == NULL) {
    bufcat(buf, buf1);
    free(buf1);
    return 1;
  }
  return 0;
#else
  va_list ap;
  static char buf1[1024];

  va_start(ap, fmt);
  vsprintf(buf1, fmt, ap);

  if (*buf == NULL || strstr(*buf, buf1) == NULL) {
    bufcat(buf, buf1);
    return 1;
  }
  return 0;
#endif
}

/*----------------------------------------------------------------------*/

/**
 ** Ge'ne'ration des commandes pour construire les substitutions
 **/

void
print_sed_subst(FILE *f, const char *from, const char *fmt, ...)
{
    va_list ap;
    static char sedquotes[] = "^!@#$%&*()-+[]{}|";
    static char f1[] = "    s";
    static char f2[] = "\\$";
    static char f3[] = "g;\n";
    int size;
    char *c, *format;

    /* examine from and fmt strings in order to find an appropriate quote
     * for sed substitutions */
    for(c = sedquotes; *c; c++) {
       if (from && strchr(from, *c)) continue;
       if (fmt && strchr(fmt, *c)) continue;

       break;
    }
    if (!*c) {
       fprintf(stderr,
	       "genom: strings `%s' or `%s' contain "
	       "too many weird characters\n", from, fmt);
       exit(4);
    }

    va_start(ap, fmt);

    size =
       strlen(f1) + 1/*sed quote*/ + strlen(f2) +
       (from != NULL ? strlen(from) : 0) + strlen(f2) + 1/*sed quote*/ +
       ( fmt != NULL ? strlen(fmt) : 0) + 1/*sed quote*/ +
       strlen(f3) + 1/*terminating nul*/;

    format = xalloc(size);
    strcpy(format, f1);
    strncat(format, c, 1);
    strcat(format, f2);
    if (from != NULL)
	strcat(format, from);
    strcat(format, f2);
    strncat(format, c, 1);
    if (fmt != NULL)
	strcat(format, fmt);
    strncat(format, c, 1);
    strcat(format, f3);

    vfprintf(f, format, ap);

    free(format);

    va_end(ap);
} /* print_sed_subst */

/*----------------------------------------------------------------------*/

/**
 ** Marque un type et ses membres comme utilise's
 **/
void
markUsed(TYPE_STR *t)
{
    DCL_NOM_LIST *lm;

    if (t == NULL) {
	return;
    }
    t->used++;

    for (lm = t->members; lm != NULL; lm = lm-> next) {
	if (lm->dcl_nom->type != NULL) {
	    lm->dcl_nom->type->used++;
	}
    } /* for */

} /* markUsed */

/*----------------------------------------------------------------------*/

void *
xalloc(size_t n)
{
    void *ptr;

    ptr = malloc(n);
    if (ptr == NULL) {
	fprintf(stderr, "genom: error: out of memory\n");
	exit(4);
    }
    memset(ptr, 0, n);
    return(ptr);
}

/*----------------------------------------------------------------------*/

void *
xrealloc(void *p, size_t n)
{
    void *ptr;

    ptr = realloc(p, n);
    if (ptr == NULL) {
	fprintf(stderr, "genom: error: out of memory\n");
	exit(4);
    }
    return(ptr);
}

/*----------------------------------------------------------------------*/

/**
 ** genere l'ouverture d'un nouveau fichier
 **/
void
script_open(FILE *out)
{
    fprintf(out, "open (OUT,\">/tmp/gen$$\") "
	    "|| die \"Can't open output\";\n");
}

/*----------------------------------------------------------------------*/

/**
 ** Genere la fermeture d'un fichier et le 'move-if-change' final
 **/
void
script_close(FILE *out, const char *newName,...)
{
    va_list ap;
    char buf[1024];

    va_start(ap, newName);

    vsprintf(buf, newName, ap);
    fprintf(out, "close OUT;\n");
    fprintf(out, "move_if_change(\"/tmp/gen$$\", \"%s\");\n", buf);
    fprintf(out, "\n");
    va_end(ap);
}

void
script_close_exec(FILE *out, const char *newName,...)
{
    va_list ap;
    char buf[1024];

    va_start(ap, newName);

    vsprintf(buf, newName, ap);
    fprintf(out, "close OUT;\n");
    fprintf(out, "move_if_change(\"/tmp/gen$$\", \"%s\");\n", buf);
    fprintf(out, "chmod 00775, \"%s\";\n", buf);
    fprintf(out, "\n");
    va_end(ap);
}

/*----------------------------------------------------------------------*/

/**
 ** Genere l'execution d'un script
 **
 ** (permet de recuperer un "die" )
 **/
void
script_do(FILE *out, const char *dir, const char *doFileName)
{
  fprintf(out, "#-----------------------------------"
	  "-----------------------------------\n\n"
	  "do '%s/%s';\n"
	  "if ($@) {\n"
	  "   die \"Error: $@ \\n\";\n"
	  "}\n\n", dir, doFileName);
  fprintf(out, "#-----------------------------------"
	  "-----------------------------------\n\n");
}

/*----------------------------------------------------------------------*/

/**
 ** simple copy of a file
 **/
void
copy_script(FILE *out, const char *protoName)
{
	fprintf(out, "\ncopy(\"%s/%s\", \"%s\") || "
	    "die \"Copy failed: $!\";\n",
	    protoDir, protoName, protoName);
	fprintf(out, "\nchmod(0755, \"%s\") || die \"chmod failed: $!\";\n\n",
	    protoName);
}

/*----------------------------------------------------------------------*/

/**
 ** Genere le debut d'une substitution
 **/
void
subst_begin(FILE *out, const char *protoName)
{

    fprintf(out, "open (IN,\"%s/%s\") \n\t|| die \"Can't open skeleton %s/%s\";\n",
	    protoDir, protoName, protoDir, protoName);
    fprintf(out,
	    "$commentMode = 0;\n"
	    "while (<IN>) {\n"
	    "   chop;\n"
	    "   s/\\$commentbegin\\$.*$// and $commentMode = 1;\n"
	    "   s/^.*\\$commentend\\$// and $commentMode = 0;\n"
       );
}

/*----------------------------------------------------------------------*/

/**
 ** Genere la fin d'une substitution
 **/
void
subst_end(FILE *out)
{
    fprintf(out, "    print OUT unless $commentMode;\n");
    fprintf(out, "}\n");
    fprintf(out, "close IN;\n");
}


/*----------------------------------------------------------------------*/

/**
 ** Genere le debut d'une insertion litterale
 **/

void
cat_begin(FILE *out)
{
    fprintf(out, "print OUT <<'__END_OF_FILE__';\n");
}

/*----------------------------------------------------------------------*/

/**
 ** Genere la fin d'une intertion litterale
 **/
void
cat_end(FILE *out)
{
    fprintf(out, "__END_OF_FILE__\n");
}
