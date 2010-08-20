
/* 
 * Copyright (c) 1993-2003 LAAS/CNRS
 * Matthieu Herrb - Thu Jul 15 1993
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
#include <stdarg.h>
#include <stdlib.h>

#include "genom.h"
#include "parser.tab.h"
#include "tclservClientMsgLibGen.h"

static void msgLibSubstRqst(FILE *out, RQST_STR *r);

int genTclservClientMsgLib(FILE *out)
{
    RQST_LIST *l;
    RQST_STR *r;
    char *rqstList = NULL;
    int abortNum;

    /*
     * MsgLib.c
     */
    script_open(out);

    subst_begin(out, PROTO_TCLSERV_CLIENT_MSG_TITLE);
    
    /* nom du module */
    print_sed_subst(out, "module", module->name);
    print_sed_subst(out, "MODULE", module->NAME);

    /* numero de la requete d'abort */
    abortNum = trouve_abort_num();
    print_sed_subst(out, "abortRequestNum", "%s_ABORT_RQST", module->NAME);

    subst_end(out);

    /* Every request */
    for (l = requetes; l != NULL; l=l->next) {
        r=l->rqst;
	subst_begin(out,r->type == CONTROL ? 
			PROTO_TCLSERV_CLIENT_MSG_CTRL : 
			PROTO_TCLSERV_CLIENT_MSG_EXEC);
	msgLibSubstRqst(out, l->rqst);
	subst_end(out);
    } /* for */

    script_close(out, "server/tclservClient/%sTclservClientMsgLib.c", module->name);

    /* 
     * MsgLib.h
     */
    script_open(out);
    subst_begin(out, PROTO_TCLSERV_CLIENT_MSG_HEADER);

    /* Nom du Module */
    print_sed_subst(out, "module", module->name);
    print_sed_subst(out, "MODULE", module->NAME);

    subst_end(out);

    /* Prototypes */
    /* Every request */
    for (l = requetes; l != NULL; l=l->next) {
        r=l->rqst;
	subst_begin(out,r->type == CONTROL ? 
			PROTO_TCLSERV_CLIENT_MSG_CTRL_PROTO :
			PROTO_TCLSERV_CLIENT_MSG_CTRL_EXEC);
	msgLibSubstRqst(out, l->rqst);
	subst_end(out);
    } /* for */

    cat_begin(out);
    fprintf(out, "#ifdef __cplusplus\n");
    fprintf(out, "}\n");
    fprintf(out, "#endif\n");
    fprintf(out, "#endif /* %sTCLSERV_CLIENT_MSG_LIB_H */\n", module->name);
    cat_end(out);

    script_close(out, "server/tclservClient/%sTclservClientMsgLib.h", module->name);


    return(0);
}


/* 
 *  msgLibSubstRqst - all request data substitution
 *
 */
static void msgLibSubstRqst(FILE *out, RQST_STR *r)
{
  char *type, *var, *size, *type1;

  print_sed_subst(out, "module", module->name);
  print_sed_subst(out, "request", r->name);
  if (r->input != NULL) {
    dcl_nom_decl(r->input->dcl_nom, &type, &var);
    size = genSizeof(r->input->dcl_nom);
    print_sed_subst(out, "input", "%s %s(in_%s)", type, 
		    r->input->dcl_nom->ndimensions == 0 
		    ? "*" : "", var);
    print_sed_subst(out, "inputSize", size, 
		    r->input->dcl_nom->name);
	print_sed_subst(out, "hasInput", "");
	type1 = nom_type1(r->input->dcl_nom->type);
	print_sed_subst(out, "type_input", type1);
	free(type1);
    free(type);
    free(var);
    free(size);
    print_sed_subst(out, "inputName", "in_%s", r->input->dcl_nom->name);
  } else {
    print_sed_subst(out, "input", "");
	print_sed_subst(out, "type_input", "");
	print_sed_subst(out, "hasInput", "//");
    print_sed_subst(out, "inputSize", "0");
    print_sed_subst(out, "inputName", "NULL");
  }
  if (r->output != NULL) {
    dcl_nom_decl(r->output->dcl_nom, &type, &var);
    size = genSizeof(r->output->dcl_nom);
    print_sed_subst(out, "output", "%s %s(out_%s),", type,
		    r->output->dcl_nom->ndimensions == 0
		    ? "*" : "", var);
    print_sed_subst(out, "outputSize", size, 
		    r->output->dcl_nom->name);
	type1 = nom_type1(r->output->dcl_nom->type);
	print_sed_subst(out, "output_type", type1);
	print_sed_subst(out, "has_output", "");
	free(type1);
    free(type);
    free(var);
    free(size);
    print_sed_subst(out, "outputName", "out_%s", r->output->dcl_nom->name);
  } else {
    print_sed_subst(out, "output", "");
	print_sed_subst(out, "output_type", "");
	print_sed_subst(out, "has_output", "//");
    print_sed_subst(out, "outputSize", "0");
    print_sed_subst(out, "outputName", "NULL");
  }
}
