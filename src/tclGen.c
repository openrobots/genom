/*	$LAAS$ */

/* 
 * Copyright (c) 1999-2003 LAAS/CNRS
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
#include <ctype.h>

#include "genom.h"
#include "parser.tab.h"
#include "tclGen.h"

static int tclArgNumber(DCL_NOM_STR *nom);
static int tclParseInputGen(char **, char *, DCL_NOM_STR *, int);
static int tclOutputGen(char **, char *, DCL_NOM_STR *, int, int);
static int tclGenEnumList(char **, TYPE_STR *);
static int tclGenArgList(char **, int);
static int tclCreateCommandsGen(int create, char **buf, RQST_LIST *rqsts,
			POSTER_LIST *posters);


int
tclGen(FILE *out)
{
   int maxRqstNameLen = 0;
   int n, i;
   char *buf = NULL, *type = NULL, *var = NULL;
   char *addrstr=NULL;
   RQST_LIST *rqst;
   POSTER_LIST *poster;
   int nDim, *dims;
   DCL_NOM_STR *dcln;

#if 0
   /* proto */
   script_open(out);
   cat_begin(out);

   bufcat(&buf, module->name);
   buf[0] = toupper(buf[0]);
   for(n=1;n<strlen(buf);n++) buf[n] = tolower(buf[n]);
   fprintf (out, "extern int %s_Init(Tcl_Interp *interp);\n", buf);
   if (buf != NULL) free(buf); buf = NULL;

   cat_end(out);
   script_close(out,  "server/tcl/%sTclProto.h", module->name);
