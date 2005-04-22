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
#include <stdlib.h>
#include <string.h>

#include "genom.h"
#include "parser.tab.h"
#include "tclClientGen.h"

static int tclInputFormatGen(char **buf, char *name, DCL_NOM_STR *nom,
			     RQST_INPUT_INFO_LIST **iinfo, int level);
static int tclOutputFormatGen(char **buf, char *name, DCL_NOM_STR *nom);
static int tclArgNumberGen(DCL_NOM_STR *nom);
static int tclUsageGen(char **buf, DCL_NOM_STR *nom, char *prefix);
static int tclGenEnumList(char **buf, TYPE_STR *type);
static char *tclQuoteString(char *string);

int
tclClientGen(FILE *out)
{
   int n;
   char *buf = NULL, *type = NULL, *var = NULL, *tmp;
   RQST_LIST *rqst;
   POSTER_LIST *poster;
   RQST_INPUT_INFO_LIST *iinfo;

   /* Client part */
   script_open(out);
   subst_begin(out, PROTO_TCLCLIENT);
   print_sed_subst(out, "module", module->name);

   for(rqst = requetes; rqst != NULL; rqst=rqst->next) {
      bufcat(&buf, "\"%s\" ", rqst->name);
   }
   for(poster = posters; poster != NULL; poster=poster->next) {
      if (poster->data != NULL) {
	 /* SDI Reference list: generate one function per struct */
	 DCL_NOM_LIST *memberList;

	 for(memberList = poster->type->members;
	     memberList != NULL; memberList = memberList->next) {

	    bufcat(&buf, "\"%s%sPosterRead\" ",
		   poster->name, memberList->dcl_nom->name);
	 }
      } else {
	 /* User defined type: generate only one function */

	 bufcat(&buf, "\"%sPosterRead\" ", poster->name);
      }
   }
   print_sed_subst(out, "rqstList", buf);
   if (buf != NULL) free(buf); buf = NULL;

   subst_end(out);

   /* requêtes */

   for(rqst = requetes; rqst != NULL; rqst=rqst->next) {
      subst_begin(out, PROTO_TCLCLIENTRQST);

      print_sed_subst(out, "module", module->name);
      print_sed_subst(out, "requestName", rqst->name);
      if (rqst->rqst->doc == NULL)
	 print_sed_subst(out, "requestDoc", "** undocumented **");
      else {
	 tmp = tclQuoteString(rqst->rqst->doc);
	 print_sed_subst(out, "requestDoc", tmp);
	 free(tmp);
      }

      /* input ------------------------------------------------------- */

      if (rqst->rqst->input == NULL) {
	 print_sed_subst(out, "inputFormat", "");
	 print_sed_subst(out, "requestUsage", "no arguments");
	 print_sed_subst(out, "argNumber", "0");
      } else {
	 dcl_nom_decl(rqst->rqst->input->dcl_nom, &type, &var);

	 if (type == NULL || var == NULL) {
	    /* should not happen */
	    fprintf(stderr, "No input type for %s", rqst->name);
	    return -1;
	 }
      
	 iinfo = rqst->rqst->input_info;
	 tclInputFormatGen(&buf, rqst->rqst->input->dcl_nom->name,
			   rqst->rqst->input->dcl_nom, &iinfo, 0);
/* 	 tmp = tclQuoteString(buf); */
	 print_sed_subst(out, "inputFormat", buf);
/* 	 free(tmp); */
	 if (buf != NULL) free(buf); buf = NULL;

	 n = tclArgNumberGen(rqst->rqst->input->dcl_nom);
	 if (n < 0) {
	    fprintf(stderr,
		    "Error: could not enumerate input of %s.\n", rqst->name);
	    return -1;
	 }
	 print_sed_subst(out, "argNumber", "%d", n);

	 tclUsageGen(&buf, rqst->rqst->input->dcl_nom, NULL);
	 tmp = tclQuoteString(buf);
	 print_sed_subst(out, "requestUsage", tmp);
	 free(tmp);
	 if (buf != NULL) free(buf); buf = NULL;

	 if (type != NULL) free(type);
	 if (var != NULL) free(var);
      }

      /* output ------------------------------------------------------ */

      if (rqst->rqst->output == NULL) {
	 print_sed_subst(out, "outputFormat", "{} status ");
      } else {
	 if (rqst->rqst->output->dcl_nom->name == NULL) {
	    /* should not happen */
	    fprintf(stderr, "No output name for %s", rqst->name);
	    return -1;
	 }
	 
	 bufcat(&buf, "{} status ");
	 tclOutputFormatGen(&buf, rqst->rqst->output->dcl_nom->name,
			    rqst->rqst->output->dcl_nom);
	 tmp = tclQuoteString(buf);
	 print_sed_subst(out, "outputFormat", tmp);
	 free(tmp);
	 if (buf != NULL) free(buf); buf = NULL;
      }

      subst_end(out);
   }

   /* posters */

   for(poster = posters; poster != NULL; poster=poster->next) {
      DCL_NOM_LIST *memberList;

      if (poster->data != NULL) {
	 /* SDI Reference list: generate one function per struct */

	 for(memberList = poster->type->members;
	     memberList != NULL; memberList = memberList->next) {

	    subst_begin(out, PROTO_TCLCLIENTPOSTER);

	    print_sed_subst(out, "module", module->name);
	    bufcat(&buf, "%s%s", poster->name, memberList->dcl_nom->name);
	    print_sed_subst(out, "posterName", buf);
	    if (buf != NULL) free(buf); buf = NULL;

	    bufcat(&buf, "{} status ");

	    tclOutputFormatGen(&buf, memberList->dcl_nom->name,
			       memberList->dcl_nom);
	    tmp = tclQuoteString(buf);
	    print_sed_subst(out, "outputFormat", tmp);
	    free(tmp);
	    if (buf != NULL) free(buf); buf = NULL;
	    
	    tclUsageGen(&buf, memberList->dcl_nom, NULL);
	    tmp = tclQuoteString(buf);
	    print_sed_subst(out, "posterDoc", tmp);
	    free(tmp);
	    if (buf != NULL) free(buf); buf = NULL;
	    
	    subst_end(out);
	 }
      } else {
	 /* User defined type: generate only one function */

	 subst_begin(out, PROTO_TCLCLIENTPOSTER);

	 print_sed_subst(out, "module", module->name);
	 print_sed_subst(out, "posterName", poster->name);

	 bufcat(&buf, "{} status ");

	 for(memberList = poster->type->members;
	     memberList != NULL; memberList = memberList->next) {

	    tclOutputFormatGen(&buf, memberList->dcl_nom->name,
			       memberList->dcl_nom);
	 }
	 tmp = tclQuoteString(buf);
	 print_sed_subst(out, "outputFormat", tmp);
	 free(tmp);
	 if (buf != NULL) free(buf); buf = NULL;
	 
	 for(memberList = poster->type->members;
	     memberList != NULL; memberList = memberList->next) {

	    tclUsageGen(&buf, memberList->dcl_nom, NULL);
	 }
	 tmp = tclQuoteString(buf);
	 print_sed_subst(out, "posterDoc", tmp);
	 free(tmp);
	 if (buf != NULL) free(buf); buf = NULL;

	 subst_end(out);
      }
   }

   script_close(out, "server/tcl/%sClient.tcl", module->name);
   return 0;
}


