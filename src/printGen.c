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
#include <string.h>
#include <stdlib.h>

#include "genom.h"
#include "parser.tab.h"
#include "printGen.h"

static char *format_type(TYPE_STR *type);
void genPrintVal(FILE *out, DCL_NOM_STR *n, char *muette);
void genPrintEnum(FILE *out, DCL_NOM_LIST *members);

/*** 
 *** Génération des fonctions d'impression des structures 
 ***/

int
printGen(FILE *out)
{
    TYPE_LIST *l;
    TYPE_STR *t;
    DCL_NOM_LIST *m, *ltypedefs;
    char buf[80];
    char *str;
    ID_LIST *ln;

    const char *func_header_proto = 
"extern void print_%s ( FILE *out,\n"
"     %s *x,\n"
"     int indent, int nDim, int *dims, FILE *in );\n";

    const char *func_header = 
"void print_%s(FILE *out, %s *x,\n"
"                      int indent, int nDim, int *dims, FILE *in)\n{\n"
"  char *indstr;\n"
"  indstr=strdup(indentStr(nDim?++indent:indent));\n"
"  indent++;\n"
"  FOR_NB_elt(nDim,dims) {\n"
"    if (nDim != 0)\n"
"      fprintf(out, \"%%s%%s%s\", "
"indentStr(indent-2), getIndexesStr(nDim, dims, elt));\n\n";

    /* protos */
    script_open(out);
    cat_begin(out);
    /* Generation des fonctions print */
    for (l = types; l != NULL; l = l->next) {
	t = l->type;
	if (/* t->used == 0 || */ (t->flags & TYPE_IMPORT)) {
	    continue;
	}
	fprintf(out, func_header_proto, nom_type1(t), nom_type(t));
    }

    fprintf(out, "\n/* ======================== PRINT DES TYPEDEF ============================= */\n\n");
    for (ltypedefs = typedefs; ltypedefs != NULL; 
	 ltypedefs = ltypedefs->next) {
	if (/* ltypedefs->dcl_nom->type->used == 0
	       ||*/ (ltypedefs->dcl_nom->type->flags & TYPE_IMPORT)) {
	    continue;
	}
	fprintf(out, func_header_proto, ltypedefs->dcl_nom->name, 
		ltypedefs->dcl_nom->name);
    }

    cat_end(out);
    script_close(out, "%sPrintProto.h", module->name);


    /* functions */
    script_open(out);
    cat_begin(out);

    fprintf(out, "/*------------------  Fichier généré automatiquement ------------------*/\n");
    fprintf(out, "/*------------------  Ne pas éditer manuellement !!! ------------------*/\n\n");

    fprintf(out, "#ifdef VXWORKS\n");
    fprintf(out, "#include <vxWorks.h>\n");
    fprintf(out, "#else\n");
    fprintf(out, "#include <portLib.h>\n");
    fprintf(out, "#endif\n");
    fprintf(out, "#include <stdio.h>\n");
    fprintf(out, "#include <stdlib.h>\n");
    fprintf(out, "#include \"csLib.h\"\n");
    fprintf(out, "#include \"posterLib.h\"\n");
    fprintf(out, "#include \"%sPrint.h\"\n\n", module->name);

    /* Generation des fonctions print */
    for (l = types; l != NULL; l = l->next) {
	t = l->type;
	if (/* t->used == 0 || */ (t->flags & TYPE_IMPORT)) {
	    continue;
	}

	/* Entete de la fonction */
	fprintf(out, func_header, nom_type1(t), nom_type(t), 
		t->type == ENUM ? "" : "\\n");

	/* Corps de la fonction */
	switch (t->type) {

	  /* Types de base: normalement on ne passe pas la... */
	case CHAR:
	case SHORT:
	case INT:
	case FLOAT:
	case DOUBLE:
	  fprintf(out, "    fprintf(out, \"%s\", *x);\n}\n\n", 
		  format_type(t));
	  break;

	  /* Structure: on traite chaque membre */
	case STRUCT:
	case UNION:
	  for (m = t->members; m != NULL; m = m->next) {
	    sprintf(buf, "((x+elt)->%s)", m->dcl_nom->name);
	    genPrintVal(out, m->dcl_nom, buf);
	  }
	  break;

	  /* Affichage en clair des symboles de l'enum */
	case ENUM:
	  genPrintEnum(out, t->members);
	  break;

	case TYPEDEF:
	  fprintf(stderr, "%s: TYPEDEF dans liste types\n", nomfic);
	  break;
	}

	/* Termine la fonction */
	fprintf(out, 
		"  } END_FOR\n"
		"  free(indstr);\n"
		"}\n\n");
    }

    /* 
     * Affichage des typedef: on appelle les fonctions ci-dessus
     * (qui appele directement scan_type ? )
     */
    fprintf(out, "\n/* ======================== PRINT DES TYPEDEF ============================= */\n\n");
    for (ltypedefs = typedefs; ltypedefs != NULL; 
	 ltypedefs = ltypedefs->next) {
	if (/* ltypedefs->dcl_nom->type->used == 0
	       ||*/ (ltypedefs->dcl_nom->type->flags & TYPE_IMPORT)) {
	    continue;
	}
	fprintf(out, func_header, ltypedefs->dcl_nom->name, 
		ltypedefs->dcl_nom->name, "\\n");
	genPrintVal(out, ltypedefs->dcl_nom, "(*(x+elt))");
	fprintf(out, 
		"  } END_FOR\n"
		"  free(indstr);\n"
		"}\n\n");
    }

    cat_end(out);
    script_close(out, "%sPrint.c", module->name);

    /*
     * Generation header
     */
    script_open(out);
    subst_begin(out, PROTO_PRINT_H);

    /* Structures importees d'autres modules */
    str = NULL;
    for (ln = externLibs; ln != NULL; ln = ln->next) {
      bufcat(&str, "\n#include \"auto/%sPrint.h\"\n", ln->name);
    } /* for */
    if (str != NULL) {
      print_sed_subst(out, "externPrintLibs", str);
      free(str);
    } else {
      print_sed_subst(out, "externPrintLibs", "");
    }

    print_sed_subst(out, "module", module->name);
    print_sed_subst(out, "MODULE", module->NAME);

    subst_end(out);
    script_close(out, "%sPrint.h", module->name);

    return(0);
} /* printGen */

