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
#include <stdlib.h>

#include "genom.h"
#include "parser.tab.h"
#include "typeGen.h"

/***
 *** Ge'ne'ration des de'clarations de type du module
 ***/

int typeGen(FILE *out)
{
    ID_LIST *il;
    DCL_NOM_LIST *l, *m, *l1;
    TYPE_STR *t;
    char *type, *var;

    script_open(out);
    subst_begin(out, PROTO_TYPE_H);
    /* Nom du  module */
    print_sed_subst(out, "module", module->name);
    print_sed_subst(out, "MODULE", module->NAME);
    
    /* Nombre de taches d'exec */
    print_sed_subst(out, "nbExecTask", "%d", nbExecTask);

    subst_end(out);
    
    cat_begin(out);

    fprintf(out, "#ifndef %s_TYPE1_H\n", module->name);
    fprintf(out, "#define %s_TYPE1_H\n\n",  module->name);

    for (il = includeFiles; il != NULL; il = il->next) {
	fprintf(out, "#include \"%s\"\n", il->name);
    } /* for */
    fprintf(out, "\n");

    /* Structure de donne'es interne */
    for (l = typedefs; l != NULL; l = l->next) {
	t = l->dcl_nom->type;
	if (t->used == 0) {
	    continue;
	}
	/* Ne genere pas les types deja declares */
	if (((t->flags & TYPE_INTERNAL_DATA) != TYPE_INTERNAL_DATA)
	    && ((t->flags & TYPE_GENOM) != TYPE_GENOM)) {
	    continue;
	}
	if (t->flags & TYPE_INTERNAL_DATA) {
	    /* trouve' la structure interne */
	    fprintf(out, "/*\n"
		    " * Structure de donnees interne\n"
		    " */\n");
	} else {
	    fprintf(out, "/*\n"
		    " * Type declare' dans %s\n"
		    " */\n", nomfic);
	}
	l->dcl_nom->flags = 1;
	fprintf(out, "typedef %s", nom_type(t));
	if (t->type == STRUCT || t->type == UNION || t->type == ENUM) {
	    fprintf(out, " {\n");
	    if (t->members == NULL) {
		trouve_members(t);
	    }
	    for (m = t->members; m != NULL; m = m->next) {
	       if (t->type == ENUM) {
		  if (m->dcl_nom->flags == ENUM_VALUE)
		     fprintf(out, "\t%s = %d%s\n",
			     m->dcl_nom->name, m->dcl_nom->pointeur,
			     m->next?",":"");
		  else
		     fprintf(out, "\t%s%s\n", m->dcl_nom->name,
			     m->next?",":"");
	       } else {
		  dcl_nom_decl(m->dcl_nom, &type, &var);
		  fprintf(out, "\t%s %s;\n", type, var);
		  free(type);
		  free(var);
	       }
	    } /* for */
	    fprintf(out, "} ");
	}
	dcl_nom_decl(l->dcl_nom, &type, &var);
	fprintf(out, "%s", var);
	free(type);
	free(var);
	/* recherche d'autres noms pour ce type */
	for (l1 = typedefs; l1 != NULL; l1 = l1->next) {
	    if (l1->dcl_nom->flags == 1) {
		continue;
	    }
	    if (l1->dcl_nom->type == t) {
		dcl_nom_decl(l1->dcl_nom, &type, &var);
		l1->dcl_nom->flags = 1;
		fprintf(out, ", %s", var);
		free(type);
		free(var);
	    }
	} /* for */
	fprintf(out, ";\n\n");
    } /* for */


    fputs("\n#endif\n", out);
    
    cat_end(out);
    script_close(out, "%sType.h", module->name);

    return(0);
}