/*
 * Returns format of input struct for tcl -------------------------------
 */

static int
tclInputFormatGen(char **buf, char *name, DCL_NOM_STR *nom,
		  RQST_INPUT_INFO_LIST **iinfo, int level)
{
   char *arrayName = NULL;	/* in the case of an array */
   static char defaultDefault[20];

   if (IS_POINTER(nom)) {
      fprintf(stderr, "Pointer %s does not make sense here.\n", name);
      return -1;
   }

   /* For an array, must do some loop */
   if (IS_ARRAY(nom)) {
      int i;

      bufcat(&arrayName, name);

      for(i=0;i<nom->ndimensions-IS_STRING(nom)?1:0;i++) {
	 bufcat(buf,"    for {set loop%d(%d) 0} "
		"{ \\$loop%d(%d)<%d } { incr loop%d(%d)} {\n",
		level, i, level, i, nom->dimensions[i], level, i);
	 bufcat(&arrayName, "\\\\[\\$loop%d(%d)\\\\]", level, i);
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

      bufcat(buf, "    lappend format [list ");

      /* Echo the tcl func call */
      switch (nom->type->type) {

	 case CHAR:
	    if (IS_STRING(nom))
	       bufcat(buf, "string");
	    else
	       bufcat(buf, "short");
	    strcpy(defaultDefault, "\"\"");
	    break;
	 
	 case SHORT:
	    bufcat(buf, "short");
	    strcpy(defaultDefault, "0");
	    break;

	 case INT:
	    bufcat(buf, "int");
	    strcpy(defaultDefault, "0");
	    break;
		
	 case FLOAT:
	    bufcat(buf, "float");
	    strcpy(defaultDefault, "0.0");
	    break;

	 case DOUBLE:
	    bufcat(buf, "double");
	    strcpy(defaultDefault, "0.0");
	    break;
	 
	 case STRUCT:
	 case UNION:
	    /* see 'else' statement below */
	    break;

	    /* Affichage en clair des symboles de l'enum */
	 case ENUM:
	    bufcat(buf, "{ ");
	    if (tclGenEnumList(buf, nom->type) != 0)
	       return -1;
	    bufcat(buf, "}");
	    strcpy(defaultDefault, "\"\"");
	    break;

	 case TYPEDEF:
	    fprintf(stderr, "Warning: encountered a typedef for %s.\n", name);
	    break;
      }

      if (*iinfo != NULL && (*iinfo)->doc != NULL) {
	 bufcat(buf, " \"%s\"", (*iinfo)->doc);
	 switch((*iinfo)->type) {
	    case INT:
	       if (nom->type->type == ENUM) {
		  int ok = 0;
		  DCL_NOM_LIST *member;

		  for(member = nom->type->members;
		      member != NULL;
		      member = member->next) {
		     if (member->dcl_nom->pointeur ==
			 (*iinfo)->default_val.i_val) {
			bufcat(buf, " \"%s\"", member->dcl_nom->name);
			ok = 1;
			break;
		     }
		  }
		  if (!ok)
		     bufcat(buf, " %d", (*iinfo)->default_val.i_val);

	       } else
		  bufcat(buf, " %d", (*iinfo)->default_val.i_val);
	       break;
	    case DOUBLE:
	       bufcat(buf, " %f", (*iinfo)->default_val.d_val);
	       break;
	    case CHAR:
	       bufcat(buf, " \"%s\"", (*iinfo)->default_val.str_val);
	       break;
	 }
      }
      else
	 bufcat(buf, " \"%s\" %s", name, defaultDefault);

      bufcat(buf, " ]\n");

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

	 if (tclInputFormatGen(buf, var, m->dcl_nom, iinfo, level+1))
	    fprintf(stderr, "Warning: struct member %s was problematic.\n",
		    var);

	 free(var);
      }
   }
	  
   if (IS_ARRAY(nom)) {
      int i;

      /* End array scanning */
      for(i=IS_STRING(nom)?1:0;i<nom->ndimensions;i++) {
	 bufcat(buf, "}\n");
      }
      free(arrayName);
   }

   if (nom->type->type != STRUCT && nom->type->type != UNION)
      if ((*iinfo) != NULL) *iinfo = (*iinfo)->next;
   return 0;
}