#endif

   /* func */
   script_open(out);
   subst_begin(out, PROTO_TCL_PROLOG);
   print_sed_subst(out, "module", module->name);
   print_sed_subst(out, "MODULE", module->NAME);
   subst_end(out);

   /* requêtes */

   for(rqst = requetes; rqst != NULL; rqst=rqst->next) {
      if (strlen(rqst->name) > maxRqstNameLen)
	 maxRqstNameLen = strlen(rqst->name);

      subst_begin(out, PROTO_TCL_RQST);

      print_sed_subst(out, "module", module->name);
      print_sed_subst(out, "requestName", rqst->name);
      print_sed_subst(out, "requestNum", "%s_%s_RQST", 
		      module->NAME, rqst->rqst->NAME);

      /* input ------------------------------------------------------- */

      if (rqst->rqst->input == NULL) {
	 print_sed_subst(out, "inputType", "/* no input */");
	 print_sed_subst(out, "inputVar;", "");
	 print_sed_subst(out, "&inputVar", "NULL");
	 print_sed_subst(out, "inputSize", "0");
	 print_sed_subst(out, "&inputVar,", "NULL,");
	 print_sed_subst(out, "argNumber", "0");
	 print_sed_subst(out, "argList", "");
	 print_sed_subst(out, "parseInput", "");
      } else {
	 dcl_nom_decl(rqst->rqst->input->dcl_nom, &type, &var);

	 if (type == NULL || var == NULL) {
	    /* should not happen */
	    fprintf(stderr, "No input type for %s", rqst->name);
	    return -1;
	 }
      
	 print_sed_subst(out, "inputType", "static %s", type);
	 print_sed_subst(out, "inputVar;", "%s;", var);
	 print_sed_subst(out, "&inputVar", "&%s", var);

	 if (IS_ARRAY(rqst->rqst->input->dcl_nom) || 
	     IS_STRING(rqst->rqst->input->dcl_nom)) {
	    char *bracket = strchr(var, '[');
	    if (bracket == NULL) {
	       print_sed_subst(out, "&inputVar,", "%s,", var);
	       print_sed_subst(out, "inputSize", "sizeof(%s)", var);

	    } else {
	       *bracket = '\0';
	       print_sed_subst(out, "&inputVar,", "%s,", var);
	       print_sed_subst(out, "inputSize", "sizeof(%s)", var);
	       *bracket = '[';
	    }
	 } else {
	    print_sed_subst(out, "&inputVar,", "&%s,", var);
	    print_sed_subst(out, "inputSize", "sizeof(%s)", var);
	 }

	 n = tclArgNumber(rqst->rqst->input->dcl_nom);
	 if (n < 0) {
	    fprintf(stderr,
		    "Error: could not enumerate input of %s.\n", rqst->name);
	    return -1;
	 }

	 bufcat(&buf, "%d", n);
	 print_sed_subst(out, "argNumber", buf);
	 if (buf != NULL) free(buf); buf = NULL;

	 tclGenArgList(&buf, n);
	 print_sed_subst(out, "argList", buf);
	 if (buf != NULL) free(buf); buf = NULL;

	 tclParseInputGen(&buf, rqst->rqst->input->dcl_nom->name,
			  rqst->rqst->input->dcl_nom, 0);
	 print_sed_subst(out, "parseInput", buf);
	 if (buf != NULL) free(buf); buf = NULL;

	 if (type != NULL) free(type);
	 if (var != NULL) free(var);
      }

      /* output ------------------------------------------------------ */

      if (rqst->rqst->output == NULL) {
	 print_sed_subst(out, "outputType", "/* no output */");
	 print_sed_subst(out, "outputVar;", "");
	 print_sed_subst(out, "&outputVar", "NULL");
	 print_sed_subst(out, "outputSize", "0");
	 print_sed_subst(out, "&outputVar,", "NULL,");
	 print_sed_subst(out, "output", "");

      } else {
	 dcl_nom_decl(rqst->rqst->output->dcl_nom, &type, &var);

	 print_sed_subst(out, "outputType", "static %s", type);
	 print_sed_subst(out, "outputVar;", "%s;", var);
	 print_sed_subst(out, "&outputVar", "&%s", var);

	 if (IS_ARRAY(rqst->rqst->output->dcl_nom) || 
	     IS_STRING(rqst->rqst->output->dcl_nom)) {
	    char *bracket = strchr(var, '[');
	    if (bracket == NULL) {
	       print_sed_subst(out, "&outputVar,", "%s,", var);
	       print_sed_subst(out, "outputSize", "sizeof(%s)", var);

	    } else {
	       *bracket = '\0';
	       print_sed_subst(out, "&outputVar,", "%s,", var);
	       print_sed_subst(out, "outputSize", "sizeof(%s)", var);
	       *bracket = '[';
	    }
	 } else {
	    print_sed_subst(out, "&outputVar,", "&%s,", var);
	    print_sed_subst(out, "outputSize", "sizeof(%s)", var);
	 }

	 tclOutputGen(&buf, rqst->rqst->output->dcl_nom->name,
		      rqst->rqst->output->dcl_nom, 0, 0);
	 print_sed_subst(out, "output", buf);
	 if (buf != NULL) free(buf); buf = NULL;

	 if (type != NULL) free(type);
	 if (var != NULL) free(var);
      }

      subst_end(out);
   }

   /* posters */

   for(poster = posters; poster != NULL; poster=poster->next) {
      DCL_NOM_LIST *memberList;

      if (poster->type == NULL) {
	 fprintf(stderr,
		 "Error: no type for poster %s.\n", poster->name);
	 return -1;
      }

      if (poster->data != NULL) {

#if WHOLEPOSTER 
	/* XXXXX WHOLE POSTER ALSO TO BE DONE HERE */
	subst_begin(out, PROTO_TCL_POSTER);
	
	print_sed_subst(out, "module", module->name);
	print_sed_subst(out, "posterName", poster->name);
	print_sed_subst(out, "posterFullName", poster->name);
	print_sed_subst(out, "posterStructType", poster->type->name);
	print_sed_subst(out, "dataType", poster->type->name);

	tclOutputGen(&buf, "_posterData", poster->type->members->dcl_nom, 0, 1);
	print_sed_subst(out, "output", buf);
	if (buf != NULL) free(buf); buf = NULL;
	
	subst_end(out);
	/* .... */
#endif

	 /* SDI Reference list: generate one function per struct */

	 for(memberList = poster->type->members;
	     memberList != NULL; memberList = memberList->next) {

	    subst_begin(out, PROTO_TCL_POSTER);

	    print_sed_subst(out, "module", module->name);

	    print_sed_subst(out, "posterName", poster->name);
	    bufcat(&buf, "%s%s", poster->name, memberList->dcl_nom->name);
	    print_sed_subst(out, "posterFullName", buf);
	    if (buf != NULL) free(buf); buf = NULL;

	    /* Dimension du tableau */
	    dcln = memberList->dcl_nom;
	    nDim = dcln->ndimensions;
	    dims = dcln->dimensions;

	    print_sed_subst(out, "posterStructType", 
			    nom_type(memberList->dcl_nom->type));

	    /* C'est pas un tableau */
	    if(nDim == 0) {
	      bufcat(&addrstr, nom_type(dcln->type));
	      type = NULL;
	      bufcat(&type, "(*_posterData)");
	    }

	    /* C'est un tableau */
	    else {
	      bufcat(&addrstr, "%s[%d]", nom_type(dcln->type), dims[0]);
	      for (i=1;i<nDim;i++) {
		bufcat(&addrstr, "[%d]", dims[i]);
	      }
	      type = NULL;
	      bufcat(&type, "_posterData");
	    }
	    print_sed_subst(out, "dataType", addrstr);
	    if (addrstr != NULL) free(addrstr); addrstr = NULL;

	    tclOutputGen(&buf, type, memberList->dcl_nom, 0, 1);
	    if (type != NULL) free(type);

	    print_sed_subst(out, "output", buf);
	    if (buf != NULL) free(buf); buf = NULL;

	    subst_end(out);
	 }

      } else {
	 /* User defined type: generate only one function */

	 subst_begin(out, PROTO_TCL_POSTER);

	 print_sed_subst(out, "module", module->name);
	 print_sed_subst(out, "posterName", poster->name);
	 print_sed_subst(out, "posterFullName", poster->name);

	 print_sed_subst(out, "posterStructType", poster->type->name);

	 print_sed_subst(out, "dataType", poster->type->name);
	    
	 for(memberList = poster->type->members;
	     memberList != NULL; memberList = memberList->next) {

	    type = NULL;
	    bufcat(&type, "_posterData->%s", memberList->dcl_nom->name);

	    tclOutputGen(&buf, type, memberList->dcl_nom, 0, 1);

	    if (type != NULL) free(type);
	 }
	 print_sed_subst(out, "output", buf);
	 if (buf != NULL) free(buf); buf = NULL;

	 subst_end(out);
      }
   }

   /* epilogue */

   subst_begin(out, PROTO_TCL_EPILOG);

   print_sed_subst(out, "module", module->name);
   print_sed_subst(out, "MODULE", module->NAME);

   bufcat(&buf, module->name);
   buf[0] = toupper(buf[0]);
   for(n=1;n<strlen(buf);n++) buf[n] = tolower(buf[n]);
   print_sed_subst(out, "Module", buf);
   if (buf != NULL) free(buf); buf = NULL;

   bufcat(&buf, "%d", maxRqstNameLen+50/*XXX*/);
   print_sed_subst(out, "maxCmdLen", buf);
   if (buf != NULL) free(buf); buf = NULL;
   
   tclCreateCommandsGen(1, &buf, requetes, posters);
   print_sed_subst(out, "createCommands", buf);
   if (buf != NULL) free(buf); buf = NULL;

   tclCreateCommandsGen(0, &buf, requetes, posters);
   print_sed_subst(out, "deleteCommands", buf);
   if (buf != NULL) free(buf); buf = NULL;

   subst_end(out);

   script_close(out, "server/tcl/%sTcl.c", module->name);
   return 0;
}

