/*	$LAAS$ */

/* 
 * Copyright (c) 1993-2003 LAAS/CNRS
 * Matthieu Herrb - Fri Aug 13 1993
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

/***
 *** Ge'ne'ration du posterLib
 ***/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "genom.h"
#include "parser.tab.h"
#include "posterLibGen.h"

static void posterLibMemberGen(FILE *out, POSTER_LIST *p, int readFunctions, int protos);
static void posterWriteLibMemberGen(FILE *out, POSTER_LIST *p, int protos);
static char * posterExecTaskNameTab(void);
static char * posterRequestNameTab(int *nbRqst);


static char *tabPosterXML=NULL;
static int  nbPosterXML=1;

int posterLibGen(FILE *out) 
{
    TYPE_STR *pt;
    DCL_NOM_LIST *l, *m;
    POSTER_LIST *p;
    char *ptstr=NULL, *type, *var;
    int nbRqsts;

    /**
     ** PosterLib.c
     **/
    script_open(out);
    subst_begin(out, PROTO_POSTER_LIB);

    /* Nom du  module */
    print_sed_subst(out, "module", module->name);
    print_sed_subst(out, "MODULE", module->NAME);
    
    subst_end(out);

    cat_begin(out);

    /*
     * Poster Ids
     */
    for (p = posters; p != NULL; p = p->next) {
	fprintf(out, "static POSTER_ID %sPosterId=NULL;\n", p->name);
    }

    /*
     * PosterInit
     */
    fprintf(out, "\n/* -- posterInit ------------------------------------------------------- */\n\n");
    fprintf(out, "STATUS %sPosterInit(void)\n{\n", module->name);
    fprintf(out, "  int status = OK;\n\n");

    /* Init poster de controle */
    fprintf(out, 
	    "  if (%sCntrlPosterInit() == ERROR) {\n"
	    "    h2perror(\"%sCntrlPosterInit\");\n"
	    "    status = ERROR;\n  }\n", 
	    module->name,
	    module->name);

    /* Init pour chaque poster fonctionnel */
    for (p = posters; p != NULL; p = p->next) {
	fprintf(out, 
		"  if (%s%sPosterInit() == ERROR) {\n"
		"    h2perror(\"%s%sPosterInit\");\n"
		"    status = ERROR;\n  }\n",
		module->name, p->name,
		module->name, p->name);
    } /* for */

    fprintf(out, "  return(status);\n}\n\n");
    

    /*
     * Fonctions d'init, de lecture et d'affichage des posters
     */

    fprintf(out, "/* --  Cntrl ------------------------------------------------- */\n\n");
    
    /* la fonction d'init du poster */
    fprintf(out,
	    "STATUS %sCntrlPosterInit(void)\n{\n"
	    "  if (posterFind(%s_CNTRL_POSTER_NAME, &%sCntrlPosterId) == ERROR) {\n"
	    "     h2perror(\"%sCntrlPosterInit\");\n"
	    "     return ERROR;\n"
	    "  }\n"
	    "  if (posterEndianness(%sCntrlPosterId, &posterDataEndianness) == ERROR) {\n"
	    "    h2perror(\"%sCntrlPosterInit\");\n"
	    "    return ERROR;\n"
	    "  }\n"
	    "  return OK;\n"
	    "}\n"
	    "\n",
	    module->name, 
	    module->NAME, module->name,
	    module->name, 
	    module->name, 
	    module->name);

    /* init des autres poster */
    for (p = posters; p != NULL; p = p->next) {

      fprintf(out, "/* --  %s ------------------------------------------------- */\n\n",
	      p->name);

      /* la fonction d'init du poster */
      fprintf(out,
	      "STATUS %s%sPosterInit(void)\n{\n"
	      "  if (posterFind(%s_%s_POSTER_NAME, &%sPosterId) == ERROR) {\n"
	      "     h2perror(\"%s%sPosterInit\");\n"
	      "     return ERROR;\n"
	      "  }\n"
	      "  if (posterEndianness(%sPosterId, &posterDataEndianness) == ERROR) {\n"
	      "    h2perror(\"%s%sPosterInit\");\n"
	      "    return ERROR;\n"
	      "  }\n"
	      "  return OK;\n"
	      "}\n"
	      "\n",
	      module->name, p->name,
	      module->NAME, p->NAME, p->name,
	      module->name, p->name,
	      p->name,
	      module->name, p->name);

    } /* for */


    /*
     * Fonctions de lecture et d'affichage des posters
     */
    for (p = posters; p != NULL; p = p->next) {

      fprintf(out, "/* --  %s ------------------------------------------------- */\n\n",
	      p->name);

      /* la fonction pour lire tout le poster */
      fprintf(out,
	      "STATUS %s%sPosterRead(%s *x)\n{\n"
	      "  if (%sPosterId == NULL) {\n"
	      "     fprintf (stderr, warnMsgNotInit, \"%s\");\n"
	      "     if (%s%sPosterInit() == ERROR) {\n"
	      "        h2perror(\"%s%sPosterInit\");\n"
	      "        return ERROR;\n"
	      "     }\n"
	      "  }\n"
	      "  if (posterRead(%sPosterId, 0, (char *)x, sizeof(*x))\n"
	      "           != sizeof(*x)) return ERROR;\n"
	      "  if (posterDataEndianness != H2_LOCAL_ENDIANNESS)\n"
	      "     endianswap_%s(x, 0, NULL);\n"
	      "  return OK;\n"
	      "}\n\n",
	      module->name, p->name, p->type->name, 
	      p->name, 
	      p->name, 

	      module->name, p->name,
	      module->name, p->name,

	      p->name,      /* endian */
	      p->type->name);

	/* Les fonctions pour les e'le'ments de la structure */
      posterLibMemberGen(out, p, 1 /* read */, 0 /* ! protos */); 

    } /* for */

    cat_end(out);
    script_close(out, "server/%sPosterLib.c", module->name);

    /**
     ** PosterLibProto.h
     **/
    script_open(out);
    cat_begin(out);

    /* fonctions communes */
    fprintf(out, "extern STATUS %sPosterInit ( void );\n", module->name);
    fprintf(out, "extern STATUS %sCntrlPosterRead ( %s_CNTRL_STR *%sCntrlStrId );\n", module->name, module->NAME, module->name);
    fprintf(out, "extern STATUS %sCntrlPosterInit ( void );\n", module->name);

    for (p = posters; p != NULL; p = p->next) {
      /* la fonction pour lire tout le poster */
     fprintf(out, "extern STATUS %s%sPosterInit ( void );\n",
	      module->name, p->name);
      fprintf(out, "extern STATUS %s%sPosterRead ( %s *x );\n",
	      module->name, p->name, p->type->name);

      /* Les fonctions pour les e'le'ments de la structure */
      posterLibMemberGen(out, p, 1 /* read */, 1 /* protos */); 
    } /* for */

    cat_end(out);
    script_close(out, "server/%sPosterLibProto.h", module->name);


    /**
     ** PosterShowLib.c
     **/
    script_open(out);
    subst_begin(out, PROTO_POSTER_SHOW_LIB);

    /* Nom du  module */
    print_sed_subst(out, "module", module->name);
    print_sed_subst(out, "MODULE", module->NAME);
    
    ptstr = posterExecTaskNameTab();
    print_sed_subst(out, "execTaskNameTabDeclare", ptstr);
    free(ptstr);
    ptstr = NULL;

    ptstr = posterRequestNameTab(&nbRqsts);
    print_sed_subst(out, "nbExecRqst", "%d", nbRqsts);
    print_sed_subst(out, "requestNameTabDeclare", ptstr);
    free(ptstr);
    ptstr = NULL;

    subst_end(out);

    cat_begin(out);

    /*
     * Fonctions de lecture et d'affichage des posters
     */
    for (p = posters; p != NULL; p = p->next) {

      fprintf(out, "/* --  %s ------------------------------------------------- */\n\n",
	      p->name);

      /* la fonction pour afficher tout le poster */
      fprintf(out, 
	      "STATUS %s%sPosterShow(void)\n{\n"
	      "  %s *x;\n\n"
	      "  if ((x = (%s *)malloc(sizeof(* x))) == NULL) {\n"
	      "     h2perror(\"%s%sPosterShow\");\n"
	      "     return ERROR;\n"
	      "  }\n"
	      "  if (%s%sPosterRead(x) == ERROR) {\n"
	      "     h2perror(\"%s%sPosterShow\");\n"
	      "     free(x);\n"
	      "     return ERROR;\n"
	      "  }\n"
	      "  print_%s(stdout, x, 0, 0, NULL, stdin);\n"
	      "  free(x);\n"
	      "  return OK;\n}\n\n",
	      module->name, p->name, /* nom fonction posterShow */
	      p->type->name,         /* Déclaration variable */
	      p->type->name,         /* malloc variable */
	      module->name, p->name, /* h2perror */
	      module->name, p->name, /* PosterRead */
	      module->name, p->name, /* h2perror */
	      nom_type1(p->type));        /* print variable */
      
	/* Les fonctions pour les e'le'ments de la structure */
      posterLibMemberGen(out, p, 0 /* ! read */, 0 /* ! protos */); 

    } /* for */

    cat_end(out);
    script_close(out, "server/%sPosterShowLib.c", module->name);

    /**
     ** PosterShowLibProto.h
     **/
    script_open(out);
    cat_begin(out);

    fprintf (out, "extern STATUS %sCntrlPosterShow ( void );\n",
	     module->name);
    fprintf (out, "extern STATUS %sCntrlPosterActivityShow ( void );\n",
	     module->name);
    /*
     * Fonctions de lecture et d'affichage des posters
     */
    for (p = posters; p != NULL; p = p->next) {

      /* la fonction pour afficher tout le poster */
      fprintf(out, "extern STATUS %s%sPosterShow ( void );\n",
	      module->name, p->name);

      /* Les fonctions pour les e'le'ments de la structure */
      posterLibMemberGen(out, p, 0 /* ! read */, 1 /* protos */); 

    } /* for */

    cat_end(out);
    script_close(out, "server/%sPosterShowLibProto.h", module->name);


    /** ----------------------------------------------------------------------
     ** PosterXMLLib.c
     **/

    /**
     ** PosterXMLLibProto.h
     **/
    script_open(out);
    cat_begin(out);

    fprintf (out, "void web%s(FILE *f, int argc, char **argv, char **argn);\n",
	     module->name);
    fprintf (out, "extern STATUS %sCntrlPosterXML (FILE *f);\n",
	     module->name);
    fprintf (out, "extern STATUS %sCntrlPosterActivityXML (FILE *f);\n",
	     module->name);
    /*
     * fonction de lecture donnees XML  des posters
     */
    for (p = posters; p != NULL; p = p->next) {

      /* la fonction pour afficher tout le poster */
      fprintf(out, "extern STATUS %s%sPosterXML (FILE *f);\n",
	      module->name, p->name);

      if (tabPosterXML != NULL) bufcat(&tabPosterXML, ", \\\\\n");
      bufcat(&tabPosterXML, "{\"%s\", \"\", %s%sPosterXML}",
	     p->name, module->name, p->name);
      nbPosterXML++;
	     
      /* Les fonctions pour les e'le'ments de la structure */
      posterLibMemberGen(out, p, 2 /* ! read */, 1 /* protos */); 

    } /* for */

    cat_end(out);
    script_close(out, "server/%sPosterXMLLibProto.h", module->name);


    /** 
     ** PosterXMLLib.c
     **/

    script_open(out);
    subst_begin(out, PROTO_POSTER_XML_LIB);

    /* Nom du  module */
    print_sed_subst(out, "module", module->name);
    print_sed_subst(out, "MODULE", module->NAME);

    print_sed_subst(out, "nbPosterXML", "%d", nbPosterXML);
    print_sed_subst(out, "tabPosterXML", tabPosterXML);
    free(tabPosterXML);
    tabPosterXML=NULL;

    ptstr = posterExecTaskNameTab();
    print_sed_subst(out, "execTaskNameTabDeclare", ptstr);
    free(ptstr);
    ptstr = NULL;

    ptstr = posterRequestNameTab(&nbRqsts);
    print_sed_subst(out, "nbExecRqst", "%d", nbRqsts);
    print_sed_subst(out, "requestNameTabDeclare", ptstr);
    free(ptstr);
    ptstr = NULL;

    subst_end(out);

    cat_begin(out);

    /*
     * XML de lecture donnees format XML des posters
     */
    for (p = posters; p != NULL; p = p->next) {

      fprintf(out, "/* --  %s ------------------------------------------------- */\n\n",
	      p->name);

      /* la fonction pour lire tout le poster */
      fprintf(out, 
	      "STATUS %s%sPosterXML(FILE *f)\n{\n"
	      "  BOOL err=FALSE;\n"
	      "  %s *x;\n\n"
	      "  xmlBalise(\"%s\",BEGIN_BALISE_NEWLINE,f,1);\n"
	      "  xmlBalise(\"error\",BEGIN_BALISE,f,2);\n"
	      "\n"
	      "  if ((x = (%s *)malloc(sizeof(* x))) == NULL) {\n"
	      "     h2perror(\"%s%sPosterXML\");\n"
	      "     err=TRUE;\n"
	      "  }\n"
	      "  if (!err) {\n"
	      "    if (%s%sPosterRead(x) == ERROR) {\n"
	      "       h2perror(\"%s%sPosterXML\");\n"
	      "       free(x);\n"
	      "       err=TRUE;\n"
	      "    }\n"
	      "  }\n"
	      "  fprintfBuf(f, \"</error>\\n\");\n" 
	      "  printXML_%s(f, \"data\", x, 2, 0, NULL, NULL);\n"
	      "  xmlBalise(\"%s\",TERMINATE_BALISE,f,1);\n"
	      "  free(x);\n"
	      "  return OK;\n}\n\n",
	      module->name, p->name, /* nom fonction posterXML */
	      p->type->name,         /* Déclaration variable */
	      p->name,               /* balise poster */
	      p->type->name,         /* malloc variable */
	      module->name, p->name, /* h2perror */
	      module->name, p->name, /* PosterRead */
	      module->name, p->name, /* h2perror */
	      nom_type1(p->type),  /* printXML */
	      p->name);        /* end balise */
      
      /* Les fonctions pour les e'le'ments de la structure */
      posterLibMemberGen(out, p, 2 /* ! read */, 0 /* ! protos */); 

    } /* for */

    cat_end(out);
    script_close(out, "server/%sPosterXMLLib.c", module->name);


    /** ----------------------------------------------------------------------
     ** PosterLib.h
     **/
    script_open(out);
    subst_begin(out, PROTO_POSTER_LIB_H);

    /* Nom du  module */
    print_sed_subst(out, "module", module->name);
    print_sed_subst(out, "MODULE", module->NAME);

    /* Noms des posters */
    ptstr = NULL;
    for (p = posters; p != NULL; p = p->next) {
	bufcat(&ptstr, "#define %s_%s_POSTER_NAME \"%s%s\"\n",
	       module->NAME, p->NAME, module->name, p->name);
    } /* for */
    print_sed_subst(out, "listPosterNameDeclare", ptstr);
    free(ptstr);

    /* Types des posters */
    ptstr = NULL;
    for (l = typedefs; l != NULL; l = l->next) {
	pt = l->dcl_nom->type;
	if (pt->flags & TYPE_POSTER) {
	    bufcat(&ptstr, "typedef struct %s {\n", pt->name);
	    for (m = pt->members; m != NULL; m = m->next) {
		dcl_nom_decl(m->dcl_nom, &type, &var);
		bufcat(&ptstr, "  %s %s;\n", type, var);
		free(type);
		free(var);
	    } /* for */
	    bufcat(&ptstr, "} %s;\n\n", pt->name);
	}
    } /* for */
    print_sed_subst(out, "listPosterTypeDeclare", ptstr);
    free(ptstr);

    subst_end(out);
    script_close(out, "server/%sPosterLib.h", module->name);

    /**
     ** PosterShowLib.h
     **/
    script_open(out);
    subst_begin(out, PROTO_POSTER_SHOW_LIB_H);

    /* Nom du  module */
    print_sed_subst(out, "module", module->name);
    print_sed_subst(out, "MODULE", module->NAME);

    subst_end(out);
    script_close(out, "server/%sPosterShowLib.h", module->name);

    /**
     ** PosterXMLLib.h
     **/
    script_open(out);
    subst_begin(out, PROTO_POSTER_XML_LIB_H);

    /* Nom du  module */
    print_sed_subst(out, "module", module->name);
    print_sed_subst(out, "MODULE", module->NAME);

    subst_end(out);
    script_close(out, "server/%sPosterXMLLib.h", module->name);


    /** ----------------------------------------------------------------------
     ** PosterWriteLib.c
     **/
    script_open(out);
    cat_begin(out);

    fprintf(out,
	    "/* ---"
	    " Generated file, do not edit by hand "
	    "--------------------------- */\n");

    fprintf(out, "#ifdef VXWORKS\n");
    fprintf(out, "# include <vxWorks.h>\n");
    fprintf(out, "#else\n");
    fprintf(out, "# include <portLib.h>\n");
    fprintf(out, "#endif\n\n");
    fprintf(out, "#include \"%sPosterLib.h\"\n\n", module->name);

    /*
     * Fonction d'écriture des posters
     */
    for (p = posters; p != NULL; p = p->next) {

      /* Entête */
      fprintf(out, "/* --  %s  ------------------------------------------------- */\n\n",
	      p->name);

      /* la fonction pour écrire tout le poster */
      fprintf(out,
	      "STATUS %s%sPosterWrite(POSTER_ID pid, %s *x)\n{\n"
	      "  return (posterWrite(pid, 0, (char *)x, sizeof(*x))\n"
	      "            == sizeof(*x) ? OK : ERROR);\n"
	      "}\n\n",
	      module->name, p->name, p->type->name);

      /* Les fonctions pour les e'le'ments de la structure */
      posterWriteLibMemberGen(out, p, 0 /* ! protos */); 

    } /* for */

    cat_end(out);
    script_close(out, "server/%sPosterWriteLib.c", module->name);

    /**
     ** PosterWriteLibProto.h
     **/
    script_open(out);
    cat_begin(out);

    for (p = posters; p != NULL; p = p->next) {

      /* fonction pour tout le poster */
      fprintf(out,
	      "extern STATUS %s%sPosterWrite ( POSTER_ID pid, %s *x );\n",
	      module->name, p->name, p->type->name);

      /* Les fonctions pour les e'le'ments de la structure */
      posterWriteLibMemberGen(out, p, 1 /* protos */); 
    }

    cat_end(out);
    script_close(out, "server/%sPosterWriteLibProto.h", module->name);

    return(0);
}

