/*	$LAAS$ */

/* 
 * Copyright (c) 1996-2003 LAAS/CNRS
 * Sara Fleury -  Sept 5 1996
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
#include "userExecCodelsGen.h"


static char * inList(ID_STR *codel, ID_STR *rqst);

/***
 *** Ge'ne'ration des bibliotheques taches d'execution
 ***/

int 
userExecCodelsGen(FILE *out)
{

  EXEC_TASK_LIST *lt;
  EXEC_TASK_STR *t;
  ID_LIST *lf;
  RQST_LIST *lr;
  RQST_STR *r;
  POSTER_LIST *p;
  char *params=NULL, *type, *var;
  static char formatCodel[] = "/* %s  -  codel %s of %s\n"
			      "   Returns:  %s */\n"
			      "ACTIVITY_EVENT\n%s(%s)\n{\n"
			      "  /* ... add your code here ... */\n"
			      "  return ETHER;\n}\n\n";
  static char formatAlreadyCodel[] = "/* %s  -  codel %s of %s\n"
				     "   see above (request %s) */\n";
  char *prev_rqst_name;

  /* Pour chaque tache d'execution */
  for (lt = taches; lt != NULL; lt = lt->next) {
    t = lt->exec_task;
    
    script_open(out);

    cat_begin(out);

    /* 
     * Entete du fichier
     */
    fprintf(out, 
	    "/**\n"
	    " ** %s%sCodels.c\n"
	    " **\n"
	    " ** Codels called by execution task %s%s\n"
	    " **\n"
	    " ** Author: \n"
	    " ** Date: \n"
	    " **\n"
	    " **/\n\n", module->name, t->name, module->name, t->name);

    fprintf(out, "#ifdef VXWORKS\n");
    fprintf(out, "# include <vxWorks.h>\n");
    fprintf(out, "#else\n");
    fprintf(out, "# include <portLib.h>\n");
    fprintf(out, "#endif\n");
    fprintf(out, "#include \"server/%sHeader.h\"\n\n\n", module->name);
    
    /* 
     * Codel d'initialisation 
     */
    if (t->codel_task_start != NULL) {
      fprintf(out, 
	      "/*------------------------------------------------------------------------\n *\n"
	      " * %s  --  Initialization codel (fIDS, ...)\n *\n"
	      " * Description: \n * \n"
	      " * Returns:    OK or ERROR\n */\n\n", t->codel_task_start);
      fprintf(out, 
	      "STATUS\n%s(int *report)\n"
	      "{\n"
	      "  /* ... add your code here ... */\n"
	      "  return OK;\n"
	      "}\n\n", t->codel_task_start);
    }

    /* 
     * Codel de terminaison
     */
    if (t->codel_task_end != NULL) {
      fprintf(out, 
	      "/*------------------------------------------------------------------------\n *\n"
	      " * %s  --  Termination codel\n *\n"
	      " * Description: \n * \n"
	      " * Returns:    OK or ERROR\n */\n\n", t->codel_task_end);
      fprintf(out, 
	      "STATUS\n%s(void)\n"
	      "{\n"
	      "  return OK;\n"
	      "}\n\n", t->codel_task_end);
    }

    /* 
     * Codel activite permanente 
     */
    if (t->codel_task_main != NULL) {
      fprintf(out, 
	      "/*------------------------------------------------------------------------\n *\n"
	      " * %s  --  codel of permanent activity\n *\n"
	      " * Description: \n * \n"
	      " * Reports:      OK\n", t->codel_task_main);
      for (lf=t->fail_reports; lf != NULL; lf=lf->next) {
	fprintf(out, " *              S_%s_%s\n",  
		module->name, lf->name);
      }
	      
      fprintf(out,
	      " * \n * Returns:    OK or ERROR\n */\n\n");
      fprintf(out, 
	      "STATUS\n%s(int *report)\n"
	      "{\n"
	      "  /* ... add your code here ... */\n"
	      "  return OK;\n"
	      "}\n", t->codel_task_main);
    }
      
    /* 
     * Codels des requetes
     */
    for (lr = requetes; lr != NULL; lr = lr->next) {
      r = lr->rqst;

      /* Ne concerne pas cette tache d'execution */
      if (r->exec_task != t) continue;

      /* On le traite */
      fprintf(out, 
	      "/*------------------------------------------------------------------------\n"
	      " * %s\n *\n"
	      " * Description: \n *\n"
	      " * Reports:      OK\n", r->name);
      for (lf=r->fail_reports; lf != NULL; lf=lf->next) {
	fprintf(out, " *              S_%s_%s\n",  module->name, lf->name);
      }
      fprintf(out, " */\n\n");
      
      /* Parametres */
      if (params != NULL) {
	free(params);
	params = NULL;
      }
      if (r->input != NULL) {
	dcl_nom_decl(r->input->dcl_nom, &type, &var);
	bufcat(&params, "%s %s%s, ", type, 
	       r->input->dcl_nom->ndimensions == 0 ? "*" : "", var);
	free(type);
	free(var);
      }
      if (r->output != NULL) {
	dcl_nom_decl(r->output->dcl_nom, &type, &var);
	bufcat(&params, "%s %s%s, ", type, 
	       r->output->dcl_nom->ndimensions == 0 ? "*" : "", var);
	free(type);
	free(var);
      }
      bufcat(&params, "int *report");
      
      /* Codels */

      /* START */
      if (r->codel_start != NULL) {
	if ((prev_rqst_name = inList(r->codel_start,r->name))) {
	  fprintf(out, formatAlreadyCodel, 
		  r->codel_start, "START", r->name, 
		  prev_rqst_name); 
	}
	else {
	  fprintf(out, formatCodel, 
		  r->codel_start, "START", 
		  r->name, "START EXEC END ETHER FAIL ZOMBIE",
		  r->codel_start, params);
	}
      }

      /* EXEC */
      if (r->codel_main != NULL) {
	if ((prev_rqst_name = inList(r->codel_main,r->name))) {
	    fprintf(out, formatAlreadyCodel, 
		    r->codel_main, "EXEC", r->name, 
		    prev_rqst_name); 
	}
	else {
	  fprintf(out, formatCodel, 
		  r->codel_main, "EXEC", 
		  r->name, "EXEC END ETHER FAIL ZOMBIE",
		  r->codel_main, params);
	}
      }

      /* END */
      if (r->codel_end != NULL) {
	if ((prev_rqst_name = inList(r->codel_end,r->name))) {
	  fprintf(out, formatAlreadyCodel, 
		  r->codel_end, "END", r->name, 
		  prev_rqst_name); 
	}
	else {
	  fprintf(out, formatCodel, 
		  r->codel_end, "END", 
		  r->name, "END ETHER FAIL ZOMBIE",
		  r->codel_end, params);
	}
      }

      /* FAIL */
      if (r->codel_fail != NULL) {
	if ((prev_rqst_name = inList(r->codel_fail,r->name))) {
	  fprintf(out, formatAlreadyCodel, 
		  r->codel_fail, "FAIL", r->name, 
		  prev_rqst_name); 
	}
	else {
	  fprintf(out, formatCodel, 
		  r->codel_fail, "FAIL", 
		  r->name, "FAIL ZOMBIE",
		  r->codel_fail, params);
	}
      }

      /* INTER */
      if (r->codel_inter != NULL) {
	if ((prev_rqst_name = inList(r->codel_inter,r->name))) {
	  fprintf(out, formatAlreadyCodel, 
		  r->codel_inter, "INTER", r->name, 
		  prev_rqst_name); 
	}
	else {
	  fprintf(out, formatCodel, 
		  r->codel_inter, "INTER", 
		  r->name, "INTER ETHER FAIL ZOMBIE",
		  r->codel_inter, params);
	}
      }

    } /* for (requetes) */
  

    /*
     * fonctions de creation des posters 
     */
    for (p = posters; p != NULL; p = p->next) {
	if (p->exec_task != t) {
	    continue;
	}
	if (p->codel_poster_create != NULL) {
	    fprintf(out, "/*----------------------------------------------------------------------\n *\n");
	    fprintf(out, " * %s\n", p->codel_poster_create);
	    fprintf(out, " */\n");
	    fprintf(out, "STATUS\n%s(char *name, int size, "
		    "POSTER_ID *pPosterId)\n", p->codel_poster_create);
	    fprintf(out, "{\n  /* ... add your code here ... */\n  return OK;\n}\n");
	}
    }
	
    cat_end(out);

    script_close(out, "codels/%s%sCodels.c", module->name, t->name);
    
  } /* for */
  return(0);

} /* userExecCodelsGen */

static char * inList(ID_STR *codel, ID_STR *rqst) 
{
  typedef struct list_codel {
    char *nameCodel;
    char *nameRqst;
    struct list_codel *next;
  }list_codel;

  static list_codel listCodel={NULL, NULL, NULL};
  static list_codel *last = &listCodel;
  list_codel *c;
  
  for (c = listCodel.next; c != NULL; c = c->next) {
    if (strcmp(c->nameCodel,codel)==0) {
      return c->nameRqst;
    }
  }

  c = (list_codel *)xalloc(sizeof(list_codel));
  c->nameCodel = (ID_STR *)strdup(codel);
  c->nameRqst = (ID_STR *)strdup(rqst);
  c->next = NULL;

  last->next = c;
  last = last->next;

  return NULL;
}
