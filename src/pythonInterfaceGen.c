#include <stdio.h>
#include <stdlib.h>

#include "genom.h"
#include "parser.tab.h"

int genTypeName(const DCL_NOM_STR* dcl, char* buf);

int genTypeName(const DCL_NOM_STR* dcl, char* buf)
{
	char buf2[80];
	char *addr = NULL;
	int i;

	switch(dcl->type->type) {
		case CHAR:
			sprintf(buf2, "c_char");
			break;
		case INT:
			if (dcl->type->flags & SHORT_INT) {
				if (dcl->type->flags & UNSIGNED_TYPE) 
					sprintf(buf2, "c_ushort");
				else 
					sprintf(buf2, "c_short");
			} else if (dcl->type->flags & LONG_INT) {
				if (dcl->type->flags & UNSIGNED_TYPE) 
					sprintf(buf2, "c_ulong");
				else 
					sprintf(buf2, "c_long");
			} else if (dcl->type->flags & LONG_LONG_INT) {
				if (dcl->type->flags & UNSIGNED_TYPE) 
					sprintf(buf2, "c_ulonglong");
				else 
					sprintf(buf2, "c_longlong");
			} else {
				if (dcl->type->flags & UNSIGNED_TYPE) 
					sprintf(buf2, "c_uint");
				else 
					sprintf(buf2, "c_int");
			}
			break;
		case FLOAT:
			sprintf(buf2, "c_float");
			break;
		case DOUBLE:
			sprintf(buf2, "c_double");
			break;
		case ENUM:
			sprintf(buf2, "c_uint");
			break;

		default:
			sprintf(buf2, "%s", dcl->type->name);
	}

	if (IS_ARRAY(dcl) || IS_STRING(dcl)) {
		for (i = 0; i < dcl->ndimensions; i++) {
			bufcat(&addr, " * %d", dcl->dimensions[i]);
		}
		sprintf(buf, "%s %s", buf2, addr);
		free(addr);
	} else {
		sprintf(buf, "%s", buf2);
	}

	return 0;
}

int genPythonInterface(FILE *out)
{
    TYPE_LIST *l;
	int i, j, k, n = 0, p;
    TYPE_STR *t;
    DCL_NOM_LIST *m, *ltypedefs;
    char buf[80];
    char *str;
    ID_LIST *ln;

    script_open(out);
    cat_begin(out);

    fprintf(out, "#------------------  Fichier généré automatiquement ------------------\n");
    fprintf(out, "#------------------  Ne pas éditer manuellement !!! ------------------\n\n");
    fprintf(out, "from ctypes import *\n\n");
    for (ln = imports; ln != NULL; ln = ln->next) {
      fprintf(out, "from %s.struct import *\n", ln->name);
    } 

	/* Reverse the list */
    for (l = types; l != NULL; l = l->next) {
		n++;
	}

	TYPE_LIST** reversed = malloc(n * sizeof(TYPE_LIST*));
	if (reversed == NULL)
		return -1;

    for (i = n-1, l = types; l != NULL; l = l->next, i--) {
		reversed[i] = l;
	}

    for (i = 0; i < n; i++) {
		l = reversed[i];
		t = l->type;
		if (/* t->used == 0 || */ (t->flags & TYPE_IMPORT)) {
			continue;
		}

		/* Corps de la fonction */
		switch (t->type) {

			/* Types de base: normalement on ne passe pas la... */
			case CHAR:
			case SHORT:
			case INT:
			case FLOAT:
			case DOUBLE:
				break;

				/* Structure: on traite chaque membre */
			case STRUCT:
				fprintf(out, "class %s(Structure):\n", t->name);
				fprintf(out, "    _fields_ = [\n"); 
				for (m = t->members; m != NULL; m = m->next) {
					genTypeName(m->dcl_nom, buf);
					fprintf(out, "            (\"%s\", %s),\n", m->dcl_nom->name, buf);
				}
				fprintf(out, "        ]\n\n");
				break;

			case UNION:
				fprintf(out, "class %s(Union):\n", t->name);
				fprintf(out, "    _fields_ = [\n"); 
				for (m = t->members; m != NULL; m = m->next) {
					genTypeName(m->dcl_nom, buf);
					fprintf(out, "            (\"%s\", %s),\n", m->dcl_nom->name, buf);
				}
				break;

			case ENUM:
				fprintf(out, "# enum value for %s\n",  t->name);
				fprintf(out, "(");
				p = 0;
				for (m = t->members; m != NULL; m = m->next) {
					fprintf(out, "%s, ", m->dcl_nom->name);
					p++;
				}
				fprintf(out, ") = (");
				for (k = 0; k < p; k++)
					fprintf(out, "%d, ", k);
				fprintf(out, ")\n\n");
				break;

			case TYPEDEF:
				break;
		}
    }

	free(reversed);
    cat_end(out);
    script_close(out, "server/struct.py");
	return 0;
}