static void posterLibMemberGen(FILE *out, POSTER_LIST *p, 
			       int readFunctions, int protos)
{
    STR_REF_LIST *l;
    STR_REF_STR *m;
    DCL_NOM_STR *n;
    char *type, *var, *type1;
    char *decla=NULL, *addrstr=NULL;
    int nDim, i, *dims, newline;
    
    
    for (l = p->data; l != NULL; l = l->next) {
	m = l->str_ref;
	n = m->dcl_nom;
	n->pointeur++;
	dcl_nom_decl(n, &type, &var);
	n->pointeur--;

	/* Declaration des tailles du tableau (si c'est un tableau) */
	nDim = n->ndimensions;
	dims = n->dimensions;
	if (nDim==0) {
	  bufcat(&decla, "*dims = NULL");
	  bufcat(&addrstr, nom_type(n->type));
	}
	else {
	  bufcat(&decla, "dims[%d] = {%d", nDim, dims[0]);
	  bufcat(&addrstr, "%s[%d]", nom_type(n->type), dims[0]);
	  for (i=1;i<nDim;i++) {
	    bufcat(&decla, ",%d", dims[i]);
	    bufcat(&addrstr, "[%d]", dims[i]);
	  }
	  bufcat(&decla, "}");
	}

	/*-- Fonction read
	 */
	if (readFunctions==1) {

	  if (protos) {
	    fprintf(out, "extern STATUS %s%s%sPosterRead ( %s *%s );\n",
		    module->name, p->name, n->name, type, n->name);
	  }
	  else {

	    /* Type de la donnee a lire */
	    if(n->flags & STRING)
	      type1 = strdup("string");
	    else 
	      type1 = nom_type1(n->type);

	    fprintf(out, "/* --  %s -> %s ---------------------------------------- */\n\n",
		    p->name, n->name);

	    fprintf(out, "STATUS %s%s%sPosterRead(%s *%s /* %s */)\n{\n",
		    module->name, p->name, n->name, type, n->name, addrstr);
	    fprintf(out, "  %s *x = NULL;\n", p->type->name);
	    fprintf(out, "  int offset = (int)&(x->%s) - (int)(x);\n", n->name);
	    fprintf(out, "  int size = sizeof(x->%s);\n", n->name);
	    fprintf(out, 
		    "  if (%sPosterId == NULL) {\n"
		    "     fprintf (stderr, warnMsgNotInit, \"%s\");\n"

		    "     if (%s%sPosterInit() == ERROR) {\n"
		    "        h2perror(\"%s%sPosterInit\");\n"
		    "        return ERROR;\n"
		    "     }\n"
		    "  }\n",
		    p->name, 
		    p->name, 
		    module->name, p->name,
		    module->name, p->name);

	    fprintf(out, 
		    "  if (posterRead(%sPosterId, offset, "
		    "(char *)(%s), size) != size)\n"
		    "    return ERROR;\n"
		    "  if (posterDataEndianness != H2_LOCAL_ENDIANNESS)\n"
		    "     endianswap_%s(%s, 0, NULL);\n"
		    "  return OK;\n"
		    "}\n\n",
		    p->name, n->name,
		    type1, n->name);

	    free(type1);

	  }
	}

	/*-- Fonction print 
	 */
	else if (readFunctions==0) {

	  if (protos) {
	    fprintf(out, 
		    "extern STATUS %s%s%sPosterShow ( void );\n",
		    module->name, p->name, n->name);
	  }
	  else {
	    fprintf(out, "/* --  %s -> %s ---------------------------------------- */\n\n",
		    p->name, n->name);

	    fprintf(out, 
		    "STATUS %s%s%sPosterShow(void)\n"
		    "{\n  %s *%s;\n", 
		    module->name, p->name, n->name, type, n->name);
	    fprintf(out, "  int %s;\n\n", decla);
	    
	    /* Corps de la fonction */
	    fprintf(out, 
		    "  if ((%s = malloc(sizeof(%s))) == NULL) {\n"
		    "     h2perror(\"%s%s%sPosterShow\");\n"
		    "     return ERROR;\n"
		    "  }\n", 
		    n->name, addrstr,
		    module->name, p->name, n->name);
	    fprintf(out, 
		    "  if (%s%s%sPosterRead(%s) == ERROR) {\n"
		    "     h2perror(\"%s%s%sPosterRead\");\n"
		    "     free(%s);\n"
		    "     return ERROR;\n"
		    "  }\n", 
		    module->name, p->name, n->name, n->name,
		    module->name, p->name, n->name, n->name);
	    if (n->type->type == STRUCT || n->type->type == UNION ||
		n->type->type == TYPEDEF || n->flags & ARRAY) newline = 1;
	    else newline = 0;
	    fprintf(out, 
		    "  printf (\"%s%s\");\n"
		    "  print_%s(stdout, %s, %d, %d, dims, stdin);\n",
		    n->name,  newline ? ":\\n" : " = ",
		    n->flags & STRING ? "string":nom_type1(n->type), 
		    n->name, newline?1:0, nDim);
	    fprintf(out, "  free(%s);\n  return OK;\n}\n\n", n->name);
	  }
	}


	/*-- Fonction XML 
	 */
	else {

	  if (protos) {
	    fprintf(out, 
		    "extern STATUS %s%s%sPosterXML (FILE *f);\n",
		    module->name, p->name, n->name);

	    if (tabPosterXML != NULL) bufcat(&tabPosterXML, ", \\\\\n");
	    bufcat(&tabPosterXML, "{\"%s\", \"%s\", %s%s%sPosterXML}",
		   p->name, n->name, module->name, p->name, n->name);
	    nbPosterXML++;
	  }
	  else {
	    fprintf(out, "/* --  %s -> %s ---------------------------------------- */\n\n",
		    p->name, n->name);

	    fprintf(out, 
		    "STATUS %s%s%sPosterXML(FILE *f)\n"
		    "{\n  %s *%s;\n", 
		    module->name, p->name, n->name, type, n->name);
	    fprintf(out, "  int %s;\n\n", decla);
	    
	    /* Corps de la fonction */
	    fprintf(out, 
		    "  xmlBalise(\"%s%s\",BEGIN_BALISE_NEWLINE,f,1);\n"
		    "  xmlBalise(\"error\",BEGIN_BALISE,f,2);\n"
		    "\n"
		    "  if ((%s = malloc(sizeof(%s))) == NULL) {\n"
		    "     fprintfBuf(f, \"%s%s%sPosterXML: allocation error\");\n"
		    "     h2perror(\"%s%s%sPosterXML\");\n"
		    "     return ERROR;\n"
		    "  }\n", 
		    p->name, n->name,
		    n->name, addrstr,
		    module->name, p->name, n->name,
		    module->name, p->name, n->name);
	    fprintf(out, 
		    "  if (%s%s%sPosterRead(%s) == ERROR) {\n"
		    "     fprintfBuf(f, \"%s%s%sPosterXML: poster read failed\");\n"
		    "     h2perror(\"%s%s%sPosterRead\");\n"
		    "     free(%s);\n"
		    "     return ERROR;\n"
		    "  }\n"
		    "  fprintfBuf(f, \" </error>\\n\");\n", 
		    module->name, p->name, n->name, n->name,
		    module->name, p->name, n->name, 
		    module->name, p->name, n->name, 
		    n->name);
	    if (n->type->type == STRUCT || n->type->type == UNION ||
		n->type->type == TYPEDEF || n->flags & ARRAY) newline = 1;
	    else newline = 0;
	    fprintf(out, 
		    "  printXML_%s(f, \"data\", %s, 2, %d, dims, NULL);\n",
		    n->flags & STRING ? "string":nom_type1(n->type), 
		    n->name, nDim);
	    fprintf(out, 
		    "  free(%s);\n"
		    "  xmlBalise(\"%s%s\",TERMINATE_BALISE,f,1);\n"
		    "  return OK;\n}\n\n", 
		    n->name,
		    p->name, n->name);
	  }
	}

	free(type);
	free(var);
	free(addrstr); addrstr = NULL;
	free(decla); decla = NULL;
    } /* for */
}


