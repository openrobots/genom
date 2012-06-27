
/* 
 * Copyright (c) 1996-2003 LAAS/CNRS
 * Sara Fleury - Sept 1996
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
#include <string.h>
#include <stdlib.h>

#include "genom.h"
#include "parser.tab.h"
#include "initGen.h"
#include "genom/printScan.h" /* pour getIndexesStr */

#define VX_MAX_PARAMS 10
#define VX_MAX_PARAMS_SIZE VX_MAX_PARAMS*4 /* Taille max pour l'ensemble des parametres */

static int flatDcl(DCL_NOM_STR *dcl);
static char *atotype_func(TYPE_STR *type);
static int addParamToStrings (char *prefix, DCL_NOM_STR *dcl_nom);
static int getsize(TYPE_STR *type);
static void addVerifEnum(DCL_NOM_STR *dcl_nom, int numvar);
static void free_strings();
static char *get_format(TYPE_STR *type);

char *strUsage=NULL;   /* TOTO toto, ... (liste des arguments) */
char *strList=NULL;    /* TOTO var1, TITI var2, ... (avec nom unique) */
char *strDeclar=NULL;  /* TOTO var1; ... */
char *strA2Type=NULL;  /* var1 = atoTOTO(argv[0]); ... */
char *strFillStruct=NULL;  /* memcpy(input+offset,var1,size); ... */
char *strName=NULL;    /* var1, var2, ... */
char *strVerif=NULL;
char *sizeStr;
static int numvar = 1;

int
initGen(FILE *out)
{
  RQST_LIST *l_rqst = NULL;
  RQST_STR *rinit = NULL;
  char *var, *type;
  DCL_NOM_STR *dcl;

  /* FUNC */
  script_open(out);
  subst_begin(out, PROTO_INIT);
  
  /* nom du module */
  print_sed_subst(out, "module", module->name);
  print_sed_subst(out, "MODULE", module->NAME);
  
  /* 
   * Recherche de la requete d'init 
   */
  l_rqst = requetes;
  while (l_rqst != NULL && l_rqst->rqst->type != INIT)
    l_rqst=l_rqst->next;


  /* Pas de requete d'init : on sort */
  if (l_rqst == NULL) {

    print_sed_subst(out, "requestFlag", "(0)");
    print_sed_subst(out, "nbInputParams", "0");
    print_sed_subst(out, "inputDeclare", "");
    print_sed_subst(out, "inputNamePtrC", "");
    print_sed_subst(out, "inputSize", "0");
    print_sed_subst(out, "inputShow", "");
    print_sed_subst(out, "inputNamePtr", "NULL");
    print_sed_subst(out, "requestNum", "0");

    print_sed_subst(out, "inputUsage", "");
    print_sed_subst(out, "inputA2Type", "");
    print_sed_subst(out, "inputFlat", "void");
    print_sed_subst(out, "inputFlatDeclare", "");
    print_sed_subst(out, "inputFillStruct", "");
    print_sed_subst(out, "inputFlatNamePtrC", "");
    print_sed_subst(out, "inputVerif", "");

    subst_end(out);
    script_close(out, "server/%sInit.c", module->name);

    return(0);
  }
  
  /* Il y a une requete d'init */
  rinit = l_rqst->rqst;
  print_sed_subst(out, "requestFlag", "(1)");
  print_sed_subst(out, "request", rinit->name);
  

  print_sed_subst(out, "requestNum", "%d", rinit->rqst_num);

  /* 
   * Traitement des parametres (input)
   */

  /* Pas de parame`tre (bizard) : on s'arrete la */
  if (rinit->input == NULL) {

    print_sed_subst(out, "nbInputParams", "0");
    print_sed_subst(out, "inputDeclare", "");
    print_sed_subst(out, "inputNamePtrC", "");
    print_sed_subst(out, "inputSize", "0");
    print_sed_subst(out, "inputShow", "");
    print_sed_subst(out, "inputNamePtr", "NULL");
    print_sed_subst(out, "requestNum", "0");

    print_sed_subst(out, "inputUsage", "");
    print_sed_subst(out, "inputA2Type", "");
    print_sed_subst(out, "inputFlat", "void");
    print_sed_subst(out, "inputFlatDeclare", "");
    print_sed_subst(out, "inputFillStruct", "");
    print_sed_subst(out, "inputFlatNamePtrC", "");
    print_sed_subst(out, "inputVerif", "");

    fprintf(stderr, "genInit warning: init request without parameters\n");
    subst_end(out);
    script_close(out, "server/%sInit.c", module->name);
    
    return(0);
  }

  /* Parametre de la requete d'init */
  dcl = rinit->input->dcl_nom;
  dcl_nom_decl(dcl, &type, &var);
  print_sed_subst(out, "inputDeclare", "%s %s;", type, var);
  sizeStr = genSizeof(dcl);
  print_sed_subst(out, "inputSize", sizeStr, dcl->name);
  free(sizeStr);
  print_sed_subst(out, "inputShow", "  print_%s(stdout, %s);\n", 
		  nom_type1(dcl->type), var);
  free(type);
  free(var);
  print_sed_subst(out, "inputNamePtr", "&%s", rinit->input->dcl_nom->name);
  print_sed_subst(out, "inputNamePtrC", "&%s,", 
		  dcl->name);


  /* Mise a plat de la structure d'input */
  if (flatDcl (dcl) != 0) {
    free_strings();
    subst_end(out);
    script_close(out, "server/%sInit.c", module->name);
    return (1);
  }

  /* Substitution */
  print_sed_subst(out, "nbInputParams", "%d", numvar-1);
  print_sed_subst(out, "inputUsage", "%s", strUsage);
  print_sed_subst(out, "inputA2Type", "%s", strA2Type);
  print_sed_subst(out, "inputFlat", "%s", strList);
  print_sed_subst(out, "inputFlatNamePtrC", "%s", strName);
  print_sed_subst(out, "inputFlatDeclare", "%s", strDeclar);
  print_sed_subst(out, "inputFillStruct", "%s", strFillStruct);
  print_sed_subst(out, "inputVerif", "%s", strVerif);

  /* C'est fini */
  subst_end(out);
  script_close(out, "server/%sInit.c", module->name);
    
  free_strings();
  return(0);

}