/*
 * Returns number of arguments needed to fill the given type ------------
 */

static int
tclArgNumber(DCL_NOM_STR *nom)
{
   int n = -1; /* -1 is used as error value */

   switch (nom->type->type) {

      case CHAR:
      case SHORT:
      case INT:
      case FLOAT:
      case DOUBLE:
      case ENUM:
	 n = 1;
	 break;

      case STRUCT:
      case UNION:
      {
	 int p;
	 DCL_NOM_LIST *m;

	 n = 0;
	 for (m = nom->type->members; m != NULL; m = m->next) {
	    p = tclArgNumber(m->dcl_nom);
	    if (p < 0) {
	       fprintf(stderr,
		       "Error: struct member %s was problematic.\n", m->dcl_nom->name);
	       n = -1;
	       break;
	    } else
	       n += p;
	 }
	 break;
      }

      case TYPEDEF:
	 fprintf(stderr, "Warning: encountered a typedef for %s.\n", nom->name);
	 break;
   }

   if (IS_ARRAY(nom)) {
      int i;

      for(i=0; i<nom->ndimensions-(IS_STRING(nom)?1:0); i++)
	 n *= nom->dimensions[i];
   }

   return n;
}


/*
 * Returns list of tcl function calls to scan a type --------------------
 */

static char *indentQuantum = "   "; /* width of indent tabs */