static void posterWriteLibMemberGen(FILE *out, POSTER_LIST *p, int protos)
{
    STR_REF_LIST *l;
    STR_REF_STR *m;
    DCL_NOM_STR *n;
    char *type, *var;
    char *addrstr=NULL;
    int nDim, i, *dims;
    
    
    for (l = p->data; l != NULL; l = l->next) {
	m = l->str_ref;
	n = m->dcl_nom;
	n->pointeur++;
	dcl_nom_decl(n, &type, &var);
	n->pointeur--;

	/* Declaration des tailles du tableau (si c'est un tableau) */
	nDim = n->ndimensions;
	dims = n->dimensions;
	if (nDim==0) {
	  bufcat(&addrstr, nom_type(n->type));
	}
	else {
	  bufcat(&addrstr, "%s[%d]", nom_type(n->type), dims[0]);
	  for (i=1;i<nDim;i++) {
	    bufcat(&addrstr, "[%d]", dims[i]);
	  }
	}

	/*-- Fonction write
	 */
	if (protos) {
	  fprintf(out, "extern STATUS %s%s%sPosterWrite ( POSTER_ID pid, %s *%s );\n",
		  module->name, p->name, n->name, type, n->name);
	}
	else {
	  fprintf(out, "/* --  %s -> %s ---------------------------------------- */\n\n",
		  p->name, n->name);

	  fprintf(out, "STATUS %s%s%sPosterWrite (POSTER_ID pid, %s *%s /* %s */)\n{\n",
		  module->name, p->name, n->name, type, n->name, addrstr);
	  fprintf(out, "  %s *x = 0;\n", p->type->name);
	  fprintf(out, "  int size = sizeof(x->%s);\n", n->name);
	  fprintf(out, "  return (posterWrite(pid, (int)&x->%s - (int)x, "
		  "(char *)(%s), size) \n\t== size ? OK : ERROR);\n}\n\n",
		  n->name, n->name);
	}
	free(type);
	free(var);
	free(addrstr); addrstr = NULL;
    } /* for */
}


