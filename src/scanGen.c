
/* 
 * Copyright (c) 1993-2003 LAAS/CNRS
 * Matthieu Herrb - Fri Jul 23 1993
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
#include "scanGen.h"
#include "genom/printScan.h"  /* Pour TAB_INDENT */

static char *scan_format_type(TYPE_STR *type);
static void genScanVal(FILE *out, DCL_NOM_STR *n, char *muette);
void genScanEnum (FILE *out, DCL_NOM_LIST *members) ;

/*** 
 *** Génération des fonctions d'impression des structures 
 ***/

int
scanGen(FILE *out)
{
    TYPE_LIST *l;
    TYPE_STR *t;
    DCL_NOM_LIST *m, *ltypedefs;
    char buf[80];
    char *str;
    ID_LIST *ln;

    const char *func_header_proto = 
"extern int scan_%s ( FILE *in, FILE *out,\n"
"       %s *x, int indent, int nDim, int *dims );\n";

    const char *func_header = 
"int scan_%s(FILE *in, FILE *out,\n"
"                 %s *x, int indent, int nDim, int *dims)\n{\n"
"  char *indstr;\n"
"  indstr=strdup(indentStr(nDim?++indent:indent));\n"
"  indent++;\n"
"  FOR_EACH_elt(nDim,dims) {\n"
"    if (nDim != 0)\n"
"      fprintf(out, \"%%s%%s%s\", "
"indentStr(indent-2), getIndexesStr(nDim, dims, elt));\n\n";


    /* FUNCS */
    script_open(out);
    cat_begin(out);

    fprintf(out, "/*------------------  Fichier généré automatiquement ------------------*/\n");
    fprintf(out, "/*------------------  Ne pas éditer manuellement !!! ------------------*/\n\n");

    fprintf(out, "#include <stdio.h>\n");
    fprintf(out, "#include <stdlib.h>\n");
    fprintf(out, "\n");
    fprintf(out, "#include <portLib.h>\n");
    fprintf(out, "#include <csLib.h>\n");
    fprintf(out, "#include <posterLib.h>\n");
    fprintf(out, "#include \"%sScan.h\"\n\n", module->name);

    /* Generation des fonctions scan */
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
	  fprintf(out, "    fscanf(in, \"%s\", x);\n}\n\n", 
		  scan_format_type(t));
	  break;

	  /* Structure: on traite chaque membre */
	case STRUCT:
	case UNION:
	  for (m = t->members; m != NULL; m = m->next) {
	    sprintf(buf, "(x+elt)->%s", m->dcl_nom->name);
	    genScanVal(out, m->dcl_nom, buf);
	  }
	  break;

	  /* Affichage en clair des symboles de l'enum */
	case ENUM:
	  genScanEnum(out, t->members);
	  break;

	case TYPEDEF:
	  fprintf(stderr, "%s: TYPEDEF dans liste types\n", nomfic);
	  break;
	}

	/* Termine la fonction */
	fprintf(out, 
		"  } END_FOR\n"
		"  free(indstr);\n"
		"  return OK;\n"
		"}\n\n");
    }

    /* 
     * Affichage des typedef: on appelle les fonctions ci-dessus
     * (qui appele directement scan_type ? )
     */
    fprintf(out, "\n/* ======================== SCAN DES TYPEDEF ============================= */\n\n");
    for (ltypedefs = typedefs; ltypedefs != NULL; 
	 ltypedefs = ltypedefs->next) {
	if (/* ltypedefs->dcl_nom->type->used == 0
	       || */ (ltypedefs->dcl_nom->type->flags & TYPE_IMPORT)) {
	    continue;
	}
	fprintf(out, func_header, ltypedefs->dcl_nom->name, 
		ltypedefs->dcl_nom->name, "\\n");
	ltypedefs->dcl_nom->pointeur++;
	genScanVal(out, ltypedefs->dcl_nom, "x+elt");
	ltypedefs->dcl_nom->pointeur--;
	/* Termine la fonction */
	fprintf(out, 
		"  } END_FOR\n"
		"  free(indstr);\n"
		"  return OK;\n"
		"}\n\n");
    }

    cat_end(out);
    script_close(out, "server/%sScan.c", module->name);

    /*
     * Generation header
     */
    script_open(out);
    subst_begin(out, PROTO_SCAN_H);

    /* Structures importees d'autres modules */
    str = NULL;
    for (ln = imports; ln != NULL; ln = ln->next) {
      bufcat(&str, "\n#include \"server/%sScan.h\"\n", ln->name);
    } /* for */
    if (str != NULL) {
      print_sed_subst(out, "externScanLibs", str);
      free(str);
    } else {
      print_sed_subst(out, "externScanLibs", "");
    }

    print_sed_subst(out, "module", module->name);
    print_sed_subst(out, "MODULE", module->NAME);

    subst_end(out);

    /* PROTOS */
    cat_begin(out);
    fprintf(out,
	"#ifdef __cplusplus\n"
	"extern \"C\" {\n"
	"#endif\n");

    /* Generation des fonctions scan */
    for (l = types; l != NULL; l = l->next) {
	t = l->type;
	if (/* t->used == 0 || */ (t->flags & TYPE_IMPORT)) {
	    continue;
	}

	/* Entete de la fonction */
	fprintf(out, func_header_proto, nom_type1(t), nom_type(t));
    }

    /* 
     * Affichage des typedef: on appelle les fonctions ci-dessus
     */
    fprintf(out, "\n/* ======================== SCAN DES TYPEDEF ============================= */\n\n");
    for (ltypedefs = typedefs; ltypedefs != NULL; 
	 ltypedefs = ltypedefs->next) {
	if (/* ltypedefs->dcl_nom->type->used == 0
	       || */ (ltypedefs->dcl_nom->type->flags & TYPE_IMPORT)) {
	    continue;
	}
	fprintf(out, func_header_proto, ltypedefs->dcl_nom->name,
		ltypedefs->dcl_nom->name);
    }
    fprintf(out,
	"#ifdef __cplusplus\n"
	"}\n"
	"#endif\n");

    fprintf(out, "\n#endif /* %s_SCAN_H */\n", module->NAME);

    cat_end(out);
    script_close(out, "server/%sScan.h", module->name);

    return(0);
} /* scanGen */

