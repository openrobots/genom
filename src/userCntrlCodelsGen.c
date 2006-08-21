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
#include "parser.tab.h"
#include "userCntrlCodelsGen.h"

static char * inList(ID_STR *codel, ID_STR *rqst);

/***
 *** Ge'ne'ration des bibliotheques taches d'execution
 ***/

int 
userCntrlCodelsGen(FILE *out)
{

  ID_LIST *lf;
  RQST_LIST *lr;
  RQST_STR *r;
  char *params=NULL, *type, *var;
  char *prev_rqst_name;

  script_open(out);
  
  cat_begin(out);
  
  /* 
   * Entete du fichier
   */
  fprintf(out, 
	  "/**\n"
	  " ** %sCntrlTaskCodels.c\n"
	  " **\n"
	  " ** Codels used by the control task %sCntrlTask\n"
	  " **\n"
	  " ** Author: \n"
	  " ** Date: \n"
	  " **\n"
	  " **/\n\n", module->name, module->name);
  
  fprintf(out, "#ifdef VXWORKS\n");
  fprintf(out, "# include <vxWorks.h>\n");
  fprintf(out, "#else\n");
  fprintf(out, "# include <portLib.h>\n");
  fprintf(out, "#endif\n");
  fprintf(out, "#include \"server/%sHeader.h\"\n\n\n", module->name);

  /* 
   * Codels de controle
   */
  for (lr = requetes; lr != NULL; lr = lr->next) {

    r = lr->rqst;

    /* Pas de controle */
    if (r->codel_control == NULL) continue;

    /* Codel deja traite */
    if ((prev_rqst_name = inList(r->codel_control,r->name))) {
      fprintf(out, 
	      "/*------------------------------------------------------------------------\n"
	      " * %s  -  control codel of %s request %s\n *\n"
	      " * Description:    see above (request %s)\n *\n"
	      " * Report: OK\n", 
	      r->codel_control, 
	      r->type == EXEC ? "EXEC" : r->type == INIT ? "INIT" : "CONTROL",
	      r->name, prev_rqst_name);
      for (lf=r->fail_reports; lf != NULL; lf=lf->next) {
	fprintf(out, " *                 S_%s_%s\n",  
		module->name, 
		lf->name);
      }
      fprintf(out, " *\n * Returns:    OK or ERROR\n"
	      " */\n\n");
      
      /* On passe a la requete suivante */
      continue;
    }
    
    /* Entete de la fonction */
    fprintf(out, 
	    "/*------------------------------------------------------------------------\n"
	    " * %s  -  control codel of %s request %s\n *\n"
	    " * Description:    \n"
	    " * Report: OK\n",
	    r->codel_control, 
	    r->type == EXEC ? "EXEC" : r->type == INIT ? "INIT" : "CONTROL",
	    r->name);
    for (lf=r->fail_reports; lf != NULL; lf=lf->next) {
      fprintf(out, " *                 S_%s_%s\n",  
	      module->name, 
	      lf->name);
    }
    fprintf(out, " *\n * Returns:    OK or ERROR\n"
	    " */\n\n");
    
    /* Parametres */
    if(params != NULL) {
      free(params);
      params=NULL;
    }
    if (r->input != NULL) {
      dcl_nom_decl(r->input->dcl_nom, &type, &var);
      bufcat(&params, "%s %s%s, ", type, 
	     r->input->dcl_nom->ndimensions == 0 ? "*" : "", var);
      free(type);
      free(var);
    }
    bufcat(&params, "int *report");
    
    /* Codel */
    fprintf(out, 
	    "STATUS\n%s(%s)\n"
	    "{\n"
	    "  /* ... add your code here ... */\n"
	    "  return OK;\n}\n\n", r->codel_control, params); 
  
  } /* for (requetes) */
  
  cat_end(out);
  
  script_close(out, "codels/%sCntrlTaskCodels.c", module->name);
    
  return(0);

} /* userCntrlCodelsGen */

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