static char * posterExecTaskNameTab(void) 
{
    EXEC_TASK_LIST *lt;
    EXEC_TASK_STR *t;
    char *msg = NULL;

    bufcat(&msg, "\nstatic char *%sExecTaskNameTab[] = {\n",
	   module->name); 

    for (lt = taches; lt != NULL; lt = lt->next) {
	t = lt->exec_task;
	bufcat(&msg, "\"%s\"", t->name);
	if (lt->next != NULL) {
	    bufcat(&msg, ",\n");
	}
    } /* for */
    bufcat(&msg, "\n};");
    return(msg);
}

static char * posterRequestNameTab(int *nbRqsts) 
{
    RQST_LIST *l;
    RQST_STR *r;
    char *msg = NULL;
    int nb=0;

    bufcat(&msg, "\nstatic char *%sExecRqstNameTab[] = {\n",
	   module->name); 

    for (l = requetes; l != NULL; l = l->next) {
        r = l->rqst;
	if(r->type == EXEC || r->type == INIT) {
	  bufcat(&msg, "\"%s\"", r->name);
	  if (l->next != NULL)
	    bufcat(&msg, ",\n");
	}
    } /* for */
    bufcat(&msg, "\n};");

    bufcat(&msg, "\nstatic int %sExecRqstNumTab[] = {\n",
	   module->name);
    for (l = requetes; l != NULL; l = l->next) {
	r = l->rqst;
	if(r->type == EXEC || r->type == INIT) {
	  nb++;
	  bufcat(&msg, "%d", r->rqst_num);
	  if (l->next != NULL)
	    bufcat(&msg, ", ");
	}
    }
    bufcat(&msg, "};\n");
    
    *nbRqsts = nb;
    return(msg);
}