static int
tclParseInputGen(char **buf, char *name, DCL_NOM_STR *nom, int level)
{
   char *arrayName = NULL;	/* in the case of an array */
   char *indent = NULL;		/* useless, but generates nicer code... */

   /* indentation is at least one quantum */
   bufcat(&indent, "%s", indentQuantum);

   if (IS_POINTER(nom)) {
      fprintf(stderr, "Pointer %s does not make sense here.\n", name);
      return -1;
   }

   /* For an array, must do some loop */
   if (IS_ARRAY(nom)) {
      int i;

      bufcat(buf, "%s{\n", indent);
      bufcat(&indent, indentQuantum);
      bufcat(buf,"%sint loop%d[%d];\n",
	     indent, level, nom->ndimensions-(IS_STRING(nom)?1:0));

      bufcat(&arrayName, name);

      for(i=0;i<nom->ndimensions-IS_STRING(nom)?1:0;i++) {
	 bufcat(buf,"%sfor(loop%d[%d]=0; loop%d[%d]<%d; loop%d[%d]++) {\n",
		indent, level, i, level, i, nom->dimensions[i], level, i);
	 bufcat(&indent, indentQuantum);

	 bufcat(&arrayName, "[loop%d[%d]]", level, i);
      }

      if (arrayName == NULL) {
	 /* must give up */
	 fprintf(stderr, "Warning: could not generate array name for %s.\n",
		 name);
	 return -1;
      }

      name = arrayName;
   }

   if (nom->type->type != STRUCT && nom->type->type != UNION) {

      /* Echo the tcl func call */
      switch (nom->type->type) {

	 case CHAR:
	    if (IS_STRING(nom))
	       bufcat(buf,
		      "%sstrncpy(%s, \n"
		      "%s\tTcl_GetStringFromObj(objv[++curObjc], NULL), "
		      "%s\t%d);\n"
		      "%sret = TCL_OK;\n",
		      indent, name,
		      indent,
		      indent, nom->dimensions[nom->ndimensions-1],
		      indent,
		      indent);
	    else
	       bufcat(buf,
		      "%s{\n"
		      "%s%sint tmpInt;\n"
		      "%s%sret = Tcl_GetIntFromObj(interp, objv[++curObjc], "
		      "%s%s\t&tmpInt);\n"
		      "%s%s%s = (char)tmpInt;\n"
		      "%s}\n",
		      indent,
		      indent, indentQuantum,
		      indent, indentQuantum,
		      indent, indentQuantum,
		      indent, indentQuantum, name,
		      indent);
	    break;
	 
	 case SHORT: /* XXX SHORT is never used as a type, is it? */
	 case LONG:  /* XXX idem */
	 case INT:
	    if (nom->type->flags & SHORT_INT) {
	       bufcat(buf,
		      "%s{\n"
		      "%s%sint tmpInt;\n"
		      "%s%sret = Tcl_GetIntFromObj(interp, objv[++curObjc], "
		      "%s%s\t&tmpInt);\n"
		      "%s%s%s = (short)tmpInt;\n"
		      "%s}\n",
		      indent,
		      indent, indentQuantum,
		      indent, indentQuantum,
		      indent, indentQuantum,
		      indent, indentQuantum, name,
		      indent);
	    } else if (nom->type->flags & LONG_INT) {
	       bufcat(buf,
		      "%sret = Tcl_GetLongFromObj(interp, objv[++curObjc], "
		      "%s\t&%s);\n",
		      indent, indent, name);
	    } else if (nom->type->flags & LONG_LONG_INT) {
	       bufcat(buf,
		      "%sret = Tcl_GetWideIntFromObj(interp, objv[++curObjc], "
		      "%s\t&%s);\n",
		      indent, indent, name);
	    } else {
	       bufcat(buf,
		      "%sret = Tcl_GetIntFromObj(interp, objv[++curObjc], "
		      "%s\t&%s);\n",
		      indent, indent, name);
	    }
	    break;
		
	 case FLOAT:
	    bufcat(buf,
		   "%s{\n"
		   "%s%sdouble tmpDouble;\n"
		   "%s%sret = Tcl_GetDoubleFromObj(interp, objv[++curObjc], "
		   "%s%s\t&tmpDouble);\n"
		   "%s%s%s = (float)tmpDouble;\n"
		   "%s}\n",
		   indent,
		   indent, indentQuantum,
		   indent, indentQuantum,
		   indent, indentQuantum,
		   indent, indentQuantum, name,
		   indent);
	    break;

	 case DOUBLE:
	    bufcat(buf,
		   "%sret = Tcl_GetDoubleFromObj(interp, objv[++curObjc], "
		   "%s\t&%s);\n",
		   indent, indent, name);
	    break;
	 
	 case STRUCT:
	 case UNION:
	    /* see 'else' statement below */
	    break;

	    /* Affichage en clair des symboles de l'enum */
	 case ENUM:
	    bufcat(buf,
		   "%sret = Tcl_GetEnumFromObj(interp, objv[++curObjc], "
		   "%s\t(int *)&%s,",
		   indent, indent, name);
	    if (tclGenEnumList(buf, nom->type) != 0)
	       return -1;
	    bufcat(buf, ", NULL);\n");
	    break;

	 case TYPEDEF:
	    fprintf(stderr, "Warning: encountered a typedef for %s.\n", name);
	    break;
      }

      /* Check the result of the tcl func */
      bufcat(buf, "%sif (ret != TCL_OK) return TCL_ERROR;\n", indent);

   } else { /* STRUCT || UNION */
      /* recurse */
      DCL_NOM_LIST *m;
      char *var;

      for (m = nom->type->members; m != NULL; m = m->next) {
	 var = NULL;

	 bufcat(&var, "%s.%s", name, m->dcl_nom->name);
	 if (var == NULL) { 
	    fprintf(stderr, "Warning: null member name in %s.\n", name);
	    continue;
	 }

	 if (tclParseInputGen(buf, var, m->dcl_nom, level+1))
	    fprintf(stderr, "Warning: struct member %s was problematic.\n",
		    var);
	 free(var);
      }
   }
	  

   if (IS_ARRAY(nom)) {
      int i;

      /* End array scanning */
      for(i=IS_STRING(nom)?0:-1;i<nom->ndimensions;i++) {
	 indent[strlen(indent)-3] = '\0';
	 bufcat(buf, "%s}\n", indent);
      }
      free(arrayName);
   }

   if (indent != NULL) free(indent);

   return 0;
}