/*----------------------------------------------------------------------*/

static char *
scan_format_type(TYPE_STR *type)
{
   switch (type->type) {
     case CHAR:
       return("%c");
     case INT:
       return(type->flags & UNSIGNED_TYPE ? "%u" : "%d");
     case FLOAT:
       return("%f");
     case DOUBLE:
       return("%lf");
     default:
       return(NULL);
    } /* switch */
} /* scan_format_type */

/*----------------------------------------------------------------------*/

static void
genScanVal(FILE *out, DCL_NOM_STR *n, char *muette)
{
    char *addrstr=NULL;
    char *var, *type1;
    int newline, i;
    
    /* 
     * affiche le nom de la variable
     */
    dcl_nom_decl(n, &type1, &var);
    newline = (n->type->type == STRUCT || n->type->type == UNION ||
	       n->type->type == TYPEDEF || n->flags & ARRAY);

    fprintf(out, "    fprintf(out, \"%%s%s%s\", indstr);\n", 
	    var, newline? ":\\n" : " ");

    /*
     * appel de la fonction de lecture de cette variable
     */

    /* Type de la donnee a lire */
    free(type1);
    if(n->flags & STRING)
      type1 = strdup("string");
    else 
      type1 = nom_type1(n->type);

    /* Tableau de valeurs */
    if (n->flags & ARRAY || n->flags & STRING) {
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

    /* Appel de la fonction scan_ */
    fprintf(out, "    if (scan_%s(in, out, ", type1);
    
    if (n->pointeur == 0) {
      fprintf(out, "%s", addrstr);
    } 
    else {
      type1[0] = '\0';
      for (i = 1; i < n->pointeur; i++)
	fprintf(out, "%s*", addrstr);
    }

    if(n->flags & ARRAY || n->flags & STRING)
      fprintf(out, 
	      "%s, \n"
	      "                     %s, %d, dims) == ABORT) {\n"
	      "      free (indstr);\n"
	      "      return ABORT; } }\n\n",
	      muette, newline?"indent":"0", n->ndimensions);
    else
      fprintf(out, 
	      "%s, \n"
	      "                     %s, 0, NULL) == ABORT) {\n"
	      "      free (indstr);\n"
	      "      return ABORT; }\n\n",
	      muette, newline?"indent":"0");
    
    free(type1);
    free(var);
    free(addrstr);
    addrstr = NULL;

} /* genScanVal */

/*----------------------------------------------------------------------*/

void 
genScanEnum (FILE *out, DCL_NOM_LIST *members) 

{
  DCL_NOM_LIST *m;
  
  /* Affichage en clair des symboles de l'enum */
  fprintf(out, 
	  "    /* Affiche l'ancienne valeur */\n"
	  "    switch (*(x+elt)) {\n");
  for (m = members; m != NULL; m = m->next) {
    fprintf(out, "    case %s:\n", m->dcl_nom->name);
    fprintf(out, "      fprintf(out, \"(%s=%%d) \",%s);\n      break;\n",
	    m->dcl_nom->name, m->dcl_nom->name);
  }
  fprintf(out, "    } /* switch */\n\n");
  
  /* Saisie de l'enum */
  fprintf(out, 
	  "    /* Scan l'enum */\n"
	  "    { int trouve;\n"
	  "      do {\n"
	  "        trouve = 1;\n"
	  "        if (scan_type(in, out, \"%%d\", x+elt) == ABORT) {\n"
	  "          free (indstr);\n"
	  "          return ABORT; }\n");
  m = members;

  /* Test validite */
  fprintf(out, 
	  "        if ("
	  "%s != *(x+elt)", m->dcl_nom->name);
  if (m->next != NULL) 
    for (m = members->next; m != NULL; m = m->next) 
      fprintf(out, " && %s != *(x+elt)", m->dcl_nom->name);

  fprintf(out, 
	  ") {\n"
	  "          fprintf(out, \"Unknown value, select between:\");\n");
  for (m = members; m != NULL; m = m->next) {
    fprintf(out, "          fprintf(out, \"\\n   %s=%%d   \", %s);\n",
	    m->dcl_nom->name, m->dcl_nom->name);
  }
  fprintf(out, 
	  "          trouve = 0;\n"
	  "        }\n"
	  "      } while (! trouve);\n"
	  "    }\n"); 
}