/*
 * Returns format of output struct for tcl ------------------------------
 */

static int
tclOutputFormatGen(char **buf, char *name, DCL_NOM_STR *nom)
{
   int i;

   if (IS_POINTER(nom)) {
      fprintf(stderr, "Pointer %s does not make sense here.\n", name);
      return -1;
   }

   /* List of dimensions */
   bufcat(buf, "{ ");
   for(i=0;i<nom->ndimensions-IS_STRING(nom)?1:0;i++) {
      bufcat(buf, "%d ", nom->dimensions[i]);
   }
   bufcat(buf, "} ");

   if (nom->type->type != STRUCT && nom->type->type != UNION) {
      bufcat(buf, "%s ", name);
   } else {
      /* recurse */
      DCL_NOM_LIST *m;

      bufcat(buf, "{ %s ", name);

      for (m = nom->type->members; m != NULL; m = m->next) {
	 if (m->dcl_nom->name == NULL) { 
	    fprintf(stderr, "Warning: null member name in %s.\n", name);
	    continue;
	 }

	 if (tclOutputFormatGen(buf, m->dcl_nom->name, m->dcl_nom))
	    fprintf(stderr,
		    "Warning: struct member %s was problematic.\n",
		    m->dcl_nom->name);
      }

      bufcat(buf, "} ");
   }
   return 0;
}

