/*	$LAAS$ */

/* 
 * Copyright (c) 1997-2003 LAAS/CNRS
 * Sara Fleury - May 1997
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
#include <string.h>
#include <stdlib.h>

#include "genom.h"
#include "parser.tab.h"
#include "scopeGen.h"

static void genScopeDisplayVal(FILE *out, int nbMembers, DCL_NOM_LIST *m);
static void genScopeGetVal(FILE *out, DCL_NOM_LIST *m);
static char * genScopePosterFunc(int *nb);

  const char *file_header = 
"/*------------------  Fichier généré automatiquement ------------------*/\n"
"/*------------------  Ne pas éditer manuellement !!! ------------------*/\n\n"
"#ifndef UNIX\n"
"#include <vxWorks.h>\n"
"#else\n"
"#include <portLib.h>\n"
"#endif\n"
"#include <stdio.h>\n"
"#include <stdlib.h>\n"
"#include \"%sScopeLib.h\"\n\n"
"/*\n"
" * Fonction est passe en parametre de h2scopeIhmTxtMain\n"
" */\n"
"BOOL %sScopeAll (char *rack, char *cpu, H2_SCOPE_SIG_STR **scopeList,\n"
"                    H2_SCOPE_SDI_OR_POSTER sdiOrPoster)\n"
"{\n"
"  int selection;\n"
"  while(1) {\n"
"    selection = h2scopeIhmTxtSelectSdiOrPoster(sdiOrPoster, \"%s\");\n"
"\n"
"    if (selection == H2_SCOPE_GO_UP) return TRUE;\n"
"    if (selection == H2_SCOPE_GO_TOP) return FALSE;\n"
"    if (selection == H2_SCOPE_MODULE_SDI) {\n"
"      if (%sScopeSdi (rack, cpu, scopeList)) continue;\n"
"      else return FALSE;\n"
"    }\n"
"    if (selection == H2_SCOPE_MODULE_POSTER) {\n"
"      if (%sScopePoster (rack, scopeList)) continue;\n"
"      else return FALSE;\n"
"    }\n"
"  }\n"
"}\n"
"\n"
"BOOL %sScopeSdi (char *rack, char *cpu, H2_SCOPE_SIG_STR **scopeList)\n"
"{\n"
"  return scope_struct_%s(1, \"%sSdi\", rack, cpu, \n"
"				     \"*%sDataStrId\",\n"
"				     0, 0, NULL, scopeList);\n"
"}\n\n";

const char *func_header_proto = 
"extern BOOL scope_%s (int indent, char *title, char *rack, char *cpu, char *sdi, int off, int nDim, int *dims, H2_SCOPE_SIG_STR **list);\n";

  const char *func_header = 
"BOOL scope_%s(int indent, char *title, char *rack, char *cpu, char *sdi, int off, int nDim, int *dims, H2_SCOPE_SIG_STR **list)\n{\n"
"  int selection;\n"
"  int elt, nElts, addoff;\n\n"
"  indent++;\n"
"  while (1) {\n"
"      printf(\"\\nselect in %%s (0: up, -1: top):\\n\",  title);\n";
/*"      printf(\"%%*s 0  (up ...)\\n\", indent*INDENT_TAB,\"\");\n";*/

  const char *foreach_str =
"      nElts=1;\n"
"      for (elt=0;elt<nDim;elt++) nElts *= (dims)[elt];\n"
"      for (elt=0;elt<nElts;elt++) {\n";

  const char *func_start =
"        if (nDim != 0)\n"
"          printf(\"%%s\", h2scopeGetIndexesStr(nDim, dims, elt));\n\n";

  const char *func_middle =
"      } /* for */ \n"
"      selection=0;\n"
"      h2scopeIhmTxtGetNumber(\"\\nselection\", &selection);\n"
"      if(selection == H2_SCOPE_GO_UP) return TRUE;\n"
"      if(selection == H2_SCOPE_GO_TOP) return FALSE;\n"
"      if(selection > nElts*%d || selection < 0) continue;\n"
"      elt = (int)((selection-1)/%d);\n"
"      selection -= %d*elt;\n"
"      addoff = elt*sizeof(%s);\n"
"    \n";

  const char *func_end =
"  } /* while */\n"
"}\n\n";

/*** 
 *** Génération des fonctions d'installation de signaux scope sur
 ***  des elements de la SDI
 ***/