/*
 * Returns list of tcl function calls to output a type as a Tcl_Obj -----
 */

static int
tclOutputGen(char **buf, char *name, DCL_NOM_STR *nom, int level, 
	     int allocFlag)
{
   char *arrayName = NULL;	/* in the case of an array */
   char *indent = NULL;		/* useless, but generates nicer code... */

   /* indentation is at least one quantum */
   bufcat(&indent, "%s", indentQuantum);

   if (IS_POINTER(nom)) {
      fprintf(stderr, "Pointer %s does not make sense here.\n", name);
      return -1;
   }

   /* For an array, must do some loop */
   if (IS_ARRAY(nom)) {
      int i;

      bufcat(buf, "%s{\n", indent);
      bufcat(&indent, indentQuantum);
      bufcat(buf,"%sint loop%d[%d];\n",
	     indent, level, nom->ndimensions-(IS_STRING(nom)?1:0));

      bufcat(&arrayName, name);

      for(i=0;i<nom->ndimensions-IS_STRING(nom)?1:0;i++) {
	 bufcat(buf,"%sfor(loop%d[%d]=0; loop%d[%d]<%d; loop%d[%d]++) {\n",
		indent, level, i, level, i, nom->dimensions[i], level, i);
	 bufcat(&indent, indentQuantum);

	 bufcat(&arrayName, "[loop%d[%d]]", level, i);
      }

      if (arrayName == NULL) {
	 /* must give up */
	 fprintf(stderr, "Warning: could not generate array name for %s.\n",
		 name);
	 return -1;
      }

      name = arrayName;
   }

   if (nom->type->type != STRUCT && nom->type->type != UNION) {

      bufcat(buf,
	     "%sret = Tcl_ListObjAppendElement(interp, my_own_private_unique_result,\n",
	     indent);

      /* Echo the tcl func call */
      switch (nom->type->type) {

	 case CHAR:
	    if (IS_STRING(nom)) {
	       bufcat(buf, "\tTcl_NewStringObj(%s, -1));\n", name);
	       break;
	    }
	    /* fall through */
	 case SHORT:
	 case INT:
	    if (nom->type->flags & LONG_LONG_INT)
	       bufcat(buf, "\tTcl_NewWideIntObj(%s));\n", name);
	    else
	       bufcat(buf, "\tTcl_NewIntObj(%s));\n", name);
	    break;
		
	 case FLOAT:
	 case DOUBLE:
	    bufcat(buf, "\tTcl_NewDoubleObj(%s));\n", name);
	    break;
	 
	 case ENUM:
	    bufcat(buf, "\tTcl_NewEnumObj(interp, (int)%s,\n\t", name);
	    if (tclGenEnumList(buf, nom->type) != 0)
	       return -1;
	    bufcat(buf, ", NULL));\n");
	    break;

	 case STRUCT:
	 case UNION:
	    /* see 'else' statement below */
	    break;

	 case TYPEDEF:
	    fprintf(stderr,
		    "Warning: encountered a typedef for %s.\n", name);
	    break;
      }

      /* Check the result of the tcl func */
      if (allocFlag)
	bufcat(buf, "%sif (ret != TCL_OK) {free(_posterData); return TCL_ERROR;}\n", indent);
      else
	bufcat(buf, "%sif (ret != TCL_OK) return TCL_ERROR;\n", indent);

   } else { /* STRUCT || UNION */
      /* recurse */
      DCL_NOM_LIST *m;
      char *var;

      for (m = nom->type->members; m != NULL; m = m->next) {
	 var = NULL;

/* 	 if (strcmp(name,"_posterData")==0) */
/* 	   bufcat(&var, "%s->%s", name, m->dcl_nom->name); */
/* 	 else */
	   bufcat(&var, "%s.%s", name, m->dcl_nom->name);
	 if (var == NULL) { 
	    fprintf(stderr, "Warning: null member name in %s.\n", name);
	    continue;
	 }

	 if (tclOutputGen(buf, var, m->dcl_nom, level+1, allocFlag))
	    fprintf(stderr,
		    "Warning: struct member %s was problematic.\n", var);

	 free(var);
      }
   }
	  

   if (IS_ARRAY(nom)) {
      int i;

      /* End array scanning */
      for(i=IS_STRING(nom)?0:-1;i<nom->ndimensions;i++) {
	 indent[strlen(indent)-3] = '\0';
	 bufcat(buf, "%s}\n", indent);
      }
      free(arrayName);
   }

   if (indent != NULL) free(indent);

   return 0;
}