/*----------------------------------------------------------------------*/

static char *
format_type(TYPE_STR *type)
{
   switch (type->type) {
     case CHAR:
       return("%c");
     case INT:
       return(type->flags & UNSIGNED_TYPE ? "%u" : "%d");
     case FLOAT:
     case DOUBLE:
       return("%f");
     default:
       return(NULL);
    }
} /* format_type */

/*----------------------------------------------------------------------*/

void
genPrintVal(FILE *out, DCL_NOM_STR *n, char *muette)
{
    char *addrstr=NULL;
    int newline, i;
    char *var, *type1;

    /* 
     * affiche le nom de la variable
     */
    dcl_nom_decl(n, &type1, &var);
    newline = (n->type->type == STRUCT || n->type->type == UNION ||
	       n->type->type == TYPEDEF || n->flags & ARRAY);

    fprintf(out, "    fprintf(out, \"%%s%s%s\", indstr);\n", 
	    var, newline? ":\\n" : " = ");

    /*
     * appel de la fonction d'affichage de cette variable
     */

    /* Type de la donnee a afficher */
    free(type1);
    if(n->flags & STRING)
      type1 = strdup("string");
    else 
      type1 = nom_type1(n->type);

    /* Tableau de valeurs */
    if(n->flags & ARRAY || n->flags & STRING) {
      fprintf(out, 
	      "    { int dims[%d] = {%d",  
	      n->ndimensions, n->dimensions[0]);
      for (i=1;i<n->ndimensions;i++)
	fprintf(out, ",%d", n->dimensions[i]);
      fprintf(out, "};\n  ");
      bufcat(&addrstr, "(%s *)", nom_type(n->type));
    }
    else 
      bufcat(&addrstr, "&");

    /* Appel de la fonction print_ */
    if (n->pointeur == 0) {
	fprintf(out, "    print_%s(out, %s%s, ", 
		type1, addrstr, muette);
    } 
    else {
#ifdef POINTER_PRINT_ADDR
	fprintf(out, "    print_addr(out, (void *)(%s), ",
		muette);
#else
	fprintf(out, "    print_%s(out, ", type1);
	for (i = 1; i < n->pointeur; i++) {
	    fprintf(out, "*");
	} /* for */
	fprintf(out, "(%s%s), ", addrstr, muette);
#endif
    }
    if(n->flags & ARRAY || n->flags & STRING)
      fprintf(out, "%s, %d, dims, in); }\n\n", 
	      newline==1?"indent":"0", n->ndimensions);
    else
      fprintf(out, "%s, 0, NULL, in);\n\n",
	      newline==1?"indent":"0");

    free(var);
    free(type1);
    free(addrstr);
    addrstr = NULL;

} /* genPrintVal */

/*----------------------------------------------------------------------*/

void 
genPrintEnum(FILE *out, DCL_NOM_LIST *members)

{
  DCL_NOM_LIST *m;
  
  fprintf(out, "    switch (*(x+elt)) {\n");
  for (m = members; m != NULL; m = m->next) {
    fprintf(out, 
	    "    case %s:\n"
	    "      fprintf(out, \"%s=%%d\\n\", %s);\n"
	    "      break;\n",
	    m->dcl_nom->name, m->dcl_nom->name, m->dcl_nom->name);
  } /* for */
  fprintf(out, "   default:\n");  
  fprintf(out, "     fprintf(out, \"unknown enum value %%d\\n\", *(x+elt));\n");
  fprintf(out, "    } /* switch */\n");

} /* genPrintEnum */