/*
 * Returns number of arguments needed to fill the given type ------------
 */

static int
tclArgNumberGen(DCL_NOM_STR *nom)
{
   int n = -1;

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
	    p = tclArgNumberGen(m->dcl_nom);
	    if (p < 0) {
	       fprintf(stderr,
		       "Error: struct member %s was problematic.\n",
		       m->dcl_nom->name);
	       n = -1;
	       break;
	    } else
	       n += p;
	 }
	 break;
      }

      case TYPEDEF:
	 fprintf(stderr, "Warning: encountered a typedef for %s.\n",
		 nom->name);
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
 * Returns request usage ------------------------------------------------
 */

static int
tclUsageGen(char **buf, DCL_NOM_STR *nom, char *prefix)
{
   char *type, *name;		/* input type */

   if (prefix == NULL) prefix = "";

   /* Find requested type */
   dcl_nom_decl(nom, &type, &name);

   switch (nom->type->type) {

      case CHAR:
      case SHORT:
      case INT:
      case FLOAT:
      case DOUBLE:
	 bufcat(buf,"%s%s %s\\\\n", prefix, type, name);
	 break;
	 
      case STRUCT:
      case UNION:
      {
	 DCL_NOM_LIST *m;
	 char *newPrefix = NULL;

	 bufcat(buf, "%s%s\\\\n", prefix, type);
	 bufcat(&newPrefix, "%s| ", prefix);

	 for (m = nom->type->members; m != NULL; m = m->next)
	    if (tclUsageGen(buf, m->dcl_nom, newPrefix))
	       fprintf(stderr, "Warning: struct member %s was problematic.\n",
		       m->dcl_nom->name);

	 if (newPrefix != NULL) free(newPrefix);
	 break;
      }

      /* Affichage en clair des symboles de l'enum */
      case ENUM:
	 bufcat(buf,"%s%s %s\\\\n", prefix, type, name);
	 /* 	  genScanEnum(out, t->members); */
	 break;

      case TYPEDEF:
	 fprintf(stderr, "Warning: encountered a typedef for %s.\n", nom->name);
	 break;
   }

   if (type != NULL) free(type);
   if (name != NULL) free(name);

   return 0;
}

/*
 * Generate a list of « "enumText" » ------------------------------------
 */

static int
tclGenEnumList(char **buf, TYPE_STR *type)
{
   DCL_NOM_LIST *member;

   if (type->type != ENUM) return -1;

   for(member = type->members; member != NULL; member = member->next) {
      bufcat(buf, "\"%s\" ", member->dcl_nom->name);
   }

   return 0;
}

/*
 * Quote a string for TCL -----------------------------------------------
 */

static char *
tclQuoteString(char *string)
{
   int i,j, l = strlen(string);
   static const char *mustBeQuoted = "[]";

   /* we add at most two backslashes per caracter, so: */
   char *quoted = malloc(l*3+1);
   
   for(i=j=0;i<l;i++) {
      if (strchr(mustBeQuoted, string[i]) == NULL)
	 quoted[j++] = string[i];
      else {
	 quoted[j++] = '\\';
	 quoted[j++] = '\\'; /* 2 \ because perl */
	 quoted[j++] = string[i];
      }
   }
   quoted[j++]=0;
   return quoted;
}