/*
 * Generate a list of « "enumText", enumValue » -------------------------
 */

static int
tclGenEnumList(char **buf, TYPE_STR *type)
{
   DCL_NOM_LIST *member;

   if (type->type != ENUM) return -1;

   for(member = type->members; member != NULL; member = member->next) {
      bufcat(buf, "\"%s\", %d",
	     member->dcl_nom->name, member->dcl_nom->pointeur);
      if (member->next != NULL)
	 bufcat(buf, ", ");
   }

   return 0;
}

/*
 * Generates 'argNumber' « [ lindex $args i ] » -------------------------
 */

static int
tclGenArgList(char **buf, int argNumber)
{
   int i;
   for (i=0;i<argNumber;i++) bufcat(buf, "[lindex \\$args %d] ", i);
   return 0;
}

/*
 * Returns list of tcl calls to create commands  ------------------------
 */

static int
tclCreateCommandsGen(int create, char **buf, RQST_LIST *rqsts,
		     POSTER_LIST *posters)
{
   RQST_LIST *rqst;
   POSTER_LIST *poster;

   for(rqst = rqsts; rqst != NULL; rqst = rqst->next)
      if (create) {
	 bufcat(buf,
		"%s%sstrcpy(name+offset, \"%sSend\");\n"
		"%s%sTcl_CreateObjCommand(interp, name, "
		"%s%sRqstSendCb, m, NULL);\n"
		"%s%sstrcpy(name+offset, \"%sRcv\");\n"
		"%s%sTcl_CreateObjCommand(interp, name, "
		"%s%sReplyRcvCb, m, NULL);\n",
		indentQuantum, indentQuantum, rqst->rqst->name,
		indentQuantum, indentQuantum,
		module->name, rqst->rqst->name,
		indentQuantum, indentQuantum, rqst->rqst->name,
		indentQuantum, indentQuantum,
		module->name, rqst->rqst->name);

      } else {
	 bufcat(buf,
		"%s%sstrcpy(name+offset, \"%sSend\");\n"
		"%s%sTcl_DeleteCommand(interp, name);\n"
		"%s%sstrcpy(name+offset, \"%sRcv\");\n"
		"%s%sTcl_DeleteCommand(interp, name);\n",
		indentQuantum, indentQuantum, rqst->rqst->name,
		indentQuantum, indentQuantum,
		indentQuantum, indentQuantum, rqst->rqst->name,
		indentQuantum, indentQuantum);
      }

   for(poster = posters; poster != NULL; poster = poster->next)
      if (poster->data != NULL) {
	 /* SDI Reference list: generate one function per struct */
	 DCL_NOM_LIST *memberList;

	 for(memberList = poster->type->members;
	     memberList != NULL; memberList = memberList->next) {

	    if (create) {
	       bufcat(buf,
		      "%s%sstrcpy(name+offset, \"%s%sPosterRead\");\n"
		      "%s%sTcl_CreateObjCommand(interp, name, "
		      "%s%s%sPosterTclRead, m, NULL);\n",
		      indentQuantum, indentQuantum,
		      poster->name, memberList->dcl_nom->name,
		      indentQuantum, indentQuantum,
		      module->name, poster->name, memberList->dcl_nom->name);

	    } else {
	       bufcat(buf,
		      "%s%sstrcpy(name+offset, \"%s%sPosterRead\");\n"
		      "%s%sTcl_DeleteCommand(interp, name);\n",
		      indentQuantum, indentQuantum,
		      poster->name, memberList->dcl_nom->name,
		      indentQuantum, indentQuantum);
	    }
	 }

      } else {
	 /* User defined type: generate only one function */
	 if (create) {
	    bufcat(buf,
		   "%s%sstrcpy(name+offset, \"%sPosterRead\");\n"
		   "%s%sTcl_CreateObjCommand(interp, name, "
		   "%s%sPosterTclRead, m, NULL);\n",
		   indentQuantum, indentQuantum, poster->name,
		   indentQuantum, indentQuantum,
		   module->name, poster->name);

	 } else {
	    bufcat(buf,
		   "%s%sstrcpy(name+offset, \"%sPosterRead\");\n"
		   "%s%sTcl_DeleteCommand(interp, name);\n",
		   indentQuantum, indentQuantum, poster->name,
		   indentQuantum, indentQuantum);
	 }
      }
   return 0;
}