static void free_strings()
{
  free(strUsage);
  free(strList);
  free(strName);
  free(strDeclar);
  free(strA2Type);
  free(strFillStruct);
  free(strVerif);
}

/**--------------------------------------------------------------------
 ** 
 **  flatDcl: fonction recursive qui met a plat une structure dcl
 **
 **/
static int flatDcl(DCL_NOM_STR *dcl) 
{
  DCL_NOM_LIST *dcl_members=NULL, *l_dcl;
  int i;
  static char prefix[1024]="";
  char index[8];
  int elt, nElts=1;
  char *lastpointaddr;

  if (dcl->type != NULL)
    dcl_members = dcl->type->members;


  /* 
   * Element terminal 
   */
  if (dcl_members == NULL || dcl->type->type == ENUM) {
    if (addParamToStrings (prefix, dcl) != 0) 
      return 1;
    return 0;
  }
 
  /* 
   * Structure a decomposer 
   */

  /* On construit le prefixe de la variable (acces) */
  strcat (prefix, dcl->name);
  
  /* On parcours chaque element (1 seul si pas un tableau) */
  nElts = 1;
  for (i=0; i<dcl->ndimensions; i++) nElts *= (dcl->dimensions)[i];
  for (elt=0; elt<nElts; elt++) {

    /* Indice du tableau */
    if (dcl->ndimensions != 0) {
      sprintf (index, "%s", 
	       getIndexesStr(dcl->ndimensions, dcl->dimensions, elt));
      strcat (prefix, index);
    }
    else 
      index[0] = '\0';

    /* Fin prefixe */
    strcat (prefix, ".");

    /* Appel recursif pour les membres de la structure */
    for (l_dcl=dcl_members; l_dcl!=NULL; l_dcl=l_dcl->next)
      if (flatDcl(l_dcl->dcl_nom) != 0) 
	return 1;

    /* Nettoyage indice pour iteration suivante */
    prefix[strlen(prefix)-strlen(index)-1] = '\0';
  }
  
  /* Fini pour cette structure: on revient a la structure precedente */
  if ((lastpointaddr = strrchr(prefix, '.')) != NULL)
    lastpointaddr[1] = '\0';
  
  return 0;
} 