int
scopeGen(FILE *out)
{
  TYPE_LIST *l;
  TYPE_STR *t;
  DCL_NOM_LIST *m;
  char *str;
  ID_LIST *ln;
  int nbMembers;
  int nbPosters;
  char *posterFunc;

  /**
   ** Les prototypes
   **/
  script_open(out);
  cat_begin(out);

  fprintf (out, 
	   "extern BOOL %sScopeAll (char *rack, char *cpu, H2_SCOPE_SIG_STR **scopeList,\n"
	   "                    H2_SCOPE_SDI_OR_POSTER sdiOrPoster);\n",
	   module->name);

  fprintf (out, 
	   "extern BOOL %sScopePoster (char *rack, H2_SCOPE_SIG_STR **scopeList);\n", module->name);

  fprintf (out, 
	   "extern BOOL %sScopeSdi (char *rack, char *cpu, H2_SCOPE_SIG_STR **scopeList);\n",
	   module->name);

  for (l = types; l != NULL; l = l->next) {
    t = l->type;
    if (t->used == 0 || (t->flags & TYPE_IMPORT)) {
      continue;
    }
    
    /* Structure: on traite chaque membre */
    if (t->type == STRUCT || t->type == UNION) {
	
	/* Entete de la fonction */
	fprintf(out, func_header_proto, nom_type1(t));
    }
  }

  cat_end(out);
  script_close(out, "server/%sScopeLibProto.h", module->name);

  /**
   ** La fonction main totoScope dans totoScope.c
   **/
  script_open(out);
  subst_begin(out, PROTO_SCOPE_MAIN_C);
  print_sed_subst(out, "module", module->name);
  print_sed_subst(out, "MODULE", module->NAME);
  print_sed_subst(out, "internalData", module->internal_data->name);
  subst_end(out);
  script_close(out, "server/%sScopeMain.c", module->name);
  
  /**
   ** Fichier header totoScopeLib.h de la bibilotheque totoScopeLib.c
   **/
  script_open(out);
  subst_begin(out, PROTO_SCOPE_LIB_H);
  
  /* Structures importees d'autres modules */
  str = NULL;
  for (ln = externLibs; ln != NULL; ln = ln->next) {
    bufcat(&str, "\n#include \"server/%sScopeLib.h\"\n", ln->name);
  } /* for */
  if (str != NULL) {
    print_sed_subst(out, "externScopeLibs", str);
    free(str);
  } else {
    print_sed_subst(out, "externScopeLibs", "");
  }
  
  print_sed_subst(out, "module", module->name);
  print_sed_subst(out, "MODULE", module->NAME);
  
  subst_end(out);
  script_close(out, "server/%sScopeLib.h", module->name);

  /**
   ** La bibliotheque totoScopeLib.c des fonctions pour chaque type
   **/
  script_open(out);
  cat_begin(out);

  /* Entete du fichier */
  fprintf(out, file_header, 
	  module->name, module->name, module->name, module->name,
	  module->name, module->name, module->internal_data->name,
	  module->name, module->name);

  /* Fonction d'acces aux posters */
  posterFunc = genScopePosterFunc(&nbPosters);
  fprintf(out, "%s", posterFunc);
  free(posterFunc);

  for (l = types; l != NULL; l = l->next) {
    t = l->type;
    if (t->used == 0 || (t->flags & TYPE_IMPORT)) {
      continue;
    }
    
    /* Structure: on traite chaque membre */
    if (t->type == STRUCT || t->type == UNION) {
	
	/* Entete de la fonction */
	fprintf(out, func_header, nom_type1(t));
	fprintf(out, foreach_str);
	fprintf(out, func_start);

	/* Comptage du nombre de membre */
	nbMembers=0;
	for (m = t->members; m != NULL; m = m->next)
	  nbMembers++;

	/* Corps de la partie d'affichage */
	genScopeDisplayVal(out, nbMembers, t->members);

	/* Partie commune */
	fprintf(out, func_middle, 
		nbMembers, nbMembers, nbMembers, nom_type(t));
	
	/* Corps de la partie de saisie  */
	genScopeGetVal(out, t->members);
	
	/* Termine la fonction */
	fprintf(out, func_end);
    }
    /*    else
      printf ("Case %d not handle for %s\n", t->type, t->name);*/

  } /* for */

  cat_end(out);
  script_close(out, "server/%sScopeLib.c", module->name);
  
  
  return(0);
} /* scopeGen */

static char *
genScopePosterFunc(int *nbPosters)
{
  POSTER_LIST *p;
  int num=0;
  char *msg = NULL;

  /* Entete fonction + menu */
  bufcat(&msg,
	 "\nBOOL %sScopePoster (char *rack, H2_SCOPE_SIG_STR **scopeList)\n"
	 "{\n"
	 "  int selection;\n"
	 "  while(1) {\n"
	 "    printf (\"\nselect poster (0: up, -1: top):\\n\"",
	 module->name); 
  
  /* Nom de chaque poster */
  for (p = posters; p != NULL; p = p->next) {
    bufcat(&msg, "\n    \"    %d  %s%s\\n\"", ++num, module->name, p->name);
  } 
  /* Pas de poster */
  if (num == 0)
    bufcat(&msg, "\n    \"      (no poster)\\n\"");
  /* Termine le menu */
  bufcat(&msg,");\n");

  bufcat(&msg,
	 "    selection = 1;\n"
	 "    h2scopeIhmTxtGetNumber(\"\\nselection\", &selection);\n"
	 "    if(selection == H2_SCOPE_GO_UP) return TRUE;\n"
	 "    if(selection == H2_SCOPE_GO_TOP) return FALSE;\n");

  num = 0;

  for (p = posters; p != NULL; p = p->next) {
    bufcat(&msg,
	   "    if (selection == %d) \n"
	   "      if(scope_struct_%s (1, \"%s%s\", rack, \n"
	   "                   H2_SCOPE_CPU_POSTER, \"%s%s\",\n"
	   "                   0, 0, NULL, scopeList)) continue;\n"
	   "      else return FALSE;\n",
	   ++num, p->type->name, module->name, p->name, module->name, p->name);

  } /* for */
  
  bufcat(&msg,
	 "  }\n"
	 "}\n\n");
  *nbPosters = num;
  return msg;
}

