/*	$LAAS$ */

/* 
 * Copyright (c) 1993-2003 LAAS/CNRS
 * Matthieu Herrb - Tue Aug 10 1993
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
#include "essayGen.h"

static void
essayMakeInput(RQST_STR *r, char **decla, char **scanOrPrint);
static void
essayMakeOutput(RQST_STR *r, char **decla, char **scanOrPrint);
static void
essayMakeGetInput(RQST_STR *rqst, char *var, char **scanOrPrint);


/***
 *** Ge'ne'ration des commandes pour construire la tache d'essai
 ***/
static char *
essayTabRequestFunc(void) 
{
    RQST_LIST *l;
    RQST_STR *r;
    char *msg = NULL;
    char *getRqst = NULL;
    char *sizeIn = NULL;
    char *sizeOut = NULL;
    char *size;

    for (l = requetes; l != NULL; l = l->next) {
        r = l->rqst;
	bufcat(&msg,
	       "static BOOL "
	       "%sEssay%s(ESSAY_STR *id, int rq, int ac, BOOL ch);\n", 
		module->name, r->name);
    }

    bufcat(&msg, "\nstatic  ESSAY_RQST_DESC_STR %sEssayRqstFuncTab[] = {\n",
	   module->name); 

    for (l = requetes; l != NULL; l = l->next) {
        r = l->rqst;

	/* Calcul des tailles des parametres */
	if (r->input != NULL) {
	  size = genSizeof(r->input->dcl_nom);
	  bufcat(&sizeIn, size, r->input->dcl_nom->name);
	  free(size);

	  /* Regarde s'il y a une fonction de lecture des parametres */
	  bufcat(&getRqst, "-1");

	}
	else {
	  bufcat(&sizeIn, "0");
	  bufcat(&getRqst, "-1");
	}

	if (r->output != NULL) {
	  size = genSizeof(r->output->dcl_nom);
	  bufcat(&sizeOut,  size, r->output->dcl_nom->name);
	  free(size);
	}
	else {
	  bufcat(&sizeOut, "0");
	}

	/* Description de la requete */
	bufcat(&msg, "  {%s_%s_RQST, %sEssay%s, %s, %s}",
	       module->NAME, r->NAME, module->name, r->name,
	       sizeIn, sizeOut);
#if 0
	if (r->input_info != NULL) {
	    RQST_INPUT_INFO_LIST *ril;
	    printf ("** %s \n", r->name );
	    if (r->doc != NULL)
	      printf ("doc: %s\n", r->doc);
	    for (ril = r->input_info; ril != NULL; ril=ril->next) {
	      if (ril->type == DOUBLE)
		printf ("%f %s\n", ril->default_val.d_val , ril->doc);
	      else	      if (ril->type == INT)
		printf ("%d %s\n", ril->default_val.i_val , ril->doc);
	      else
		printf ("%s %s\n", ril->default_val.str_val , ril->doc);
	    }
	}
#endif
	if (l->next != NULL) {
	    bufcat(&msg, ",\n");
	}
	free(sizeIn);
	sizeIn = NULL;
	free(sizeOut);
	sizeOut = NULL;
	free(getRqst);
	getRqst = NULL;
    } /* for */	

    bufcat(&msg, "\n};");
 
    return(msg);
}

static char *
essayTabRequestName(void) 
{
    RQST_LIST *l;
    RQST_STR *r;
    char *msg = NULL;

    bufcat(&msg, "\nstatic char *%sEssayRequestNameTab[] = {\n",
	   module->name); 

    for (l = requetes; l != NULL; l = l->next) {
        r = l->rqst;
	bufcat(&msg, "\"%s%s\"", 
	       r->name,
	       r->type == EXEC && reentrant(r) ? " (nE)" :
	       r->type == EXEC ? " (E)" :
	       r->type == INIT ? " (I)" :
	       "");
	    
	if (l->next != NULL) {
	    bufcat(&msg, ",\n");
	}
    } /* for */
    bufcat(&msg, "\n};");
    return(msg);
}