/**--------------------------------------------------------------------
 **  addParamToStrings
 ** 
 **/
static int addParamToStrings (char *prefix, DCL_NOM_STR *dcl_nom)
{
  static int params_size = 0;
  static int params_nb = 0;
  int i; 
  int ndims = dcl_nom->ndimensions;
  int *dims = dcl_nom->dimensions;
  char *str;
  int elt, nElts;

  /* Nouvel element: une virgule */
  if (numvar != 1) {
    bufcat(&strUsage, ", ");
    bufcat(&strList, ", ");
    bufcat(&strName, ", ");
  }  

  /* Variable de type STRING ou tableau de STRING */
  if (dcl_nom->flags & STRING)
    ndims--;

  /* Calcul du nombre d'elements */
  for (nElts=1, i=0; i<ndims; i++) 
    nElts *= (dims)[i];

  /* Controle de la taille totale 
     (la taille de la somme des parametres est limitee pour VxWorks) */
  params_nb += nElts;
  if (params_nb > VX_MAX_PARAMS) {
    fprintf (stderr, "initGen warning: too many parameters (>%d) limited to %d on VxWorks\n", params_nb, VX_MAX_PARAMS);
    /* only a problem for VxWorks */
    /*     return 1; */
  }

  params_size += nElts * getsize(dcl_nom->type);

  if (params_size > VX_MAX_PARAMS_SIZE) {
    fprintf (stderr, "initGen warning: total size of the parameters (>%d) limited to %d on VxWorks\n", params_size, VX_MAX_PARAMS_SIZE);
    /* only a problem for VxWorks */
    /*     return 1; */
  }

  /* On traite chaque element */
  for (elt=0; elt<nElts; elt++) {
    
    /** 
    ** Variable de type STRING ou tableau de STRING
    **/
    if (dcl_nom->flags & STRING) {
      bufcat(&strUsage,  "char %s%d%s", dcl_nom->name, dims[ndims],
	     getIndexesStr(ndims, dims, elt));
      bufcat(&strList,   "char var%d[%d]", numvar, dims[ndims]);
      bufcat(&strDeclar, "  char *var%d;\n", numvar);
      bufcat(&strName,   "var%d", numvar);
      
      bufcat(&strFillStruct, 
	     "  if(strlen(var%d)+1 > %d) {\n"
	     "    printf(\"%sInit: Error: String %%s too long (%%d>%d)\\\\n\", "
	     "var%d, strlen(var%d)+1);\n"
	     "  %sInitEnd (initCId);\n  }\n",
	     numvar, dims[ndims],
	     module->name, dims[ndims], 
	     numvar, numvar,
	     module->name);
      bufcat(&strFillStruct, "  strcpy(%s%s%s, var%d);\n", 
	     prefix, dcl_nom->name, 
	     getIndexesStr(ndims, dims, elt), 
	     numvar); 
      
      bufcat(&strA2Type, "  var%d = argv[%d];\n", numvar, numvar);
      bufcat(&strVerif, "  printf (\"%s%s%s = %%s\\\\n\", %s%s%s);\n",
	     prefix, dcl_nom->name, getIndexesStr(ndims, dims, elt), 
	     prefix, dcl_nom->name, getIndexesStr(ndims, dims, elt));
    } /* STRING ou tableau de STRING */
    
    /** 
    ** Autre variable ou tableau de variables 
    **/
    else {
      bufcat(&strUsage, "%s %s%s", nom_type(dcl_nom->type), dcl_nom->name, 
	     getIndexesStr(ndims, dims, elt));
      bufcat(&strList, "%s var%d", nom_type(dcl_nom->type), numvar);
      bufcat(&strDeclar, "  %s var%d;\n", nom_type(dcl_nom->type), numvar);
      bufcat(&strName, "var%d", numvar);
      
      if (dcl_nom->type->type == ENUM) {
	addVerifEnum(dcl_nom, numvar);
      }
      
      bufcat(&strFillStruct, "  %s%s%s = var%d;\n", 
	     prefix, dcl_nom->name, 
	     getIndexesStr(ndims, dims, elt), 
	     numvar);
      
      if ((str = atotype_func(dcl_nom->type)) == NULL) {
	fprintf (stderr, "initGen: type %s invalid\n",
		 nom_type(dcl_nom->type));
	return 1;
      }
      bufcat(&strA2Type, "  var%d = %s(argv[%d]);\n", numvar, str, numvar);
      /* XXXX SI C"EST UN ENUM, L"AFFICHER EN CLAIR XXX*/
      if (dcl_nom->type->type != ENUM) {
	bufcat(&strVerif, "  printf (\"%s%s%s = %s\\\\n\", %s%s%s);\n",
	       prefix, dcl_nom->name, getIndexesStr(ndims, dims, elt), 
	       get_format(dcl_nom->type),
	       prefix, dcl_nom->name, getIndexesStr(ndims, dims, elt));
      }
      else {
	DCL_NOM_LIST *m;
	bufcat(&strVerif, 
	       "  printf (\"%s%s%s = %%s (= %%d) \\\\n\",",
	       prefix, dcl_nom->name, getIndexesStr(ndims, dims, elt));
	for (m = dcl_nom->type->members; m->next != NULL; m = m->next)
	  bufcat(&strVerif, 
		 "\n    %s%s%s == %s ? \"%s\" : ",
		 prefix, dcl_nom->name, getIndexesStr(ndims, dims, elt),
		 m->dcl_nom->name, m->dcl_nom->name);
	bufcat(&strVerif, 
	       "\"%s\",\n"
	       "    %s%s%s);\n",
	       m->dcl_nom->name,
	       prefix, dcl_nom->name, getIndexesStr(ndims, dims, elt));
      }
    } /* autre variable ou tableau de variables */
    
    /**
    ** Separation par une virgule, sauf dernier element 
    **/
    if (elt != nElts-1) {
      bufcat(&strUsage, ", ");
      bufcat(&strList, ", ");
      bufcat(&strName, ", ");
    }
    
    /* Incremente le numero de l'element */
    numvar++;
    
  } /* for: Adjonction de chaque element */

  return 0;
} 