/*----------------------------------------------------------------------*/

static void
genScopeDisplayVal(FILE *out, int nbMembers, DCL_NOM_LIST *list)
{
    int composed, notscope;
    int i=1;
    char *var, *type1;
    DCL_NOM_LIST *m;
    DCL_NOM_STR *n;

    for (m = list; m != NULL; m = m->next) {
      n = m->dcl_nom;

      dcl_nom_decl(n, &type1, &var);
      composed = (n->type->type == STRUCT || n->type->type == UNION ||
		  n->type->type == TYPEDEF || n->flags & ARRAY);

      notscope = (n->type->type == CHAR || n->type->type == SHORT
		  || n->flags & STRING || n->pointeur != 0);

      fprintf(out, "        printf(\"%%*s%%2d  %s%s\\n\", (indent-1)*INDENT_TAB, \"\", elt*%d+%d);\n", 
	      var, notscope ? " (not scopable)": (composed?" ...":""),
	      nbMembers, i++);
      
      free(var);
      free(type1);
    }
} /* genScopeVal */

/*----------------------------------------------------------------------*/

static void
genScopeGetVal(FILE *out, DCL_NOM_LIST *list)
{
    int i=0;
    int nbMembers=0;
    char *var, *type1;
    char *tabsize=NULL;
    DCL_NOM_LIST *m;
    DCL_NOM_STR *n;

    for (m = list; m != NULL; m = m->next) {
      n = m->dcl_nom;

      fprintf (out, "      if(selection>%d)", ++nbMembers);
      
      /* Nom de la variable */
      dcl_nom_decl(n, &type1, &var);
      free(type1);

      /* Type de la donnee a scoper */
      if(n->type->type == ENUM || n->pointeur != 0)
	type1=strdup("int");
      else
	type1 = nom_type1(n->type);
      
      /* Offset pour un tableau de valeurs */
      if(n->flags & ARRAY || n->flags & STRING)
	for (i=0;i<n->ndimensions;i++)
	  bufcat(&tabsize, "%d*", n->dimensions[i]);
      else
	bufcat(&tabsize, "");
      
      /* Calcul de l'offset */
      fprintf(out,
	      " addoff += %ssizeof(%s);\n",
	      tabsize, n->pointeur == 0?nom_type(n->type):"int");
      

      /* Type de base non scope */
      if (n->type->type == CHAR || n->type->type == SHORT 
	  || n->flags & STRING || n->pointeur != 0) {
	fprintf(out,"      else continue;\n");

	/* fin traitement */
	free(var);
	free(type1);
	free(tabsize);
	tabsize=NULL;
	continue;
      }
      else
	fprintf(out,"      else { ");

      /* Tableau de valeurs */
      if(n->flags & ARRAY) {
	fprintf(out, "int dims[%d] = {%d", n->ndimensions, n->dimensions[0]);
	for (i=1;i<n->ndimensions;i++)
	  fprintf(out, ",%d", n->dimensions[i]);
	fprintf(out, "};\n             ");
      }
      
      /* Appel de la fonction scope_ */
      
      /* Type de base */
      if (n->type->type == INT || n->type->type == FLOAT || 
	  n->type->type == DOUBLE || n->type->type == ENUM) {
	

	fprintf(out,
		"if(h2scopeIhmTxtRecordModuleSig(rack, cpu, sdi, \"%s\", off+addoff,\n"
		"                                   \"%s\", %d, %s, list)) continue;\n"
		"             else return FALSE;}\n",
		type1, var, 
		n->flags & ARRAY ? n->ndimensions : 0,
		n->flags & ARRAY ? "dims" : "NULL");
      }

      /* Type specifique */
      else {
	fprintf(out, 
		"\n#ifdef VXWORKS\n"
		"        if(!h2scopeSymTest(\"scope_%s\")) continue;\n"
		"#endif\n"
		"        if(scope_%s(indent, \"%s\", rack, cpu, sdi, \n"
		"                      off+addoff, %d, %s, list)) continue;\n"
		"        return FALSE;}\n",
		type1, type1, var,
		n->flags & ARRAY ? n->ndimensions : 0,
		n->flags & ARRAY ? "dims" : "NULL");
      }

      /* fin traitement */
      free(var);
      free(type1);
      free(tabsize);
      tabsize=NULL;
    }

} /* genScopeVal */