static char *
essayTabPosterFunc(int *nbPosterData) 
{
    POSTER_LIST *p;
    char *msg = NULL;
    STR_REF_LIST *d;

    *nbPosterData = 0;
    bufcat(&msg, "\nstatic STATUS (*%sEssayPosterShowFuncTab[])() = {\n",
	   module->name); 

    for (p = posters; p != NULL; p = p->next) {
	bufcat(&msg, "%s%sPosterShow,\n", module->name, p->name);
	(*nbPosterData)++;

	/* Chaque membre s'il y en a plusieurs */
	if (p->data != NULL && p->data->next != NULL)
	  for (d = p->data; d != NULL; d = d->next) {
	    (*nbPosterData)++;
	    bufcat(&msg, "%s%s%sPosterShow,\n", 
		   module->name, p->name, d->str_ref->dcl_nom->name);
	  }
    } /* for */
    bufcat(&msg, 
	   "%sCntrlPosterShow,\n"
	   "%sCntrlPosterActivityShow\n};", module->name, module->name);
    return(msg);
}

static char *
essayTabPosterName(void) 
{
    POSTER_LIST *p;
    char *msg = NULL;
    STR_REF_LIST *d;

    bufcat(&msg, "\nstatic char *%sEssayPosterNameTab[] = {\n",
	   module->name); 

    if (posters == NULL)
      bufcat(&msg, "NULL");
    
    else {
      for (p = posters; p != NULL; p = p->next) {
	bufcat(&msg, "\"%s\"", p->name);
	
	/* Chaque membre s'il y en a plusieurs */
	if (p->data != NULL && p->data->next != NULL)
	  for (d = p->data; d != NULL; d = d->next) {
	    bufcat(&msg, ",\n");
	    bufcat(&msg, "\" | %s->%s\"", 
		   p->name, d->str_ref->dcl_nom->name);
	  }
	if (p->next != NULL) {
	  bufcat(&msg, ",\n");
	}
      } /* for */
    }

    bufcat(&msg, "\n};");
    return(msg);
}

int 
essayGen(FILE *out)
{
    RQST_LIST *l;
    RQST_STR *r;
    char *p;
    int nbPosters;
    char *decla=NULL, *scanOrPrint=NULL;
    
    /* La tache d'essay */
    script_open(out);
    subst_begin(out, PROTO_ESSAY);
    
    /* nom du module */
    print_sed_subst(out, "module", module->name);
    print_sed_subst(out, "MODULE", module->NAME);
    /* Nombre de requetes */
    print_sed_subst(out, "nbRequest", "%d", nbRequest);
    /* tableau des fonctions d'essai */
    p = essayTabRequestFunc();
    print_sed_subst(out, "requestFuncTabDeclare", p);
    free(p);
    /* tableau des noms des fonctions */
    p = essayTabRequestName();
    print_sed_subst(out, "requestNameTabDeclare", p);
    free(p);
    /* tableaux des fonctions des posters */
    p = essayTabPosterFunc(&nbPosters);
    print_sed_subst(out, "posterShowFuncTabDeclare", p);
    free(p);
    print_sed_subst(out, "nbPosterData", "%d", nbPosters);
    /* tableaux des noms des donnees des posters */
    p = essayTabPosterName();
    print_sed_subst(out, "posterNameTabDeclare", p);
    free(p);

    subst_end(out);

    /* On traite chaque requete */
    for (l = requetes; l != NULL; l=l->next) {

	r = l->rqst;

	/* EXEC or INIT requests */
	if (r->type == EXEC || r->type == INIT)
	  subst_begin(out, PROTO_ESSAY_EXEC_RQST);

	/* CONTROL requests */
	else 
	  subst_begin(out, PROTO_ESSAY_CNTRL_RQST);

	print_sed_subst(out, "module", module->name);
	print_sed_subst(out, "MODULE", module->NAME);
	print_sed_subst(out, "request", r->name);


	/* Saisie des paramètres */
	if (r->input != NULL) {
	    essayMakeInput(r, &decla, &scanOrPrint);
	    print_sed_subst(out, "inputDeclarations", "%s", decla);
	    print_sed_subst(out, "inputScan", "%s", scanOrPrint);
	    free(decla); decla = NULL;
	    free(scanOrPrint); scanOrPrint = NULL;
	} else {
	    print_sed_subst(out, "inputDeclarations", "");
	    print_sed_subst(out, "inputScan", "");
	}

	/* Affichage donnée résultat */
	if (r->output != NULL) {
	    essayMakeOutput(r, &decla, &scanOrPrint);
	    print_sed_subst(out, "outputDeclarations", "%s", decla);
	    print_sed_subst(out, "outputPrint", "%s", scanOrPrint);
	    free(decla); decla = NULL;
	    free(scanOrPrint); scanOrPrint = NULL;
	} else {
	    print_sed_subst(out, "outputDeclarations", "");
	    print_sed_subst(out, "outputPrint", "");
	}
	subst_end(out);
    } /* for */

    
    script_close(out, "server/%sEssay.c", module->name);
    
    return(0);
}

