/*	$LAAS$ */

/* 
 * Copyright (c) 1999-2003 LAAS/CNRS
 * Sara Fleury - Fri Jun  4 1999
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
#include "propiceGen.h"

static void genDecodeTerm(FILE *out, DCL_NOM_STR *n, int typedefFlag);
static void genDecodeEnum (FILE *out, DCL_NOM_LIST *members, char *name, char *name1) ;
static void genPropiceDecode (FILE *out, int protos);

static const char *func_header_proto = 
"extern Term *pu_decode_genom_%s(char *name, %s *str, int tabsize);\n";

static const char *func_header = 
"Term *pu_decode_genom_%s(char *name, %s *str, int tabsize)\n{\n"
"  Pred_Func_Rec *fr = find_or_create_pred_func(declare_atom((name?name:\"%s\")));\n"
"  TermList tl = sl_make_slist();\n"
"  int elt;\n"
"  for(elt=0;elt<tabsize;elt++) {\n";

static const char *func_tail =  
"  }\n"
" return build_term_expr(build_expr_pfr_terms(fr, tl));\n"
"}\n\n";

static const char *func_td_header_proto = 
"extern Term *pu_decode_genom_%s(char *name, %s *str, int tabsize);\n";

static const char *func_td_header = 
"Term *pu_decode_genom_%s(char *name, %s *str, int tabsize)\n{\n";

static const char *func_td_tail =  
"}\n\n";

/*** 
 *** Génération des fonctions de decodage des structures (C -> Propice)
 ***/

int
propiceDecodeGen(FILE *out)
{
    char *str;
    ID_LIST *ln;

    /* ------------------------------------------------------------
     *  LES FICHERS SOURCES
     */
    script_open(out);
    
    /* Entete */
    subst_begin(out, PROTO_PROPICE_DECODE_C);
    print_sed_subst(out, "module", module->name);
    print_sed_subst(out, "MODULE", module->NAME);
    subst_end(out);
    
    cat_begin(out);
    genPropiceDecode(out, 0 /* ! protos */);

    cat_end(out);
    script_close(out, "propice/%sDecodePropice.c", module->name);

    /* ------------------------------------------------------------
     *  PROTO
     */
    script_open(out);
    cat_begin(out);
    genPropiceDecode(out, 1 /* ! protos */);

    cat_end(out);
    script_close(out, "propice/%sDecodePropiceProto.h", module->name);

    /* ---------------------------------------------------------------------
     * Generation fichier header
     */
    script_open(out);
    subst_begin(out, PROTO_PROPICE_DECODE_H);

    /* Structures importees d'autres modules */
    str = NULL;
    for (ln = externLibs; ln != NULL; ln = ln->next) {
      bufcat(&str, "\n#include \"%sDecodePropice.h\"\n", ln->name);
    } /* for */
    if (str != NULL) {
      print_sed_subst(out, "externPropiceDecodeLibs", str);
      free(str);
    } else {
      print_sed_subst(out, "externPropiceDecodeLibs", "");
    }

    print_sed_subst(out, "module", module->name);
    print_sed_subst(out, "MODULE", module->NAME);

    subst_end(out);
    script_close(out, "propice/%sDecodePropice.h", module->name);

    return(0);
} /* scanGen */



/*-------------------------------------------------------------------------*/
/*                          FONCTIONS LOCALES                              */
/*-------------------------------------------------------------------------*/
static void genPropiceDecode (FILE *out, int protos)
{
    TYPE_LIST *l;
    TYPE_STR *t;
    DCL_NOM_LIST *m, *ltypedefs;
    int numTerm;

    /* Generation des fonctions d'encodage */
    for (l = types; l != NULL; l = l->next) {
      t = l->type;
      if (/* t->used == 0 ||*/ (t->flags & TYPE_IMPORT) 
	  || (t->flags & TYPE_INTERNAL_DATA))
	continue;

      numTerm = 1;
      
      if (protos) {
	/* Entete de la fonction */
	fprintf(out, func_header_proto, nom_type1(t), nom_type(t), t->name);
      }
      else {

	/* Entete de la fonction */
	fprintf(out, func_header, nom_type1(t), nom_type(t), t->name);

	/* Corps de la fonction */
	switch (t->type) {

	  /* Types de base: normalement on ne passe pas la... */
	case CHAR:
	case SHORT:
	case INT:
	case FLOAT:
	case DOUBLE:
	  fprintf(stdout, "propiceDecodeGen error while parsing %s\n", 
		  nom_type1(t));
	  break;

	  /* Structure: on traite chaque membre */
	case STRUCT:
	case UNION:
	  for (m = t->members; m != NULL; m = m->next) {
	    genDecodeTerm(out, m->dcl_nom,  0);
	  }
	  break;

	  /* Affichage en clair des symboles de l'enum */
	case ENUM:
	  genDecodeEnum(out, t->members, nom_type(t), t->name);
	  break;

	case TYPEDEF:
	  fprintf(stderr, "%s: TYPEDEF dans liste types\n", nomfic);
	  break;
	}

	/* Termine la fonction */
	fprintf(out, func_tail);
      }
    }

    /* 
     * Manipulation des typedef (utilise les fonctions precedement produites)
     */
    fprintf(out, "\n/* ======================== DECODE TYPEDEF ============================= */\n\n");
    for (ltypedefs = typedefs; ltypedefs != NULL; 
	 ltypedefs = ltypedefs->next) {

      if (/* ltypedefs->dcl_nom->type->used == 0
	     ||*/ (ltypedefs->dcl_nom->type->flags & TYPE_IMPORT)
		  || (ltypedefs->dcl_nom->type->flags & TYPE_INTERNAL_DATA))
	continue;
      
      if (protos) {
	fprintf(out, func_td_header_proto, ltypedefs->dcl_nom->name, 
		ltypedefs->dcl_nom->name);
      }
      else {
	fprintf(out, func_td_header, ltypedefs->dcl_nom->name, 
		ltypedefs->dcl_nom->name);

	ltypedefs->dcl_nom->pointeur++;

	fprintf(out, 
		"      return  pu_decode_genom_%s(name, str, tabsize);\n",
		nom_type1(ltypedefs->dcl_nom->type));


/* 	genDecodeTerm(out, ltypedefs->dcl_nom, 1); */
	ltypedefs->dcl_nom->pointeur--;
	/* Termine la fonction */
	fprintf(out, func_td_tail);
      }
    }
}