static char *atotype_func(TYPE_STR *type)
{

  switch (type->type) {
  case CHAR:
    return("");
  case SHORT:
    return(NULL);
  case INT:
  case ENUM:
    return(type->flags & UNSIGNED_TYPE ? NULL: "atoi");
  case FLOAT:
    return("atof");
  case DOUBLE:
    return("atof");
  default:
    return(NULL);
  } /* switch */

}

static char *get_format(TYPE_STR *type)
{

  switch (type->type) {
  case CHAR:
    return("%c");
  case SHORT:
  case INT:
  case ENUM:
    return("%d");
  case FLOAT:
  case DOUBLE:
    return("%f");
  default:
    return(NULL);
  } /* switch */

}


static int getsize(TYPE_STR *type)
{

  switch (type->type) {
  case CHAR:
    return sizeof(char);
  case SHORT:
    return sizeof(short);
  case INT:
  case ENUM:
    return sizeof(int);
  case FLOAT:
    return sizeof(float);
  case DOUBLE:
    return sizeof(double);
  default:
    printf ("initGen: warning do not know size of type %d (set to 1)\n", type->type);
    return(1);
  } /* switch */

}


static void addVerifEnum(DCL_NOM_STR *dcl_nom, int numvar)
{
  DCL_NOM_LIST *m;
  DCL_NOM_LIST *members;
  
  members = dcl_nom->type->members;
  bufcat(&strFillStruct, "  if (");
  for (m = members; m != NULL; m = m->next) {
    bufcat(&strFillStruct, "var%d != %s ", numvar, m->dcl_nom->name);
    if(m->next != NULL)
      bufcat(&strFillStruct, "&& ");
  }
  bufcat(&strFillStruct, ") {\n");
  bufcat(&strFillStruct, "    printf(\"Unknown value for enum %s, select between:\\\\n\");\n", dcl_nom->name);
  for (m = members; m != NULL; m = m->next) {
    bufcat(&strFillStruct, "    printf(\"   %s=%%d   \\\\n\", %s);\n",
	   m->dcl_nom->name, m->dcl_nom->name);
  }
  bufcat(&strFillStruct, "    return ERROR;\n  }\n");
}
 