static void
essayMakeOutput(RQST_STR *r, char **decla, char **scanOrPrint)
{
  int i, newline, nDim, *dims;
  char *type=NULL;
  char *var=NULL;
  char *arg=NULL;
  DCL_NOM_STR *dcl;
	    
  dcl = r->output->dcl_nom;	    

  newline = (dcl->type->type == STRUCT || dcl->type->type == UNION ||
	     dcl->type->type == TYPEDEF || dcl->flags & ARRAY);
  nDim = dcl->ndimensions;
  dims = dcl->dimensions;
  dcl_nom_decl(dcl, &type, &var);
  if (nDim==0) {
    bufcat(decla, "int *outputDims = NULL;");
    bufcat(&arg, "(%s *)ESSAY_RQST_OUTPUT(essayId,rqstNum)", type);
  }
  else {
    bufcat(decla, "  int outputDims[%d] = {%d", nDim, dims[0]);
    for (i=1;i<nDim;i++) 
      bufcat(decla, ",%d", dims[i]);
    bufcat(decla, "};");
    bufcat(&arg, "(%s *)ESSAY_RQST_OUTPUT(essayId,rqstNum)", 
	   nom_type(dcl->type));
  }

  bufcat(scanOrPrint, 
	 "if (!silent) {\n"
	 "    printf (\"%s%s\");\n"
	 "    print_%s(stdout, %s, %d, %d, outputDims, stdin);\n"
	 "  }\n",
	 dcl->name,  newline ? ":\\\\n" : " = ",
	 dcl->flags & STRING?"string":nom_type1(dcl->type),
	 arg, newline?1:0, nDim);
  free(type);
  free(var); 
  free(arg); 
}

static void
essayMakeInput(RQST_STR *r, char **decla, char **scanOrPrint)
{
  int i, newline, nDim, *dims;
  char *type=NULL;
  char *var=NULL;
  char *arg=NULL;
  DCL_NOM_STR *dcl;
  
  dcl = r->input->dcl_nom;

  newline = (dcl->type->type == STRUCT || dcl->type->type == UNION ||
	     dcl->type->type == TYPEDEF || dcl->flags & ARRAY);
  nDim = dcl->ndimensions;
  dims = dcl->dimensions;
  dcl_nom_decl(dcl, &type, &var);

  if (nDim==0) {
    bufcat(decla, "int *inputDims = NULL;");
    bufcat(&arg, "(%s *)ESSAY_RQST_INPUT(essayId,rqstNum)", type);
  }

  else {
    bufcat(decla, "  int inputDims[%d] = {%d", nDim, dims[0]);
    for (i=1;i<nDim;i++) 
      bufcat(decla, ",%d", dims[i]);
    bufcat(decla, "};");
    bufcat(&arg, "(%s *)ESSAY_RQST_INPUT(essayId,rqstNum)", nom_type(dcl->type));

    if (dcl->flags & STRING) {
      free(type); type = NULL;
      free(var); var = NULL;
      bufcat(&type, "string%d", dims[nDim-1]);
      bufcat(&var, "%s", dcl->name);
      for (i=0;i<nDim-1;i++) 
	bufcat(&var, "[%d]", dims[i]);
    }
  }

  essayMakeGetInput(r, var, scanOrPrint);

  bufcat(scanOrPrint, "printf (\"** Enter %s %s%s\");\n"
	 "    scan_%s(stdin, stdout, %s, %d, %d, "
	 "inputDims);",
	 type, var, newline ? ":\\\\n" : ": ",
	 dcl->flags&STRING? "string":nom_type1(dcl->type), 
	 arg, newline?1:0, nDim);
  free(type);
  free(arg);
  free(var);
}