static void
genDecodeTerm(FILE *out, DCL_NOM_STR *dclDesc, int typedefFlag)
{
    char *addrstr=NULL;
    char *var, *type1;
    int i;
    int nbelts = 1;
    int maxLength = 0;


    /* Nom de la variable */
    dcl_nom_decl(dclDesc, &type1, &var);
    free(type1);

    /* Type de la donnee a lire */
    if(dclDesc->flags & STRING)
      type1 = strdup("string");
    else 
      type1 = nom_type1(dclDesc->type);

    /* Tableau de valeurs */
    nbelts = 1;
    bufcat(&addrstr, "");
    if (dclDesc->flags & ARRAY) {
      for(i=0;i<dclDesc->ndimensions;i++)
	nbelts *= dclDesc->dimensions[i];
      /* cast */
      bufcat(&addrstr, "(%s *)", nom_type(dclDesc->type));
    }
    else if(dclDesc->flags & STRING) {
      maxLength = dclDesc->dimensions[0];
      for(i=1;i<dclDesc->ndimensions;i++)
	nbelts *= dclDesc->dimensions[i];
    }
    else 
      bufcat(&addrstr, "&");

    /* Debut de la fonction */
    fprintf(out, 
	    "      sl_add_to_tail(tl, pu_decode_genom_%s(\"%s\", ",
	    type1, dclDesc->name);
	    
    /* Prefixe selon type de la variable: Pointeur? Tableau? ... */
    if (dclDesc->pointeur == 0) 
      fprintf(out, "%s", addrstr);
    else 
      for (i = 1; i < dclDesc->pointeur; i++)
	fprintf(out, "%s*", addrstr);

    /* Reference a la variable dans la structure */
    if (typedefFlag)
      fprintf(out, "str+elt");
    else
      fprintf(out, "(str+elt)->%s", dclDesc->name);


    /* La fin de la fonction */
    /* La fin de la fonction */
    if(dclDesc->flags & STRING)
      fprintf(out,
	       ", %d, %d));\n",
	      nbelts, maxLength);
    else
	 fprintf(out,
		 ", %d));\n",
		 nbelts);
    
    free(type1);
    free(var);
    free(addrstr);
    addrstr = NULL;

} /* genDecodeTerm */

/*----------------------------------------------------------------------*/

static void 
genDecodeEnum (FILE *out, DCL_NOM_LIST *members, char *type, char *type1) 

{
  DCL_NOM_LIST *m;

  m = members;

  /* Cherche l'enum correspondant  (test le premier) */
  fprintf(out, 
	  "      if (*(str+elt) == %s) add_to_tail(tl, PUMakeTermAtom(\"%s\"));\n",
	  m->dcl_nom->name, m->dcl_nom->name);

  /* Cherche l'enum correspondant (test les suivants) */
  if (m->next != NULL) 
       for (m = members->next; m != NULL; m = m->next) 
	    fprintf(out, 
		    "      else if (*(str+elt) == %s) add_to_tail(tl, PUMakeTermAtom(\"%s\"));\n",
		    m->dcl_nom->name, m->dcl_nom->name);
  
  /* Pas trouve */
  fprintf(out, 
	  "      else {\n"
	  "        fprintf(stdout, \"Not valid value %%d for %s\\n\", *(str+elt));\n"
	  "        add_to_tail(tl, PUMakeTermAtom(\"UNKNOWN_%s_ENUM_VALUE\"));\n"
	  "      }\n",
	  type,type1);
}