static void
essayMakeGetInput(RQST_STR *rqst, char *varName, char **scanOrPrint)
{
  RQST_LIST *l;
  RQST_STR *r;
  ID_LIST *il;
  STR_REF_LIST *lm;
  POSTER_LIST *p;
  DCL_NOM_STR *n;
  char *var, *type;
  int i=0;
  char *bufin=NULL;
  char *bufout=NULL;
  char *funcnames=NULL;
  char *funcget=NULL;

  /* Adresse dans la sdi de input */
  for (il = rqst->input->sdi_ref; il != NULL; il = il->next) {
    bufcat(&bufin, ".%s", il->name);
  }

  /* Recherche d'une requete dont output a meme adresse */
  for (l = requetes; l != NULL; l = l->next) {
    r = l->rqst;
    /* not itself */
    if (!strcmp(r->name, rqst->name))
      continue;
    if (r->output != NULL) {
      for (il = r->output->sdi_ref; il != NULL; il = il->next) {
	bufcat(&bufout, ".%s", il->name);
      }
      if (bufout != NULL) {
	if (!strcmp(bufin,bufout)) {
	  if (funcnames != NULL)
	    bufcat(&funcnames, ", ");
	  bufcat(&funcnames, "\"request %s%s%s\"",
		 r->name,
		 r->type == EXEC && reentrant(r) ? " (nE)" :
		 r->type == EXEC ? " (E)" :
		 r->type == INIT ? " (I)" : 
		 r->type == INIT && reentrant(r) ? " (nI)" : 
		 "",
		 r->input ? " - needs input -" : "");
	  
	  bufcat(&funcget, 
		 "    case %d:\n"
		 "    if (!essaySendAndGetInput(essayId, %s_%s_RQST, acti, ESSAY_RQST_INPUT(essayId,rqstNum)))\n"
		 "       return FALSE;\n"
		 "    break;\n",
		 i++, module->NAME, r->NAME);
	}
	free(bufout);
	bufout = NULL;
      }
    }
  }

  /* Recherche d'un poster dont output a meme adresse */
  for (p = posters; p != NULL; p = p->next) {
    if (p->data != NULL) {
      for (lm = p->data; lm != NULL; lm = lm->next) {
	for (il = lm->str_ref->sdi_ref; il != NULL; il = il->next) {
	  bufcat(&bufout, ".%s", il->name);
	}
	if (bufout != NULL && !strcmp(bufin,bufout)) {
	  n = lm->str_ref->dcl_nom;
	  if (funcnames != NULL)
	    bufcat(&funcnames, ", ");
	  bufcat(&funcnames, "\"poster %s%s\"", p->name, n->name);
	  n->pointeur++;
	  dcl_nom_decl(n, &type, &var);
	  n->pointeur--;
	  bufcat(&funcget, 
		 "    case %d:\n"
		 "    %s%s%sPosterRead((%s *)ESSAY_RQST_INPUT(essayId,rqstNum));\n"
		 "    break;\n",
		 i++, module->name, p->name, lm->str_ref->dcl_nom->name,
		 type);
	  free(var);
	  free(type);
	}
	free(bufout);
	bufout = NULL;
      }
    }
  }



  if (funcnames != NULL) {
    bufcat(scanOrPrint, 
	   "  const char *menu[] = {\"%s\", %s};\n"
	   "  if(essayPrintGetInputMenu(essayId, %d, menu, rqstNum)) {\n"
	   "  switch(ESSAY_RQST_GET_CMD(essayId,rqstNum)) {\n"
	   "%s"
	   "    default:\n"
	   "    break;\n   }\n  }\n"
	   "  printf(\"\\\\n\");",
	   varName, funcnames, i, funcget);
  }

  free(bufin);
  free(funcnames);
  free(funcget);
}
